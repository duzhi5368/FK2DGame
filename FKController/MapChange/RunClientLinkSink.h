/**
*	created:		2012-6-24   22:16
*	filename: 		RunClientLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../FKControllerInclude.h"
#include "../MACSender/MACSender.h"
#include "../ExtraPassword/ExtraPasswordSaver.h"
#include "../../FKGame/Controller/IController.h"
#include "../../FKGame/Network/IClientSocket.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../../FKGame/Network/IAppNetworkLink.h"
#include "../../FKGame/Event/IEvent.h"
#include "../../FKGame/Thread/IThreadRoute.h"
#include "../../FKFileSystem/Include/FKTempl.h"
//------------------------------------------------------------------------
struct stReceiverInfoNode
{
	char*	pData;
	int		nLen;
};
//------------------------------------------------------------------------
class CRunClientLinkSink : public IAppNetworkLink, public IEventSink
{
private:
	typedef list<IDataReceiver *>  RECEIVER_LIST;
	typedef vector<RECEIVER_LIST>  MESSAGE_LIST;

	//信息列表  (不包括CMD_ROOT_THING ,  下面的列表会单独处理)
	MESSAGE_LIST m_MsgList;

	//CMD_ROOT_THING 的信息列表
	MESSAGE_LIST m_ThingMsgList;

	BOOL m_GracefulClose;

	CThreadRouteHelper		m_Thread;
	HANDLE					m_hEventWakeDecode;
	CList<stReceiverInfoNode,stReceiverInfoNode&> m_NodeList;
	CCriticalSectionLock	m_Lock;
	BOOL					m_bThreadRun;

public:
	void	LockReceiver(BOOL bLock);
	void	SetThreadRun(BOOL bRun);

	BOOL	Receiver();
	virtual void			OnEvent(DWORD dwEventID);

	// 连接服务器失败
	// szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
	virtual void			OnConnectError(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode);

	// 连接建立
	virtual void			OnConnectOK(DWORD dwSocketID);
	
	// 收到一个数据包
	// 注：	如果接收数据包的线程与IThreadRoute不是同一线程，
	//		如由一个窗口(ILinkWnd)负责管理通讯，则ILinkWnd必须负责建立一个接收数据包队列
	virtual BOOL			OnRecv(DWORD dwSocketID, LPCSTR buf, int nLen);

	// 当前连接断开
	// szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
	virtual void			OnConnectClose(DWORD dwSocketID, LPCSTR szMsg, DWORD dwErrorCode);

	// 发送数据包
	virtual int				Send(LPCSTR buf, int nLen);

	// 取得IClientLink指针, 使用方法更丰富
	virtual IClientLink *	GetClientLink(void);

	//需要从网络获取数据的模块, 通过此函数注册后,才可收到消息 
	BOOL AddReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode) ;
	
	//移除模块接收者
	BOOL RemoveReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode);

	CRunClientLinkSink(void);
	virtual ~CRunClientLinkSink(void);

	//设为平滑关闭
	void SetGraceful(BOOL flag);

	int GetType(void);

public:	
	IClientLink *	m_pClientLink;
};
//------------------------------------------------------------------------
extern CRunClientLinkSink g_RunClientLinkSink;
//------------------------------------------------------------------------