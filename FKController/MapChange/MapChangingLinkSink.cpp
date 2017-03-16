/**
*	created:		2012-6-24   21:12
*	filename: 		MapChangingLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "MapChangingLinkSink.h"
//------------------------------------------------------------------------
CMapChangingLinkSink g_MapChangingLinkSink;
//------------------------------------------------------------------------
CMapChangingLinkSink::CMapChangingLinkSink()
{
	m_pClientLink = NULL;	
	m_username[0] = '\0';
	m_password[0] = '\0';
	m_lActorID = 0;
	memset(&m_info, 0, sizeof(SServerKey));
	m_dwpActor = 0;

	m_nRetryCount = 0;
	m_nErrorRetryCount = 0;
	m_bReConnectingFlag = FALSE;
}
//------------------------------------------------------------------------
CMapChangingLinkSink::~CMapChangingLinkSink()
{

}
//------------------------------------------------------------------------
void CMapChangingLinkSink::OnConnectError(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	/*
	char szTemp[256];
	sprintf(szTemp, "CMapChangingLinkSink::OnConnectError连接服务器失败。[SockID = %d， Msg：%s，ErrorCode：%d]", dwSocketID, szMsg, dwErrorCode);
	tPrintEventLn(szTemp);

	if (!m_bReConnectingFlag)
	{
		g_pGlobal->m_pWndMgr->CustomMsgBox("地图切换状态出错(ERROR)");
		g_pGlobal->GetSoundEngine()->PlaySound(504);
	}	
	*/
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::OnConnectOK(DWORD dwSocketID)
{
	tPrintEventLn("连接OK,  CMapChangingLinkSink::OnConnectOK ");
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::OnConnectClose(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	/*
	m_nErrorRetryCount--;
	if (m_nErrorRetryCount >= 0)
	{
		m_bReConnectingFlag = TRUE;
		__ConnectToTargetMap();
	}
	else
	{
		char szTemp[256];
		sprintf(szTemp, "CMapChangingLinkSink::OnConnectClose连接服务器失败。[SockID = %d， Msg：%s，ErrorCode：%d]", dwSocketID, szMsg, dwErrorCode);
		tPrintEventLn(szTemp);
		g_pGlobal->m_pWndMgr->CustomMsgBox("地图切换状态出错(Close)");
		g_pGlobal->GetSoundEngine()->PlaySound(504);
	}
	*/
}
//------------------------------------------------------------------------
int	CMapChangingLinkSink::Send(LPCSTR buf, int nLen)
{
	if(m_pClientLink == NULL)
	{
		return 0;
	}

	return m_pClientLink->Send(buf, nLen);
}
//------------------------------------------------------------------------
IClientLink * CMapChangingLinkSink::GetClientLink(void)
{
	return m_pClientLink;
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::setData(char *user, char *password)
{
	//strncpy(m_username, user, USER_NAME_MAX_LEN + 1);	
	//if (password)  //如果不是第一次登录, 密码为空, 仍用上一次的密码就可以了
	//{
	//	strncpy(m_password, password, PASSWORD_MAX_LEN);
	//	m_password[PASSWORD_MAX_LEN] = '\0';
	//}	
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::setActorID(int ActorID)
{
	m_lActorID = ActorID;
}
//------------------------------------------------------------------------
BOOL CMapChangingLinkSink::OnRecv(DWORD dwSocketID, LPCSTR buf, int nLen)
{
	/*
	if(nLen < sizeof(WORD))
	{
		return FALSE;
	}

	WORD wMsgRoot = *((WORD *)(buf));
	switch(wMsgRoot)
	{
	case CMD_ROOT_ERROR:
		{
			TraceLn("切换地图出错了 , CMapChangingLinkSink::OnRecv, CMD_ROOT_ERROR");

			SC_ERRORCODE_MSGFORMAT * pError = (SC_ERRORCODE_MSGFORMAT *)(buf);
			//TraceLn(pError->m_szErrorDesc);
			char *pErrDesc = pError->m_szErrorDesc;
			//没返回错误信息,要自己添加  #_#
			if (strlen(pErrDesc) == 0)
			{				
				pErrDesc = ErrDescTable[pError->m_wErrorCode];				
			}

			tPrintEventLn("%s   (%d) ", pErrDesc, pError->m_wErrorCode);

			g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_ERRORINFO, 
				(DWORD)pErrDesc, 0);
		}
		break;
	case CMD_ROOT_SELECTACTOR:
		{
			if(nLen < sizeof(SC_SELECTACTOR_HEAD))
			{
				return FALSE;
			}

			SC_SELECTACTOR_HEAD * pSelectMsg = (SC_SELECTACTOR_HEAD *)(buf);			
			switch(pSelectMsg->m_wKeyMain)
			{
			case ROOTSELECTACTOR_SC_MAIN_TURNRUN:
				{
					tPrintEventLn("切换地图完成转运行态 , CMapChangingLinkSink::OnRecv, ROOTSELECTACTOR_SC_MAIN_TURNRUN");

					//切换到运行态去
					g_RunClientLinkSink.m_pClientLink = m_pClientLink;
					m_pClientLink->GotoNextState(&g_RunClientLinkSink);
					g_pGlobal->m_pAppNetworkLink = &g_RunClientLinkSink;

					//关闭平滑关闭模式, 连接断开时会提示出错
					g_RunClientLinkSink.SetGraceful(FALSE);   

					//聊天模块
					g_pGlobal->GetChatClient()->ShowWindow(CW_SHOW);

					//使用第一种操作模式
					g_pGameControl->changeControlMode(1);	

				}
				break;		

			case ROOTSELECTACTOR_SC_MAIN_ENTERCODE:
				{
					//收到ENTERCODE
					tPrintEventLn("CMapChangingLinkSink::OnRecv 收到  ENTERCODE");	
					if (buf == NULL  || nLen != sizeof(SC_SELECTACTOR_ENTERCODE))
					{
						return FALSE;
					}

					//停掉计时器, 否则时间到后会重连
					g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->KillTimer(TIMER_EVENT_WAITENTERCODE_EXPIRE, this);
					m_bReConnectingFlag = FALSE;

					SC_SELECTACTOR_ENTERCODE *pEnterCode = (SC_SELECTACTOR_ENTERCODE*)buf;
					SetMacPosition(pEnterCode->m_dwEnterCode);
					__SendMacToServer();
					__SendAccountAndPassword();

				}
				break;
			}
		}
		break;

	default:break;
	}
	*/
	return TRUE;
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::OnTimer(DWORD dwEventID)
{
	/*
	if (dwEventID == TIMER_EVENT_WAITENTERCODE_EXPIRE)
	{
		g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->KillTimer(TIMER_EVENT_WAITENTERCODE_EXPIRE, this);
		m_nRetryCount--;
		if (m_nRetryCount >= 0)
		{
			tPrintEventLn("MapChangingLinkSink等待超时, 重连...");
			m_bReConnectingFlag = TRUE;
			__ConnectToTargetMap();
		}
		else
		{
			tPrintEventLn("MapChangingLinkSink等待超时, 而且重连失败...");			
			g_pGlobal->m_pAppNetworkLink->GetClientLink()->CloseConnect();
			g_pGlobal->m_pWndMgr->ShowWnd(WND_CHAT, SW_HIDE);			
			g_pGlobal->m_pWndMgr->ShowWnd(WND_LOADING, SW_HIDE);			
			g_pGlobal->m_pWndMgr->ShowWnd(WND_LOGIN_BAK, SW_SHOW);
			g_pGlobal->m_pWndMgr->ShowWnd(WND_LOGIN, SW_SHOW);			
			g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_ERRORINFO, 
				(DWORD)"连接服务器超时, 请重新登录!", 0);	
		}
	}
	*/
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::SetServer(const char *pData, int nLen)
{
	if (nLen != sizeof(SServerKey))
	{
		tPrintEventLn("CMapChangingLinkSink::SetServer, 目标服务器数据出错");
		return;
	}

	memcpy(&m_info, pData, nLen);
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::__ConnectToTargetMap(void)
{
	/*
	if (m_info.wPort != 0)
	{
		try
		{
			g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->SetTimer(TIMER_EVENT_WAITENTERCODE_EXPIRE, this, 10000, "CMapChangingLinkSink::__ConnectToTargetMap");

			g_pGameControl->createConnection(SINKSTATE_MAPCHINGING);		
			g_MapChangingLinkSink.m_pClientLink->Connect(m_info.szIP, m_info.wPort);	
		}
		catch (...) {
			tPrintEventLn("CMapChangingLinkSink::__ConnectToTargetMap发生非法");			
		}

	}
	else
	{
		tPrintEventLn("CMapChangingLinkSink::__ConnectToTargetMap, 服务器数据错");
	}
	*/
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::OnAction(IEvent *pEvent)
{	 
	/*
	if (pEvent == NULL)
	{
		tPrintEventLn("CMapChangingLinkSink::OnAction, pEvent == NULL");
		return;
	}

	int nLen;
	SC_MSG_PLAYERLOGOUT_CONTEXT *pContext;
	try
	{
		pContext = (SC_MSG_PLAYERLOGOUT_CONTEXT *)pEvent->GetContext(nLen);
	}
	catch (...) {
		tPrintEventLn("CMapChangingLinkSink::OnAction()    pEvent->GetContext() 非法");

	}

	if (pContext && nLen == sizeof(SC_MSG_PLAYERLOGOUT_CONTEXT))
	{		
		if (pContext->dwStateFlag == 3)
		{
			WORD wEventID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_PLAYERLOGOUT);
			try
			{
				g_pGlobal->m_pEventServer->RemoveListener(this, wEventID, SRC_TYPE_ID_ACTOR, m_dwpActor);
			}
			catch(...)
			{
				tPrintEventLn("CMapChangingLinkSink::OnAction()    pEventServer->RemoveListener() 非法");
			}

			m_dwpActor = 0;		

			m_nRetryCount = 1;   //设置重连次数, 在5秒后收不到entercode会重连		
			m_nErrorRetryCount = 1; //设置重连次数, 连接出错后会重连 1 次
			__ConnectToTargetMap();	
		}
	}
	else
	{
		tPrintEventLn("CMapChangingLinkSink::OnAction, 接收到现场出错");
	}
	*/
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::RegisterLogonMsg(void)
{
	//if (g_pGlobal->m_pEventServer)
	//{
	//	WORD wEventID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_PLAYERLOGOUT);
	//	m_dwpActor = (DWORD)g_pGlobal->m_pPlayer;
	//	g_pGlobal->m_pEventServer->AddListener(this, wEventID, SRC_TYPE_ID_ACTOR, m_dwpActor, "CMapChangingLinkSink::RegisterLogonMsg111");
	//}
}
//------------------------------------------------------------------------
int CMapChangingLinkSink::GetType(void)
{
	return NETWORKLINKTYPE_MAPCHANGING;
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::__SendMacToServer(void)
{
	//CS_SELECTACTOR_MAC_HEAD  MacHead(const_cast<char *>(GetMacPackage()));
	//char temp[2048];
	//CDataArchive DataArchive(temp, 2048);
	//DataArchive.Write(&MacHead, sizeof(CS_SELECTACTOR_MAC_HEAD));
	//m_pClientLink->Send(temp, DataArchive.GetBufferOffset());
}
//------------------------------------------------------------------------
void CMapChangingLinkSink::__SendAccountAndPassword(void)
{
	//IQQEnterGame *pQQ = g_pGlobal->GetQQEnterGame();
	//if (pQQ == NULL)
	//{
	//	tPrintEventLn("QQ登录模块出错");
	//	return;
	//}

	//char szMsgBuf[1024];
	//CDataArchive dataarchive(szMsgBuf, sizeof(szMsgBuf));		  

	//if (pQQ->IsQQEnter())
	//{
	//	CS_SELECTACTOR_SELECT_HEAD_QQ selecthead(m_lActorID);	
	//	selecthead.lFirstLogin = 0;
	//	NETTOOLS::GetMACAddress(selecthead.szSerialNO);
	//	strcpy(selecthead.m_szQQ, pQQ->GetQQ());
	//	int nQQDataLen;
	//	const char *ptQQData = pQQ->GetQQData(nQQDataLen);
	//	memcpy(selecthead.m_szQQData, ptQQData, nQQDataLen);
	//	selecthead.m_nQQDataLen = nQQDataLen;

	//	MD5 md5;
	//	char temp[256];
	//	memcpy(temp, ptQQData, nQQDataLen);
	//	md5.update((unsigned char *)temp, nQQDataLen);
	//	char szMAC[20];
	//	NETTOOLS::GetMACAddress(szMAC);
	//	md5.update((unsigned char *)szMAC, 17);
	//	md5.finalize();		
	//	strncpy(selecthead.m_szQQDataMD5, md5.hex_digest().c_str(), 32);
	//	strncpy(selecthead.m_szSureCode, GetExtraPassword(), 32);  //附加码
	//	dataarchive.Write(&selecthead, sizeof(CS_SELECTACTOR_SELECT_HEAD_QQ));	
	//} 
	//else
	//{
	//	CS_SELECTACTOR_SELECT_HEAD selecthead(m_lActorID);	
	//	selecthead.lFirstLogin = 0;
	//	NETTOOLS::GetMACAddress(selecthead.szSerialNO);
	//	strncpy(selecthead.m_szUserName, m_username, USER_NAME_MAX_LEN);
	//	strncpy(selecthead.m_szPassword, m_password, PASSWORD_MAX_LEN);	
	//	strncpy(selecthead.m_szSureCode, GetExtraPassword(), 32);  //附加码
	//	dataarchive.Write(&selecthead, sizeof(CS_SELECTACTOR_SELECT_HEAD));	
	//}

	//if (m_pClientLink->Send(szMsgBuf, dataarchive.GetBufferOffset()) == FALSE)
	//{
	//	tPrintEventLn("CMapChangingLinkSink 发送帐号密码失败");
	//}	
}
//------------------------------------------------------------------------