/**
*	created:		2012-6-26   3:14
*	filename: 		Adapter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "Adapter.h"
#include "../GameApp/CommonFunc.h"
#include <wingdi.h>
//------------------------------------------------------------------------
CAdapter::CAdapter()
{
	m_bWndMode = FALSE;

	m_OldDevMode.dmFields |= (DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY);
	m_OldDevMode.dmSize = sizeof(DEVMODE);

	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&m_OldDevMode);
}
//------------------------------------------------------------------------
CAdapter::~CAdapter()
{
	if(!m_bWndMode)ChangeDisplaySettings(&m_OldDevMode,CDS_FULLSCREEN);
}
//------------------------------------------------------------------------
BOOL CAdapter::FullScreen(DEVMODE& RecDevMode)
{
	RecDevMode.dmDisplayOrientation = DMDO_DEFAULT;
	RecDevMode.dmFields |= DM_DISPLAYORIENTATION;

	long lRet = ChangeDisplaySettings(&RecDevMode, CDS_FULLSCREEN);
	return DISP_CHANGE_SUCCESSFUL == lRet;
}
//------------------------------------------------------------------------
BOOL CAdapter::TestMode(DEVMODE& dm)
{
	dm.dmFields |= (DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY);
	long lRet = ChangeDisplaySettings(&dm, CDS_TEST);
	return DISP_CHANGE_SUCCESSFUL == lRet;
}
//------------------------------------------------------------------------
BOOL CAdapter::ToggleFullScreen(int nWidth, int nHeight, int nColorDepth, int nFrequency)
{
	DEVMODE DevMode;
	DEVMODE RecDevMode;
	int nIndex = 0;
	RecDevMode.dmDisplayFrequency	= 0;
	bool bHaveOne	= false;

	//win9x下，取到的刷新率是0
	DWORD dwStandard = (m_OldDevMode.dmDisplayFrequency > 0 ? m_OldDevMode.dmDisplayFrequency : nFrequency);

	PrintEventLn("OriginFreq = %d",m_OldDevMode.dmDisplayFrequency);

	while( EnumDisplaySettings(NULL,nIndex,&DevMode)!=0 )
	{
		if(DevMode.dmPelsWidth == (DWORD)nWidth && DevMode.dmPelsHeight == (DWORD)nHeight && DevMode.dmBitsPerPel == (DWORD)nColorDepth)
		{
			PrintEventLn("%d*%d*%d %d",DevMode.dmPelsWidth,DevMode.dmPelsHeight,DevMode.dmBitsPerPel,DevMode.dmDisplayFrequency);
			if(DevMode.dmDisplayFrequency <= dwStandard)
			{
				{
					PrintEventLn("TestMode Ok! %d*%d*%d %d",DevMode.dmPelsWidth,DevMode.dmPelsHeight,DevMode.dmBitsPerPel,DevMode.dmDisplayFrequency);
					if(DevMode.dmDisplayFrequency >= RecDevMode.dmDisplayFrequency)
					{
						memcpy(&RecDevMode,&DevMode,sizeof(DEVMODE));
						bHaveOne	= true;
					}
				}
			}
		}
		nIndex++;
	}

	BOOL bRet = FALSE;
	if(bHaveOne)
	{
		if(RecDevMode.dmDisplayFrequency == 0)
			RecDevMode.dmDisplayFrequency = nFrequency;

		bRet = FullScreen(RecDevMode);

		PrintEventLn("DEVMODE = %d*%d*%d %d",DevMode.dmPelsWidth,DevMode.dmPelsHeight,DevMode.dmBitsPerPel,DevMode.dmDisplayFrequency);
	}

	return bRet;
}
//------------------------------------------------------------------------
BOOL CAdapter::ToggleBack()
{
	return DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&m_OldDevMode,CDS_FULLSCREEN);
}
//------------------------------------------------------------------------
// 在两个分辨间切换
// 返回：窗口模式返回TRUE，否则返回FALSE
BOOL CAdapter::ToggleMode(HWND hWnd,int nWidth, int nHeight, int nColorDepth, int nFrequency)
{
	static RECT mainRect;
	BOOL bOk = FALSE;
	if(m_bWndMode)
	{
		if(ToggleFullScreen(nWidth,nHeight,nColorDepth,nFrequency))
		{
			GetWindowRect(hWnd,&mainRect);
			// remove the caption of the mainWnd:
			LONG style=::GetWindowLong(hWnd,GWL_STYLE);
			style &= ~WS_CAPTION;
			::SetWindowLong(hWnd,GWL_STYLE,style);

			int screenx=GetSystemMetrics(SM_CXSCREEN);
			int screeny=GetSystemMetrics(SM_CYSCREEN);
			int xTop = GetSystemMetrics(SM_CXDLGFRAME) + GetSystemMetrics(SM_CXBORDER) - 1;
			int yTop = GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYBORDER) - 1;

			// resize:
			SetWindowPos(hWnd,NULL,/*-xTop*/0,/*-yTop*/0,screenx/*+8*/,screeny/*+8*/,SWP_NOZORDER);

			m_bWndMode = FALSE;
			bOk = TRUE;
		}
	}
	else
	{
		if(ToggleBack())
		{
			LONG style=::GetWindowLong(hWnd,GWL_STYLE);
			style|=WS_CAPTION;
			SetLastError(0);
			long l = ::SetWindowLong(hWnd,GWL_STYLE,style);
			DWORD dw = GetLastError();
			DWORD	dwScreenWidth=GetSystemMetrics(SM_CXSCREEN);
			DWORD	dwScreenHeight=GetSystemMetrics(SM_CYSCREEN);
			RECT rcWindow = {0,0,nWidth,nHeight};
			dw = GetWindowLong(hWnd,GWL_STYLE);
			HMENU h = GetMenu(hWnd);
			AdjustWindowRectEx(&rcWindow,GetWindowLong(hWnd,GWL_STYLE),GetMenu(hWnd) != 0,GetWindowLong(hWnd,GWL_EXSTYLE));
			rcWindow.right = rcWindow.right - rcWindow.left;
			rcWindow.bottom = rcWindow.bottom - rcWindow.top;
			rcWindow.left=(int)((int)dwScreenWidth-(int)rcWindow.right)/2;
			rcWindow.top=(int)((int)dwScreenHeight-(int)rcWindow.bottom)/2;
			rcWindow.right+=rcWindow.left;
			rcWindow.bottom+=rcWindow.top;
			
			MoveWindow(hWnd,rcWindow.left,rcWindow.top,rcWindow.right-rcWindow.left,rcWindow.bottom-rcWindow.top,TRUE);
			ShowWindow(hWnd,SW_SHOWNORMAL);	
			UpdateWindow(hWnd);

			m_bWndMode = TRUE;
			bOk = TRUE;
		}
	}
	return m_bWndMode;
}
//------------------------------------------------------------------------