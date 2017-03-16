/**
*	created:		2012-6-25   0:42
*	filename: 		GameController
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Controller/GameController.h"
#include "../MapChange/RunClientLinkSink.h"
//------------------------------------------------------------------------
// 游戏输入控制对象指针
CGameControl *g_pGameControl = NULL;
//------------------------------------------------------------------------
CGameControl::CGameControl()
{
	m_pControlMode = NULL;
	m_nMsgID_Count = 0;
}
//------------------------------------------------------------------------
CGameControl::~CGameControl()
{	
	//RemoveReceiver(&g_MapChanger, CMD_ROOT_THING, ROOTTHING_SC_MAIN_CHANGEMAP);
	//g_HttpAttacker.Stop();
}
//------------------------------------------------------------------------
LRESULT     CGameControl::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	return m_pControlMode->OnRButtonUp(wParam, lParam);
}
//------------------------------------------------------------------------
LRESULT CGameControl::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	return m_pControlMode->OnLButtonUp(wParam, lParam);
}
//------------------------------------------------------------------------
LRESULT CGameControl::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	LRESULT lret = -1;
	
	lret = m_pControlMode->OnLButtonDown(wParam, lParam);

	return lret;
}
//------------------------------------------------------------------------
LRESULT CGameControl::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{	
	return m_pControlMode->OnRButtonDown(wParam, lParam);
}
//------------------------------------------------------------------------
LRESULT CGameControl::OnMouseMove(WPARAM wParam, LPARAM lParam)
{	
	return m_pControlMode->OnMouseMove(wParam, lParam);
}
//------------------------------------------------------------------------
LRESULT CGameControl::OnKeyDown(WPARAM wParam, LPARAM lParam)
{	
	return m_pControlMode->OnKeyDown(wParam, lParam);
}
//------------------------------------------------------------------------
LRESULT CGameControl::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	return m_pControlMode->OnKeyUp(wParam, lParam);
}
//------------------------------------------------------------------------
void CGameControl::LockReceiver(BOOL bLock)
{
	g_RunClientLinkSink.LockReceiver(bLock);
}
//------------------------------------------------------------------------
void CGameControl::SetThreadRun(BOOL bRun)
{
	g_RunClientLinkSink.SetThreadRun(bRun);
}
//------------------------------------------------------------------------
LRESULT CGameControl::OnCommand(DWORD dwCmd,DWORD dwParam1,DWORD dwParam2)
{
	switch( dwCmd )
	{
	case enPlayerLogin:
		{
			
		}
		break;
	default:break;
	}

	return -1;
}
//------------------------------------------------------------------------
int CGameControl::Loop()
{
	return false;
}
//------------------------------------------------------------------------
void CGameControl::Release()
{
	g_pGameControl = NULL;
	delete this;
}
//------------------------------------------------------------------------
// 创建过程
BOOL CGameControl::Create(void)
{
	/*
	g_UserSetting.ReadSetting();

	// 创建网络
	if (createConnection(0) == FALSE)
	{
		TraceLn("创建网络连接  createConnection()");
		return FALSE;
	}
	
	// 注册地图切换者
	AddReceiver(&g_MapChanger, CMD_ROOT_THING, ROOTTHING_SC_MAIN_CHANGEMAP);	
  */
	// 创建窗口系统
	if(m_WndHelper.Create(g_ClientGlobal, static_cast< IWndCallback *>(&m_WndCallback)) == FALSE)
	{			
		TraceLn("创建窗口系统  m_WndHelper  失败");
		return FALSE;
	}
	//g_pGlobal
 
	//初始化控制模式
	m_pControlMode = &m_DefaultMode;
	
	
	g_ClientGlobal->m_pWndMgr = m_WndHelper.m_pWndMgr;
	/*
	g_pGlobal->m_pWndMgr->CreateWnd(WND_INTRO);	
	g_LoginWndController.Create();  
	

	//注册监听信息中心事件
	m_MsgCenterPop.Connect(g_pGlobal->m_pWndMgr, 
		WND_MAIN, 
		WE_WC_MAIN_MSGCENTER_POP,
		this,
		&CGameControl::OnMsgCenterClicked);
	
	g_MinMapShowFlagStrategy.Create();

	g_DllVersion.Init();

	//装入函数的地址
	g_MiscFacade.PT1 = NULL;//GetCurrentThreadId;
	//__ipd = __gPT1();
	
	//创建检证码控制器
	g_ValidationCodeWndController.Create();
	*/
	return TRUE;
}
//------------------------------------------------------------------------
//	下面的代码要对每个状态的m_pClientLink 赋值, 
//		所以参数不能传IAppNetworkLink 的指针,只能传
BOOL CGameControl::createConnection(int SinkState)
{
/*
	if (g_pGlobal->m_pAppNetworkLink != NULL)
	{
		IClientLink *pLink = g_pGlobal->m_pAppNetworkLink->GetClientLink();
		if (pLink && pLink->IsConnected())
		{
			pLink->CloseConnect();
		}
	}

	switch(SinkState) {
	case SINKSTATE_LOGIN:
		if(m_SocketHelper.Create(12345, static_cast<IClientLinkSink*>(&g_LoginClientLinkSink), g_pGlobal->m_TraceHelper.m_pTrace) == FALSE)
		{
			//MessageBox(NULL, "网络连接创建失败!", "error", MB_OK);
			g_pGlobal->m_pWndMgr->CustomMsgBox("网络连接创建失败");
			return FALSE;
		}
		g_LoginClientLinkSink.m_pClientLink = m_SocketHelper.m_pClientLink;
		g_pGlobal->m_pAppNetworkLink = &g_LoginClientLinkSink;
		break;

	case SINKSTATE_ACTORSELECT:
		if(m_SocketHelper.Create(12345, static_cast<IClientLinkSink*>(&g_SelectActorClientLinkSink), g_pGlobal->m_TraceHelper.m_pTrace) == FALSE)
		{
			//MessageBox(NULL, "网络连接创建失败!", "error", MB_OK);
			g_pGlobal->m_pWndMgr->CustomMsgBox("网络连接创建失败");
			return FALSE;
		}
		g_SelectActorClientLinkSink.m_pClientLink = m_SocketHelper.m_pClientLink;
		g_pGlobal->m_pAppNetworkLink = &g_SelectActorClientLinkSink;
		break;

	case SINKSTATE_SCENE:
		if(m_SocketHelper.Create(12345, static_cast<IClientLinkSink*>(&g_SceneClientLinkSink), g_pGlobal->m_TraceHelper.m_pTrace) == FALSE)
		{
			//MessageBox(NULL, "网络连接创建失败!", "error", MB_OK);
			g_pGlobal->m_pWndMgr->CustomMsgBox("网络连接创建失败");
			return FALSE;
		}
		g_SceneClientLinkSink.m_pClientLink = m_SocketHelper.m_pClientLink;
		g_pGlobal->m_pAppNetworkLink = &g_SceneClientLinkSink;
		break;		
	
	case SINKSTATE_RUNNING:
		if(m_SocketHelper.Create(12345, static_cast<IClientLinkSink*>(&g_RunClientLinkSink), g_pGlobal->m_TraceHelper.m_pTrace) == FALSE)
		{
			//MessageBox(NULL, "网络连接创建失败!", "error", MB_OK);
			g_pGlobal->m_pWndMgr->CustomMsgBox("网络连接创建失败");
			return FALSE;
		}
		g_RunClientLinkSink.m_pClientLink = m_SocketHelper.m_pClientLink;	
		g_pGlobal->m_pAppNetworkLink = &g_RunClientLinkSink;
		break;
	
	case SINKSTATE_MAPCHINGING:
		if(m_SocketHelper.Create(12345, static_cast<IClientLinkSink*>(&g_MapChangingLinkSink), g_pGlobal->m_TraceHelper.m_pTrace) == FALSE)
		{
			//MessageBox(NULL, "网络连接创建失败!", "error", MB_OK);
			g_pGlobal->m_pWndMgr->CustomMsgBox("网络连接创建失败");
			return FALSE;
		}
		g_MapChangingLinkSink.m_pClientLink = m_SocketHelper.m_pClientLink;	
		g_pGlobal->m_pAppNetworkLink = &g_MapChangingLinkSink;
		break;
	default:
		TraceLn("unknow SinkState : %d", SinkState);
		break;
	}	
*/
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CGameControl::changeControlMode(int mode)
{	
	m_pControlMode->DisConnectSingal();
	switch(mode) 
	{
	case 1:
		m_pControlMode = &m_ControlMode1;
		break;

	case NULL:		
		//停止使用线程模式处理消息(否则会非法)
		m_pControlMode = &m_DefaultMode;
		break;
	default:
		//停止使用线程模式处理消息(否则会非法)
		m_pControlMode = &m_DefaultMode;
		return FALSE;
	};

	m_pControlMode->ConnectSingal();
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CGameControl::AddReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode)
{
	return g_RunClientLinkSink.AddReceiver(pReceiver, MainMsgCode, SubMsgCode);
}
//------------------------------------------------------------------------
BOOL CGameControl::RemoveReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode)
{
	return g_RunClientLinkSink.RemoveReceiver(pReceiver, MainMsgCode, SubMsgCode);
}
//------------------------------------------------------------------------
int  CGameControl::PushBufferToDataCenterQueue(int nQueueID, LPCSTR pBuffer, int nLen)
{
	/*
	if (nQueueID < 0 && nQueueID >= MSG_CENTER_TYPE_MAX)
	{
		return -1;
	}
	DataCenterMessage data;
	data.nMsgID = m_nMsgID_Count++;
	data.pBuffer = new char[nLen];
	memcpy(data.pBuffer, pBuffer, nLen);
	data.nLen = nLen;
	m_lsQueue[nQueueID].push_back(data);
	if (g_pGlobal->m_pWndMgr)
	{
		g_pGlobal->m_pWndMgr->OnEvent(WND_MAIN, WE_CW_MAIN_MSGCENTER_PUSH,
			nQueueID, 0);
	}
	return data.nMsgID;
	*/
	return -1;
};
//------------------------------------------------------------------------
BOOL CGameControl::PopBufferFromDataCenterQueue(int nQueueID)
{
	/*
	if (nQueueID < 0 && nQueueID >= MSG_CENTER_TYPE_MAX)
	{
		return FALSE;
	}

	if (m_lsQueue[nQueueID].empty())
	{
		return FALSE;
	}

	IEventServer *pEventServer = g_pGlobal->m_pEventServer;
	if (pEventServer == NULL)
	{
		return FALSE;
	}
	 
	DataCenterMessage Msg = m_lsQueue[nQueueID].front();
	SYSTEM_CLIENT_MSGCENTER_CONTEXT Context;
	Context.nQueueID = nQueueID;
	Context.nIndex = Msg.nMsgID;
	memcpy(Context.Buffer , Msg.pBuffer, min(Msg.nLen, 512));	
	pEventServer->FireAction((BYTE*)&Context, sizeof(SYSTEM_CLIENT_MSGCENTER_CONTEXT), MSG_SYSTEM_CLIENT_MSGCENTER_POP, SRC_TYPE_ID_SYSTEM, (DWORD)this);
	m_lsQueue[nQueueID].pop_front();
	delete[] (Msg.pBuffer);
	if (m_lsQueue[nQueueID].empty() && g_pGlobal->m_pWndMgr)
	{
		g_pGlobal->m_pWndMgr->OnEvent(WND_MAIN, WE_CW_MAIN_MSGCENTER_STOPFLASH,
			nQueueID, 0);
	}
	*/
	return TRUE;
};
//------------------------------------------------------------------------
BOOL CGameControl::DeleteDataFromDataCenterQueue(int nIndex)
{
	/*
	for (int i = 0; i < MSG_CENTER_TYPE_MAX; ++i)
	{
		for (std::list<DataCenterMessage>::iterator itor = m_lsQueue[i].begin();
				itor != m_lsQueue[i].end(); ++itor)
		{
			if ((*itor).nMsgID == nIndex)
			{				
				m_lsQueue[i].erase(itor);
				if (m_lsQueue[i].empty() && g_pGlobal->m_pWndMgr)
				{
					g_pGlobal->m_pWndMgr->OnEvent(WND_MAIN, WE_CW_MAIN_MSGCENTER_STOPFLASH,
						i, 0);
				}	
				return TRUE;
			}				
		}		
	}
	*/
	return FALSE;
};
//------------------------------------------------------------------------
BOOL CGameControl::ClearDataFromDataCenterQueue(int nQueueID)
{
	/*
	if (nQueueID < 0 && nQueueID >= MSG_CENTER_TYPE_MAX)
	{
		return FALSE;
	}
	m_lsQueue[nQueueID].clear();
	g_pGlobal->m_pWndMgr->OnEvent(WND_MAIN, WE_CW_MAIN_MSGCENTER_STOPFLASH,
		nQueueID, 0);
	*/
	return TRUE;
};
//------------------------------------------------------------------------
void CGameControl::OnMsgCenterClicked(WPARAM wParam, LPARAM lParam,BOOL &bContinue)
{
	if (m_lsQueue[wParam].empty())
	{
		return;
	}
	
	PopBufferFromDataCenterQueue(wParam);	
}
//------------------------------------------------------------------------
BOOL CGameControl::IsAutoOperate(void)
{
	// return g_BehaviorMgr.IsRunning();	

	return TRUE;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL dllCreateController(IClientGlobal * pGlobal, IController **ppInput)
{
	if(pGlobal == NULL || ppInput == NULL)
	{
		return FALSE;
	}

	g_ClientGlobal = pGlobal;

	TraceLn("开始创建 Controller 控制器。");

	CGameControl * pGameControl = new CGameControl;
	if(NULL == pGameControl)
	{
		TraceLn("[thing]: New Controller fail.");
		return FALSE;
	}

	if(FALSE == pGameControl->Create())
	{
		SAFE_DELETE(pGameControl);
		TraceLn("[thing]: Init Controller fail.");
		return FALSE;
	}

	g_pGameControl = pGameControl;
	*ppInput = static_cast<IController *>(pGameControl);

	TraceLn("创建 Controller 完成。");

	return TRUE;
}
//------------------------------------------------------------------------