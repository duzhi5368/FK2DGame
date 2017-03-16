/**
*	created:		2012-6-24   21:38
*	filename: 		LoginClientLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "LoginClientLinkSink.h"
//------------------------------------------------------------------------
CLoginClientLinkSink g_LoginClientLinkSink;
//------------------------------------------------------------------------
LPCSTR szLoginErrDesc = "无法连接服务器(L), 请稍候再登录! 如果情况仍没改善, 请致电客服!";
//------------------------------------------------------------------------
void CLoginClientLinkSink::setData(char *user, char *password)
{
	strncpy(m_username, user, USER_NAME_MAX_LEN + 1)  ;
	if (password)  //如果不是第一次登录, 密码为空, 仍用上一次的密码就可以了
	{
		strncpy(m_password, password, PASSWORD_MAX_LEN);  //只传32个字符
		m_password[PASSWORD_MAX_LEN] = '\0';
	}	
}
//------------------------------------------------------------------------
CLoginClientLinkSink::CLoginClientLinkSink(void)
{
	m_pClientLink = NULL;
	m_username[0] = '\0';
	m_password[0] = '\0';

}
//------------------------------------------------------------------------
// 描  述：连接服务器失败
// 参  数：szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
void CLoginClientLinkSink::OnConnectError(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	//char szTemp[256];
	//sprintf(szTemp, "连接LOGIN服务器失败。[SockID = %d， Msg：%s，ErrorCode：%d]", dwSocketID, szMsg, dwErrorCode);
	//tPrintEventLn(szTemp);
	//g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_ERRORINFO, 
	//	(DWORD)szLoginErrDesc, 0);	

	//g_pGlobal->GetSoundEngine()->PlaySound(504);
}
//------------------------------------------------------------------------
// 描  述：连接建立
void CLoginClientLinkSink::OnConnectOK(DWORD dwSocketID)
{	
	/*
	//已经成功连接, 通知登录窗口清掉密码
	g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_CLEARPASSWORD, 0, 0);
	
	//版本号如果无效, 马上登录, 如果有效, 则向服务器取版本号,对比完后再登录
	if (g_DllVersion.isValid())
	{
		SendMsg(ROOTLOGIN_CS_MAIN_CLIENTVER, NULL, 0);
	}
	else
	{
		g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_ERRORINFO, 
			(DWORD)"无法获取游戏版本信息, 请运行自动更新.如果情况依旧,请与客服联系!", 0);	
	}
	*/
}
//------------------------------------------------------------------------
void CLoginClientLinkSink::__SendMACAccountAndPassword(void)
{	
	/*
	IQQEnterGame *pQQ = g_pGlobal->GetQQEnterGame();
	if (pQQ == NULL)
	{
		tPrintEventLn("QQ登录模块出错");
		return;
	}
 
	if (pQQ->IsQQEnter())
	{ 
		SSUserLoginContext_QQ Context;
		strcpy(Context.szUserName, pQQ->GetQQ());
		//取QQ数据
		int nQQDataLen;
		const char *ptQQData = pQQ->GetQQData(nQQDataLen);
		memcpy(Context.szQQData, ptQQData, nQQDataLen);	
		Context.nQQDataLen  = nQQDataLen;

		memcpy(Context.szNetworkCard, GetMacPackage(), 512);	
		char temp[256];
		memcpy(temp, ptQQData, nQQDataLen);
		MD5 md5;		
		md5.update((unsigned char *)temp, nQQDataLen);
		char szMAC[20];
		NETTOOLS::GetMACAddress(szMAC);
		md5.update((unsigned char *)szMAC, 17);
		md5.finalize();
		strncpy(Context.szQQDataMD5, md5.hex_digest().c_str(), 32);
		Context.lPassPodPwd = atol(g_PassportMgr.GetPassport());

		strncpy(Context.szMD5SureCode, GetExtraPassword(), 32);  //附加码

		SendMsg(ROOTLOGIN_CS_MAIN_USERLOGIN_QQ, (LPCSTR)&Context, sizeof(SSUserLoginContext_QQ));		
		 
	}
	else
	{
		SSUserLoginContext Context;	
		strcpy(Context.szUserName, m_username);
		memcpy(Context.szPassword, m_password, 32);		
		memcpy(Context.szNetworkCard, GetMacPackage(), 512);		
		Context.lPassPodPwd = atol(g_PassportMgr.GetPassport());
		
		strncpy(Context.szMD5SureCode, GetExtraPassword(), 32);  //附加码		
		
		SendMsg(ROOTLOGIN_CS_MAIN_USERLOGIN, (LPCSTR)&Context, sizeof(SSUserLoginContext));
	}
	*/
}
//------------------------------------------------------------------------
BOOL CLoginClientLinkSink::SendMsg(WORD wMain, LPCSTR buf, int nLen)
{
	/*
	if (g_LoginClientLinkSink.m_pClientLink == NULL)
	{
		return FALSE;
	}
	
	CS_LOGIN_HEAD LoginHead(wMain);
	
	char temp[2048];
	CDataArchive DataArchive(temp, 2048);
	DataArchive.Write(&LoginHead, sizeof(CS_LOGIN_HEAD));
	DataArchive.Write(buf, nLen);
	
	return g_LoginClientLinkSink.Send(temp, DataArchive.GetBufferOffset());	
	*/

	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：当前连接断开
// 参  数：szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
void CLoginClientLinkSink::OnConnectClose(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	//char szTemp[256];
	//sprintf(szTemp, "连接服务器失败。[SockID = %d， Msg：%s，ErrorCode：%d]", dwSocketID, szMsg, dwErrorCode);
	//tPrintEventLn(szTemp);
	//g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_ERRORINFO, 
	//	(DWORD)szLoginErrDesc, 0);	

	//g_pGlobal->GetSoundEngine()->PlaySound(504);
}
//------------------------------------------------------------------------
// 描  述：发送数据包
int	CLoginClientLinkSink::Send(LPCSTR buf, int nLen)
{
	if(m_pClientLink == NULL)
	{
		return 0;
	}

	return m_pClientLink->Send(buf, nLen);
}
//------------------------------------------------------------------------
// 描  述：取得IClientLink指针, 使用方法更丰富
IClientLink * CLoginClientLinkSink::GetClientLink(void)
{
	return m_pClientLink;
}
//------------------------------------------------------------------------
// 描  述：收到一个数据包
// 参  数：如果接收数据包的线程与IThreadRoute不是同一线程，
// 返回值：如由一个窗口(ILinkWnd)负责管理通讯，则ILinkWnd必须负责建立一个接收数据包队列
BOOL CLoginClientLinkSink::OnRecv(DWORD dwSocketID, LPCSTR buf, int nLen)
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
			SC_ERRORCODE_MSGFORMAT * pError = (SC_ERRORCODE_MSGFORMAT *)(buf);
			
			//TraceLn(pError->m_szErrorDesc);
			char *pErrDesc = pError->m_szErrorDesc;
			//没返回错误信息,要自己添加  #_#
			if (strlen(pErrDesc) == 0)
			{				
				pErrDesc = ErrDescTable[pError->m_wErrorCode];				
			}
			
			tPrintEventLn("%s   (%d) ", pErrDesc, pError->m_wErrorCode);

			if (g_pGlobal->GetQQEnterGame()->IsQQEnter())
			{
				//删掉"QQ登录中"的窗口
				g_pGlobal->m_pWndMgr->OnEvent(WND_CUSTOMMSGBOX, WE_AW_MSGBOX_CLEARONCE, MAKECOURSE(POPCOURSE_QQLOGIN, 0), 0);
				g_pGlobal->m_pWndMgr->CustomMsgBox(pErrDesc, 
					1, 0, 0, new CQQLoginErrorCallBack);				
			}
			else
			{
				g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_ERRORINFO, 
					(DWORD)pErrDesc, 0);
			}

						
		}
		break;

	case CMD_ROOT_LOGIN:
		{
			buf += sizeof(WORD);
			nLen -= sizeof(WORD);
			
			WORD wMain = *(WORD*)buf;
			buf += sizeof(WORD);
			nLen -= sizeof(WORD);
			
			switch(wMain) {
			case ROOTLOGIN_SC_MAIN_TURNSELECT:
				{						
					//将网络数据的收发任务转换到 "角色选择" 状态
					g_SelectActorClientLinkSink.m_pClientLink = m_pClientLink;
					m_pClientLink->GotoNextState(&g_SelectActorClientLinkSink);
					g_pGlobal->m_pAppNetworkLink = &g_SelectActorClientLinkSink;

					//窗口显示调整
					g_pGlobal->m_pWndMgr->CreateWnd(WND_ACTORSELECT_BAK);
					g_pGlobal->m_pWndMgr->CreateWnd(WND_ACTORSELECT);					
					g_ActorWndController.Create();
					g_ActorWndController.refreshActor(buf, nLen);
					g_pGlobal->m_pWndMgr->ShowWnd(WND_LOGIN, SW_HIDE);
					g_pGlobal->m_pWndMgr->ShowWnd(WND_LOGIN_BAK, SW_HIDE);
					//g_pGlobal->m_pWndMgr->ShowWnd(WND_LOGIN_BAK_NOTICE, SW_HIDE);
					g_pGlobal->m_pWndMgr->ShowWnd(WND_ACTORSELECT_BAK, SW_SHOW);
					g_pGlobal->m_pWndMgr->ShowWnd(WND_ACTORSELECT, SW_SHOW);					
					
					//登录窗口可能因为排除被锁定,所以这里解除一下
					g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_UNLOCK, 0, 0);					
					g_pGlobal->m_pWndMgr->OnEvent(WND_CUSTOMMSGBOX, WE_CW_MSGBOX_CLEAR, 0, 0);
					
					break; 
				}
			case ROOTLOGIN_SC_MAIN_LISTNUM:
				{
					if (buf != NULL && nLen == sizeof(SSListLocationContext))
					{
						g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_LOCK, 0, 0);//锁定登录窗口,防止用户输入
						SSListLocationContext *pContext = (SSListLocationContext*)buf;
						g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_WAIT, pContext->nLocation + 1, 0);
					}
					
				}
				break;
 
			case ROOTLOGIN_SC_MAIN_CLIENTVER:
				{					
					if (buf == NULL)
					{
						return FALSE;
					}

					if (!g_DllVersion.isVersionPass(buf + 4, nLen - 4))  //这时留起4字节随机数
					{
						g_pGlobal->m_pWndMgr->OnEvent(WND_LOGIN, WE_CW_LOGIN_ERRORINFO, 
							(DWORD)"游戏版本过旧,请及时更新! 如更新后情况依旧, 请联系客服!", 0);
						return TRUE;
					}						
					
					DWORD dwMacPos;
					memcpy(&dwMacPos, buf, 4);
					SetMacPosition(dwMacPos);
					__SendMACAccountAndPassword();     //发送密码帐号MAC
					break;
				}

			default:
				break;
			}
		}
		break;
	default:
		{
			tPrintEventLn("CLoginClientLinkSink::OnRecv收到不明消息 %d", wMsgRoot);
		}
		break;
	}
	*/
	return TRUE;
}
//------------------------------------------------------------------------
int CLoginClientLinkSink::GetType(void)
{
	return NETWORKLINKTYPE_LOGIN;
}
//------------------------------------------------------------------------