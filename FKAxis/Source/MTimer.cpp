/**
*	created:		2012-6-24   2:51
*	filename: 		MTimer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/AxisCommon.h"
#include <winnt.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include "../Include/TimeAxisAux.h"
#include "../Include/MTimer.h"
#include "../Include/MSpeed.h"
using std::vector;
//------------------------------------------------------------------------
extern bool SpeedstepSupported(void);
extern unsigned int speedstep_detect_processor (void);
extern bool CheckRDTSCTechnology(void);
// ------------------------------------------------------------------------
MTimer::MTimer() : m_StartCycleCount( 0 ) , m_RdtscPerSecond( 0 )
{
}
//------------------------------------------------------------------------
MTimer::~MTimer()
{
}
//------------------------------------------------------------------------
/* Pauses for a specified number of milliseconds. */
void MTimer::sleep( clock_t wait )
{
	clock_t goal;
	goal = wait + clock();
	while( goal > clock() )
		;
}
//------------------------------------------------------------------------
//这个不准
UINT64  MTimer::CalculateRdtscPerSecond3()
{
	UINT64 time1 = GetRdtscCycleCount();

	// 提高线程优先级
    DWORD dwOldProcessP = GetPriorityClass(GetCurrentProcess());
    DWORD dwOldThreadP = GetThreadPriority(GetCurrentThread());

    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	sleep((clock_t)CLOCKS_PER_SEC >> 2);

	// 换会线程优先级
    SetThreadPriority(GetCurrentThread(), dwOldThreadP);
    SetPriorityClass(GetCurrentProcess(), dwOldProcessP);

	UINT64 time2 = GetRdtscCycleCount();

	return ((time2 - time1))/1000 << 2;
}
//------------------------------------------------------------------------
BOOL	MTimer::TimerInit()
{
	if(CalcCpuFreq())
	{
		m_StartCycleCount= GetRdtscCycleCount();
		return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
BOOL    MTimer::CalcCpuFreq()
{
	UINT64 ui64_1 = CalculateRdtscPerSecond();
	UINT64 ui64_2 = CalculateRdtscPerSecond2();
	//UINT64 ui64_3 = CalculateRdtscPerSecond3();

	CTimeAxisAux::Instance()->Log("CPU1 : %08I64u",ui64_1);
	CTimeAxisAux::Instance()->Log("CPU2 : %08I64u",ui64_2);

	m_RdtscPerSecond = max(ui64_1,ui64_2) * 1000;
	return TRUE;

	if(ui64_2 == 0)return FALSE;
	double dVar = (double)(__int64)ui64_1 / (double)(__int64)ui64_2;

	CTimeAxisAux::Instance()->Log("dVar: %f",dVar);

	if(dVar > 0.8 && dVar < 1.2)
	{
		m_RdtscPerSecond = ui64_2;
		m_StartCycleCount= GetRdtscCycleCount();
		return TRUE;
	}
	else
	{
		m_RdtscPerSecond = ui64_1;
		m_StartCycleCount= GetRdtscCycleCount();
		return TRUE;
	}
	
	return FALSE;
}
//------------------------------------------------------------------------
// 取得当前CPU时钟周期
#pragma warning( push )
#pragma warning( disable : 4035 4244) 
//------------------------------------------------------------------------
inline UINT64  MTimer::GetRdtscCycleCount()
{
		__asm rdtsc
		/*
		__asm _emit 0x0F // 因为RDTSC不被C++的内嵌汇编器直接支持
		__asm _emit 0x31 // 所以我们要用_emit伪指令直接嵌入该指令的机器码形式	  
		*/
}
//------------------------------------------------------------------------
#pragma warning( pop )
//------------------------------------------------------------------------
// 取得初始CPU时钟周期
inline UINT64  MTimer::GetStartCycleCount()
{
	return m_StartCycleCount;
}
//------------------------------------------------------------------------
// 取得每毫秒时间周期数
inline UINT64  MTimer::GetRdtscPerSecond()
{
	return m_RdtscPerSecond;
}
//------------------------------------------------------------------------
// 类似系统API返回毫秒级的TickCount数
DWORD	MTimer::GetTickCount()
{
	static DWORD s_dwLastTick = 0;

	return (DWORD)(1000 * (GetRdtscCycleCount() - m_StartCycleCount) / (m_RdtscPerSecond));
}
//------------------------------------------------------------------------
// 计算每秒的时间周期数
// 有多种不同的方法可以获得（策略模式？：）
// 必要的时候可以比较几种不同方法计算出来的结果
// 就可以很好的判断是否加速，也可以使结果更精确
UINT64  MTimer::CalculateRdtscPerSecond()
{
	// 提高线程优先级
    DWORD dwOldProcessP = GetPriorityClass(GetCurrentProcess());
    DWORD dwOldThreadP = GetThreadPriority(GetCurrentThread());

    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	//start calculate flush time
	UINT64  flushTimeRdtsc = 0;
	UINT64  testFlush = 0;
	UINT64  time1=0;
	UINT64  time2=0;
	for( int i = 0 ; i < __test_flush_times ; i ++ )
	{
		//make sure everything is flushed
		//before calculating flush function, this is overkill though
		__asm
		{
			cpuid
			cpuid
			cpuid
		}

		time1 = GetRdtscCycleCount();
		__asm
		{
			cpuid
		}
		time2 = GetRdtscCycleCount();
		
		testFlush += time2 - time1;
	}

	flushTimeRdtsc = testFlush / __test_flush_times;
	//end calculate flush time

	UINT64  testRdtscCount = 0;
	for( int j = 0 ; j < __sleep_times ; j++ )
	{
		__asm
		{
			cpuid
		}

		time1 = GetRdtscCycleCount();
		Sleep( __sleep_time_forcheck );
		__asm
		{
			cpuid
		}
		time2 = GetRdtscCycleCount();
		
		if( time2 - time1 <= flushTimeRdtsc )
			continue;

		time2 -= flushTimeRdtsc;
		testRdtscCount += time2 - time1;
	}

	// 换会线程优先级
    SetThreadPriority(GetCurrentThread(), dwOldThreadP);
    SetPriorityClass(GetCurrentProcess(), dwOldProcessP);

	return testRdtscCount / (__sleep_times * __sleep_time_forcheck );
}
//------------------------------------------------------------------------
// 考虑到变频(speedstep)的CPU需要重新计算频率
// 所以这时候可以采用这种方法
UINT64  MTimer::CalculateRdtscPerSecond2()
{
	LARGE_INTEGER waitTime, startCount, curCount;
	QueryPerformanceFrequency(&waitTime);
	int scale = 5;		// Take 1/32 of a second for the measurement.
	waitTime.QuadPart >>= scale;
	
	// 提高线程优先级
	DWORD dwOldProcessP = GetPriorityClass(GetCurrentProcess());
	DWORD dwOldThreadP = GetThreadPriority(GetCurrentThread());
	
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	
	
	vector<UINT64> v;
#define _test_times_1_	5
	for(int i = 0;i < _test_times_1_;i++)
	{	
		QueryPerformanceCounter(&startCount);
		UINT64 time1 = GetRdtscCycleCount();
		do
		{
			QueryPerformanceCounter(&curCount);
		}
		while(curCount.QuadPart - startCount.QuadPart < waitTime.QuadPart);
		UINT64 time2 = GetRdtscCycleCount();

		v.push_back(((time2 - time1) << scale)/1000);
	}

	// 换会线程优先级
    SetThreadPriority(GetCurrentThread(), dwOldThreadP);
    SetPriorityClass(GetCurrentProcess(), dwOldProcessP);

	return v[_test_times_1_ / 2];
}
//------------------------------------------------------------------------