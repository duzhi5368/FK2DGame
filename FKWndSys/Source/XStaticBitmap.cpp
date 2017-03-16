/**
*	created:		2012-6-24   0:15
*	filename: 		XStaticBitmap
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XStaticBitmap::XStaticBitmap()
{
	m_bStretch = FALSE;  
	m_pWndBitmap = NULL;
}
//------------------------------------------------------------------------
void XStaticBitmap::SetStretch(BOOL flag)
{
	m_bStretch = flag;  
}
//------------------------------------------------------------------------
XStaticBitmap::XStaticBitmap(XWindow* pParentWnd) : XWindow(pParentWnd)
{
	m_pWndBitmap = NULL;
}
//------------------------------------------------------------------------
XStaticBitmap::~XStaticBitmap()
{

}
//------------------------------------------------------------------------
void XStaticBitmap::OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip)
{
	if (!m_pWndBitmap) return;
	DWORD style = BLIT_NORMAL;

	if (m_nAlpha < 256)
	{
		m_pWndBitmap->SetUserData(m_nAlpha);
		style |= BLIT_TRANS;
	}
	
	if(IsMask())
		style |= BLIT_MASK;

	if (m_bStretch)
	{//重复贴图模式
		int bmpWidth = m_pWndBitmap->GetWidth();
		int bmpHeight = m_pWndBitmap->GetHeight();
		 
		int x, y;  //贴图座标
		//重复贴图
		for (x = m_rcWnd.left; x <= m_rcWnd.right; x += bmpWidth)
		{
			for (y = m_rcWnd.top; y <= m_rcWnd.bottom; y += bmpHeight)
			{
				int mapping_width = (x + bmpWidth) < m_rcWnd.right ? bmpWidth:m_rcWnd.right - x;
				int mapping_height = (y + bmpHeight) < m_rcWnd.bottom ? bmpHeight:m_rcWnd.bottom - y;
				m_pWndBitmap->Draw(pCanvas, x, y, mapping_width, mapping_height, 
					0, 0, style);
			}
		}		 
	}
	else
	{//旧的模式
		m_pWndBitmap->Draw(pCanvas, rcClip.left, rcClip.top, rcClip.right - rcClip.left, rcClip.bottom - rcClip.top, 
			rcClip.left - m_rcWnd.left, rcClip.top - m_rcWnd.top, style);
	}
}
//------------------------------------------------------------------------
/*
// 消息相关
DWORD XStaticBitmap::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		break;

	case WM_LBUTTONUP:
		//if (!GetCursor()->GetAttach())
			GetParent()->OnMessage(WM_SELECTOBJECT, MK_LBUTTON, (LPARAM)GetID());
		break;
	case WM_RBUTTONUP:
		//if (!GetCursor()->GetAttach())
			GetParent()->OnMessage(WM_SELECTOBJECT, MK_RBUTTON, (LPARAM)GetID());
		break;

	case WM_MOUSEMOVE:
		break;

	case WM_MOUSEENTER:
		SetCursor(XCT_HAND);
		break;

	case WM_MOUSEEXIT:
		SetCursor(XCT_ARROW);
		break;

	default:
		break;
	}
	return XWindow::OnMessage(uMsg, wParam, lParam);
}*/
//------------------------------------------------------------------------