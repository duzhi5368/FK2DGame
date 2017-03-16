/**
*	created:		2012-6-25   1:37
*	filename: 		WndMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		窗口管理器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/IWndMgr.h"
#include "../../FKResManager/IFKResManager.h"
#include "../../FKFileSystem/IFKFileSystem.h"
#include "../../FKController/ValidationCode/WndMgrSignal.h"
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKTrace/IFKTrace.h"
//------------------------------------------------------------------------
#define  MAX_WINDOW_NUM	512
//------------------------------------------------------------------------
#define FKTraceColor		g_DefaultTrace.ThreadSafeDebugPrintColor
//------------------------------------------------------------------------
#ifndef	MY_TRACE_COLOR
#define MY_TRACE_COLOR		RGB(146,0,123)
#endif
//------------------------------------------------------------------------
#ifdef _DEBUG
	#define VerifyR(Expression, ReturnValue, Description ) \
		if (Expression) \
	{\
		FKTraceColor(MY_TRACE_COLOR, "错误 : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,Description);\
		return ReturnValue;\
	}
	#define AssertR(Expression, ReturnValue, Description ) \
		if (!(Expression)) \
	{\
		FKTraceColor(MY_TRACE_COLOR, "ERROR : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,Description);\
		return ReturnValue;\
	}
	#define Assert(Expression) \
		if (!(Expression))\
	{\
		FKTraceColor(MY_TRACE_COLOR, "错误 : %s[%d]\r\n\t%s\r\n",__FILE__,__LINE__,#Expression);\
		return 0;\
	}
	#ifdef FKTraceLn
	#undef FKTraceLn
	#endif
	#define	FKTraceLn			g_DefaultTrace.DebugPrintLn
#else
	#define VerifyR(Expression, ReturnValue, Description ) \
		if (Expression) \
	{\
		return ReturnValue;\
	}
	#define AssertR(Expression, ReturnValue, Description ) \
		if (!(Expression)) \
	{\
		return ReturnValue;\
	}
	#define Assert(Expression) \
		if (!(Expression))\
	{\
		return 0;\
	}
	#ifdef FKTraceLn
	#undef FKTraceLn
	#endif
	#define	FKTraceLn			__noop
#endif
//------------------------------------------------------------------------
struct IClientGlobal;
//------------------------------------------------------------------------
class CWndMgr : public IWndMgr
{
public:
	CWndMgr();
	virtual ~CWndMgr();
public:
	virtual void* CreateWnd(UINT uWndID);
	virtual BOOL  CloseWnd(UINT uWndID);
	virtual DWORD OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL  OnEvent(UINT uWndID, UINT uEventID, WPARAM wParam, LPARAM lParam);
	virtual BOOL  ShowWnd(UINT uWndID, int nCmdShow);
	virtual BOOL  IsWindowOpened(UINT uWndID);
	virtual ICursorData *GetCursorAttachData();
	virtual void  SetCursorAttach(IBitmap *pBitmap, ICursorData *pData);	
	virtual int  GetCursorAttachType() ;
	virtual void ClearCursorAttach();

	virtual BOOL  SubscibeMessage(UINT uWndID, UINT uEventID, 
		WMMESSAGE_CONNECTION *pConnect);

	virtual BOOL  UnSubscibeMessage(UINT uWndID, UINT uEventID, 
		WMMESSAGE_CONNECTION *pConnect);

	virtual void  PostWindowMessage(UINT uWndID, UINT uEventID, 
		WPARAM wParam, LPARAM lParam);

	// 注册某个窗口，在打开或关闭时，要收到WE_CW_ALLWND_SHOW消息，并且此窗口要继承IWndEnableCall	
	virtual void  RegTellShowWnd (UINT uWndID);

	// 取消注册某个窗口，在打开或关闭时，要收到WE_CW_ALLWND_SHOW消息。
	virtual void  UnRegTellShowWnd(UINT uWndID);

	virtual void  Release();

	//取得鼠标越过的窗口ID
	UINT GetWindowBelowCursor(POINT pt, int nFlags)
	{
		XWindow *pWndDesktop = m_WndSysHelper.GetGameDesktop();
		Assert(pWndDesktop);
		XWindow *pWnd = pWndDesktop->GetWindowBelowCursor(pt, nFlags);
		return pWnd == NULL ? 0 : pWnd->GetID();
	}

	//关闭所有功能性窗口
	BOOL CloseAllFunctionWnd();

	BOOL Init(IClientGlobal *pGlobal);

	//自定义的消息框
	virtual void  CustomMsgBox(LPCSTR pMsg, DWORD BtnNO1 = 1, 
		DWORD BtnNO2 = 0, 
		DWORD BtnNO3 = 0,
		IWndCallback * pCallBack = NULL,
		RECT * rcWnd = NULL,
		WPARAM wParam = 0,
		LPARAM lParam = 0,
		int    nCourse = POPCOURSE_UNKNOWN);

	//自定义的输入框
	virtual void  CustomInputBox(LPCSTR pMsg, 
		IWndCallback * pCallBack = NULL,
		DWORD  dwMaxLen = 6,
		BOOL   bDigitOnly = TRUE,
		BOOL   bNullFlag = FALSE,
		BOOL   bPasswordFlag = FALSE,
		int    nCourse = POPCOURSE_UNKNOWN);

	// 在打开的主窗口中, 关闭其中一个,如果没有打开着的主窗口, 则返回FALSE
	virtual BOOL HideOnePrimacyWnd(void);

private:
	BOOL ShowWndAux(UINT uWndID, int nCmdShow);
	void CloseAllWnd(); //关闭所有窗口
	//读取所有WNDSYS所需的TOOLTIP
	void __LoadWndToolTip(IResScript *pRescript);

	//声明哪些窗口是按主要的(按ESC可关闭) 
	void __DeclarePrimaryWnd(void);

public:
	IFileSystem		*m_pFileSystem;	//窗口系统的文件系统
	IFileSystem		*m_pFileSystemEx;
	IResScript		*m_pResScript;	//所有鼠标和窗口的脚本
	CWndSysHelper	m_WndSysHelper;	//窗口系统辅助类

private:
	CIResMgrHelper m_ResMgrHelper;
	CIFileSystemHelper m_FileSystemHelper;
	CWMMessageSender<256> m_MessageSender[MAX_WINDOW_NUM];
	//如果某个窗口在需要打开或关闭时，
	//要收到WE_CW_ALLWND_SHOW此消息，就必须注册
	DWORD m_WndTellShow[MAX_WINDOW_NUM+1];

	//主要的窗口列表, 每当按下ESC时, 在其中找一个来关闭
	std::list<int> m_lsPrimaryWnd;
};
//------------------------------------------------------------------------