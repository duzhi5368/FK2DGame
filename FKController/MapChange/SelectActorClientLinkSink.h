/**
*	created:		2012-6-24   21:32
*	filename: 		SelectActorClientLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		在登录状态，处理网络事情
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
//------------------------------------------------------------------------
class CSelectActorClientLinkSink : public IAppNetworkLink
{
private:
	int m_lActorID;

public:
	//以选定的人物开始游戏
	BOOL startGame(int lActorID);
	//往服务器发信息,这个方法封装了send方法
	BOOL SendMsg(WORD wMain, LPCSTR buf, int nLen);
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
	int GetType(void);
public:
	CSelectActorClientLinkSink(void);
public:	
	IClientLink *	m_pClientLink;
};
//------------------------------------------------------------------------
extern CSelectActorClientLinkSink g_SelectActorClientLinkSink;
//------------------------------------------------------------------------