/**
*	created:		2012-6-14   23:41
*	filename: 		IFKTrace
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
/*
	使用说明：
	1： Main所在文件中包含本文件头
	2： 注意在运行文件同级目录下放置对应的动态链接库。
	DEBUG版本放置	FKTrace_D.dll 
	RELEASE版本放置	FKTrace.dll
	3： 在Main前声明全局变量 CFKTrace g_Trace;
	4： 调用 Create 函数，并传入相应参数 Create 函数参数下面将详细讲解。
	5： 若 Create 时标示了使用窗口模式 OS_WINDOW。则需要 WinMain() 内拥有下列代码：
		<code>
			MSG msg;
			while (GetMessage (&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		</code>
		若您代码中已有类似功能代码，则可不写。
	6：若使用了 OS_RICHEDIT 模式，请在 Create 之前加载 RichEdit32.dll ,例代码如下
		<code>
			HINSTANCE hDll = LoadLibrary("RICHED32.DLL");
			if( NULL == hDll )
			{
				// DoSth 或者 Create 另外一种不带RichEdit的模式
			}
		</code>
	7：你可以通过下面代码定制自己喜欢的Trace的颜色
		<code>
			#define MY_TRACE_COLOR		RGB(146,0,123)
		</code>
	8：你可以使用 g_Trace 提供的大量接口参数实现Log输出。
	
	完成。

	注意：
	1：	当前版本尚未支持Unicode字节。请将应用程序改为多字节。
	2： ThreadSafeShowTraceXXXX 系列函数 和 ShowTraceXXXX 系列函数区别是线程安全性区别
	3： ShowTrace			最普通的输出
		ShowTraceLn			自带换行符的输出
		ShowTraceWithTime	带时间的输出
		ShowTraceLnWithTime	带时间的输出，每次输出自带换行符
		其他 ThreadSafeShowTraceXXXX 系列 以及 DebugPrintXXX 系列同上。
	4： 本文件可不支持被包含于其他Lib或Dll内。若需要该功能，可联系FreeKnight获取更高版本
		
	若有不便，可联系FreeKnight  
	http://freeknigt.jimdo.com
*/
//------------------------------------------------------------------------
// Create 函数详细说明
//------------------------------------------------------------------------
/*
参数说明：
@In：
	hInst			:	传入你的外部程序的hInst
	dwOutputStyle	:	输出类型( 参考下面的 输出类型(Output Style)定义  )
	hWndMain		:	若主应用窗口中没有 RichEdit 或者你不准备使用 OS_RICHEDIT 类型，
						则可传入NULL。
						主应用程序窗口句柄,设置此句柄能绑定Trace窗口,同时能使主应用
						程序窗口上的RichEdit控件输出Trace信息,当然必须保证hRichEdit
						和dwOutputStyle同时传入有效值,当hWndMain=NULL时,失去上面两个
						特性.
	hRichEdit		:	若主应用窗口中没有 RichEdit 或者你不准备使用 OS_RICHEDIT 类型，
						则可传入NULL。
						如果输出类别中有 OS_RICHEDIT, 此处则给出主应用程序中用于显示
						Trace信息的RichEdit控件的句柄,并同时保证hWndMain有效
	szFileName		:	如果输出类别中有 OS_FILE, 此处则给出 Log 输出的相对文件名(含路径)
	bExitAll		:	标志Trace窗口关闭时,是否终止主程序的消息循环并退出主程序,防止有些
						创建了Trace窗口但本身无窗口的程序在关闭Trace窗口后没法退出进程
@return：
	BOOL			:	成功返回TRUE，否则返回FALSE

@Comment：
	其中的ITrace*可通过g_Trace.GetTraceObject()获得
*/
//------------------------------------------------------------------------
// FreeKnight 推荐部分代码方式：
//------------------------------------------------------------------------
/*
	1： 定义简易宏，例如
	#define FKTRACE(a) g_Trace.ThreadSafeShowTraceLnWithTime(a)

	2： DEBUG 和 RELEASE 模式分离，例如
	#ifdef _DEBUG
		#define FKTRACE(a) g_Trace.ThreadSafeShowTraceLnWithTime(a)
	#else
		#define FKTRACE(a) void(0)
	#endif

	3： 分级处理，例如
	#define TRACE_LEVEL_1		// 普通级别
	#define TRACE_LEVEL_2		// 高级级别

	#if defined(TRACE_LEVEL_1) || defined(TRACE_LEVEL_2)
		#define FKTRACE(a)	g_Trace.ThreadSafeShowTraceWithTime(a)
	#else
		#define FKTRACE(a) void(0)
	#endif

	#if defined(TRACE_LEVEL_2)
		#define FKTRACEEX(a) g_Trace.ThreadSafeShowTraceLnWithTime(a)
	#else
		#define FKTRACE(a) void(0)
	#endif
	当对外发布时，可去除对应的宏定义

	4：分色分级处理，例如
	#define FKINFO(a)		g_Trace.ThreadSafeShowTrace(a, RGB(255, 255, 255) )
	#define FKWARNING(a)	g_Trace.ThreadSafeShowTraceWithTime(a)
	#define FKERROR(a)		g_Trace.ThreadSafeShowTraceLnWithTime(a, RGB(255, 0, 0 ), 12, TS_ITALIC )

	5：组合成其他函数处理以及颜色自定义处理，例如
	#define MY_TRACE_COLOR		RGB(255,0,0)
	#define AssertR(Expression, ReturnValue, Description ) \
		if (!(Expression)) \
		{\
			TraceColor(MY_TRACE_COLOR, "ERROR : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,Description);\
			return ReturnValue;\
		}
	#define Assert(Expression) \
		if (!(Expression))\
		{\
			TraceColor(MY_TRACE_COLOR, "ERROR : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,#Expression);\
			return 0;\
		}
*/
//------------------------------------------------------------------------
#include <Windows.h>
#include <richedit.h>
#include <stdio.h>
//------------------------------------------------------------------------
// Trace当前版本号
#define VERSION_QUERY_TRACE			0x0101
//------------------------------------------------------------------------
// 一些常量定义
#ifdef _DEBUG
#define MAX_RICHEDIT_MESSAGE_LEN	(60 * 1024)		// RichEdit中最大容纳长度
#else
#define MAX_RICHEDIT_MESSAGE_LEN	(32 * 1024)
#endif
#define MAX_TRACE_MSG_LEN			2048			// 每次Trace的最大长度
#define MAX_THREAD_MESSAGE			4000			// 最大线程消息容纳数
#define WM_THREAD_TRACE_EX			(WM_USER+0x127)	// 线程安全Trace消息
//------------------------------------------------------------------------
// 输出类型(Output Style)定义
#define OS_OUTPUTDEBUGSTRING		0x00000001L		// OutputDebugString输出
#define OS_FILE						0x00000002L		// 输出到文件
#define OS_RICHEDIT					0x00000004L		// 输出到RichEdit
#define OS_WINDOW					0x00000008L		// 输出到窗口
//------------------------------------------------------------------------
// 系统Trace的颜色
#define TC_DEFAULT					RGB(0,0,255)	// 默认颜色
#define TC_WARNING					RGB(106,79,154)	// 警告颜色
#define TC_ERROR					RGB(255,0,0)	// 错误颜色
#define TC_SYSTEMMESSAGE			RGB(155,65,14)	// 系统消息颜色
//------------------------------------------------------------------------
// 系统Trace的字体大小
#define TH_DEFAULT					9				// 默认9号字
#define TH_SYSTEMMESSAGE			12				// 系统消息12号字
//------------------------------------------------------------------------
// 系统Trace字体样式
#define TS_DEFAULT					0				// 默认样式
#define TS_SYSTEMMESSAGE			TS_BOLD			// 系统消息为粗体
//------------------------------------------------------------------------
// RichEdit属性
#define TS_BOLD						0x00000001
#define TS_ITALIC					0x00000002
#define TS_UNDERLINE				0x00000004
#define TS_STRIKEOUT				0x00000008
#define TS_PROTECTED				0x00000010
#define TS_LINK						0x00000020		/* Exchange hyperlink extension */
#define TS_SIZE						0x80000000
#define TS_COLOR					0x40000000
#define TS_FACE						0x20000000
#define TS_OFFSET					0x10000000
#define TS_CHARSET					0x08000000
//------------------------------------------------------------------------
// 接口定义
struct ITrace
{
	// 直接调试信息显示
	// 说明：不带换行回车符("\r\n"), dwStyle可选值: TS_BOLD,TS_ITALIC ... 见上面的宏定义
	virtual BOOL FKShowTrace(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
							int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
							BOOL bLn =	FALSE, BOOL bShowTime = FALSE) = 0;
	// 线程安全调试信息显示(通过消息机制实现)
	// 说明：不带换行回车符("\r\n"), dwStyle可选值: TS_BOLD,TS_ITALIC ... 见上面的宏定义
	virtual BOOL FKThreadSafeShowTrace(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
							int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
							BOOL bLn = FALSE, BOOL bShowTime = FALSE) = 0;
	// 线程安全获取消息队列中的消息并显示出来
	virtual BOOL ThreadSafeGetTraceMsg() = 0;
	// 获取内部创建的窗口句柄
	virtual HWND GetTraceWindow() = 0;
	// 释放(包括自带窗口的销毁)
	virtual BOOL Release() = 0;
	// 线程安全获取消息队列中的消息，不主动显示
	// 这里void* szBuffer对应trace文本，nSize为传入的文本缓冲大小
	// BOOL bRemoveMsg;表示取完消息后是否同时删除消息队列里面的消息，如果不删除消息
	// 请同时调用ThreadSafeGetTraceMsg函数
	virtual BOOL ThreadSafeGetTraceMsgContext(char* szBuffer, int nSize, BOOL bRemoveMsg=FALSE) = 0;
	// 动态设置输出调试信息类型的接口，可以在程序运行过程中打开或关闭文件Trace
	virtual BOOL SetOutputFile(bool bOutFile, LPCSTR szFileName = 0) = 0;
};
//------------------------------------------------------------------------
class CFKTrace
{
public:
	typedef BOOL (*ProcCreateTraceObject)(DWORD dwExpectVersion, 
									DWORD dwOutputStyle, HWND hMainWnd, 
									HWND hRichEdit, LPCSTR szFileName, 
									HINSTANCE hInst, BOOL bExitAll, 
									ITrace** ppTrace);
	CFKTrace()
	{
		m_hdll = NULL;
		m_pTrace = NULL;
		m_Flags = FALSE;
	}
	~CFKTrace()
	{
		Close();
	}
	// 函数参数说明请参见上方详细描述
	BOOL Create(HINSTANCE hInst, DWORD dwOutputStyle = OS_WINDOW | OS_FILE, HWND hWndMain = NULL, 
					HWND hRichEdit = NULL, LPCSTR szFileName = NULL, BOOL bExitAll = FALSE)
	{
		Close();

		try
		{
			if(m_hdll == NULL)
			{
#ifdef _DEBUG
				m_hdll = ::LoadLibrary("FKTrace_D.dll");
				if(m_hdll == NULL)
					throw "加载 FKTraceD.dll 失败！";
#else
				m_hdll = ::LoadLibrary("FKTrace.dll");
				if(m_hdll == NULL)
					throw "加载 FKTrace.dll 失败！";
#endif
			}
			ProcCreateTraceObject proc;
			proc = (ProcCreateTraceObject)GetProcAddress(m_hdll, "CreateTraceObject");
			if(proc == NULL)
				throw "无法获取CreateTraceObject函数地址！";

			if(!proc(VERSION_QUERY_TRACE, dwOutputStyle, hWndMain, hRichEdit, szFileName, m_hdll, bExitAll, &m_pTrace))
				throw "创建Trace对象失败！";

			return m_pTrace != NULL;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return FALSE;
		}
		catch(...)
		{
			OutputDebugString("创建Trace对象出现异常！\r\n");
			return FALSE;
		}
	}
	//------------------------------------------------------------------------
	BOOL Create(ITrace* pTrace)
	{
		if (m_pTrace == NULL)
			m_pTrace = pTrace;
		m_Flags = TRUE;
		return m_pTrace != NULL;
	}
	BOOL ThreadSafeGetTraceMsg()
	{
		if (m_pTrace == NULL)
			return FALSE;
		return m_pTrace->ThreadSafeGetTraceMsg();
	}
	HWND GetTraceWindow()
	{
		if (m_pTrace == NULL)
			return NULL;
		return m_pTrace->GetTraceWindow();
	}
	ITrace* GetTraceObject()
	{
		return m_pTrace;
	}
	void Close()
	{
		if (m_Flags) return;
		if (m_pTrace)
		{
			m_pTrace->Release();
			m_pTrace = NULL;
		}
		if(m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}
	BOOL ShowTrace(LPCSTR szMsg, COLORREF color = TC_DEFAULT, int nTextHeight = TH_DEFAULT, 
								DWORD dwStyle = TS_DEFAULT)
	{
		if (m_pTrace == NULL)
		{
			OutputDebugString(szMsg);
			return TRUE;
		}
		return m_pTrace->FKShowTrace(szMsg,color,nTextHeight,dwStyle,FALSE,FALSE);
	}
	BOOL ShowTraceWithTime(LPCSTR szMsg, COLORREF color = TC_DEFAULT, int nTextHeight = TH_DEFAULT, 
								DWORD dwStyle = TS_DEFAULT)
	{
		if (m_pTrace == NULL)
		{
			OutputDebugString(szMsg);
			return TRUE;
		}
		return m_pTrace->FKShowTrace(szMsg,color,nTextHeight,dwStyle,FALSE,TRUE);
	}
	BOOL ShowTraceLn(LPCSTR szMsg, COLORREF color = TC_DEFAULT, int nTextHeight = TH_DEFAULT, 
								DWORD dwStyle = TS_DEFAULT)
	{
		if (m_pTrace == NULL)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return TRUE;
		}
		return 	m_pTrace->FKShowTrace(szMsg,color,nTextHeight,dwStyle,TRUE,FALSE);
	}
	BOOL ShowTraceLnWithTime(LPCSTR szMsg, COLORREF color = TC_DEFAULT, int nTextHeight = TH_DEFAULT, 
								DWORD dwStyle = TS_DEFAULT)
	{
		if (m_pTrace == NULL)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return TRUE;
		}
		return m_pTrace->FKShowTrace(szMsg,color,nTextHeight,dwStyle,TRUE,TRUE);
	}
	void DebugPrint(LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL)
			OutputDebugString(szBuffer);
		else
			m_pTrace->FKShowTrace(szBuffer,TC_DEFAULT,TH_DEFAULT,TS_DEFAULT,FALSE,FALSE);

		va_end(args);
	}
	void DebugPrintWithTime(LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL)
			OutputDebugString(szBuffer);
		else
			m_pTrace->FKShowTrace(szBuffer,TC_DEFAULT,TH_DEFAULT,TS_DEFAULT,FALSE,TRUE);

		va_end(args);
	}
	void DebugPrintLn(LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}
		
		if (m_pTrace == NULL){
			OutputDebugString(szBuffer);OutputDebugString("\r\n");}
		else
			m_pTrace->FKShowTrace(szBuffer,TC_DEFAULT,TH_DEFAULT,TS_DEFAULT,TRUE,FALSE);

		va_end(args);
	}
	void DebugPrintLnWithTime(LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL){
			OutputDebugString(szBuffer);OutputDebugString("\r\n");}
		else
			m_pTrace->FKShowTrace(szBuffer,TC_DEFAULT,TH_DEFAULT,TS_DEFAULT,TRUE,TRUE);

		va_end(args);
	}
	void DebugPrintColor(COLORREF color, LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);
		
		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];
		
		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
#ifdef _DEBUG
			m_pTrace->FKShowTrace("警告：字符串过长！\r\n", TC_ERROR);
#endif
			// 字符串太长,切断!
			if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
				szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}
		
		if (m_pTrace == NULL)
			OutputDebugString(szBuffer);
		else
			m_pTrace->FKShowTrace(szBuffer,color,TH_DEFAULT,TS_DEFAULT,FALSE,FALSE);
		
		va_end(args);
	}
	void DebugPrintColorWithTime(COLORREF color, LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);
		
		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];
		
		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
#ifdef _DEBUG
			m_pTrace->FKShowTrace("警告：字符串过长！\r\n", TC_ERROR);
#endif
			// 字符串太长,切断!
			if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
				szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}
		
		if (m_pTrace == NULL)
			OutputDebugString(szBuffer);
		else
			m_pTrace->FKShowTrace(szBuffer,color,TH_DEFAULT,TS_DEFAULT,FALSE,TRUE);
		
		va_end(args);
	}
	void DebugPrintColorLn(COLORREF color, LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);
		
		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];
		
		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
#ifdef _DEBUG
			m_pTrace->FKShowTrace("警告：字符串过长！\r\n", TC_ERROR);
#endif
			// 字符串太长,切断!
			if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
				szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}
		
		if (m_pTrace == NULL){
			OutputDebugString(szBuffer);OutputDebugString("\r\n");}
		else
			m_pTrace->FKShowTrace(szBuffer,color,TH_DEFAULT,TS_DEFAULT,TRUE,FALSE);
		
		va_end(args);
	}
	void DebugPrintColorLnWithTime(COLORREF color, LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);
		
		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];
		
		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
			#ifdef _DEBUG
			m_pTrace->FKShowTrace("警告：字符串过长！\r\n", TC_ERROR);
			#endif
			// 字符串太长,切断!
			if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
				szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}
		
		if (m_pTrace == NULL){
			OutputDebugString(szBuffer);OutputDebugString("\r\n");}
		else
			m_pTrace->FKShowTrace(szBuffer,color,TH_DEFAULT,TS_DEFAULT,TRUE,TRUE);
		
		va_end(args);
	}
	BOOL ThreadSafeShowTrace(LPCSTR szMsg, COLORREF color = TC_DEFAULT, int nTextHeight = TH_DEFAULT, 
								DWORD dwStyle = TS_DEFAULT)
	{
		if (m_pTrace == NULL)
		{
			OutputDebugString(szMsg);
			return TRUE;
		}
		return m_pTrace->FKThreadSafeShowTrace(szMsg, color, nTextHeight, dwStyle,FALSE,FALSE);
	}
	BOOL ThreadSafeShowTraceWithTime(LPCSTR szMsg, COLORREF color = TC_DEFAULT, int nTextHeight = TH_DEFAULT, 
								DWORD dwStyle = TS_DEFAULT)
	{
		if (m_pTrace == NULL)
		{
			OutputDebugString(szMsg);
			return TRUE;
		}
		return m_pTrace->FKThreadSafeShowTrace(szMsg, color, nTextHeight, dwStyle,FALSE,TRUE);
	}
	BOOL ThreadSafeShowTraceLn(LPCSTR szMsg, COLORREF color = TC_DEFAULT, int nTextHeight = TH_DEFAULT, 
								DWORD dwStyle = TS_DEFAULT)
	{
		if (m_pTrace == NULL)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return TRUE;
		}
		return m_pTrace->FKThreadSafeShowTrace(szMsg, color, nTextHeight, dwStyle,TRUE,FALSE);
	}
	BOOL ThreadSafeShowTraceLnWithTime(LPCSTR szMsg, COLORREF color = TC_DEFAULT, int nTextHeight = TH_DEFAULT, 
								DWORD dwStyle = TS_DEFAULT)
	{
		if (m_pTrace == NULL)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return TRUE;
		}

		return m_pTrace->FKThreadSafeShowTrace(szMsg, color, nTextHeight, dwStyle,TRUE,TRUE);
	}
	void ThreadSafeDebugPrint(LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKThreadSafeShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL)
			OutputDebugString(szBuffer);
		else
			m_pTrace->FKThreadSafeShowTrace(szBuffer,TC_DEFAULT,TH_DEFAULT,TS_DEFAULT,FALSE,FALSE);

		va_end(args);
	}
	void ThreadSafeDebugPrintLn1(LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKThreadSafeShowTrace("警告：字符串过长 !\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL){
			OutputDebugString(szBuffer);OutputDebugString("\r\n");}
		else
			m_pTrace->FKThreadSafeShowTrace(szBuffer,TC_DEFAULT,TH_DEFAULT,TS_DEFAULT);

		va_end(args);
	}
	void ThreadSafeDebugPrintWithTime(LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKThreadSafeShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL)
			OutputDebugString(szBuffer);
		else
			m_pTrace->FKThreadSafeShowTrace(szBuffer,TC_DEFAULT,TH_DEFAULT,TS_DEFAULT,FALSE,TRUE);

		va_end(args);
	}
	void ThreadSafeDebugPrintLn(LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKThreadSafeShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL){
			OutputDebugString(szBuffer);OutputDebugString("\r\n");}
		else
			m_pTrace->FKThreadSafeShowTrace(szBuffer,TC_DEFAULT,TH_DEFAULT,TS_DEFAULT,TRUE,FALSE);

		va_end(args);
	}
	void ThreadSafeDebugPrintLnWithTime(LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKThreadSafeShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL){
			OutputDebugString(szBuffer);OutputDebugString("\r\n");}
		else
			m_pTrace->FKThreadSafeShowTrace(szBuffer,TC_DEFAULT,TH_DEFAULT,TS_DEFAULT,TRUE,TRUE);

		va_end(args);
	}
	void ThreadSafeDebugPrintColor(COLORREF color, LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKThreadSafeShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL)
			OutputDebugString(szBuffer);
		else
			m_pTrace->FKThreadSafeShowTrace(szBuffer,color,TH_DEFAULT,TS_DEFAULT,FALSE,FALSE);

		va_end(args);
	}
	void ThreadSafeDebugPrintColorWithTime(COLORREF color, LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKThreadSafeShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL)
			OutputDebugString(szBuffer);
		else
			m_pTrace->FKThreadSafeShowTrace(szBuffer,color,TH_DEFAULT,TS_DEFAULT,FALSE,TRUE);

		va_end(args);
	}
	void ThreadSafeDebugPrintColorLn(COLORREF color, LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKThreadSafeShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL){
			OutputDebugString(szBuffer);OutputDebugString("\r\n");}
		else
			m_pTrace->FKThreadSafeShowTrace(szBuffer,color,TH_DEFAULT,TS_DEFAULT,TRUE,FALSE);

		va_end(args);
	}
	void ThreadSafeDebugPrintColorLnWithTime(COLORREF color, LPCSTR lpszFormat, ...)
	{
		va_list args;
		va_start(args, lpszFormat);

		int nBuf;char szBuffer[MAX_TRACE_MSG_LEN];

		nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		if(nBuf < 0)
		{
		#ifdef _DEBUG
				m_pTrace->FKThreadSafeShowTrace("警告：字符串过长！\r\n", TC_ERROR);
		#endif
		// 字符串太长,切断!
		if(strlen(szBuffer) >= MAX_TRACE_MSG_LEN-1)
			szBuffer[MAX_TRACE_MSG_LEN-1] = 0;
		}

		if (m_pTrace == NULL){
			OutputDebugString(szBuffer);OutputDebugString("\r\n");}
		else
			m_pTrace->FKThreadSafeShowTrace(szBuffer,color,TH_DEFAULT,TS_DEFAULT,TRUE,TRUE);

		va_end(args);
	}
public:
	ITrace*			m_pTrace;
private:
	HINSTANCE		m_hdll;
	BOOL			m_Flags;
};
//------------------------------------------------------------------------
// 对外提供的标准Trace
extern CFKTrace g_DefaultTrace;
//------------------------------------------------------------------------