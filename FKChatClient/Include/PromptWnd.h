/**
*	created:		2012-6-28   9:04
*	filename: 		PromptWnd
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		系统提示信息窗口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Reminder.h"
#include "../../FKResManager/IFKResManager.h"
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
// 系统提示信息窗口
class CPromptWnd : public XMainWnd,
                   public CReminder
{
public:
	CPromptWnd(void);
	virtual ~CPromptWnd(void);

public:	
	BOOL Create(IResObject * pResObject);
	virtual	void SetWindowPos(RECT& rc);
	virtual void OnDraw(ICanvas * pCanvas, RECT & rcClip);
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual BOOL IsTransparent(POINT & pt);
	virtual void AddMessage(LPCSTR lpszText);	
	virtual void AddLine(string strText);
	void ClearMessage(void);
	void ShowWindow(BOOL bShow);
};
//------------------------------------------------------------------------