/**
*	created:		2012-6-24   2:21
*	filename: 		NTimer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Axis.h"
#include <vector>
using std::vector;
//------------------------------------------------------------------------
#define TIMER_120HZ 0x2D67 // 120 hz
#define TIMER_60HZ 0x4DAE // 60 hz
#define TIMER_30HZ 0x965C // 30 hz
#define TIMER_20HZ 0xE90B // 20 hz
#define TIMER_18HZ 0xFFFF // 18.2 hz (the standard count)
//------------------------------------------------------------------------
struct IAxisStrategy;
//------------------------------------------------------------------------
extern BOOL   IsWindows98();
extern BOOL   IsWindows98A();
//------------------------------------------------------------------------
class NTimer : public IAxisStrategy
{
public:
	virtual void	Start(LPCSTR pBuf,long lLen)
	{
	}
	virtual void	Stop()
	{
	}
	virtual BOOL	TimerInit();
	virtual BOOL	CalcCpuFreq();
	// 类似系统API返回毫秒级的TickCount数
	virtual DWORD	GetTickCount()
	{
		return (DWORD)(1000 * (GetRdtscCycleCount() - m_ui64Begin) / g_ui64Freq);
	}
	virtual UINT64	GetCpuFreq()
	{
		return g_ui64Freq;
	}
private:
	UINT64	m_ui64Begin;
public:
	NTimer()
	{
	}
private:
	void WriteFreq(UINT64 ui64Freq)
	{
		HKEY hKey;
		LARGE_INTEGER li;
		li.QuadPart = ui64Freq;
		if(RegOpenKeyExA(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",0,KEY_WRITE,&hKey) == ERROR_SUCCESS)
		{
			RegSetValueExA(hKey,"low",0,REG_DWORD,(LPBYTE)&li,sizeof(DWORD));
			RegSetValueExA(hKey,"high",0,REG_DWORD,(LPBYTE)((DWORD*)&li+1),sizeof(DWORD));
			RegCloseKey(hKey);
		}
	}
	UINT64 ReadFreq()
	{
		HKEY hKey;
		LARGE_INTEGER li;
		li.QuadPart = 0;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",0,KEY_READ,&hKey) == ERROR_SUCCESS)
		{
			DWORD dwSize = 256;
			RegQueryValueExA(hKey,"low",0,0,(LPBYTE)&li,&dwSize);
			RegQueryValueExA(hKey,"high",0,0,(LPBYTE)((DWORD*)&li+1),&dwSize);
			RegCloseKey(hKey);
		}

		return li.QuadPart;
	}
	// 初始化时钟
	BOOL	TimerInit_9x()
	{
		BOOL bHook = SetGMHook();
		if(bHook)
		{
			RemGMHook();
		}
		if(!g_ui64Freq)
		{
			return TimerInit_Here();
		}

		//写入注册表
		WriteFreq(g_ui64Freq);

		return TRUE;
	}
	BOOL	TimerInit_NT()
	{
		char szFileName[MAX_PATH];
		GetModuleFileName(g_hInst,szFileName,MAX_PATH);
		if(InjectDll(g_dwProcessID,szFileName))
		{
			EjectDll(g_dwProcessID,szFileName);
		}

		if(!g_ui64Freq)
		{
			return TimerInit_Here();
		}

		//写入注册表
		WriteFreq(g_ui64Freq);

		return TRUE;
	}
	BOOL    TimerInit_Here()
	{
		HINSTANCE hInstance = LoadLibrary(g_szWinioFakeDllName);
		if(hInstance)
		{
			g_pfInitializeWinIo		= (fInitializeWinIo)GetProcAddress(hInstance,(LPCSTR)INITIALIZE_WIN_IO_ID);
			g_pfShutdownWinIo		= (fShutdownWinIo)GetProcAddress(hInstance,(LPCSTR)SHUTDOWN_WIN_IO_ID);
			g_pfGetPortVal			= (fGetPortVal)GetProcAddress(hInstance,(LPCSTR)GET_PORT_VAL);
			g_pfSetPortVal			= (fSetPortVal)GetProcAddress(hInstance,(LPCSTR)SET_PORT_VAL);

			if(g_pfInitializeWinIo && g_pfShutdownWinIo && g_pfGetPortVal && g_pfSetPortVal)
			{
				g_pfShutdownWinIo();
				if(g_pfInitializeWinIo())
				{
					g_ui64Freq = GetCpuFreqEx();
					g_pfShutdownWinIo();

					if(g_ui64Freq)
					{
						//写入注册表
						WriteFreq(g_ui64Freq);
						return TRUE;
					}
				}
				g_pfShutdownWinIo();
			}
		}
		//如果无法加载winio
		//这个值是可信的
		UINT64 ui64Freq = ReadFreq();
		if(ui64Freq > 0)
		{
			g_ui64Freq = ui64Freq;

			return TRUE;
		}
		return FALSE;
	}
};
//------------------------------------------------------------------------