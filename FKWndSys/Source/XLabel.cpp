/**
*	created:		2012-6-23   23:43
*	filename: 		XLabel
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XLabel::XLabel()
{
	m_nMargin = 2;
	m_pSelfFont = XFontCreate::GetInstance()->CreateFont("宋体", 12, FALSE, FALSE, FALSE, TRUE);
}
//------------------------------------------------------------------------
XLabel::XLabel(XWindow* pParentWnd) :XWindow(pParentWnd)
{
	m_nMargin = 2;
}
//------------------------------------------------------------------------
XLabel::~XLabel()
{
}
//------------------------------------------------------------------------
void XLabel::OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip)
{

}
//------------------------------------------------------------------------
void XLabel::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	ITTFont* pFont = GetFont();
	if (!m_strCaption.empty())
	{
		int x,y;
		switch (GetAlign())
		{
		case AS_LEFT:
			x = m_rcWnd.left + m_nMargin;
			break;
		case AS_CENTER:
			x = m_rcWnd.left + (GetWidth() - (int)pFont->GetLength(m_strCaption.c_str()))/2;
			break;
		case AS_RIGHT:
			x = m_rcWnd.left + GetWidth() - (int)pFont->GetLength(m_strCaption.c_str()) - m_nMargin;
			break;
		}
		
		y = m_rcWnd.top + (GetHeight() - pFont->GetHeight())/2;	
		//这里画一个阴影
		pCanvas->DrawClipText(pFont, x + 1, y + 1, m_strCaption.c_str(), WPixel(0), rcClip, FONT_NOALPHA);
		pCanvas->DrawClipText(pFont, x, y, m_strCaption.c_str(), GetForeColor(), rcClip, FONT_NOALPHA);
	}
	
	XWindow::OnDraw(pCanvas, rcClip);
}
//------------------------------------------------------------------------