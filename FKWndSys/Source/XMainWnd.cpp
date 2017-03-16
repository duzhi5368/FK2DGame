/**
*	created:		2012-6-23   21:05
*	filename: 		XMainWnd
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XMainWnd::XMainWnd()
{
	m_dwWndTraits |= XWT_MOVABLE;

}
//------------------------------------------------------------------------
XMainWnd::XMainWnd(XWindow* pParentWnd) : XWindow(pParentWnd)
{
	m_dwWndTraits |= XWT_MOVABLE;

}
//------------------------------------------------------------------------
XMainWnd::~XMainWnd()
{

}
//------------------------------------------------------------------------
void XMainWnd::OnActivate(BOOL bActive)
{
	if (bActive) // 激活
	{
		if (m_pFocusChild)
		{
			GetDesktop()->SetFocusWindow(m_pFocusChild);
			m_pFocusChild->OnSetFocus();
		}
	}
	else
	{
		if (m_pFocusChild)
		{
			m_pFocusChild->OnKillFocus();
		}
	}
	
	//增加绘制矩形
	InvalidRect();
}
//------------------------------------------------------------------------
// 将某个窗口(包括子窗口)置于前端
void XMainWnd::BringToTopLevel()
{
	GetDesktop()->AddWindow(this);
}
//------------------------------------------------------------------------
DWORD XMainWnd::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	XWindow* pWnd = GetMouseMove();
	if (pWnd)
	{
		if (pWnd->IsMainWindow())
			return XWindow::OnMouseDown(nButton, nShift, pt);

		XControl* pControl = NULL;
		XWindow* pWndFind = pWnd;
		while (pWndFind)
		{
			if (pWndFind->IsControl())
			{
				pControl = (XControl*)pWndFind;
				break;
			}
			pWndFind = pWndFind->GetParent();
		}
		if (pControl)
			return pControl->OnMouseDown(nButton, nShift, pt);

		if (pWnd->IsEnabled() && pWnd->GetMainWnd() == this)
			return pWnd->OnMouseDown(nButton, nShift, pt);
	}
	return XWindow::OnMouseDown(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XMainWnd::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	XWindow* pWnd = GetMouseMove();
	if (pWnd)
	{
		if (pWnd->IsMainWindow())
			return XWindow::OnMouseUp(nButton, nShift, pt);

		XControl* pControl = NULL;
		XWindow* pWndFind = pWnd;
		while (pWndFind)
		{
			if (pWndFind->IsControl())
			{
				pControl = (XControl*)pWndFind;
				break;
			}
			pWndFind = pWndFind->GetParent();
		}
		if (pControl)
			return pControl->OnMouseUp(nButton, nShift, pt);

		if (pWnd->IsEnabled() && pWnd->GetMainWnd() == this)
			return pWnd->OnMouseUp(nButton, nShift, pt);
	}
	return XWindow::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XMainWnd::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{
	XWindow* pWnd = GetMouseMove();
	if (pWnd)
	{
		if (pWnd->IsMainWindow())
			return XWindow::OnMouseMove(nButton, nShift, pt);

		XControl* pControl = NULL;
		XWindow* pWndFind = pWnd;
		while (pWndFind)
		{
			if (pWndFind->IsControl())
			{
				pControl = (XControl*)pWndFind;
				break;
			}
			pWndFind = pWndFind->GetParent();
		}
		if (pControl)
			return pControl->OnMouseMove(nButton, nShift, pt);

		if (pWnd->GetMainWnd() == this)
			return pWnd->OnMouseMove(nButton, nShift, pt);
	}
	return XWindow::OnMouseMove(nButton, nShift, pt);
}
//------------------------------------------------------------------------
// 键盘消息
DWORD XMainWnd::OnKeyDown(UINT nKeyCode, UINT nShift)
{
	XWindow* pWnd = GetFocus();
	if (pWnd) // 活动窗口中的焦点窗口处理
		return pWnd->OnKeyDown(nKeyCode, nShift);
	return XWindow::OnKeyDown(nKeyCode, nShift);
}
//------------------------------------------------------------------------
BOOL XMainWnd::CanEscape()
{
	return (m_dwWndTraits & XWT_ESCAPE);
}
//------------------------------------------------------------------------
void XMainWnd::OnEscape()
{
	SetVisible(FALSE);
}
//------------------------------------------------------------------------
DWORD XMainWnd::OnKeyUp(UINT nKeyCode, UINT nShift)
{
	XWindow* pWnd = GetFocus();
	if (pWnd) // 活动窗口中的焦点窗口处理
		return pWnd->OnKeyUp(nKeyCode, nShift);
	return XWindow::OnKeyUp(nKeyCode, nShift);
}
//------------------------------------------------------------------------
DWORD XMainWnd::OnChar(UINT nKeyAscii)
{
	XWindow* pWnd = GetFocus();
	if (pWnd) // 活动窗口中的焦点窗口处理
		return pWnd->OnChar(nKeyAscii);
	return XWindow::OnChar(nKeyAscii);
}
//------------------------------------------------------------------------
void XMainWnd::DestroyWindow()
{
	UnRegisterAccelKey();
	XWindow::DestroyWindow();
}
//------------------------------------------------------------------------
void XMainWnd::SetVisible(BOOL bVisible/* = TRUE*/)
{
	XWindow::SetVisible(bVisible);

	//如果鼠标在区域内, 把鼠标移动一下
	//这里解决鼠标移到NPC上, 再打开窗口,无法档住鼠标的问题
	if (bVisible)
	{
		XCursor *pCursor = GetCursor();
		if (pCursor)
		{
			POINT ptMouse;
			pCursor->GetMouseXY(ptMouse);
			if (PtInRect(&m_rcWnd, ptMouse))
			{
				SetMouseMove();
			}
		}
	}
}
//------------------------------------------------------------------------
void XMainWnd::SetEnabled(BOOL bEnabled/* = TRUE*/)
{
	XWindow::SetEnabled(bEnabled);
}
//------------------------------------------------------------------------
// 通用主窗口
XGeneralMainWnd::XGeneralMainWnd() : XMainWnd()
{
	m_nBorderAlpha = 256;
	m_caption_x = 0;
	m_caption_y = 0; 
	m_pSelfFont = XFontCreate::GetInstance()->CreateFont("宋体", 12, FALSE, FALSE, FALSE, TRUE);
}
//------------------------------------------------------------------------
XGeneralMainWnd::XGeneralMainWnd(XWindow* pParentWnd) : XMainWnd(pParentWnd)
{
	m_nBorderAlpha = 256;
	m_caption_x = 0;
	m_caption_y = 0; 
	m_pSelfFont = XFontCreate::GetInstance()->CreateFont("宋体", 12, FALSE, FALSE, FALSE, TRUE);
}
//------------------------------------------------------------------------
XGeneralMainWnd::~XGeneralMainWnd()
{
	
}
//------------------------------------------------------------------------
void XGeneralMainWnd::SetBorderAlpha(int alpha)
{
	m_nBorderAlpha = alpha;
}
//------------------------------------------------------------------------
int XGeneralMainWnd::GetBorderAlpha()
{
	return m_nBorderAlpha;
}
//------------------------------------------------------------------------
const XGeneralMainWnd::AREA * XGeneralMainWnd::GetGraphicsPos()
{
	return m_Pos;
}
//------------------------------------------------------------------------
BOOL XGeneralMainWnd::InitParamList(long* posList, int nCount)
{
	if (::IsRectEmpty(&m_rcWnd) || nCount != MaxAreaCount*4)
		return FALSE;

	memcpy(m_Pos, posList, MaxAreaCount*4*sizeof(long));

	CalculateCaptionPos();

	return TRUE;
}
//------------------------------------------------------------------------
void XGeneralMainWnd::SetWindowText(LPCSTR lpszString)
{
	XMainWnd::SetWindowText(lpszString); 
	CalculateCaptionPos();
}
//------------------------------------------------------------------------
void XGeneralMainWnd::CalculateCaptionPos(void)
{
	ITTFont* pFont = GetFont();
	int strWidth  = pFont->GetLength(m_strCaption.c_str());
	int strHeight = pFont->GetHeight();
	
	m_caption_x = m_Pos[CaptionPos].x + (m_Pos[CaptionPos].w - strWidth) / 2;
	m_caption_y = m_Pos[CaptionPos].y + (m_Pos[CaptionPos].h - strHeight) / 2;
	
}
//------------------------------------------------------------------------
BOOL XGeneralMainWnd::IsTransparent(POINT& pt)
{
	if (!m_pWndBitmap)
		return FALSE;
	POINT ptSelect = pt;
	ptSelect.x -= m_rcWnd.left;
	ptSelect.y -= m_rcWnd.top;

	POINT ptSelectSrc = ptSelect;
	if (ptSelect.y < m_Pos[Top].y) // 上面
	{
		if (ptSelect.x >= GetWidth() - m_Pos[RightTop].w) // 在右上角区域
			ptSelectSrc.x = m_pWndBitmap->GetWidth() - (GetWidth() - ptSelect.x);
		else if (ptSelect.x > m_Pos[Top].x) // 上面中间的空隙
			return TRUE;
	}
	else if (ptSelect.y >= GetHeight() - m_pWndBitmap->GetHeight() + m_Pos[Bottom].y + m_Pos[Bottom].h) // 下面
	{
		ptSelectSrc.y = m_pWndBitmap->GetHeight() - (GetHeight() - ptSelect.y);
		if (ptSelect.x >= GetWidth() - m_Pos[RightBottom].w) // 在右下角区域
			ptSelectSrc.x = m_pWndBitmap->GetWidth() - (GetWidth() - ptSelect.x);
		else if (ptSelect.x > m_Pos[Bottom].x) // 下面中间的空隙
			return TRUE;
	}
	else if (ptSelect.x < m_Pos[Left].x) // 左面
	{
		if (ptSelect.y >= GetHeight() - m_Pos[LeftBottom].h) // 在左下角区域
			ptSelectSrc.y = m_pWndBitmap->GetHeight() - (GetHeight() - ptSelect.y);
		else if (ptSelect.y > m_Pos[Left].y) // 左面中间的空隙
			return TRUE;
	}
	else if (ptSelect.x >= GetWidth() - m_pWndBitmap->GetWidth() + m_Pos[Right].x + m_Pos[Right].w) // 右面
	{
		ptSelectSrc.x = m_pWndBitmap->GetWidth() - (GetWidth() - ptSelect.x);
		if (ptSelect.y >= GetHeight() - m_Pos[RightBottom].h) // 在右下角区域
			ptSelectSrc.y = m_pWndBitmap->GetHeight() - (GetHeight() - ptSelect.y);
		else if (ptSelect.y > m_Pos[Right].y) // 右面中间的空隙
			return TRUE;
	}
	else
	{
		return FALSE;
	}

	return m_pWndBitmap->IsTransparent(ptSelectSrc);
}
//------------------------------------------------------------------------
void XGeneralMainWnd::SetWindowPos(RECT& rc)
{
	if (m_pWndBitmap)
	{
		if (rc.right < rc.left || rc.right - rc.left < m_pWndBitmap->GetWidth())
			rc.right = rc.left + m_pWndBitmap->GetWidth();
		if (rc.bottom < rc.top || rc.bottom - rc.top < m_pWndBitmap->GetHeight())
			rc.bottom = rc.top + m_pWndBitmap->GetHeight();
	}
	XMainWnd::SetWindowPos(rc);
}
//------------------------------------------------------------------------
void XGeneralMainWnd::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	ITTFont* pFont = GetFont();

	if (!m_strCaption.empty())
	{
		//画阴影
		pCanvas->DrawClipText(pFont, 
			m_rcWnd.left + m_caption_x + 1, 
			m_rcWnd.top + m_caption_y + 1, 
			m_strCaption.c_str(), WPixel(0, 0, 0), rcClip, FONT_NOALPHA);

		pCanvas->DrawClipText(pFont, 
			m_rcWnd.left + m_caption_x, 
			m_rcWnd.top + m_caption_y, 
			m_strCaption.c_str(), GetForeColor(), rcClip, FONT_NOALPHA);

	}
	
	XMainWnd::OnDraw(pCanvas, rcClip);
}
//------------------------------------------------------------------------
void XGeneralMainWnd::OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip)
{
	DWORD style = BLIT_NORMAL;
	
	// 没有图片则绘制一个矩形
	if (!m_pWndBitmap)
	{
		if (m_nAlpha == 0) return;
		if (m_nAlpha < 256)
			pCanvas->DrawBox(rcClip, GetBackColor(), style|BLIT_TRANS, m_nAlpha); // WPixel(212,208,200)
		else
			pCanvas->Clear(GetBackColor(), &rcClip);
		return;
	}
	
	// 绘制图片背景
	if (m_nAlpha < 256)
	{		
		m_pWndBitmap->SetUserData(m_nAlpha);
		style |= BLIT_TRANS;
	}
	
	if(IsMask())
		style |= BLIT_MASK;
	
	int x, y, i, j;
	RECT rc;
	// 先填充背景
	rc.left = m_rcWnd.left + m_Pos[Fill].x;
	rc.top = m_rcWnd.top + m_Pos[Fill].y;
	rc.right = rc.left + GetWidth() - m_pWndBitmap->GetWidth() + m_Pos[Fill].w;
	rc.bottom = rc.top + GetHeight() - m_pWndBitmap->GetHeight() + m_Pos[Fill].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int rows = (rc.bottom - rc.top) / m_Pos[SrcBack].h;
		int rows_remind = (rc.bottom - rc.top) % m_Pos[SrcBack].h;
		int cols = (rc.right - rc.left) / m_Pos[SrcBack].w;
		int cols_remind = (rc.right - rc.left) % m_Pos[SrcBack].w;
		y = rc.top;
		for (i=0; i<rows; i++) // 一行一行的填充
		{
			x = rc.left;
			for (j=0; j<cols; j++)
			{
				m_pWndBitmap->Draw(pCanvas, 
					x, y, m_Pos[SrcBack].w, m_Pos[SrcBack].h, 
					m_Pos[SrcBack].x, m_Pos[SrcBack].y, style);
				x+=m_Pos[SrcBack].w;
			}
			if (cols_remind > 0) // 最后一列
			{
				m_pWndBitmap->Draw(pCanvas, 
					x, y, cols_remind, m_Pos[SrcBack].h, 
					m_Pos[SrcBack].x, m_Pos[SrcBack].y, style);
			}
			y+=m_Pos[SrcBack].h;
		}
		// 最后一行
		if (rows_remind > 0)
		{
			x = rc.left;
			for (j=0; j<cols; j++)
			{
				m_pWndBitmap->Draw(pCanvas, 
					x, y, m_Pos[SrcBack].w, rows_remind, 
					m_Pos[SrcBack].x, m_Pos[SrcBack].y, style);
				x+=m_Pos[SrcBack].w;
			}
			if (cols_remind > 0) // 最后一列
			{
				m_pWndBitmap->Draw(pCanvas, 
					x, y, cols_remind, rows_remind, 
					m_Pos[SrcBack].x, m_Pos[SrcBack].y, style);
			}
		}
	}

	// 绘制边框图片	
	if (m_nBorderAlpha == 0) return;
	m_pWndBitmap->SetUserData(m_nBorderAlpha);
	style |= BLIT_TRANS;
	
	
	// 再绘制四个角落的部分
	// 左上
	rc.left = m_rcWnd.left + m_Pos[LeftTop].x;
	rc.top = m_rcWnd.top + m_Pos[LeftTop].y;
	rc.right = rc.left + m_Pos[LeftTop].w;
	rc.bottom = rc.top + m_Pos[LeftTop].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		m_pWndBitmap->Draw(pCanvas,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			rcClip.left - m_rcWnd.left, rcClip.top - m_rcWnd.top, style);
	}

	// 右上
	rc.left = m_rcWnd.right - m_Pos[RightTop].w;
	rc.top = m_rcWnd.top + m_Pos[RightTop].y;
	rc.right = m_rcWnd.right;
	rc.bottom = rc.top + m_Pos[RightTop].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{

		long deltax = rc.left - m_rcWnd.right + m_Pos[RightTop].w;
		if (deltax <= 0) deltax = 0;
		m_pWndBitmap->Draw(pCanvas,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			m_Pos[RightTop].x + deltax, rcClip.top - m_rcWnd.top, style);
	}

	// 左下
	rc.left = m_rcWnd.left + m_Pos[LeftBottom].x;
	rc.top = m_rcWnd.bottom - m_Pos[LeftBottom].h;
	rc.right = rc.left + m_Pos[LeftBottom].w;
	rc.bottom = m_rcWnd.bottom;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		long deltay = rc.top - m_rcWnd.bottom + m_Pos[LeftBottom].h;
		if (deltay <= 0) deltay = 0;
		m_pWndBitmap->Draw(pCanvas,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			rcClip.left - m_rcWnd.left, m_Pos[LeftBottom].y + deltay, style);
	}

	// 右下
	rc.right = m_rcWnd.right;
	rc.bottom = m_rcWnd.bottom;
	rc.left = m_rcWnd.right - m_Pos[RightBottom].w;
	rc.top = m_rcWnd.bottom - m_Pos[RightBottom].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		long deltax = rc.left - m_rcWnd.right + m_Pos[RightBottom].w;
		if (deltax <= 0) deltax = 0;
		long deltay = rc.top - m_rcWnd.bottom + m_Pos[RightBottom].h;
		if (deltay <= 0) deltay = 0;
		m_pWndBitmap->Draw(pCanvas,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			m_Pos[RightBottom].x + deltax, m_Pos[RightBottom].y + deltay, style);
	}

	// 绘制可拉伸部分
	// 上部
	rc.left = m_rcWnd.left + m_Pos[Top].x;
	rc.top = m_rcWnd.top + m_Pos[Top].y;
	rc.right = m_rcWnd.right - m_Pos[RightTop].w;
	rc.bottom = rc.top + m_Pos[Top].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int cols = (rc.right - rc.left) / m_Pos[Top].w;
		int cols_remind = (rc.right - rc.left) % m_Pos[Top].w;
		long deltay = rc.top - m_rcWnd.top - m_Pos[Top].y;
		if (deltay <= 0) deltay = 0;

		x = rc.left;
		y = rc.top;
		for (j=0; j<cols; j++)
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Top].w, m_Pos[Top].h, 
				m_Pos[Top].x, m_Pos[Top].y + deltay, style);
			x+=m_Pos[Top].w;
		}
		if (cols_remind > 0) // 最后一列
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, cols_remind, m_Pos[Top].h, 
				m_Pos[Top].x, m_Pos[Top].y + deltay, style);
		} 
	}	
	
	// 左部
	rc.left = m_rcWnd.left + m_Pos[Left].x;
	rc.top = m_rcWnd.top + m_Pos[Left].y;
	rc.right = rc.left + m_Pos[Left].w;
	rc.bottom = m_rcWnd.bottom - m_Pos[LeftBottom].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int rows = (rc.bottom - rc.top) / m_Pos[Left].h;
		int rows_remind = (rc.bottom - rc.top) % m_Pos[Left].h;
		long deltax = rc.left - m_rcWnd.left - m_Pos[Left].x;
		if (deltax <= 0) deltax = 0;
		
		x = rc.left;
		y = rc.top;
		for (i=0; i<rows; i++)
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Left].w, m_Pos[Left].h, 
				m_Pos[Left].x + deltax, m_Pos[Left].y, style);
			y+=m_Pos[Left].h;
		}
		if (rows_remind > 0) // 最后一行
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Left].w, rows_remind, 
				m_Pos[Left].x + deltax, m_Pos[Left].y, style);
		} 
	}

	// 下部
	rc.left = m_rcWnd.left + m_Pos[Bottom].x;
	rc.top = m_rcWnd.bottom - m_pWndBitmap->GetHeight() + m_Pos[Bottom].y;
	rc.right = m_rcWnd.right - m_Pos[RightBottom].w;
	rc.bottom = rc.top + m_Pos[Bottom].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int cols = (rc.right - rc.left) / m_Pos[Bottom].w;
		int cols_remind = (rc.right - rc.left) % m_Pos[Bottom].w;
		long deltay = rc.top - m_rcWnd.bottom + m_pWndBitmap->GetHeight() - m_Pos[Bottom].y;
		if (deltay <= 0) deltay = 0;
		
		x = rc.left;
		y = rc.top;
		for (j=0; j<cols; j++)
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Bottom].w, m_Pos[Bottom].h, 
				m_Pos[Bottom].x, m_Pos[Bottom].y + deltay, style);
			x+=m_Pos[Bottom].w;
		}
		if (cols_remind > 0) // 最后一列
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, cols_remind, m_Pos[Bottom].h, 
				m_Pos[Bottom].x, m_Pos[Bottom].y + deltay, style);
		} 
	}

	
	// 右部
	rc.left = m_rcWnd.right - m_pWndBitmap->GetWidth() + m_Pos[Right].x;
	rc.top = m_rcWnd.top + m_Pos[Right].y;
	rc.right = m_rcWnd.right;
	rc.bottom = m_rcWnd.bottom - m_Pos[RightBottom].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int rows = (rc.bottom - rc.top) / m_Pos[Right].h;
		int rows_remind = (rc.bottom - rc.top) % m_Pos[Right].h;
		long deltax = rc.left - m_rcWnd.right + m_pWndBitmap->GetWidth() - m_Pos[Right].x;
		if (deltax <= 0) deltax = 0;
		
		x = rc.left;
		y = rc.top;
		for (i=0; i<rows; i++)
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Right].w, m_Pos[Right].h, 
				m_Pos[Right].x + deltax, m_Pos[Right].y, style);
			y+=m_Pos[Right].h;
		}
		if (rows_remind > 0) // 最后一行
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Right].w, rows_remind, 
				m_Pos[Right].x + deltax, m_Pos[Right].y, style);
		} 
	}
}
//------------------------------------------------------------------------