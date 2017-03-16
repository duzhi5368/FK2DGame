/**
*	created:		2012-6-14   20:16
*	filename: 		MyCrashReporter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "stdafx.h"
#include "MyCrashReporter.h"
//------------------------------------------------------------------------
#define XCRASHREPORT_MINI_DUMP_FILE		_T("Report.dmp")
#define XCRASHREPORT_ERROR_LOG_FILE		_T("Errorlog.txt")
#define XCRASHREPORT_TRACE_FILE			_T("Trace.txt")
//------------------------------------------------------------------------
#ifdef _DEBUG
#define XCRASHREPORT_CRASH_REPORT_APP	_T("ReportD.dll")
#else
#define XCRASHREPORT_CRASH_REPORT_APP	_T("Report.dll")
#endif
//------------------------------------------------------------------------
#define _countof(array) (sizeof(array)/sizeof(array[0]))
//------------------------------------------------------------------------
CCrashReport g_CrashReport;
//------------------------------------------------------------------------
void CCrashReport::OnDumpStart(ICrashDump* pDump)
{
	if(!pDump)return;

	pDump->SetLogFileName(XCRASHREPORT_ERROR_LOG_FILE);
	pDump->SetDumpFileName(XCRASHREPORT_MINI_DUMP_FILE);
}
//------------------------------------------------------------------------
void CCrashReport::OnDump(ICrashDump* pDump,PEXCEPTION_POINTERS pExceptionInfo)
{
	// 你自己的代码
}
//------------------------------------------------------------------------
LONG CCrashReport::OnDumpFinish(ICrashDump* pDump,PEXCEPTION_POINTERS pExceptionInfo)
{
	TCHAR pszFilePart[MAX_PATH];
	lstrcpy(pszFilePart, XCRASHREPORT_CRASH_REPORT_APP);
	
	TCHAR szModuleName[MAX_PATH];
	lstrcpy(szModuleName,pDump->GetCrashModuleName(pExceptionInfo));

	TCHAR szCommandLine[MAX_PATH];
	lstrcpy(szCommandLine,pszFilePart);
	lstrcat(szCommandLine," ");
	
	lstrcat(szCommandLine, _T(" \""));	// surround app name with quotes
	ZeroMemory(szModuleName, sizeof(szModuleName));
	GetModuleFileName(0, szModuleName, _countof(szModuleName)-2);

	char *pModuleName = strrchr(szModuleName,'\\');
	if(pModuleName)
	{
		pModuleName++;
		lstrcat(szCommandLine,pModuleName);
	}
	else
	{
		lstrcat(szCommandLine,"Game.exe");
	}
	lstrcat(szCommandLine, _T("\""));
	
	//取得版本号
	char strVer[256] = {0};
	GetPrivateProfileString("OPTION","Help","",strVer,sizeof(strVer) - 1,".\\update.ini");

	//取得崩溃模块的产品版本号
	WORD pwVer[4];
	pDump->GetCrashModuleProductVer(pExceptionInfo,pwVer);

	//将版本号的第3位加到标题
	TCHAR szSubject[256];
	LPSTR szCrashModuleName = (LPSTR)pDump->GetCrashModuleName(pExceptionInfo);

	//将这些改成小写
	for(unsigned int i = 0;i < strlen(szCrashModuleName);i++)
	{
		if(szCrashModuleName[i] >= 'A' && szCrashModuleName[i] <= 'Z')
			szCrashModuleName[i] -= ('A' - 'a');
	}
	strcpy(szSubject,szCrashModuleName);
	sprintf(szSubject,"%s %04d %08x",/*pDump->GetExceptionDesc(pExceptionInfo),*/szSubject,pwVer[2],pDump->GetCrashFileOffset(pExceptionInfo));


	lstrcat(szCommandLine, _T(" \""));	// surround app name with quotes
	lstrcat(szCommandLine,szSubject);
	lstrcat(szCommandLine, _T("\""));

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	
	if (CreateProcess(
		NULL,					// name of executable module
		szCommandLine,			// command line string
		NULL,					// process attributes
		NULL,					// thread attributes
		FALSE,					// handle inheritance option
		0,						// creation flags
		NULL,					// new environment block
		NULL,					// current directory name
		&si,					// startup information
		&pi))					// process information
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}
	else
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
}
//------------------------------------------------------------------------