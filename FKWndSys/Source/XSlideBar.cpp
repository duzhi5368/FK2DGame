/**
*	created:		2012-6-24   0:12
*	filename: 		XSlideBar
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
#include "../../FKResManager/IFKResManager.h"
//------------------------------------------------------------------------
XSlideBar::XSlideBar()
{
	m_pBmp = NULL;
	m_nMin = m_nPos = 0;
	m_nMax = 99;
	m_nStyle = 0;
	SetRectEmpty(&m_rcBlock);
}
//------------------------------------------------------------------------
XSlideBar::XSlideBar(XWindow* pParentWnd) :XWindow(pParentWnd)
{
	m_pBmp = NULL;
	m_nMin = m_nPos = 0;
	m_nMax = 99;
	m_nStyle = 0;	
	SetRectEmpty(&m_rcBlock);
}
//------------------------------------------------------------------------
XSlideBar::~XSlideBar()
{
}
//------------------------------------------------------------------------
void XSlideBar::DestroyWindow()
{
	SAFE_RELEASE_IMAGE(m_pBmp);
	XWindow::DestroyWindow();
}
//------------------------------------------------------------------------
void XSlideBar::SetRange(int nMin, int nMax, BOOL bRedraw)
{
	m_nMin = nMin;
	m_nMax = nMax;
	if (bRedraw)
		InvalidRect();
}
//------------------------------------------------------------------------
void XSlideBar::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	if (m_pBmp)
	{
		DWORD style = BLIT_NORMAL;
		
		if(m_nAlpha < 256)
		{
			m_pBmp->SetUserData(m_nAlpha);
			style |= BLIT_TRANS;
		}
		
		if(IsMask())
			style |= BLIT_MASK;

		RECT rc = m_rcBlock;
		OffsetRect(&rc, m_rcWnd.left, m_rcWnd.top);
		IntersectRect(&rc, &rc, &rcClip);
		m_pBmp->Draw(pCanvas, rc.left, rc.top, 
			rc.right - rc.left, rc.bottom - rc.top, 
			rc.left-m_rcBlock.left-m_rcWnd.left, rc.top-m_rcBlock.top-m_rcWnd.top, style);
	}

	XWindow::OnDraw(pCanvas, rcClip);
}
//------------------------------------------------------------------------
// 消息相关
DWORD XSlideBar::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	if (nButton == LeftButton)
	{
		POINT ptOffset;
		ptOffset.x = pt.x - m_rcWnd.left; 
		ptOffset.y = pt.y - m_rcWnd.top;
		if (PtInRect(&m_rcBlock, ptOffset)) // 按在滑块上
		{
			SetCapture();
			// 鼠标点击处距滑块左顶的偏移量
			POINT ptTemp;
			ptTemp.x = ptOffset.x - m_rcBlock.left;
			ptTemp.y = ptOffset.y - m_rcBlock.top;
			WritePoint(ptTemp);
		}
	}
	return XWindow::OnMouseDown(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XSlideBar::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{
	POINT ptOffset;
	ptOffset.x = pt.x - m_rcWnd.left; 
	ptOffset.y = pt.y - m_rcWnd.top;

	if (nButton == LeftButton) // 拖动滑块
	{
		int nPos;
		POINT ptTemp = ReadPoint();
		if (m_nStyle == S_HORZ)
			nPos = (ptOffset.x - ptTemp.x)*(m_nMax - m_nMin)/(GetWidth() - (m_rcBlock.right - m_rcBlock.left));
		else if (m_nStyle == S_VERT)
			nPos = (ptOffset.y - ptTemp.y)*(m_nMax - m_nMin)/(GetHeight() - (m_rcBlock.bottom - m_rcBlock.top));

		SetPos(nPos);
	}
	else
	{
		if (PtInRect(&m_rcBlock, ptOffset)) // 移在滑块上
			SetMouseMove();
	}
	
	return XWindow::OnMouseMove(nButton, nShift, pt);
}
//------------------------------------------------------------------------
void XSlideBar::OnMouseEnter()
{
	SetCursor(CT_HAND);
}
//------------------------------------------------------------------------
void XSlideBar::OnMouseExit()
{
	SetCursor(CT_ARROW);
}
//------------------------------------------------------------------------
void XSlideBar::SetPos(int nPos)
 {
	if (m_nMin == m_nMax)
		return;
	int oldPos = m_nPos;
	m_nPos = nPos;
	if (m_nPos < m_nMin)
		m_nPos = m_nMin;
	if (m_nPos > m_nMax)
		m_nPos = m_nMax;

	int n;
	RECT rc;
	CopyRect(&rc, &m_rcBlock);

	if (m_nStyle == S_HORZ)
	{
		n = (GetPos() - m_nMin)*(GetWidth() - (m_rcBlock.right - m_rcBlock.left))/(m_nMax - m_nMin);
		SetRect(&m_rcBlock, n, rc.top, n + rc.right - rc.left, rc.bottom - rc.top);
	}
	else if (m_nStyle == S_VERT)
	{
		n = (GetPos() - m_nMin)*(GetHeight() - (m_rcBlock.bottom - m_rcBlock.top))/(m_nMax - m_nMin);
		SetRect(&m_rcBlock, rc.left, n, rc.right - rc.left, n + rc.bottom - rc.top);
	}

	s_posChanged((DWORD)&m_nPos);

	InvalidRect();
}
//------------------------------------------------------------------------
void XSlideBar::LoadAllImage()
{
	XWindow::LoadAllImage();
	if (m_pResObj)
		SetBlockBitmap(CImageLoader::GetInstance()->LoadImage(m_pResObj->GetPropertyString("blockimg")));
}
//------------------------------------------------------------------------
void XSlideBar::ReleaseAllImage()
{
	XWindow::ReleaseAllImage();
	SAFE_RELEASE_IMAGE(m_pBmp);
}
//------------------------------------------------------------------------