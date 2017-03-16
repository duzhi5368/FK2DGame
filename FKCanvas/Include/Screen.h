/**
*	created:		2012-6-20   20:15
*	filename: 		Screen
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../IFKScreen.h"
#include "../Include/WindowCanvas.h"
#include "../Include/Timer.h"
//------------------------------------------------------------------------
class CScreen : public IScreen  
{
private:
	WCanvas m_canvas;
	HWND	m_hwnd;
	int m_dx, m_dy;
	int m_nWidth, m_nHeight;
	WBitmap m_bmp;
	WTimer m_timer;
	WTimer m_timerFPS;
	int m_nFrames;
	int m_nAimFrames;
	BOOL m_bShowFPS;
	BOOL m_bLimitSpeed;
public:
	CScreen();
	virtual ~CScreen();
public:
	virtual BOOL Create(HWND hwnd, int x, int y, int w, int h);	//创建
	virtual ICanvas* GetCanvas();
	virtual BOOL MessageHandle(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnUpdate();						//更新窗口
	virtual void Release();							//释放结口
	virtual HWND GetHWnd(){return m_hwnd;}
	virtual void KeepFrames(int frames)
	{
		m_bLimitSpeed = TRUE;
		m_nAimFrames = frames;
	}
	virtual void ShowFPS(BOOL b)
	{
		m_bShowFPS = b;
	}
	virtual void SetKeepFrames(BOOL b)
	{
		m_bLimitSpeed = b;
	}
};
//------------------------------------------------------------------------