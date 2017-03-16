/**
*	created:		2012-6-22   23:40
*	filename: 		IWndMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "WndSys.h"
//------------------------------------------------------------------------
// 改变游戏鼠标
#define WM_CHANGE_GAMECUROSR		WM_APP + 502
// 光标类型(Cursor Type)
#ifndef CT_ARROW
#define CT_ARROW	0	// 箭头
#endif

#ifndef CT_HAND
#define CT_HAND		1	// 手形
#endif

#ifndef CT_BEAN
#define CT_BEAN		2	// I字形
#endif

#ifndef CT_VSIZE
#define CT_VSIZE	3	// 垂直变化尺寸
#endif

#ifndef CT_HANDED
#define CT_HANDED   4   // 握紧手形
#endif

#ifndef CT_ATTACK
#define CT_ATTACK   5   // 攻击形
#endif

#ifndef CT_TALK
#define CT_TALK     6   // 交谈形
#endif

#ifndef CT_INVALID
#define CT_INVALID  7   // 无效形
#endif

#ifndef CT_PICKUP
#define CT_PICKUP   8   // 拾取形
#endif

#ifndef CT_MINE
#define CT_MINE   9   // 采矿
#endif

#ifndef CT_FIX
#define CT_FIX   10   // 修理
#endif

#ifndef CT_HERB
#define CT_HERB   11  // 采药
#endif

#ifndef CT_GOLD
#define CT_GOLD   12  // 金币
#endif
//------------------------------------------------------------------------
struct ICursorData;
struct IBitmap;
// 客户端要实现的回调接口
struct IWndCallback
{
	virtual BOOL OnEvent(UINT uWndID, UINT uEventID, WPARAM wParam, LPARAM lParam) = 0;
};
//------------------------------------------------------------------------
struct IWndEnableCallEx
{
    virtual BOOL OnEvent(UINT uEventID, WPARAM wParam, LPARAM lParam) = NULL;
};
//------------------------------------------------------------------------
// 非通用窗口
struct IWndEnableCall : public XMainWnd, public IWndEnableCallEx
{
	/*  这段以后再加
	DWORD OnKeyDown(UINT nKeyCode, UINT nShift)
		{
			if (nKeyCode == VK_ESCAPE)
			{
				
				return 0;
			}		
			return XMainWnd::OnKeyDown(nKeyCode, UINT nShift);
	}*/
};
//------------------------------------------------------------------------
// 通用窗口
struct IWndEnableCall4 : public XGeneralMainWnd, public IWndEnableCallEx
{
	
};
//------------------------------------------------------------------------
template< class ARG1, class ARG2, class ARG3 >
struct __BasicConnection3;
typedef __BasicConnection3< WPARAM, LPARAM, BOOL& > WMMESSAGE_CONNECTION;
//------------------------------------------------------------------------
// 窗口管理器接口
struct IWndMgr
{
	enum    //弹出原因对话框与输入框的原因
	{
		POPCOURSE_UNKNOWN = 0,    //不明原因		
	};

	// 描  述：创建窗口
	// 参  数：[输入] UINT uWndID  - 窗口ID
	// 返回值：void*- 返回窗口指针
	virtual void* CreateWnd(UINT uWndID) = NULL;

	// 描  述：释放窗口
	// 参  数：[输入] UINT uWndID  - 窗口ID
	// 返回值：BOOL - 如果释放成功，则返回TRUE,否则返回FALSE
	virtual BOOL  CloseWnd(UINT uWndID) = NULL;

	// 描  述：游戏主循环把鼠标键盘等消息派发给窗口管理器
	virtual DWORD OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = NULL;

	// 描  述：逻辑层控制显示层的各种事件
	virtual BOOL OnEvent(UINT uWndID, UINT uEventID, WPARAM wParam, LPARAM lParam) = NULL;

	// 描  述：显示/隐藏 窗口
	virtual BOOL ShowWnd(UINT uWndID, int nCmdShow) = NULL;

	//描  述：检查某个窗口是否是打开的
	virtual BOOL  IsWindowOpened(UINT uWndID) = NULL;

	//描  述：取的鼠标上附加的数据,一般用在拖放操作上
	virtual ICursorData* GetCursorAttachData() = NULL;

	//描  述：设置鼠标上附加的数据
	virtual void SetCursorAttach(IBitmap*pBitmap, ICursorData*pData) = NULL;

	//描  述：检查取得鼠标上的附加的类型
	virtual int GetCursorAttachType() = NULL;

	//描  述：清除鼠标上的附加的数据 
	virtual void ClearCursorAttach() = NULL;

	// 描  述：订阅窗口发出的消息
	// 参  数：[输入] UINT uWndID  - 窗口ID
	// 参  数：[输入] UINT uEventID  - 消息ID
	// 参  数：[输入] WMMESSAGE_CONNECTION*pConnect  - 连接对象
	// 返回值：BOOL
	virtual BOOL SubscibeMessage(UINT uWndID, UINT uEventID, 
		WMMESSAGE_CONNECTION *pConnect) = NULL;
	
	// 描  述：退定消息
	// 参  数：[输入] UINT uWndID  - 窗口ID
	// 参  数：[输入] UINT uEventID  - 消息ID
	// 参  数：[输入] WMMESSAGE_CONNECTION*pConnect  - 连接对象
	// 返回值：BOOL
	virtual BOOL UnSubscibeMessage(UINT uWndID, UINT uEventID,
		WMMESSAGE_CONNECTION *pConnect) = NULL;
	
	// 描  述：窗口广播一个消息，先前SubscibeMessage过的对象可以收到
	virtual void PostWindowMessage(UINT uWndID, UINT uEventID, 
		WPARAM wParam, LPARAM lParam) = NULL;

	// 描  述：释放窗口管理器
	virtual void Release() = NULL;
	
	// 描  述：注册某个窗口，在打开或关闭时，要收到WE_CW_ALLWND_SHOW消息，
	//         并且此窗口要继承IWndEnableCall	
	virtual void RegTellShowWnd  (UINT uWndID) = NULL;

	// 描  述：取消注册某个窗口，在打开或关闭时，要收到WE_CW_ALLWND_SHOW消息。
	virtual void UnRegTellShowWnd(UINT uWndID) = NULL;

	// 创建自定义的消息框
	virtual void  CustomMsgBox(LPCSTR pMsg, DWORD BtnNO1 = 1, 
		DWORD BtnNO2 = 0, 
		DWORD BtnNO3 = 0,
		IWndCallback * pCallBack = NULL,  
		RECT * rcWnd = NULL,
		WPARAM wParam = 0,
		LPARAM lParam = 0,
		int    nCourse = POPCOURSE_UNKNOWN) = NULL;

	// 创建自定义的输入框
	virtual void  CustomInputBox(LPCSTR pMsg, 
		IWndCallback * pCallBack = NULL,
		DWORD  dwMaxLen = 6,
		BOOL   bDigitOnly = TRUE,
		BOOL   bNullFlag = FALSE,
		BOOL   bPasswordFlag = FALSE,
		int    nCourse = POPCOURSE_UNKNOWN) = NULL;

	// 在打开的主窗口中, 关闭其中一个,如果没有打开着的主窗口, 则返回FALSE
	virtual BOOL HideOnePrimacyWnd(void) = NULL;
};
//------------------------------------------------------------------------
struct IClientGlobal;
//------------------------------------------------------------------------
// 窗口管理器辅助类
class CIWndMgrHelper
{
public:
	typedef BOOL (*ProcCreateWndMgr)(IClientGlobal *pGlobal, IWndMgr **ppWndMgr, IWndCallback *pCallBack);
	
	CIWndMgrHelper()
	{
		m_hdll = NULL;
		m_pWndMgr = NULL;
	}
	~CIWndMgrHelper()
	{
		Close();
	}
	IWndMgr* operator ->()
	{
		return m_pWndMgr;
	}
	
	BOOL Create(IClientGlobal *pGlobal, IWndCallback *pCallBack)
	{
		Close();
		try
		{
			if (m_hdll == NULL)
			{
#ifdef _DEBUG
				m_hdll = ::LoadLibrary("FKWndMgr_D.dll");
				if (m_hdll == NULL)
					throw "无法加载 FKWndMgr_D.dll";
#else				
				m_hdll = ::LoadLibrary("FKWndMgr.dll");
				if (m_hdll == NULL)
					throw "无法加载 FKWndMgr.dll";
#endif
			}
			ProcCreateWndMgr proc;
			proc = (ProcCreateWndMgr)GetProcAddress(m_hdll, "CreateWndMgrDLL");
			if (proc == NULL)
				throw "Can not GetProcAddress('CreateWndMgrDll')";
			
			if (!proc(pGlobal, &m_pWndMgr, pCallBack))
				throw "CreateWndMgr() error";

			return m_pWndMgr != NULL;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
			return FALSE;
		}
		catch(...)
		{
			OutputDebugString("CIWndMgrHelper::Create() : catch error\r\n");
			return FALSE;
		}
	}

	void Close()
	{
		if (m_pWndMgr)
		{
			m_pWndMgr->Release();
			m_pWndMgr = NULL;
		}
		if (m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}

public:
	HINSTANCE	m_hdll;
	IWndMgr		*m_pWndMgr;
};
//------------------------------------------------------------------------