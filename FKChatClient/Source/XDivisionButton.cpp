/**
*	created:		2012-6-29   12:31
*	filename: 		XDivisionButton
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现在一个窗口对其进行分隔出两部分
*/
//------------------------------------------------------------------------
#include "../Include/XDivisionButton.h"
//------------------------------------------------------------------------
XDivisionButton::XDivisionButton(XWindow * pParentWnd)
: XButton(pParentWnd)
, m_bDown(false)
, m_nOffset(100)
{
}
//------------------------------------------------------------------------
XDivisionButton::~XDivisionButton(void)
{
}
//------------------------------------------------------------------------
DWORD XDivisionButton::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	m_bDown = true;
	SetCapture();
	
	return XButton::OnMouseDown(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XDivisionButton::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	m_bDown = false;
	ReleaseCapture();
	
	return XButton::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XDivisionButton::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{	
	RECT rcParentWnd;
	m_pParent->GetWindowRect(&rcParentWnd);
	
	if (m_bDown && PtInRect(&rcParentWnd, pt))
	{		
		m_nOffset = pt.y - rcParentWnd.top;
		
		m_rcWnd.top = pt.y;
		m_rcWnd.bottom = pt.y + DIVISIONBUTTON_HEIGHT;
		
		// 更新父窗体
		m_pParent->SetWindowPos(rcParentWnd);
	}

	return 0;
}
//------------------------------------------------------------------------
void XDivisionButton::SetWindowPos(RECT& rc)
{
	m_pParent->GetWindowRect(&m_rcWnd);
	
	m_rcWnd.top = m_rcWnd.top + m_nOffset;
	m_rcWnd.bottom = m_rcWnd.top + DIVISIONBUTTON_HEIGHT;
	
	XButton::SetWindowPos(m_rcWnd);
}
//------------------------------------------------------------------------
void XDivisionButton::SetOffset(int nOffset)
{
	m_nOffset = nOffset;
}
//------------------------------------------------------------------------
int XDivisionButton::GetOffset(void) const
{
	return m_nOffset;
}
//------------------------------------------------------------------------
void XDivisionButton::SetWindowPos(void)
{
	SetWindowPos(m_rcWnd);
}
//------------------------------------------------------------------------