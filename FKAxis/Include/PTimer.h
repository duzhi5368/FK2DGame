/**
*	created:		2012-6-24   2:24
*	filename: 		PTimer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Axis.h"
//------------------------------------------------------------------------
struct IAxisStrategy;
//------------------------------------------------------------------------
class PTimer : public IAxisStrategy
{
public:
	virtual void	Start(LPCSTR pBuf,long lLen)
	{
		m_dwBeginTime = 0;
		m_dwBeginTick = ::GetTickCount();

		if(pBuf && lLen > 0)
			m_dwBeginTime = *(DWORD*)pBuf;
	}
	virtual void	Stop()
	{
	}
	virtual BOOL    CalcCpuFreq()
	{
		return TRUE;
	}
	virtual BOOL	TimerInit()
	{
		return TRUE;
	}
	// 类似系统API返回毫秒级的TickCount数
	DWORD	GetTickCount()
	{
		return ::GetTickCount() + m_dwBeginTime - m_dwBeginTick;
	}
	virtual UINT64	GetCpuFreq()
	{
		return 0;
	}
private:
	DWORD	m_dwBeginTime;
	DWORD	m_dwBeginTick;
};
//------------------------------------------------------------------------