/**
*	created:		2012-6-24   21:20
*	filename: 		IEvent
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		事件系统接口头文件
*/
//------------------------------------------------------------------------
/*
	事件系统使用说明:
	定义事件服务器
	CEventServerHelper EventServerHelper;
	if(EventServerHelper.Create(pResMgr, pFileName, bEncrypt))
	{
		//...
	}

	订阅者举例
	class CMoveAction : public IEventActionListener
	{
		virtual void OnAction(IEvent *pEvent)
		{
			//...
		}
	};

	CMoveAction MoveAction;
	if (EventServerHelper.IsValid())
	{
		EventServerHelper->AddListener(this, MSG_THING_MOVE, SRC_TYPE_ID_PLAYER, 0,
		"CMoveAction") ;
	}

	发布者举例1
	class CMoveEvent : public IEvent
	{
	public:
		CMoveEvent(CPlay *pPlayer):m_pPlayer(pPlayer){}
		WORD GetID() {return MSG_THING_MOVE;}	
		BYTE GetSrcTypeID() {return SRC_TYPE_ID_PLAYER;}
		DWORD GetSrcPointer() {return (DWORD)m_pPlayer;}
		BYTE* GetContext(int &nLen) {nLen = 0; return NULL;}
	private:
		CPlayer *m_pPlayer;
	}

	CPlayer Player;
	CMoveEvent MoveEvent;
	if (EventServerHelper.IsValid())
	{
		EventServerHelper->FireAction(&MoveEvent);
	}

	发布者举例2 
	struct SContext
	{
	//..
	};
	SContext Context;
	if (EventServerHelper.IsValid())
	{
	EventServerHelper->FireAction(&Context, sizeof(SContext),
		MSG_THING_MOVE, SRC_TYPE_ID_PLAYER, (DWORD)&Player);
	}
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
enum
{
	SRC_TYPE_ID_ROOT = 0,			//类型ID 根
	SRC_TYPE_ID_THING = 1,			//THING
	SRC_TYPE_ID_CREATURE =2,		//生物
	SRC_TYPE_ID_ACTOR = 3,			//玩家
	SRC_TYPE_ID_MONSTER = 4,		//怪物
	SRC_TYPE_ID_CHAT = 5,			//聊天
	SRC_TYPE_ID_GOODS = 6,			//物品
	SRC_TYPE_ID_GOODS_CONTAINER = 7,//通用物品栏
	SRC_TYPE_ID_FRIEND = 8,			//好友
	SRC_TYPE_ID_CHUNNEL = 9,		//传送门
	SRC_TYPE_ID_BOX = 10,			//盒子
	SRC_TYPE_ID_STATUS = 11,		//状态
	SRC_TYPE_ID_TEAM = 12,			//组队 
	SRC_TYPE_ID_SYSTEM = 13,		//系统事件
	SRC_TYPE_ID_RESOURCE = 14,		//资源	
	SRC_TYPE_ID_STALL = 15,			//摊位
	SRC_TYPE_ID_CLAN  = 16,         //氏族
	SRC_TYPE_ID_RELATION = 17,		//社会关系
	MAX_SRC_TYPE_ID,
};
//------------------------------------------------------------------------
class IEventVoteListener;
class IEventActionListener;
class IEventResponseListener;
//------------------------------------------------------------------------
class IEvent
{
public:
	virtual ~IEvent(){};
	//获取事件ID
	virtual WORD GetID() = NULL;
	//获取事件源类型ID
	virtual BYTE GetSrcTypeID() = NULL;
	//获取事件源指针
	virtual DWORD GetSrcPointer() = NULL;
	// 描  述：获取现场
	// 参  数：[输出] int &nLen  - 现场大小	
	// 返回值：BYTE* - 现场指针
	virtual BYTE* GetContext(int &nLen) = NULL;
};
//------------------------------------------------------------------------
class CEventServerHelper;
//------------------------------------------------------------------------
class IEventServer
{
public:
	virtual ~IEventServer(){}
	//触发否决事件
	virtual BOOL FireVote(IEvent *pEvent) = NULL;
	virtual BOOL FireVote(BYTE *pContext, int nLen,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer) = NULL;
	//触发行为
	virtual void FireAction(IEvent *pEvent) = NULL;
	virtual void FireAction(BYTE *pContext, int nLen,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer) = NULL;
	//触发响应
	virtual void FireResponse(IEvent *pEvent) = NULL;
	virtual void FireResponse(BYTE *pContext, int nLen,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer) = NULL;

	// 描  述：增加Listener,订阅否决事件
	// 参  数：[输出] IEventVoteListener *pListener  - 事件否决者	
	// 参  数：[输出] WORD wEventID  - 事件ID
	// 参  数：[输出] DWORD dwSrcPointer  - 事件源指针 	
	// 参  数：[输出] const char *pDescription  - 订阅者描述信息,可以就填订阅者的类名	
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
	virtual BOOL AddListener(IEventVoteListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0,
		const char *pDescription = NULL) = NULL;

	// 描  述：增加Listener,订阅行为事件
	// 参  数：[输出] IEventActionListener *pListener  - 事件行为执行者	
	// 参  数：[输出] WORD wEventID  - 事件ID
	// 参  数：[输出] DWORD dwSrcPointer  - 事件源指针 	
	// 参  数：[输出] const char *pDescription  - 订阅者描述信息,可以就填订阅者的类名	
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
	virtual BOOL AddListener(IEventActionListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0,
		const char *pDescription = NULL) = NULL;

	// 描  述：增加Listener,订阅响应事件
	// 参  数：[输出] IEventResponseListener *pListener  - 事件响应者	
	// 参  数：[输出] WORD wEventID  - 事件ID
	// 参  数：[输出] DWORD dwSrcPointer  - 事件源指针 	
	// 参  数：[输出] const char *pDescription  - 订阅者描述信息,可以就填订阅者的类名	
	// 返回值：BOOL - 订阅成功，则返回TRUE,否则返回FALSE
	virtual BOOL AddListener(IEventResponseListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0,
		const char *pDescription = NULL) = NULL;

	// 描  述：删除Listener,取消订阅否决事件,不会delete pListener指针
	// 参  数：[输出] IEventVoteListener *pListener  - 事件否决者	
	// 参  数：[输出] WORD wEventID  - 事件ID
	// 参  数：[输出] DWORD dwSrcPointer  - 事件源指针 		
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE
	virtual BOOL RemoveListener(IEventVoteListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0) = NULL;

	// 描  述：删除Listener,取消订阅行为事件,不会delete pListener指针
	// 参  数：[输出] IEventActionListener *pListener  - 事件行为执行者	
	// 参  数：[输出] WORD wEventID  - 事件ID
	// 参  数：[输出] DWORD dwSrcPointer  - 事件源指针 		
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE
	virtual BOOL RemoveListener(IEventActionListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0) = NULL;

	// 描  述：删除Listener,取消订阅响应事件,不会delete pListener指针
	// 参  数：[输出] IEventResponseListener *pListener  - 事件响应者	
	// 参  数：[输出] WORD wEventID  - 事件ID
	// 参  数：[输出] DWORD dwSrcPointer  - 事件源指针 		
	// 返回值：BOOL - 取消订阅成功，则返回TRUE,否则返回FALSE
	virtual BOOL RemoveListener(IEventResponseListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0) = NULL;

protected:
	virtual void Release() = NULL;
	friend CEventServerHelper;
};
//------------------------------------------------------------------------
//事件否决者
class IEventVoteListener 
{
public:
	virtual BOOL OnVote(IEvent *pEvent) = NULL;	
};
//------------------------------------------------------------------------
//行为执行者
class IEventActionListener 
{
public:
	virtual void OnAction(IEvent *pEvent) = NULL;
};
//------------------------------------------------------------------------
//事件响应者
class IEventResponseListener 
{
public:
	virtual void OnResponse(IEvent *pEvent) = NULL;
};
//------------------------------------------------------------------------
struct IResMgr;
struct ITrace;
//------------------------------------------------------------------------
class CEventServerHelper
{
private:
	typedef BOOL (*ProcCreateEventServer)(IEventServer **ppEventServer, 
		IResMgr *pResMgr,ITrace *pTrace, const char *pFileName, BOOL bEncrypt);	
public:
	CEventServerHelper()
	{
		m_hDll = NULL;
		m_pEventServer = NULL;
		m_szErrMsg[0] = 0;
	}
	~CEventServerHelper()
	{		
		if (m_pEventServer != NULL)
		{
			m_pEventServer->Release();
			m_pEventServer = NULL;
		}
		if (m_hDll != NULL)//注意，如果是lib方式，由于m_hDll总是NULL,所以下面的::FreeLibary()不会被调用
		{
			::FreeLibrary(m_hDll);
			m_hDll=NULL;
		}
	}	
	// 描  述：创建事件服务器
	// 参  数：[输出] IResMgr *pResMgr  - 资源管理器	
	// 参  数：[输出] ITrace *pTrace  - Trace	
	// 参  数：[输出] const char *pFileName  - 脚本文件名,包含路径
	// 参  数：[输出] BOOL bEncrypt  - 文件是否加密,扩展名为scp是没有加密的,扩展名为res的是加密的
	// 返回值：BOOL - 创建成功则返回TRUE,否则返回FALSE,重复调用不会重复创建,保持单例
	BOOL Create(IResMgr *pResMgr,ITrace *pTrace, const char *pFileName = NULL, BOOL bEncrypt = FALSE)//创建接口对象
	{	
		if (IsValid())
		{
			//共用同一个对象
			return TRUE;
		}
		try
		{
			if (m_hDll == NULL)
			{
				//第一次调用时未加载dll,下面进行加载dll
#ifdef _DEBUG
				m_hDll = ::LoadLibrary("FKEvent_D.dll");
				if (m_hDll == NULL)
					throw "无法加载 FKEvent_D.dll";
#else
				m_hDll = ::LoadLibrary("FKEvent.dll");
				if (m_hDll == NULL)
					throw "无法加载FKEvent.dll";
#endif
			}
			//下面获得dll的输出函数，即接口对象的创建函数
			ProcCreateEventServer proc = NULL;
			proc=(ProcCreateEventServer)::GetProcAddress(m_hDll,
				"CreateEventServer");
			if (proc == NULL)
				throw "Can't GetProcAddress('CreateEventServer')";
			if (!proc(&m_pEventServer, pResMgr, pTrace, pFileName, bEncrypt))
				throw "CreateEventServer() error!";
			
			return TRUE;//接口对象创建成功
		}
		catch (LPCSTR szMsg) 
		{
			lstrcpyn(m_szErrMsg, szMsg, sizeof(m_szErrMsg));
			return FALSE;
		}
		catch(...)
		{
			lstrcpyn(m_szErrMsg, "Unknown Error!", sizeof(m_szErrMsg));
			return FALSE;
		}
	}
	
	//重载->,将helper对象的引用直接转为接口的引用
	IEventServer* operator ->()
	{
		return m_pEventServer;
	}
	IEventServer* GetEventServer()
	{
		return m_pEventServer;
	}
	//判断接口指针是否有效
	BOOL IsValid()
	{
		return (m_pEventServer != NULL);
	}
	char* GetErrMsg()
	{
		return m_szErrMsg;
	}
public:
	IEventServer *m_pEventServer;						//接口对象
	HINSTANCE m_hDll;									//动态链接库加载与否标志
	char m_szErrMsg[128];								//错误信息
};
//------------------------------------------------------------------------