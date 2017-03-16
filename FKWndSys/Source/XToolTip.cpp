/**
*	created:		2012-6-23   21:07
*	filename: 		XToolTip
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XToolTip::XToolTip()
{
	m_cForeColor = WPixel(0, 255, 0);
	m_nAlpha	 = 96;
	m_nMargin	 = 2;
	m_nLineSpace = 2;
	m_nInterval  = 0;
}
//------------------------------------------------------------------------
XToolTip::XToolTip(XWindow* pParentWnd) :XWindow(pParentWnd)
{
	m_cForeColor = WPixel(0, 255, 0);
	m_nAlpha	 = 96;
	m_nMargin	 = 2;
	m_nLineSpace = 2;
	m_nInterval  = 0;
}
//------------------------------------------------------------------------
XToolTip::~XToolTip()
{
}
//------------------------------------------------------------------------
void XToolTip::LimitedToScreen(LPRECT lpRect)
{
	SIZE size = GetDesktop()->GetDesktopSize();
	if (lpRect->left < 0)
		OffsetRect(lpRect, -lpRect->left, 0);
	if (lpRect->top < 0)
		OffsetRect(lpRect, 0, -lpRect->top);
	if (lpRect->right > size.cx)
		OffsetRect(lpRect, size.cx-lpRect->right, 0);
	if (lpRect->bottom > size.cy)
		OffsetRect(lpRect, 0, size.cy-lpRect->bottom);
}
//------------------------------------------------------------------------
void XToolTip::CalcPos(LPPOINT lpptPos)
{
	m_rcWnd.left = lpptPos->x; m_rcWnd.top = lpptPos->y;
	if (m_pWndBitmap)
	{
		m_rcWnd.right = m_rcWnd.left + m_pWndBitmap->GetWidth();
		m_rcWnd.bottom = m_rcWnd.top + m_pWndBitmap->GetHeight();
		LimitedToScreen(&m_rcWnd);
		return;
	}

	// text
	if (m_strCaption.empty())
	{
		m_rcWnd.right = m_rcWnd.left + m_nMargin*2;
		m_rcWnd.bottom = m_rcWnd.top + GetFont()->GetHeight() + m_nMargin*2;
		return;
	}
 
	char* pStr = (char*)m_strCaption.c_str();
	char* pStart = pStr;
	char buf[256];
	int nMaxWidth = 0;
	int nRaw = 0;
	while (*pStr != 0)
	{
		if (*pStart == '[' && *(pStart+3) == ']')
			pStart += 4;

		if (__isNewLine(pStr))
		{
			memcpy(buf, pStart, pStr - pStart);
			buf[pStr - pStart] = '\0';
			int nWidth = GetFont()->GetLength(buf);
			if (nWidth > nMaxWidth) nMaxWidth=nWidth;
			nRaw ++;
			pStr ++;
			pStart = pStr + 1;
		}
		pStr ++;
	}

	memcpy(buf, pStart, pStr - pStart);
	buf[pStr - pStart] = '\0';
	int nWidth = GetFont()->GetLength(buf);
	if (nWidth > nMaxWidth) nMaxWidth=nWidth;
	nRaw ++;

	m_rcWnd.right = m_rcWnd.left + nMaxWidth + m_nMargin*2;
	m_rcWnd.bottom = m_rcWnd.top + nRaw*(GetFont()->GetHeight()+m_nLineSpace) + m_nMargin*2 - m_nLineSpace;
	LimitedToScreen(&m_rcWnd);
}
//------------------------------------------------------------------------
void XToolTip::SetNewPos(int x, int y)
{
	InvalidRect(NULL);
	::OffsetRect(&m_rcWnd, x+32-m_rcWnd.left, y+32-m_rcWnd.top); // x，y加上32是为了不让鼠标箭头覆盖在tooltip上
	LimitedToScreen(&m_rcWnd);
	InvalidRect(NULL);
}
//------------------------------------------------------------------------
// 每行文字通过这样的文字显示：[rc]MyText 注:r-红色,c-居中对齐，格式可要可不要
void XToolTip::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	ITTFont* pFont = GetFont();
	int nFontHeight = pFont->GetHeight();

	if (!m_strCaption.empty())
	{
		int x = m_rcWnd.left + m_nMargin;
		int y = m_rcWnd.top + m_nMargin;
 
		char* pStr = (char*)m_strCaption.c_str();
		char* pStart = pStr;
		char buf[1024];
		int nMaxWidth = 0;
		int nRaw = 0;
		WPixel cLineColor = m_cForeColor;
		int nAlign = 0; // 0-left,1-center,2-right
		int nLimitedWidth = GetWidth() - m_nMargin*2; // 最大受限宽度
		while (*pStr != 0)
		{
			if (*pStart == '[' && *(pStart+3) == ']') // 有格式说明，则使用格式
			{
				switch (*(pStart+1))
				{
				case 'R':
				case 'r':	// 这行用红色显示
					cLineColor = WPixel(255,0,0);
					break;
				case 'G':
				case 'g':	// 这行用绿色显示
					cLineColor = WPixel(0,255,0);
					break;
				case 'B':
				case 'b':	// 这行用蓝色显示
					cLineColor = WPixel(0,0,255);
					break;
				case 'Y':
				case 'y':	// 这行用黄色显示
					cLineColor = WPixel(255,255,0);
					break;
				case 'C':
				case 'c':	// 这行用青色显示
					cLineColor = WPixel(0,255,255);
					break;
				case 'W':
				case 'w':
					cLineColor = WPixel(255,255,255);
					break;
				case 'H':
				case 'h':	// 这行用灰显示
					cLineColor = WPixel(124,124,124);
					break;
				}

				switch (*(pStart+2))
				{
				case 'C':	// center align
				case 'c':
					nAlign = 1;
					break;
				case 'R':	// right align
				case 'r':
					nAlign = 2;
					break;
				}

				pStart += 4;
			}

			//在这里解释自己的格式串
			pStart += __PhraseNewFormat(pStart, &cLineColor, &nAlign);

			if (__isNewLine(pStr))   // 检测是否遇到了换行符
			{
				if ((*pStr == '\\' && *(pStr+1) == 'n'))
				{
					int a = 90;					
				}
				memcpy(buf, pStart, pStr - pStart);
				buf[pStr - pStart] = '\0';
				int nWidth = pFont->GetLength(buf);
				if (nWidth > nMaxWidth) nMaxWidth=nWidth;
				nRaw ++;
				pStr ++;
				pStart = pStr + 1;

				switch (nAlign) 
				{
				case 0:	// left align
					break;
				case 1: // center align
					if (nWidth < nLimitedWidth)
						x += ((nLimitedWidth - nWidth)>>1);
					break;
				case 2: // right align
					if (nWidth < nLimitedWidth)
						x += nLimitedWidth - nWidth;
					break;
				}

				pCanvas->DrawClipText(pFont, x, y, buf, cLineColor, rcClip, FONT_NOALPHA);
				x = m_rcWnd.left + m_nMargin;
				y += pFont->GetHeight() + m_nLineSpace;
				//cLineColor = m_cForeColor;
				//nAlign = 0;
			}
			pStr ++;
		}

		memcpy(buf, pStart, pStr - pStart);
		buf[pStr - pStart] = '\0';
		int nWidth = pFont->GetLength(buf);
		if (nWidth > nMaxWidth) nMaxWidth=nWidth;
		nRaw ++;

		switch (nAlign) 
		{
		case 0:	// left align
			break;
		case 1: // center align
			if (nWidth < nLimitedWidth)
				x += (nLimitedWidth - nWidth)>>1;
			break;
		case 2: // right align
			if (nWidth < nLimitedWidth)
				x += nLimitedWidth - nWidth;
			break;
		}

		pCanvas->DrawClipText(pFont, x, y, buf, cLineColor, rcClip, FONT_NOALPHA);
	}

	POINT ptFrom,ptTo;
	WPixel clr = WPixel(130,110,76);
	ptFrom.x=m_rcWnd.left-1, ptFrom.y=m_rcWnd.top-1, ptTo.x=m_rcWnd.right, ptTo.y=m_rcWnd.top-1;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 上横
	ptFrom.x=m_rcWnd.right, ptFrom.y=m_rcWnd.top-1, ptTo.x=m_rcWnd.right, ptTo.y=m_rcWnd.bottom+1;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 右竖
	ptFrom.x=m_rcWnd.right, ptFrom.y=m_rcWnd.bottom, ptTo.x=m_rcWnd.left-1, ptTo.y=m_rcWnd.bottom;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 下横
	ptFrom.x=m_rcWnd.left-1, ptFrom.y=m_rcWnd.bottom, ptTo.x=m_rcWnd.left-1, ptTo.y=m_rcWnd.top;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 左竖

	XWindow::OnDraw(pCanvas, rcClip);
}
//------------------------------------------------------------------------
//用来解释新格式 的
int XToolTip::__PhraseNewFormat(LPCSTR szStr, WPixel *pColor, int *pAlign)
{
	if (*szStr != '[' || *(szStr + 1) != ':' || *(szStr + 15) != ']')  //判断是否新的格式说明
	{
		return 0;
	}
	
	char temp[4];
	temp[3] = '\0';
	strncpy(temp, szStr + 2, 3);
	int r = atol(temp);
	strncpy(temp, szStr + 6, 3);
	int g = atol(temp);
	strncpy(temp, szStr + 10, 3);
	int b = atol(temp);
	*pColor = WPixel(r,g,b);

	switch (*(szStr + 14))
	{
		case 'L':
		case 'l':
			*pAlign = 0;
			break;
		case 'C':	// center align
		case 'c':
			*pAlign = 1;
			break;
		case 'R':	// right align
		case 'r':
			*pAlign = 2;
			break;
	}
	
	return 16;
}
//------------------------------------------------------------------------
/*
// 每行文字通过这样的文字显示：[rc]MyText 注:r-红色,c-居中对齐，格式可要可不要
void XToolTip::OnDraw(ICanvas* pCanvas)
{
	ITTFont* pFont = GetFont();
	int nFontHeight = pFont->GetHeight();

	if (!m_strCaption.empty())
	{
		// 求出与父窗口的交叉矩形
		RECT rc,rcParent;
		CopyRect(&rc, &m_rcWnd);
		GetParent()->GetWindowRect(&rcParent);
		IntersectRect(&rc, &m_rcWnd, &rcParent);
		if (IsRectEmpty(&rc))
			return;
		
		int xOff = max((LONG)0, rc.left); // 因为当rc.left < 0 时,SubBitmap()会将他偏移到0，所以要记录个偏移
		int yOff = max((LONG)0, rc.top);
		IBitmap* pSubCanvas = pCanvas->SubBitmap(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);

		int x = m_rcWnd.left + m_nMargin - xOff;
		int y = m_rcWnd.top + m_nMargin - yOff;

		char* pStr = (char*)m_strCaption.c_str();
		char* pStart = pStr;
		char buf[1024];
		int nMaxWidth = 0;
		int nRaw = 0;
		WPixel cLineColor = m_cForeColor;
		int nAlign = 0; // 0-left,1-center,2-right
		int nLimitedWidth = GetWidth() - m_nMargin*2; // 最大受限宽度
		while (*pStr != 0)
		{
			if (*pStart == '[' && *(pStart+3) == ']') // 有格式说明，则使用格式
			{
				switch (*(pStart+1))
				{
				case 'R':
				case 'r':	// 这行用红色显示
					cLineColor = WPixel(255,0,0);
					break;
				case 'G':
				case 'g':	// 这行用绿色显示
					cLineColor = WPixel(0,255,0);
					break;
				case 'B':
				case 'b':	// 这行用蓝色显示
					cLineColor = WPixel(0,0,255);
					break;
				case 'Y':
				case 'y':	// 这行用黄色显示
					cLineColor = WPixel(255,255,0);
					break;
				case 'C':
				case 'c':	// 这行用青色显示
					cLineColor = WPixel(0,255,255);
					break;
				case 'W':
				case 'w':
					cLineColor = WPixel(255,255,255);
					break;
				}

				switch (*(pStart+2))
				{
				case 'C':	// center align
				case 'c':
					nAlign = 1;
					break;
				case 'R':	// right align
				case 'r':
					nAlign = 2;
					break;
				}

				pStart += 4;
			}

			if (*pStr == '\r' && *(pStr+1) == '\n') // 遇到回车换行符
			{
				memcpy(buf, pStart, pStr - pStart);
				buf[pStr - pStart] = '\0';
				int nWidth = pFont->GetLength(buf);
				if (nWidth > nMaxWidth) nMaxWidth=nWidth;
				nRaw ++;
				pStr ++;
				pStart = pStr + 1;

				switch (nAlign) 
				{
				case 0:	// left align
					break;
				case 1: // center align
					if (nWidth < nLimitedWidth)
						x += ((nLimitedWidth - nWidth)>>1);
					break;
				case 2: // right align
					if (nWidth < nLimitedWidth)
						x += nLimitedWidth - nWidth;
					break;
				}

				pSubCanvas->DrawText(pFont, x, y, buf, cLineColor, FONT_NOALPHA);
				x = m_rcWnd.left + m_nMargin - xOff;
				y += pFont->GetHeight() + m_nLineSpace;
				cLineColor = m_cForeColor;
				nAlign = 0;
			}
			pStr ++;
		}

		memcpy(buf, pStart, pStr - pStart);
		buf[pStr - pStart] = '\0';
		int nWidth = pFont->GetLength(buf);
		if (nWidth > nMaxWidth) nMaxWidth=nWidth;
		nRaw ++;

		switch (nAlign) 
		{
		case 0:	// left align
			break;
		case 1: // center align
			if (nWidth < nLimitedWidth)
				x += (nLimitedWidth - nWidth)>>1;
			break;
		case 2: // right align
			if (nWidth < nLimitedWidth)
				x += nLimitedWidth - nWidth;
			break;
		}

		pSubCanvas->DrawText(pFont, x, y, buf, cLineColor, FONT_NOALPHA);
	}

	XWindow::OnDraw(pCanvas);
}*/
//------------------------------------------------------------------------
void XToolTip::OnTimer(DWORD dwEventID)
{
	Hide();
	GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
	return;
}
//------------------------------------------------------------------------
void XToolTip::SetInfo(IBitmap* pBitmap, LPCSTR szToolTip, WPixel cBackColor, WPixel cForeColor, int nInterval)
{
	SetBitmap(pBitmap);
	if (pBitmap) // 直接显示图片(提示文字已经做在图片上)
	{}
	else // 显示提示文字
	{
		SetText(szToolTip);
		SetBackColor(cBackColor);
		SetForeColor(cForeColor);
	}
	SetInterval(nInterval);
}
//------------------------------------------------------------------------
void XToolTip::Show(LPPOINT lpptPos)
{
	if (!lpptPos) return;

	if (IsVisible())
		InvalidRect(NULL);
 
	CalcPos(lpptPos);
	InvalidRect(NULL);
	SetVisible(TRUE);

	if (m_nInterval != 0) // 当 m_nInterval=0 时，一直显示，没有时间控制
	{
		GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
		GetTimeAxis()->SetTimer(0, static_cast<ITimerSink*>(this), m_nInterval);
	}
}
//------------------------------------------------------------------------
void XToolTip::Hide()
{
	if (!IsVisible())
		return;
	SetBitmap(NULL);
	InvalidRect(NULL);
	SetVisible(FALSE);
	m_strCaption.clear();
}
//------------------------------------------------------------------------
BOOL XToolTip::__isNewLine(LPCSTR szStr)
{
	if ((*szStr == '\r' && *(szStr+1) == '\n')
		|| (*szStr == '\\' && *(szStr+1) == 'n'))
	{
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------