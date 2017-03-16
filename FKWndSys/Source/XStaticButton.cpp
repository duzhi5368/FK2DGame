/**
*	created:		2012-6-24   0:14
*	filename: 		XStaticButton
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKResManager/IFKResManager.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XStateButton::XStateButton()
{
	m_nState = BS_NORMAL;
	m_pBmpState1 = m_pBmpState2 = NULL;
	m_bShowState1 = true;
}
//------------------------------------------------------------------------
XStateButton::XStateButton(XWindow* pParentWnd) :XWindow(pParentWnd)
{
	m_nState = BS_NORMAL;
	m_pBmpState2 = m_pBmpState1 = NULL;
	m_bShowState1 = true;
}
//------------------------------------------------------------------------
XStateButton::~XStateButton()
{
}
//------------------------------------------------------------------------
void XStateButton::DestroyWindow()
{
	m_pWndBitmap = NULL;
	SAFE_RELEASE_IMAGE(m_pBmpState1);
	SAFE_RELEASE_IMAGE(m_pBmpState2);
	XWindow::DestroyWindow();
}
//------------------------------------------------------------------------
DWORD XStateButton::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	if (nButton == LeftButton)
	{
		WritePoint(pt);
		InvalidRect();
	}

	return XWindow::OnMouseDown(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XStateButton::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	if (nButton == LeftButton)
	{
		if (PtInRect(&m_rcWnd, pt))
		{
			m_bShowState1 = !m_bShowState1;
			if (m_bShowState1)
				SetBitmap(m_pBmpState1);
			else
				SetBitmap(m_pBmpState2);
		}
		InvalidRect();
	}
	
	return XWindow::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
void XStateButton::OnMouseEnter()
{
	SetCursor(CT_HAND);
}
//------------------------------------------------------------------------
void XStateButton::OnMouseExit()
{
	SetCursor(CT_ARROW);
}
//------------------------------------------------------------------------
void XStateButton::SetState(int nState)
{
	bool show_state1 = (nState==0)?true:false;
	if (show_state1 == m_bShowState1) return;
	m_bShowState1 = show_state1;
	if (m_bShowState1)
		SetBitmap(m_pBmpState1);
	else
		SetBitmap(m_pBmpState2);
	InvalidRect();
}
//------------------------------------------------------------------------
void XStateButton::LoadAllImage()
{
	XWindow::LoadAllImage();
	if (m_pResObj)
	{
		SetBitmapState2(CImageLoader::GetInstance()->LoadImage(m_pResObj->GetPropertyString("state2img")));
		SetBitmapState1(CImageLoader::GetInstance()->LoadImage(m_pResObj->GetPropertyString("state1img")));
	}
}
//------------------------------------------------------------------------
void XStateButton::ReleaseAllImage()
{
	XWindow::ReleaseAllImage();
	SAFE_RELEASE_IMAGE(m_pBmpState2);
	SAFE_RELEASE_IMAGE(m_pBmpState1);
}
//------------------------------------------------------------------------