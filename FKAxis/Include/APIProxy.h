/**
*	created:		2012-6-24   2:25
*	filename: 		APIProxy
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		隐藏对API的调用
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
#include <Tlhelp32.h>
#include "AxisCommon.h"
//------------------------------------------------------------------------
// 取得一个API函数的地址
#define LOAD_API_FUNCTION(APINAME,APIDLL)					\
	P##APINAME	API##APINAME()								\
	{														\
	static P##APINAME	s_pAPIFuntion = NULL;				\
	if( s_pAPIFuntion )										\
	return s_pAPIFuntion;								\
	HINSTANCE hInstance = GetModuleHandle(APIDLL);			\
	if( hInstance == NULL )									\
	hInstance = LoadLibrary(APIDLL);					\
	ASSERT(hInstance);										\
	s_pAPIFuntion = 										\
	(P##APINAME)GetProcAddress(hInstance,#APINAME);		\
	ASSERT(s_pAPIFuntion);									\
	return s_pAPIFuntion;									\
	}														\

// 用数组下标的方式载入
#define LOAD_API_BYFUNINDEX(APINAME,APIDLL,index)			\
	P##APINAME	API##APINAME()								\
	{														\
	static P##APINAME	s_pAPIFuntion = NULL;				\
	if( s_pAPIFuntion )										\
	return s_pAPIFuntion;									\
	HINSTANCE hInstance = GetModuleHandle(APIDLL);			\
	if( hInstance == NULL )									\
	hInstance = LoadLibrary(APIDLL);						\
	ASSERT(hInstance);										\
	s_pAPIFuntion = 										\
	(P##APINAME)GetProcAddress(hInstance,(LPCSTR)index);	\
	ASSERT(s_pAPIFuntion);									\
	return s_pAPIFuntion;									\
	}														\

#define DECLARE_API_FUNCTION(APINAME)						\
	P##APINAME API##APINAME();								\

//------------------------------------------------------------------------
// 定义函数指针类型
typedef HANDLE (WINAPI *PCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL   (WINAPI *PProcess32First)( HANDLE hSnapshot, LPPROCESSENTRY32 lppe );
typedef BOOL   (WINAPI *PProcess32Next)( HANDLE hSnapshot, LPPROCESSENTRY32 lppe );
typedef BOOL   (WINAPI *PModule32First)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );
typedef BOOL   (WINAPI *PModule32Next)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );
typedef BOOL   (*PEnumWindows)(WNDENUMPROC lpEnumFunc, LPARAM lParam );
typedef bool   (_stdcall *PInitializeWinIo)();
typedef void   (_stdcall *PShutdownWinIo)();
typedef bool   (_stdcall *PGetPortVal)(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
typedef bool   (_stdcall *PSetPortVal)(WORD wPortAddr, DWORD dwPortVal, BYTE bSize);
typedef HANDLE (WINAPI *PCreateRemoteThread)(HANDLE hProcess,LPSECURITY_ATTRIBUTES lpThreadAttributes,SIZE_T dwStackSize,LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter,DWORD dwCreationFlags,LPDWORD lpThreadId);
typedef BOOL   (WINAPI *PDisableThreadLibraryCalls)(HMODULE hModule);
typedef BOOL   (WINAPI *PQueryPerformanceCounter)(LARGE_INTEGER *lpPerformanceCount);
typedef BOOL   (WINAPI *PQueryPerformanceFrequency)(LARGE_INTEGER *lpFrequency);
typedef BOOL   (WINAPI *PWriteProcessMemory)(HANDLE hProcess,LPVOID lpBaseAddress,LPCVOID lpBuffer,SIZE_T nSize,SIZE_T* lpNumberOfBytesWritten);
typedef HANDLE (WINAPI *POpenProcess)(DWORD dwDesiredAccess,BOOL bInheritHandle,DWORD dwProcessId);
typedef HHOOK  (WINAPI *PSetWindowsHookExA)(int idHook,HOOKPROC lpfn,HINSTANCE hMod,DWORD dwThreadId);
typedef BOOL   (WINAPI *PUnhookWindowsHookEx)(HHOOK hhk);
typedef LRESULT (WINAPI *PCallNextHookEx)(HHOOK hhk,int nCode,WPARAM wParam,LPARAM lParam);
typedef LONG   (WINAPI *PRegCloseKey)(HKEY hKey);
typedef LONG   (WINAPI *PRegOpenKeyExA)(HKEY hKey,LPCTSTR lpSubKey,DWORD ulOptions,REGSAM samDesired,PHKEY phkResult);
typedef LONG   (WINAPI *PRegQueryValueExA)(HKEY hKey,LPCTSTR lpValueName,LPDWORD lpReserved,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData);
typedef LONG   (WINAPI *PRegSetValueExA)(HKEY hKey,LPCTSTR lpValueName,DWORD Reserved,DWORD dwType,const BYTE* lpData,DWORD cbData);
//------------------------------------------------------------------------
DECLARE_API_FUNCTION(CreateToolhelp32Snapshot)
DECLARE_API_FUNCTION(Process32First)
DECLARE_API_FUNCTION(Process32Next)
DECLARE_API_FUNCTION(Module32First)
DECLARE_API_FUNCTION(Module32Next)
DECLARE_API_FUNCTION(EnumWindows)
DECLARE_API_FUNCTION(InitializeWinIo)
DECLARE_API_FUNCTION(ShutdownWinIo)
DECLARE_API_FUNCTION(GetPortVal)
DECLARE_API_FUNCTION(SetPortVal)
DECLARE_API_FUNCTION(CreateRemoteThread)
DECLARE_API_FUNCTION(DisableThreadLibraryCalls)
DECLARE_API_FUNCTION(QueryPerformanceCounter)
DECLARE_API_FUNCTION(QueryPerformanceFrequency)
DECLARE_API_FUNCTION(WriteProcessMemory)
DECLARE_API_FUNCTION(OpenProcess)
DECLARE_API_FUNCTION(SetWindowsHookExA)
DECLARE_API_FUNCTION(UnhookWindowsHookEx)
DECLARE_API_FUNCTION(CallNextHookEx)
DECLARE_API_FUNCTION(RegCloseKey)
DECLARE_API_FUNCTION(RegOpenKeyExA)
DECLARE_API_FUNCTION(RegQueryValueExA)
DECLARE_API_FUNCTION(RegSetValueExA)
//------------------------------------------------------------------------
#define CreateToolhelp32Snapshot	APICreateToolhelp32Snapshot()
#define Process32First				APIProcess32First()
#define Process32Next				APIProcess32Next()
#define Module32First				APIModule32First()
#define Module32Next				APIModule32Next()
#define EnumWindows					APIEnumWindows()
#define InitializeWinIo				APIInitializeWinIo()
#define ShutdownWinIo				APIShutdownWinIo()
#define GetPortVal					APIGetPortVal()
#define SetPortVal					APISetPortVal()
#define CreateRemoteThread			APICreateRemoteThread()
#define DisableThreadLibraryCalls	APIDisableThreadLibraryCalls()
#define QueryPerformanceCounter		APIQueryPerformanceCounter()
#define QueryPerformanceFrequency	APIQueryPerformanceFrequency()
#define QueryPerformanceFrequency	APIQueryPerformanceFrequency()
#define WriteProcessMemory			APIWriteProcessMemory()
#define OpenProcess					APIOpenProcess()
#define SetWindowsHookExA			APISetWindowsHookExA()
#define UnhookWindowsHookEx			APIUnhookWindowsHookEx()
#define CallNextHookEx				APICallNextHookEx()
#define RegCloseKey					APIRegCloseKey()
#define RegOpenKeyExA				APIRegOpenKeyExA()
#define RegQueryValueExA			APIRegQueryValueExA()
#define RegSetValueExA				APIRegSetValueExA()
//------------------------------------------------------------------------