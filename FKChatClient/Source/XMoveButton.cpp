/**
*	created:		2012-6-29   12:33
*	filename: 		XMoveButton
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现移动窗体按钮
*/
//------------------------------------------------------------------------
#include "../Include/XMoveButton.h"
//------------------------------------------------------------------------
XMoveButton::XMoveButton(XWindow * pParentWnd)
: XButton(pParentWnd)
, m_bDown(false)
{
}
//------------------------------------------------------------------------
XMoveButton::~XMoveButton(void)
{
}
//------------------------------------------------------------------------
DWORD XMoveButton::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	m_bDown = true;
	m_ptDown = pt;
	SetCapture();

	return XButton::OnMouseDown(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XMoveButton::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	m_bDown = false;
	ReleaseCapture();

	return XButton::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XMoveButton::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{
	RECT rcDesktop;
	GetDesktop()->GetWindowRect(&rcDesktop);
	
	if (m_bDown && PtInRect(&rcDesktop, pt))
	{
		int dx = pt.x - m_ptDown.x;
		int dy = pt.y - m_ptDown.y;

		// 移动父窗体
		m_pParent->MoveWindow(dx, dy);		
		m_ptDown = pt;
	}
	
	return 0;
}
//------------------------------------------------------------------------