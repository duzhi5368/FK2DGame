/**
*	created:		2012-6-24   21:28
*	filename: 		SceneClientLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "SceneClientLinkSink.h"
//------------------------------------------------------------------------
static const int NOTNEED_RECONNECT_LOGINSERVER = 0;            //  不需重连登陆服
static const int NEED_RECONNECT_LOGINSERVER = 1;               //  需重连登陆服
//------------------------------------------------------------------------
CSceneClientLinkSink g_SceneClientLinkSink;
//------------------------------------------------------------------------
void CSceneClientLinkSink::setData(char *user, char *password)
{
	strncpy(m_username, user, USER_NAME_MAX_LEN + 1);	
	if (password)  //如果不是第一次登录, 密码为空, 仍用上一次的密码就可以了
	{
		strncpy(m_password, password, PASSWORD_MAX_LEN);
		m_password[PASSWORD_MAX_LEN] = '\0';
	}	
}
//------------------------------------------------------------------------
CSceneClientLinkSink::CSceneClientLinkSink()
{
	m_pClientLink = NULL;	
	m_username[0] = '\0';
	m_password[0] = '\0';
	m_lActorID = 0;
	m_bConnecting = FALSE;
}
//------------------------------------------------------------------------
CSceneClientLinkSink::~CSceneClientLinkSink()
{

}
//------------------------------------------------------------------------
void CSceneClientLinkSink::setActorID(int ActorID)
{
	m_lActorID = ActorID;
}
//------------------------------------------------------------------------
void CSceneClientLinkSink::OnConnectError(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	//m_bConnecting = FALSE;
	//g_pGlobal->m_pWndMgr->ShowWnd(WND_LOADING, SW_HIDE);

	//char szTemp[256];
	//sprintf(szTemp, "连接Scene服务器失败(Error)。[SockID = %d， Msg：%s，ErrorCode：%d]", dwSocketID, szMsg, dwErrorCode);	
	//tPrintEventLn(szTemp);
	//g_pGlobal->m_pWndMgr->OnEvent(WND_ACTORSELECT, WE_CW_ACTOR_ERROR, 
	//	(DWORD)"连接服务器(S)失败,请重新登录.", NEED_RECONNECT_LOGINSERVER);

	//PLAYSOUND(504);
}
//------------------------------------------------------------------------
void CSceneClientLinkSink::OnConnectOK(DWORD dwSocketID)
{	
	//tPrintEventLn("收到S Server连接OK消息");	
	//g_pGlobal->m_pWndMgr->CreateWnd(WND_LOADING);
	//g_pGlobal->m_pWndMgr->ShowWnd(WND_LOADING, SW_HIDE);
	//g_pGlobal->m_pWndMgr->OnEvent(WND_LOADING, WE_CW_LOGINLOADING, 0, 0);
}
//------------------------------------------------------------------------
void CSceneClientLinkSink::OnConnectClose(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	//g_pGlobal->m_pWndMgr->ShowWnd(WND_LOADING, SW_HIDE);
	//m_bConnecting = FALSE;
	//char szTemp[256];
	//sprintf(szTemp, "连接Scene服务器失败 (Close)。[SockID = %d， Msg：%s，ErrorCode：%d]", dwSocketID, szMsg, dwErrorCode);
	//tPrintEventLn(szTemp);
	//g_pGlobal->m_pWndMgr->OnEvent(WND_ACTORSELECT, WE_CW_ACTOR_ERROR, 
	//	(DWORD)"连接服务器(S)失败,请重新登录.", NEED_RECONNECT_LOGINSERVER);

	//PLAYSOUND(504);
}
//------------------------------------------------------------------------
int	CSceneClientLinkSink::Send(LPCSTR buf, int nLen)
{
	if(m_pClientLink == NULL)
	{
		return 0;
	}

	return m_pClientLink->Send(buf, nLen);
}
//------------------------------------------------------------------------
IClientLink * CSceneClientLinkSink::GetClientLink(void)
{
	return m_pClientLink;
}
//------------------------------------------------------------------------
BOOL CSceneClientLinkSink::OnRecv(DWORD dwSocketID, LPCSTR buf, int nLen)
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
			//恢复连接标置, 否则以后就不能再连了
			m_bConnecting = FALSE;

			SC_ERRORCODE_MSGFORMAT * pError = (SC_ERRORCODE_MSGFORMAT *)(buf);
			//TraceLn(pError->m_szErrorDesc);
			char *pErrDesc = pError->m_szErrorDesc;
			//没返回错误信息,要自己添加  #_#
			if (strlen(pErrDesc) == 0)
			{				
				pErrDesc = ErrDescTable[pError->m_wErrorCode];				
			}

			tPrintEventLn("%s   (%d) ", pErrDesc, pError->m_wErrorCode);

			g_pGlobal->m_pWndMgr->OnEvent(WND_ACTORSELECT, WE_CW_ACTOR_ERROR, 
				(DWORD)pErrDesc, NOTNEED_RECONNECT_LOGINSERVER);
		}
		break;
	case CMD_ROOT_SELECTACTOR:
		{
			if(nLen < sizeof(SC_SELECTACTOR_HEAD))
			{
				tPrintEventLn("SceneClientLink OnRece__CMD_ROOT_SELECTACTOR 数据大小出错");
				return FALSE;
			}

			SC_SELECTACTOR_HEAD * pSelectMsg = (SC_SELECTACTOR_HEAD *)(buf);

			switch(pSelectMsg->m_wKeyMain)
			{		
			case ROOTSELECTACTOR_SC_MAIN_TURNRUN:
				{					
					//恢复连接标置, 否则以后就不能再连了
					m_bConnecting = FALSE;
					tPrintEventLn("SceneClientLink 准备TURNRUN");

					//切换到运行态去
					g_RunClientLinkSink.m_pClientLink = m_pClientLink;
					m_pClientLink->GotoNextState(&g_RunClientLinkSink);
					g_pGlobal->m_pAppNetworkLink = &g_RunClientLinkSink;

					g_RunClientLinkSink.SetGraceful(FALSE);   //关闭平滑关闭模式, 连接断开时会提示出错

					//聊天模块
					g_pGlobal->GetChatClient()->ShowWindow(CW_SHOW);

					//使用第一种操作模式
					g_pGameControl->changeControlMode(1);					
				}
				break;	
			case ROOTSELECTACTOR_SC_MAIN_ENTERCODE:
				{
					//收到ENTERCODE
					tPrintEventLn("CSceneClientLinkSink::OnRecv 收到  ENTERCODE");						
					if (buf == NULL  || nLen != sizeof(SC_SELECTACTOR_ENTERCODE))
					{
						return FALSE;
					}

					SC_SELECTACTOR_ENTERCODE *pEnterCode = (SC_SELECTACTOR_ENTERCODE*)buf;
					SetMacPosition(pEnterCode->m_dwEnterCode);
					__SendMacToServer();
					__SendAccountAndPassword();

				}
				break;
			}
		}
		break;
	default:
		tPrintEventLn("SceneClientLink 收到不明消息, 编号 %d", wMsgRoot);
		break;
	}
	*/
	return TRUE;
}
//------------------------------------------------------------------------
void CSceneClientLinkSink::TryConnect(LPCSTR szIP, int nPort)
{
	if (m_pClientLink == NULL 
		|| m_bConnecting)
	{
		if (m_pClientLink == NULL)
		{
			tPrintEventLn("[CSceneClientLinkSink::TryConnect]: 无法创建连接");
		}
		if (m_bConnecting)
		{
			tPrintEventLn("S Server连接标识已经置位, 连接取消");
		}		
		return;
	}

	//m_bConnecting = TRUE;		
	tPrintEventLn("[Controller]: 开始连接S Server %s: %d", szIP, nPort);
	m_pClientLink->Connect(szIP, nPort);
}
//------------------------------------------------------------------------
int CSceneClientLinkSink::GetType(void) 
{
	return NETWORKLINKTYPE_SCENE;
}
//------------------------------------------------------------------------
void CSceneClientLinkSink::__SendMacToServer(void)
{
	/*
	CS_SELECTACTOR_MAC_HEAD  MacHead(const_cast<char *>(GetMacPackage()));
	char temp[2048];
	CDataArchive DataArchive(temp, 2048);
	DataArchive.Write(&MacHead, sizeof(CS_SELECTACTOR_MAC_HEAD));
	m_pClientLink->Send(temp, DataArchive.GetBufferOffset());
	*/
}
//------------------------------------------------------------------------
void CSceneClientLinkSink::__SendAccountAndPassword(void)
{
	/*
	IQQEnterGame *pQQ = g_pGlobal->GetQQEnterGame();
	if (pQQ == NULL)
	{
		tPrintEventLn("QQ登录模块出错");
		return;
	}

	char szMsgBuf[1024];
	CDataArchive dataarchive(szMsgBuf, sizeof(szMsgBuf));		  

	if (pQQ->IsQQEnter())
	{
		CS_SELECTACTOR_SELECT_HEAD_QQ selecthead(m_lActorID);	
		selecthead.lFirstLogin = 1;
		NETTOOLS::GetMACAddress(selecthead.szSerialNO);
		strcpy(selecthead.m_szQQ, pQQ->GetQQ());
		int nQQDataLen;
		const char *ptQQData = pQQ->GetQQData(nQQDataLen);
		memcpy(selecthead.m_szQQData, ptQQData, nQQDataLen);	
		selecthead.m_nQQDataLen = nQQDataLen;

		MD5 md5;
		char temp[256];
		temp[255] = '\0';
		memcpy(temp, ptQQData, nQQDataLen);
		md5.update((unsigned char *)temp, nQQDataLen);
		char szMAC[20];
		NETTOOLS::GetMACAddress(szMAC);
		md5.update((unsigned char *)szMAC, 17);
		md5.finalize();		
		strncpy(selecthead.m_szQQDataMD5, md5.hex_digest().c_str(), 32);

		strncpy(selecthead.m_szSureCode, GetExtraPassword(), 32);  //附加码

		dataarchive.Write(&selecthead, sizeof(CS_SELECTACTOR_SELECT_HEAD_QQ));	

	}
	else
	{		
		CS_SELECTACTOR_SELECT_HEAD selecthead(m_lActorID);
		selecthead.lFirstLogin = 1;
		NETTOOLS::GetMACAddress(selecthead.szSerialNO);
		strncpy(selecthead.m_szUserName, m_username, USER_NAME_MAX_LEN);
		strncpy(selecthead.m_szPassword, m_password, PASSWORD_MAX_LEN);

		strncpy(selecthead.m_szSureCode, GetExtraPassword(), 32);  //附加码

		dataarchive.Write(&selecthead, sizeof(CS_SELECTACTOR_SELECT_HEAD));		  
	}

	if (m_pClientLink->Send(szMsgBuf, dataarchive.GetBufferOffset()) == FALSE)
	{
		tPrintEventLn("S Server OnConnectOK收到进入码, 但发送数据失败");
	}
	*/
}
//------------------------------------------------------------------------