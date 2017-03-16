/**
*	created:		2012-6-24   21:25
*	filename: 		SceneClientLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		连接到场景服时的通信接收态
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
class CSceneClientLinkSink : public IAppNetworkLink, protected CMACSender, public CExtraPasswordSaver
{
private:
	char m_username[USER_NAME_MAX_LEN + 1];
	char m_password[PASSWORD_MAX_LEN + 1];
	int m_lActorID;

	//标记是否正在连接, 如果正在连接的话,则不会再向服务器发出请求
	//以降低服务器的负荷
	BOOL m_bConnecting;   

	//发送帐号和密码到服务器
	void __SendAccountAndPassword(void);
	
	//将MAC发送到服务器
	void __SendMacToServer(void);

public:
	CSceneClientLinkSink();
	virtual ~CSceneClientLinkSink();


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

	//设置帐号和密码
	void setData(char *user, char *password);

	//设置进入游戏的角色ID
	void setActorID(int ActorID);

	//尝试去连接某个地址, 如果此时已经有连接,则没有反应
	void TryConnect(LPCSTR szIP, int nPort);

	virtual int GetType(void) ;

public:	
	IClientLink *	m_pClientLink;


};
//------------------------------------------------------------------------
extern CSceneClientLinkSink g_SceneClientLinkSink;
//------------------------------------------------------------------------
