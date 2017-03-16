/**
*	created:		2012-6-24   2:23
*	filename: 		Axis
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define LOW_BYTE(n) (n & 0x00ff)
#define HI_BYTE(n) ((n>>8) & 0x00ff)
//------------------------------------------------------------------------
typedef bool (_stdcall *fInitializeWinIo)();
typedef void (_stdcall *fShutdownWinIo)();
typedef PBYTE (_stdcall *fMapPhysToLin)(PBYTE pbPhysAddr, DWORD dwPhysSize, HANDLE *pPhysicalMemoryHandle);
typedef bool (_stdcall *fUnmapPhysicalMemory)(HANDLE PhysicalMemoryHandle, PBYTE pbLinAddr);
typedef bool (_stdcall *fGetPhysLong)(PBYTE pbPhysAddr, PDWORD pdwPhysVal);
typedef bool (_stdcall *fSetPhysLong)(PBYTE pbPhysAddr, DWORD dwPhysVal);
typedef bool (_stdcall *fGetPortVal)(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
typedef bool (_stdcall *fSetPortVal)(WORD wPortAddr, DWORD dwPortVal, BYTE bSize);
typedef bool (_stdcall *fInstallWinIoDriver)(PSTR pszWinIoDriverPath, bool IsDemandLoaded);
typedef bool (_stdcall *fRemoveWinIoDriver)();
//------------------------------------------------------------------------
extern WORD GetTick();
extern inline UINT64  GetRdtscCycleCount();
extern UINT64 GetCpuFreq(WORD w);
extern BOOL InjectDll(DWORD dwProcessID,char *szLibFile);
extern BOOL EjectDll(DWORD dwProcessID,char *szLibFile);
extern DWORD GetWinIoProcessID();
extern WORD GetESI();
extern UINT64 GetCpuFreqEx();
//------------------------------------------------------------------------
extern fInitializeWinIo		g_pfInitializeWinIo;
extern fShutdownWinIo		g_pfShutdownWinIo;
extern fGetPortVal			g_pfGetPortVal;
extern fSetPortVal			g_pfSetPortVal;
//------------------------------------------------------------------------
extern char g_szWinioDllName[MAX_PATH];
extern char g_szWinioFakeDllName[MAX_PATH];
//------------------------------------------------------------------------
extern HINSTANCE	g_hInst;
extern UINT64		g_ui64Freq;
extern DWORD		g_dwProcessID;
//------------------------------------------------------------------------
extern HHOOK hGM;
extern BOOL SetGMHook();
extern BOOL RemGMHook();
//------------------------------------------------------------------------
#define INITIALIZE_WIN_IO_ID		3
#define SHUTDOWN_WIN_IO_ID			9
#define GET_PORT_VAL				2
#define SET_PORT_VAL				8
//------------------------------------------------------------------------