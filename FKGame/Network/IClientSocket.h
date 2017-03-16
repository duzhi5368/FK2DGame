/**
*	created:		2012-6-22   0:20
*	filename: 		IClientSocket
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifndef MAX_NAME_STRING_LEN
#define MAX_NAME_STRING_LEN			32								// 用户名和密码、代理服务器名需用到这个宏
#endif
//------------------------------------------------------------------------
#define VERSION_SingleSocketProxy	1
#define VERSION_CLIENTLINK			2
//------------------------------------------------------------------------
// 代理服务器设置相关定义
enum PROXY_TYPE
{
	proxyNone = 0, proxy4 = 4,proxy5 = 5
};
//------------------------------------------------------------------------
struct NET_PROXY_OPTION 
{
	NET_PROXY_OPTION ()
	{
		Close();
	}
	void Close()
	{
		memset(this,0,sizeof(NET_PROXY_OPTION )) ;
	}
	BOOL		bEnableProxy;								// 是否使用代理服务器
	PROXY_TYPE	socksType;									// 代理服务器类型，默认为不支持代理服务器
	char		szProxyServerName[MAX_NAME_STRING_LEN];		// 代理服务器名
	WORD		wProxyPort;									// 代理服务器连接端口号，如1080
	WORD		wReserved;
	BOOL		bCheckAuthority;							// 代理服务器是否需要身份验证
	char		szProxyUserName[MAX_NAME_STRING_LEN];		// 如果需要验证，需要设置用设置用户名和密码
	char		szProxyPassword[MAX_NAME_STRING_LEN];
};
//------------------------------------------------------------------------
struct IEncryptData;
struct IClientSocket;
struct IClientSocketSink;
struct IEventSink;
struct IThreadRoute;
struct ITrace;
//------------------------------------------------------------------------
struct IClientSocket
{
	// 当前通道标识
	virtual DWORD GetSocketID() = NULL;

	// 发起连接，连接结果：当收到OnConnectOK()时，表示连接成功，收到OnConnectError()时表示连接失败
	// 返回：用户可忽略返回结果，但参数错时也会返回FALSE
	virtual BOOL Connect(LPCSTR szServerAddress, WORD wServerPort,NET_PROXY_OPTION* pProxyOption = NULL) = NULL;

	// 关闭此连接，下次可以再次调用Connect()发起连接
	// 不会回调IClientSocketSink::OnConnectClose()
	virtual BOOL CloseConnect() = NULL;

	/*
	 功能：发送数据包
	 输入：nLen不能超过MAX_SOCKET_RAW_DATA_LEN,buf不能为空
	 返回：发送错或参数错，返回FALSE
		   如果发送成功：返回TRUE或WSA_IO_INCOMPLETE
				WSA_IO_INCOMPLETE表示数据发送成功，但没有立即发送出去
				这在文件传送等大量数据传送时很有用，应用层可以根据这个返回值决定延迟再发
			    WSA_IO_INCOMPLETE是Overlapped sockets的WSASend()返回WSAEWOULDBLOCK消息时返回的
				语意上等同于WSA_IO_INCOMPLETE,因为IClientSocket会等待时机再次发送
	 说明：1、发送算法为先立即发送，数据发不通时才将数据放入发送队列，返回WSA_IO_INCOMPLETE，等待下次发送
		   2、如果socket发送失败，系统将在OnConnectClose中返回出错信息，因此不必在发送错误作出出错处理
	*/
	virtual int Send(LPCSTR buf,int nLen) = NULL;

	// 功能：当收到OnDataArrived()通知时，才能调用此接口接收数据
	// 输入：nBufLen为buf大小
	// 返回：buf中含有长度为nLen的数据，如果返回TRUE，但nLen为0，表示暂时无数据返回
	virtual BOOL Recv(int nMaxBufLen,/*out*/char* buf,/*out*/int& nLen) = NULL;

	// 查询当前是否在连接状态
	virtual BOOL IsConnected() = NULL;

	/* 切换到下一个状态
	   如：
		  应用层的登录态和运行态由两个类来实现，都实现IClientSocketSink
		  当登录完成以后，调用此函数从登录态切换到运行态
	*/
	virtual void GotoNextState(IClientSocketSink* pSink) = NULL;


	// 将来可以获取或设置一些内部数据，如当前发送队列长度、收发速度、当前socket是否阻塞等。暂不支持此功能。
	enum 
	{
		setMaxSendQueueCount	= 1,		// 发送缓冲队列最大长度设置，默认为10000，dwParam为新的长度，pData返回当前长度
		getMaxSendQueueCount	= 2,
		getSocketHandle			= 3,		// 获取socket句柄，以便直接对socket进行操作，比如setsockopt()
	};
	/*
		例：为发送量很大的应用设置一个更大的发送队列：ioctrl(IClientSocket::setMaxSendQueueCount,1000000,NULL);
	*/
	virtual BOOL ioctrl(DWORD dwCmd,DWORD dwParam,/*out*/void* pData) = NULL;

	// 释放对象
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
/*
 * 说明：
 *			因为IClientSocketSink在IThreadRoute中被回调，所以:
 *			对于服务器端程序，接收器IClientSocketSink可以在服务器逻辑里实现，此时接收器
 *		跟主程序在同一线程，不必考虑线程同步问题
 *			但对于客户端程序，通讯层和主程序不在同一线程，需要考虑线程同步问题
 *
 */
struct IClientSocketSink
{
	// 连接服务器失败
	// szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
	virtual void OnConnectError(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode) = NULL;

	// 连接建立
	virtual void OnConnectOK(DWORD dwSocketID) = NULL;
	
	// 有数据可以接收
	// 说明：此时底层调用m_pClientSocket->Recv()从系统socket层接收数据
	// 注：	如果接收数据包的线程与IThreadRoute不是同一线程，
	//		如由一个窗口(ILinkWnd)负责管理通讯，则ILinkWnd必须负责建立一个接收数据包队列
	virtual void OnDataArrived(DWORD dwSocketID) = NULL;

	/* 当前连接断开
	   szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
	   注意：
			1、应用层需在对这个事件处理完以后执行IClientSocket::CloseConnect()清理连接资源
	  		   这样做是因为：为保证应用层在连接断开时能接收到残余数据，IClientSocket并没有清理连接资源
			   这在用HWND同步的线程模式下尤其重要，因为PostMessage()发送的消息到达有时会非常慢
			2、CreateSingleSocketProxy()创建的IClientSocket已经帮助用户完成CloseConnect()操作
	*/
	virtual void OnConnectClose(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode) = NULL;
};
//------------------------------------------------------------------------
/*
 *  XXXXXXXXX此接口已经改为IKeyStream，不久将删除之XXXXXXXXXXXXXXXX
 *
 * 功能：将数据流加密或解密，同时具有包管理能力
 * 说明：1、通过接口的方式实现允许了通过多种不同的加密手段进行加密
 *		 2、当一个数据包未到齐时，IEncryptData缓冲当前数据，等待数据包的到来一齐返回给用户，
 *			所以nMaxDataBufLen最好取MAX_SOCKET_RAW_DATA_LEN
 */
struct IEncryptData
{	
	// 功能：初始化加密层，设置发送和接收初始密钥
	// 说明：增加Init()接口是为了允许用户在连接断开以后重新设置通讯包初始密钥
	//
	virtual BOOL Init(DWORD dwInitKeyData,DWORD dwInitUnKeyData,DWORD dwParam) = NULL;

	// 功能：加密数据
	// 输入：nLen不能超过32*1024,nMaxDataBufLen为szData的长度，加密后的长度不得超过此nMaxDataBufLen
	// 返回：加密后的数据放在szData中，nLen为加密后的数据长度，
	virtual BOOL KeyData(/*in out*/char* szData, /*in out*/int& nLen,int nMaxDataBufLen) = NULL;

	// 功能：解密数据
	// 输入：nLen不能超过16*1024,nMaxDataBufLen为szData的长度，加密后的长度不得超过此长度
	// 返回：加密后的数据放在szData中，nLen为加密后的数据长度，
	virtual BOOL UnKeyData(/*in out*/char* szData, /*in out*/int& nLen,int nMaxDataBufLen) = NULL;
};
//------------------------------------------------------------------------
/*
*	功能：
*		管理多个连接，支持多线程同步支持(基于HWND)和单线程高速数据处理
*/
struct ILinkProxy
{
	virtual BOOL AddSocket(DWORD dwSocketID,IClientSocketSink *pSink,IClientSocket** ppClientSocket) = NULL;
	virtual BOOL RemoveSocket(IClientSocket* pClientSocket) = NULL;
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
extern "C" BOOL CreateLinkProxyWnd(IThreadRoute *pThreadRoute,ITrace* pTrace,BOOL bSyncByWnd,BOOL bInSameThread,/*out*/ILinkProxy** ppLinkProxy);
// 输入：
//		dwSocketID	:	相当于SetTimer()中的dwTimerID，用户用此ID标识当前连接
//						这样就允许用户用一个IClientSocketSink管理多个连接，一般取用户的连接的索引即可
//						可以通过IClientSocket::GetSocketID()读取创建时的dwSocketID
//		bInSameThread : 主线程和通讯线程是否在同一线程，高性能应用最好两者在同一线程，从而避免使用用互斥锁
extern "C"  BOOL CreateClientSocket(DWORD dwSocketID,IClientSocketSink *pSink,IThreadRoute *pThreadRoute, ITrace* pTrace,BOOL bInSameThread,/*out*/IClientSocket** ppClientSocket);

// 创建一个连接，由一个线程管理一个连接，适合于连接很少，性能要求不高的情况，如有窗口的客户端程序通讯需求
extern "C" BOOL CreateSingleSocketProxy(DWORD dwSocketID,IClientSocketSink* pSink,ITrace* pTrace,/*out*/IClientSocket** ppClientSocket);
//------------------------------------------------------------------------
class CSingleSocketProxyHelper
{
public:
	typedef BOOL (*ProcCreateSingleSocketProxy)(DWORD dwVersion,DWORD dwSocketID,IClientSocketSink* pSink,ITrace* pTrace,/*ou*/IClientSocket** ppClientSocket);
	CSingleSocketProxyHelper()
	{
		m_hdll = NULL;
		m_pClientSocket = NULL;
	}
	~CSingleSocketProxyHelper()
	{
		Close();
	}
	void Close()
	{
		if(m_pClientSocket)
		{
			m_pClientSocket->Release();
			m_pClientSocket = NULL;
		}
		if(m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}
	BOOL Create(DWORD dwSocketID,IClientSocketSink* pSink,ITrace* pTrace)
	{
		Close();
		try
		{
#if defined(_DEBUG)
			m_hdll = ::LoadLibrary("SocketD.dll");
			if(m_hdll == NULL)
				throw "can not load SocketD.dll";
#else
			m_hdll = ::LoadLibrary("Socket.dll");
			if(m_hdll == NULL)
				throw "can not load Socket.dll";
#endif
			ProcCreateSingleSocketProxy proc = (ProcCreateSingleSocketProxy)GetProcAddress(m_hdll, "dllCreateSingleSocketProxy");
			if(proc == NULL)
				throw "Can not GetProcAddress('dllCreateSingleSocketProxy')!";
			if(!proc(VERSION_SingleSocketProxy,dwSocketID,pSink,pTrace,&m_pClientSocket))
				throw "CreateSingleSocketProxy() failed";
			return m_pClientSocket != NULL;

		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return FALSE;
		}
		catch(...)
		{
			OutputDebugString("Error : CSingleSocketProxyHelper::Create() : catch error!\r\n");
			return FALSE;
		}
		return TRUE;
	}

public:
	IClientSocket* m_pClientSocket;
private:
	HINSTANCE		m_hdll;
};
//------------------------------------------------------------------------
// 带封包和登录的连接管理
/* 连接事件接收器
 说明：
		1、单连接可以忽略所有的dwSocketID值
		2、如果有登录过程，建议采用状态模式。
			即登录态和运行态都实现IClientLinkSink接口，
			当登录完成以后调用IClientLink::GotoNextState(IClientLinkSink* pSink)实现状态切换
 */
struct IClientLinkSink
{
	// 连接服务器失败
	// szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
	virtual void OnConnectError(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode) = NULL;

	// 连接建立
	virtual void OnConnectOK(DWORD dwSocketID) = NULL;
	
	// 收到一个数据包
	// 注：	如果接收数据包的线程与IThreadRoute不是同一线程，
	//		如由一个窗口(ILinkWnd)负责管理通讯，则ILinkWnd必须负责建立一个接收数据包队列
	virtual BOOL OnRecv(DWORD dwSocketID,LPCSTR buf,int nLen) = NULL;

	// 当前连接断开
	// szMsg为错误信息具体内容。dwErrorCode为socket错误代码，参见Win32 API : GetLastError()
	virtual void OnConnectClose(DWORD dwSocketID,LPCSTR szMsg,DWORD dwErrorCode) = NULL;
};
//------------------------------------------------------------------------
// 支持登录过程、包和加密管理的连接
struct IClientLink
{
	// 当前通道标识
	// 单连接可以忽略dwSocketID值
	virtual DWORD GetSocketID() = NULL;

	/*
		发起连接并登录服务器

		参数：
			szServerAddress和wServerPort:
				服务器地址和端口
			pProxyOption:
				代理服务器参数，如果为NULL，表示不支持代理服务器

		返回：
			用户可忽略返回结果，但参数错时也会返回FALSE
			成功：TRUE
			失败：FALSE
				如果连接失败，IClientLinkSink::OnConnectError()将被回调
				如果连接成功，但在等待登录结果时连接断开，IClientLinkSink::OnConnectClose()将被调用
	*/
	virtual BOOL Connect(LPCSTR szServerAddress, WORD wServerPort,NET_PROXY_OPTION* pProxyOption = NULL) = NULL;

	/* 切换到下一个状态
	   如：
		  应用层的登录态和运行态由两个类来实现，都实现IClientLinkSink
		  当登录完成以后，调用此函数从登录态切换到运行态
	*/
	virtual void GotoNextState(IClientLinkSink* pSink) = NULL;

	// 关闭此连接，下次可以再次调用Connect()发起连接，IClientLinkSink::OnLoginResult()不会被调用
	// 不会回调IClientSocketSink::OnConnectClose()
	virtual BOOL CloseConnect() = NULL;

	virtual BOOL GetCallList( void ** pList , int & nLen , int nVer ) = NULL;
	
	/*
	 功能：发送数据包
	 输入：nLen不能超过16 x 1024,buf不能为空
	 返回：发送错或参数错，返回FALSE
		   如果发送成功：返回TRUE或WSA_IO_INCOMPLETE
				WSA_IO_INCOMPLETE表示数据发送成功，但没有立即发送出去
				这在文件传送等大量数据传送时很有用，应用层可以根据这个返回值决定延迟再发
			    WSA_IO_INCOMPLETE是Overlapped sockets的WSASend()返回WSAEWOULDBLOCK消息时返回的
				语意上等同于WSA_IO_INCOMPLETE,因为IClientSocket会等待时机再次发送
	 说明：1、发送算法为先立即发送，数据发不通时才将数据放入发送队列，返回WSA_IO_INCOMPLETE，等待下次发送
		   2、如果socket发送失败，系统将在OnConnectClose中返回出错信息，因此不必在发送错误作出出错处理
	*/
	virtual int Send(LPCSTR buf,int nLen) = NULL;

	// 查询当前是否在连接状态
	virtual BOOL IsConnected() = NULL;

	// 设置一些参数
	virtual BOOL ioctrl(DWORD dwCmd,DWORD dwParam,/*out*/void* pData) = NULL;

	// 释放对象
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
struct IKeyStream
{
	enum {MaxBufLen = 16* 1024 + 256/**/};

	/* 功能：初始化加密层，设置发送和接收初始密钥
	 说明：增加InitKey()接口是为了：
				1、允许用户在连接断开以后重新设置通讯包初始化收发密钥
				2、用户登录包到达服务器时，服务器需要初始化连接密钥；
				3、服务器返回登录结果时，会带上一个密鈅，客户端也需要变更收发密钥
	*/
	virtual BOOL InitKey(LPCSTR buf,int nLen) {return FALSE;};


	virtual BOOL KeyFirstData(/*in out*/char* szData, /*in*/int nMaxDataBufLen,/*in out*/int& nLen) = NULL;

	virtual BOOL UnKeyFirstData(/*in out*/char* szData,/*in*/int nMaxDataBufLen,/*in out*/int& nLen) = NULL;

	// 功能：加密数据
	// 输入：nLen不能超过MaxBufLen,nMaxDataBufLen为szData的长度，加密后的长度不得超过此nMaxDataBufLen
	// 返回：加密后的数据放在szData中，nLen为加密后的数据长度，
	virtual BOOL KeyData(/*in out*/char* szData, /*in*/int nMaxDataBufLen,/*in out*/int& nLen) = NULL;


	// 功能：解密数据
	// 输入：nLen不能超过MaxBufLen,nMaxDataBufLen为szData的长度，加密后的长度不得超过此长度
	// 返回：加密后的数据放在szData中，nLen为加密后的数据长度，
	virtual BOOL UnKeyData(/*in out*/char* szData,/*in*/int nMaxDataBufLen,/*in out*/int& nLen) = NULL;

};
//------------------------------------------------------------------------
/*
  功能：
		创建一个支持封包、加密、登录连接管理代理
  说明：调用关系如下

			    IClientLinkSink				 IClientSocketSink
		应用层 <--------------- IClientLink <------------------- IClientSocket
		       ---------------->			------------------->
			     IClientLink				 IClientSocket
  参数：
	pSink			: 通讯事件接收器

  可选参数:
    pOption			: 用于未来设置自定义的IKeyStream接口、设置其它选项等
    pClientSocket	: 可以是单线程模式或多线程模式的IClientSocket
					  当pClientSocket == NULL,表示取由CreateClientLink内部通过
						CreateSingleSocketProxy()创建一个基于HWND同步的IClientSocket
	pTrace			: 通讯层的调试信息显示接口

　返回：
		返回创建出的IClientLink对象
*/
extern "C" IClientLink* CreateClientLink(DWORD dwSocketID,IClientLinkSink* pSink,ITrace* pTrace = NULL,IClientSocket* pClientSocket = NULL,void* pOption = NULL);
//------------------------------------------------------------------------
class CClientLinkHelper
{
public:
	typedef IClientLink* (*ProcCreateClientLink)(DWORD dwVersion,DWORD dwSocketID,IClientLinkSink* pSink,ITrace* pTrace,IClientSocket* pClientSocket,void* pOption);
	CClientLinkHelper()
	{
		m_hdll = NULL;
		m_pClientLink = NULL;
	}
	~CClientLinkHelper()
	{
		Close();
	}
	void Close()
	{
		if(m_pClientLink)
		{
			m_pClientLink->Release();
			m_pClientLink = NULL;
		}
	}
	BOOL Create(DWORD dwSocketID,IClientLinkSink* pSink,ITrace* pTrace = NULL,IClientSocket* pClientSocket = NULL,void* pOption = NULL)
	{
		if(m_pClientLink)
		{
			try
			{
				m_pClientLink->Release();
			}
			catch(...)
			{
			}

			m_pClientLink = NULL;
		}

		try
		{
#if defined(_DEBUG)
			m_hdll = ::LoadLibrary("FKSocket_D.dll");
			if(m_hdll == NULL)
				throw "无法加载 FKSocket_D.dll";
#else
			m_hdll = ::LoadLibrary("FKSocket.dll");
			if(m_hdll == NULL)
				throw "无法加载 Socket.dll";
#endif
			ProcCreateClientLink proc = (ProcCreateClientLink)GetProcAddress(m_hdll, "dllCreateClientLink");
			if(proc == NULL)
				throw "Can not GetProcAddress('dllProcCreateClientLink')!";
			m_pClientLink = proc(VERSION_CLIENTLINK,dwSocketID,pSink,pTrace,pClientSocket,pOption);
			if(m_pClientLink == NULL)
				throw "dllCreateClientLink() failed";
			return TRUE;

		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return FALSE;
		}
		catch(...)
		{
			OutputDebugString("Error : CClientLinkHelper::Create() : catch error!\r\n");
			return FALSE;
		}
		return TRUE;
	}

public:
	IClientLink* m_pClientLink;
private:
	HINSTANCE		m_hdll;
};
//------------------------------------------------------------------------