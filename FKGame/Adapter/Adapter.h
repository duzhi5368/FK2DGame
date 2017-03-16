/**
*	created:		2012-6-26   3:13
*	filename: 		Adapter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		用于调整显示器的分辨率和颜色深度等参数
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CAdapter  
{
protected:
	CAdapter();
	BOOL FullScreen(DEVMODE& dm);
public:
	~CAdapter();

	static CAdapter *GetInstance()
	{
		static CAdapter a;
		return &a;
	}

	BOOL ToggleMode(HWND hWnd,int nWidth, int nHeight, int nColorDepth, int nFrequency);
	BOOL ToggleBack();
	BOOL ToggleFullScreen( int nWidth, int nHeight,int nColorDepth, int nFrequency );

	BOOL TestMode(DEVMODE& dm);
	BOOL IsFullscreen()	{return !m_bWndMode;}
private:
	DEVMODE m_OldDevMode;
	BOOL	m_bWndMode;
};
//------------------------------------------------------------------------