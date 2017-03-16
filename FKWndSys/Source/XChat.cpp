/**
*	created:		2012-6-23   23:22
*	filename: 		XChat
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
inline void XChat::XChatScrollBar::LineLeftUp()
{
	int nPos = GetScrollPos();
	nPos -= 16;
	SetScrollPos(nPos);
}
//------------------------------------------------------------------------
inline void XChat::XChatScrollBar::LineRightDown()
{
	int nPos = GetScrollPos();
	nPos += 16;
	SetScrollPos(nPos);
}
//------------------------------------------------------------------------
XChat::XChat()
{
	m_bAutoSroll = TRUE;
	m_nMaxLines = 500;
	m_pParser = NULL;

	m_pVScrollBar = new XChatScrollBar(this);
	m_pVScrollBar->Attach(this);
	AddWindow(m_pVScrollBar);
	m_pVScrollBar->SetStyle(XScrollBar::S_VERT);
	SIZE size;
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pVScrollBar->SetSize(size);
	m_pVScrollBar->SetScrollRange(0, 0);
	m_pVScrollBar->SetScrollPos(0);

	m_pContainer = new XItemContainer(this);
	m_pContainer->SetAutoWrap(TRUE);
	m_pContainer->SetVScrollBar(m_pVScrollBar);
	AddWindow(m_pContainer);
}
//------------------------------------------------------------------------
XChat::XChat(XWindow* pParentWnd) : XWindow(pParentWnd)
{
	m_bAutoSroll = TRUE;
	m_nMaxLines = 500;
	m_pParser = NULL;

	m_pVScrollBar = new XChatScrollBar(this);
	m_pVScrollBar->Attach(this);
	AddWindow(m_pVScrollBar);
	m_pVScrollBar->SetVisible(FALSE);
	m_pVScrollBar->SetStyle(XScrollBar::S_VERT);
	SIZE size;
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pVScrollBar->SetSize(size);
	m_pVScrollBar->SetScrollRange(0, 0);
	m_pVScrollBar->SetScrollPos(0);

	m_pContainer = new XItemContainer(this);
	m_pContainer->SetAutoWrap(TRUE);
	m_pContainer->SetVScrollBar(m_pVScrollBar);
	AddWindow(m_pContainer);
}
//------------------------------------------------------------------------
XChat::~XChat()
{
}
//------------------------------------------------------------------------
void XChat::SetMargin(int nMargin)	
{
	if (m_pContainer)
		m_pContainer->SetMargin(nMargin);
}
//------------------------------------------------------------------------
void XChat::SetAutoWrap(BOOL bWrap)	
{
	if (m_pContainer)
		m_pContainer->SetAutoWrap(bWrap);
}
//------------------------------------------------------------------------
void XChat::SetParser(XParserBase* pParser)
{
	if (pParser)
	{
		pParser->SetItemContainer(m_pContainer);
		m_pParser = pParser;
	}
}
//------------------------------------------------------------------------
void XChat::Clear()
{
	m_pContainer->Clear();
	AdjustScroll();
}
//------------------------------------------------------------------------
/*
// 添加一般性文本
BOOL XChat::AddText(LPCSTR szText, BOOL bNewLine, WPixel color, ITTFont* pFont)
{
	// 确保字体不为空
	if (pFont == NULL)
		pFont = GetFont();

	// 新建一个文本条目，并加入条目数组
	XItemText* pItem = new XItemText;
	pItem->SetText(szText);
	pItem->SetFont(pFont);
	pItem->SetTextColor(color);

	if (!m_pContainer->AddItem(static_cast<XItem*>(pItem), bNewLine))
	{
		TraceColorLn(MY_TRACE_COLOR, "XChat::AddText, AddItem return FALSE");
		return FALSE;
	}

	AdjustScroll();
	//InvalidRect();
	return TRUE;
}

// 添加链接文本
BOOL XChat::AddLinkText(LPCSTR szDescText, LPCSTR szLink, BOOL bNewLine, 
							WPixel cNotVisited,	WPixel cVisited, WPixel cRollover, ITTFont* pFont)
{
	// 确保字体不为空
	if (pFont == NULL)
		pFont = GetFont();
	
	// 新建一个文本链接条目，并加入条目数组
	XItemLinkText* pItem = new XItemLinkText;
	pItem->SetText(szDescText);
	pItem->SetFont(pFont);
	pItem->SetTextColor(cNotVisited);
	pItem->SetLinkText(szLink);
	pItem->SetVisited(FALSE);
	pItem->SetNotVisitedColor(cNotVisited);
	pItem->SetVisitedColor(cVisited);
	pItem->SetRolloverColor(cRollover);

	if (!m_pContainer->AddItem(static_cast<XItem*>(pItem), bNewLine))
	{
		TraceColorLn(MY_TRACE_COLOR, "XChat::AddLinkText, AddItem return FALSE");
			return FALSE;
	}
	
	AdjustScroll();
	//InvalidRect();
	return TRUE;
}

// 添加一张图片
BOOL XChat::AddImage(IBitmap* pImage, int nFrameCount, BOOL bNewLine)
{
	// 新建一个图象条目，并加入条目数组
	XItemImage* pItem = new XItemImage;
	pItem->SetBitmap(pImage);
	pItem->SetFrameCount(nFrameCount);

	if (!m_pContainer->AddItem(static_cast<XItem*>(pItem), bNewLine))
	{
		TraceColorLn(MY_TRACE_COLOR, "XChat::AddImage, AddItem return FALSE");
		return FALSE;
	}	
	
	AdjustScroll();
	//InvalidRect();
	return TRUE;
}

// 添加链接图片
BOOL XChat::AddLinkImage(IBitmap* pLinkImage, int nFrameCount, LPCSTR szLink, BOOL bNewLine)
{
	// 新建一个图象链接条目，并加入条目数组
	XItemLinkImage* pItem = new XItemLinkImage;
	pItem->SetBitmap(pLinkImage);
	pItem->SetFrameCount(nFrameCount);
	pItem->SetLinkText(szLink);
	
	if (!m_pContainer->AddItem(static_cast<XItem*>(pItem), bNewLine))
	{
		TraceColorLn(MY_TRACE_COLOR, "XChat::AddLinkImage, AddItem return FALSE");
		return FALSE;
	}
	
	AdjustScroll();
	//InvalidRect();
	return TRUE;
}*/
//------------------------------------------------------------------------
void XChat::AdjustScroll()
{
	// 先检查总行数是否超过，如超过，则一次性删除多行
	if (m_pContainer->GetLineCount() > m_nMaxLines)
	{
		m_pContainer->DeleteLines(m_nMaxLines/4);
	}


	if (IsAutoScroll())
	{
		if (m_pVScrollBar && m_pVScrollBar->IsEnabled())
		{
			POINT* ptOff = m_pContainer->GetOffsetPtr();
			int nMaxRange = m_pVScrollBar->GetMaxRange();
			if (-ptOff->y != nMaxRange)
				m_pVScrollBar->SetScrollPos(nMaxRange);
		}
	}
}
//------------------------------------------------------------------------
void XChat::SetWindowPos(RECT& rc)
{
	XWindow::SetWindowPos(rc);
	
	RECT rcScroll, rcContainer;
	CopyRect(&rcScroll, &rc);
	CopyRect(&rcContainer, &rc);
	SIZE size;
	if (m_pVScrollBar)
	{
		m_pVScrollBar->GetSize(size);
		rcScroll.left = m_rcWnd.left;
		rcScroll.right = rcScroll.left + size.cx;
		m_pVScrollBar->SetWindowPos(rcScroll);
		m_pVScrollBar->SetSize(size);
		m_pVScrollBar->Refresh();
		rcContainer.left += size.cx;
	}
	m_pContainer->SetWindowPos(rcContainer);
}
//------------------------------------------------------------------------
DWORD XChat::OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt)
{
	return m_pContainer->OnMouseWheel(nButton, nShift, zDelta, pt);
}
//------------------------------------------------------------------------
DWORD XChat::OnKeyDown(UINT nKeyCode, UINT nShift)
{
	return m_pContainer->OnKeyDown(nKeyCode, nShift);
}
//------------------------------------------------------------------------
DWORD XChat::OnKeyUp(UINT nKeyCode, UINT nShift)
{
	return m_pContainer->OnKeyUp(nKeyCode, nShift);
}
//------------------------------------------------------------------------
BOOL XChat::ParseString(LPCSTR szUnFormatText)
{
	if (!m_pParser)
		return FALSE;

	m_pParser->ParseLine(szUnFormatText);
	AdjustScroll();
	return TRUE;
}
//------------------------------------------------------------------------