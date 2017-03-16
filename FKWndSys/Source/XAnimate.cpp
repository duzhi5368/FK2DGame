/**
*	created:		2012-6-23   23:31
*	filename: 		XAnimate
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XAnimate::XAnimate() : m_nFrameCount(0), m_nCurFrame(0), m_nDelay(0), 
m_nOffX(0), m_nFrameWidth(0), m_nState(statePlay)
{

}
//------------------------------------------------------------------------
XAnimate::XAnimate(XWindow* pParentWnd) : XWindow(pParentWnd), m_nFrameCount(0), 
m_nCurFrame(0), m_nDelay(0), m_nOffX(0), m_nFrameWidth(0), m_nState(statePlay)
{
}
//------------------------------------------------------------------------
XAnimate::~XAnimate()
{

}
//------------------------------------------------------------------------
void XAnimate::DestroyWindow()
{
	if (m_nDelay)
		GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
	XWindow::DestroyWindow();
}
//------------------------------------------------------------------------
void XAnimate::OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip)
{
	if (!m_pWndBitmap) return;
	DWORD style = BLIT_NORMAL;
	
	if (m_nAlpha < 256)
	{
		m_pWndBitmap->SetUserData(m_nAlpha);
		style |= BLIT_TRANS;
	}
	
	if (IsMask())
		style |= BLIT_MASK;

	if (m_nFrameWidth == 0)
		m_nFrameWidth = m_pWndBitmap->GetWidth() / m_nFrameCount;

	m_pWndBitmap->Draw(pCanvas, rcClip.left, rcClip.top, 
		rcClip.right - rcClip.left, rcClip.bottom - rcClip.top, 
		rcClip.left - m_rcWnd.left + m_nOffX, rcClip.top - m_rcWnd.top, style);
}
//------------------------------------------------------------------------
void XAnimate::OnTimer(DWORD dwEventID)
{
	if (m_pWndBitmap == NULL || m_nFrameCount == 0)
		return;
	if (m_nFrameWidth == 0)
		m_nFrameWidth = m_pWndBitmap->GetWidth() / m_nFrameCount;
	if (++m_nCurFrame >= m_nFrameCount)
	{
		m_nCurFrame = 0;
		m_nOffX = 0;
		return;
	}
	m_nOffX += m_nFrameWidth;
	InvalidRect();
}
//------------------------------------------------------------------------
void XAnimate::SetDelay(int nDelay)				
{
	if (m_nDelay)
		GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
	
	m_nDelay = nDelay;

	if (m_nDelay > 0)
		GetTimeAxis()->SetTimer(0, static_cast<ITimerSink*>(this), m_nDelay);
}
//------------------------------------------------------------------------
void XAnimate::Play()
{
	SetCurFrame(0);
	if (IsPlaying()) return;
	if (m_nDelay > 0)
		GetTimeAxis()->SetTimer(0, static_cast<ITimerSink*>(this), m_nDelay);
}
//------------------------------------------------------------------------
void XAnimate::Pause()
{
	if (!IsPlaying()) return;
	if (m_nDelay)
		GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
}
//------------------------------------------------------------------------
void XAnimate::Resume()
{
	if (IsPlaying()) return;
	if (m_nDelay > 0)
		GetTimeAxis()->SetTimer(0, static_cast<ITimerSink*>(this), m_nDelay);
}
//------------------------------------------------------------------------
void XAnimate::SetCurFrame(int nFrame)
{
	if (m_nCurFrame != nFrame)
		InvalidRect();
	m_nCurFrame = nFrame;
}
//------------------------------------------------------------------------