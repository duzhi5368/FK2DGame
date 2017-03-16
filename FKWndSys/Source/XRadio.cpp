/**
*	created:		2012-6-23   23:57
*	filename: 		XRadio
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKResManager/IFKResManager.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
#define DX_RADIO		2
//------------------------------------------------------------------------
XRadio::XRadio()
{
	m_pGroup = NULL;
	m_pBmp = NULL;
	m_nState = RS_UNCHECK;
}
//------------------------------------------------------------------------
XRadio::XRadio(XWindow* pParentWnd) :XWindow(pParentWnd)
{
	m_pGroup = NULL;
	m_pBmp = NULL;
	m_nState = RS_UNCHECK;
}
//------------------------------------------------------------------------
XRadio::~XRadio()
{
}
//------------------------------------------------------------------------
void XRadio::Release()
{
	// 先从XRadioGroup中注销自己
	if (m_pGroup)
		m_pGroup->RemoveRadio(this);
	
	// 释放本窗口
	DestroyWindow();
}
//------------------------------------------------------------------------
void XRadio::DestroyWindow()
{
	SAFE_RELEASE_IMAGE(m_pBmp);
	XWindow::DestroyWindow();
}
//------------------------------------------------------------------------
void XRadio::OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip)
{
}
//------------------------------------------------------------------------
void XRadio::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	// 状态方框
	int w = 0, h, nSrcX;
	if (m_pBmp)
	{
		w = m_pBmp->GetWidth() / 4; // 4张图
		h = m_pBmp->GetHeight();

		// 4张图的排列顺序：1-有效,未复选；2-有效，复选；3-无效，未复选；4-无效，复选
		nSrcX = ((IsEnabled()) ? 0:(2*w));
		nSrcX += (m_nState == RS_UNCHECK) ? 0:w;

		int y = m_rcWnd.top + (GetHeight() - h)/2;
		RECT rc; // 复选框图片的矩形
		rc.left = m_rcWnd.left;
		rc.top = y;
		rc.right = rc.left + w;
		rc.bottom = rc.top + h;
		IntersectRect(&rc, &rc, &rcClip);

		DWORD style = BLIT_NORMAL;
		
		if(m_nAlpha < 256)
		{
			m_pBmp->SetUserData(m_nAlpha);
			style |= BLIT_TRANS;
		}
		
		if(IsMask())
			style |= BLIT_MASK;

		m_pBmp->Draw(pCanvas, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, nSrcX+rc.left-m_rcWnd.left, rc.top-y , style);
	}
	
	// 标题文字
	if (!m_strCaption.empty())
	{
		ITTFont* pTTFont = GetFont();
		int x = m_rcWnd.left + w + DX_RADIO;
		int y = m_rcWnd.top + (GetHeight() - pTTFont->GetHeight())/2;
		
		pCanvas->DrawClipText(pTTFont, x, y, m_strCaption.c_str(), GetForeColor(), rcClip, FONT_NOALPHA);
	}

	XWindow::OnDraw(pCanvas, rcClip);	
}
//------------------------------------------------------------------------
DWORD XRadio::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	if (IsCapture())
	{
		if (m_pGroup && PtInRect(&m_rcWnd, pt))
		{
			m_pGroup->SelectRadio(this);

			s_selected();

			InvalidRect();
		}
	}
	
	return XWindow::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
void XRadio::OnMouseEnter()
{
	SetCursor(CT_HAND);
}
//------------------------------------------------------------------------
void XRadio::OnMouseExit()
{
	SetCursor(CT_ARROW);
}
//------------------------------------------------------------------------
void XRadio::LoadAllImage()
{
	XWindow::LoadAllImage();
	if (m_pResObj)
		SetStateBitmap(CImageLoader::GetInstance()->LoadImage(m_pResObj->GetPropertyString("stateimg")));
}
//------------------------------------------------------------------------
void XRadio::ReleaseAllImage()
{
	XWindow::ReleaseAllImage();
	SAFE_RELEASE_IMAGE(m_pBmp);
}
//------------------------------------------------------------------------