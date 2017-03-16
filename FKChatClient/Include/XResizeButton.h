/**
*	created:		2012-6-29   12:34
*	filename: 		XResizeButton
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		改变窗体大小按钮
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
// 改变窗体大小按钮
class XResizeButton : public XButton  
{
public:
	XResizeButton(XWindow * pParentWnd);
	virtual ~XResizeButton(void);

public:
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseUp(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);

private:
	bool m_bDown;   // 是否已经按下了按钮
};
//------------------------------------------------------------------------