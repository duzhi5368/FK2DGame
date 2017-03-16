/**
*	created:		2012-6-24   21:12
*	filename: 		MapChangingLinkSink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		跨服务器切换地图时,会调到这里来
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
const int TIMER_EVENT_WAITENTERCODE_EXPIRE = 99;  //计时器事件, 等待进入码
//------------------------------------------------------------------------
class CMapChangingLinkSink : public IAppNetworkLink ,public ITimerSink, public IEventActionListener, protected CMACSender, public CExtraPasswordSaver
{
private:
	struct SServerKey
	{
		char szIP[128];
		WORD wPort;				//服务器开放的端口
	};		 
	SServerKey m_info;			//目标地图所在的服务器
	int m_nRetryCount;			//因为等不到进入码, 进行重连的计数
	int m_nErrorRetryCount;		//因为连接出错, 进行重连的计数
	BOOL m_bReConnectingFlag;	//重连标置
	char m_username[USER_NAME_MAX_LEN + 1];
	char m_password[PASSWORD_MAX_LEN + 1];
	int m_lActorID;
private:
	//收到服务器发下来的READY消息, 这时会连到新服务器
	void __ConnectToTargetMap(void);
	//定义人物指针, 仅用于监听下线消息
	DWORD m_dwpActor;
	//发送帐号和密码到服务器
	void __SendAccountAndPassword(void);
	//将MAC发送到服务器
	void __SendMacToServer(void);
public:
	CMapChangingLinkSink();
	virtual ~CMapChangingLinkSink();

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
	//计时器回调
	void	OnTimer(DWORD dwEventID);
	//设置目标地图所在的服务器
	void    SetServer(const char *pData, int nLen);
	virtual void OnAction(IEvent *pEvent);
	//注册监视下线消息 
	void RegisterLogonMsg(void);
	int GetType(void);
public:	
	IClientLink *	m_pClientLink;
};
//------------------------------------------------------------------------
extern CMapChangingLinkSink g_MapChangingLinkSink;
//------------------------------------------------------------------------