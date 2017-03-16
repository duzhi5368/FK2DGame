/**
*	created:		2012-6-29   12:17
*	filename: 		XDivisionButton
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		窗体的分隔按钮
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
#define DIVISIONBUTTON_HEIGHT 2		// 高度
//------------------------------------------------------------------------
// 窗体的分隔按钮
class XDivisionButton : public XButton  
{
public:
	XDivisionButton(XWindow * pParentWnd);
	virtual ~XDivisionButton(void);

public:
	int GetOffset(void) const;
	void SetOffset(int nOffset);
	void SetWindowPos(void);

public:
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	virtual	void SetWindowPos(RECT& rc);
	
private:
	bool m_bDown;   // 是否已经按下了按钮
	int m_nOffset;
};
//------------------------------------------------------------------------