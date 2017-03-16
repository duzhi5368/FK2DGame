/**
*	created:		2012-6-24   3:03
*	filename: 		TimeMan
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <winnt.h>
#include "NTimer.h"
#include "MTimer.h"
#include "HTimer.h"
#include "MSpeed.h"
//------------------------------------------------------------------------
extern bool CheckRDTSCTechnology(void);
//------------------------------------------------------------------------
class CTimeMan
{
public:
	// 初始化时钟
	void TimerInit()
	{
		try
		{
			if( ! CheckRDTSCTechnology() )
			{
				m_HTimer.TimerInit();
				m_bInitSucceed = TRUE;
				return;
			}

		}
		catch(...)
		{
			//
		}
		m_bInitSucceed = m_NTimer.TimerInit();
		if(!m_bInitSucceed)
			m_bInitSucceed = m_MTimer.TimerInit();
	}
	// 类似系统API返回毫秒级的TickCount数
	DWORD	GetTickCount()
	{
		if(m_NTimer.IsInitSucceed()) return m_NTimer.GetTickCount();
		if(m_MTimer.IsInitSucceed()) return m_MTimer.GetTickCount();
		if(m_HTimer.IsInitSucceed()) return m_HTimer.GetTickCount();

		return ::GetTickCount();
	}
private:
	BOOL	m_bInitSucceed;
	MTimer	m_MTimer;
	NTimer	m_NTimer;
	HTimer	m_HTimer;
public:
	CTimeMan() : m_bInitSucceed(FALSE)
	{
		try{
			// 先防止修改8253控制器

			if( IsWindows98() )
			{
				//				if( GetClock9x()!= 0 )
				//				{
				//				MessageBox( NULL , "初始化失败,系统发现可疑的作弊程序!" , "ERROR" , MB_OK );
				//				PostQuitMessage(0);
				//				return FALSE;
				//				}
			}else
			{
				//本来准备禁止变频，但变频是无法禁止的(?)
				//DisableSpeedSteping();
			}
		}
		catch(...)
		{}
	}
	~CTimeMan()
	{
		try{ 
			if( !IsWindows98() )
			{
				//本来准备还原电源管理
				//ReturnOriginalPowerScheme();
			}
		}catch(...)
		{
			//
		}	
	}
};
//------------------------------------------------------------------------