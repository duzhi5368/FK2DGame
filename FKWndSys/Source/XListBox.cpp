/**
*	created:		2012-6-23   23:45
*	filename: 		XlistBox
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
#define XLB_ERR (-1)
//------------------------------------------------------------------------
XListBox::XListBox()
{
	m_nSelectIndex = -1;
	m_nVisibleCount = 0;
	m_nItemHeight = 0;
	m_bAutoSort = FALSE;
	m_lstItem.clear();

	m_pVScrollBar = new XScrollBar(static_cast<XWindow*>(this));
	m_pVScrollBar->Attach(this);
	AddWindow(m_pVScrollBar);
	m_pVScrollBar->SetVisible(FALSE);
	m_pVScrollBar->SetStyle(XScrollBar::S_VERT);
	SIZE size;
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pVScrollBar->SetSize(size);
	m_pVScrollBar->SetScrollRange(0, 0);
	m_pVScrollBar->SetScrollPos(0);

	CONN_posChanged(m_pVScrollBar, XListBox, OnScrollPosChanged);
}
//------------------------------------------------------------------------
XListBox::XListBox(XWindow* pParentWnd) : XWindow(pParentWnd)
{
	m_nSelectIndex = -1;
	m_nVisibleCount = 0;
	m_nItemHeight = 0;
	m_bAutoSort = FALSE;
	m_lstItem.clear();

	m_pVScrollBar = new XScrollBar(static_cast<XWindow*>(this));
	m_pVScrollBar->Attach(this);
	AddWindow(m_pVScrollBar);
	m_pVScrollBar->SetVisible(FALSE);
	m_pVScrollBar->SetStyle(XScrollBar::S_VERT);
	SIZE size;
	size.cx = size.cy = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	m_pVScrollBar->SetSize(size);
	m_pVScrollBar->SetScrollRange(0, 0);
	m_pVScrollBar->SetScrollPos(0);

	CONN_posChanged(m_pVScrollBar, XListBox, OnScrollPosChanged);
}
//------------------------------------------------------------------------
XListBox::~XListBox() 
{

}
//------------------------------------------------------------------------
int XListBox::GetCount()
{
	return m_lstItem.size();
}
//------------------------------------------------------------------------
int XListBox::ItemFromPoint(POINT pt)
{
	POINT ptTemp = pt;
	ptTemp.y -= m_rcWnd.top;

	int nIndex = GetTopIndex() + ptTemp.y / GetItemHeight();
	if (nIndex < 0 || nIndex > GetCount()) return -1;
	return nIndex;
}
//------------------------------------------------------------------------
void XListBox::SetItemHeight(int nHeight)
{
	m_nItemHeight = nHeight;
}
//------------------------------------------------------------------------
int XListBox::GetItemHeight()
{
	if (m_nItemHeight == 0)
		m_nItemHeight = GetFont()->GetHeight();
	return m_nItemHeight ;
}
//------------------------------------------------------------------------
int XListBox::SetTopIndex(int nIndex)
{
	m_pVScrollBar->SetScrollPos(nIndex);
	return 1;
}
//------------------------------------------------------------------------
int XListBox::GetTopIndex()
{
	return m_pVScrollBar->GetScrollPos();
}
//------------------------------------------------------------------------
int XListBox::AddItem(LPCSTR lpszItem)
{
	return AddItem(lpszItem, 0);
}
//------------------------------------------------------------------------
int XListBox::AddItem(LPCSTR lpszItem, DWORD dwItemData)
{
	if (!lpszItem)
		return 0;
	
	ItemData id;
	id.dwItemData = dwItemData;
	id.str = lpszItem;
	BOOL bFound = FALSE;
	if (IsAutoSort())
	{
		ItemListPtr it = m_lstItem.begin();
		ItemListPtr _end = m_lstItem.end();
		while (it != _end)
		{
			if (id < (*it))
			{
				bFound = TRUE;
				m_lstItem.insert(it, id);
				break;
			}
			else
			{
				++it;
				continue;
			}
		}
	}
	if (!bFound)
		m_lstItem.push_back(id);
	
	int nCount = GetCount();
	
	if (nCount > m_nVisibleCount)
	{
		if (!m_pVScrollBar->IsVisible())
			m_pVScrollBar->SetVisible(TRUE);
		m_pVScrollBar->SetScrollRange(0, nCount - m_nVisibleCount, TRUE);
		m_pVScrollBar->Refresh();
	}
	return nCount;
}
//------------------------------------------------------------------------
int XListBox::RemoveItem(int nIndex)
{
	if (nIndex < 0) return 0;
	if (nIndex == m_nSelectIndex)
		m_nSelectIndex = -1;
	if (nIndex < GetCount())
	{
		list<ItemData>::iterator it = m_lstItem.begin();
		advance(it, nIndex);
		m_lstItem.erase(it);
		return TRUE;
	}
	return 0;
}
//------------------------------------------------------------------------
int XListBox::InsertItem(int nIndex, LPCSTR lpszItem)
{
	ASSERT(nIndex >= 0);
	ASSERT(lpszItem);

	if (nIndex <= m_nSelectIndex)
		m_nSelectIndex ++;
	if (nIndex < GetCount())
	{
		list<ItemData>::iterator it = m_lstItem.begin();
		advance(it, nIndex);
		ItemData id;
		id.dwItemData = 0;
		id.str = lpszItem;
		m_lstItem.insert(it, id);
		return TRUE;
	}
	return 0;
}
//------------------------------------------------------------------------
void XListBox::RemoveAll()
{
	m_nSelectIndex = -1;
	m_lstItem.clear();
	m_pVScrollBar->SetScrollRange(0, 0);
	m_pVScrollBar->SetScrollPos(0);
	m_pVScrollBar->SetVisible(FALSE);
}
//------------------------------------------------------------------------
int XListBox::FindItem(int nStartAfter, LPCSTR lpszItem)
{
	list<ItemData>::iterator it = m_lstItem.begin();
	advance(it, nStartAfter);
	int nIndex = nStartAfter;
	for (; it != m_lstItem.end(); it ++)
	{
		if (stricmp((*it).str.c_str(), lpszItem) == 0)
			return nIndex;

		nIndex ++;
	}
	return -1;
}
//------------------------------------------------------------------------
int XListBox::SelectItem(int nStartAfter, LPCSTR lpszItem)
{
	int nIndex = FindItem(nStartAfter, lpszItem);
	if (nIndex != XLB_ERR)
		SetCurSel(nIndex);
	return 1;
}
//------------------------------------------------------------------------
LPCSTR XListBox::GetText(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetCount())
		return NULL;
	list<ItemData>::iterator it = m_lstItem.begin();
	advance(it, nIndex);
	return (*it).str.c_str();
}
//------------------------------------------------------------------------
int XListBox::SetItemData(int nIndex, DWORD dwItemData)
{
	if (nIndex < 0 || nIndex >= GetCount())
		return XLB_ERR;
	list<ItemData>::iterator it = m_lstItem.begin();
	advance(it, nIndex);
	(*it).dwItemData = dwItemData;
	return TRUE;
}
//------------------------------------------------------------------------
DWORD XListBox::GetItemData(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetCount())
		return 0;
	list<ItemData>::iterator it = m_lstItem.begin();
	advance(it, nIndex);
	return (*it).dwItemData;
}
//------------------------------------------------------------------------
int XListBox::SetCurSel(int nSelect)
{
	if (nSelect >= (int)m_lstItem.size())
		return -1;
	if (nSelect == m_nSelectIndex)
		return m_nSelectIndex;
	if (nSelect < -1)
		nSelect = -1;
	m_nSelectIndex = nSelect;
	InvalidRect();

	s_selChanged((DWORD)&m_nSelectIndex);

	return m_nSelectIndex;
}
//------------------------------------------------------------------------
int XListBox::GetCurSel()
{
	return m_nSelectIndex;
}
//------------------------------------------------------------------------
void XListBox::SetWindowPos(RECT& rc)
{
	XWindow::SetWindowPos(rc);
	RECT rcScroll;
	CopyRect(&rcScroll, &rc);
	int nDelta = GetDesktop()->GetSysScrollBarBitmap()->GetHeight();
	rcScroll.left = m_rcWnd.right - nDelta;
	m_pVScrollBar->SetWindowPos(rcScroll);
	SIZE size;size.cx = nDelta;size.cy = nDelta;
	m_nVisibleCount = GetHeight()/GetItemHeight();
	m_pVScrollBar->SetVisibleCount(m_nVisibleCount);
	m_pVScrollBar->SetSize(size);
	m_pVScrollBar->Refresh();
}
//------------------------------------------------------------------------
// 将一个大字符串解析成字符串数组，并加入到列表框
void XListBox::ParseStringToListBox(LPCSTR lpszContext, char cSeparator)
{
	char * start, * str;
	char val[256];
	start = str = (char*)lpszContext;
	if (!lpszContext || lpszContext[0] == '\0')
		return;
	while (*str != 0)
	{
		if (*str == cSeparator)
		{
			memcpy(val, start, str - start);
			val[str - start] = 0;
			start = str + 1;
			AddItem(val);
		}
		str ++;
	}
	memcpy(val, start, str - start);
	val[str - start] = 0;
	AddItem(val);
}
//------------------------------------------------------------------------
void XListBox::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	ITTFont* pFont = GetFont();
	if (!m_lstItem.empty())
	{
		int nCount = m_lstItem.size();
		int nTopIndex = GetTopIndex();
		list<ItemData>::iterator it = m_lstItem.begin();

		if (nCount > m_nVisibleCount)
		{
			nCount = m_nVisibleCount + nTopIndex;
			advance(it, nTopIndex);
		}

		// 绘制可见的条目
		for (int i=nTopIndex; i<nCount; i++,++it)
		{
			int x = m_rcWnd.left + 2;
			int y = m_rcWnd.top + (i - nTopIndex)*GetItemHeight();
			RECT rcNewClip = rcClip;
			
			if (GetCurSel() == i)
			{
				RECT rc;
				rc.left = m_rcWnd.left;
				if (m_pVScrollBar->IsVisible())
					rc.right = m_rcWnd.right - m_pVScrollBar->GetWidth();
				else
					rc.right = m_rcWnd.right;
				rc.top = y;
				rc.bottom = y + GetItemHeight();

				WPixel selbkcolor, seltextcolor;
				selbkcolor.red = 31 - m_cBackColor.red;
				selbkcolor.green = 63 - m_cBackColor.green;
				selbkcolor.blue = 31 - m_cBackColor.blue;
				seltextcolor.red = 31 - m_cForeColor.red;
				seltextcolor.green = 63 - m_cForeColor.green;
				seltextcolor.blue = 31 - m_cForeColor.blue;
				rcNewClip.right = rc.right;
				if (IsGlobalFocus())
				{
					pCanvas->DrawBox(rc, selbkcolor, BLIT_NORMAL, 255);
					pCanvas->DrawClipText(pFont, x, y, (*it).str.c_str(), seltextcolor, rcNewClip, FONT_NOALPHA);
				}
				else
				{
					selbkcolor.red /= 2;
					selbkcolor.green /= 2;
					selbkcolor.blue /= 2;
					seltextcolor.red /= 2;
					seltextcolor.green /= 2;
					seltextcolor.blue /= 2;
					pCanvas->DrawBox(rc, selbkcolor, BLIT_NORMAL, 255);
					pCanvas->DrawClipText(pFont, x, y, (*it).str.c_str(), seltextcolor, rcNewClip, FONT_NOALPHA);
				}
			}
			else
			{
				if (m_pVScrollBar->IsVisible())
					rcNewClip.right -= m_pVScrollBar->GetWidth();				
				pCanvas->DrawClipText(pFont, x, y, (*it).str.c_str(), m_cForeColor, rcNewClip, FONT_NOALPHA);
			}
		}

	}

	XWindow::OnDraw(pCanvas, rcClip);
}
//------------------------------------------------------------------------
void XListBox::OnScrollPosChanged(DWORD dwParam)
{
	InvalidRect();
}
//------------------------------------------------------------------------
DWORD XListBox::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	if (nButton == LeftButton)
	{
		if (PtInRect(&m_rcWnd, pt))
		{
			if (m_pVScrollBar->IsVisible())
			{
				RECT rc;
				m_pVScrollBar->GetWindowRect(&rc);
				if (PtInRect(&rc, pt))
				{
					m_pVScrollBar->OnMouseDown(nButton, nShift, pt);
					return 0;
				}
			}
			
			SetCurSel(ItemFromPoint(pt));
		}
	}

	return XWindow::OnMouseDown(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XListBox::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{
	if (IsCapture())
	{ 
		if (nButton == LeftButton)
		{
			int nIndex = ItemFromPoint(pt);
			int nTopIndex = GetTopIndex();
			if (nIndex >= nTopIndex && nIndex < nTopIndex + m_nVisibleCount) // 限制在可视区域
				SetCurSel(nIndex);
		}
	}
	else
	{
		if (m_pVScrollBar->IsVisible())
		{
			RECT rc;
			m_pVScrollBar->GetWindowRect(&rc);
			if (PtInRect(&rc, pt))
			{
				m_pVScrollBar->OnMouseMove(nButton, nShift, pt);
				return 0;
			}
		}
		SetCursor(CT_ARROW);
	}

	return XWindow::OnMouseMove(nButton, nShift, pt);
}
//------------------------------------------------------------------------
/*
#if WNDSYS_VERSION > 1202
void XListBox::OnMouseEnterItem(int nIndex)
{
	list<ItemData>::iterator it = m_lstItem.begin();
	advance(it, nIndex);
	ToolTipSetInfo(NULL, (*it).str.c_str(), GetBackColor(), GetForeColor());
}
#endif
*/
//------------------------------------------------------------------------
DWORD XListBox::OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt)
{
	if (m_pVScrollBar->IsVisible())
	{
		if (zDelta < 0)
		{
			zDelta = -zDelta;
			while (zDelta--)
				m_pVScrollBar->LineRightDown();
		}
		else
		{
			while (zDelta--)
				m_pVScrollBar->LineLeftUp();
		}
		return 0;
	}

	return XWindow::OnMouseWheel(nButton, nShift, zDelta, pt);
}
//------------------------------------------------------------------------
DWORD XListBox::OnKeyDown(UINT nKeyCode, UINT nShift)
{
	switch (nKeyCode)
	{
	case VK_HOME:
		{
			SetCurSel(0);
			m_pVScrollBar->SetScrollPos(0);
		}
		break;
	case VK_END:
		{
			SetCurSel(GetCount()-1);
			m_pVScrollBar->SetScrollPos(m_pVScrollBar->GetMaxRange());
		}
		break;
	case VK_UP:
		{
			if (GetCurSel() >= 1)
				SetCurSel(GetCurSel() - 1);
			if (GetCurSel() < GetTopIndex())
				m_pVScrollBar->LineLeftUp();
		}
		break;
	case VK_DOWN:
		{
			SetCurSel(GetCurSel() + 1);
			if (GetCurSel() >= GetTopIndex() + m_nVisibleCount)
				m_pVScrollBar->LineRightDown();
		}
		break;
	case VK_PRIOR:
		{
			int nIdx = GetCurSel() - m_nVisibleCount;
			if (nIdx < 0) nIdx = 0;
			SetCurSel(nIdx);
			if (GetCurSel() < GetTopIndex())
				m_pVScrollBar->PageLeftUp();
		}
		break;
	case VK_NEXT:
		{
			int nIdx = GetCurSel() + m_nVisibleCount;
			if (nIdx > GetCount()-1) nIdx = GetCount()-1;
			SetCurSel(nIdx);
			if (GetCurSel() >= GetTopIndex() + m_nVisibleCount)
				m_pVScrollBar->PageRightDown();
		}
		break;
	default:
		{
			return XWindow::OnKeyDown(nKeyCode, nShift);
		}
	}

	return 0;
}
//------------------------------------------------------------------------
void XListBox::OnMouseEnter()
{
	SetCursor(CT_ARROW);
}
//------------------------------------------------------------------------
void XListBox::OnMouseExit()
{
	SetCursor(CT_ARROW);
}
//------------------------------------------------------------------------
void XListBox::SetScrollBar(XWindow* pVScroll) 
{
	if (m_pVScrollBar)
	{
		RemoveWindow((XWindow*)m_pVScrollBar);
		m_pVScrollBar->Release();
	}
	m_pVScrollBar = static_cast<XScrollBar*>(pVScroll);
}
//------------------------------------------------------------------------
void XListBox::OnSetFocus()
{
	InvalidRect();
}
//------------------------------------------------------------------------
void XListBox::OnKillFocus()
{
	InvalidRect();
}
//------------------------------------------------------------------------