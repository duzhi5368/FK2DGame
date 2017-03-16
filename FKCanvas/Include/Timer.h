/**
*	created:		2012-6-19   21:03
*	filename: 		Timer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
#ifndef _MSVC
	#define _MSVC
#endif
//------------------------------------------------------------------------
class WTimer  
{
private:
	union uINT64 {
		__int64 time;
		struct {
			DWORD low;
			DWORD high;
		};
	};

	int m_iPriority;
	HANDLE m_hThread;
	uINT64 m_i64Timer;
	DWORD m_StartTime;
	static DWORD s_CP10K;		// 万分之一秒的周期数
	static DWORD s_CyclePerSec;	// 一秒 CPU 运行周期
public:
	WTimer();

	static int CPU();			//返回 CPU 频率
	void Reset(int t=0);		//初始化计时器
	void Start();				//启动时间检测
	DWORD Stop();				//得到时间检测时间(最小单位是 16 时钟周期, 最大周期 P200 上 5 分)
	DWORD GetTime();			//返回当前计数值 (最小单位为 1/10000 秒)
};
//------------------------------------------------------------------------