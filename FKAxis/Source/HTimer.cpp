/**
*	created:		2012-6-24   2:48
*	filename: 		HTimer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <windows.h>
#include "../Include/TimeAxisAux.h"
#include "../Include/HTimer.h"
#include "../Include/MSpeed.h"
//------------------------------------------------------------------------
BOOL    HTimer::TimerInit()
{
	if(CalcCpuFreq())
	{
		if( m_uQueryPerformanceCounter )
			m_uQueryPerformanceCounter( (LARGE_INTEGER*)&m_firstCount );
		else
			QueryPerformanceCounter( (LARGE_INTEGER*)&m_firstCount );

		return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
// 初始化时钟
BOOL    HTimer::CalcCpuFreq()
{	
	if( IsWindows98() == FALSE )
	{
		// 把系统dll拷贝过来
		char lpBuffer[MAX_PATH];
		GetSystemDirectory(lpBuffer,MAX_PATH);
		if(lpBuffer[strlen(lpBuffer) - 1] == '\\')
			lpBuffer[strlen(lpBuffer) - 1] = 0;
		strcat(lpBuffer,"\\kernel32.dll");
		char lpNew[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,lpNew);
		if(lpNew[strlen(lpNew) - 1] == '\\')
			lpNew[strlen(lpNew) - 1] = 0;
		strcat(lpNew,"\\hx.dat");
		CopyFile(lpBuffer,lpNew,FALSE);

		// 自己加载		
		HINSTANCE hInstance = LoadLibrary("hx.dat");
		if(hInstance)
		{
			m_uQueryPerformanceCounter = (PQueryPerformanceCounter)GetProcAddress(hInstance,"QueryPerformanceCounter");
			m_uQueryPerformanceFrequency = (PQueryPerformanceFrequency)GetProcAddress(hInstance,"QueryPerformanceFrequency");
		}

	}

	// 得到初始值
	__int64 perfFreq;
	if( m_uQueryPerformanceFrequency )
		m_uQueryPerformanceFrequency( (LARGE_INTEGER*)&perfFreq );
	else
		QueryPerformanceFrequency( (LARGE_INTEGER*)&perfFreq );

	m_freq = 1000.0 / (double)perfFreq;

	return TRUE;
}
//------------------------------------------------------------------------
// 类似系统API返回毫秒级的TickCount数
DWORD	HTimer::GetTickCount()
{
	__int64	curCount;
	if( m_uQueryPerformanceCounter )
		m_uQueryPerformanceCounter( (LARGE_INTEGER*)&curCount );
	else
		QueryPerformanceCounter ( (LARGE_INTEGER*)&curCount );
	curCount -= m_firstCount;
	double time = (double)curCount * m_freq;
	return static_cast<DWORD>(time);
}
//------------------------------------------------------------------------