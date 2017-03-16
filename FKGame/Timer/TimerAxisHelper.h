/**
*	created:		2012-6-21   0:30
*	filename: 		TimerAxisHelper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "ITimerAxis.h"
//------------------------------------------------------------------------
// °æ±¾ºÅ
#define TIMER_AXIS_VERSION	1
//------------------------------------------------------------------------
class CTimerAxisHelper
{
	typedef ITimeAxis * (*ProcCreateAxis)(DWORD dwVersion);
public:
	//------------------------------------------------------------------------
	CTimerAxisHelper(void)
	{
		m_pTimeAxis = NULL;
	}
	//------------------------------------------------------------------------
	~CTimerAxisHelper(void)
	{
		Close();
	}
	//------------------------------------------------------------------------
	void Close(void)
	{
		if(m_pTimeAxis)
		{
			m_pTimeAxis->Release();
			m_pTimeAxis = 0;
		}
	}
	//------------------------------------------------------------------------
	BOOL Create(void)
	{
		try
		{
			HINSTANCE m_hdll = NULL;
			LoadDLL(m_hdll);

			ProcCreateAxis proc;
			proc = (ProcCreateAxis)GetProcAddress(m_hdll, "CreateAxis");

			if(proc == NULL)
			{
				throw "Can not GetProAddress('CreateAxis')";
			}

			m_pTimeAxis = proc(TIMER_AXIS_VERSION);

			return m_pTimeAxis != NULL;
		}
		catch(...)
		{
			OutputDebugString("Create timer axis catch error\r\n");
			LPVOID lpMsgBuf;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0,
				(LPTSTR)&lpMsgBuf,
				0,
				NULL);
			MessageBox(NULL, (LPCTSTR)lpMsgBuf, "ERROR!! load Axis.dll failed", MB_OK | MB_ICONINFORMATION);
			LocalFree(lpMsgBuf);
			return FALSE;		
		}
	}
	//------------------------------------------------------------------------
	ITimeAxisAux * GetTimeAxisAux(void)
	{
		HINSTANCE hdll = GetAxisModuleHandle();
		if(hdll)
		{
			typedef ITimeAxisAux* (*ProcCreateAxisAux)(DWORD dwVersion);

			ProcCreateAxisAux procAux;
			procAux = (ProcCreateAxisAux)GetProcAddress(hdll,"CreateAxisAux");

			if(procAux)
			{
				return procAux(TIMER_AXIS_VERSION);
			}
		}

		return 0;
	}
	//------------------------------------------------------------------------
	void LoadDLL(HINSTANCE &m_hdll)
	{
		if(m_hdll == NULL)
		{
#ifdef _DEBUG
			m_hdll = LoadLibrary("FKAxis_D.dll");
			if(m_hdll == NULL)
			{
				throw "Can not load AxisD.dll";
			}
#else
			m_hdll = LoadLibrary("FKAxis.dll");
			if(m_hdll == NULL)
			{
				throw "Can not load Axis.dll";
			}
#endif
		}
	}
	//------------------------------------------------------------------------
	HINSTANCE GetAxisModuleHandle()
	{
#ifdef _DEBUG
		return GetModuleHandle("FKAxis_D.dll" );
#else
		return GetModuleHandle("FKAxis.dll" );
#endif
	}
	//------------------------------------------------------------------------
public:
	ITimeAxis *m_pTimeAxis;
};
//------------------------------------------------------------------------