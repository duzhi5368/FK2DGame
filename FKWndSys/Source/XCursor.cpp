/**
*	created:		2012-6-23   20:51
*	filename: 		XCursor
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
IBitmap*	XCursor::m_pAttachWnd = NULL;
DWORD		XCursor::m_dwUserData = 0;
BOOL		XCursor::m_bUseOwnCursor = 0;
//------------------------------------------------------------------------
XCursor::XCursor()
{
	m_bAnimate	= 0;
	m_nInterval	= 0;
	m_nFrames	= 1;
	m_ptHot.x	= 0;
	m_ptHot.y	= 0;
	m_nCurrentFrame = 0;
	m_ptCursor.x = 0;
	m_ptCursor.y = 0;
	m_pWndBitmap = NULL;
	SetRectEmpty(&m_rcWnd);
}
//------------------------------------------------------------------------
XCursor::~XCursor()
{
}
//------------------------------------------------------------------------
void XCursor::Release()
{
	if (XCursor::m_bUseOwnCursor)
		InvalidRect(NULL);
	if (m_bAnimate)
		XDesktop::GetInstance()->GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
	SAFE_RELEASE_IMAGE(m_pWndBitmap);
	delete this;
}
//------------------------------------------------------------------------
void XCursor::OnTimer(DWORD dwEventID)
{
	if (!m_bAnimate || !XCursor::m_bUseOwnCursor)
		return;

	if (++m_nCurrentFrame >= m_nFrames)
		m_nCurrentFrame = 0;

	m_ptCursor.x = m_pWndBitmap->GetWidth()*m_nCurrentFrame/m_nFrames;

	InvalidRect();
}
//------------------------------------------------------------------------
void XCursor::InvalidRect(LPRECT lpRect)
{
	//增加绘制矩形
	RECT rc;
	if (lpRect)
		CopyRect(&rc, lpRect);
	else
		CopyRect(&rc, &m_rcWnd);
	
	ICanvas* pCanvas = XDesktop::GetInstance()->GetCanvas();
	pCanvas->AddDirtyRect(rc);
	if (!pCanvas->IsInEqualOut())
		pCanvas->AddOutDirtyRect(rc);
}
//------------------------------------------------------------------------
void XCursor::GetMouseXY(POINT& pt)
{
	pt.x = m_rcWnd.left + m_ptHot.x;
	pt.y = m_rcWnd.top + m_ptHot.y;
}
//------------------------------------------------------------------------
void XCursor::SetMouseXY(POINT& pt)
{
	int dx = pt.x  - m_ptHot.x - m_rcWnd.left;
	int dy = pt.y  - m_ptHot.y - m_rcWnd.top;
	OffsetRect(&m_rcWnd, dx, dy);
}
//------------------------------------------------------------------------
void XCursor::SetAttach(IBitmap* pAttachWnd) 
{
	// 如果当前有吸附物，则将他的矩形区域设脏(但不要释放图片，因为不是我创建的)
	IBitmap* pOldBmp = GetAttach();
	if (pOldBmp)
	{
		int bmpX,bmpY;
		RECT rc;
		bmpX = m_rcWnd.left + m_ptHot.x - pOldBmp->GetWidth()/2;
		bmpY = m_rcWnd.top + m_ptHot.y - pOldBmp->GetHeight()/2;
		SetRect(&rc, bmpX, bmpY, bmpX+pOldBmp->GetWidth(), bmpY+pOldBmp->GetHeight());
		InvalidRect(&rc);
	}

	XCursor::m_pAttachWnd = pAttachWnd;
	if (!pAttachWnd) return;
}
//------------------------------------------------------------------------
DWORD XCursor::MoveTo(POINT pt)
{
	int dx = pt.x - m_rcWnd.left - m_ptHot.x;
	int dy = pt.y - m_rcWnd.top - m_ptHot.y;
	
	// 光标没有移动
	if(dx == 0 && dy == 0)
		return 0;

	IBitmap* pAttachBmp = GetAttach();
	if (pAttachBmp)
	{ 
		int bmpX,bmpY;
		RECT rc;
		bmpX = m_rcWnd.left + m_ptHot.x - pAttachBmp->GetWidth()/2;
		bmpY = m_rcWnd.top + m_ptHot.y - pAttachBmp->GetHeight()/2;
		SetRect(&rc, bmpX, bmpY, bmpX+pAttachBmp->GetWidth(), bmpY+pAttachBmp->GetHeight());
		InvalidRect(&rc);
		OffsetRect(&m_rcWnd, dx, dy);
		OffsetRect(&rc, dx, dy);
		InvalidRect(&rc);
	} 
	else
	{
		if (XCursor::m_bUseOwnCursor)
			InvalidRect();	// 更新旧区域
		OffsetRect(&m_rcWnd, dx, dy);
		if (XCursor::m_bUseOwnCursor)
			InvalidRect();	// 更新新区域
	}

	return 0;
}
//------------------------------------------------------------------------
void XCursor::OnDraw(ICanvas* pCanvas)
{
	if (!XCursor::m_bUseOwnCursor) return;
	if (!pCanvas->IsInDirty(m_rcWnd))
		return;

	if(!m_pWndBitmap)	//无背景
		return;

	DWORD style = BLIT_NORMAL;
	if(m_bMask)
		style |= BLIT_MASK;

	// 如果光标上有吸附图片，则仅仅绘制吸附图片
	IBitmap* pAttachBmp = GetAttach();
	if (pAttachBmp)
	{
		
		int nBitmapWidth = pAttachBmp->GetWidth();
		int nBitmapHeight = pAttachBmp->GetHeight();
 
		int bmpX,bmpY;
		bmpX = m_rcWnd.left + m_ptHot.x - nBitmapWidth / 2;
		bmpY = m_rcWnd.top + m_ptHot.y - nBitmapHeight / 2;

		if (nBitmapWidth < 35)
		{
			RECT rcBox;	
			rcBox.left = bmpX - 1;
			rcBox.top = bmpY- 1;
			rcBox.right = bmpX + nBitmapWidth + 1;
			rcBox.bottom = bmpY + nBitmapHeight + 1;
			pCanvas->DrawBox(rcBox, WPixel(255,174,8));
		}
		 
		pAttachBmp->Draw(pCanvas, bmpX, bmpY, style);
		return;
	}

	// 绘制光标
	if (m_nAlpha == 0) return;
	if(m_nAlpha < 256)
	{
		m_pWndBitmap->SetUserData(m_nAlpha);
		style |= BLIT_TRANS;
	}

	if(m_bAnimate)
		m_pWndBitmap->Draw(pCanvas, m_rcWnd.left , m_rcWnd.top, m_rcWnd.right - m_rcWnd.left,
			m_rcWnd.bottom - m_rcWnd.top,
			m_ptCursor.x, 0, style);
	else
		m_pWndBitmap->Draw(pCanvas, m_rcWnd.left, m_rcWnd.top, style);
}
//------------------------------------------------------------------------
// 显示和隐藏光标
void XCursor::Show(BOOL bShow /* = TRUE */)
{
	if (!XCursor::m_bUseOwnCursor) return;
	if (m_bAnimate && bShow)
		XDesktop::GetInstance()->GetTimeAxis()->SetTimer(0, static_cast<ITimerSink*>(this), m_nInterval);
	if (!bShow)
		XDesktop::GetInstance()->GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
}
//------------------------------------------------------------------------
// 消息相关
DWORD XCursor::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}
//------------------------------------------------------------------------
void XCursor::SetCursorOwnDraw(BOOL bOwnDraw)
{
	XCursor::m_bUseOwnCursor = bOwnDraw;
	if (XCursor::m_bUseOwnCursor)
	{
		::SetCursor(NULL);
		Show(TRUE);
	}
	else
	{
		Show(FALSE);
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		InvalidRect();
	}
}
//------------------------------------------------------------------------