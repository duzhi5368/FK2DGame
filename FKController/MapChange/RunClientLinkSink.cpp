/**
*	created:		2012-6-24   22:22
*	filename: 		RunClientLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "RunClientLinkSink.h"
//------------------------------------------------------------------------
CRunClientLinkSink g_RunClientLinkSink;
//------------------------------------------------------------------------
LPCSTR  szRunErrDesc = "与服务器(S)连接断开, 请重新登录游戏! 如情况再度出现,请与客服联系!";
//------------------------------------------------------------------------
CRunClientLinkSink::CRunClientLinkSink(void)
		//:m_MsgList(CMD_ROOT_MAX), m_ThingMsgList(MAX_ROOTTHING_MAIN_KEY)
{
	m_bThreadRun = FALSE;
	m_pClientLink = NULL;
	m_GracefulClose = FALSE;
	m_hEventWakeDecode = NULL;
	
	m_Thread.Close();
	if(!m_Thread.Create(NULL))
	{
		return;
	}

	DWORD dwEventID;
	if(!m_Thread.m_pThreadRoute->AddEvent(static_cast<IEventSink*>(this),dwEventID))
		return;

	m_hEventWakeDecode = m_Thread.m_pThreadRoute->GetEventHandle(dwEventID);

	if(!m_Thread.m_pThreadRoute->Start())
	{
		m_Thread.Close();
		return;
	}

	return;
}
//------------------------------------------------------------------------
CRunClientLinkSink::~CRunClientLinkSink(void)
{
	m_Thread.Close();
}
//------------------------------------------------------------------------
// 描  述：设置关闭标置, 如果关闭标置为TRUE, 则断开连接时不会弹出错误信息
void CRunClientLinkSink::SetGraceful(BOOL flag)
{
	m_GracefulClose = flag;
}
//------------------------------------------------------------------------
// 描  述：连接服务器失败
// 参  数：szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
void CRunClientLinkSink::OnConnectError(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	//char szTemp[256];
	//sprintf(szTemp, "CRunClientLinkSink::OnConnectError, 与服务器通信发生错误。ErrorCode：%d", dwErrorCode);
	//tPrintEventLn(szTemp);
	//
	//g_pGlobal->m_pWndMgr->CustomMsgBox(szRunErrDesc);
	//g_pGlobal->GetSoundEngine()->PlaySound(504);
}
//------------------------------------------------------------------------
// 描  述：连接建立
void CRunClientLinkSink::OnConnectOK(DWORD dwSocketID)
{
	//char szTemp[256];
	//sprintf(szTemp, "连接服务器成功。[SockID = %d]", dwSocketID);
	//MessageBox(NULL, szTemp, "网络成功", MB_OK);
}
//------------------------------------------------------------------------
// 描  述：当前连接断开
// 参  数：szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
void CRunClientLinkSink::OnConnectClose(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode)
{
	//if (!m_GracefulClose)
	//{
	//	char szTemp[256];
	//	sprintf(szTemp, "CRunClientLinkSink::OnConnectClose, 与服务器的连接关闭。ErrorCode：%d", dwErrorCode);
	//	tPrintEventLn(szTemp);
	//	g_pGlobal->m_pWndMgr->CustomMsgBox(szRunErrDesc);
	//	g_pGlobal->GetSoundEngine()->PlaySound(504);
	//}
}
//------------------------------------------------------------------------
// 描  述：发送数据包
int	CRunClientLinkSink::Send(LPCSTR buf, int nLen)
{
	if(m_pClientLink == NULL)
	{
		return 0;
	}

	return m_pClientLink->Send(buf, nLen);
}
//------------------------------------------------------------------------
// 描  述：取得IClientLink指针, 使用方法更丰富
IClientLink * CRunClientLinkSink::GetClientLink(void)
{
	return m_pClientLink;
}
//------------------------------------------------------------------------
//需要从网络获取数据的模块 , 通过此函数注册后,才可收到消息 
BOOL CRunClientLinkSink::AddReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode)
{
	////先确定对CMD_ROOT_THING 处理,还是对普通命令处理,将要处理的表确定好
	//MESSAGE_LIST  *pList  = NULL;
	//int           MsgCode = 0;
	//if (MainMsgCode == CMD_ROOT_THING)
	//{
	//	pList   = &m_ThingMsgList;
	//	MsgCode = SubMsgCode;
	//}
	//else
	//{
	//	pList = &m_MsgList;
	//	MsgCode = MainMsgCode;
	//}


	////在表中添加新注册者
	//RECEIVER_LIST::iterator itor = find((*pList)[MsgCode].begin(), 
	//	(*pList)[MsgCode].end(), pReceiver);
	//
	//if (itor != (*pList)[MsgCode].end())
	//{//接收者已经注册了
	//	return FALSE;
	//}

	//(*pList)[MsgCode].push_back(pReceiver);
	return TRUE;
}
//------------------------------------------------------------------------
//移除模块接收者
BOOL CRunClientLinkSink::RemoveReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode) 
{
	////先确定对CMD_ROOT_THING 处理,还是对普通命令处理,将要处理的表确定好
	//MESSAGE_LIST  *pList  = NULL;
	//int           MsgCode = 0;
	//if (MainMsgCode == CMD_ROOT_THING)
	//{
	//	pList   = &m_ThingMsgList;
	//	MsgCode = SubMsgCode;
	//}
	//else
	//{
	//	pList = &m_MsgList;
	//	MsgCode = MainMsgCode;
	//}
	//

	//(*pList)[MsgCode].remove(pReceiver);
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：收到一个数据包
// 参  数：如果接收数据包的线程与IThreadRoute不是同一线程，
// 返回值：如由一个窗口(ILinkWnd)负责管理通讯，则ILinkWnd必须负责建立一个接收数据包队列
BOOL CRunClientLinkSink::OnRecv(DWORD dwSocketID, LPCSTR buf, int nLen)
{
	if(nLen < sizeof(WORD))
	{
		return FALSE;
	}
	
	stReceiverInfoNode node;
	node.pData = new char[nLen];
	memcpy(node.pData, buf, nLen);
	node.nLen = nLen;

	m_NodeList.AddTail(node);
	
	if (!m_bThreadRun)
	{
		while(m_NodeList.GetCount() != 0)
			Receiver();
		return TRUE;
	}

	SetEvent(m_hEventWakeDecode);

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CRunClientLinkSink::Receiver()
{
	/*
	int nCount = m_NodeList.GetCount();
	if(nCount == 0)
	{
		return FALSE;
	}

	stReceiverInfoNode node = m_NodeList.RemoveHead();
	LPCSTR buf = node.pData;
	
	WORD wMsgRoot = *((WORD *)(buf));
	int hsize = sizeof(WORD);         //消息码的长度

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
			g_pGlobal->m_pWndMgr->CustomMsgBox(pErrDesc);
		}
		break;
	case CMD_ROOT_THING:
		{
			hsize *= 2;   //THING有两个消息码,所以这里要乘2
			WORD wMsgSub = *((WORD *)(buf) + 1);
			if (wMsgSub >= MAX_ROOTTHING_MAIN_KEY)
			{
				break;
			}
			
			for (RECEIVER_LIST::iterator itor = m_ThingMsgList[wMsgSub].begin();
			itor != m_ThingMsgList[wMsgSub].end(); ++itor)
			{
				(*itor)->OnReceived(buf + hsize, node.nLen - hsize);
			}
		}
		 
		break;
	default:
		{
			if (wMsgRoot >= CMD_ROOT_MAX)
			{
				break;
			}
			for (RECEIVER_LIST::iterator itor = m_MsgList[wMsgRoot].begin();
			itor != m_MsgList[wMsgRoot].end(); ++itor)
			{
				(*itor)->OnReceived(buf + hsize, node.nLen - hsize);
			} 
		}
		break;
	}

	delete[] node.pData;
	*/
	return TRUE;
}
//------------------------------------------------------------------------
void CRunClientLinkSink::OnEvent(DWORD dwEventID)
{
	m_Lock.Lock();

	if(Receiver())
		SetEvent(m_hEventWakeDecode);

	m_Lock.Unlock();

	return;
}
//------------------------------------------------------------------------
void CRunClientLinkSink::LockReceiver(BOOL bLock)
{
	if(bLock)
	{
		m_Lock.Lock();		
		if (!m_bThreadRun)
		{
			while(m_NodeList.GetCount() != 0)
				Receiver();
		}
	}
	else
		m_Lock.Unlock();
}
//------------------------------------------------------------------------
void CRunClientLinkSink::SetThreadRun(BOOL bRun)
{
	m_bThreadRun = bRun;
}
//------------------------------------------------------------------------
int CRunClientLinkSink::GetType(void)
{
	return NETWORKLINKTYPE_RUNNING;
}
//------------------------------------------------------------------------