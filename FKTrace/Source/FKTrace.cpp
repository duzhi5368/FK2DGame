/**
*	created:		2012-6-15   0:07
*	filename: 		FKTrace
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "../include/FKTrace.h"
//------------------------------------------------------------------------
CFKTrace g_DefaultTrace;
//------------------------------------------------------------------------
CTrace::CTrace()
{
	m_dwOutputStyle = 0L;
	m_hWndMain = NULL;
	m_pFileName	= NULL;
	m_hFile = NULL;
	m_hRichEdit = NULL;

	m_hMyRichEdit = NULL;
	m_hmodRichEdit = NULL;
	m_hWndTrace = NULL;
}
//------------------------------------------------------------------------
CTrace::~CTrace()
{
}
//------------------------------------------------------------------------
// 直接调试信息显示
BOOL CTrace::FKShowTrace(LPCSTR szMsg, COLORREF color, int nTextHeight, 
					   DWORD dwStyle, BOOL bLn, BOOL bShowTime)
{
	if (m_dwOutputStyle == 0)
		return FALSE;

	TRACE_MSG tm;
	// 字符串太长,切断!
	if(strlen(szMsg) >= MAX_TRACE_MSG_LEN-1)
	{
		char* pStr = new char[MAX_TRACE_MSG_LEN];
		memcpy(pStr, szMsg, MAX_TRACE_MSG_LEN-1);
		*(pStr + MAX_TRACE_MSG_LEN-1) = 0;
		tm = SetMsg(pStr, color, nTextHeight, dwStyle, bLn, bShowTime);
		delete pStr;
		pStr = NULL;
	}
	else
		tm = SetMsg(szMsg, color, nTextHeight, dwStyle, bLn, bShowTime);

	if (bLn)
		tm.str += "\r\n";

	// 输出到VC的Output窗口中
	if (m_dwOutputStyle & OS_OUTPUTDEBUGSTRING)
		ShowToOutput(tm);

	// 写入文件
	if (m_dwOutputStyle & OS_FILE)
		ShowToFile(tm);

	// 输出到主应用程序窗口中的RichEdit控件中
	if (m_dwOutputStyle & OS_RICHEDIT)
		ShowToRichEdit(m_hRichEdit, tm);

	// 输出到自创窗口中的RichEdit控件中
	if (m_dwOutputStyle & OS_WINDOW)
		ShowToRichEdit(m_hMyRichEdit, tm);
	
	return TRUE;
}
//------------------------------------------------------------------------
// 线程安全调试信息显示(对于OS_OUTPUTDEBUGSTRING和OS_FILE，同FKShowTrace())
BOOL CTrace::FKThreadSafeShowTrace(LPCSTR szMsg, COLORREF color, int nTextHeight, 
					   DWORD dwStyle, BOOL bLn, BOOL bShowTime)
{
	if (m_dwOutputStyle == 0)
		return FALSE;

	TRACE_MSG tm;

	// 字符串太长,切断!
	if(strlen(szMsg) >= MAX_TRACE_MSG_LEN -1)
	{
		char* pStr = new char[MAX_TRACE_MSG_LEN];
		memcpy(pStr, szMsg, MAX_TRACE_MSG_LEN-1);
		*(pStr + MAX_TRACE_MSG_LEN-1) = 0;
		tm = SetMsg(pStr, color, nTextHeight, dwStyle, bLn, bShowTime);
		delete pStr;
		pStr = NULL;
	}
	else
		tm = SetMsg(szMsg, color, nTextHeight, dwStyle, bLn, bShowTime);

	// 如果内外窗口句柄都为空，那么只可能输出到文件或VC调试输出窗口
	if (m_hWndTrace == NULL && m_hWndMain == NULL)
	{
		FKShowTrace(tm.str.c_str(), tm.crColor, tm.nHeight, tm.dwStyle, tm.bLn, tm.bShowTime);
		return TRUE;
	}

	m_mtxQueue.Lock();

	if(m_MessageQueue.size() >= MAX_THREAD_MESSAGE) // 消息队列满
	{
		TRACE_MSG tm1 = m_MessageQueue.front();
		m_MessageQueue.pop(); // 弹出第一条消息
		m_MessageQueue.push(tm); // 压入现在的消息

		//char str[MAX_TRACE_MSG_LEN];
		if (tm1.bLn)
			tm1.str += "\r\n";
		tm1.crColor = TC_WARNING;

		// 输出到主应用程序窗口中的RichEdit控件中
		if (m_dwOutputStyle & OS_RICHEDIT)
			ShowToRichEdit(m_hRichEdit, tm1, TRUE);

		// 输出到自创窗口中的RichEdit控件中
		if (m_dwOutputStyle & OS_WINDOW)
			ShowToRichEdit(m_hMyRichEdit, tm1, TRUE);
	}
	else
	{
		m_MessageQueue.push(tm);

		// 向内外窗口发送消息
		if (m_hWndTrace)
			PostMessage(m_hWndTrace, WM_THREAD_TRACE_EX, 0, 0);
		else if (m_hWndMain)
			PostMessage(m_hWndMain, WM_THREAD_TRACE_EX, 0, 0);
	}

	if (tm.bLn)
		tm.str += "\r\n";
	// 输出到VC的Output窗口中
	if (m_dwOutputStyle & OS_OUTPUTDEBUGSTRING)
		ShowToOutput(tm);

	// 写入文件
	if (m_dwOutputStyle & OS_FILE)
		ShowToFile(tm);

	m_mtxQueue.Unlock();
	return TRUE;
}
//------------------------------------------------------------------------
// 线程安全获得消息队列中的消息
BOOL CTrace::ThreadSafeGetTraceMsg()
{
	TRACE_MSG tm;
	m_mtxQueue.Lock();
	if(m_MessageQueue.empty())
		m_mtxQueue.Unlock();
	else
	{
		tm = m_MessageQueue.front();
		m_MessageQueue.pop();
		m_mtxQueue.Unlock();
	}

	if (m_dwOutputStyle == 0)
		return FALSE;

	if (tm.bLn)
		tm.str += "\r\n";
	// 输出到主应用程序窗口中的RichEdit控件中
	if (m_dwOutputStyle & OS_RICHEDIT)
		ShowToRichEdit(m_hRichEdit, tm, TRUE);

	// 输出到自创窗口中的RichEdit控件中
	if (m_dwOutputStyle & OS_WINDOW)
		ShowToRichEdit(m_hMyRichEdit, tm, TRUE);

	return TRUE;
}
//------------------------------------------------------------------------
// 线程安全获取消息队列中的消息，不主动显示
BOOL CTrace::ThreadSafeGetTraceMsgContext(char* szBuffer, int nSize, BOOL bRemoveMsg)
{
	if (!szBuffer) return FALSE;
	TRACE_MSG tm;
	m_mtxQueue.Lock();
	if(m_MessageQueue.empty())
	{
		m_mtxQueue.Unlock();
		return FALSE;
	}
	else
	{
		tm = m_MessageQueue.front();
		if (bRemoveMsg)
			m_MessageQueue.pop();
		m_mtxQueue.Unlock();
		if (tm.bLn)
			tm.str += "\r\n";
		lstrcpyn(szBuffer, tm.str.c_str(), nSize);
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
// 动态设置输出调试信息类型的接口，可以在程序运行过程中打开或关闭文件Trace
BOOL CTrace::SetOutputFile(bool bOutFile, LPCSTR szFileName)
{
	// 设置输出到文件时，必须指定文件名
	if(bOutFile == TRUE && !szFileName)
	{
		return false;
	}

	// 打开Trace到文件
	if(bOutFile == TRUE)
	{
		// 如果原来已经打开了文件，则先将原文件关闭
		if(m_hFile)
		{
			fclose(m_hFile);
			m_hFile = 0;
		}

		// 创建新文件
		if(!CreateTraceFile(szFileName))
			return false;

		m_dwOutputStyle |= OS_FILE;
	}
	else
	{
		// 如果有打开的文件句柄，则将其关闭
		if(m_hFile)
		{
			fclose(m_hFile);
			m_hFile = 0;
		}

		if(m_dwOutputStyle & OS_FILE)
		{
			m_dwOutputStyle &= (~OS_FILE);
		}
	}
	return true;
}
//------------------------------------------------------------------------
// 释放(包括自带窗口的销毁)
BOOL CTrace::Release()
{
	// 释放文件
	if (m_hFile)
	{
		fclose(m_hFile);
		m_hFile = NULL;
	}

	if (m_pFileName)
	{
		delete m_pFileName;
		m_pFileName = NULL;
	}

	if (m_hWndTrace)
	{
		::DestroyWindow(m_hWndTrace);
		m_hWndTrace = NULL;
	}

	// 释放riched32.dll
	if (m_hmodRichEdit)
	{
		::FreeLibrary(m_hmodRichEdit);
		m_hmodRichEdit = NULL;
	}

#ifdef _DEBUG
	OutputDebugString("释放FKTrace完成\r\n");
#endif

	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
// 设置调试选项
BOOL CTrace::SetOption(DWORD dwOutputStyle)
{
	m_dwOutputStyle = dwOutputStyle;
	return m_dwOutputStyle != 0;
}
//------------------------------------------------------------------------
// 初始化
BOOL CTrace::Init(HWND hWndMain, HWND hRichEdit, LPCSTR szFileName, HINSTANCE hInst, BOOL bExitAll)
{
#ifdef _DEBUG
	BOOL bCreateFile = FALSE, bCreateWindow = FALSE;
#endif

	m_bExitAll = bExitAll;
	m_hWndMain = hWndMain;	// 主应用程序窗口

	// 创建互斥体
	if (!m_mtxPrint.Create())
		return FALSE;

	// 创建互斥体
	if (!m_mtxQueue.Create())
		return FALSE;

	// 创建空的调试文件
	if (szFileName != NULL && (m_dwOutputStyle & OS_FILE))
#ifdef _DEBUG
		bCreateFile = 
#endif
		CreateTraceFile(szFileName);

	if (hWndMain != NULL && hRichEdit != NULL && (m_dwOutputStyle & OS_RICHEDIT))
		m_hRichEdit = hRichEdit;

	if (hInst != NULL && (m_dwOutputStyle & OS_WINDOW))
#ifdef _DEBUG
		bCreateWindow = 
#endif
		CreateTraceWindow(hInst);

	return TRUE;
}
//------------------------------------------------------------------------
// 创建内部自带窗口
BOOL CTrace::CreateTraceWindow(HINSTANCE hInst)
{
	LRESULT rs;
	static BOOL bIsCreated = FALSE;
	if (bIsCreated)
		return TRUE;

	m_hmodRichEdit = ::LoadLibrary("RICHED32.DLL");
	assert(m_hmodRichEdit != NULL);

	WNDCLASS            wc;
    wc.style		=	CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc	=	(WNDPROC)TraceWndProc;
    wc.cbClsExtra	=	0;
    wc.cbWndExtra	=	0;
    wc.hInstance	=	hInst;
    wc.hIcon		=	::LoadIcon(hInst,"WS_ICON");
    wc.hCursor		=	::LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=	(HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName	=	NULL;
    wc.lpszClassName=	"TRACE_WINDOW";
    RegisterClass(&wc);
	char szTitle[MAX_PATH];
	wsprintf(szTitle, "【FK调试窗口】 版本号 = %d", VERSION_QUERY_TRACE);

	RECT rcMain;
	SetRectEmpty(&rcMain);
	if (m_hWndMain != NULL)
		::GetWindowRect(m_hWndMain, &rcMain);

	m_hWndTrace = ::CreateWindow("TRACE_WINDOW", szTitle, 
					WS_OVERLAPPEDWINDOW, 
					rcMain.right, rcMain.top, 420, 768, 0, 0, hInst, 0);
	assert(m_hWndTrace != NULL);
	m_hMyRichEdit = ::CreateWindow("RICHEDIT", "",
		WS_CHILD |ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER | WS_VSCROLL | WS_TABSTOP,
		rcMain.right, rcMain.top, 420, 768,m_hWndTrace,0,hInst,0);
	assert(m_hMyRichEdit != NULL);
	
	assert(m_hWndTrace != NULL);
	::SetWindowLong(m_hWndTrace, GWL_USERDATA, (LONG)this);
	::ShowWindow(m_hWndTrace, SW_SHOWNORMAL);
	if (m_hWndMain != NULL)
	{
		::GetWindowRect(m_hWndTrace, &m_rcClient);

		::SetWindowPos(m_hWndTrace, HWND_TOP,rcMain.right, rcMain.top, m_rcClient.right - m_rcClient.left, 
			m_rcClient.bottom -  m_rcClient.top, SWP_SHOWWINDOW);
		::SetActiveWindow(m_hWndMain);
	}

	assert(::IsWindow(m_hMyRichEdit));

	CHARFORMAT2 cf;
	cf.cbSize = sizeof (CHARFORMAT);
	cf.dwMask = CFM_FACE | CFM_SIZE;
	cf.bCharSet = GB2312_CHARSET;
	cf.yHeight = 9*20;
	sprintf(cf.szFaceName, "MS Sans Serif");

	rs = ::SendMessage(m_hMyRichEdit, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
	rs = ::SendMessage(m_hMyRichEdit, EM_SETBKGNDCOLOR, FALSE, RGB(137,202,157) );
	long lEventMask = (long)::SendMessage(m_hMyRichEdit, EM_GETEVENTMASK, 0, 0L);
	rs = ::SendMessage(m_hMyRichEdit, EM_SETEVENTMASK, 0, lEventMask | ENM_LINK);
	rs = ::SendMessage(m_hMyRichEdit, EM_AUTOURLDETECT, TRUE,0);

	bIsCreated = TRUE;
	return TRUE;
}
//------------------------------------------------------------------------
// 创建存储调试信息的文件
BOOL CTrace::CreateTraceFile(LPCSTR szTraceFile)
{
	// 记录文件是否已经打开
	static BOOL bIsCreated = FALSE;
	if (bIsCreated)
		return TRUE;

	// 记录文件名
	int nLen = lstrlen(szTraceFile);
	m_pFileName = new char[nLen+1];
	lstrcpy(m_pFileName, szTraceFile);

	// 创建或清空文件内容
	m_hFile = fopen(szTraceFile, "wt");
	if (m_hFile == NULL)
	{
		OutputDebugString("错误：FKTrace创建调试信息文件，打开文件失败！\r\n");
		return FALSE;
	}

	bIsCreated = TRUE;
	return TRUE;
}
//------------------------------------------------------------------------
// 向RichEdit中添加信息
BOOL CTrace::AddToRichEdit(HWND hWnd, TRACE_MSG tm)
{
	if (hWnd == NULL || !::IsWindow(hWnd))
		return FALSE;

	CHARRANGE crOld;
	::SendMessage(hWnd, EM_EXGETSEL, 0, (LPARAM)&crOld);

	int nLen = (int)::SendMessage(hWnd, WM_GETTEXTLENGTH, NULL, NULL);
	int nStrLen = tm.str.size();
	CHARRANGE cr;
	if(nLen + nStrLen > MAX_RICHEDIT_MESSAGE_LEN)
	{
		cr.cpMin = 0;
		cr.cpMax = nLen + nStrLen - MAX_RICHEDIT_MESSAGE_LEN;
		::SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&cr);
		::SendMessage(hWnd, EM_REPLACESEL, (WPARAM)0, (LPARAM)"");
		nLen = (int)::SendMessage(hWnd, WM_GETTEXTLENGTH, NULL, NULL);
	}

	cr.cpMin = nLen;
	cr.cpMax = nLen;
	::SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&cr);

	CHARFORMAT2 cf;
	memset(&cf,0,sizeof(CHARFORMAT2));
	cf.dwMask = CFM_COLOR | CFM_CHARSET | CFM_SIZE 
		| CFM_BOLD | CFM_ITALIC | CFM_STRIKEOUT |  CFM_UNDERLINE | CFM_LINK | CFM_SHADOW;
	cf.dwEffects = tm.dwStyle;
	cf.bCharSet = GB2312_CHARSET;
	cf.crTextColor = tm.crColor;
	cf.yHeight = tm.nHeight * 20;
	cf.cbSize = sizeof(CHARFORMAT);
	::SendMessage(hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	::SendMessage(hWnd, EM_REPLACESEL, (WPARAM) 0, (LPARAM)tm.str.c_str());

	if(crOld.cpMax > crOld.cpMin)
	{
		::SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM)&crOld);
	}

	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize = sizeof(SCROLLINFO);
	ScrollInfo.fMask = SIF_ALL;
	::GetScrollInfo(hWnd, SB_VERT, &ScrollInfo);

	int nTotalLine = (int)::SendMessage(hWnd, EM_GETLINECOUNT, 0, 0);
	int nUpLine = 0 ;
	if(nTotalLine > 0 && ScrollInfo.nMax > 0 && ScrollInfo.nMax / nTotalLine > 0)
		nUpLine = (ScrollInfo.nMax - ScrollInfo.nPos - (ScrollInfo.nPage - 1)) / (ScrollInfo.nMax / nTotalLine);
	if(nUpLine > 0)
		::SendMessage(hWnd, EM_LINESCROLL, 0, nUpLine);
	
	return TRUE;
}
//------------------------------------------------------------------------
// 信息填充
TRACE_MSG CTrace::SetMsg(LPCSTR str, COLORREF color, int nHeight, DWORD dwStyle, BOOL bLn, BOOL bShowTime)
{
	TRACE_MSG tm;
	tm.str	= str;
	tm.crColor = color;
	tm.nHeight = nHeight;
	tm.dwStyle = dwStyle;
	tm.bLn = bLn;
	tm.bShowTime = bShowTime;
	return tm;
}
//------------------------------------------------------------------------
// 写文件
BOOL CTrace::WriteString(LPCSTR szMsg, int nSize)
{
	if(m_hFile == NULL)
	{
		return FALSE;
	}

	fwrite(szMsg, nSize, 1, m_hFile);
	fflush(m_hFile);
	return TRUE;
}
//------------------------------------------------------------------------
// 尺寸改变
BOOL CTrace::OnSize()
{
	::GetClientRect(m_hWndTrace, &m_rcClient);
	::ClientToScreen(m_hWndTrace, (POINT*)&m_rcClient.left);
	::ClientToScreen(m_hWndTrace, (POINT*)&m_rcClient.right);

	::SetWindowPos(m_hMyRichEdit, HWND_TOP,0,0, m_rcClient.right - m_rcClient.left, 
		m_rcClient.bottom -  m_rcClient.top, SWP_SHOWWINDOW);
	return TRUE;
}
//------------------------------------------------------------------------
// 获取日期时间字符串
BOOL CTrace::GetDateTimeString(char* str)
{
	time_t	long_time;
	struct tm* local_time;
	time(&long_time);
	local_time = localtime(&long_time);

	sprintf(str, "[%02d月%02d日 %02d:%02d:%02d] ", local_time->tm_mon+1, local_time->tm_mday,
		local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
	return TRUE;
}
//------------------------------------------------------------------------
// 显示到VC的调试窗口
BOOL CTrace::ShowToOutput(TRACE_MSG& tm)
{
	char strDateTime[48];
	if (tm.bShowTime)
	{
		GetDateTimeString(strDateTime);
		OutputDebugString(strDateTime);
	}

	OutputDebugString(tm.str.c_str());
	
	return TRUE;
}
//------------------------------------------------------------------------
// 显示到文件
BOOL CTrace::ShowToFile(TRACE_MSG& tm)
{
	char strDateTime[48];
	if (tm.bShowTime)
	{
		GetDateTimeString(strDateTime);
		WriteString(strDateTime, strlen(strDateTime));
	}

	WriteString(tm.str.c_str(), tm.str.size());
	
	return TRUE;
}
//------------------------------------------------------------------------
// 显示到RichEdit框
BOOL CTrace::ShowToRichEdit(HWND hWnd, TRACE_MSG& tm, BOOL bUseThread)
{
	if (hWnd == NULL)
		return FALSE;
	
	if (bUseThread)
		m_mtxPrint.Lock();
	
	char strDateTime[48];
	if (tm.bShowTime)
	{
		GetDateTimeString(strDateTime);
		AddToRichEdit(hWnd, SetMsg(strDateTime, 0));
	}

	AddToRichEdit(hWnd, tm);

	if (bUseThread)
		m_mtxPrint.Unlock();

	return TRUE;
}
//------------------------------------------------------------------------
// 可以退出
BOOL CTrace::IsExitAll()
{
	return m_bExitAll;
}
//------------------------------------------------------------------------
// 对话框消息处理过程
LRESULT CALLBACK TraceWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch( uMsg )
    {
        case WM_DESTROY:
			if (((CTrace*)::GetWindowLong(hWnd, GWL_USERDATA))->IsExitAll())
				 PostQuitMessage(0);
			break;
		case WM_CLOSE:
			if (!((CTrace*)::GetWindowLong(hWnd, GWL_USERDATA))->IsExitAll())
			{
				::ShowWindow(hWnd, SW_HIDE);
				return TRUE;
			}
			break;
		case WM_SIZE:
			((CTrace*)::GetWindowLong(hWnd, GWL_USERDATA))->OnSize();
			return TRUE;
		case WM_THREAD_TRACE_EX:
			((CTrace*)::GetWindowLong(hWnd, GWL_USERDATA))->ThreadSafeGetTraceMsg();
			if (::IsIconic(hWnd)) // 最小化则恢复
				::ShowWindow(hWnd, SW_RESTORE);
			else
				::ShowWindow(hWnd, SW_SHOW);
			break;
		case WM_QUERYOPEN:
			::ShowWindow(hWnd, SW_RESTORE);
			break;
        default:
			break;
    }
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//------------------------------------------------------------------------
BOOL libCreateTraceObject(DWORD dwExpectVersion, 
										DWORD dwOutputStyle, HWND hWndMain, 
										HWND hRichEdit, LPCSTR szFileName, 
										HINSTANCE hInst, BOOL bExitAll, 
										ITrace** ppTrace)
{
	// 封闭静态库包含数用
	return FALSE;


	// 版本校验
	// 首先判断客户传入的dwExpectVersion中的高位Word
	if (HIWORD(dwExpectVersion) != HIWORD(VERSION_QUERY_TRACE))
		return FALSE;		// 高Word不同
	
	// 然后判断dwExpectVersion中的低位Word
	if (LOWORD(dwExpectVersion) > LOWORD(VERSION_QUERY_TRACE))
		return FALSE;		// 客户默认的的低Word太高

	// 验证OS_RICHEDIT输出是否真正有效
	if (hWndMain == NULL || hRichEdit == NULL || !(dwOutputStyle & OS_RICHEDIT))
	{
		hWndMain = NULL;
		hRichEdit = NULL;
		if (dwOutputStyle & OS_RICHEDIT)
			dwOutputStyle &= ~OS_RICHEDIT;
	}

	// 验证OS_FILE输出是否真正有效
	if (szFileName == NULL || !(dwOutputStyle & OS_FILE) || *szFileName == 0)
	{
		szFileName = NULL;
		if (dwOutputStyle & OS_FILE)
			dwOutputStyle &= ~OS_FILE;
	}

	// 验证OS_WINDOW输出是否真正有效
	if (hInst == NULL || !(dwOutputStyle & OS_WINDOW))
	{

		if (dwOutputStyle & OS_WINDOW)
			dwOutputStyle &= ~OS_WINDOW;
	}

	CTrace* pTrace = new CTrace();
	if (pTrace == NULL)
		return FALSE;

	if (hInst == NULL)
		hInst = ::GetModuleHandle(NULL);

	pTrace->SetOption(dwOutputStyle);
	pTrace->Init(hWndMain, hRichEdit, szFileName, hInst, bExitAll);

	*ppTrace = static_cast<ITrace*>(pTrace);
#ifdef _DEBUG
	pTrace->FKShowTrace("创建 FKTrace 对象成功。\r\n");
#endif
	return *ppTrace != NULL;
}
//------------------------------------------------------------------------
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			_CrtDumpMemoryLeaks();
			break;
    }
    return TRUE;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateTraceObject(DWORD dwExpectVersion, 
										DWORD dwOutputStyle, HWND hWndMain, 
										HWND hRichEdit, LPCSTR szFileName, 
										HINSTANCE hInst, BOOL bExitAll, 
										ITrace** ppTrace)
{
	// 版本校验
	// 首先判断客户传入的dwExpectVersion中的高位Word
	if (HIWORD(dwExpectVersion) != HIWORD(VERSION_QUERY_TRACE))
		return FALSE;		// 高Word不同
	
	// 然后判断dwExpectVersion中的低位Word
	if (LOWORD(dwExpectVersion) > LOWORD(VERSION_QUERY_TRACE))
		return FALSE;		// 客户默认的的低Word太高

	// 验证OS_RICHEDIT输出是否真正有效
	if (hWndMain == NULL || hRichEdit == NULL || !(dwOutputStyle & OS_RICHEDIT))
	{
		hRichEdit = NULL;
		if (dwOutputStyle & OS_RICHEDIT)
			dwOutputStyle &= ~OS_RICHEDIT;
	}

	// 验证OS_FILE输出是否真正有效
	if (szFileName == NULL || !(dwOutputStyle & OS_FILE) || *szFileName == 0)
	{
		szFileName = NULL;
		if (dwOutputStyle & OS_FILE)
			dwOutputStyle &= ~OS_FILE;
	}

	CTrace* pTrace = new CTrace();
	if (pTrace == NULL)
		return FALSE;

	if (hInst == NULL)
		hInst = ::GetModuleHandle(NULL);

	pTrace->SetOption(dwOutputStyle);
	pTrace->Init(hWndMain, hRichEdit, szFileName, hInst, bExitAll);

	*ppTrace = static_cast<ITrace*>(pTrace);
#ifdef _DEBUG
	pTrace->FKShowTrace("创建 FKTrace 对象成功。\r\n");
#endif
	return *ppTrace != NULL;
}
//------------------------------------------------------------------------