// FKCrashDumpTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
using namespace std;
//------------------------------------------------------------------------
// 是否开启简易非自定义DUMP
#define SIMPLE_DUMP
//------------------------------------------------------------------------
#include "../../IFKCrashDumpHelper.h"
//------------------------------------------------------------------------
#ifndef SIMPLE_DUMP
#include "MyCrashReporter.h"
#endif
//------------------------------------------------------------------------
CCrashDumpHelper g_CrashDumpHelper;
//------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	if( g_CrashDumpHelper.IsValid() == FALSE )
	{
		// 初始化失败，很可能读取动态链接库失败
		cout << "初始化 CrashDump 失败！请检查动态链接库是否存在或完整！" << endl;
		getchar();
		return -1;
	}

#ifndef SIMPLE_DUMP
	if( g_CrashDumpHelper.IsValid() )
	{
		g_CrashDumpHelper->AttachCrashDumpSink(&g_CrashReport);
	}
#endif


	int* p = 0;
	*p = 0;


#ifndef SIMPLE_DUMP
	if( g_CrashDumpHelper.IsValid() )
	{
		g_CrashDumpHelper->DetachCrashDumpSink(&g_CrashReport);
	}
#endif
	return 0;
}
//------------------------------------------------------------------------