/**
*	created:		2012-6-24   2:16
*	filename: 		IStrategy
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "AxisCommon.h"
//------------------------------------------------------------------------
//策略基类
struct IAxisStrategy
{
	virtual void	Start(LPCSTR pBuf,long lLen) = 0;
	virtual void	Stop() = 0;
	virtual BOOL	TimerInit() = 0;
	virtual BOOL	CalcCpuFreq() = 0;
	virtual DWORD	GetTickCount() = 0;
	virtual UINT64	GetCpuFreq() = 0;
};
//------------------------------------------------------------------------
//转换器
template< class STATE >
class CStrategySwitch
{
	STATE * m_pCurrentSate;
public:
	CStrategySwitch() : m_pCurrentSate( NULL )
	{
	}

	~CStrategySwitch()
	{
		if( m_pCurrentSate != NULL ) m_pCurrentSate->Stop();
	}

	void GoToState( STATE * pNextState,LPCSTR pBuf = 0,long lLen = 0)
	{
		ASSERT( pNextState );
		if( m_pCurrentSate == pNextState ) return;
		if( m_pCurrentSate!= NULL ) m_pCurrentSate->Stop();
		m_pCurrentSate = pNextState;
		m_pCurrentSate->Start(pBuf,lLen);
	}

	STATE * GetCurrentState()
	{
		ASSERT( m_pCurrentSate );
		return m_pCurrentSate;
	}
};
//------------------------------------------------------------------------