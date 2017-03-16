/**
*	created:		2012-6-28   9:38
*	filename: 		TitleWnd
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		称号系统窗口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "XListBoxEx.h"
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKGame/WndSys/IWndMgr.h"
#include "../Include/TitleDef.h"
//------------------------------------------------------------------------
class CTitleWnd:public IWndEnableCall4 
{
public:
	CTitleWnd();
	virtual ~CTitleWnd();
public:
	virtual int OnEvent(UINT uEventID, WPARAM wParam, LPARAM lParam);
public:
	BOOL Create(IResObject *pResObject,int nWndId);
	void OnVScrollPosChanged(DWORD dwParam);
	void ReSetScroll();

public:
	void AddActorTitle(SActorTitle2 *pActorTitle);
	void DeleteActorTitle(long lTitleId);
	void UnUseActorTitle(long lTitleId);
	void StatusChange(long lTitleId,int nStatus);
	void UpdateScroll();
public:
	// 按钮处理函数
	void OnUseTitle();
	void OnDeleteTitle();
	void OnUnUseTitle();
	void OnClose();
	void OnSelChange(DWORD dwParam);
	void OnHide();
	virtual void OnEscape();
	
private:
	XEdit  *m_pBorderEdit1;     // 边框1
	XEdit  *m_pBorderEdit2;     // 边框2
	XButton *m_pUserTitleBtn;   // 使用称号
	XButton *m_pUnUseTitleBtn;  // 不使用称号
	XButton *m_pDeleteTitleBtn; // 删除称号
	XButton *m_pCloseBtn;       // 关闭按钮
	XCheckBox *m_pHideBtn;      // 隐藏称号
	XListBoxEx   *m_pListBox;   // 称号列表
	XScrollBar *m_pScrollBar;   // 滚动称号列表的滚动条
    BOOL    m_bReget;
};
//------------------------------------------------------------------------
