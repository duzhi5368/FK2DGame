/**
*	created:		2012-6-24   2:38
*	filename: 		TTimer
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
class TTimer : public IAxisStrategy
{
public:
	virtual void	Start(LPCSTR pBuf,long lLen)
	{
	}
	virtual void	Stop()
	{
	}
	virtual BOOL	TimerInit();
	virtual BOOL	CalcCpuFreq();
	// 类似系统API返回毫秒级的TickCount数
	virtual DWORD	GetTickCount()
	{
		return (DWORD)(1000 * (GetRdtscCycleCount() - m_ui64Begin) / m_ui64Freq);
	}
	virtual UINT64	GetCpuFreq()
	{
		return m_ui64Freq;
	}
private:
	UINT64	m_ui64Begin;
	UINT64	m_ui64Freq;
};
//------------------------------------------------------------------------