/**
*	created:		2012-6-24   2:43
*	filename: 		Axis
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <Windows.h>
#include <tlhelp32.h>
#include "../Include/Axis.h"
#include <vector>
#include <stdio.h>
#include <algorithm>
using std::vector;
//------------------------------------------------------------------------
fInitializeWinIo	g_pfInitializeWinIo = 0;
fShutdownWinIo		g_pfShutdownWinIo = 0;
fGetPortVal			g_pfGetPortVal = 0;
fSetPortVal			g_pfSetPortVal = 0;
//------------------------------------------------------------------------
void OUTPUT(char *lpszFormat,...)
{
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;
	char szBuffer[256] = {0};
	nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
	va_end(args);

	OutputDebugString(szBuffer);
}
//------------------------------------------------------------------------
void PrintModule(DWORD dwProcessID)
{
	char sz[2048] = {0};
	HANDLE hModuleShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwProcessID);
	MODULEENTRY32 me2 = {sizeof(me2)};
	BOOL bOk = Module32First(hModuleShot,&me2);
	for(;bOk;bOk = Module32Next(hModuleShot,&me2))
	{
		sprintf(sz,"%s\n%s",sz,me2.szModule);
	}
	CloseHandle(hModuleShot);

	MessageBox(0,sz,0,0);
}
//------------------------------------------------------------------------
inline WORD GetTick()
{
	DWORD dwPortValLow = 0,dwPortValHi = 0;
	WORD wCount = 0;

	g_pfSetPortVal(0x43, 0x0, 1);
	g_pfGetPortVal(0x40, &dwPortValLow, 1);
	g_pfGetPortVal(0x40, &dwPortValHi, 1);

	wCount |= (BYTE)dwPortValLow;
	wCount |= (BYTE)dwPortValHi << 8;

	return wCount;
}
//------------------------------------------------------------------------
inline UINT64  GetRdtscCycleCount()
{
	__asm rdtsc
	/*
	__asm _emit 0x0F // 因为RDTSC不被C++的内嵌汇编器直接支持
	__asm _emit 0x31 // 所以我们要用_emit伪指令直接嵌入该指令的机器码形式	  
	*/
}
//------------------------------------------------------------------------
UINT64 GetCpuFreq(WORD w)
{
	DWORD dwBegin = GetTick();
	
	//提高线程优先级
	DWORD dwOldProcessP = GetPriorityClass(GetCurrentProcess());
	DWORD dwOldThreadP = GetThreadPriority(GetCurrentThread());
#define _test_times_	5
	vector<UINT64> vFreq;
	
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	
	for(int i = 0;i < _test_times_;i++)
	{
		DWORD dwCount = 0;
		UINT64 ui64Begin = GetRdtscCycleCount();
		//1193177 / 128 = 9321.6953125
		//1193177 / 64 = 18643.390625
		while(dwCount < 18643)
		{
			DWORD dwTick = GetTick();
			dwCount += (dwBegin > dwTick ? dwBegin - dwTick : w + dwBegin - dwTick);
			dwBegin = dwTick;
		}
		UINT64 ui64End = GetRdtscCycleCount();
		vFreq.push_back(ui64End - ui64Begin);
	}
	
	// 换会线程优先级
	SetThreadPriority(GetCurrentThread(), dwOldThreadP);
	SetPriorityClass(GetCurrentProcess(), dwOldProcessP);
	
	std::sort(vFreq.begin(),vFreq.end());
	//ui64FreqCount << 7 >> 2
	//m_ui64Freq = (ui64FreqCount << 5);
	return vFreq[_test_times_ / 2] << 6;
}
//------------------------------------------------------------------------
BOOL APIENTRY DllMain( HINSTANCE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			g_hInst = hModule;

			OUTPUT("\n\nhModule = 0x%x\n",hModule);
			OUTPUT("g_dwProcessID = %d\n",g_dwProcessID);
			OUTPUT("CurrentProcessID = %d\n",GetCurrentProcessId());
			OUTPUT("FakeWinioDllName = %s\n",g_szWinioFakeDllName);

			DisableThreadLibraryCalls(hModule);

			if(g_dwProcessID != GetCurrentProcessId())
				return TRUE;

			HINSTANCE hWinIO = GetModuleHandle(g_szWinioDllName);
			if(hWinIO)
			{
				OUTPUT("GetWinioHandle OK!\n");
				g_pfGetPortVal			= (fGetPortVal)GetProcAddress(hWinIO,"GetPortVal");
				g_pfSetPortVal			= (fSetPortVal)GetProcAddress(hWinIO,"SetPortVal");

				if(g_pfGetPortVal && g_pfSetPortVal)
				{
					OUTPUT("g_pfGetPortVal = 0x%x g_pfSetPortVal = 0x%x\n",g_pfGetPortVal,g_pfSetPortVal);
					g_ui64Freq = GetCpuFreqEx();
					OUTPUT("GetFreq OK!\n");
				}
			}
			//for general purpose,this is adequate
			if(g_ui64Freq)
			{
				OUTPUT("g_ui64Freq != 0\n");
				return FALSE;
			}

			OUTPUT("\n----------\n\n");

			return FALSE;
		}
	case DLL_THREAD_ATTACH:
		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}
//------------------------------------------------------------------------
BOOL InjectDll(DWORD dwProcessID,char *szLibFile)
{
	HANDLE hProcess=0,hThread=0;
	char *pRemote=0;

	hProcess=OpenProcess(PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION|PROCESS_VM_WRITE,FALSE,dwProcessID);
	if(!hProcess)
	{
		CloseHandle(hProcess);
		return FALSE;
	}
	pRemote=(char*)VirtualAllocEx(hProcess,NULL,strlen(szLibFile)+1,MEM_COMMIT,PAGE_READWRITE);
	if(!pRemote)
	{
		CloseHandle(hProcess);
		return FALSE;
	}
	if(!WriteProcessMemory(hProcess,pRemote,(void*)szLibFile,strlen(szLibFile)+1,NULL))
	{
		VirtualFreeEx(hProcess,pRemote,0,MEM_RELEASE);
		CloseHandle(hProcess);
		return FALSE;
	}
	PTHREAD_START_ROUTINE pfnThread=(PTHREAD_START_ROUTINE)
		GetProcAddress(GetModuleHandle("kernel32"),"LoadLibraryA");
	if(!pfnThread)
	{
		VirtualFreeEx(hProcess,pRemote,0,MEM_RELEASE);
		CloseHandle(hProcess);
		return FALSE;
	}
//	PrintModule(dwProcessID);
	hThread=CreateRemoteThread(hProcess,NULL,0,pfnThread,pRemote,0,NULL);
	if(!hThread)
	{
		VirtualFreeEx(hProcess,pRemote,0,MEM_RELEASE);
		CloseHandle(hProcess);
		return FALSE;
	}
	WaitForSingleObject(hThread,INFINITE);
//	PrintModule(dwProcessID);

	if(pRemote)VirtualFreeEx(hProcess,pRemote,0,MEM_RELEASE);
	if(hThread)CloseHandle(hThread);
	if(hProcess)CloseHandle(hProcess);

	return TRUE;
}
//------------------------------------------------------------------------
BOOL EjectDll(DWORD dwProcessID,char *szLibFile)
{
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwProcessID);
	MODULEENTRY32 me={sizeof(me)};
	BOOL fFound=FALSE;
	BOOL fGoon=Module32First(hSnapshot,&me);
	for(;fGoon;fGoon=Module32Next(hSnapshot,&me))
	{
		fFound=(stricmp(me.szModule,szLibFile)==0)||(stricmp(me.szExePath,szLibFile)==0);
		if(fFound)break;
	}
	if(!fFound)return FALSE;
	HANDLE hProcess=OpenProcess(PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION,FALSE,dwProcessID);
	PTHREAD_START_ROUTINE pfnThread=(PTHREAD_START_ROUTINE)
		GetProcAddress(GetModuleHandle("kernel32"),"FreeLibrary");
	HANDLE hThread=CreateRemoteThread(hProcess,NULL,0,pfnThread,me.modBaseAddr,0,NULL);
	WaitForSingleObject(hThread,INFINITE);
	CloseHandle(hSnapshot);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}
//------------------------------------------------------------------------
DWORD GetWinIoProcessID()
{
	DWORD dwProcessID = (DWORD)-1;
	BOOL bFound = FALSE;
	PROCESSENTRY32 me={sizeof(me)};
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,GetCurrentProcessId());
	BOOL fOk=Process32First(hSnapshot,&me);
	for(;fOk;fOk=Process32Next(hSnapshot,&me))
	{
		HANDLE hModuleShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,me.th32ProcessID);
		MODULEENTRY32 me2 = {sizeof(me2)};
		BOOL bOk = Module32First(hModuleShot,&me2);
		for(;bOk;bOk = Module32Next(hModuleShot,&me2))
		{
			if(stricmp(me2.szModule,g_szWinioDllName) == 0)
			{
				bFound = TRUE;
				break;
			}
			if(stricmp(me2.szModule,"NTPORT.DLL") == 0)
			{
				bFound = TRUE;
				break;
			}
		}
		CloseHandle(hModuleShot);
		if(bFound)
		{
			dwProcessID = me.th32ProcessID;
			break;
		}
	}
	CloseHandle(hSnapshot);

	return dwProcessID;
}
//------------------------------------------------------------------------
WORD GetESI()
{
	DWORD _ebx = 0;
	WORD w1 = 0;
	while(_ebx < 0x000186A0)
	{
		w1 = GetTick();
		if(w1 < 0x00000320)break;
		++_ebx;
	}
	_ebx = 0;
	WORD w2 = 0;
	while(_ebx < 0x00002EE0)
	{
		w2 = GetTick();
		if(w2 > w1)
			w1 = w2;
		++_ebx;
	}
	
	return w1;
/*从ASpeeder(ver = 2004.9.4.4373)找出来的代码，上面是对应的C代码
__asm
{
	XOR		EBX,EBX
N1:	XOR		EDX,EDX
	MOV		AX,0x0043
	CALL	g_pfSetPortVal
	MOV		AX,0x0040
	CALL	g_pfGetPortVal
	MOV		ESI,EAX
	MOV		AX,0x0040
	CALL	g_pfGetPortVal
	SHL		EAX,8
	ADD	I,EAX
	CMP		ESI,0x00000320
	JB			ESN2
	INC		EBX
	CMP		EBX,0x000186A0
	JB		N1
N2:	XOR		EBX,EBX
N3:	XOR		EDX,EDX
	MOV		AX,0x0043
	CALL	g_pfSetPortVal
	MOV		AX,0x0040
	CALL	g_pfGetPortVal
	MOV		EDI,EAX
	MOV		AX,0x0040
	CALL	g_pfGetPortVal
	SHL		EAX,8
	ADD		EDI,EAX
	MOV		EAX,EDI
	CMP		ESI,EAX
	JAE		N4
	MOV		ESI,EAX
N4:	INC		EBX
	CMP		EBX,0x00002EE0
	JB		N3
	
	MOV		EAX,ESI
  }
*/
}
//------------------------------------------------------------------------
UINT64 GetCpuFreqEx()
{
	UINT64 ui64Freq = 0;
	
	WORD w = GetESI();
	bool bOk = g_pfSetPortVal(0x43,0x34,1);
	if(!bOk)
	{
		return 0;
	}
	bOk = g_pfSetPortVal(0x40,LOW_BYTE(w),1);
	if(!bOk)
	{
		return 0;
	}

	bOk = g_pfSetPortVal(0x40,HI_BYTE(w),1);
	if(!bOk)
	{
		return 0;
	}

	#define _get_cpu_freq_times_	5
	for(int i = 0;i < _get_cpu_freq_times_;i++)
	{
		UINT64 ui64FreqTemp = GetCpuFreq(w ? w : 65536);
		if(ui64FreqTemp > ui64Freq)
			ui64Freq = ui64FreqTemp;
	}

	return ui64Freq;
}
//------------------------------------------------------------------------
HINSTANCE	g_hInst = 0;
//------------------------------------------------------------------------
// 定义共享数据段 
#pragma data_seg("shared") 
UINT64		g_ui64Freq = 0;
DWORD		g_dwProcessID = (DWORD)-1;
char		g_szWinioDllName[MAX_PATH] = "winio.dll";
char		g_szWinioFakeDllName[MAX_PATH] = "castle.dll";
#pragma data_seg() 
#pragma comment(linker, "/section:shared,rws") 
//------------------------------------------------------------------------
HHOOK hGM = 0;
//win9x下挂钩
DWORD __stdcall gmCallBack(int nCode,WPARAM wParam,LPARAM lParam)
{
	return CallNextHookEx(hGM,nCode,wParam,lParam);
}
//------------------------------------------------------------------------
BOOL SetGMHook()
{
	BOOL bSuccessed=FALSE;
	if(!hGM)
	{
		hGM=SetWindowsHookExA(WH_GETMESSAGE,(HOOKPROC)gmCallBack,g_hInst,0);
		if(hGM) bSuccessed=TRUE;
	}

	return bSuccessed;
}
//------------------------------------------------------------------------
BOOL RemGMHook()
{
	BOOL bSuccessed=FALSE;
	if(hGM)
	{
		bSuccessed=UnhookWindowsHookEx(hGM);
		if(bSuccessed) hGM=0;
	}
	
	return bSuccessed;
}
//------------------------------------------------------------------------