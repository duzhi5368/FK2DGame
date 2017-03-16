/**
*	created:		2012-6-29   12:35
*	filename: 		XResizeButton
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现可以改变窗体大小的按钮
*/
//------------------------------------------------------------------------
#include "../Include/XResizeButton.h"
#include "../Include/OutputWindow.h"
//------------------------------------------------------------------------
XResizeButton::XResizeButton(XWindow * pParentWnd)
: XButton(pParentWnd)
, m_bDown(false)
{
}
//------------------------------------------------------------------------
XResizeButton::~XResizeButton(void)
{
}
//------------------------------------------------------------------------
DWORD XResizeButton::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	m_bDown = true;
	SetCapture();
	
	return XButton::OnMouseDown(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XResizeButton::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	m_bDown = false;
	ReleaseCapture();
	
	return XButton::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XResizeButton::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{	
	if (m_bDown)
	{
		RECT rcWindow;
		m_pParent->GetWindowRect(&rcWindow);

		if (rcWindow.bottom - pt.y > WINDOW_MIN_HEIGHT 
			&& rcWindow.bottom - pt.y < WINDOW_MAX_HEIGHT)
		{
			rcWindow.top = pt.y;

			// 改变父窗体大小
			m_pParent->SetWindowPos(rcWindow);
		}				
	}
	
	return 0;
}
//------------------------------------------------------------------------