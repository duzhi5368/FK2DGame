/**
*	created:		2012-6-23   23:44
*	filename: 		XLinkText
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XLinkText::XLinkText()
{
	m_cNotVisitedColor	= WPixel(0,0,255);
	m_cVisitedColor		= WPixel(128,0,128);
	m_cRollOverColor	= WPixel(255,0,0);
	m_bVisited			= FALSE;
	m_cCurColor			= m_cNotVisitedColor;
}
//------------------------------------------------------------------------
XLinkText::XLinkText(XWindow* pParentWnd) :XWindow(pParentWnd)
{
	m_cNotVisitedColor	= WPixel(0,0,255);
	m_cVisitedColor		= WPixel(128,0,128);
	m_cRollOverColor	= WPixel(255,0,0);
	m_bVisited			= FALSE;
	m_cCurColor			= m_cNotVisitedColor;
}
//------------------------------------------------------------------------
XLinkText::~XLinkText()
{

}
//------------------------------------------------------------------------
void XLinkText::OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip)
{
	
}
//------------------------------------------------------------------------
void XLinkText::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{

	ITTFont* pTTFont = GetFont();
	if (!m_strCaption.empty())
	{
		int x = m_rcWnd.left + 2;
		int y = m_rcWnd.top + (GetHeight() - pTTFont->GetHeight())/2;
		
		pCanvas->DrawClipText(pTTFont, x, y, m_strCaption.c_str(), m_cCurColor, rcClip, FONT_NOALPHA);
	}

	XWindow::OnDraw(pCanvas, rcClip);
}
//------------------------------------------------------------------------
// 消息相关
DWORD XLinkText::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	if (IsCapture())
	{
		m_bVisited = TRUE;
		s_runURL((DWORD)GetURL());
	}

	return XWindow::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
void XLinkText::OnMouseEnter()
{
	SetCursor(CT_HAND);
	m_cCurColor = m_cRollOverColor;
	InvalidRect();
}
//------------------------------------------------------------------------
void XLinkText::OnMouseExit()
{
	SetCursor(CT_ARROW);
	if (m_bVisited)
		m_cCurColor = m_cVisitedColor;
	else
		m_cCurColor = m_cNotVisitedColor;
	InvalidRect();
}
//------------------------------------------------------------------------