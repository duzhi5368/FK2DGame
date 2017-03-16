/**
*	created:		2012-6-14   18:12
*	filename: 		IFKCrashDumpHelper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
/*
	使用说明：
	1： Main所在文件中包含本文件头
	2： 注意在运行文件同级目录下放置对应的动态链接库。
		DEBUG版本放置	FKCrashDump_D.dll 
		RELEASE版本放置	FKCrashDump.dll
	3： 在Main前声明全局变量 CCrashDumpHelper g_CrashDumpHelper;
	完成。

	注意：
	1：	当前版本尚未支持Unicode字节。请将应用程序改为多字节。
		若有不便，可联系FreeKnight  
	http://freeknigt.jimdo.com
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
#include <string>
//------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------
struct ICrashDump;
//------------------------------------------------------------------------
enum ENUM_DumpInfoState
{
	enDumpLog,				//是否开启Log------如果此项为FALSE，则一下几项除了MiniDump都无效
	enDumpBasicInfo,		//基本信息
	enDumpModuleList,		//模块列表
	enDumpWindowList,		//窗口列表
	enDumpProcessList,		//进程列表
	enDumpRegister,			//寄存器
	enDumpCallStack,		//调用栈
	enDumpMiniDump,			//输出dmp文件
	enDumpCount
};
//------------------------------------------------------------------------
struct ICrashDumpSink
{
	// 开始dump信息会调用这个函数----实现这个函数，设置错误报告内容
	virtual void OnDumpStart(ICrashDump*) = 0;
	// 可以用ICrashDump的DumpLn Dump想输出到LogFile的信息
	virtual void OnDump(ICrashDump*,PEXCEPTION_POINTERS) = 0;
	// Dump结束会调用此函数，此函数返回值会被当作异常捕获的返回值
	virtual LONG OnDumpFinish(ICrashDump*,PEXCEPTION_POINTERS) = 0;
};
//------------------------------------------------------------------------
struct ICrashDump
{
	// 挂接/卸载 一个ICrashDumpSink的回调
	virtual BOOL AttachCrashDumpSink(ICrashDumpSink *) = 0;
	virtual BOOL DetachCrashDumpSink(ICrashDumpSink *) = 0;
	// 设置Log文件名
	virtual void SetLogFileName(LPCSTR szLogFileName) = 0;
	// 设置Dmp文件名
	virtual void SetDumpFileName(LPCSTR szDumpFileName) = 0;
	// 设置Dump信息开关（对应 ENUM_DumpInfoState ）
	virtual void SetDumpOn(int iEnum,BOOL bOn) = 0;

	// 设置Dump类型，lDumpType可以为一下这几种类型
	/*
		注意:MiniDumpWithIndirectlyReferencedMemory在win98下不能用

		MiniDumpNormal                         = 0x0000,
		MiniDumpWithDataSegs                   = 0x0001,
		MiniDumpWithFullMemory                 = 0x0002,
		MiniDumpWithHandleData                 = 0x0004,
		MiniDumpFilterMemory                   = 0x0008,
		MiniDumpScanMemory                     = 0x0010,
		MiniDumpWithUnloadedModules            = 0x0020,
		MiniDumpWithIndirectlyReferencedMemory = 0x0040,
		MiniDumpFilterModulePaths              = 0x0080,
		MiniDumpWithProcessThreadData          = 0x0100,
		MiniDumpWithPrivateReadWriteMemory     = 0x0200,
		MiniDumpWithoutOptionalData            = 0x0400,
	*/
	virtual void SetDumpType(long lDumpType) = 0;
	// 得到异常码
	virtual DWORD GetExceptionCode(PEXCEPTION_POINTERS pExceptionInfo) = 0;
	// 得到异常描述
	virtual LPCTSTR	GetExceptionDesc(PEXCEPTION_POINTERS pExceptionInfo) = 0;
	// 得到崩溃的模块文件偏移
	virtual DWORD	GetCrashFileOffset(PEXCEPTION_POINTERS pExceptionInfo) = 0;
	// 得到崩溃的模块文件名
	virtual LPCTSTR	GetCrashModuleName(PEXCEPTION_POINTERS pExceptionInfo) = 0;
	// 输出一些Dump信息到Log文件
	virtual void DumpLn(LPCTSTR szMsg) = 0;
	// 取得崩溃模块的产品版本号
	virtual void GetCrashModuleProductVer(PEXCEPTION_POINTERS pExceptionInfo,WORD pwVer[4]) = 0;
	// 取得当前调用栈
	virtual string GetCallStack(CONTEXT& Context,HANDLE hThread) = 0;
};
//------------------------------------------------------------------------
class CCrashDumpHelper
{
public:
	CCrashDumpHelper()
	{
		m_pCrashDump = 0;
		m_hDll = 0;

		Create();
	}

	~CCrashDumpHelper()
	{
		Close();
	}
	ICrashDump *operator->()
	{
		return m_pCrashDump;
	}
	void Close()
	{
		if (m_hDll)
		{
			FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}
	BOOL IsValid()
	{
		return m_pCrashDump != NULL;
	}
	BOOL Create()
	{
		if(!m_hDll)
			m_hDll = LoadDLL();
		
		if(!m_hDll)return FALSE;
		
		typedef ICrashDump* (*ProcCreateCrashDump)();
		
		ProcCreateCrashDump proc;
		proc = (ProcCreateCrashDump)GetProcAddress(m_hDll, "CreateCrashDump");
		
		if(proc == NULL)return FALSE;
		
		m_pCrashDump = proc();
		
		return m_pCrashDump != NULL;
	}
	
	HINSTANCE LoadDLL()
	{
#ifdef _DEBUG
		return LoadLibrary("FKCrashDump_D.dll");
#else
		return LoadLibrary("FKCrashDump.dll");
#endif
	}
private:
	ICrashDump *m_pCrashDump;
	HINSTANCE	m_hDll;
};
//------------------------------------------------------------------------