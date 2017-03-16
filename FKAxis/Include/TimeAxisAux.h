/**
*	created:		2012-6-24   2:39
*	filename: 		TimerAxisAux
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <stdio.h>
#include <MMSystem.h>
//------------------------------------------------------------------------
#include "../../FKGame/Timer/ITimerAxis.h"
#include "IStrategy.h"
#include "NTimer.h"
#include "MTimer.h"
#include "HTimer.h"
#include "PTimer.h"
#include "TTimer.h"
//------------------------------------------------------------------------
class CTimeAxisAux : public ITimeAxisAux,public CStrategySwitch<IAxisStrategy>
{
public:
	virtual BOOL UsingGear();
	virtual void RecalcCpuFreq();
	virtual UINT64 GetCpuFreq();
public:
	DWORD	GetTickCount();
	void	TimerInit();
	void	InitTimerStrategy();
	UINT64	GetCpuSpeedFromRegistry();
	void Log(char *chBuf,...)
	{
		va_list args;
		va_start(args, chBuf);

		int nBuf;
		char szBuffer[1024] = {0};

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), chBuf, args);

		va_end(args);

		fprintf(m_fp,szBuffer);
		fprintf(m_fp,"\r\n");
	}
	~CTimeAxisAux()
	{
		timeEndPeriod( 1 );		//Çå³ýÁ£¶È
		if (m_fp != NULL)
		{
			fclose(m_fp);
		}
	}
protected:
	CTimeAxisAux()
	{
		m_fp = NULL;
		m_fp = fopen("Log\\axis.txt","wb+");
		m_bUsingGear = FALSE;
		m_dwTick = 0;
		m_bInit = FALSE;

		GoToState(&m_PTimer);
	}
public:
	static CTimeAxisAux *Instance()
	{
		static CTimeAxisAux taa;
		return &taa;
	}
private:
	BOOL	m_bUsingGear;
private:
	NTimer	m_NTimer;
	MTimer	m_MTimer;
	HTimer	m_HTimer;
	PTimer	m_PTimer;
	TTimer	m_TTimer;
private:
	DWORD	m_dwTick;
	FILE	*m_fp;
private:
	BOOL	m_bInit;
};
//------------------------------------------------------------------------
#define GET_TICK_COUNT CTimeAxisAux::Instance()->GetTickCount()
//------------------------------------------------------------------------