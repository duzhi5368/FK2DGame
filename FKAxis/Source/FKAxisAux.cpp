/**
*	created:		2012-6-24   3:00
*	filename: 		FKAxisAux
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/AxisCommon.h"
#include "../Include/TimeAxisAux.h"
#include "../Include/Axis.h"
//------------------------------------------------------------------------
extern BOOL IsWindows98A();
//------------------------------------------------------------------------
BOOL CTimeAxisAux::UsingGear()
{
	return m_bUsingGear;
}
//------------------------------------------------------------------------
void CTimeAxisAux::InitTimerStrategy()
{
	Log("Is98A = %d",IsWindows98A());
	Log("Is98 = %d",IsWindows98());

	GoToState(&m_PTimer);

	return; 
}
//------------------------------------------------------------------------
void CTimeAxisAux::RecalcCpuFreq()
{
	DWORD dwTick = GetTickCount();
	GoToState(&m_PTimer,(LPCSTR)&dwTick,sizeof(DWORD));
	return;

	try
	{
		if(!GetCurrentState()->CalcCpuFreq())
		{
			GoToState(&m_PTimer);
		}
	}
	catch(...)
	{
		GoToState(&m_PTimer);
	}
}
//------------------------------------------------------------------------
DWORD CTimeAxisAux::GetTickCount()
{
	return ::timeGetTime();
}
//------------------------------------------------------------------------
UINT64 CTimeAxisAux::GetCpuFreq()
{
	UINT64 ui64Freq = GetCurrentState()->GetCpuFreq();
	if(!ui64Freq)
		ui64Freq = GetCpuSpeedFromRegistry();

	return ui64Freq;
}
//------------------------------------------------------------------------
void CTimeAxisAux::TimerInit()
{
	if(!m_bInit)
	{
		InitTimerStrategy();
		timeBeginPeriod( 1 );	//…Ë÷√timeGetTime()¡£∂»Œ™1ms

		m_bInit = TRUE;
	}
}
//------------------------------------------------------------------------
UINT64 CTimeAxisAux::GetCpuSpeedFromRegistry()
{
	DWORD dwSpeed = 0;
	HKEY hKey;
	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE,"HARDWARE\\DESCRIPTION\\SYSTEM\\CentralProcessor\\0",0,KEY_READ,&hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize = 256;
		if(RegQueryValueExA(hKey,"~MHz",0,0,(LPBYTE)&dwSpeed,&dwSize) == ERROR_SUCCESS)
		{
		}
		RegCloseKey(hKey);
	}
	return dwSpeed * 1000000;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) ITimeAxisAux* CreateAxisAux(DWORD dwVer)
{
	return CTimeAxisAux::Instance();
}
//------------------------------------------------------------------------
BOOL IsWindows98()
{
	DWORD dwVersion = GetVersion();
	// Get the Windows version.
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	DWORD dwBuild;
	// Get the build number.
	
	BOOL bBeforeWin2000 = FALSE;

	if (dwVersion < 0x80000000)              // Windows NT/2000, Whistler
	{
		dwBuild = (DWORD)(HIWORD(dwVersion));
	}
	else if (dwWindowsMajorVersion < 4)      // Win32s
	{
		dwBuild = (DWORD)(HIWORD(dwVersion) & ~0x8000);
		bBeforeWin2000 = TRUE;
	}
	else                                     // Windows 95/98/Me
	{
		bBeforeWin2000 = TRUE;
	}

	return bBeforeWin2000;
}
//------------------------------------------------------------------------
BOOL IsWindows98A()
{
	if(IsWindows98())
	{
		OSVERSIONINFO osInfo;
		osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osInfo);

		if(stricmp(osInfo.szCSDVersion,"SE"))return TRUE;

		if(stricmp(osInfo.szCSDVersion,"A") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion," A") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion,"A ") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion," A ") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion,"B") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion," B") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion,"B ") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion," B ") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion,"  B ") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion," B  ") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion,"  B  ") == 0)
			return TRUE;
		if(stricmp(osInfo.szCSDVersion,"C") == 0)
			return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------