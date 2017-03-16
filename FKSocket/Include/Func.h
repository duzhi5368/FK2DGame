/**
*	created:		2012-6-25   0:11
*	filename: 		Func
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
class CCriticalSectionLock
{
public:
	CCriticalSectionLock()
	{
		InitializeCriticalSection(&m_sect);
		m_bNoneLock = FALSE;
	}
public:
	CRITICAL_SECTION m_sect;
	BOOL m_bNoneLock;
public:
	inline void Lock()
	{
		if(m_bNoneLock)
			return;
		EnterCriticalSection(&m_sect);
	}
	inline void Unlock()
	{
		if(m_bNoneLock)
			return;
		LeaveCriticalSection(&m_sect);
	}
	// 设置锁无效，当为同一线程时无须锁时使用
	void SetNoneLock(BOOL bNoneLock)
	{
		m_bNoneLock = bNoneLock;
	}
public:
	virtual ~CCriticalSectionLock(){}
};
//------------------------------------------------------------------------
extern char *MyAllocMemory(int nLen);
//------------------------------------------------------------------------