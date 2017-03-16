/**
*	created:		2012-6-24   2:19
*	filename: 		MTimer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		精确时间类
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "AxisCommon.h"
#include <stdlib.h>
#include <time.h>
#include "HTimer.h"
//------------------------------------------------------------------------
struct IAxisStrategy;
//------------------------------------------------------------------------
class MTimer : public IAxisStrategy
{
public:
	virtual void	Start(LPCSTR pBuf,long lLen)
	{
	}
	virtual void	Stop()
	{
	}
	// 初始化时钟
	virtual BOOL    TimerInit();
	virtual BOOL	CalcCpuFreq();
	// 类似系统API返回毫秒级的TickCount数
	virtual DWORD	GetTickCount();
	virtual UINT64	GetCpuFreq()
	{
		return m_RdtscPerSecond;
	}
public:
	MTimer();
	virtual ~MTimer();
private:
	UINT64  CalculateRdtscPerSecond();
	UINT64  CalculateRdtscPerSecond2();
	UINT64  CalculateRdtscPerSecond3();
	void	sleep( clock_t wait );
	// 取得当前CPU时钟周期
	inline UINT64  GetRdtscCycleCount();
	// 取得初始CPU时钟周期
	inline UINT64  GetStartCycleCount();
	// 取得每毫秒时间周期数
	inline UINT64  GetRdtscPerSecond();

	enum
	{
		__ticks_per_second  = 1000 ,	// 每秒1000TICKS
		__sleep_time_forcheck=256  ,	// sleep的时间
		__test_flush_times  = 4  ,		// 测试flush时间次数
		__sleep_times		= 2	   ,	// 测试sleep的次数
	};
private:
	UINT64  m_StartCycleCount;
	UINT64	m_RdtscPerSecond;
	DWORD   m_dwLastTickCount;
};
//------------------------------------------------------------------------