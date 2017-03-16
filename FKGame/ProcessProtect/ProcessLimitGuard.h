/**
*	created:		2012-6-26   2:13
*	filename: 		ProcessLimitGuard
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
template<int nLimitCount>
class CProcessLimitGuard
{
public:
	CProcessLimitGuard()
	{
		m_hHandle = CreateSemaphore(NULL,nLimitCount,nLimitCount,"FKGame");
		m_bOwned = (m_hHandle != NULL && WAIT_OBJECT_0 == WaitForSingleObject(m_hHandle,0));
	}
	~CProcessLimitGuard()
	{
		if (m_bOwned)
		{
			ReleaseSemaphore(m_hHandle,1,NULL);
		}

		if (m_hHandle)
		{
			CloseHandle(m_hHandle);
		}
	}

	bool IsOwned()
	{
		return m_bOwned;
	}

	static CProcessLimitGuard* Instance()
	{ 
		static CProcessLimitGuard processLimitGuard;
		return &processLimitGuard;
	}

private:
	bool m_bOwned;
	HANDLE m_hHandle;
};
//------------------------------------------------------------------------