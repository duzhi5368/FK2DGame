/**
*	created:		2012-6-30   4:54
*	filename: 		ChatClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现聊天模块
*/
//------------------------------------------------------------------------
#include "../IPaoPao.h"
#include "../Include/ChatClient.h"
#include "../Include/PaoPao.h"
#include "../Include/TitleClient.h"
#include "../../FKTrace/IFKTrace.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../../FKGame/Team/ITeamClient.h"
#include "../../FKGame/Friend/IFriendClient.h"
#include "../../FKGame/SceneObj/ThingDef.h"
#include "../../FKGame/ClientWorld/IThingTable.h"
#include "../../FKGame/ClientWorld/IClientWorld.h"
#include "../../FKGame/SceneObj/IThingFactory.h"
#include "../../FKGame/WndSys/WndDef.h"
#include "../../MsgCommon/MsgKeyRoot.h"
#include "../../MsgCommon/MsgKeySubAction.h"
#include "../../MsgCommon/MsgKeySystem.h"
//------------------------------------------------------------------------
CChatClient * CChatClient::s_pModule = NULL;
CFKTrace				g_DefaultTrace;
CWndSysHelper			g_wndHelper;
IClientGlobal *			g_pGlobal		= NULL;
//------------------------------------------------------------------------
CChatClient::CChatClient(void)
: m_pGlobal(NULL)
, m_pOutputWindow(NULL)
, m_bCreated(FALSE)
, m_pInfoWindow(NULL)
, m_pResScript(NULL)
, m_pBulletinWnd(NULL)
, m_pHelpInfoWnd(NULL)
{
	m_dwPingStartTime = GetTickCount();
}
//------------------------------------------------------------------------
CChatClient::~CChatClient(void)
{
	// 退订聊天消息
	IController * pController = m_pGlobal->GetController();
	if (pController != NULL)
	{	
		pController->RemoveReceiver(static_cast<IDataReceiver *>(this), 
		                            CMD_ROOT_CHAT, 0);
	}
	
	IEventServer * pEventServer = m_pGlobal->m_pEventServer;
	if (pEventServer != NULL)
	{
		// 退订系统提示信息
		WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_SYSTEMMESSAGE);
		pEventServer->RemoveListener(static_cast<IEventActionListener *>(&m_sysMsgReceiver),
			wMsgID, SRC_TYPE_ID_ACTOR);

		// 退订进入游戏信息
		pEventServer->RemoveListener(static_cast<IEventActionListener *>(this), 
			MSG_SYSTEM_CLIENT_PLAY, SRC_TYPE_ID_SYSTEM);
		
		// 退订小退信息
		pEventServer->RemoveListener(static_cast<IEventActionListener *>(this), 
			MSG_SYSTEM_CLIENT_BACKSELEHERO, SRC_TYPE_ID_SYSTEM);
	}	
	
	if (m_pOutputWindow != NULL)
	{
		delete m_pOutputWindow;
		m_pOutputWindow = NULL;
	}
	if (m_pInfoWindow != NULL)
	{
		delete m_pInfoWindow;
		m_pInfoWindow = NULL;
	}
	if (m_pBulletinWnd != NULL)
	{
		delete m_pBulletinWnd;
		m_pBulletinWnd = NULL;
	}
	if (m_pHelpInfoWnd != NULL)
	{
		delete m_pHelpInfoWnd;
		m_pHelpInfoWnd = NULL;
	}
}

//-------------------------------------------------------------------
// 描  述：获取模块的唯一实例 
// 参  数：void
// 返回值：CChatClient * - 模块实例
//-------------------------------------------------------------------
CChatClient * CChatClient::Instance(void)
{
	if (s_pModule == NULL)
	{
		s_pModule = new CChatClient();
	}

	return s_pModule;
}

//-------------------------------------------------------------------
// 描  述：创建聊天模块，载入相关的资源 
// 参  数：[输入] IClientGlobal *pGlobal - 全局管理器
// 返回值：BOOL - 创建成功返回TRUE，否则返回FALSE
//-------------------------------------------------------------------
BOOL CChatClient::Create(IClientGlobal * pGlobal)
{
	if (m_bCreated)
	{
		return TRUE;
	}

	if (pGlobal == NULL)
	{
		return FALSE;
	}
	m_pGlobal = pGlobal;
	g_pGlobal = pGlobal;
	// 载入脚本资源		
#ifdef _DEBUG
	m_pResScript = m_pGlobal->m_ResMgrHelper.m_pResMgr->LoadResScript("Data\\Scp\\Chat.scp");
#else
	BYTE pKey[] = "abcd";
	m_pResScript = m_pGlobal->m_ResMgrHelper.m_pResMgr->LoadResScript("Data\\Scp\\Chat.res", FALSE, pKey);
#endif
	
	if (m_pResScript == NULL)
	{
		PrintEventLn("创建 ChatClient 失败！缺少资源配置文件。");
		return FALSE;
	}
	TraceLn("ChatClient - Chat.res LOADED");

	// 创建信息输出窗口
	m_pOutputWindow = new COutputWindow();
	if (m_pOutputWindow == NULL 
		|| !m_pOutputWindow->Create(m_pResScript->GetResObject(OUTPUTWINDOW_ID)))
	{
		PrintEventLn("ChatClient (FAIL) - m_pOutputWindow->Create");
		return FALSE;
	}	
	
	// 创建装备属性显示窗口
	m_pInfoWindow = new CInfoWindow();
	if (m_pInfoWindow == NULL 
		|| !m_pInfoWindow->Create(m_pResScript->GetResObject(INFOWINDOW_ID)))
	{
		PrintEventLn("ChatClient (FAIL) - m_pInfoWindow->Create");
		return FALSE;
	}

	// 初始化系统信息接收器
	if (!m_sysMsgReceiver.Init(m_pResScript->GetResObject(PROMPTWINDOW_ID)))
	{
		PrintEventLn("ChatClient (FAIL) - m_sysMsgReceiver.Init");
		return FALSE;
	}

	// 创建系统公告窗口
	m_pBulletinWnd = new CBulletinWnd();
	if (m_pBulletinWnd == NULL
		|| !m_pBulletinWnd->Create(m_pResScript->GetResObject(BULLETINWINDOW_ID)))
	{
		PrintEventLn("ChatClient (FAIL) - m_pBulletinWnd->Create");
		return FALSE;
	}
	
	// 创建帮助提示栏
	m_pHelpInfoWnd = new CPromptWnd();
	if (m_pHelpInfoWnd == NULL
		|| !m_pHelpInfoWnd->Create(m_pResScript->GetResObject(HELPINFOWINDOW_ID)))
	{
		PrintEventLn("ChatClient (FAIL) - m_pHelpInfoWnd->Create");
		return FALSE;
	}
	
	// 创建称号系统
	if( !CTitleClient::Instance()->Create(pGlobal,m_pResScript))
	{
		PrintEventLn("ChatClient (FAIL) - Create TitleClient Error");
		return FALSE;
	}
    

	// 订阅聊天消息
	IController * pController = m_pGlobal->GetController();
	if (pController == NULL
		|| !pController->AddReceiver(static_cast<IDataReceiver *>(this), CMD_ROOT_CHAT, 0))
	{
		return FALSE;
	}	
		
	IEventServer * pEventServer = m_pGlobal->m_pEventServer;
	if (pEventServer == NULL)
	{
		return FALSE;
	}

	//  创建聊天菜单
	m_pMenu = new CChatMenu();
	if (m_pMenu == NULL || !m_pMenu->Create(m_pResScript->GetResObject(CHAT_MENU_WND_ID)))
	{
		PrintEventLn("ChatClient (FAIL) - Create ChatMenu Error");
		return FALSE;
	}
	SetMenu(g_menu);
	m_pMenu->SetEventReceiver(static_cast<IMenuEventReceiver *>(this));

	// 订阅系统提示信息
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_SYSTEMMESSAGE);
	if (!pEventServer->AddListener(static_cast<IEventActionListener *>(&m_sysMsgReceiver),
		wMsgID, SRC_TYPE_ID_ACTOR, 0, "CChatClient::Create"))
	{
		return FALSE;
	}	
	
	// 订阅进入游戏信息
	if (!pEventServer->AddListener(static_cast<IEventActionListener *>(this), 
		MSG_SYSTEM_CLIENT_PLAY, SRC_TYPE_ID_SYSTEM, 0, "CChatClient::Create"))
	{
		return FALSE;
	}

	// 订阅小退信息
	if (!pEventServer->AddListener(static_cast<IEventActionListener *>(this), 
		MSG_SYSTEM_CLIENT_BACKSELEHERO, SRC_TYPE_ID_SYSTEM, 0, "CChatClient::Create"))
	{
		return FALSE;
	}

	TraceLn("ChatClient - CChatClient::Create OK");
	m_bCreated = TRUE;
	return TRUE;
}

//-------------------------------------------------------------------
// 描  述：用户发言 
// 参  数：[输入] int nChannel - 聊天频道
// 参  数：[输入] const char * pszReceiver - 信息的接收者
// 参  数：[输入] const char * pszText - 发言的文本
// 返回值：BOOL - 想要说的话如果有效且可以发送返回TRUE，否则返回FALSE
//-------------------------------------------------------------------
BOOL CChatClient::Speak(int nChannel, const char * pszReceiver, const char * pszText)
{
	return m_controller.Speak(nChannel, pszReceiver, pszText);
}

//-------------------------------------------------------------------
// 描  述： 设置聊天频道的开闭
// 参  数：[输入] int nChannel - 聊天频道
// 参  数：[输入] BOOL bOnOff - 是否打开
// 返回值：void
//-------------------------------------------------------------------
void CChatClient::ChannelOn(int nChannel, BOOL bOnOff)
{
	m_pOutputWindow->ChannelOn(nChannel, bOnOff);
}

//-------------------------------------------------------------------
// 描  述：设定是否显示聊天信息窗口 
// 参  数：[输入] int nType - 显示方式
// 返回值：void
//-------------------------------------------------------------------
void CChatClient::ShowWindow(int nType)
{	
	if (nType == CW_ALTERNATE)
	{	
		// 交替显示信息输出窗口
		if (m_pOutputWindow->IsVisible())
		{
			m_pOutputWindow->ShowWindow(FALSE);
		}
		else
		{
			m_pOutputWindow->ShowWindow(TRUE);
		}
	}
	else if (nType == CW_HIDE)
	{
		m_pOutputWindow->ShowWindow(FALSE);
	}
	else if (nType == CW_SHOW)
	{
		m_pOutputWindow->ShowWindow(TRUE);
	}
}

///-------------------------------------------------------------------
// 描  述：设置信息显示窗口的透明度 
// 参  数：[输入] int nLevel - 透明度
// 返回值：void
//-------------------------------------------------------------------
void CChatClient::SetWindowTrans(int nLevel)
{
	m_pOutputWindow->SetTransparence(nLevel);
}

//-------------------------------------------------------------------
// 描  述：显示接收到的聊天信息 
// 参  数：[输入] const char * pData - 接收到的数据
// 参  数：[输入] int nLen - 数据的长度
// 返回值：BOOL - 如果聊天信息可以正确显示则返回TRUE，否则返回FALSE
//-------------------------------------------------------------------
BOOL CChatClient::OnReceived(const char * pData, int nLen)
{
	return m_pOutputWindow->Display(pData, nLen);
}

//-------------------------------------------------------------------
// 描  述：释放聊天模块使用的资源
// 参  数：void
// 返回值：void
//-------------------------------------------------------------------
void CChatClient::Release(void)
{
	delete this;
}

//-------------------------------------------------------------------
// 描  述：接收到小退信息 
//-------------------------------------------------------------------
void CChatClient::OnAction(IEvent *pEvent)
{
	switch (pEvent->GetID())
	{
	case MSG_SYSTEM_CLIENT_PLAY:
		m_sysMsgReceiver.Reset();
		m_sysMsgReceiver.SetPromptOn();
		m_pHelpInfoWnd->ClearMessage();
		break;
		
	case MSG_SYSTEM_CLIENT_BACKSELEHERO:
		m_sysMsgReceiver.Reset();
		m_pHelpInfoWnd->ClearMessage();
		m_pOutputWindow->Clear();
		m_InvlidThingMgr.ReleaseAll();
		break;

	default:
		break;
	}
}

//-------------------------------------------------------------------
// 描  述：显示系统的提示信息
// 参  数：const char * pszText - 提示文本
// 返回值：void
//-------------------------------------------------------------------
void CChatClient::DisplaySysMessage(const char * pszText)
{
	char buf[1024];
	int nLen = strlen(pszText);

	// 提示文本的长度不能超过缓冲区的大小
	if (sizeof(SSystemChatMsg) + nLen > sizeof(buf))
	{
		return;
	}

	SSystemChatMsg * pMsg = (SSystemChatMsg *)buf;
	pMsg->header.nChannel = CHANNEL_SYSTEM;
	pMsg->nCustomCmd = CHAT_CMD_NONE;
	pMsg->wTextLen = nLen + 1;
	strncpy(pMsg->szText, pszText, nLen + 1);
	
	m_pOutputWindow->Display(buf, sizeof(SSystemChatMsg) + nLen);
}

//-------------------------------------------------------------------
// 描  述：创建一个泡泡 
//-------------------------------------------------------------------
IPaoPao * CChatClient::CreatePaoPao(UID uid)
{
	return new CPaoPao(uid);
}

//-------------------------------------------------------------------
// 描  述：获取时间轴 
//-------------------------------------------------------------------
ITimeAxis * CChatClient::GetTimeAxis(void)
{
	return m_pGlobal->m_TimerAxisHelper.m_pTimeAxis;
}

//-------------------------------------------------------------------
// 描  述：显示浮动提示信息
//-------------------------------------------------------------------
void CChatClient::DisplayFloatMessage(int nType, const char * lpszText)
{
	m_sysMsgReceiver.ShowMessage(nType, lpszText);
}

//-------------------------------------------------------------------
// 描  述：提示器画图 
//-------------------------------------------------------------------
void CChatClient::OnSysMsgDraw(ICanvas * pCanvas)
{
	m_sysMsgReceiver.OnSysMsgDraw(pCanvas);
}

//-------------------------------------------------------------------
// 描  述：通过UID获取玩家指针
//-------------------------------------------------------------------
IActor * CChatClient::GetPlayer(UID uidPlayer)
{
	IThingTable * pThingTable = m_pGlobal->m_pClientWorld->GetThingTable();
	if (pThingTable == NULL)
	{
		return NULL;
	}
	
	IThing * pThing = pThingTable->Find(uidPlayer);
	if (pThing == NULL)
	{
		return NULL;		
	}
	
	if (pThing->GetThingClass() == NULL 
		|| pThing->GetThingClass()->GetThingClass() != THINGCLASS_ACTOR)
	{
		return NULL;		
	}
	
	return static_cast<IActor *>(pThing);
}

//-------------------------------------------------------------------
// 描  述：通过UID获取生物指针
//-------------------------------------------------------------------
ICreature * CChatClient::GetCreature(UID uidCreature)
{
	IThingTable * pThingTable = m_pGlobal->m_pClientWorld->GetThingTable();
	if (pThingTable == NULL)
	{
		return NULL;
	}
	
	IThing * pThing = pThingTable->Find(uidCreature);
	if (pThing == NULL)
	{
		return NULL;		
	}
	return static_cast<ICreature *>(pThing);
}

//-------------------------------------------------------------------
// 描  述：开始ping
//-------------------------------------------------------------------
void CChatClient::PingStart(void)
{
	m_dwPingStartTime = GetTickCount();
}

//-------------------------------------------------------------------
// 描  述：获取ping的开始时间
//-------------------------------------------------------------------
DWORD CChatClient::PingStop(void)
{
	return (GetTickCount() - m_dwPingStartTime) / 2;
}

void CChatClient::ClearFloatMessage(int nType)
{
    m_sysMsgReceiver.Clear(-1);
}


void CChatClient::ShowTitleWnd(BOOL bTrue,BOOL bTurn)
{
	g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,405,0,0);
}

void CChatClient::Clicked(int nId)
{
    switch(nId)
	{
	case CHAT_PRIVATE:
		{
      
			m_controller.SetChatPlayer(m_strMenu);
			break;
		}
	case CHAT_TEAM:
		{
            ITeamClient *pTeamClient = g_pGlobal->GetTeamClient();
			if( pTeamClient!= NULL)
			{
                pTeamClient->InviteJionTeam(m_strMenu.c_str());
			}
			break;
		}
	case CHAT_FRIEND:
		{
			IFriendClient *pFriendClient = g_pGlobal->GetFriendClient();
			if( pFriendClient!= NULL)
			{
                pFriendClient->AddFriend(m_strMenu.c_str());
			}
		    break;	
		}
	case CHAT_INFO:
		{
			//委托给好友模块完成
			g_pGlobal->m_pWndMgr->OnEvent(FRIEND_WND,403,(WPARAM)(m_strMenu.c_str()),0);

		    break;	
		}
		
	}
}

//-------------------------------------------------------------------
// 获取玩家之间的距离
//-------------------------------------------------------------------	
int CChatClient::GetDistance(UID uid)
{
    return m_controller.GetDistance(uid);
}
//------------------------------------------------------------------------
void CChatClient::SetMenu(const SMenuItem menu[])
{
	m_pMenu->RemoveAll();
	
	for (int i = 0; menu[i].nId != 0; i++)
	{
		m_pMenu->AddItem(menu[i].nId, menu[i].pszName);
	}
}
//------------------------------------------------------------------------
void CChatClient::ShowMenu(POINT pt,LPCSTR lpszPlayer)
{
   m_pMenu->Show(&pt,TRUE);
   m_strMenu = lpszPlayer;
}
//------------------------------------------------------------------------
IThing *CChatClient::GetThing(UID uid)
{
	IThingTable * pThingTable = m_pGlobal->m_pClientWorld->GetThingTable();
	if (pThingTable == NULL)
	{
		return NULL;
	}
	
	IThing * pThing = pThingTable->Find(uid);
	return pThing;
}
//------------------------------------------------------------------------
IThing *CChatClient::CreateGoods(char *buf,int nLen,UID uid)
{
	IThing *pThing = g_pGlobal->m_pThingFactory->CreatePublicThing(THINGCLASS_GOODS,uid,buf,nLen);
	m_InvlidThingMgr.AddThing(pThing);
	return pThing;
}
//------------------------------------------------------------------------