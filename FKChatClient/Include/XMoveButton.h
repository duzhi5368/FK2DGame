/**
*	created:		2012-6-29   12:33
*	filename: 		XMoveButton
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		移动窗体按钮
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
// 移动窗体按钮
class XMoveButton : public XButton  
{
public:
	XMoveButton(XWindow * pParentWnd);
	virtual ~XMoveButton(void);

public:
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);

private:
	POINT m_ptDown; // 按下按钮时所处的坐标
	bool m_bDown;   // 是否已经按下了按钮
};
//------------------------------------------------------------------------