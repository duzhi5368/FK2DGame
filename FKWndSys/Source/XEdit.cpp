/**
*	created:		2012-6-23   23:36
*	filename: 		XEdit
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKResManager/IFKResManager.h"
#include "../Include/FKWndSysCommon.h"
#include "../Include/KeyCode.h"
//------------------------------------------------------------------------
#define TIMERID_BEAMFLASH	1001
#define INTERVAL_BEAMFLASH	500
//------------------------------------------------------------------------
// 判断某个字符串的某个位置是否是双字节字符的前半个字符
// 如:pStr="abc我们def"
// nPos=3,5时 return ture;
inline bool IsDBCSLead(const char* pStr, int nPos)
{
	//int nLen = strlen(pStr);
	//if (nPos >= nLen) nPos = nLen - 1;
	const unsigned char* str = (const unsigned char*)pStr;
	int i = 0;
	while (str[i])
	{
		if (str[i] > 0x80 && ++i > nPos)
			return true;
		if (++i > nPos)
			return false;
	}
	return false;
}
//------------------------------------------------------------------------
// 判断某个字符串的某个位置(或位置后面)是否是双字节字符
// 如:pStr="abc我们def"
// nPos=3,4,5,6时 return ture;
// nPos=3,5时是双字节字符的Lead字节
// nPos=4,6时是双字节字符的Trail字节
inline bool IsDBCS(const char* pStr, int nPos)
{
	bool bIsDBCS = false;
	const unsigned char* str = (const unsigned char*)pStr;
	int i = 0;
	while (str[i])
	{
		bIsDBCS = false;
		if (str[i] > 0x80)
		{
			i++;
			bIsDBCS = true;
		}
		if (++i > nPos)
			break;
	}
	return bIsDBCS;
}
//------------------------------------------------------------------------
XEdit::XEdit()
{
	m_bPassword = FALSE;
	m_bReadOnly = FALSE;
	m_bDigitOnly = FALSE;
	m_bDrawBeam = FALSE;
	m_bMultiLine = FALSE;
	m_bAutoWrap = FALSE;
	m_nCaretPos = 0;
	m_nMaxLen = 255;
	m_nMargin = 2;
	m_nSelStart = -1;
	m_ptOffset.x = m_ptOffset.y = 0;
	m_ptCaretPos.x = m_ptCaretPos.y = 0;
	m_cPasswordChar = '*';
	m_nLineSpace = 2;
	m_nValidWidth = m_nValidHeight = 0;
	m_pHScrollBar = m_pVScrollBar = NULL;

	m_bStretch = FALSE;
	ZeroMemory(m_Pos, MaxAreaCount * sizeof(AREA));
} 
//------------------------------------------------------------------------
XEdit::XEdit(XWindow* pParentWnd) :XWindow(pParentWnd)
{
	m_bPassword = FALSE;
	m_bReadOnly = FALSE;
	m_bDigitOnly = FALSE;
	m_bDrawBeam = FALSE;
	m_bMultiLine = FALSE;
	m_bAutoWrap = FALSE;
	m_nCaretPos = 0;
	m_nMaxLen = 255;
	m_nMargin = 2;
	m_nSelStart = -1;
	m_ptOffset.x = m_ptOffset.y = 0;
	m_ptCaretPos.x = m_ptCaretPos.y = 0;
	m_cPasswordChar = '*';
	m_nLineSpace = 2;
	m_nValidWidth = m_nValidHeight = 0;
	m_pHScrollBar = m_pVScrollBar = NULL;

	m_bStretch = FALSE;
	ZeroMemory(m_Pos, MaxAreaCount * sizeof(AREA));
}
//------------------------------------------------------------------------
XEdit::~XEdit()
{

}
//------------------------------------------------------------------------
const XEdit::AREA * XEdit::GetGraphicsPos()
{
	return m_Pos;
}
//------------------------------------------------------------------------
void XEdit::DestroyWindow()
{
	if (IsGlobalFocus())
		GetTimeAxis()->KillTimer(TIMERID_BEAMFLASH, static_cast<ITimerSink*>(this));
	XWindow::DestroyWindow();
}
//------------------------------------------------------------------------
void XEdit::SetHScrollBar(XScrollBar* pBar)
{
	m_pHScrollBar = pBar;
	if (m_pHScrollBar)
	{
		m_pHScrollBar = static_cast<XScrollBar*>(pBar);
		CONN_posChanged(m_pHScrollBar, XEdit, OnHScrollPosChanged);
		m_pHScrollBar->Refresh();
	}
}
//------------------------------------------------------------------------
void XEdit::SetVScrollBar(XScrollBar* pBar)
{
	m_pVScrollBar = pBar;
	if (m_pVScrollBar)
	{
		m_pVScrollBar = static_cast<XScrollBar*>(pBar);
		CONN_posChanged(m_pVScrollBar, XEdit, OnVScrollPosChanged);
		m_pVScrollBar->Refresh();
	}
}
//------------------------------------------------------------------------
void XEdit::OnHScrollPosChanged(DWORD dwParam)
{
	int nPos = *(int*)dwParam;
	m_ptOffset.x = -nPos;
}
//------------------------------------------------------------------------
void XEdit::OnVScrollPosChanged(DWORD dwParam)
{
	int nPos = *(int*)dwParam;
	m_ptOffset.y = -nPos;
}
//------------------------------------------------------------------------
void XEdit::AdjustHScroll()
{
	RECT rcValid;
	rcValid.left = m_rcWnd.left + m_nMargin;
	rcValid.top = m_rcWnd.top + m_nMargin;
	rcValid.right = m_rcWnd.right - m_nMargin;
	rcValid.bottom = m_rcWnd.bottom - m_nMargin;
	int _0_25len = (rcValid.right - rcValid.left) / 4;
	int x = m_ptCaretPos.x + m_ptOffset.x;
	if (x < 0) // 插入符在矩形的左面
	{
		int dx = x +_0_25len;
		if (dx < 0)
			m_ptOffset.x = -m_ptCaretPos.x;
		else
		{
			m_ptOffset.x += dx;
			if (m_ptOffset.x > 0)
				m_ptOffset.x = 0;
		}
	}
	else if (x >= GetWidth() - 2*GetMargin())
	{
		int xOff = x - GetWidth() + 2*GetMargin();
		int dx = _0_25len - xOff;
		if (dx < 0)
			m_ptOffset.x -= xOff + 1; // 多加1是为了让最后的Caret能够显示出来
		else
		{
			m_ptOffset.x -= dx + 1;
		}
	}

	if (GetValidWidth() < rcValid.right - rcValid.left && m_ptOffset.x < 0)
		m_ptOffset.x = 0;
	
	if (IsMultiLine() && !IsAutoWrap())
	{
		if (m_pHScrollBar && GetValidWidth() > rcValid.right - rcValid.left)
		{
			m_pHScrollBar->SetVisible(TRUE);
			m_pHScrollBar->SetScrollRange(0, GetValidWidth() - rcValid.right + rcValid.left + 1);// 多加1是为了让最后的Caret能够显示出来
			m_pHScrollBar->SetVisibleCount(rcValid.right - rcValid.left);
			m_pHScrollBar->SetScrollPos(-m_ptOffset.x);
			m_pHScrollBar->Refresh();
		}
	}
	else if (m_pHScrollBar)
	{
		m_pHScrollBar->SetVisible(FALSE);
		m_pHScrollBar->SetScrollRange(0, 0);
		m_pHScrollBar->SetVisibleCount(0);
		m_pHScrollBar->SetScrollPos(0);
		m_pHScrollBar->Refresh();
	}
}
//------------------------------------------------------------------------
void XEdit::AdjustVScroll()
{
	RECT rcValid;
	rcValid.left = m_rcWnd.left + m_nMargin;
	rcValid.top = m_rcWnd.top + m_nMargin;
	rcValid.right = m_rcWnd.right - m_nMargin;
	rcValid.bottom = m_rcWnd.bottom - m_nMargin;
	int nLineHeight = GetFont()->GetHeight() + m_nLineSpace;
	int y1 = m_ptCaretPos.y + m_ptOffset.y;
	int y2 = y1 + nLineHeight;
	if (y1 < 0) // 插入符在矩形的上面
	{
		int yOff = ((-y1 + nLineHeight - 1) / nLineHeight)*nLineHeight;
		m_ptOffset.y += yOff;
	}
	else if (y2 > GetHeight() - 2*GetMargin())
	{
		int yOff = ((y2 - GetHeight() - 2*GetMargin() + nLineHeight - 1) / nLineHeight)*nLineHeight;
		m_ptOffset.y -= yOff;
	}

	if (GetValidHeight() < rcValid.bottom - rcValid.top && m_ptOffset.y < 0)
		m_ptOffset.y = 0;

	if (IsMultiLine() && m_pVScrollBar)
	{
		if (GetValidHeight() > rcValid.bottom - rcValid.top)
		{
			m_pVScrollBar->SetVisible(TRUE);
			int h = rcValid.bottom - rcValid.top;
			m_pVScrollBar->SetScrollRange(0, GetValidHeight() - h);
			m_pVScrollBar->SetVisibleCount(h);
			m_pVScrollBar->SetScrollPos(-m_ptOffset.y);
			m_pVScrollBar->Refresh();
		}
		else
		{
			m_pVScrollBar->SetVisible(FALSE);
			m_pVScrollBar->SetScrollRange(0, 0);
			m_pVScrollBar->SetVisibleCount(0);
			m_pVScrollBar->SetScrollPos(0);
			m_pVScrollBar->Refresh();
		}
	}
}
//------------------------------------------------------------------------
void XEdit::SetValidWidth(int nValidWidth)
{
	if ((!IsMultiLine() || (IsMultiLine() && !IsAutoWrap())) && nValidWidth > GetValidWidth())
		m_nValidWidth = nValidWidth;
}
//------------------------------------------------------------------------
void XEdit::SetValidHeight(int nValidHeight)
{
	if (IsMultiLine() && nValidHeight > GetValidHeight())
		m_nValidHeight = nValidHeight;
}
//------------------------------------------------------------------------
void XEdit::SetAutoWrap(BOOL bAutoWrap)
{
	if (bAutoWrap)
	{
		if (IsMultiLine())
		{
			m_bAutoWrap = bAutoWrap;
			m_nValidWidth = GetWidth() - 2*GetMargin();
			SetValidHeight(GetHeight() - 2*GetMargin());
		}
		else
		{
			TraceLn("ERROR:XEdit::SetAutoWrap, edit is not multi-line mode!");
		}
	}
	else
	{
		m_bAutoWrap = bAutoWrap;
		m_nValidWidth = GetWidth() - 2*GetMargin();
		m_nValidHeight = GetHeight() - 2*GetMargin();
	}

	if (IsRectEmpty(&m_rcWnd)) // 表明窗口还没有创建完毕，此时调用AdjustHScroll会导致m_ptOffset的改变
		return;

	CalcValidSize();
	CalcCaretPos();
	AdjustHScroll();
	AdjustVScroll();
}
//------------------------------------------------------------------------
void XEdit::OnTimer(DWORD dwEventID)
{
	if (IsGlobalFocus())
	{
		m_bDrawBeam = !	m_bDrawBeam;
		InvalidRect();
	}
}
//------------------------------------------------------------------------
BOOL XEdit::InitParamList(long* posList, int nCount)
{
	if (::IsRectEmpty(&m_rcWnd) || nCount != MaxAreaCount * 4)
		return FALSE;
	
	memcpy(m_Pos, posList, MaxAreaCount*4*sizeof(long));
	m_bStretch = TRUE;
	return TRUE;
}
//------------------------------------------------------------------------
void XEdit::OnEraseBkgnd(ICanvas* pCanvas, RECT& rcClip)
{
	if (m_pWndBitmap && m_bStretch)
	{
		// 绘制图片背景
		DWORD style = BLIT_NORMAL;
	if (m_nAlpha < 256)
	{
		if (m_nAlpha == 0) return;
		m_pWndBitmap->SetUserData(m_nAlpha);
		style |= BLIT_TRANS;
	}
	
	if(IsMask())
		style |= BLIT_MASK;
	
	int x, y, i, j;
	RECT rc;
	// 先填充背景
	rc.left = m_rcWnd.left + m_Pos[Fill].x;
	rc.top = m_rcWnd.top + m_Pos[Fill].y;
	rc.right = rc.left + GetWidth() - m_pWndBitmap->GetWidth() + m_Pos[Fill].w;
	rc.bottom = rc.top + GetHeight() - m_pWndBitmap->GetHeight() + m_Pos[Fill].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int rows = (rc.bottom - rc.top) / m_Pos[SrcBack].h;
		int rows_remind = (rc.bottom - rc.top) % m_Pos[SrcBack].h;
		int cols = (rc.right - rc.left) / m_Pos[SrcBack].w;
		int cols_remind = (rc.right - rc.left) % m_Pos[SrcBack].w;
		y = rc.top;
		for (i=0; i<rows; i++) // 一行一行的填充
		{
			x = rc.left;
			for (j=0; j<cols; j++)
			{
				m_pWndBitmap->Draw(pCanvas, 
					x, y, m_Pos[SrcBack].w, m_Pos[SrcBack].h, 
					m_Pos[SrcBack].x, m_Pos[SrcBack].y, style);
				x+=m_Pos[SrcBack].w;
			}
			if (cols_remind > 0) // 最后一列
			{
				m_pWndBitmap->Draw(pCanvas, 
					x, y, cols_remind, m_Pos[SrcBack].h, 
					m_Pos[SrcBack].x, m_Pos[SrcBack].y, style);
			}
			y+=m_Pos[SrcBack].h;
		}
		// 最后一行
		if (rows_remind > 0)
		{
			x = rc.left;
			for (j=0; j<cols; j++)
			{
				m_pWndBitmap->Draw(pCanvas, 
					x, y, m_Pos[SrcBack].w, rows_remind, 
					m_Pos[SrcBack].x, m_Pos[SrcBack].y, style);
				x+=m_Pos[SrcBack].w;
			}
			if (cols_remind > 0) // 最后一列
			{
				m_pWndBitmap->Draw(pCanvas, 
					x, y, cols_remind, rows_remind, 
					m_Pos[SrcBack].x, m_Pos[SrcBack].y, style);
			}
		}
	}

	// 再绘制四个角落的部分
	// 左上
	rc.left = m_rcWnd.left + m_Pos[LeftTop].x;
	rc.top = m_rcWnd.top + m_Pos[LeftTop].y;
	rc.right = rc.left + m_Pos[LeftTop].w;
	rc.bottom = rc.top + m_Pos[LeftTop].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		m_pWndBitmap->Draw(pCanvas,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			rcClip.left - m_rcWnd.left, rcClip.top - m_rcWnd.top, style);
	}

	// 右上
	rc.left = m_rcWnd.right - m_Pos[RightTop].w;
	rc.top = m_rcWnd.top + m_Pos[RightTop].y;
	rc.right = m_rcWnd.right;
	rc.bottom = rc.top + m_Pos[RightTop].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{

		long deltax = rc.left - m_rcWnd.right + m_Pos[RightTop].w;
		if (deltax <= 0) deltax = 0;
		m_pWndBitmap->Draw(pCanvas,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			m_Pos[RightTop].x + deltax, rcClip.top - m_rcWnd.top, style);
	}

	// 左下
	rc.left = m_rcWnd.left + m_Pos[LeftBottom].x;
	rc.top = m_rcWnd.bottom - m_Pos[LeftBottom].h;
	rc.right = rc.left + m_Pos[LeftBottom].w;
	rc.bottom = m_rcWnd.bottom;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		long deltay = rc.top - m_rcWnd.bottom + m_Pos[LeftBottom].h;
		if (deltay <= 0) deltay = 0;
		m_pWndBitmap->Draw(pCanvas,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			rcClip.left - m_rcWnd.left, m_Pos[LeftBottom].y + deltay, style);
	}

	// 右下
	rc.right = m_rcWnd.right;
	rc.bottom = m_rcWnd.bottom;
	rc.left = m_rcWnd.right - m_Pos[RightBottom].w;
	rc.top = m_rcWnd.bottom - m_Pos[RightBottom].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		long deltax = rc.left - m_rcWnd.right + m_Pos[RightBottom].w;
		if (deltax <= 0) deltax = 0;
		long deltay = rc.top - m_rcWnd.bottom + m_Pos[RightBottom].h;
		if (deltay <= 0) deltay = 0;
		m_pWndBitmap->Draw(pCanvas,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			m_Pos[RightBottom].x + deltax, m_Pos[RightBottom].y + deltay, style);
	}

	// 绘制可拉伸部分
	// 上部
	rc.left = m_rcWnd.left + m_Pos[Top].x;
	rc.top = m_rcWnd.top + m_Pos[Top].y;
	rc.right = m_rcWnd.right - m_Pos[RightTop].w;
	rc.bottom = rc.top + m_Pos[Top].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int cols = (rc.right - rc.left) / m_Pos[Top].w;
		int cols_remind = (rc.right - rc.left) % m_Pos[Top].w;
		long deltay = rc.top - m_rcWnd.top - m_Pos[Top].y;
		if (deltay <= 0) deltay = 0;

		x = rc.left;
		y = rc.top;
		for (j=0; j<cols; j++)
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Top].w, m_Pos[Top].h, 
				m_Pos[Top].x, m_Pos[Top].y + deltay, style);
			x+=m_Pos[Top].w;
		}
		if (cols_remind > 0) // 最后一列
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, cols_remind, m_Pos[Top].h, 
				m_Pos[Top].x, m_Pos[Top].y + deltay, style);
		} 
	}	
	
	// 左部
	rc.left = m_rcWnd.left + m_Pos[Left].x;
	rc.top = m_rcWnd.top + m_Pos[Left].y;
	rc.right = rc.left + m_Pos[Left].w;
	rc.bottom = m_rcWnd.bottom - m_Pos[LeftBottom].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int rows = (rc.bottom - rc.top) / m_Pos[Left].h;
		int rows_remind = (rc.bottom - rc.top) % m_Pos[Left].h;
		long deltax = rc.left - m_rcWnd.left - m_Pos[Left].x;
		if (deltax <= 0) deltax = 0;
		
		x = rc.left;
		y = rc.top;
		for (i=0; i<rows; i++)
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Left].w, m_Pos[Left].h, 
				m_Pos[Left].x + deltax, m_Pos[Left].y, style);
			y+=m_Pos[Left].h;
		}
		if (rows_remind > 0) // 最后一行
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Left].w, rows_remind, 
				m_Pos[Left].x + deltax, m_Pos[Left].y, style);
		} 
	}

	// 下部
	rc.left = m_rcWnd.left + m_Pos[Bottom].x;
	rc.top = m_rcWnd.bottom - m_pWndBitmap->GetHeight() + m_Pos[Bottom].y;
	rc.right = m_rcWnd.right - m_Pos[RightBottom].w;
	rc.bottom = rc.top + m_Pos[Bottom].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int cols = (rc.right - rc.left) / m_Pos[Bottom].w;
		int cols_remind = (rc.right - rc.left) % m_Pos[Bottom].w;
		long deltay = rc.top - m_rcWnd.bottom + m_pWndBitmap->GetHeight() - m_Pos[Bottom].y;
		if (deltay <= 0) deltay = 0;
		
		x = rc.left;
		y = rc.top;
		for (j=0; j<cols; j++)
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Bottom].w, m_Pos[Bottom].h, 
				m_Pos[Bottom].x, m_Pos[Bottom].y + deltay, style);
			x+=m_Pos[Bottom].w;
		}
		if (cols_remind > 0) // 最后一列
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, cols_remind, m_Pos[Bottom].h, 
				m_Pos[Bottom].x, m_Pos[Bottom].y + deltay, style);
		} 
	}

	
	// 右部
	rc.left = m_rcWnd.right - m_pWndBitmap->GetWidth() + m_Pos[Right].x;
	rc.top = m_rcWnd.top + m_Pos[Right].y;
	rc.right = m_rcWnd.right;
	rc.bottom = m_rcWnd.bottom - m_Pos[RightBottom].h;
	IntersectRect(&rc, &rcClip, &rc);
	if (!IsRectEmpty(&rc))
	{
		int rows = (rc.bottom - rc.top) / m_Pos[Right].h;
		int rows_remind = (rc.bottom - rc.top) % m_Pos[Right].h;
		long deltax = rc.left - m_rcWnd.right + m_pWndBitmap->GetWidth() - m_Pos[Right].x;
		if (deltax <= 0) deltax = 0;
		
		x = rc.left;
		y = rc.top;
		for (i=0; i<rows; i++)
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Right].w, m_Pos[Right].h, 
				m_Pos[Right].x + deltax, m_Pos[Right].y, style);
			y+=m_Pos[Right].h;
		}
		if (rows_remind > 0) // 最后一行
		{
			m_pWndBitmap->Draw(pCanvas, 
				x, y, m_Pos[Right].w, rows_remind, 
				m_Pos[Right].x + deltax, m_Pos[Right].y, style);
		} 
	}
		return;
	}
	
	XWindow::OnEraseBkgnd(pCanvas, rcClip);
}
//------------------------------------------------------------------------
void XEdit::DrawString(ICanvas* pCanvas, RECT& rcClip, string& strText, WPixel& clr, POINT& ptCurPos)
{
	ITTFont* pFont = GetFont();
	int nW = pFont->GetLength(strText.c_str());
	if (IsAutoWrap() && (ptCurPos.x + nW > m_rcWnd.right - m_nMargin)) // 文字在当前行写不下
	{
		string str = strText;
		do {
			POINT pt = {m_rcWnd.right - m_nMargin - ptCurPos.x, 0};
			int nPos = GetCharIndexByOffset(str.c_str(), pt);
			string s = str.substr(0, nPos);
			pCanvas->DrawClipText(pFont, ptCurPos.x, ptCurPos.y, s.c_str(), clr, rcClip, FONT_NOALPHA);
			ptCurPos.x = m_rcWnd.left + m_nMargin + m_ptOffset.x;
			ptCurPos.y += pFont->GetHeight() + m_nLineSpace;
			str = str.substr(nPos, str.length()-nPos);
			nW = pFont->GetLength(str.c_str());

		} while(ptCurPos.x + nW > m_rcWnd.right - m_nMargin);
		if (nW > 0)
		{
			pCanvas->DrawClipText(pFont, ptCurPos.x, ptCurPos.y, str.c_str(), clr, rcClip, FONT_NOALPHA);
			ptCurPos.x += nW;
		}
	}
	else
	{
		pCanvas->DrawClipText(pFont, ptCurPos.x, ptCurPos.y, strText.c_str(), clr, rcClip, FONT_NOALPHA);				
		ptCurPos.x += nW;
	}
}
//------------------------------------------------------------------------
void XEdit::DrawSelectedString(ICanvas* pCanvas, RECT& rcClip, string& strText, WPixel& clr, POINT& ptCurPos)
{
	ITTFont* pFont = GetFont();
	int nW = pFont->GetLength(strText.c_str());
	RECT rcSel;
	if (IsAutoWrap() && (ptCurPos.x + nW > m_rcWnd.right - m_nMargin)) // 文字在当前行写不下
	{
		string str = strText;
		do {
			POINT pt = {m_rcWnd.right - m_nMargin - ptCurPos.x, 0};
			int nPos = GetCharIndexByOffset(str.c_str(), pt);
			string s = str.substr(0, nPos);
			rcSel.left = ptCurPos.x+1;	// 偏移1个像素，是为了能够看到Caret的跳动
			rcSel.top = ptCurPos.y;
			rcSel.right = ptCurPos.x + pFont->GetLength(s.c_str());
			rcSel.bottom = ptCurPos.y + pFont->GetHeight() + m_nLineSpace;
			IntersectRect(&rcSel, &rcClip, &rcSel);
			pCanvas->DrawBox(rcSel, GetForeColor());
			pCanvas->DrawClipText(pFont, ptCurPos.x, ptCurPos.y, s.c_str(), clr, rcClip, FONT_NOALPHA);
			ptCurPos.x = m_rcWnd.left + m_nMargin + m_ptOffset.x;
			ptCurPos.y += pFont->GetHeight() + m_nLineSpace;
			str = str.substr(nPos, str.length()-nPos);
			nW = pFont->GetLength(str.c_str());
			
		} while(ptCurPos.x + nW > m_rcWnd.right - m_nMargin);
		if (nW > 0)
		{
			rcSel.left = ptCurPos.x+1;	// 偏移1个像素，是为了能够看到Caret的跳动
			rcSel.top = ptCurPos.y;
			rcSel.right = ptCurPos.x + pFont->GetLength(str.c_str());
			rcSel.bottom = ptCurPos.y + pFont->GetHeight() + m_nLineSpace;
			IntersectRect(&rcSel, &rcClip, &rcSel);
			pCanvas->DrawBox(rcSel, GetForeColor());
			pCanvas->DrawClipText(pFont, ptCurPos.x, ptCurPos.y, str.c_str(), clr, rcClip, FONT_NOALPHA);
			ptCurPos.x += nW;
		}
	}
	else
	{
		rcSel.left = ptCurPos.x+1;	// 偏移1个像素，是为了能够看到Caret的跳动
		rcSel.top = ptCurPos.y;
		rcSel.right = ptCurPos.x + nW;
		rcSel.bottom = ptCurPos.y + pFont->GetHeight() + m_nLineSpace;
		IntersectRect(&rcSel, &rcClip, &rcSel);
		pCanvas->DrawBox(rcSel, GetForeColor());
		pCanvas->DrawClipText(pFont, ptCurPos.x, ptCurPos.y, strText.c_str(), clr, rcClip, FONT_NOALPHA);				
		ptCurPos.x += nW;
	}
}
//------------------------------------------------------------------------
void XEdit::DrawUnSelectText(ICanvas* pCanvas, RECT& rcClip, POINT& ptCurPos, string& strText)
{
	ITTFont* pFont = GetFont();
	if (IsMultiLine())
	{
		string& str = strText;
		int i=0;
		int nStartPos = 0;
		int nLen = str.length();
		while (i<nLen)
		{
			if ((BYTE)str.at(i) == '\r' && i+1<nLen && (BYTE)str.at(i+1) == '\n')
			{
				string s = str.substr(nStartPos, i-nStartPos);
				DrawString(pCanvas, rcClip, s, GetForeColor(), ptCurPos);
				i++;
				nStartPos = i+1;
				ptCurPos.x = m_rcWnd.left + m_nMargin + m_ptOffset.x;
				ptCurPos.y += pFont->GetHeight() + m_nLineSpace;
			}
			i++;
		}
		if (nStartPos == 0)
		{
			DrawString(pCanvas, rcClip, str, GetForeColor(), ptCurPos);
		}
		else if (nStartPos != i)
		{
			string s = str.substr(nStartPos, i-nStartPos);
			DrawString(pCanvas, rcClip, s, GetForeColor(), ptCurPos);
		}
	}
	else // 单行
	{
		string str = GetDisplayString(strText);
		DrawString(pCanvas, rcClip, str, GetForeColor(), ptCurPos);
	}
}
//------------------------------------------------------------------------
void XEdit::DrawSelectText(ICanvas* pCanvas, RECT& rcClip, POINT& ptCurPos, string& strText)
{
	ITTFont* pFont = GetFont();
	if (IsMultiLine())
	{
		string& str = strText;		
		int i=0;
		int nStartPos = 0;
		int nLen = str.length();
		while (i<nLen)
		{
			if ((BYTE)str.at(i) == '\r' && i+1<nLen && (BYTE)str.at(i+1) == '\n')
			{
				string s = str.substr(nStartPos, i-nStartPos);
				DrawSelectedString(pCanvas, rcClip, s, GetBackColor(), ptCurPos);
				i++;
				nStartPos = i+1;
				ptCurPos.x = m_rcWnd.left + m_nMargin + m_ptOffset.x;
				ptCurPos.y += pFont->GetHeight() + m_nLineSpace;
			}
			i++;
		}
		if (nStartPos == 0)
		{
			DrawSelectedString(pCanvas, rcClip, str, GetBackColor(), ptCurPos);
		}
		else if (nStartPos != i)
		{
			string s = str.substr(nStartPos, i-nStartPos);
			DrawSelectedString(pCanvas, rcClip, s, GetBackColor(), ptCurPos);
		}
	}
	else // 单行
	{
		string str = GetDisplayString(strText);		
		DrawSelectedString(pCanvas, rcClip, str, GetBackColor(), ptCurPos);
	}
} 
//------------------------------------------------------------------------
void XEdit::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	ITTFont* pFont = GetFont();
	if (rcClip.left < m_rcWnd.left + m_nMargin)
		rcClip.left = m_rcWnd.left + m_nMargin;
	if (rcClip.right > m_rcWnd.right - m_nMargin)
		rcClip.right = m_rcWnd.right - m_nMargin;
	if (rcClip.top < m_rcWnd.top + m_nMargin)
		rcClip.top = m_rcWnd.top + m_nMargin;
	if (rcClip.bottom > m_rcWnd.bottom - m_nMargin)
		rcClip.bottom = m_rcWnd.bottom - m_nMargin;
	
	int x = m_rcWnd.left + m_nMargin + 2;
	int y = m_rcWnd.top + m_nMargin;
	
	x += m_ptOffset.x;
	y += m_ptOffset.y;
	POINT ptOffset = {x, y};
	if (IsValidSelection() && IsGlobalFocus())
	{
		string strLeft = m_strCaption.substr(0, m_sSel.startIndex);
		DrawUnSelectText(pCanvas, rcClip, ptOffset, strLeft);
		
		string strSel = m_strCaption.substr(m_sSel.startIndex, m_sSel.endIndex-m_sSel.startIndex);
		DrawSelectText(pCanvas, rcClip, ptOffset, strSel);

		string strRight = m_strCaption.substr(m_sSel.endIndex, m_strCaption.length()-m_sSel.endIndex);
		DrawUnSelectText(pCanvas, rcClip, ptOffset, strRight);
	}
	else
		DrawUnSelectText(pCanvas, rcClip, ptOffset, m_strCaption);
	
	// 画Beam
	if (IsGlobalFocus() && m_bDrawBeam)
	{
		RECT rcValid;
		rcValid.left = m_rcWnd.left + m_nMargin;
		rcValid.top = m_rcWnd.top + m_nMargin;
		rcValid.right = m_rcWnd.right - m_nMargin;
		rcValid.bottom = m_rcWnd.bottom - m_nMargin;
		IntersectRect(&rcValid, &rcValid, &rcClip);
		if (!IsRectEmpty(&rcValid))
		{
			RECT rcCaret;
			rcCaret.left = m_ptCaretPos.x + m_ptOffset.x + m_rcWnd.left + m_nMargin;
			rcCaret.top = m_ptCaretPos.y + m_ptOffset.y + m_rcWnd.top + m_nMargin;
			rcCaret.right = rcCaret.left + 1;
			rcCaret.bottom = rcCaret.top + pFont->GetHeight() + m_nLineSpace;
			IntersectRect(&rcValid, &rcValid, &rcCaret);
			if (!IsRectEmpty(&rcValid))
			{
				POINT ptFrom, ptTo;
				ptFrom.x = ptTo.x = rcValid.left;
				ptFrom.y = rcValid.top;
				ptTo.y = rcValid.bottom;
				pCanvas->DrawLine(ptFrom, ptTo, GetForeColor());
			}
		}
	}
	
	XWindow::OnDraw(pCanvas, rcClip);
}
//------------------------------------------------------------------------
DWORD XEdit::OnChar(UINT nKeyAscii)
{ 
	if (!IsVisible() || !IsEnabled())
		return XWindow::OnChar(nKeyAscii);
	if (nKeyAscii < 32)
	{
		switch(nKeyAscii)
		{
		case '\b':	// backspace
			doBackSpace();
			break;

		case '\t':	//tab
		case '\n':	//linefeed
			break;
		case '\r':	//carriage return
			if (IsMultiLine())
				doCrLf();
			else
				s_returnPressed();
			break;

		case 1:		// ctrl + a
			doSelAll();
			break;
		case 3:		// ctrl + c
			doCopy();
			break;
		case 22:	// ctrl + v
			doPaste();
			break;
		case 24:	// ctrl + x
			doCut();
			break;
		default:
			return XWindow::OnChar(nKeyAscii);
		}
	}
	else
	{
		OnOtherChar(nKeyAscii);
	}
	return 0;
}
//------------------------------------------------------------------------
void XEdit::OnLeftArrow(UINT nShift)
{
	int offset = - GetCharBytes(GetCaretPos() - 1);
	if (nShift & CtrlKey)
		offset = GetLastSpaceBefore(GetCaretPos()) - GetCaretPos();
	if (nShift & ShiftKey)
	{
		if (m_nSelStart < 0) m_nSelStart = GetCaretPos();
		SetSelection(m_nCaretPos + offset, m_nSelStart);
	}
	else
	{
		ClearSelection();
	}

	if (m_nCaretPos > 0)
	{
		SetCaretPos(m_nCaretPos + offset);
		CalcCaretPos();
		ReDrawCaret();
		AdjustHScroll();
		AdjustVScroll();
		InvalidRect();
	}

}
//------------------------------------------------------------------------
void XEdit::OnRightArrow(UINT nShift)
{
	int offset = GetCharBytes(m_nCaretPos);
	if (nShift & CtrlKey)
		offset = GetNextWordStartPosAfter(GetCaretPos()) - GetCaretPos();
	if (nShift & ShiftKey)
	{
		if (m_nSelStart < 0) m_nSelStart = m_nCaretPos;
		SetSelection(m_nSelStart, m_nCaretPos + offset);
	}
	else
	{
		ClearSelection();
	}

	if (m_nCaretPos < m_strCaption.length())
	{
		SetCaretPos(m_nCaretPos + offset);
		CalcCaretPos();
		ReDrawCaret();
		AdjustHScroll();
		AdjustVScroll();
		InvalidRect();
	}
}
//------------------------------------------------------------------------
void XEdit::OnUpArrow(UINT nShift)
{
	int nLineHeight = GetFont()->GetHeight() + m_nLineSpace;
	if (m_ptCaretPos.y >= nLineHeight) // 能够上移
	{
		POINT pt;
		pt.x = m_ptCaretPos.x;
		pt.y = m_ptCaretPos.y - nLineHeight;
		int nIndex = GetCharIndexByOffset(m_strCaption.c_str(), pt, TRUE, TRUE);
		if (nShift & ShiftKey)
		{
			if (m_nSelStart < 0) m_nSelStart = m_nCaretPos;
			SetSelection(m_nSelStart, nIndex);
		}
		else
			ClearSelection();
		SetCaretPos(nIndex);
		ReDrawCaret();
		AdjustHScroll();
		AdjustVScroll();
		InvalidRect();
	}
}
//------------------------------------------------------------------------
void XEdit::OnDownArrow(UINT nShift)
{
	int nLineHeight = GetFont()->GetHeight() + m_nLineSpace;
	if (m_ptCaretPos.y < GetValidHeight() - nLineHeight) // 能够下移
	{
		string str = GetDisplayString(m_strCaption);
		POINT pt;
		pt.x = m_ptCaretPos.x;
		pt.y = m_ptCaretPos.y + nLineHeight;
		int nIndex = GetCharIndexByOffset(str.c_str(), pt, TRUE, TRUE);
		if (nShift & ShiftKey)
		{
			if (m_nSelStart < 0) m_nSelStart = m_nCaretPos;
			SetSelection(m_nSelStart, nIndex);
		}
		else
			ClearSelection();
		SetCaretPos(nIndex);
		ReDrawCaret();
		AdjustHScroll();
		AdjustVScroll();
		InvalidRect();
	}
}
//------------------------------------------------------------------------
void XEdit::doDelete()
{
	if (IsReadOnly())
		return;

    if (IsValidSelection())
    {
        DeleteSelectedText();
    }
    else if (GetCaretPos() < m_strCaption.length())
    {
		int nBytes = GetCharBytes(GetCaretPos());
        string s = stringFromCaret();
        s = s.substr(nBytes, s.length()-nBytes);
		string str = stringUpToCaret() + s;
		m_strCaption = str;
    }

	s_textChanged((DWORD)m_strCaption.c_str());

	CalcValidSize();
	AdjustHScroll();
	AdjustVScroll();
	InvalidRect();
}
//------------------------------------------------------------------------
void XEdit::doCrLf()
{
	if (IsReadOnly())
		return;
	
	if (IsDigitOnly())
		return;
	
	if (m_strCaption.length() >= m_nMaxLen-1) // 即使只剩一个字节，也不让回车换行，防止内存越界
		return;
	
	if (IsValidSelection())
		DeleteSelectedText();
	
	string str = stringUpToCaret() + "\r\n" + stringFromCaret();
	m_strCaption = str;		
	SetCaretPos(GetCaretPos() + 2);
	CalcCaretPos();
	s_textChanged((DWORD)str.c_str());
	
	CalcValidSize();
	AdjustHScroll();
	AdjustVScroll();
	InvalidRect();
}
//------------------------------------------------------------------------
void XEdit::OnOtherChar(UINT nKeyAscii)
{
	if (IsReadOnly())
		return;

	if (IsDigitOnly() && !isdigit(nKeyAscii))
		return;

	if (m_strCaption.length() >= m_nMaxLen)
		return;

	if (IsValidSelection())
		DeleteSelectedText();
	
	static bool bIsDBCS = false;
	static char c[3] = {0,0,0};

	if (bIsDBCS == false) // 开始一个字符
	{
		if (nKeyAscii > 0x80) // 开始一个双字节字符的Lead字节
		{
			bIsDBCS = true;
			c[0] = nKeyAscii;
			return;
		}
		else // ascii字符
		{
			c[0] = nKeyAscii;
			string str = stringUpToCaret() + c + stringFromCaret();
			m_strCaption = str;
			SetCaretPos(GetCaretPos() + 1);
			CalcCaretPos();
			s_textChanged((DWORD)str.c_str());
		}
	}
	else // Trail字节
	{
		c[1] = nKeyAscii;
		string str = stringUpToCaret() + c + stringFromCaret();
		m_strCaption = str;
		SetCaretPos(GetCaretPos() + 2);
		CalcCaretPos();
		s_textChanged((DWORD)str.c_str());
		c[1] = 0;
		bIsDBCS = false;
	}

	CalcValidSize();
	AdjustHScroll();
	AdjustVScroll();
	InvalidRect();
}
//------------------------------------------------------------------------
DWORD XEdit::OnKeyDown(UINT nKeyCode, UINT nShift)
{
	switch (nKeyCode)
	{
	case VK_LEFT:
		OnLeftArrow(nShift);
		break;

	case VK_RIGHT:
		OnRightArrow(nShift);
		break;

	case VK_UP:
		OnUpArrow(nShift);
		break;

	case VK_DOWN:
		OnDownArrow(nShift);
		break;

	case VK_DELETE:
		doDelete();
		break;

	case VK_HOME:
		{
			POINT pt = m_ptCaretPos;
			pt.x = 0;
			if (nShift & CtrlKey)
				pt.y = 0;
			int nIndex = GetCharIndexByOffset(m_strCaption.c_str(), pt, TRUE, TRUE);
			if ((nShift & ShiftKey))
			{
				if (m_nSelStart < 0) m_nSelStart = m_nCaretPos;
				SetSelection(nIndex, m_nSelStart);
			}
			else
			{
				ClearSelection();
			}
			SetCaretPos(nIndex);

			ReDrawCaret();
			AdjustHScroll();
			AdjustVScroll();
			InvalidRect();
		}
		break;
		
	case VK_END:
		{
			POINT pt = m_ptCaretPos;
			pt.x = GetValidWidth();
			if (nShift & CtrlKey)
				pt.y = GetValidHeight();
			int nIndex = GetCharIndexByOffset(m_strCaption.c_str(), pt, TRUE, TRUE);
			if ((nShift & ShiftKey))
			{
				if (m_nSelStart < 0) m_nSelStart = m_nCaretPos;
				SetSelection(nIndex, m_nSelStart);
			}
			else
			{
				ClearSelection();
			}
			SetCaretPos(nIndex);

			ReDrawCaret();
			AdjustHScroll();
			AdjustVScroll();
			InvalidRect();
		}
		break;
	default:
		{
			unsigned char type = KEY_CODE[nKeyCode];
			if (//nShift == 0 && // 按了shift、alt、ctrl等则不处理
				(type&alpha_key || // 65~90,32 英文子母键和空格
				type&num_key || // 48~57,96~105 大键盘数字键,小键盘数字键
				type&sign_key|| // 106~111,186~192,219~222 小键盘的-./ 大键盘的;=,-./`[\]'
				nKeyCode == 8 ||	// backspace
				nKeyCode == 9 ||	// tab
				nKeyCode == 13)		// enter
				)
			{				 
				if (IsValidSelection() && nShift == 0 && !IsReadOnly())
				{
					//DeleteSelectedText();
				}
				
				return 0;
			}
		}
		return XWindow::OnKeyDown(nKeyCode, nShift);
	}
	return 0;
}
//------------------------------------------------------------------------
DWORD XEdit::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	// 得到Beam的位置
	if (nButton == LeftButton)
	{
		if (PtInRect(&m_rcWnd, pt))// 鼠标按在Edit的范围内(包括边框空白的地方)
		{
			POINT ptOffset;
			ptOffset.x = pt.x - m_rcWnd.left;
			ptOffset.y = pt.y - m_rcWnd.top;
			if (ptOffset.x < m_nMargin)
				ptOffset.x = m_nMargin;
			if (ptOffset.x > GetWidth() - m_nMargin)
				ptOffset.x = GetWidth() - m_nMargin;
			if (ptOffset.y < m_nMargin)
				ptOffset.y = m_nMargin;
			if (ptOffset.y > GetHeight() - m_nMargin)
				ptOffset.y = GetHeight() - m_nMargin;
			POINT ptOff = {ptOffset.x - m_nMargin - m_ptOffset.x, ptOffset.y - m_nMargin  - m_ptOffset.y};
			m_ptCaretPos.x = m_ptCaretPos.y = 0;
			int nHitIndex = GetCharIndexByOffset(m_strCaption.c_str(), ptOff, TRUE, TRUE);
			SetCaretPos(nHitIndex);
			ClearSelection();
			m_nSelStart = nHitIndex;
		}

		ReDrawCaret();
		InvalidRect();
	}

	return XWindow::OnMouseDown(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XEdit::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{
#define __OFFSET	5
	if (IsCapture())
	{
		POINT ptOffset;
		ptOffset.x = pt.x - m_rcWnd.left - m_nMargin;
		ptOffset.y = pt.y - m_rcWnd.top - m_nMargin;
		if (ptOffset.x > 0 && ptOffset.x < GetWidth() - 2*m_nMargin &&
			ptOffset.y > 0 && ptOffset.y < GetHeight() - 2*m_nMargin) // 鼠标在Edit的可写文字的范围内按着移动
		{
			POINT ptOff = {ptOffset.x - m_ptOffset.x, ptOffset.y - m_ptOffset.y};
			int nHitIndex = GetCharIndexByOffset(m_strCaption.c_str(), ptOff, TRUE, TRUE);
			SetSelection(m_nSelStart, nHitIndex);
			SetCaretPos(nHitIndex);
		}
		else
		{
			if (!IsMultiLine())
			{
				if (ptOffset.x <= -__OFFSET)
				{
					SetSelection(0, m_nSelStart);
					SetCaretPos(0);
					CalcCaretPos();
					AdjustHScroll();
				}
				else if (ptOffset.x > -__OFFSET && ptOffset.x <= 0)
				{
					if (m_nCaretPos > 0)
					{
						int nBytes = GetCharBytes(m_nCaretPos-1);
						SetSelection(m_nCaretPos-nBytes, m_nSelStart);
						SetCaretPos(m_nCaretPos-nBytes);
						CalcCaretPos();
						AdjustHScroll();
					}
				}
				else if (ptOffset.x >= GetWidth() - m_nMargin*2 + __OFFSET)
				{
					int nLen = m_strCaption.length();
					SetSelection(m_nSelStart, nLen);
					SetCaretPos(nLen);
					CalcCaretPos();
					AdjustHScroll();
				}
				else if (ptOffset.x >= GetWidth() - m_nMargin*2 && ptOffset.x < GetWidth() - m_nMargin*2 + __OFFSET)
				{
					if (m_nCaretPos < m_strCaption.length())
					{
						int nBytes = GetCharBytes(m_nCaretPos);
						SetSelection(m_nSelStart, m_nCaretPos+nBytes);
						SetCaretPos(m_nCaretPos+nBytes);
						CalcCaretPos();
						AdjustHScroll();
					}
				}
				else
				{
					POINT ptOff = {ptOffset.x - m_ptOffset.x, 0};
					int nHitIndex = GetCharIndexByOffset(m_strCaption.c_str(), ptOff, TRUE, TRUE);
					SetSelection(m_nSelStart, nHitIndex);
					SetCaretPos(nHitIndex);
					AdjustHScroll();
				}
			}
			else if ((IsMultiLine() && !IsAutoWrap()))
			{
				POINT pt = ptOffset;
				pt.y -= m_ptOffset.y;
				if (ptOffset.x <= -__OFFSET){
					pt.x = 0;
				}else if (ptOffset.x > -__OFFSET && ptOffset.x <= 0){
					pt.x = m_ptCaretPos.x - GetWidth() / 4;
					if (pt.x < 0) pt.x = 0;
				}else if (ptOffset.x >= GetWidth() - m_nMargin*2 + __OFFSET){
					pt.x = GetValidWidth();
				}else if (ptOffset.x >= GetWidth() - m_nMargin*2 && ptOffset.x < GetWidth() - m_nMargin*2 + __OFFSET){
					pt.x = m_ptCaretPos.x + GetWidth() / 4;
					if (pt.x > GetValidWidth()) pt.x = GetValidWidth();
				}
				int nIndex = GetCharIndexByOffset(m_strCaption.c_str(), pt, TRUE, TRUE);
				SetSelection(m_nSelStart, nIndex);
				SetCaretPos(nIndex);
				AdjustHScroll();
			}
			
			if (IsMultiLine()) // 垂直滚动
			{
				POINT pt = ptOffset;
				pt.x -= m_ptOffset.x;
				if (ptOffset.y <= -__OFFSET)
					pt.y = 0;
				else if (ptOffset.y > -__OFFSET && ptOffset.y <= 0)
					pt.y = m_ptCaretPos.y - GetFont()->GetHeight() + m_nLineSpace;
				else if (ptOffset.y >= GetHeight() - m_nMargin*2 + __OFFSET)
					pt.y = GetValidHeight();
				else if (ptOffset.y >= GetHeight() - m_nMargin*2 && ptOffset.y < GetHeight() - m_nMargin*2 + __OFFSET)
					pt.y = m_ptCaretPos.y + GetFont()->GetHeight() + m_nLineSpace;
				int nIndex = GetCharIndexByOffset(m_strCaption.c_str(), pt, TRUE, TRUE);
				SetSelection(m_nSelStart, nIndex);
				SetCaretPos(nIndex);
				AdjustVScroll();
			}
		}

		InvalidRect();
	}
	
	return XWindow::OnMouseMove(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XEdit::OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt)
{
	if (IsMultiLine())
	{
		if (zDelta < 0)
		{
			zDelta = -zDelta;
			zDelta *= 16;
			m_ptOffset.y -= zDelta;
			if (m_ptOffset.y < GetHeight() - 2*GetMargin() - GetValidHeight())
				m_ptOffset.y = GetHeight() - 2*GetMargin() - GetValidHeight();
		}
		else
		{
			zDelta *= 16;
			m_ptOffset.y += zDelta;
			if (m_ptOffset.y > 0)
				m_ptOffset.y = 0;
		}
		if (m_pVScrollBar && m_pVScrollBar->IsEnabled())
			m_pVScrollBar->SetScrollPos(-m_ptOffset.y);
		return 0;
	}
	
	return XWindow::OnMouseWheel(nButton, nShift, zDelta, pt);
}
//------------------------------------------------------------------------
void XEdit::OnMouseEnter()
{ 
	if (IsEnabled())
	SetCursor(CT_BEAN);
}
//------------------------------------------------------------------------
void XEdit::OnMouseExit()
{
	SetCursor(CT_ARROW);
}
//------------------------------------------------------------------------
void XEdit::OnSetFocus()
{
	// 启动时钟
	GetTimeAxis()->SetTimer(TIMERID_BEAMFLASH, static_cast<ITimerSink*>(this), INTERVAL_BEAMFLASH);
	SetDrawBeam(TRUE);
	InvalidRect();
}
//------------------------------------------------------------------------
void XEdit::OnKillFocus()
{
	// 停止时钟
	GetTimeAxis()->KillTimer(TIMERID_BEAMFLASH, static_cast<ITimerSink*>(this));
	SetDrawBeam(FALSE);
	InvalidRect();
}
//------------------------------------------------------------------------
void XEdit::doSelAll()
{
	SetCaretPos(m_strCaption.length());
	CalcCaretPos();
	m_nSelStart = 0;
    SetSelection(0, m_strCaption.length());

	AdjustHScroll();
	AdjustVScroll();
	InvalidRect();
}
//------------------------------------------------------------------------
void XEdit::doCopy()
{
	if (IsPassword() || !IsValidSelection())
		return;

	int nSelLen = m_sSel.endIndex - m_sSel.startIndex;
	if (nSelLen)
	{
		HANDLE hGlobal = GlobalAlloc(GHND | GMEM_SHARE, nSelLen+1);
		char* pGlobal = (char*)GlobalLock(hGlobal);
		if (pGlobal && OpenClipboard(GetDesktop()->GetHWnd()))
		{
			string str = m_strCaption.substr(m_sSel.startIndex, m_sSel.endIndex);
			lstrcpyn(pGlobal, str.c_str(), nSelLen+1);

			GlobalUnlock(hGlobal);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hGlobal);
			CloseClipboard();
		}
	}
}
//------------------------------------------------------------------------
void XEdit::doPaste()
{
	if (IsReadOnly())
		return;
	
	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		if (OpenClipboard(GetDesktop()->GetHWnd()))
		{
			HANDLE hGlobal = GetClipboardData(CF_TEXT);

			string pasteText = (char*)GlobalLock(hGlobal);
			GlobalUnlock(hGlobal);
			CloseClipboard();

			// 如果粘贴的字符有小于32的字符，如回车换行符等，则删除
			if (!IsMultiLine())
			{
				string::iterator it = pasteText.begin();
				while (it != pasteText.end())
				{
					unsigned char c = *it;
					if (c < 32)
						pasteText.erase(it);
					else ++it;
				}
			}
			else // 如果末尾有半个回车换行？
			{
				if ((BYTE)pasteText.at(pasteText.length()-1) == '\r')
					pasteText.erase(pasteText.end()-1);
			}

			if (IsValidSelection())
				DeleteSelectedText();

			string str = m_strCaption;
			str.insert(GetCaretPos(), pasteText);
			SetWindowText(str.c_str());

			InvalidRect();
		}
	}
}
//------------------------------------------------------------------------
void XEdit::doCut()
{
	if (IsPassword() || IsReadOnly() || !IsValidSelection())
		return;

	doCopy();

	string str = m_strCaption;
	str.erase(m_sSel.startIndex, m_sSel.endIndex - m_sSel.startIndex);
	SetCaretPos(m_sSel.startIndex);
	CalcCaretPos();
	ClearSelection();
	m_strCaption = str;
	s_textChanged((DWORD)str.c_str());

	CalcValidSize();
	AdjustHScroll();
	AdjustVScroll();
	InvalidRect();
}
//------------------------------------------------------------------------
void XEdit::doBackSpace()
{
	if (IsReadOnly())
		return;
	
    if (IsValidSelection())
    {
        DeleteSelectedText();
    }
    else if (GetCaretPos() > 0)
    {
        string s1 = stringUpToCaret();
		int nBytes = GetCharBytes(GetCaretPos() - 1);
        s1 = s1.substr(0, s1.length()-nBytes);
		string str = s1 + stringFromCaret();
		m_strCaption = str;
		SetCaretPos(GetCaretPos() - nBytes);
		CalcCaretPos();
    }

	s_textChanged((DWORD)m_strCaption.c_str());

	ReDrawCaret();
	CalcValidSize();
	AdjustHScroll();
	AdjustVScroll();
	InvalidRect();
}
//------------------------------------------------------------------------
void XEdit::SetCaretPos(int nPos)
{
    m_nCaretPos = nPos;
	if (IsMultiLine())
	{
		return;
	}
	return;
    // 当插入符越界时，校正可见的文字
	POINT pt;
    GetCaretDisc(pt);
	int xDisc = pt.x;
    int leftSideX = m_rcWnd.left + m_nMargin;
    int rightSideX = leftSideX + GetWidth() - m_nMargin*2;
    if (xDisc <= leftSideX) // 插入符跨越了左边界
    {
        m_ptOffset.x += (leftSideX - xDisc);
		if (GetCaretPos() <= 0)
			return;

		// 下面多挤出来一个文字
		int nBytes = GetCharBytes(GetCaretPos()-1);
		WORD c = (BYTE)m_strCaption.at(GetCaretPos()-1);
		if (nBytes == 2)	// 双字节文字
			c = (BYTE)m_strCaption.at(GetCaretPos()-2)*256 + c;
		int charwidth = GetFont()->GetCharLength(c);
		m_ptOffset.x += charwidth;
    }
    else if (xDisc >= rightSideX) // 插入符跨越了右边界
    {
        m_ptOffset.x -= (xDisc - rightSideX);
		if (GetCaretPos() >= m_strCaption.length())
			return;
		
		// 下面多挤出来一个文字
		WORD c = (BYTE)m_strCaption.at(GetCaretPos());
		if (c > 0x80)	// 双字节文字
			c = c*256 + (BYTE)m_strCaption.at(GetCaretPos()+1);
		int charwidth = GetFont()->GetCharLength(c);
		m_ptOffset.x -= charwidth;
    }
}
//------------------------------------------------------------------------
void XEdit::SetSelection(int nStart, int nEnd)
{
	if (nStart <= nEnd)
	{
		m_sSel.startIndex = nStart;
		m_sSel.endIndex = nEnd;
	}
	else
	{
		m_sSel.startIndex = nEnd;
		m_sSel.endIndex = nStart;
	}
}
//------------------------------------------------------------------------
void XEdit::ClearSelection()
{
	m_sSel.startIndex = m_sSel.endIndex = 0;
	m_nSelStart = -1;
}
//------------------------------------------------------------------------
/*
string XEdit::GetSelectedText()
{
	string str;
	if (IsValidSelection())
		str = m_strCaption.substr(m_sSel.startIndex, m_sSel.endIndex);
	return str;
}*/
//------------------------------------------------------------------------
void XEdit::DeleteSelectedText()
{
	string str1 = m_strCaption.substr(0, m_sSel.startIndex);
	string str2 = m_strCaption.substr(m_sSel.endIndex, m_strCaption.length() - m_sSel.endIndex);
    string str = str1 + str2;
    SetCaretPos(m_sSel.startIndex);
	CalcCaretPos();
    ClearSelection();
	m_strCaption = str;
}
//------------------------------------------------------------------------
BOOL XEdit::IsValidSelection()
{
    int s = m_sSel.startIndex;
    int e = m_sSel.endIndex;
	int l = m_strCaption.size();
    return ((s >= 0) && (s < e) && (e <= l));
}
//------------------------------------------------------------------------
void XEdit::_CalcOffsetByCharIndex(LPCSTR lpStr, int nIndex, POINT& pt)
{
	if (!lpStr || lpStr[0] == '\0') return;
	ITTFont* pFont = GetFont();
	int nLen = strlen(lpStr);
	if (nIndex > nLen) nIndex = nLen;
	if (!IsMultiLine()) // 单行
	{
		string str = lpStr;
		str = str.substr(0, nIndex);
		pt.x += pFont->GetLength(str.c_str());
		return;
	}
	
	// 多行的情况
	char* pBegin = (char*)lpStr;
	char* pEnd = pBegin + nIndex;
	int nCurDisc = 0;
	int nLineHeight = pFont->GetHeight() + m_nLineSpace;
	BOOL bIsDblByteChar = FALSE;
	int nValidWidth = GetValidWidth();
	WORD c;
	int nCharWidth = 0;
	if (IsAutoWrap())
	{
		BOOL bIndexNext = nIndex != nLen && *(pBegin + nIndex) != '\r' && *(pBegin + nIndex + 1) != '\n'; // 不是最后或者后面不是回车换行符，将nIndex向后移动一个文字
		if (bIndexNext)
		{
			c = *(pBegin + nIndex);
			if (c > 0x80)
				pEnd = pBegin + nIndex + 2;
			else
				pEnd = pBegin + nIndex + 1;
		}
		while (pBegin < pEnd)
		{
			bIsDblByteChar = FALSE;
			c = *pBegin++;
			if (c > 0x80)	// 双字节文字
			{
				c = c*256 + *pBegin++;
				bIsDblByteChar = TRUE;
			}
			
			if (c == '\r' && *pBegin == '\n')
			{
				pBegin ++;
				nCurDisc = 0;
				pt.x = 0;
				pt.y += nLineHeight;
			}
			else
			{
				nCharWidth = pFont->GetCharLength(c);
				nCurDisc += nCharWidth;
				if (nCurDisc > nValidWidth)
				{
					nCurDisc = nCharWidth;
					pt.y += nLineHeight;
				}
			}
		}

		if (bIndexNext)
			nCurDisc -= nCharWidth;
		pt.x = nCurDisc;
	}
	else
	{
		char* pDest = strstr(pBegin, "\r\n");
		while (pDest)
		{
			if (pDest - (char*)lpStr >= nIndex) // nIndex在本行
			{
				pEnd = (char*)lpStr + nIndex;
				while (pBegin < pEnd)
				{
					bIsDblByteChar = FALSE;
					c = *pBegin++;
					if (c > 0x80)	// 双字节文字
					{
						c = c*256 + *pBegin++;
						bIsDblByteChar = TRUE;
					}
					
					nCharWidth = pFont->GetCharLength(c);
					nCurDisc += nCharWidth;
				}
				pt.x += nCurDisc;
				return;
			}
			else
			{
				pEnd = pDest;
				pt.x = 0;
				pt.y += nLineHeight;
				
				pBegin = pDest + 2;
				pDest = strstr(pBegin, "\r\n");			
			}
		}

		// 只有一行,或者nIndex在最后一行
		pt.x = 0;
		pEnd = (char*)lpStr + nIndex;
		nCurDisc = 0;
		while (pBegin < pEnd)
		{
			bIsDblByteChar = FALSE;
			c = *pBegin++;
			if (c > 0x80)	// 双字节文字
			{
				c = c*256 + *pBegin++;
				bIsDblByteChar = TRUE;
			}
			
			nCharWidth = pFont->GetCharLength(c);
			nCurDisc += nCharWidth;
		}
		pt.x += nCurDisc;

	}
}
//------------------------------------------------------------------------
// 分析一行（指有回车的一行，即硬回车行）
int XEdit::_GetCharIndexByOffset(char* pFirst, char* pLast, char* pBegin, char* pEnd, POINT& pt, BOOL bCmpHalfChar, BOOL bCalcCaretPos)
{
	ITTFont* pFont = GetFont();
	int nValidWidth = GetValidWidth();
	int nLineHeight = pFont->GetHeight() + m_nLineSpace;
	int nCurDisc = 0;
	WORD c;
	BOOL bIsDblByteChar = FALSE;
	int nCharWidth = 0;
	if (IsAutoWrap())
	{
		int nIndex = -1;
		if (pt.y < nLineHeight) // 偏移点就在本行
			nValidWidth = pt.x;
		while (pBegin < pEnd)
		{
			bIsDblByteChar = FALSE;
			c = *pBegin++;
			if (c > 0x80)	// 双字节文字
			{
				c = c*256 + *pBegin++;
				bIsDblByteChar = TRUE;
			}
			
			nCharWidth = pFont->GetCharLength(c);

			if (nIndex == -1)
			{
				if (bCmpHalfChar)
				{
					if (nCurDisc + nCharWidth/2 > pt.x)
					{
						nIndex = (bIsDblByteChar) ? (pBegin - pFirst - 2) : (pBegin - pFirst - 1);
						if (bCalcCaretPos)
							m_ptCaretPos.x = nCurDisc;
						if (pt.y < nLineHeight)
							return nIndex;
					}
					if (nCurDisc + nCharWidth > pt.x)
					{
						nIndex = pBegin - pFirst;
						if (bCalcCaretPos)
							m_ptCaretPos.x = nCurDisc + nCharWidth;
						if (pt.y < nLineHeight)
							return nIndex;
					}
				}
				else
				{
					if (nCurDisc + nCharWidth > pt.x)
					{
						nIndex = (bIsDblByteChar) ? (pBegin - pFirst - 2) : (pBegin - pFirst - 1);
						if (bCalcCaretPos)
							m_ptCaretPos.x = nCurDisc;
						if (pt.y < nLineHeight)
							return nIndex;
					}
				}
			}
			nCurDisc += nCharWidth;
			if (nCurDisc > nValidWidth)
			{
				nCurDisc = nCharWidth;
				pt.y -= nLineHeight;
				if (bCalcCaretPos)
				{
					m_ptCaretPos.x = nCurDisc;
					m_ptCaretPos.y += nLineHeight;
				}
				if (pt.y < nLineHeight && nCurDisc > pt.x)
				{
					nIndex = (bIsDblByteChar) ? (pBegin - pFirst - 2) : (pBegin - pFirst - 1);
					if (bCalcCaretPos)
						m_ptCaretPos.x = 0;
					return nIndex;
				}
				nIndex = -1;
			}

		}

		if (nIndex == -1)
		{
			if (pt.y < nLineHeight || pLast == pEnd)
			{
				if (bCalcCaretPos)
					m_ptCaretPos.x = nCurDisc;
				return pBegin - pFirst;
			}
		}
		else
		{
			if (pLast == pEnd)
				return nIndex;
		}

		pt.y -= nLineHeight;
		if (bCalcCaretPos)
			m_ptCaretPos.y += nLineHeight;
		return -1;
	}
	else // 没有自动回行
	{
		if (pt.y >= nLineHeight && pEnd != pLast)
		{
			pt.y -= nLineHeight;
			if (bCalcCaretPos)
			{
				m_ptCaretPos.x = 0;
				m_ptCaretPos.y += nLineHeight;
			}
		}
		else
		{
			int nIndex = -1;
			nValidWidth = pt.x;
			while (pBegin < pEnd)
			{
				bIsDblByteChar = FALSE;
				c = *pBegin++;
				if (c > 0x80)	// 双字节文字
				{
					c = c*256 + *pBegin++;
					bIsDblByteChar = TRUE;
				}
				
				nCharWidth = pFont->GetCharLength(c);
				if (bCmpHalfChar)
				{
					if (nCurDisc + nCharWidth/2 > nValidWidth)
					{
						if (bCalcCaretPos)
							m_ptCaretPos.x = nCurDisc;
						return nIndex = (bIsDblByteChar) ? (pBegin - pFirst - 2) : (pBegin - pFirst - 1);
					}
					if (nCurDisc + nCharWidth > nValidWidth)
					{
						if (bCalcCaretPos)
							m_ptCaretPos.x = nCurDisc + nCharWidth;
						return nIndex = pBegin - pFirst;
					}					
				}
				else
				{
					if (nCurDisc + nCharWidth > nValidWidth)
					{
						if (bCalcCaretPos)
							m_ptCaretPos.x = nCurDisc;
						return nIndex = (bIsDblByteChar) ? (pBegin - pFirst - 2) : (pBegin - pFirst - 1);
					}
				}
				nCurDisc += nCharWidth;
			}

			if (nIndex == -1) // 可能偏移点已经在本行的回车换行符的后面
			{
				if (bCalcCaretPos)
					m_ptCaretPos.x = nCurDisc;
				return nIndex = pBegin - pFirst;
			}
		}
	}

	return -1;
}
//------------------------------------------------------------------------
int XEdit::GetCharIndexByOffset(LPCSTR lpStr, POINT pt, BOOL bCmpHalfChar, BOOL bCalcCaretPos)
{
	// 得到Beam的位置
	if (!lpStr || lpStr[0] == '\0' || pt.x < 0 || pt.y < 0)
		return 0;

	char* pStr = (char*)lpStr;
	int nLen = strlen(pStr);
	string strPassword;
	if (IsPassword())
	{
		strPassword = string(nLen, m_cPasswordChar);
		pStr = (char*)strPassword.c_str();
	}

	if (bCalcCaretPos)
		m_ptCaretPos.x = m_ptCaretPos.y = 0;

	char* pFirst = pStr;
	char* pLast = pFirst + nLen;
	char* pBegin = pFirst;
	char* pEnd = pLast;
	if (!IsMultiLine()) // 单行
	{
		return _GetCharIndexByOffset(pFirst, pLast, pBegin, pEnd, pt, bCmpHalfChar, bCalcCaretPos);
	}
	
	// 多行的情况
	int nIndex = 0;
	char* pDest = strstr(pBegin, "\r\n");
	while (pDest)
	{
		pEnd = pDest;
		
		if (-1 != (nIndex=_GetCharIndexByOffset(pFirst, pLast, pBegin, pEnd, pt, bCmpHalfChar, bCalcCaretPos)))
			return nIndex;
		
		pBegin = pDest + 2;
		pDest = strstr(pBegin, "\r\n");
	};
	
	if (pBegin <= pLast)
	{
		if (pBegin > pEnd)
			pEnd = pLast;
		
		if (-1 != (nIndex=_GetCharIndexByOffset(pFirst, pLast, pBegin, pEnd, pt, bCmpHalfChar, bCalcCaretPos)))
			return nIndex;
	}
	return nIndex;
}
//------------------------------------------------------------------------
// 返回从文字开头到插入符位置的字符串
string XEdit::stringUpToCaret()
{
    string ret;
    if (GetCaretPos() > 0) 
		ret = m_strCaption.substr(0, GetCaretPos());
	
    return ret;
}
//------------------------------------------------------------------------
// 返回从插入符位置到末尾的字符串
string XEdit::stringFromCaret()
{
    string ret;
    if (GetCaretPos() < m_strCaption.length()) 
        ret = m_strCaption.substr(GetCaretPos(), m_strCaption.length());
	
    return ret;
}
//------------------------------------------------------------------------
void XEdit::CalcCaretPos()
{
	POINT pt = {0, 0};
	LPCSTR lpStr;
	if (IsPassword())
	{
		string str = GetDisplayString(m_strCaption);
		lpStr = str.c_str();
		_CalcOffsetByCharIndex(lpStr, GetCaretPos(), pt);
	}
	else
	{
		lpStr = m_strCaption.c_str();
		_CalcOffsetByCharIndex(lpStr, GetCaretPos(), pt);
	}
	m_ptCaretPos = pt;
}
//------------------------------------------------------------------------
// 得到插入符位置所在的距离，是整个窗口系统范围内的坐标
void XEdit::GetCaretDisc(POINT& pt)
{
	pt.x = pt.y = 0;
	string str = GetDisplayString(m_strCaption);
	_CalcOffsetByCharIndex(str.c_str(), GetCaretPos(), pt);
	m_ptCaretPos = pt;
	pt.x += m_ptOffset.x + m_rcWnd.left + m_nMargin;
	pt.y += m_ptOffset.y + m_rcWnd.top + m_nMargin;
}
//------------------------------------------------------------------------
// 向前查找一个单词的末尾位置
int XEdit::GetLastSpaceBefore(int pos)
{
    // 得到 pos 前面的字符串
    string str = m_strCaption.substr(0, pos);
	
    int ret = 0;
    if (str.length() > 0)
    {
        // 剪切掉后面的空白字符
        str.erase(str.find_last_not_of(' '), str.length());
		
		// 查找最后一个空白字符的位置
        string::size_type p = str.find_last_of(' ');
		
        if (p != string::npos) // 找到了
			ret = int(p+1);
    }
    return ret;
}
//------------------------------------------------------------------------
// 向后查找下一个单词的开始位置
int XEdit::GetNextWordStartPosAfter(int pos)
{
    // 得到 pos 后面的字符串
    string str = m_strCaption.substr(pos, m_strCaption.length()-pos);
	
    // 找出字符串开头非空白字符的位置
    const string::size_type firstNonSpacePos = str.find_first_not_of(' ');
	
    int ret = int(m_strCaption.length());
    if (firstNonSpacePos != string::npos)
    {
        // 剪切掉前面的空白字符
        str.erase(0, firstNonSpacePos);
		
        // 找下一个空白字符
        string::size_type p = str.find_first_of(' ');
		
        if (p != string::npos)
        {
            // 从空白字符到字符串末尾的字符串
            string str2 = str.substr(p, str.length());
            string::size_type p2 = str2.find_first_not_of(' ');
            if (p2 != string::npos)
                ret = int(p2) + int(p) + pos;
        }
    }
	
    return ret;
}
//------------------------------------------------------------------------
string XEdit::GetDisplayString(string& str)
{
	if (!IsPassword())
		return str.c_str();

	string strPassword(str.length(), m_cPasswordChar);
	return strPassword;
}
//------------------------------------------------------------------------
int XEdit::GetDisplayStringWidth(string& str)
{
	if (!IsPassword())
		return GetFont()->GetLength(str.c_str());

	string strPassword(str.length(), m_cPasswordChar);
	return GetFont()->GetLength(strPassword.c_str());
}
//------------------------------------------------------------------------
// 得到nPos位置处字符的字节数
int XEdit::GetCharBytes(int nPos)
{
	int nLen = m_strCaption.length();
	if (nPos < 0 || nPos >= nLen) return 0;
	if ((BYTE)m_strCaption.at(nPos) == '\r' && nPos+1 < nLen && (BYTE)m_strCaption.at(nPos+1) == '\n')
		return 2;
	if ((BYTE)m_strCaption.at(nPos) == '\n' && nPos-1 >= 0 && (BYTE)m_strCaption.at(nPos-1) == '\r')
		return 2;
	if ((BYTE)m_strCaption.at(nPos) < 0) // 是双字节的第一个字符(GB2312汉字的第二个也<0)
		return 2;
	if (IsDBCS(m_strCaption.c_str(), nPos))
		return 2;
	return 1;
}
//------------------------------------------------------------------------
void XEdit::SetWindowText(LPCSTR lpszString)
{
	if (!lpszString) return;
	if (m_nMaxLen <= 0) return;
	
	if (IsDigitOnly())
	{
		int nLen = lstrlen(lpszString);
		for (int i=0; i<nLen; i++)
		{
			if (!isdigit((int)lpszString[i]))
				return;
		}
	}

	if (IsValidSelection())
		ClearSelection();
	
	m_nCaretPos = 0;
	string str = lpszString;
	if (str.length() > m_nMaxLen)
	{
		int nClipLen = m_nMaxLen;
		int nPos = m_nMaxLen - 1;
		if (IsDBCSLead(str.c_str(), nPos))
			nClipLen --;
		str.erase(str.begin() + nClipLen, str.end());
	}
	if (str.length() > 1 && (BYTE)str.at(str.length()-1) == '\r') // 如果最后有半个回车换行符号
		str.erase(str.end()-1);
	XWindow::SetWindowText(str.c_str());
	s_textChanged((DWORD)str.c_str());
	SetCaretPos(str.length());
	CalcCaretPos();
	CalcValidSize();
	AdjustHScroll();
	AdjustVScroll();
}
//------------------------------------------------------------------------
void XEdit::ReDrawCaret()
{
	SetDrawBeam(TRUE);
	GetTimeAxis()->KillTimer(TIMERID_BEAMFLASH, static_cast<ITimerSink*>(this));
	GetTimeAxis()->SetTimer(TIMERID_BEAMFLASH, static_cast<ITimerSink*>(this), INTERVAL_BEAMFLASH);
}
//------------------------------------------------------------------------
void XEdit::CalcValidSize()
{
	if (IsMultiLine())
	{
		if (IsAutoWrap())
			CalcValidSize_MultiLine_AutoWrap();
		else
			CalcValidSize_MultiLine_NoAutoWrap();
	}
	else
	{
		CalcValidSize_NoMultiLine();
	}
}
//------------------------------------------------------------------------
void XEdit::CalcValidSize_NoMultiLine()
{
	ITTFont* pFont = GetFont();
	if (IsPassword())
		m_nValidWidth = m_strCaption.length()*pFont->GetCharLength(m_cPasswordChar);
	else
		m_nValidWidth = pFont->GetLength(m_strCaption.c_str());
	m_nValidWidth ++;	// 为了在最后一个字符后面的插入符能够显示出来
	m_nValidHeight = pFont->GetHeight();
}
//------------------------------------------------------------------------
void XEdit::CalcValidSize_MultiLine_AutoWrap()
{
	ITTFont* pFont = GetFont();
	string& str = m_strCaption;
	int nLen = str.length();
	char* pBegin = (char*)str.c_str();
	char* pEnd = pBegin + nLen;
	int nCurDisc = 0;
	int nLineHeight = pFont->GetHeight() + m_nLineSpace;
	BOOL bIsDblByteChar = FALSE;
	WORD c;
	int nCharWidth = 0;
	m_nValidWidth = GetWidth() - 2*GetMargin();	
	int nMaxHeight = nLineHeight;
	
	while (pBegin < pEnd)
	{
		bIsDblByteChar = FALSE;
		c = *pBegin++;
		if (c > 0x80)	// 双字节文字
		{
			c = c*256 + *pBegin++;
			bIsDblByteChar = TRUE;
		}
		
		if (c == '\r' && *pBegin == '\n')
		{
			pBegin ++;
			nCurDisc = 0;
			nMaxHeight += nLineHeight;
		}
		else
		{
			nCharWidth = pFont->GetCharLength(c);
			nCurDisc += nCharWidth;
			if (nCurDisc > m_nValidWidth)
			{
				nMaxHeight += nLineHeight;
				nCurDisc = nCharWidth;
			}
		}
	}
	m_nValidHeight = nMaxHeight;
}
//------------------------------------------------------------------------
void XEdit::CalcValidSize_MultiLine_NoAutoWrap()
{
	string str = GetDisplayString(m_strCaption);
	ITTFont* pFont = GetFont();
	int nLen = str.length();
	char* pBegin = (char*)str.c_str();
	char* pEnd = pBegin + nLen;
	int nCurDisc = 0;
	int nLineHeight = pFont->GetHeight() + m_nLineSpace;
	BOOL bIsDblByteChar = FALSE;
	WORD c;
	int nCharWidth = 0;
	int nMaxWidth = 0;
	int nMaxHeight = nLineHeight;

	while (pBegin < pEnd)
	{
		bIsDblByteChar = FALSE;
		c = *pBegin++;
		if (c > 0x80)	// 双字节文字
		{
			c = c*256 + *pBegin++;
			bIsDblByteChar = TRUE;
		}
		
		if (c == '\r' && *pBegin == '\n')
		{
			pBegin ++;
			nCurDisc = 0;
			nMaxHeight += nLineHeight;
		}
		else
		{
			nCharWidth = pFont->GetCharLength(c);
			nCurDisc += nCharWidth;
			if (nCurDisc > nMaxWidth)
				nMaxWidth = nCurDisc;
		}
	}
	
	m_nValidWidth = nMaxWidth;
	m_nValidHeight = nMaxHeight;
}
//------------------------------------------------------------------------
void XEdit::SetWindowPos(RECT& rc)
{
	XWindow::SetWindowPos(rc);
	CalcValidSize();
}
//------------------------------------------------------------------------