/**
*	created:		2012-6-14   23:54
*	filename: 		FKTrace
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifdef _DEBUG
#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
#include <windows.h>
#include <richedit.h>
#include <queue>
#include <string>
#include "../IFKTrace.h"
#include "Mutex.h"
//------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------
// 全局函数声明
LRESULT CALLBACK TraceWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//------------------------------------------------------------------------
// 消息结构
typedef struct _TRACE_MSG
{
public:
	string		str;		// Trace文本
	COLORREF	crColor;	// Trace颜色
	int			nHeight;	// Trace字号
	DWORD		dwStyle;	// Trace字样
	BOOL		bLn;		// 回车换行
	BOOL		bShowTime;	// 显示时间

	_TRACE_MSG()
	{
		str.clear();
		crColor=TC_DEFAULT;
		nHeight=TH_DEFAULT;
		dwStyle=TS_DEFAULT;
		bLn=bShowTime=0;
	}

	~_TRACE_MSG()
	{
		str.erase(str.begin(),str.end());
		crColor=TC_DEFAULT;
		nHeight=TH_DEFAULT;
		dwStyle=TS_DEFAULT;
		bLn=bShowTime=0;
	}
} TRACE_MSG;
//------------------------------------------------------------------------
class CTrace : public ITrace
{
public:
	CTrace();
	~CTrace();
public:
	// 直接调试信息显示
	virtual BOOL FKShowTrace(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
		int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
		BOOL bLn = FALSE, BOOL bShowTime = FALSE);
	// 线程安全调试信息显示
	virtual BOOL FKThreadSafeShowTrace(LPCSTR szMsg, COLORREF color = TC_DEFAULT, 
		int nTextHeight = TH_DEFAULT, DWORD dwStyle = TS_DEFAULT, 
		BOOL bLn = FALSE, BOOL bShowTime = FALSE);
	// 线程安全获得消息队列中的消息
	virtual BOOL ThreadSafeGetTraceMsg();
	// 得到自带窗口的句柄
	virtual HWND GetTraceWindow(){return m_hWndTrace;}
	// 释放(包括自带窗口的销毁)
	virtual BOOL Release();
	// 线程安全获取消息队列中的消息，不主动显示
	virtual BOOL ThreadSafeGetTraceMsgContext(char* szBuffer, int nSize, BOOL bRemoveMsg=FALSE);

	// 动态设置输出调试信息类型的接口，可以在程序运行过程中打开或关闭文件Trace
	virtual BOOL SetOutputFile(bool bOutFile, LPCSTR szFileName = 0);
	// 设置调试选项
	BOOL SetOption(DWORD dwOutputStyle);
	// 初始化
	BOOL Init(HWND hWndMain, HWND hRichEdit, LPCSTR szFileName, HINSTANCE hInst, BOOL bExitAll = FALSE);
	// 创建内部自带窗口
	BOOL CreateTraceWindow(HINSTANCE hInst);
	// 创建存储调试信息的文件
	BOOL CreateTraceFile(LPCSTR szTraceFile);
	// 向RichEdit中添加信息
	BOOL AddToRichEdit(HWND hWnd, TRACE_MSG tm);
	// 写文件
	BOOL WriteString(LPCSTR szMsg, int nSize);
	// 尺寸改变
	BOOL OnSize();
	// 可以退出
	BOOL IsExitAll();
	// 信息填充
	TRACE_MSG SetMsg(LPCSTR str, COLORREF color = TC_DEFAULT, int nHeight = TH_DEFAULT, 
		DWORD dwStyle = TS_DEFAULT, BOOL bLn = FALSE, BOOL bShowTime = FALSE);
	// 得到主应用程序的窗口句柄
	HWND GetMainWindow(){return m_hWndMain;}
	// 获取日期时间字符串
	BOOL GetDateTimeString(char* str);
	// 显示到VC的调试窗口
	BOOL ShowToOutput(TRACE_MSG& tm);
	// 显示到文件
	BOOL ShowToFile(TRACE_MSG& tm);
	// 显示到RichEdit框
	BOOL ShowToRichEdit(HWND hWnd, TRACE_MSG& tm, BOOL bUseThread = FALSE);
public:
	DWORD			m_dwOutputStyle;		// 调试类型
	HWND			m_hWndMain;				// 应用程序主窗口句柄
	char*			m_pFileName;			// 供存储调试信息的文件名(包含路径)
	FILE*			m_hFile;				// 供存储调试信息的文件句柄
	HWND			m_hRichEdit;			// 供显示调试信息用的应用程序RichEdit控件句柄
	HWND			m_hMyRichEdit;			// 内部创建的窗口中的RichEdit控件句柄
	HMODULE			m_hmodRichEdit;			// 调用Riched32.dll后的模块句柄
	HWND			m_hWndTrace;			// 内部创建的窗口句柄
	CMutex			m_mtxPrint, m_mtxQueue;	// 互斥体,分别用于直接调试显示和线程安全调试显示
	queue<TRACE_MSG> m_MessageQueue;		// 消息队列
	RECT			m_rcClient;				// 渲染区
	BOOL			m_bExitAll;				// 是否允许退出，并向主程序发送退出消息
};
//------------------------------------------------------------------------