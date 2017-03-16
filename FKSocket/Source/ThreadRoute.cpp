/**
*	created:		2012-6-25   0:07
*	filename: 		ThreadRoute
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/Thread/IThreadRoute.h"
#include "../../FKTrace/IFKTrace.h"
#include "../include/ThreadRoute.h"
#include <algorithm>
#include <winbase.h>
#include <windows.h>
//------------------------------------------------------------------------
#if defined(_DEBUG) && !defined(__USE_MALLOC)
	#define __USE_MALLOC
	namespace std{class __default_alloc_template{};}
#endif
//------------------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//------------------------------------------------------------------------
DWORD WINAPI ThreadProc( LPSTR lpData);
//------------------------------------------------------------------------
CFKTrace g_DefaultTrace;
//------------------------------------------------------------------------
CThreadRoute::CThreadRoute()
{
	m_hCtrlEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_EventBuf[0] = m_hCtrlEvent;
	m_nUserEventCount = 0;
	
	m_dwEventIDLoop = 0;

	m_bSuspend = TRUE;
	m_bIsRunning = FALSE;
	m_dwThreadID = 0;
	m_hThread = NULL;

	Close();
}
//------------------------------------------------------------------------
CThreadRoute::~CThreadRoute()
{
	Close();

	if(m_hCtrlEvent)
	{
		CloseHandle(m_hCtrlEvent);
		m_hCtrlEvent = NULL;
	}
}
//------------------------------------------------------------------------
BOOL CreateThreadRoute(ITrace* pTrace,IThreadRoute** ppThreadRoute)
{
	g_DefaultTrace.Create(pTrace);

	if(ppThreadRoute == NULL)
		return FALSE;
	CThreadRoute *p = new CThreadRoute();
	if(p == NULL)
		return FALSE;
	if(!p->Create(pTrace))
	{
		delete p;
		return FALSE;
	}
	*ppThreadRoute = static_cast<IThreadRoute*>(p);
	return *ppThreadRoute != NULL;
}
//------------------------------------------------------------------------
#if !defined(_LIB_EXPORT_)
extern "C" __declspec(dllexport) BOOL dllCreateThreadRoute(DWORD dwQueryVersion,ITrace* pTrace,IThreadRoute** ppThreadRoute)
{
	if(dwQueryVersion > VERSION_ThreadRoute)
	{
		TraceLn("Error : dllCreateThreadRoute() : dwQueryVersion = %d,but only %d supported",dwQueryVersion,VERSION_ThreadRoute);
		return FALSE;
	}
	return CreateThreadRoute(pTrace,ppThreadRoute);
}
#endif
//------------------------------------------------------------------------
BOOL CThreadRoute::Start()
{
	if(IsRunning())
		return TRUE;

	DWORD dwThreadID;
	if(	NULL == (m_hThread = CreateThread(( LPSECURITY_ATTRIBUTES) NULL,0,
			(LPTHREAD_START_ROUTINE)ThreadProc,
			(LPVOID)this,0,&dwThreadID)))
    {
		return FALSE;
    }
	SetRunning(TRUE);
	m_dwThreadID = dwThreadID;
	m_bSuspend = FALSE;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::Stop(DWORD dwWaitTime)
{
	if(!IsRunning())
		return TRUE;
	ResumeThread();
	TriggerCtrlEvent(ctrlExit,dwWaitTime);
	if(IsRunning())
	{
		TraceLn("Warn : ThreadRoute::Stop : It can not be automaticly exit.Kill the thread\n");
		TerminateThread(m_hThread,-3);
		SetRunning(FALSE);
	}
	
	m_dwCtrlCmdMask = 0;
	m_dwThreadID = 0;
	CloseHandle(m_hThread);
	m_hThread = NULL;
	m_bSuspend = TRUE;

	return TRUE;
}
//------------------------------------------------------------------------
void CThreadRoute::Release()
{
	Stop(3000);
	delete this;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::IsRunning()
{
	return m_bIsRunning;
}
//------------------------------------------------------------------------
// 同上，不同之处在于由IThreadRoute创建一个Event，且由IThreadRoute删除
BOOL CThreadRoute::AddEvent(IEventSink* pEventSink,/*out*/DWORD& dwEventID)
{
	Assert(pEventSink != NULL);

	m_LockEventBuf.Lock();
	BOOL bOK = m_nUserEventCount < MAX_USER_EVENT_COUNT;
	if(bOK)
	{
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_EventBuf[m_nUserEventCount + SYS_EVENT_COUNT] = hEvent;

		++m_dwEventIDLoop;
		m_EventInfo[m_nUserEventCount].dwEventID = m_dwEventIDLoop;
		m_EventInfo[m_nUserEventCount].pEventSink = pEventSink;
		m_EventInfo[m_nUserEventCount].hLocalCreatedHandle = hEvent;
		dwEventID = m_dwEventIDLoop;

		++m_nUserEventCount;
	}
	m_LockEventBuf.Unlock();
	if(bOK)
	{
		if(!(bOK = TriggerCtrlEvent(ctrlFlipEventBuf,3000)))
			RemoveEvent(dwEventID);
	}
	return bOK;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::AddEvent(HANDLE hEvent,IEventSink* pEventSink,/*out*/DWORD& dwEventID)
{
	Assert(hEvent != NULL);
	Assert(pEventSink != NULL);

	m_LockEventBuf.Lock();
	BOOL bOK = m_nUserEventCount < MAX_USER_EVENT_COUNT;
	if(bOK)
	{
		
		m_EventBuf[m_nUserEventCount + SYS_EVENT_COUNT] = hEvent;

		++m_dwEventIDLoop;
		m_EventInfo[m_nUserEventCount].dwEventID = m_dwEventIDLoop;
		m_EventInfo[m_nUserEventCount].pEventSink = pEventSink;
		m_EventInfo[m_nUserEventCount].hLocalCreatedHandle = NULL;
		dwEventID = m_dwEventIDLoop;

		++m_nUserEventCount;
	}
	m_LockEventBuf.Unlock();

	if(bOK)
	{
		if(!(bOK = TriggerCtrlEvent(ctrlFlipEventBuf,3000)))
			RemoveEvent(dwEventID);
	}
	return bOK;
}
//------------------------------------------------------------------------
HANDLE CThreadRoute::GetEventHandle(DWORD dwEventID)
{
	HANDLE hEvent = NULL;
	m_LockEventBuf.Lock();
	for(int i = 0; i < m_nUserEventCount; i++)
	{
		if(m_EventInfo[i].dwEventID == dwEventID)
		{
			hEvent = m_EventBuf[i + SYS_EVENT_COUNT];
			break;
		}
	}
	m_LockEventBuf.Unlock();
#ifdef _DEBUG
	if(hEvent == NULL)
		OutputDebugString("Warn : CThreadRoute::GetEventHandle() : can not find event\n");
#endif
	return hEvent;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::RemoveEvent(DWORD dwEventID)
{
	m_LockEventBuf.Lock();
	BOOL bOK = FALSE;
	for(int i  = 0; i < m_nUserEventCount; i++)
	{
		if(m_EventInfo[i].dwEventID == dwEventID)
		{
			m_EventInfo[i].pEventSink->OnEventExit(dwEventID);
			if(m_EventInfo[i].hLocalCreatedHandle)
				CloseHandle(m_EventInfo[i].hLocalCreatedHandle);
			if(i != m_nUserEventCount - 1)
			{
				memmove(&m_EventInfo[i],&m_EventInfo[i+1],(m_nUserEventCount - i - 1) * sizeof(EVENT_INFO));
				memmove(&m_EventBuf[i + SYS_EVENT_COUNT],&m_EventBuf[i + SYS_EVENT_COUNT + 1],(m_nUserEventCount - i - 1) * sizeof(HANDLE));
			}
			--m_nUserEventCount;
			bOK = TRUE;
			break;
		}
	}
	m_LockEventBuf.Unlock();
	AssertR(bOK,FALSE,"CThreadRoute::RemoveEvent() : Can not find Event");

	bOK = TriggerCtrlEvent(ctrlFlipEventBuf,3000);	// 超时未能成功
	AssertR(bOK,FALSE,"CThreadRoute::RemoveEvent() : Can not TriggerCtrlEvent()");
	return bOK;
}
//------------------------------------------------------------------------
// 清空一些资源
void CThreadRoute::Close()
{
	if(m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	for(int i = 0; i < m_nUserEventCount; i++)
		m_EventInfo[i].Close();
	m_nUserEventCount = 0;
	memset(m_EventInfo,0,sizeof(m_EventInfo));

	m_dwCtrlCmdMask = 0;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::Create(ITrace *pTrace)
{
	Close();
	g_DefaultTrace.Create(pTrace);
	return TRUE;
}
//------------------------------------------------------------------------
void CThreadRoute::SetRunning(BOOL bRunning)
{
	m_LockEventBuf.Lock();
	m_bIsRunning = bRunning;
	m_LockEventBuf.Unlock();
}
//------------------------------------------------------------------------
// 线程退出
void CThreadRoute::OnTheadExit()
{
}
//------------------------------------------------------------------------
// 将pRoute的Event配置更新到线程工作Event buffer
void UpdateEventBuf(CThreadRoute *pRoute,HANDLE* pEventBuf,EVENT_INFO* pEventInfo,int& nEventCount)
{
	nEventCount = pRoute->m_nUserEventCount + SYS_EVENT_COUNT;
	memcpy(pEventBuf,pRoute->m_EventBuf,nEventCount * sizeof(HANDLE));
	memcpy(pEventInfo,&pRoute->m_EventInfo[0],pRoute->m_nUserEventCount * sizeof(EVENT_INFO));
}
//------------------------------------------------------------------------
DWORD WINAPI ThreadProc( LPSTR lpData)
{
	CThreadRoute *pRoute = (CThreadRoute*)lpData;
	int			nEventCount;
	HANDLE		EventBuf[MAXIMUM_WAIT_OBJECTS];
	EVENT_INFO	EventInfo[MAX_USER_EVENT_COUNT];		// 前SYS_EVENT_COUNT个总是系统事件
	
	UpdateEventBuf(pRoute,EventBuf,EventInfo,nEventCount);

	DWORD	dwThreadReturn = 0;
#ifdef _DEBUG
	char	szReturnMsg[128] = "";
#endif

	DWORD dwTimeOutTick = -1,dwTimerNextTick = -1;
	pRoute->OnTriggerTimer(dwTimeOutTick,dwTimerNextTick);	// 初始化与定时器相关的变量
	while(TRUE)
	{
		DWORD dwEvent = WaitForMultipleObjectsEx(nEventCount,EventBuf,FALSE,dwTimeOutTick,TRUE);
		if(dwEvent >= (WAIT_OBJECT_0 + SYS_EVENT_COUNT) && dwEvent < (WAIT_OBJECT_0 + nEventCount) )
		{
			EVENT_INFO *pInfo = &EventInfo[dwEvent - WAIT_OBJECT_0 - SYS_EVENT_COUNT];
			pInfo->pEventSink->OnEvent(pInfo->dwEventID);
		}
		else if(dwEvent == WAIT_TIMEOUT)
		{
			pRoute->OnTriggerTimer(dwTimeOutTick,dwTimerNextTick);
			continue;
		}
		else if(dwEvent == WAIT_OBJECT_0)
		{ 
			
			if(pRoute->m_dwCtrlCmdMask & ctrlExit)
			{// 退出
				pRoute->ClearCtrEventMask(ctrlExit);
#ifdef _DEBUG
				strcpy(szReturnMsg,"Set Exit Event");
#endif
				break;
			}
			if(pRoute->m_dwCtrlCmdMask & ctrlFlipEventBuf)
			{
				UpdateEventBuf(pRoute,EventBuf,EventInfo,nEventCount);
				pRoute->ClearCtrEventMask(ctrlFlipEventBuf);
			}
			if(pRoute->m_dwCtrlCmdMask & ctrlNewTimer)
			{// 增加了新的timer时，可能有新的timer需要立即执行
				pRoute->ClearCtrEventMask(ctrlNewTimer);
				pRoute->OnTriggerTimer(dwTimeOutTick,dwTimerNextTick);
			}
			
		}
		else if(dwEvent == -1)
		{ // 将出错的event删除掉? 暂无实现
			dwThreadReturn = 1;
#ifdef _DEBUG
			strcpy(szReturnMsg," {Error => Find invalid event} ");
#endif
			break;
		}
		else 
		{
			dwThreadReturn = 2;
#ifdef _DEBUG
			wsprintf(szReturnMsg," {unknown dwEvent = %d} ",dwEvent);
#endif
			break;
		}

		// 随时检查定时器是否到达
		if(dwTimerNextTick != -1)   //???? 可能会导致总不检查timer队列否？
		{
			dwTimeOutTick = dwTimerNextTick - GetTickCount();
			if(dwTimeOutTick == 0 || dwTimeOutTick > 0x7FFFFFFF)
			{// 时间到达
				pRoute->OnTriggerTimer(dwTimeOutTick,dwTimerNextTick);
			}
		}
	}
	pRoute->SetRunning(FALSE);
	pRoute->OnTheadExit();

#ifdef _DEBUG
	char szMsg[512];
	wsprintf(szMsg,"\n=>>>>>>> Exit IThreadRoute,return = %d, %s<<<<<<<<=\n",dwThreadReturn,szReturnMsg);
	OutputDebugString(szMsg);
#endif
	return dwThreadReturn;
}
//------------------------------------------------------------------------
// 增删事件时，触发ThreadProc将CThreadRoute的Event buf复制到自已的临时buf中
BOOL CThreadRoute::TriggerCtrlEvent(ThreadCtrlCmd cmd,DWORD dwWaitTime)
{
	if(!IsRunning())
		return TRUE;
	m_LockEventBuf.Lock();
	m_dwCtrlCmdMask |= cmd;
	m_LockEventBuf.Unlock();
	SetEvent(m_hCtrlEvent);
	Sleep(0);
	DWORD dwLast = GetTickCount();
	while((m_dwCtrlCmdMask & cmd) && GetTickCount() - dwLast < dwWaitTime)
	{
		Sleep(1);	// 将当前线程时间片耗完，使ThreadProc线程有机会更新事件缓冲
					// 不过，当增删多个事件时，事件设置多次，但可能只触发一次，从而导致ThreadProc忽略最后几次EventBuf的更新
					// 所以在此循环等待
	}
	if(m_dwCtrlCmdMask & cmd)
	{
		OutputDebugString("Warn : CThreadRoute::TriggerCtrlEvent() : time out!\n");
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::SetTimer(ITimerEventSink *pTimerSink,DWORD dwIDEvent,DWORD dwElapse,DWORD dwParam)
{
	if(pTimerSink == NULL)
		return FALSE;
;
	m_LockTimerList.Lock();
	CTimerNode t(pTimerSink,dwIDEvent,dwElapse,dwParam);
	list<CTimerNode>::iterator it = upper_bound(m_TimerList.begin(),m_TimerList.end(),t);
	m_TimerList.insert(it,t);
	m_LockTimerList.Unlock();

	if(IsRunning())
	{
		m_LockEventBuf.Lock();
		m_dwCtrlCmdMask |= ctrlNewTimer;
		m_LockEventBuf.Unlock();
		SetEvent(m_hCtrlEvent);	// 不需要等待线程执行完毕
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CThreadRoute::KillTimer(ITimerEventSink *pTimerSink,DWORD dwIDEvent)
{
	m_LockEventBuf.Lock();
	m_TimerList.remove(CTimerNode(pTimerSink,dwIDEvent,0,0));
	m_LockEventBuf.Unlock();
	return TRUE;
}
//------------------------------------------------------------------------
// 触发timer事件
void CThreadRoute::OnTriggerTimer(DWORD &dwNextTimeOut,DWORD& dwTimerNextTick)
{
	m_LockTimerList.Lock();
	if(m_TimerList.empty())
	{
		m_LockTimerList.Unlock();
		return;
	}
	list<CTimerNode>::iterator it = m_TimerList.begin();
	list<CTimerNode>::iterator end =m_TimerList.end();
	for(;it != end;)
	{
		dwNextTimeOut = (*it).dwNextTick - GetTickCount();
		if(dwNextTimeOut > 0 && dwNextTimeOut < 0x7FFFFFFF)	// 最快的timer还没到
			break;
		DWORD dwElapseTick = (*it).pTimerSink->OnTimer((*it).dwIDEvent,(*it).dwParam);
		if(dwElapseTick == -1)
		{
			m_TimerList.erase(it++);
			if(m_TimerList.empty())
			{
				dwTimerNextTick = -1;
				dwNextTimeOut = -1;
				m_LockTimerList.Unlock();
				return;
			}
		}
		else 
		{ // 把此timer结点移到下一个时间点
			(*it).dwElapse = dwElapseTick;
			(*it).dwNextTick = GetTickCount() + dwElapseTick;
			
			list<CTimerNode>::iterator it_next = it;
			it_next++;
			list<CTimerNode>::iterator pos = upper_bound(it_next,m_TimerList.end(),*it);
			m_TimerList.insert(pos,*it);
			m_TimerList.erase(it);
			it = it_next;
		}
	}
	dwNextTimeOut =	m_TimerList.front().dwNextTick - GetTickCount();
	if(dwNextTimeOut > 0x7FFFFFFF)
		dwNextTimeOut = 0;

	dwTimerNextTick = m_TimerList.front().dwNextTick;
	m_LockTimerList.Unlock();
}
//------------------------------------------------------------------------
void CThreadRoute::ClearCtrEventMask(ThreadCtrlCmd ctrlClearMask)
{
	m_LockEventBuf.Lock();
	m_dwCtrlCmdMask &= ~ctrlClearMask;
	m_LockEventBuf.Unlock();
}
//------------------------------------------------------------------------