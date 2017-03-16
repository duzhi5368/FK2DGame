/**
*	created:		2012-6-23   23:33
*	filename: 		XButton
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
#include "../../FKResManager/IFKResManager.h"
//------------------------------------------------------------------------
XButton::XButton()
{ 
	m_nState = BS_UP;
	m_pBmpDown = m_pBmpUp = m_pBmpHover = m_pBmpDisable = NULL;
	m_pSelfFont = XFontCreate::GetInstance()->CreateFont("宋体", 12, FALSE, FALSE, FALSE, TRUE);
	m_cForeColor        = WPixel(255, 245, 200);
	m_cForeColorUp      = WPixel(255, 245, 200);
	m_cForeColorHover   = WPixel(255, 255, 240);   //鼠标悬停时标题颜色
	m_cForeColorDown    = WPixel(205, 195, 150);   //鼠标按下时标题颜色
	m_cForeColorDisable = WPixel(0, 0, 0);   //鼠标禁用时标题颜色
}
//------------------------------------------------------------------------
XButton::XButton(XWindow* pParentWnd) : XWindow(pParentWnd)
{
	m_nState = BS_UP;
	m_pBmpDown = m_pBmpUp = m_pBmpHover = m_pBmpDisable = NULL;
	m_pSelfFont = XFontCreate::GetInstance()->CreateFont("宋体", 12, FALSE, FALSE, FALSE, TRUE);
	m_cForeColor        = WPixel(255, 245, 200);
	m_cForeColorUp      = WPixel(255, 245, 200);
	m_cForeColorHover   = WPixel(255, 255, 240);   //鼠标悬停时标题颜色
	m_cForeColorDown    = WPixel(205, 195, 150);   //鼠标按下时标题颜色
	m_cForeColorDisable = WPixel(0, 0, 0);   //鼠标禁用时标题颜色
}
//------------------------------------------------------------------------
XButton::~XButton()
{ 
}
//------------------------------------------------------------------------
void XButton::SetUpForeColor(long *color)
{
	m_cForeColorUp = WPixel(color[0], color[1], color[2]);	
}
//------------------------------------------------------------------------
void XButton::SetDisableForeColor(long *color)
{
	m_cForeColorDisable = WPixel(color[0], color[1], color[2]);	
}
//------------------------------------------------------------------------
void XButton::SetHoverForeColor(long *color)
{
	m_cForeColorHover   = WPixel(color[0], color[1], color[2]);		
}
//------------------------------------------------------------------------
void XButton::SetDownForeColor(long *color)
{
	m_cForeColorDown    = WPixel(color[0], color[1], color[2]);   
}
//------------------------------------------------------------------------
void XButton::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	
	XWindow::OnDraw(pCanvas, rcClip);
	
	if (m_strCaption.size() != 0)
	{
		int charwidth =  m_pSelfFont->GetLength(m_strCaption.c_str());
		int charheight = m_pSelfFont->GetHeight();
		int offset_x = (m_rcWnd.right - m_rcWnd.left - charwidth) / 2;
		int offset_y = (m_rcWnd.bottom - m_rcWnd.top - charheight) / 2;
		WPixel forecolor = m_cForeColor;

		switch(m_nState) {
		case BS_UP:
			 
			break;
		case BS_ROLLOVER:
			if (IsEnabled())
			{
				forecolor = m_cForeColorHover;
			}			
			break;
		case BS_DOWN:
			forecolor = m_cForeColorDown;
			offset_x++;
			offset_y++;
			break;
		}
		pCanvas->DrawClipText(m_pSelfFont, m_rcWnd.left + offset_x, m_rcWnd.top + offset_y, m_strCaption.c_str()
			, forecolor, m_rcWnd, FONT_NOALPHA);
	}
}
//------------------------------------------------------------------------
void XButton::DestroyWindow()
{
	m_pWndBitmap = NULL;
	SAFE_RELEASE_IMAGE(m_pBmpDown);
	SAFE_RELEASE_IMAGE(m_pBmpUp);
	SAFE_RELEASE_IMAGE(m_pBmpHover);
	SAFE_RELEASE_IMAGE(m_pBmpDisable);
	XWindow::DestroyWindow();
}
//------------------------------------------------------------------------
IBitmap* XButton::GetBitmapDown()			
{
	if (!m_pBmpDown)
		m_pBmpDown = CImageLoader::GetInstance()->CopyImage(GetBitmapUp());
	return m_pBmpDown;
}
//------------------------------------------------------------------------
IBitmap* XButton::GetBitmapDisable()			
{
	if (!m_pBmpDisable)
		m_pBmpDisable = CImageLoader::GetInstance()->CopyImage(GetBitmapUp());
	return m_pBmpDisable;
}
//------------------------------------------------------------------------
IBitmap* XButton::GetBitmapHover()			
{
	if (!m_pBmpHover)
		m_pBmpHover = CImageLoader::GetInstance()->CopyImage(GetBitmapUp());
	return m_pBmpHover;
}
//------------------------------------------------------------------------
DWORD XButton::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	if (nButton == LeftButton)
	{
		SetBitmap(GetBitmapDown());
		InvalidRect();
		m_nState = BS_DOWN;
	}
	return XWindow::OnMouseDown(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XButton::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	if (IsEnabled())
	{   //这里不加判断的话, SETENABLE后, 图片变灰, 又被改回来
		SetBitmap(GetBitmapUp());
	}

	InvalidRect();
	m_nState = BS_UP;

	return XWindow::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XButton::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{
	m_nState = BS_ROLLOVER;

	if (IsCapture())
	{
		if (PtInRect(&m_rcWnd, pt))
		{
			if (GetBitmap() != GetBitmapDown())
			{
				SetBitmap(GetBitmapDown());
				InvalidRect();
			}
			m_nState = BS_DOWN;
		}
		else
		{
			if (GetBitmap() != GetBitmapUp())
			{
				SetBitmap(GetBitmapUp());
				InvalidRect();		
				m_nState = BS_UP;
			}
		}
	}

	return XWindow::OnMouseMove(nButton, nShift, pt);
}
//------------------------------------------------------------------------
void XButton::OnMouseEnter()
{
	if (!IsEnabled())
		return;
	SetCursor(CT_HAND);
	SetBitmap(GetBitmapHover());
	InvalidRect();
	m_nState = BS_ROLLOVER;
	XWindow::OnMouseEnter();
}
//------------------------------------------------------------------------
void XButton::OnMouseExit()
{
	if (!IsEnabled())
		return;
	SetCursor(CT_ARROW);
	SetBitmap(GetBitmapUp());
	InvalidRect();
	m_nState = BS_UP;
}
//------------------------------------------------------------------------
void XButton::SetEnabled(BOOL bEnabled/* = TRUE*/)
{
	XWindow::SetEnabled(bEnabled);
	if (bEnabled)
	{
		SetBitmap(GetBitmapUp());
		m_cForeColor        = m_cForeColorUp;		
	}
	else
	{
		SetBitmap(GetBitmapDisable());
		m_cForeColor        = m_cForeColorDisable;		
	}
	m_nState = BS_UP;
	InvalidRect();
}
//------------------------------------------------------------------------
void XButton::LoadAllImage()
{
	XWindow::LoadAllImage();
	if (m_pResObj)
	{
		SetBitmapUp(CImageLoader::GetInstance()->LoadImage(m_pResObj->GetPropertyString("upimg")));
		SetBitmapDown(CImageLoader::GetInstance()->LoadImage(m_pResObj->GetPropertyString("downimg")));
		SetBitmapHover(CImageLoader::GetInstance()->LoadImage(m_pResObj->GetPropertyString("hoverimg")));
		SetBitmapDisable(CImageLoader::GetInstance()->LoadImage(m_pResObj->GetPropertyString("disableimg")));

		if (IsEnabled())
			SetBitmap(GetBitmapUp());
		else
			SetBitmap(GetBitmapDisable());
	}
}
//------------------------------------------------------------------------
void XButton::ReleaseAllImage()
{
	XWindow::ReleaseAllImage();
	SAFE_RELEASE_IMAGE(m_pBmpDown);
	SAFE_RELEASE_IMAGE(m_pBmpUp);
	SAFE_RELEASE_IMAGE(m_pBmpHover);
	SAFE_RELEASE_IMAGE(m_pBmpDisable);
}
//------------------------------------------------------------------------