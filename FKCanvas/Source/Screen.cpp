/**
*	created:		2012-6-20   20:22
*	filename: 		Screen
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/Screen.h"
#include "../Include/StructCommon.h"
#include "../Include/CanvasCommon.h"
//------------------------------------------------------------------------
CFKTrace g_DefaultTrace;
//------------------------------------------------------------------------
CScreen::CScreen()
{
	m_hwnd = NULL;
	m_dx = m_dy = 0;
	m_nWidth = m_nHeight = 0;

	m_nFrames = 0;
	m_nAimFrames = 40;
	m_bLimitSpeed = FALSE;
#ifdef _DEBUG
	m_bShowFPS = TRUE;
#else
	m_bShowFPS = FALSE;
#endif
}
//------------------------------------------------------------------------
CScreen::~CScreen()
{

}
//------------------------------------------------------------------------
ICanvas* CScreen::GetCanvas()
{
	return static_cast<ICanvas*>(&m_canvas);
}
//------------------------------------------------------------------------
void CScreen::Release()
{
	delete this;
}
//------------------------------------------------------------------------
BOOL CScreen::Create(HWND hwnd, int x, int y, int w, int h)
{
	if(hwnd == NULL)
	{
		FKTraceLn("CScreen::Create the hwnd is invalid");
		return FALSE;
	}

	if(!m_bmp.Create(w, h))
	{
		FKTraceLn("CScreen::Create, the bitmap create failed!");
		return FALSE;

	}
	m_canvas.SelectBitmap(static_cast<IBitmap*>(&m_bmp));

	m_dx = x;
	m_dy = y;
	m_nWidth = w;
	m_nHeight = h;
	m_hwnd = hwnd;
	m_canvas.AddWholeScreen();

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CScreen::OnUpdate()
{
	m_canvas.MergeOut();

	HDC hdc = GetDC(m_hwnd);

	if(m_bShowFPS)
	{
		char buf[50];
		sprintf(buf, "Frame: %d", m_nFrames);
		m_canvas.Draw(10, 10, buf, WPixel(255, 255, 255));
	}

	BOOL ret = m_canvas.Update2DC(hdc, m_dx, m_dy, 0, 0, m_nWidth, m_nHeight);
	ReleaseDC(m_hwnd, hdc);

	if(!m_canvas.IsInEqualOut())
	{

		m_canvas.ResetOutDirtyRect();
		m_canvas.ResetDirtyRect();
	}
	else
		m_canvas.ResetDirtyRect();


	static DWORD oldtime, framecount;
	DWORD curtime;

	framecount ++;

	curtime = m_timerFPS.GetTime();
	if(curtime - oldtime >= 10000)
	{
		m_nFrames = framecount;
		framecount = 0;
		oldtime = curtime;
	}

	if(m_bLimitSpeed)
	{

		int lastframe;
		int speed = 10000/m_nAimFrames;
		while ((lastframe=m_timer.GetTime())<speed) {
			Sleep(5);
		}
		lastframe-=speed;
		if (lastframe>10000) m_timer.Reset();
		else m_timer.Reset(-lastframe);
	}
	if(m_bShowFPS)
	{
		m_canvas.AddDirtyRect(10, 10, 90, 10);
		if(!m_canvas.IsInEqualOut())
			m_canvas.AddOutDirtyRect(10, 10, 90, 10);
	}

	return ret;
}
//------------------------------------------------------------------------
BOOL CScreen::MessageHandle(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		{
			if(!m_canvas.IsValid() || !m_hwnd)
				return TRUE;


			PAINTSTRUCT ps;
			//HDC hdc = BeginPaint(m_hwnd, &ps);
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rc = ps.rcPaint;

			if(IsRectEmpty(&rc))
			{
				EndPaint(hWnd, &ps);
				return TRUE;
			}

			if(rc.left < m_dx)
				rc.left = m_dx;
			if(rc.top < m_dy)
				rc.top = m_dy;
			if(rc.right > m_dx + m_nWidth)
				rc.right = m_dx + m_nWidth;
			if(rc.bottom  > m_dy + m_nHeight)
				rc.bottom = m_dy + m_nHeight;

			BOOL ret = m_canvas.Blit2DC(hdc, rc.left, rc.top, rc.left - m_dx,
				rc.top - m_dy, rc.right - rc.left, rc.bottom - rc.top);

			if(!ret)
				FKTraceLn("CScreen, onUpdate blit2dc failed!");

			EndPaint(hWnd, &ps);
			return ret;
		}
		break;

	case WM_SIZE:
		{
			if(!m_canvas.IsValid() || wParam == SIZE_MINIMIZED)
				return TRUE;

			int cx = LOWORD(lParam);
			int cy = HIWORD(lParam);

			if(cx == m_nWidth && cy == m_nHeight)
				return TRUE;

			BOOL ret = m_canvas.Create(cx, cy);

			if(!ret)
			{
				FKTraceLn("CScreen, resize failed!");
				return FALSE;
			}

			m_nWidth = cx;
			m_nHeight = cy;

			return ret;
		}
		break;

	default:
		break; 
	}
	return TRUE;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateIScreen(DWORD dwVersion, IScreen** ppScreen, ITrace* pTrace)
{

#ifdef _DEBUG
	if(pTrace)
		g_DefaultTrace.Create(pTrace);
#endif

	CScreen* pScreen = new CScreen;

	if(pScreen == NULL)
	{
		FKTraceLn("CreatIScreen failed!");
		return FALSE;
	}

	*ppScreen = static_cast<IScreen*>(pScreen);
	return TRUE;
}
//------------------------------------------------------------------------