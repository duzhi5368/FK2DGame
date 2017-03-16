/**
*	created:		2012-6-25   0:09
*	filename: 		ThreadRoute
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Thread/IThreadRoute.h"
#include "Func.h"
#include <list>
using std::list;
//------------------------------------------------------------------------
#define FKTraceColor		g_DefaultTrace.ThreadSafeDebugPrintColor
//------------------------------------------------------------------------
#ifndef	MY_TRACE_COLOR
#define MY_TRACE_COLOR		RGB(146,0,123)
#endif
//------------------------------------------------------------------------
#ifdef _DEBUG
	#define VerifyR(Expression, ReturnValue, Description ) \
		if (Expression) \
	{\
		FKTraceColor(MY_TRACE_COLOR, "错误 : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,Description);\
		return ReturnValue;\
	}
	#define AssertR(Expression, ReturnValue, Description ) \
		if (!(Expression)) \
	{\
		FKTraceColor(MY_TRACE_COLOR, "ERROR : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,Description);\
		return ReturnValue;\
	}
	#define Assert(Expression) \
		if (!(Expression))\
	{\
		FKTraceColor(MY_TRACE_COLOR, "错误 : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,#Expression);\
		return 0;\
	}
	#define	TraceLn			g_DefaultTrace.DebugPrintLn
#else
	#define VerifyR(Expression, ReturnValue, Description ) \
		if (Expression) \
	{\
		return ReturnValue;\
	}
	#define AssertR(Expression, ReturnValue, Description ) \
		if (!(Expression)) \
	{\
		return ReturnValue;\
	}
	#define Assert(Expression) \
		if (!(Expression))\
	{\
		return 0;\
	}
	#define	TraceLn			__noop
#endif
//------------------------------------------------------------------------
#define SYS_EVENT_COUNT			1
#define MAX_USER_EVENT_COUNT	(64 - SYS_EVENT_COUNT)		//WaitForMultiObject最多支持64个事件
//------------------------------------------------------------------------
#ifdef _DEBUG
	#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
struct CTimerNode
{
	CTimerNode(ITimerEventSink *pTimerSink,DWORD dwIDEvent,DWORD dwElapse,DWORD dwParam)
	{
		this->pTimerSink = pTimerSink;
		this->dwIDEvent = dwIDEvent;
		this->dwElapse = dwElapse;
		this->dwParam = dwParam;
		dwNextTick = GetTickCount() + dwElapse;
	}
	BOOL operator < (const CTimerNode& t) const
	{
		return dwNextTick < t.dwNextTick;
	}
	BOOL operator == (const CTimerNode& t) const
	{
		return t.pTimerSink == this->pTimerSink && t.dwIDEvent == dwIDEvent;
	}
	ITimerEventSink *pTimerSink;
	DWORD dwIDEvent;
	DWORD dwElapse;
	DWORD dwParam;
	DWORD dwNextTick;
};
//------------------------------------------------------------------------
struct EVENT_INFO
{
	DWORD		dwEventID;
	IEventSink*	pEventSink;
	HANDLE		hLocalCreatedHandle;	// 由IThreadRoute创建和维护的event
	void Close()
	{
		if(hLocalCreatedHandle)
		{
			CloseHandle(hLocalCreatedHandle);
		}
	}
};
//------------------------------------------------------------------------
enum ThreadCtrlCmd
{
	ctrlNone = 0,
	ctrlExit = 1,			// 通知线程退出
	ctrlFlipEventBuf = 2,	// 主程序增加或减少Event时，通知ThreadRoute更新Event列表
	ctrlNewTimer = 4,		// 当添加一个新的timer,且新的timer需要修改WaitForMultipleObjectsEx()中的time out值时，触发此值
	ctrlUnknown = 0			// 
};
//------------------------------------------------------------------------
struct ITrace;
class CThreadRoute : public IThreadRoute  
{
public:
	int			m_nUserEventCount;						//
	HANDLE		m_EventBuf[MAXIMUM_WAIT_OBJECTS];		// m_EventBuf[n]与m_EventInfo[n - 1]对应
	EVENT_INFO	m_EventInfo[MAX_USER_EVENT_COUNT];		// 前SYS_EVENT_COUNT个总是系统事件
	DWORD		m_dwCtrlCmdMask;						// 所有命令位所在地

	void ClearCtrEventMask(ThreadCtrlCmd ctrlClearMask);
	void OnTriggerTimer(DWORD &dwNextTimeOut,DWORD& dwTimerNextTick);
	BOOL TriggerCtrlEvent(ThreadCtrlCmd cmd,DWORD dwWaitTime = 0);
	void OnTheadExit();
	int GetTotalEventCount()
	{
		return m_nUserEventCount + SYS_EVENT_COUNT;
	}

	void SetRunning(BOOL bRunning);
	BOOL Create(ITrace* pTrace);
	void Close();
	CThreadRoute();
	~CThreadRoute();

	// 启动线程
	// 可将所有的外挂模块加到IThreadRoute以后，启动线程，也可以在启动的过程中外挂模块将自已的事件动态添加进来
	virtual BOOL Start();
	
	// 停止线程,可以指定等待时间dwWaitTick，以毫秒为单位
	virtual BOOL Stop(DWORD dwWaitTime = 3000);
	
	// 线程死了没有?
	virtual BOOL IsRunning();
	
	/*
	 	功能：外挂模块将自已的一个事件挂接到当前线程中
		输入：
			hEvent
			pEventSink	：当此事件发生时，回调此接口
		返回：
			dwEventID	：返回当此事件发生时，回传给事件接收器IEventSink的标识
	*/
	virtual BOOL AddEvent(HANDLE hEvent,IEventSink* pEventSink,/*out*/DWORD& dwEventID);

	// 同上，不同这外在于系统自动帮忙创建一个Event
	virtual BOOL AddEvent(IEventSink* pEventSink,/*out*/DWORD& dwEventID);

	virtual HANDLE GetEventHandle(DWORD dwEventID);
	
	// 外挂模块完成任务时，请求主线程清除事件服务
	virtual BOOL RemoveEvent(DWORD dwEventID);
	
	// 定时调用
	virtual BOOL SetTimer(ITimerEventSink *pTimerSink,DWORD dwIDEvent,DWORD dwElapse,DWORD dwParam);
	
	// 外挂模块完成任务时，请求主线程清除定时服务
	virtual BOOL KillTimer(ITimerEventSink *pTimerSink,DWORD dwIDEvent);

	virtual void Release();

	virtual HANDLE GetThreadHandle()
	{
		return 	m_hThread;
	}
	virtual DWORD ResumeThread()
	{
		if(m_bSuspend)
		{
			m_bSuspend = FALSE;
			return ::ResumeThread(m_hThread);
		}
		return 0;
	}
	virtual DWORD SuspendThread()
	{
		if(m_bSuspend)
			return 1;
		m_bSuspend = TRUE;
		return ::SuspendThread(m_hThread);
	}

private:
	DWORD		m_dwThreadID;
	HANDLE		m_hThread;
	BOOL		m_bIsRunning;
	int			m_bSuspend;				// 是否已经挂起
	HANDLE 		m_hCtrlEvent;			// 先设置一个ThreadCtrlCmd指定位，然后触发m_hCtrlEvent
	DWORD		m_dwEventIDLoop;		// 每增加一个event，此ID总是加1，以使每个event总是获得唯一的ID
	list<CTimerNode>		m_TimerList;
	CCriticalSectionLock	m_LockTimerList;
	CCriticalSectionLock	m_LockEventBuf;
};
//------------------------------------------------------------------------