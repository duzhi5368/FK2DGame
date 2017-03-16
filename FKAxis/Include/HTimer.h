/**
*	created:		2012-6-24   2:17
*	filename: 		HTimer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		高精度的时钟类
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
struct IAxisStrategy;
//------------------------------------------------------------------------
class HTimer : public IAxisStrategy
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
	virtual BOOL    CalcCpuFreq();

	// 类似系统API返回毫秒级的TickCount数
	DWORD	GetTickCount();

	virtual UINT64	GetCpuFreq()
	{
		return static_cast<UINT64>(m_freq);
	}
public:
	HTimer() : m_hDllInstance( NULL ) , 
		m_uQueryPerformanceCounter( 0 ) , 
		m_uQueryPerformanceFrequency( 0 ) ,
		m_freq( 0.0 ) ,
		m_firstCount( 0 ){
	}

	~HTimer()
	{
		if (m_hDllInstance)
		{
			FreeLibrary(m_hDllInstance);
			m_hDllInstance = NULL;
		}
	}

private:
	HINSTANCE m_hDllInstance;
	typedef BOOL (WINAPI *PQueryPerformanceFrequency)(LARGE_INTEGER *lp);
	typedef BOOL (WINAPI *PQueryPerformanceCounter)(LARGE_INTEGER *lp);
	PQueryPerformanceFrequency m_uQueryPerformanceFrequency;
	PQueryPerformanceCounter   m_uQueryPerformanceCounter;	

	double  m_freq;
	__int64 m_firstCount;
};
//------------------------------------------------------------------------