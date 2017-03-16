/**
*	created:		2012-6-28   7:32
*	filename: 		ChatParser
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现对聊天信息的解析
*/
//------------------------------------------------------------------------
#include "../IChatClient.h"
#include "../Include/ChatClient.h"
#include "../Include/ChatItemFactory.h"
#include "../Include/ChatParser.h"
#include "../../FKGame/SceneObj/IGoods.h"
#include "../../FKGame/KeywordFilter/IKeywordFilter.h"
#include "../../FKGame/MapView/MapItemView.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "../../FKGame/WndSys/WndDef.h"
#include <assert.h>
//------------------------------------------------------------------------
CChatParser::CChatParser(void)
: m_pChatNode(NULL)
, m_strColor("-1")
, m_nChannel(-1)
, m_uidSender(INVALID_UID)
, m_nCustomCmd(CHAT_CMD_NONE)
{
}
//------------------------------------------------------------------------
CChatParser::~CChatParser(void)
{
}

//-------------------------------------------------------------------
// 描  述：解析聊天信息 
// 参  数：[输入] const char * lpszData - 聊天信息
// 参  数：[输入] int nLen - 聊天信息长度
// 返回值：CHtmlNode * - 解析正确返回聊天信息的HTML头节点，否则返回NULL
//-------------------------------------------------------------------
CHtmlNode * CChatParser::Parse(const char * lpszData, int nLen)
{
	char buf[1024];

	memset(buf, 0, sizeof(buf));
	m_strColor = "-1";

	if (nLen > sizeof(buf))
	{
		return NULL;
	}

	SChatMsgHeader *pMsgHeader = (SChatMsgHeader *)lpszData;
	if( nLen < sizeof(SChatMsgHeader))
	{
       return NULL;
	}
	// 怪物聊天
    if( pMsgHeader->nChannel == CHANNEL_MONSTER)
	{
	   OnMonsterChat(lpszData,nLen);
       return NULL;
	}
	// 创建聊天信息的头节点
	m_pChatNode = new CHtmlNode();
	if (m_pChatNode == NULL)
	{
		return NULL;
	}
	m_pChatNode->SetName("chat");
	
	// 解析信息头
	if (!ParseHeader(&lpszData, nLen))
	{
		delete m_pChatNode;
		return NULL;
	}

	// 处理自定义命令
	if (m_nCustomCmd != CHAT_CMD_NONE)
	{
		OnCustomCmd(lpszData);
		return m_pChatNode;
	}

	// 过滤说话内容
	IKeywordFilter * pFilter = CChatClient::Instance()->m_pGlobal->GetKeywordFilter();
	if (pFilter == NULL)
	{
		delete m_pChatNode;
		return NULL;
	}
	// 先解析有特殊内容的字符串
	string strTemp;
	if( ParseSpecialContent(lpszData,nLen,strTemp) )
	{
		//
		lpszData = strTemp.c_str();
		pFilter->Filter(lpszData, buf, strTemp.length(), enKL_Replace);
	}else{
		pFilter->Filter(lpszData, buf, nLen, enKL_Replace);
		
		// 解析说话内容
		if (!ParseContent(buf, strlen(buf) + 1))
		{
			delete m_pChatNode;
			return NULL;
		}
	}
	// 是否需要显示泡泡
	if (m_nChannel == CHANNEL_LOCAL)
	{	
		ShowPaoPao(buf);	
	}else if(m_nChannel == CHANNEL_TEAM)
	{
	   //如果是组队频道,如果距离在,10个Tile之内也
		// if(CChatClient::Instance()->IsDistance(pMsgHeader->uidSender,20))
	   {
		   ShowPaoPao(buf);
	   }
      
	}
	return m_pChatNode;
}

//-------------------------------------------------------------------
// 描  述：解析聊天信息的说话内容
//-------------------------------------------------------------------
CHtmlNode * CChatParser::Parse(LPCSTR lpszText)
{
	// 创建聊天信息的头节点
	m_pChatNode = new CHtmlNode();
	if (m_pChatNode == NULL)
	{
		return NULL;
	}
	m_pChatNode->SetName("chat");

	// 解析内容
	if (!ParseContent(lpszText, strlen(lpszText) + 1))
	{
		delete m_pChatNode;
		return NULL;
	}
	
	return m_pChatNode;
}

//-------------------------------------------------------------------
// 描  述：解析信息的数据头
// 参  数：[输入] const char ** ppData - 信息的原始数据
// 参  数：[输入] int & nLen - 信息的长度
// 返回值：BOOL - 解析争取返回TRUE, 否则返回FALSE
//-------------------------------------------------------------------
BOOL CChatParser::ParseHeader(const char ** ppData, int & nLen)
{
	assert (m_pChatNode != NULL);

	if (nLen < sizeof(SChatMsgHeader))
	{
		return FALSE;
	}

	const SChatMsgHeader * pHeader = (const SChatMsgHeader *)(*ppData);
	m_nChannel = pHeader->nChannel;
	m_uidSender = pHeader->uidSender;

	// 检查频道是否有效	
	if (!IS_VALID_CHANNEL(pHeader->nChannel))
	{
		return FALSE;
	}
	m_pChatNode->SetAttribute("channel", pHeader->nChannel);
	
	// 添加频道为显示内容
	if (!AddChannel(pHeader->nChannel))
	{
		return FALSE;
	}

	// 添加发送者为显示内容
	if (pHeader->nChannel == CHANNEL_PRIVATE)
	{
		if (nLen < sizeof(SPrivateChatMsg))
		{
			return FALSE;
		}
		
		const SPrivateChatMsg * pMsg = (const SPrivateChatMsg *)(*ppData);
		
		if (nLen != sizeof(SPrivateChatMsg) + pMsg->wTextLen - 1)
		{
			return FALSE;
		}
		
		IActor * pActor = CChatClient::Instance()->m_pGlobal->m_pPlayer;
		if (pActor == NULL)
		{
			return FALSE;
		}

		if (strcmp(pMsg->header.szSender, pActor->GetName()) == 0)
		{
			if (!AddText("你对", "255,255,255"))
			{
				return FALSE;
			}			
			if (!AddSender(pMsg->szReceiver))
			{
				return FALSE;
			}
			if (!AddText("说", "255,255,255"))
			{
				return FALSE;
			}
		}
		else
		{
			if (!AddSender(pMsg->header.szSender))
			{
				return FALSE;
			}
			if (!AddText("对你说", "255,255,255"))
			{
				return FALSE;
			}
		}		
	}
	else if (pHeader->nChannel != CHANNEL_SYSTEM)
	{
		if (!AddSender(pHeader->szSender))
		{
			return FALSE;
		}		
	}

	if (!AddText(":", "255,255,255"))
	{
		return FALSE;
	}

	switch (pHeader->nChannel)
	{
	case CHANNEL_SYSTEM:
		return OnSystemChat(ppData, nLen);		
		
	case CHANNEL_LOCAL:
		return OnLocalChat(ppData, nLen);

	case CHANNEL_PRIVATE:
		return OnPrivateChat(ppData, nLen);		

	case CHANNEL_TEAM:
		return OnTeamChat(ppData, nLen);

	case CHANNEL_CLAN:
		return OnClanChat(ppData, nLen);

	case CHANNEL_WORLD:
		return OnWorldChat(ppData, nLen);

	default:
		return FALSE;
	}		
}

//-------------------------------------------------------------------
// 描  述：解析说话的内容, 结合其它的信息得出最后显示的HTML格式 
// 参  数：[输入] const char * pContent - 说话内容
// 参  数：[输入] int nLen - 说话内容长度
// 返回值：BOOL - 解析正确返回TRUE,否则返回FALSE
//-------------------------------------------------------------------
BOOL CChatParser::ParseContent(const char * pContent, int nLen)
{
	assert (m_pChatNode != NULL);

	string strText;
	
	for (int i = 0; i < nLen && *pContent != '\0'; i++)
	{
		// 解析文本颜色
		if (*pContent == '&' && isdigit(*(pContent + 1)))
		{
			if (!AddSentence(strText, m_strColor))
			{
				return FALSE;
			}
			strText = "";

			if (!ParseColor(&pContent))
			{
				return FALSE;
			}
		}
		// 解析表情图片
		else if (*pContent == '#' && isdigit(*(pContent + 1)))
		{
			if (!AddSentence(strText, m_strColor))
			{
				return FALSE;
			}
			strText = "";
			
			if (!ParseFace(&pContent))
			{
				return FALSE;
			}
		}
		// 解析装备信息
		else
		{
			strText += *pContent++;
		}
	}
	
	// 如果是系统信息
	if (!AddSentence(strText, m_strColor))
	{
		return FALSE;
	}
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：解析包含特殊内容的字符串
//-------------------------------------------------------------------
BOOL CChatParser::ParseSpecialContent(const char *pContent,int nLen,string& str)
{ 
	if( pContent[0] != SPECIAL_CONTENT)
	{
         return FALSE;
	}
	// 格式为 
	// Item 个数
	// 第一个Item 类型
	// 第一个Item长度
	// 第一个Item内容
    // .......
	if( nLen < (int)sizeof(int) )
	{
       return TRUE;
	}
	pContent += 1;
	int nItemCount = *(int *)pContent;
	pContent+=sizeof(int);
	nLen -=sizeof(int);
	for(int i=0;i<nItemCount;i++)
	{
         if( nLen < sizeof(int) )
		 {
			 return TRUE;
		 }
		 // 长度
		 int nItemLen = *(int *)pContent;
		 nLen -=sizeof(int);
		 pContent += sizeof(int);
         // 类型
         if( nLen < sizeof(int) )
		 {
			 return TRUE;
		 }		 
		 int nType = *(int *)pContent;
		 nLen -=sizeof(int);
		 pContent += sizeof(int);	
		 //长度
		 nItemLen -= 2*sizeof(int);
		 if( nItemLen < 0 )
		 {
			 return TRUE;
		 }
         if(nLen<nItemLen)
		 {
            return TRUE;
		 }
		 char *pBuf = (char *)pContent;
		 pContent += nItemLen;
		 nLen -= nItemLen;
         // 创建物品
		 if( nType == eGoods )
		 {
			 IGoods *pGoods = (IGoods *)CChatClient::Instance()->CreateGoods(pBuf,nItemLen,INVALID_UID);
		     if( pGoods!= NULL)
			 {
                 AddGoods((DWORD)pGoods);
			    str += "[";
				str += pGoods->GetName();
				str += "]";
			 }
			 
		 }else{
            ParseContent(pBuf,nItemLen);
			str += pBuf;
		 }
	}
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：解析索引号 
//-------------------------------------------------------------------
int CChatParser::ParseIndex(char * pBuf, int nLen, const char * pData)
{
	int i = 0;

	for (; i < nLen; i++)
	{
		if (!(((*pData)>='0')&&((*pData)<='9')))
		{
			break;
		}
		*pBuf++ = *pData++;
	}

	return i;
}

//-------------------------------------------------------------------
// 描  述：解析颜色
//-------------------------------------------------------------------
BOOL CChatParser::ParseColor(const char ** ppContent)
{
	char buf[3];
	const char * pData = *ppContent + 1;	// &
	
	// 获取颜色索引
	memset(buf, 0, sizeof(buf));
	int nCount = ParseIndex(buf, 2, pData);
	if (nCount == 0)
	{
		return FALSE;
	}
	
	m_strColor = buf;
	
	*ppContent += (nCount + 1);
	
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：解析表情
//-------------------------------------------------------------------
BOOL CChatParser::ParseFace(const char ** ppContent)
{
	char buf[3];
	const char * pData = *ppContent + 1;	// #

	// 获取图片索引
	memset(buf, 0, sizeof(buf));
	int nCount = ParseIndex(buf, 2, pData);
	if (nCount == 0)
	{
		return FALSE;
	}

	// 添加标签图片
	CHtmlNode * pNode = new CHtmlNode();
	if (pNode == NULL)
	{
		return FALSE;
	}
	pNode->SetName("img");
	pNode->AddAttribute("src", buf);
	m_pChatNode->AddChild(pNode);

	*ppContent += (nCount + 1);
	
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：解析装备信息    格式：[name,info]
//-------------------------------------------------------------------
BOOL CChatParser::ParseThing(const char ** ppContent)
{
	BOOL bName = TRUE;
	string strName;
	string strInfo;
	const char * pData = *ppContent;	

	// 解析装备名称和信息
	while (*pData != ']')
	{
		if (*pData == '\0')
		{			
			return FALSE;
		}

		if (*pData == ',')
		{
			bName = FALSE;
			pData++;
			continue;
		}

		if (bName)
		{
			strName += *pData++;
		}
		else
		{
			strInfo += *pData++;
		}		
	}

	// 添加装备信息链接
	CHtmlNode * pNode = new CHtmlNode();
	if (pNode == NULL)
	{
		return FALSE;
	}
	pNode->SetName("a");
	pNode->SetContent(strName);
	pNode->SetAttribute("href", strInfo);
	pNode->SetAttribute("type", LINK_THING);
	m_pChatNode->AddChild(pNode);

	*ppContent = pData;

	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：添加聊天的频道信息 
//-------------------------------------------------------------------
BOOL CChatParser::AddChannel(int nChannel)
{
	CHtmlNode * pNode = new CHtmlNode();
	if (pNode == NULL)
	{
		return FALSE;
	}
	pNode->SetName("text");

	switch (nChannel)
	{
	case CHANNEL_SYSTEM:
		pNode->SetAttribute("frontcolor", "218,212,231");
		pNode->SetAttribute("backcolor", "218,212,231");
		pNode->SetContent("[系统]");
		break;

	case CHANNEL_LOCAL:
		pNode->SetAttribute("frontcolor", "218,212,231");
		pNode->SetAttribute("backcolor", "0,139,29");
		pNode->SetContent("[附近]");		
		break;

	case CHANNEL_PRIVATE:
		pNode->SetAttribute("frontcolor", "252,116,43");
		pNode->SetAttribute("backcolor", "132,7,143");
		pNode->SetContent("[私聊]");		
		break;
		
	case CHANNEL_TEAM:
		pNode->SetAttribute("frontcolor", "81,204,249");
		pNode->SetAttribute("backcolor", "8,31,148");
		pNode->SetContent("[队伍]");
		break;
		
	case CHANNEL_CLAN:
		pNode->SetAttribute("frontcolor", "21,252,3");
		pNode->SetAttribute("backcolor", "194,105,0");
		pNode->SetContent("[氏族]");
		break;

	case CHANNEL_WORLD:
		pNode->SetAttribute("frontcolor", "251,242,3");
		pNode->SetAttribute("backcolor", "164,0,0");
		pNode->SetContent("[世界]");		
		break;

	default:
		delete pNode;
		return FALSE;
	}		
	
	m_pChatNode->AddChild(pNode);

	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：添加发送者
//-------------------------------------------------------------------
BOOL CChatParser::AddSender(string strSender)
{
	CHtmlNode * pNode = NULL;

	// '('
	pNode = new CHtmlNode();
	if (pNode == NULL)
	{
		return FALSE;
	}
	pNode->SetName("text");
	pNode->SetContent("(");
	m_pChatNode->AddChild(pNode);
	
	// (...)的中间部分
	pNode = new CHtmlNode();
	if (pNode == NULL)
	{
		return FALSE;
	}
	pNode->SetName("a");
	pNode->SetAttribute("type", LINK_PLAYER);
	pNode->SetContent(strSender);
	m_pChatNode->AddChild(pNode);

	// ')'
	pNode = new CHtmlNode();
	if (pNode == NULL)
	{
		return FALSE;
	}
	pNode->SetName("text");
	pNode->SetContent(")");
	m_pChatNode->AddChild(pNode);

	return TRUE;
}

//-------------------------------------------------------------------
// 描  述： 添加当前的时间
//-------------------------------------------------------------------
BOOL CChatParser::AddTime(void)
{
	SYSTEMTIME now;
	GetLocalTime(&now);

	// 时间显示的格式
	char buf[100];
	sprintf(buf, "(%.2d:%.2d)", now.wHour, now.wMinute);
	
	CHtmlNode * pNode = new CHtmlNode();
	if (pNode == NULL)
	{
		return FALSE;
	}
	pNode->SetName("text");
	pNode->SetContent(buf);
	
	m_pChatNode->AddChild(pNode);

	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：添加一个语句 
//-------------------------------------------------------------------
BOOL CChatParser::AddSentence(string strText, string strColor)
{
	if (!strText.empty())
	{
		CHtmlNode * pNode = new CHtmlNode();
		if (pNode == NULL)
		{
			return FALSE;
		}

		pNode->SetName("a");
		pNode->SetContent(strText);		
		pNode->SetAttribute("type", LINK_SENTENCE);
		if (m_nChannel != CHANNEL_SYSTEM)
		{
			pNode->SetAttribute("color", strColor);
		}		

		m_pChatNode->AddChild(pNode);
	}
	
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：添加一个文本
//-------------------------------------------------------------------
BOOL CChatParser::AddText(string strText, string strColor)
{
	if (!strText.empty())
	{
		CHtmlNode * pNode = new CHtmlNode();
		if (pNode == NULL)
		{
			return FALSE;
		}
		
		pNode->SetName("text");
		pNode->SetContent(strText);		
		pNode->SetAttribute("frontcolor", strColor);	
		
		m_pChatNode->AddChild(pNode);
	}
	
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：添加一个 物品
//-------------------------------------------------------------------
void CChatParser::AddGoods(DWORD dwData)
{
	IGoods *pGoods = (IGoods *)dwData;
	if( pGoods == NULL)
	{
		return ;
	}
	string str = "[";
	AddText(str,"128,128,255");
    CHtmlNode * pNode = new CHtmlNode();
	if (pNode == NULL)
	{
		return ;
	}
	pNode = new CHtmlNode();
	if (pNode == NULL)
	{
		return ;
	}
	pNode->SetName("goods");
    pNode->SetData(dwData);
	pNode->SetContent(pGoods->GetName());
    pNode->SetAttribute("type",LINK_THING);
	m_pChatNode->AddChild(pNode);
	str="]";
	AddText(str,"128,128,255");
}

//-------------------------------------------------------------------
// 描  述：系统聊天信息 
//-------------------------------------------------------------------
BOOL CChatParser::OnSystemChat(const char **ppData, int & nLen)
{
	if (nLen < sizeof(SSystemChatMsg))
	{
		return FALSE;
	}
	
	const SSystemChatMsg * pMsg = (const SSystemChatMsg *)(*ppData);
	
	if (nLen != sizeof(SSystemChatMsg) + pMsg->wTextLen - 1)
	{
		return FALSE;
	}

	m_nCustomCmd = pMsg->nCustomCmd;
	*ppData = pMsg->szText;
	nLen = pMsg->wTextLen;
		
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：附近聊天信息 
//-------------------------------------------------------------------
BOOL CChatParser::OnLocalChat(const char **ppData, int &nLen)
{
	if (nLen < sizeof(SLocalChatMsg))
	{
		return FALSE;
	}
	
	const SLocalChatMsg * pMsg = (const SLocalChatMsg *)(*ppData);
	
	if (nLen != sizeof(SLocalChatMsg) + pMsg->wTextLen - 1)
	{
		return FALSE;
	}
	
	*ppData = pMsg->szText;
	nLen = pMsg->wTextLen;
	
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：私人聊天信息 
//-------------------------------------------------------------------
BOOL CChatParser::OnPrivateChat(const char **ppData, int &nLen)
{
	if (nLen < sizeof(SPrivateChatMsg))
	{
		return FALSE;
	}
	
	const SPrivateChatMsg * pMsg = (const SPrivateChatMsg *)(*ppData);
	
	if (nLen != sizeof(SPrivateChatMsg) + pMsg->wTextLen - 1)
	{
		return FALSE;
	}
	
	*ppData = pMsg->szText;
	nLen = pMsg->wTextLen;
	
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：队伍聊天信息 
//-------------------------------------------------------------------
BOOL CChatParser::OnTeamChat(const char **ppData, int &nLen)
{
	if (nLen < sizeof(STeamChatMsg))
	{
		return FALSE;
	}
	
	const STeamChatMsg * pMsg = (const STeamChatMsg *)(*ppData);
	
	if (nLen != sizeof(STeamChatMsg) + pMsg->wTextLen - 1)
	{
		return FALSE;
	}
	
	*ppData = pMsg->szText;
	nLen = pMsg->wTextLen;
	
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：氏族聊天信息 
//-------------------------------------------------------------------
BOOL CChatParser::OnClanChat(const char **ppData, int &nLen)
{
	if (nLen < sizeof(SClanChatMsg))
	{
		return FALSE;
	}
	
	const SClanChatMsg * pMsg = (const SClanChatMsg *)(*ppData);
	
	if (nLen != sizeof(SClanChatMsg) + pMsg->wTextLen - 1)
	{
		return FALSE;
	}
	
	*ppData = pMsg->szText;
	nLen = pMsg->wTextLen;
	
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：世界聊天信息 
//-------------------------------------------------------------------
BOOL CChatParser::OnWorldChat(const char **ppData, int &nLen)
{
	if (nLen < sizeof(SWorldChatMsg))
	{
		return FALSE;
	}
	
	const SWorldChatMsg * pMsg = (const SWorldChatMsg *)(*ppData);
	
	if (nLen != sizeof(SWorldChatMsg) + pMsg->wTextLen - 1)
	{
		return FALSE;
	}
	
	*ppData = pMsg->szText;
	nLen = pMsg->wTextLen;
	
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：怪物聊天信息 
//-------------------------------------------------------------------
BOOL CChatParser::OnMonsterChat(const char *pData,int nLen)
{
	SMonsterChatMsg *pMonsterChatMsg = (SMonsterChatMsg *)pData;
	if( nLen < sizeof(SMonsterChatMsg))
	{
		return FALSE;
	}
	m_uidSender = pMonsterChatMsg->header.uidSender;
    ShowPaoPao(pMonsterChatMsg->szText);
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：显示聊天泡泡 
//-------------------------------------------------------------------
void CChatParser::ShowPaoPao(const char * pszText)
{
	IPaoPao ** ppPaoPao = NULL;
	ICreature *pCreature = CChatClient::Instance()->GetCreature(m_uidSender);
	if (pCreature == NULL || pCreature->GetMapItem() == NULL)
	{
		return;
	}
   
	// 获取泡泡
	CMapItemView * pMapItemView = pCreature->GetMapItem();		
	pMapItemView->OnCommand(CrtCmdChatInfo, (DWORD)&ppPaoPao, 0);
	if (ppPaoPao == NULL)
	{
		return;
	}

	// 如果还没有创建泡泡，则创建一个
	if (*ppPaoPao == NULL)
	{
		*ppPaoPao = CChatClient::Instance()->CreatePaoPao(m_uidSender);
		if (*ppPaoPao == NULL)
		{ 
			return;
		}
	}

	// 设置泡泡要显示的内容
	(*ppPaoPao)->SetText(pszText);
}

//-------------------------------------------------------------------
// 描  述：处理自定义的命令 
//-------------------------------------------------------------------
void CChatParser::OnCustomCmd(LPCSTR lpszText)
{
	switch(m_nCustomCmd) 
	{
	case CHAT_CMD_PING:
		OnPingCmd();
		break;
		
	case CHAT_CMD_BULLETIN:
		OnBulletinCmd(lpszText);
		break;
		
	case CHAT_CMD_FAIL:
		OnFailCmd(lpszText);
		break;

	default:
		break;
	}

	m_nCustomCmd = CHAT_CMD_NONE;
}

//-------------------------------------------------------------------
// 描  述：ping命令处理
//-------------------------------------------------------------------
void CChatParser::OnPingCmd(void)
{
	char buf[100];
	
	sprintf(buf, "PING: %d", CChatClient::Instance()->PingStop());

	if (!ParseContent(buf, strlen(buf) + 1))
	{
		delete m_pChatNode;
		m_pChatNode = NULL;
	}
}

//-------------------------------------------------------------------
// 描  述：公告处理 
//-------------------------------------------------------------------
void CChatParser::OnBulletinCmd(LPCSTR lpszText)
{
	CChatClient::Instance()->m_pBulletinWnd->AddBulletin(lpszText);
	delete m_pChatNode;
	m_pChatNode = NULL;
}

//-------------------------------------------------------------------
// 描  述：发送私聊信息失败处理 
//-------------------------------------------------------------------
void CChatParser::OnFailCmd(LPCSTR lpszText)
{
	IWndMgr * pWndMgr = CChatClient::Instance()->m_pGlobal->m_pWndMgr;
	if (pWndMgr != NULL)
	{
		pWndMgr->OnEvent(WND_CHAT_CHANNEL, WE_WW_CHANNEL_SETCHANNEL, CHANNEL_LOCAL, 0);
	}	

	if (!ParseContent(lpszText, strlen(lpszText) + 1))
	{
		delete m_pChatNode;
		m_pChatNode = NULL;
	}
}
//------------------------------------------------------------------------