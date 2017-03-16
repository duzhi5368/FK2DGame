/**
*	created:		2012-6-21   0:05
*	filename: 		CrashReport
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../PreInclude.h"
//------------------------------------------------------------------------
class CCrashReport : public ICrashDumpSink
{
public:
	// 开始dump信息会调用这个函数----实现这个函数，设置错误报告内容
	virtual void	OnDumpStart(ICrashDump *);
	// 可以用ICrashDump的DumpLn Dump想输出到LogFile的信息
	virtual void	OnDump(ICrashDump*,PEXCEPTION_POINTERS);
	// Dump结束会调用此函数，此函数返回值会被当作异常捕获的返回值
	virtual LONG	OnDumpFinish(ICrashDump*,PEXCEPTION_POINTERS);
};
//------------------------------------------------------------------------