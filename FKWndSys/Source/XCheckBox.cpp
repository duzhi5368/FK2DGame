/**
*	created:		2012-6-23   23:35
*	filename: 		XCheckBox
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKResManager/IFKResManager.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XCheckBox::XCheckBox()
{
	m_pBmp = NULL;
	m_nState = CBS_UNCHECK;
}
//------------------------------------------------------------------------
XCheckBox::XCheckBox(XWindow* pParentWnd) :XWindow(pParentWnd)
{
	m_pBmp = NULL;
	m_nState = CBS_UNCHECK;
}
//------------------------------------------------------------------------
XCheckBox::~XCheckBox()
{
}
//------------------------------------------------------------------------
void XCheckBox::DestroyWindow()
{
	SAFE_RELEASE_IMAGE(m_pBmp);
	XWindow::DestroyWindow();
}
//------------------------------------------------------------------------
void XCheckBox::OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip)
{
}
//------------------------------------------------------------------------
void XCheckBox::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	// 状态方框
	int w = 0, h, nSrcX;
	if (m_pBmp)
	{
		w = m_pBmp->GetWidth() / 4; // 4张图
		h = m_pBmp->GetHeight();

		// 4张图的排列顺序：1-有效,未复选；2-有效，复选；3-无效，未复选；4-无效，复选
		nSrcX = ((IsEnabled()) ? 0:(2*w));
		nSrcX += (m_nState == CBS_UNCHECK) ? 0:w;

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
		int x = m_rcWnd.left + w + 5;
		int y = m_rcWnd.top + (GetHeight() - pTTFont->GetHeight())/2;
		
		pCanvas->DrawClipText(pTTFont, x, y, m_strCaption.c_str(), GetForeColor(), rcClip, FONT_NOALPHA);
	}

	XWindow::OnDraw(pCanvas, rcClip);	
}
//------------------------------------------------------------------------
DWORD XCheckBox::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	if (IsCapture())
	{
		if (PtInRect(&m_rcWnd, pt))
		{
			if (m_nState == CBS_UNCHECK)
				SetState(CBS_CHECK);
			else
				SetState(CBS_UNCHECK);
			
			BOOL bChecked = m_nState == CBS_CHECK;
			s_checked((DWORD)&bChecked);
			
			InvalidRect();
		}
	}

	return XWindow::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
void XCheckBox::OnMouseEnter()
{
	SetCursor(CT_HAND);
}
//------------------------------------------------------------------------
void XCheckBox::OnMouseExit()
{
	SetCursor(CT_ARROW);
}
//------------------------------------------------------------------------
void XCheckBox::LoadAllImage()
{
	XWindow::LoadAllImage();
	if (m_pResObj)
		SetStateBitmap(CImageLoader::GetInstance()->LoadImage(m_pResObj->GetPropertyString("stateimg")));
}
//------------------------------------------------------------------------
void XCheckBox::ReleaseAllImage()
{
	XWindow::ReleaseAllImage();
	SAFE_RELEASE_IMAGE(m_pBmp);
}
//------------------------------------------------------------------------