/**
*	created:		2012-6-20   23:29
*	filename: 		ITimerAxis
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
struct ITimerSink
{
	// 定时检查时间轴
	virtual void	OnTimer(DWORD dwEventID) = NULL; 	
};
//------------------------------------------------------------------------
struct ITimeAxis
{
	// 定时检查时间轴
	virtual void	OnCheck(void) = NULL; 
	// 设置定时器，便于调试加了一个描述字符
	virtual BOOL	SetTimer(DWORD dwEventID, ITimerSink * pTimerSink, DWORD dwElapse, LPCSTR szTimerDesc = NULL) = NULL;
	// 销毁定时器
	virtual void	KillTimer(DWORD dwEventID, ITimerSink * pTimerSink)	= NULL;
	// 可以返回经过防加速处理的当前时间Tick数
	virtual DWORD	GetTimeTick(void) = NULL;
	// 销毁时间轴
	virtual void	Release(void) = NULL;
};
//------------------------------------------------------------------------
// 辅助接口 - 判断玩家是否使用加速，重新计算频率(变频CPU)
struct ITimeAxisAux
{
	virtual BOOL	UsingGear(void) = 0;
	virtual void	RecalcCpuFreq(void) = 0;
	virtual UINT64	GetCpuFreq(void) = 0;
};
//------------------------------------------------------------------------