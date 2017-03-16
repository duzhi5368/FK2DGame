/**
*	created:		2012-6-24   2:41
*	filename: 		APIProxy
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/APIProxy.h"
//------------------------------------------------------------------------
LOAD_API_FUNCTION(CreateToolhelp32Snapshot,"kernel32.dll");
LOAD_API_FUNCTION(Process32First,"kernel32.dll");
LOAD_API_FUNCTION(Process32Next,"kernel32.dll");
LOAD_API_FUNCTION(Module32First,"kernel32.dll");
LOAD_API_FUNCTION(Module32Next,"kernel32.dll");
LOAD_API_FUNCTION(EnumWindows,"user32.dll");
LOAD_API_BYFUNINDEX(InitializeWinIo,"castle.dll",3);
LOAD_API_BYFUNINDEX(ShutdownWinIo,"castle.dll",9);
LOAD_API_BYFUNINDEX(GetPortVal,"castle.dll",2);
LOAD_API_BYFUNINDEX(SetPortVal,"castle.dll",8);
LOAD_API_FUNCTION(CreateRemoteThread,"kernel32.dll");	
LOAD_API_FUNCTION(DisableThreadLibraryCalls,"kernel32.dll");	
LOAD_API_FUNCTION(QueryPerformanceCounter,"kernel32.dll");	
LOAD_API_FUNCTION(QueryPerformanceFrequency,"kernel32.dll");	
LOAD_API_FUNCTION(WriteProcessMemory,"kernel32.dll");	
LOAD_API_FUNCTION(OpenProcess,"kernel32.dll");	
LOAD_API_FUNCTION(SetWindowsHookExA,"user32.dll");
LOAD_API_FUNCTION(UnhookWindowsHookEx,"user32.dll");
LOAD_API_FUNCTION(CallNextHookEx,"user32.ll");
LOAD_API_FUNCTION(RegCloseKey,"ADVAPI32.DLL");
LOAD_API_FUNCTION(RegOpenKeyExA,"ADVAPI32.DLL");
LOAD_API_FUNCTION(RegQueryValueExA,"ADVAPI32.DLL");
LOAD_API_FUNCTION(RegSetValueExA,"ADVAPI32.DLL");

/*
	castle.dll 函数下标对照表
	1---GetPhysLong					
	2---GetPortVal
	3---InitializeWinIo
	4---InstallWinIoDriver
	5---MapPhysToLin
	6---RemoveWinIoDriver
	7---SetPhysLong
	8---SetPortVal
	9---ShutdownWinIo
	10---UnmapPhysicalMemory
*/
//------------------------------------------------------------------------