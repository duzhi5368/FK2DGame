/**
*	created:		2012-6-23   23:47
*	filename: 		XProgressBar
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
// 提供的图片中，每种颜色图片的长度
#define PB_LENGTH	10
//------------------------------------------------------------------------
XProgressBar::XProgressBar() :XWindow()
{
	m_nMin = m_nPos = 0;
	m_nMax = 99;
	m_nStyle = 0;
}
//------------------------------------------------------------------------
XProgressBar::XProgressBar(XWindow* pParentWnd) :XWindow(pParentWnd)
{
	m_nMin = m_nPos = 0;
	m_nMax = 99;
	m_nStyle = 0;	
}
//------------------------------------------------------------------------
XProgressBar::~XProgressBar()
{
}
//------------------------------------------------------------------------
void XProgressBar::SetRange(int nMin, int nMax, BOOL bRedraw)
{
	m_nMin = nMin;
	m_nMax = nMax;
	if (bRedraw)
		InvalidRect();
}
//------------------------------------------------------------------------
void XProgressBar::OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip)
{
}
//------------------------------------------------------------------------
void XProgressBar::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
/*
	if (m_pWndBitmap) // 根据两个小图片绘制进度条
	{
		DWORD style = BLIT_NORMAL;
		
		if(m_nAlpha < 256)
		{
			m_pWndBitmap->SetUserData(m_nAlpha);
			style |= BLIT_TRANS;
		}
		
		if(IsMask())
			style |= BLIT_MASK;

		if (m_nStyle == S_HORZ)
		{
			int nOffset = (GetPos() - m_nMin)*GetWidth()/(m_nMax - m_nMin);

			// 先画上层颜色块
			int n = nOffset/PB_LENGTH;
			int m = nOffset%PB_LENGTH;
			for (int i=0; i<n; i++)
				m_pWndBitmap->Draw(pCanvas, m_rcWnd.left + i*PB_LENGTH, m_rcWnd.top, PB_LENGTH, GetHeight(), 
					0, 0, style);
			if (m > 0)
				m_pWndBitmap->Draw(pCanvas, m_rcWnd.left + (i - 1)*PB_LENGTH, m_rcWnd.top, m, GetHeight(), 
					0, 0, style);

			// 再画下层颜色块
			n = (GetWidth() - nOffset)/PB_LENGTH;
			m = (GetWidth() - nOffset)%PB_LENGTH;

			for (i=0; i<n; i++)
				m_pWndBitmap->Draw(pCanvas, m_rcWnd.left + nOffset + i*PB_LENGTH, m_rcWnd.top, PB_LENGTH, GetHeight(), 
					PB_LENGTH, 0, style);
			if (m > 0)
				m_pWndBitmap->Draw(pCanvas, m_rcWnd.left + nOffset + (i - 1)*PB_LENGTH, m_rcWnd.top, m, GetHeight(), 
					PB_LENGTH, 0, style);
		}
		else if (m_nStyle == S_VERT)
		{
			int nOffset = (GetPos() - m_nMin)*GetHeight()/(m_nMax - m_nMin);
			nOffset = GetHeight() - nOffset;

			// 先画下层颜色块
			int n = nOffset/PB_LENGTH;
			int m = nOffset%PB_LENGTH;

			for (int i=0; i<n; i++)
				m_pWndBitmap->Draw(pCanvas, m_rcWnd.left, m_rcWnd.top + i*PB_LENGTH, GetWidth(), PB_LENGTH, 
					PB_LENGTH, 0, style);
			if (m > 0)
				m_pWndBitmap->Draw(pCanvas, m_rcWnd.left, m_rcWnd.top + (i - 1)*PB_LENGTH, GetWidth(), m, 
					PB_LENGTH, 0, style);

			// 再画上层颜色块
			n = (GetHeight() - nOffset)/PB_LENGTH;
			m = (GetHeight() - nOffset)%PB_LENGTH;

			for (i=0; i<n; i++)
				m_pWndBitmap->Draw(pCanvas, m_rcWnd.left, m_rcWnd.top + nOffset + i*PB_LENGTH, GetWidth(), PB_LENGTH, 
					0, 0, style);
			if (m > 0)
				m_pWndBitmap->Draw(pCanvas, m_rcWnd.left, m_rcWnd.top + nOffset + (i - 1)*PB_LENGTH, GetWidth(), m, 
					0, 0, style);
		}
	}*/
	if (m_pWndBitmap) // 直接在底图上绘制进度条(底图不给，进度条图片大小保持与进度条控件尺寸一致)
	{
		DWORD style = BLIT_NORMAL;
		
		if(m_nAlpha < 256)
		{
			m_pWndBitmap->SetUserData(m_nAlpha);
			style |= BLIT_TRANS;
		}
		
		if(IsMask())
			style |= BLIT_MASK;

		if (m_nStyle == S_HORZ)
		{
			int nOffset = (GetPos() - m_nMin)*GetWidth()/(m_nMax - m_nMin);
			m_pWndBitmap->Draw(pCanvas, m_rcWnd.left, m_rcWnd.top, nOffset, GetHeight(), 
				0, 0, style);
		}
		else if (m_nStyle == S_VERT)
		{
			int nOffset = (GetPos() - m_nMin)*GetHeight()/(m_nMax - m_nMin);
			nOffset = GetHeight() - nOffset;
			m_pWndBitmap->Draw(pCanvas, m_rcWnd.left, m_rcWnd.top + nOffset, GetWidth(), GetHeight() - nOffset, 
				0, nOffset, style);
		}
	}
	else // 直接用程序画框
	{
		DWORD style = BLIT_NORMAL;

		if (m_nStyle == S_HORZ)
		{
			int nOffset = (GetPos() - m_nMin)*GetWidth()/(m_nMax - m_nMin);

			RECT rc;
			
			if (GetAlpha() < 256)
			{
				CopyRect(&rc, &m_rcWnd);
				rc.right = rc.left + nOffset;
				IntersectRect(&rc, &rc, &rcClip);
				pCanvas->DrawBox(rc, GetForeColor(), BLIT_TRANS, GetAlpha());
				CopyRect(&rc, &m_rcWnd);
				rc.left = m_rcWnd.left + nOffset;
				IntersectRect(&rc, &rc, &rcClip);
				pCanvas->DrawBox(rc, GetBackColor(), BLIT_TRANS, GetAlpha());
			}
			else
			{
				CopyRect(&rc, &m_rcWnd);
				rc.right = rc.left + nOffset;
				IntersectRect(&rc, &rc, &rcClip);
				pCanvas->DrawBox(rc, GetForeColor(), BLIT_NORMAL, 255);
				CopyRect(&rc, &m_rcWnd);
				rc.left = m_rcWnd.left + nOffset;
				IntersectRect(&rc, &rc, &rcClip);
				pCanvas->DrawBox(rc, GetBackColor(), BLIT_NORMAL, 255);
			}
		}
		else if (m_nStyle == S_VERT)
		{
			int nOffset = (GetPos() - m_nMin)*GetHeight()/(m_nMax - m_nMin);

			RECT rc;

			if (GetAlpha() < 256)
			{
				CopyRect(&rc, &m_rcWnd);
				rc.top = m_rcWnd.bottom - nOffset;
				IntersectRect(&rc, &rc, &rcClip);
				pCanvas->DrawBox(rc, GetForeColor(), BLIT_TRANS, GetAlpha());
				CopyRect(&rc, &m_rcWnd);
				rc.bottom = m_rcWnd.bottom - nOffset;
				IntersectRect(&rc, &rc, &rcClip);
				pCanvas->DrawBox(rc, GetBackColor(), BLIT_TRANS, GetAlpha());
			}
			else
			{
				CopyRect(&rc, &m_rcWnd);
				rc.top = m_rcWnd.bottom - nOffset;
				IntersectRect(&rc, &rc, &rcClip);
				pCanvas->DrawBox(rc, GetForeColor(), BLIT_NORMAL, 255);
				CopyRect(&rc, &m_rcWnd);
				rc.bottom = m_rcWnd.bottom - nOffset;
				IntersectRect(&rc, &rc, &rcClip);
				pCanvas->DrawBox(rc, GetBackColor(), BLIT_NORMAL, 255);
			}
		}
	}

	XWindow::OnDraw(pCanvas, rcClip);	
}
//------------------------------------------------------------------------
void XProgressBar::SetPos(int nPos)
 {
	int oldPos = m_nPos;
	m_nPos = nPos;
	if (m_nPos < m_nMin)
		m_nPos = m_nMin;
	if (m_nPos > m_nMax)
		m_nPos = m_nMax;

	InvalidRect();
}
//------------------------------------------------------------------------