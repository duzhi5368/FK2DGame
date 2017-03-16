/**
*	created:		2012-6-24   2:18
*	filename: 		MSpeed
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		8253定时器速度修改
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define FREE_INT_NO 5
//------------------------------------------------------------------------
void	SetClock9x(int freq);
WORD	GetClock9x();
BOOL	IsWindows98();
__int64	GetCycleCountPerSecond();
BOOL	DisableSpeedSteping();
bool	ReturnOriginalPowerScheme( void );
//------------------------------------------------------------------------
