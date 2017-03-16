/**
*	created:		2012-6-28   4:35
*	filename: 		ChatController
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现聊天控制器
*/
//------------------------------------------------------------------------
#include "../Include/ChatController.h"
#include "../Include/ChatClient.h"
#include "../Include/ChatWndMsg.h"
#include "../../FKGameModule/Include/AccountSpace.h"
#include "../../FKGame/SceneObj/IGoods.h"
#include "../../FKGame/Team/ITeamClient.h"
#include "../../FKGame/Clan/IClanClient.h"
#include "../../FKGame/DataPack/DataPacker.h"
#include "../../FKGame/Network/IAppNetworkLink.h"
#include "../../MsgCommon/MsgKeyRoot.h"
#include "../../FKGame/KeywordFilter/IKeywordFilter.h"
#include "../../FKGame/WndSys/WndDef.h"
#include "../../FKGame/ClientWorld/IClientWorld.h"
#include "../../FKGame/ClientWorld/IThingTable.h"
#include "../../FKGame/SceneObj/ThingDef.h"
#include "../../FKGame/DataArchive/IDataArchive.h"
#include "../../FKGame/CoolTime/CoolTimeMgr.h"
//------------------------------------------------------------------------
BOOL IsCommand(const char* text)
{
	const static char* cmdTable[] =
	{
		"goto","ping","setdata","getdata",
		"公告","shutup","kick","admini",
		"home","system","get",	"view",
		"count","m",	"lua"
	};

	// 聊天命令是以‘/’开头的
	if (*text++ == '/')
	{
		// 获取命令
		string strCmd;

		while (*text != '\0' && !::isspace((u_char)*text))
		{
			strCmd += tolower(*text++);
		}

		for (int i = 0; i < sizeof(cmdTable) / sizeof(char*); i++)
		{
			if (0 == stricmp(strCmd.c_str(),cmdTable[i]))
			{
				//找到
				return TRUE;
			}
		}
	}

	return FALSE;
}
//------------------------------------------------------------------------
CChatController::CChatController(void)
: m_nWorldCount(0)
{
}
//------------------------------------------------------------------------
CChatController::~CChatController(void)
{
}

//-------------------------------------------------------------------
// 描  述：发送数据到场景服
//-------------------------------------------------------------------
BOOL CChatController::SendToServer(const char * pData, int nLen)
{
	CDataPacker packer;
	packer.AddWORD(CMD_ROOT_CHAT);
	packer.AddData(pData, nLen);
	
	IAppNetworkLink * pNetwork = CChatClient::Instance()->m_pGlobal->m_pAppNetworkLink;
	if (pNetwork != NULL)
	{
		return (pNetwork->Send(packer.GetBuffer(), packer.GetDataLen()) > 0);
	}
	else
	{
		return FALSE;
	}	
}

//-------------------------------------------------------------------
// 描  述：玩家发言 
// 参  数：[输入] int nChannel - 频道
// 参  数：[输入] const char * pszReceiver - 私聊时要发送给的玩家
// 参  数：[输入] const char * pszText - 说话内容
// 返回值：BOOL - 发送成功返回TRUE，否则返回FALSE
//-------------------------------------------------------------------
BOOL CChatController::Speak(int nChannel, const char * pszReceiver, const char * pszText)
{
	char buf[BUFFER_LEN];
	char TempBuf[BUFFER_LEN];
	int nTempLen = BUFFER_LEN;
	int nLen = 896;

	if (nChannel == CHANNEL_SYSTEM)
	{
		return TRUE;
	}

	if (pszReceiver == NULL || pszText == NULL)
	{
		return FALSE;
	}	
	
	IActor * pActor = CChatClient::Instance()->m_pGlobal->m_pPlayer;
	if (pActor == NULL)
	{
		return FALSE;
	}

	// 进行过滤
	if (!IsCommand(pszText))
	{
		IKeywordFilter * pFilter = CChatClient::Instance()->m_pGlobal->GetKeywordFilter();
		pFilter->Filter(pszText, buf, strlen(pszText), enKL_Replace);
		pszText = buf;
	}
	
	string strText = m_chatFilter.Filter(pszText);
	pszText = strText.c_str();
	int nTextLen = strText.size() + 1;
	if (nTextLen == 1 || nTextLen > CONTENT_LEN)
	{
		return FALSE;
	}
		
	// 如果私聊时没有指定目标玩家,则自动认为是附近聊天
	if ((nChannel == CHANNEL_PRIVATE))
	{
		if (strcmp(pszReceiver, pActor->GetName()) == 0)
		{
			CChatClient::Instance()->DisplayFloatMessage(SMT_FLOAT, "不能对自己私聊！");
			
			IWndMgr * pWndMgr = CChatClient::Instance()->m_pGlobal->m_pWndMgr;
			if (pWndMgr != NULL)
			{
				pWndMgr->OnEvent(WND_CHAT_CHANNEL, WE_WW_CHANNEL_SETCHANNEL, CHANNEL_LOCAL, 0);
			}

			return FALSE;
		}

		if (strlen(pszReceiver) == 0)
		{
			nChannel = CHANNEL_LOCAL;
		}		
	}
	
	// 设置消息头
	SChatMsgHeader * pHeader = (SChatMsgHeader *)buf;
	pHeader->nChannel = nChannel;
	strncpy(pHeader->szSender, pActor->GetName(), 32);

	switch (nChannel)
	{
	case CHANNEL_SYSTEM:
		{
			SSystemChatMsg * pMsg = (SSystemChatMsg *)buf;
			// 打包特殊聊天信息
			if( PackSpecial(pszText,nTextLen,TempBuf,nTempLen) )
			{
				pMsg->wTextLen = nTempLen;
				memcpy(pMsg->szText,TempBuf,nTempLen);
		        nLen = sizeof(SSystemChatMsg) + pMsg->wTextLen - 1;
			}else{
				pMsg->wTextLen = nTextLen;
				strncpy(pMsg->szText, pszText, nTextLen);
			    nLen = sizeof(SSystemChatMsg) + nTextLen - 1;
			}
			break;
		}		

	case CHANNEL_LOCAL:
		{
       		if(!CheckColdTime(FALSE))
			{
			   return FALSE;
			}

			SLocalChatMsg * pMsg = (SLocalChatMsg *)buf;
			// 打包特殊聊天信息
			if( PackSpecial(pszText,nTextLen,TempBuf,nTempLen) )
			{
				pMsg->wTextLen = nTempLen;
				memcpy(pMsg->szText,TempBuf,nTempLen);
	            nLen = sizeof(SLocalChatMsg) + pMsg->wTextLen - 1;
			}else{
				pMsg->wTextLen = nTextLen;
				strncpy(pMsg->szText, pszText, nTextLen);
			    nLen = sizeof(SLocalChatMsg) + nTextLen - 1;
			}
			break;			
		}
		
	case CHANNEL_PRIVATE:
		{
       		if(!CheckColdTime(FALSE))
			{
			   return FALSE;
			}
	
			SPrivateChatMsg * pMsg = (SPrivateChatMsg *)buf;
			strncpy(pMsg->szReceiver, pszReceiver, 32);
			if( PackSpecial(pszText,nTextLen,TempBuf,nTempLen) )
			{
				pMsg->wTextLen = nTempLen;
				memcpy(pMsg->szText,TempBuf,nTempLen);
		        nLen = sizeof(SPrivateChatMsg) + pMsg->wTextLen - 1;				
			}else{
				pMsg->wTextLen = nTextLen;
				strncpy(pMsg->szText, pszText, nTextLen);
			    nLen = sizeof(SPrivateChatMsg) + nTextLen - 1;
			}
			break;
		}

	case CHANNEL_TEAM:
		{
			// 检查是否有队伍
			ITeamClient *pTeamClient = CChatClient::Instance()->m_pGlobal->GetTeamClient();
			if( pTeamClient == NULL)
			{
				return FALSE;
			}
			// 获取队长ID,如果队长 = 0 则没有队伍
			if( pTeamClient->GetCaptainId() == 0)
			{
				return FALSE;
			}
			STeamChatMsg * pMsg = (STeamChatMsg *)buf;
			pMsg->dwTeamId = 0;
			if( PackSpecial(pszText,nTextLen,TempBuf,nTempLen) )
			{
				pMsg->wTextLen = nTempLen;
				memcpy(pMsg->szText,TempBuf,nTempLen);
		        nLen = sizeof(STeamChatMsg) + pMsg->wTextLen - 1;				
			}else{
				pMsg->wTextLen = nTextLen;
				strncpy(pMsg->szText, pszText, nTextLen);
		        nLen = sizeof(STeamChatMsg) + nTextLen - 1;
			}
			break;
		}

	case CHANNEL_CLAN:
		{
			// 氏族聊天,检查是否有氏族
            IActor *pActor = CChatClient::Instance()->m_pGlobal->m_pPlayer;
			if( pActor == NULL)
			{
			    return FALSE;
			}
			SClanChatMsg * pMsg = (SClanChatMsg *)buf;
			pMsg->dwDBID = 0;
			pMsg->dwClanId = 0;
			if( PackSpecial(pszText,nTextLen,TempBuf,nTempLen) )
			{
				pMsg->wTextLen = nTempLen;
				memcpy(pMsg->szText,TempBuf,nTempLen);
		        nLen = sizeof(SClanChatMsg) + pMsg->wTextLen - 1;				
			}else{
				pMsg->wTextLen = nTextLen;
				strncpy(pMsg->szText, pszText, nTextLen);
			    nLen = sizeof(SClanChatMsg) + nTextLen - 1;
			}
			break;
		}		

	case CHANNEL_WORLD:
		{
			if (m_strWorldString == pszText)
			{
				if (m_nWorldCount >= WORLD_CHAT_SAME_MAX)
				{
					CChatClient::Instance()->DisplayFloatMessage(SMT_FLOAT, "请不要频繁地说重复的话！");
					return FALSE;
				}
				
				m_nWorldCount++;
			}
			else
			{
				m_strWorldString = pszText;
				m_nWorldCount = 1;
			}
			
			IActor * pPlayer = CChatClient::Instance()->m_pGlobal->m_pPlayer;
			if (pPlayer == NULL)
			{
				return FALSE;
			}

			string strName = pPlayer->GetName();
			for (string::iterator itr = strName.begin(); itr != strName.end(); itr++)
			{
				*itr = tolower(*itr);
			}
			
			if (strName.find("FKGameGM") != 0)
			{
				//// 限制玩家发言的等级
				//int nActorLevel = pPlayer->GetPropNum(CREATURE_PROPID_LEVEL);
				//if (nActorLevel < 11)
				//{
				//	CChatClient::Instance()->DisplayFloatMessage(SMT_FLOAT, "对不起，12级以下角色无法在世界频道发言。");
				//	return FALSE;
				//}
				//
				// 限制玩家发言的时间间隔
				IColdTimeMgr * pMgr = (IColdTimeMgr *)pActor->GetPart(THINGPARTID_ACTOR_COLDTIMESYSTERM);
				if (pMgr != NULL)
				{
					if (!pMgr->CanOperate(COLDTIME_CHAT_TYPE, COLDTIME_CHAT_WORLD_VALUE))
					{
						CChatClient::Instance()->DisplayFloatMessage(SMT_FLOAT, "你说话的速度太快了，请慢点！");
						return FALSE;
					}
					
					pMgr->Operate(COLDTIME_CHAT_TYPE, COLDTIME_CHAT_WORLD_VALUE);
				}
			}								

			SWorldChatMsg * pMsg = (SWorldChatMsg *)buf;
			if( PackSpecial(pszText,nTextLen,TempBuf,nTempLen) )
			{
				pMsg->wTextLen = nTempLen;
				memcpy(pMsg->szText,TempBuf,nTempLen);
		        nLen = sizeof(SWorldChatMsg) + pMsg->wTextLen - 1;				
			}else{
				pMsg->wTextLen = nTextLen;
				strncpy(pMsg->szText, pszText, nTextLen);
		        nLen = sizeof(SWorldChatMsg) + nTextLen - 1;
			}

			break;
		}		

	default:
		return FALSE;
	}
	return SendToServer(buf, nLen);
}
//-------------------------------------------------------------------
// 描  述：设置要进行私聊的玩家 
// 参  数：[输入] string strPlayer - 玩家名称
// 返回值：void
//-------------------------------------------------------------------
void CChatController::SetChatPlayer(string strPlayer)
{
	CChatClient::Instance()->m_pGlobal->m_pWndMgr->OnEvent(WND_CHAT, 
		MSG_SET_PERSON, (WPARAM)strPlayer.c_str(), NULL);
}

//-------------------------------------------------------------------
// 描  述：设置要说话的文本 
// 参  数：[输入] string strText - 说话文本
// 返回值：void
//-------------------------------------------------------------------
void CChatController::SetChatString(string strText)
{
	CChatClient::Instance()->m_pGlobal->m_pWndMgr->OnEvent(WND_CHAT, 
		MSG_SET_MSG, (WPARAM)strText.c_str(), NULL);
}

//-------------------------------------------------------------------
// 描  述：获取表情位图
// 参  数：[输入] int nIndex - 表情索引
// 返回值：IBitmap * - 返回位图指针
//-------------------------------------------------------------------
IBitmap * CChatController::GetFace(int nIndex) const
{
	IBitmap * pBitmap = NULL;
	CChatClient::Instance()->m_pGlobal->m_pWndMgr->OnEvent(WND_CHAT, 
		MSG_GET_FACE, (WPARAM)(&pBitmap), nIndex);
	
	return pBitmap;
}

//-------------------------------------------------------------------
// 描  述：获取颜色 
// 参  数：[输入] int nIndex - 颜色索引
// 返回值：WPixel - 返回颜色值
//-------------------------------------------------------------------
WPixel CChatController::GetColor(int nIndex) const
{
	WPixel color;
	CChatClient::Instance()->m_pGlobal->m_pWndMgr->OnEvent(WND_CHAT, 
		MSG_GET_COLOR, (WPARAM)(&color), nIndex);

	return color;
}
//------------------------------------------------------------------------
// 获取玩家之间的距离
int  CChatController::GetDistance(UID uid)
{
	static CAccountSpace<> Space;
	if( g_pGlobal->m_pClientWorld && g_pGlobal->m_pClientWorld->GetThingTable() )
	{
		IThing *pThing = g_pGlobal->m_pClientWorld->GetThingTable()->Find(uid);
		if( pThing )
		{
			DWORD dwType = pThing->GetThingClass()->GetThingClass();
			if( dwType == THINGCLASS_MONSTER || dwType == THINGCLASS_ACTOR)
			{
				ICreature *pCure = static_cast<ICreature *>(pThing);
				POINTS ps1 = g_pGlobal->m_pPlayer->GetLoc();
				POINTS ps2 = pCure->GetLoc();
				return Space.Distance(ps1,ps2);
			}
		}
	}
	return 1000000;
}
//------------------------------------------------------------------------
BOOL CChatController::PackSpecial(LPCSTR lpszData,int nLen,char *buf,int &nBufLen)
{
	char tempBuf[1024];
	int  nSize = 1024;
    m_ChatContentParse.ClearBackText();
	string str = lpszData;
    m_ChatContentParse.Parse(str);
	if( !m_ChatContentParse.IsHasSpecialContent() )
	{
		return FALSE;
	}
	string strUid;
	CDataArchive packer(buf,nBufLen);
	// 加上标志
    char c = 22;
    packer.Write(&c,sizeof(c));
	// Item个数
	int nItemCount = m_ChatContentParse.m_ChatItemList.size();
    packer.Write(&nItemCount,sizeof(int));
    // 填写内容
	CChatParse::ChatItemList::iterator iter = m_ChatContentParse.m_ChatItemList.begin();
	for(;iter!=m_ChatContentParse.m_ChatItemList.end();iter++)
	{
        int nCount = 0 ;   //长度+类型+内容 
		int nType = (*iter).nType;
		if( nType == eGoods )
		{
           if( (*iter).strContent.size() < 10)
		   {
			   continue;
		   }
           strUid = (*iter).strContent.substr(1,20);
           if( PackGoods(strUid.c_str(),tempBuf,nSize) )
		   {
              nCount = 2*sizeof(int)+nSize;  
			  packer.Write(&nCount,sizeof(int));
		      packer.Write(&nType,sizeof(int));	
			  packer.Write(tempBuf,nSize);		  
		   }
		}else{
           nCount = 2*sizeof(int)+(*iter).strContent.size()+1;
           packer.Write(&nCount,sizeof(int));
		   packer.Write(&nType,sizeof(int));
           packer.Write((*iter).strContent.c_str(),(*iter).strContent.size()+1);
		}

	}
    packer.Write(&c,sizeof(c));
    nBufLen = packer.GetBufferOffset();
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CChatController::PackGoods(const char *lpsz,char *buf,int& nLen)
{

	UID uid =  _atoi64(lpsz); // 这里是16进制转换.
	IGoods *pGoods = (IGoods *)CChatClient::Instance()->GetThing(uid);
	if( pGoods == NULL)
	{
		return FALSE;
	}
	if( !pGoods->OnGetPublicContext(buf,nLen))
	{
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CChatController::CheckColdTime(BOOL bWorld )
{
	DWORD dwValue = 0;
    bWorld?dwValue=COLDTIME_CHAT_WORLD_VALUE:dwValue=COLDTIME_CHAT_ELSE_VALUE;
	IActor * pActor = CChatClient::Instance()->m_pGlobal->m_pPlayer;
	if (pActor == NULL)
	{
		return FALSE;
	}
	IColdTimeMgr * pMgr = (IColdTimeMgr *)pActor->GetPart(THINGPARTID_ACTOR_COLDTIMESYSTERM);
	if (pMgr != NULL)
	{
			if (!pMgr->CanOperate(COLDTIME_CHAT_TYPE, dwValue))
			{
				CChatClient::Instance()->DisplayFloatMessage(SMT_FLOAT, "你说话的速度太快了，请慢点！");
						return FALSE;
			}
			pMgr->Operate(COLDTIME_CHAT_TYPE, dwValue);
	}
    return TRUE;
}
//------------------------------------------------------------------------