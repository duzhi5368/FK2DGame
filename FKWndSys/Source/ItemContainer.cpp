/**
*	created:		2012-6-23   23:25
*	filename: 		ItemContainer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
XItemContainer::XItemContainer()
{
	m_nMargin = 0;
	m_bLineWrap = FALSE;
	m_nMouseHover = MAKELONG(-1, -1);
	m_nMaxWidth = m_nMaxHeight = 0;
	m_ptOffset.x = m_ptOffset.y = 0;
	m_pHScrollBar = m_pVScrollBar = NULL;

	CONN_clicked(this, XItemContainer, OnClick);
}
//------------------------------------------------------------------------
XItemContainer::XItemContainer(XWindow* pParentWnd) : XWindow(pParentWnd)
{
	m_nMargin = 0;
	m_bLineWrap = FALSE;
	m_nMouseHover = MAKELONG(-1, -1);
	m_nMaxWidth = m_nMaxHeight = 0;
	m_ptOffset.x = m_ptOffset.y = 0;
	m_pHScrollBar = m_pVScrollBar = NULL;

	CONN_clicked(this, XItemContainer, OnClick);
}
//------------------------------------------------------------------------
XItemContainer::~XItemContainer()
{
}
//------------------------------------------------------------------------
void XItemContainer::DestroyWindow()
{
	m_pHScrollBar = NULL;
	m_pVScrollBar = NULL;
	Clear();
	XWindow::DestroyWindow();
	
}
//------------------------------------------------------------------------
// 容器内容清除
void XItemContainer::Clear()
{
	DeleteLines(GetLineCount());
	m_nMouseHover = MAKELONG(-1, -1);
	SetMaxWidth(GetWidth() - 2*m_nMargin);
	SetMaxHeight(GetHeight() - 2*m_nMargin);
	SetOffset(0, 0);
}
//------------------------------------------------------------------------
// 将某行置为顶行(nLine为行索引，从0开始)
void XItemContainer::BringToTopLine(int nLine)
{
	if (nLine >= GetLineCount()) // 如果超过最大行数，强行限制到最后一行
		nLine = GetLineCount() - 1;
	XLine* pLine = GetLine(nLine);
	int nLineOffset = pLine->GetOffsetY();
	if (nLineOffset + GetHeight() - 2*m_nMargin > GetMaxHeight())
		nLineOffset = GetMaxHeight() - GetHeight() + 2*m_nMargin;
	if (m_pVScrollBar)
		m_pVScrollBar->SetScrollPos(nLineOffset);
	else
		m_ptOffset.y = -nLineOffset;
	AdjustVScroll();
}
//------------------------------------------------------------------------
// 新建一行，并返回他的对应行指针
XLine* XItemContainer::NewLine(int nAlign, int nLineSpace)
{
	XLine* pLine = new XLine(nAlign, static_cast<ILineContainer*>(this));
	pLine->SetLineSpace(nLineSpace);
	if (m_vLines.empty())
		pLine->SetOffsetY(0);
	else
	{
		XLine* pLinePrev = GetLine(GetLineCount()-1);
		pLine->SetOffsetY(pLinePrev->GetOffsetY() + pLinePrev->GetHeight() + pLinePrev->GetLineSpace());
	}
	m_vLines.push_back(pLine);
	return pLine;
}
//------------------------------------------------------------------------
int	XItemContainer::GetMaxValidWidth()
{
	return GetMaxWidth()/* - m_nMargin*2*/;
}
//------------------------------------------------------------------------
// 根据偏移距离得到对应行的条目索引
// nLine:行索引
// nOffsetX:相对容器逻辑区域的偏移值，即相对m_ptOffset所在位置，不是相对窗口矩形的偏移值
int XItemContainer::GetItemByOffset(int nLine, int nOffsetX)
{
	if (nLine < 0 || nLine >= GetLineCount())
		return -1;
	XLine* pLine = GetLine(nLine);
	ASSERT(pLine);

	nOffsetX -= GetAlignOffset(pLine);
	XItem* pItem = NULL;
	int nItemCount = pLine->GetItemCount();
	for (int i=0; i<nItemCount; i++)
	{
		pItem = pLine->GetItem(i);
		if (nOffsetX >= pItem->GetOffsetX() && 
			nOffsetX < pItem->GetOffsetX() + pItem->GetWidth())
			return i;
	}
	return -1;
}
//------------------------------------------------------------------------
// 从容器第一行连续删除多行
// nLines: 行数，不是索引
void XItemContainer::DeleteLines(int nLines)
{
	if (nLines <= 0)
		return;
	int nLineCount = GetLineCount();
	if (nLines >= nLineCount) // 清除所有内容
	{
		// 清除行数据
		for (int i=0; i<nLineCount; i++)
			GetLine(i)->Clear();
		m_vLines.clear();
		
		// 清除父条目
		int nParentItemCount = m_vSplitItems.size();
		for ( int i=0; i<nParentItemCount; i++)
			m_vSplitItems[i]->Release();
		m_vSplitItems.clear();

		AdjustVScroll();
		InvalidRect();
		return;
	}
	
	// 下面的过程，将在容器内删除最前面的nLines行
	// 先找到要删除的行或之前的行中的最大的父条目索引号，保证删除后的第一行的父条目不能删除
	int nMaxLineOffset = 0;
	int nParentIndex = -1;
	int nLine = nLines - 1;
	XLine* pLine = GetLine(nLine);
	ASSERT(pLine);
	nMaxLineOffset = pLine->GetOffsetY() + pLine->GetHeight() + pLine->GetLineSpace();
	int nItem = pLine->GetItemCount() - 1;
	XItem* pItem = NULL;
	if (nItem >= 0)
	{
		pItem = pLine->GetItem(nItem);
		nParentIndex = pItem->GetParent();
	}
	if (nParentIndex > XItem::NotParent)
	{
		if (pItem->GetLinkStyle() & XItem::lsLinkNext) // 如果跟删除后的第一行是连接的，则不删除他的父对象
			nParentIndex --;
	}
	else
	{
		while (nParentIndex == XItem::NotParent)
		{
			// 上一个
			if (nItem <= 0) // 转到上一行
			{
				if (--nLine < 0)
					break;
				pLine = GetLine(nLine);
				nItem = pLine->GetItemCount() - 1;
			}
			else
				nItem--;
			if (nItem >= 0)
			{
				pItem = pLine->GetItem(nItem);
				ASSERT(pItem);
				nParentIndex = pItem->GetParent();
			}
		}
	}

	// 现在的第一个item是否有XItem::IsLinkPrev属性
	pLine = GetLine(nLines);
	if (pLine)
	{
		pItem = pLine->GetItem(0);
		if (pItem && (pItem->GetLinkStyle() & XItem::lsLinkPrev)) // 确保跟前一行断开连接
			pItem->m_nLinkStyle &= ~XItem::lsLinkPrev; // 由于不好修改接口，只好这样强制了
	}
	
	// 对nLines后面的行修改数据
	if (nParentIndex >= 0) // 说明存在要删除父对象，所以删除后，后面的行的父对象的索引要偏移
	{
		for (int i=nLines; i<nLineCount; i++)
		{
			pLine = GetLine(i);
			if (pLine)
			{
				pLine->SetOffsetY(pLine->GetOffsetY() - nMaxLineOffset);
				int nItemCount = pLine->GetItemCount();
				for (int j=0; j<nItemCount; j++)
				{
					pItem = pLine->GetItem(j);
					if (pItem && (pItem->GetParent() != XItem::NotParent))
						pItem->SetParent(pItem->GetParent() - nParentIndex - 1);
				}
			}
		}
	}

	// 删除多余的行
	for (int m=0; m<nLines; m++)
		GetLine(m)->Clear();
	m_vLines.erase(m_vLines.begin(), m_vLines.begin() + nLines);

	// 删除多余的父条目
	if (nParentIndex >= 0)
	{
		for (int i=0; i<=nParentIndex; i++)
			m_vSplitItems[i]->Release();
		m_vSplitItems.erase(m_vSplitItems.begin(), m_vSplitItems.begin() + nParentIndex + 1);
	}

	int nOldLine = (short)HIWORD(m_nMouseHover);
	int nOldItem = (short)LOWORD(m_nMouseHover);
	nOldLine -= nLines;
	if (nOldLine < -1)
		nOldLine = -1;
	m_nMouseHover = MAKELONG(nOldItem, nOldLine);

	AdjustVScroll();
	InvalidRect();
}
//------------------------------------------------------------------------
// 添加一个条目
BOOL XItemContainer::AddItem(XItem* pItem, BOOL bNewLine, int nAlign, int nLineSpace)
{
	if (pItem == NULL || !pItem->IsValid())
		return FALSE;

	// 如果需要，先构建一个行结构
	XLine* pLine = NULL;
	if (bNewLine)
		pLine = NewLine(nAlign, nLineSpace);
	else
	{
		if (m_vLines.empty())
		{
			bNewLine = TRUE;
			pLine = NewLine(nAlign, nLineSpace);
		}
		else
			pLine = GetLine(GetLineCount() - 1);
	}
	int nRemainWidth = GetMaxValidWidth() - pLine->GetWidth();
	
	// 加入条目数组
	int itemWidth = pItem->GetWidth();
	int itemHeight = pItem->GetHeight();

	if (m_bLineWrap)
	{
		if (itemWidth > nRemainWidth) // 本行容纳不下本条目
		{
			if (pItem->CanSplit()) // 条目支持分割
			{
				int nParent = m_vSplitItems.size();
				XItem* pSrcItem = pItem;
				XItem* pItem1, *pItem2;
				int nSplitCount = 0;
				while (pItem->GetWidth() > nRemainWidth)
				{
					pItem->SplitItem(nRemainWidth, pItem1, pItem2);
					if (pItem1) // 如果pItem1==NULL,则说明上一行的剩余宽度没法容纳下本条目的任何部分，本条目直接加入下一行
					{
						pItem1->SetLinkStyle(XItem::lsLinkNext);
						pItem1->SetParent(nParent);
						pLine->AddItem(pItem1);
						pItem2->SetLinkStyle(XItem::lsLinkPrev);
						pItem2->SetParent(nParent);
						nSplitCount ++;
						if (nSplitCount > 1)
							pItem->Release();
					}
					else // 实际上pItem2等同于pItem的拷贝
					{
						//pItem->Release();
						//pSrcItem = pItem2;
					}
					pLine = NewLine(XLine::LeftAlign);
					nRemainWidth = GetMaxValidWidth() - pLine->GetWidth();
					pItem = pItem2;
				} // while
				pLine->AddItem(pItem);
				if (pItem->GetParent() != XItem::NotParent)
					m_vSplitItems.push_back(pSrcItem);
				else
				{
					if (nSplitCount > 0)
						pSrcItem->Release();
				}
			}
			else // 条目不支持分割,需要放到下一行
			{
				if (bNewLine) // 新建一行都容不下，这么大的"图片"!!!???
				{
					OutputDebugString("item cannot split and item is too large!\r\n");
					m_vLines.erase(m_vLines.end() - 1);	// 这里删除最后新建的一行
					return FALSE;
				}
				else // 因为当前最后一行放不下，加之又是图片不能分割，所以新建一行来放
				{
					pLine = NewLine(nAlign, nLineSpace);
					pLine->AddItem(pItem);
				}
			}
		}
		else // 能够容纳下(是否新建一行都有可能)
		{
			pLine->AddItem(pItem);
		}
	}
	else // 没有自动回行的处理
	{
		pLine->AddItem(pItem);
		int nLineWidth = pLine->GetWidth();
		if (nLineWidth > GetMaxValidWidth())
			SetMaxWidth(nLineWidth);
	}

	// 重置容器最大逻辑高度
	m_nMaxHeight = pLine->GetOffsetY() + pLine->GetHeight() + pLine->GetLineSpace();
	
	AdjustHScroll();
	AdjustVScroll();
	InvalidRect();
	return TRUE;
}
//------------------------------------------------------------------------
// 根据行对齐，得到每行相对左边界的相对距离(>0)
int XItemContainer::GetAlignOffset(XLine* pLine)
{
	int dx = 0;
	if (pLine->GetAlign() == XLine::RightAlign)
		dx = m_nMaxWidth - pLine->GetWidth();
	else if (pLine->GetAlign() == XLine::CenterAlign)
		dx = (m_nMaxWidth - pLine->GetWidth()) / 2;
	return dx;
}
//------------------------------------------------------------------------
void XItemContainer::OnDraw(ICanvas* pCanvas, RECT& rcClip)
{
	XLine* pLine = NULL;
	POINT ptOrigin; // 容器原点的绝对坐标
	ptOrigin.x = m_rcWnd.left + m_ptOffset.x + m_nMargin;
	ptOrigin.y = m_rcWnd.top + m_ptOffset.y + m_nMargin;
	int nLineCount = GetLineCount();
	RECT rc;
	CopyRect(&rc, &rcClip);
	InflateRect(&rc, -m_nMargin, -m_nMargin);
	for (int i=0; i<nLineCount; i++)
	{
		pLine = GetLine(i);
		ASSERT(pLine);
		if (ptOrigin.y + pLine->GetOffsetY() + pLine->GetHeight() < rc.top) // 前面不可见的行
			continue;
		
		// 可见的行
		try{
		pLine->OnDraw(pCanvas, ptOrigin.x + GetAlignOffset(pLine), 
			ptOrigin.y + pLine->GetOffsetY(), rc, m_nAlpha);}
		catch (...) {
			int j=0;
		}
		
		if (ptOrigin.y + pLine->GetOffsetY()/* + pLine->GetHeight()*/ >= rc.bottom) // 后面不可见的行
			break;
	}
}
//------------------------------------------------------------------------
DWORD XItemContainer::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{
	int nHitLine = -1;
	int nLineCount = GetLineCount();
	XLine* pLine = NULL;
	int yOff = pt.y - m_rcWnd.top - m_ptOffset.y - m_nMargin;
	for (int i=0; i<nLineCount; i++)
	{
		pLine = GetLine(i);
		if (yOff < pLine->GetOffsetY() + pLine->GetHeight())
		{
			nHitLine = i;
			break;
		}
	}

	int xOff = pt.x - m_rcWnd.left - m_ptOffset.x - m_nMargin;
	int nItem = GetItemByOffset(nHitLine, xOff);
	int nHover = MAKELONG(nItem, nHitLine);
	if (m_nMouseHover != nHover)
		OnItemChanged(nHitLine, nItem);
	
	return 0;
}
//------------------------------------------------------------------------
/*
DWORD XItemContainer::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	if (nButton == LeftButton)
	{
		int nLine = (short)HIWORD(m_nMouseHover);
		int nItem = (short)LOWORD(m_nMouseHover);
		if (nLine != -1 && nItem != -1)
		{
			XLine* pLine = GetLine(nLine);
			ASSERT(pLine);
			XItem* pItem = pLine->GetItem(nItem);
			ASSERT(pItem);
			if (pItem->GetParent() != XItem::NotParent) // 如果是被分割的条目，则调用父条目的鼠标事件
				m_vSplitItems[pItem->GetParent()]->OnMouseDown(nButton, nShift, pt);
			if (pItem && (pItem->GetLinkStyle() & XItem::lsNoLink))
				pItem->OnMouseDown(nButton, nShift, pt);
			else
			{
				XLine* pLine1 = pLine;
				XItem* pItem1 = pItem;
				int nLine1 = nLine;
				int nItem1 = nItem;
				while (pItem1->GetLinkStyle() & XItem::lsLinkPrev)
				{
					// 上一个
					if (nItem1 <= 0) // 转到上一行
					{
						pLine1 = GetLine(--nLine1);
						nItem1 = pLine1->GetItemCount() - 1;
					}
					else
						nItem1--;
					pItem1 = pLine1->GetItem(nItem1);
					pItem1->OnMouseDown(nButton, nShift, pt);
				}
				
				
				pLine1 = pLine;
				pItem1 = pItem;
				nLine1 = nLine;
				nItem1 = nItem;
				while (pItem1->GetLinkStyle() & XItem::lsLinkNext)
				{
					// 下一个
					if (nItem1 >= pLine1->GetItemCount()-1) // 转到下一行
					{
						pLine1 = GetLine(++nLine1);
						nItem1 = 0;
					}
					else
						nItem1++;
					pItem1 = pLine1->GetItem(nItem1);
					pItem1->OnMouseDown(nButton, nShift, pt);
				}
				pItem->OnMouseDown(nButton, nShift, pt);
			}
		}
	}
	
	return XWindow::OnMouseDown(nButton, nShift, pt);
}*/
//------------------------------------------------------------------------
DWORD XItemContainer::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	if (nButton == LeftButton || nButton == RightButton)
	{
		int nLine = (short)HIWORD(m_nMouseHover);
		int nItem = (short)LOWORD(m_nMouseHover);
		if (nLine >= 0 && nItem >= 0)
		{
			XLine* pLine = GetLine(nLine);
			if (pLine)
			{
				XItem* pItem = pLine->GetItem(nItem);
				if (pItem)
				{
					if (pItem->GetParent() != XItem::NotParent) // 如果是被分割的条目，则调用父条目的鼠标事件
					{
						XLine* pLine1 = pLine;
						XItem* pItem1 = pItem;
						int nLine1 = nLine;
						int nItem1 = nItem;
						int nLineCount = GetLineCount();

						while (pItem1->GetLinkStyle() & XItem::lsLinkPrev)
						{
							// 上一个
							if (nItem1 <= 0) // 转到上一行
							{
								if (--nLine1 < 0)
									break;
								pLine1 = GetLine(nLine1);
								nItem1 = pLine1->GetItemCount() - 1;
							}
							else
								nItem1--;
							pItem1 = pLine1->GetItem(nItem1);
							pItem1->OnMouseUp(nButton, nShift, pt);
						}
						
						
						pLine1 = pLine;
						pItem1 = pItem;
						nLine1 = nLine;
						nItem1 = nItem;
						while (pItem1->GetLinkStyle() & XItem::lsLinkNext)
						{
							// 下一个
							if (nItem1 >= pLine1->GetItemCount()-1) // 转到下一行
							{
								if (++nLine1 >= nLineCount)
									break;
								pLine1 = GetLine(nLine1);
								nItem1 = 0;
							}
							else
								nItem1++;
							pItem1 = pLine1->GetItem(nItem1);
							pItem1->OnMouseUp(nButton, nShift, pt);
						}

						m_vSplitItems[pItem->GetParent()]->OnMouseUp(nButton, nShift, pt);
						return XWindow::OnMouseUp(nButton, nShift, pt);
					}
					pItem->OnMouseUp(nButton, nShift, pt);					
				}
			}
		}
	}
	return XWindow::OnMouseUp(nButton, nShift, pt);
}
//------------------------------------------------------------------------
DWORD XItemContainer::OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt)
{
	if (m_pVScrollBar && m_pVScrollBar->IsEnabled())
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
DWORD XItemContainer::OnKeyDown(UINT nKeyCode, UINT nShift)
{
	if (m_pVScrollBar)
		return m_pVScrollBar->OnKeyDown(nKeyCode, nShift);
	return NOTHANDLE;
}
//------------------------------------------------------------------------
void XItemContainer::OnItemChanged(int nLine, int nItem)
{
	int nOldLine = (short)HIWORD(m_nMouseHover);
	int nOldItem = (short)LOWORD(m_nMouseHover);

	int nLineCount = GetLineCount();

	// 先前的item进行exit处理
	if (nOldLine >= 0 && nOldItem >= 0)
	{
		XLine* pLine = GetLine(nOldLine);
		if (pLine)
		{
			XItem* pItem = pLine->GetItem(nOldItem);
			if (pItem)
			{
				if (pItem->GetLinkStyle() == XItem::lsNoLink)
					pItem->OnMouseExit();
				else
				{
					XLine* pLine1 = pLine;
					XItem* pItem1 = pItem;
					int nLine1 = nOldLine;
					int nItem1 = nOldItem;
					while (pItem1->GetLinkStyle() & XItem::lsLinkPrev)
					{
						// 上一个
						if (nItem1 <= 0) // 转到上一行
						{
							if (--nLine1 < 0)
								break;
							pLine1 = GetLine(nLine1);
							nItem1 = pLine1->GetItemCount() - 1;
						}
						else
							nItem1--;
						pItem1 = pLine1->GetItem(nItem1);
						pItem1->OnMouseExit();
					}

					pItem->OnMouseExit();
					
					pLine1 = pLine;
					pItem1 = pItem;
					nLine1 = nOldLine;
					nItem1 = nOldItem;
					while (pItem1->GetLinkStyle() & XItem::lsLinkNext)
					{
						// 下一个
						if (nItem1 >= pLine1->GetItemCount()-1) // 转到下一行
						{
							if (++nLine1 >= nLineCount)
								break;
							pLine1 = GetLine(nLine1);
							nItem1 = 0;
						}
						else
							nItem1++;
						pItem1 = pLine1->GetItem(nItem1);
						pItem1->OnMouseExit();
					}
				}
			}
		}
	}

	if (nLine <= -1 || nItem <= -1)
	{
		SetCursor(CT_ARROW);
		m_nMouseHover = MAKELONG(nItem, nLine);
		return;
	}

	// 现在的item进行enter处理
	if (nLine >= 0 && nItem >= 0)
	{
		XLine* pLine = GetLine(nLine);
		if (pLine)
		{
			XItem* pItem = pLine->GetItem(nItem);
			if (pItem)
			{
				if (pItem->GetLinkStyle() == XItem::lsNoLink)
				{
					pItem->OnMouseEnter();
					SetCursor(pItem->GetCursorType());
				}
				else
				{
					XLine* pLine1 = pLine;
					XItem* pItem1 = pItem;
					int nLine1 = nLine;
					int nItem1 = nItem;
					while (pItem1->GetLinkStyle() & XItem::lsLinkPrev)
					{
						// 上一个
						if (nItem1 <= 0) // 转到上一行
						{
							if (--nLine1 < 0)
								break;
							pLine1 = GetLine(nLine1);
							nItem1 = pLine1->GetItemCount() - 1;
						}
						else
							nItem1--;
						pItem1 = pLine1->GetItem(nItem1);
						pItem1->OnMouseEnter();
					}
					
					pItem->OnMouseEnter();
					SetCursor(pItem->GetCursorType());
					
					pLine1 = pLine;
					pItem1 = pItem;
					nLine1 = nLine;
					nItem1 = nItem;
					while (pItem1->GetLinkStyle() & XItem::lsLinkNext)
					{
						// 下一个
						if (nItem1 >= pLine1->GetItemCount()-1) // 转到下一行
						{
							if (++nLine1 >= nLineCount)
								break;
							pLine1 = GetLine(nLine1);
							nItem1 = 0;
						}
						else
							nItem1++;
						pItem1 = pLine1->GetItem(nItem1);
						pItem1->OnMouseEnter();
					}
				}
			}
		}
	}
	
	m_nMouseHover = MAKELONG(nItem, nLine);
}
//------------------------------------------------------------------------
// 如果pItem==NULL，则将pLine整个一行设为脏
void XItemContainer::SetDirty(XLine* pLine, XItem* pItem)
{
	if (!pLine)
		return;

	RECT rc, rcClip;
	if (pItem)
	{
		rc.left = pItem->GetOffsetX();
		rc.right = rc.left + pItem->GetWidth();
		rc.top = pLine->GetOffsetY() + pLine->GetHeight() - pItem->GetHeight();
		rc.bottom = rc.top + pItem->GetHeight();
	}
	else
	{
		rc.left = 0;
		rc.right = rc.left + pLine->GetWidth();
		rc.top = pLine->GetOffsetY();
		rc.bottom = rc.top + pLine->GetHeight();
	}

	int dx = m_rcWnd.left + m_ptOffset.x + m_nMargin + GetAlignOffset(pLine);
	int dy = m_rcWnd.top + m_ptOffset.y + m_nMargin;
	OffsetRect(&rc, dx, dy);
	CopyRect(&rcClip, &m_rcWnd);
	InflateRect(&rcClip, -m_nMargin, -m_nMargin);
	IntersectRect(&rcClip, &rcClip, &rc);
	InvalidRect(&rcClip);
}
//------------------------------------------------------------------------
void XItemContainer::SetHScrollBar(XScrollBar* pBar)
{
	m_pHScrollBar = pBar;
	if (m_pHScrollBar)
	{
		m_pHScrollBar = static_cast<XScrollBar*>(pBar);
		CONN_posChanged(m_pHScrollBar, XItemContainer, OnHScrollPosChanged);
		m_pHScrollBar->Refresh();
	}
}
//------------------------------------------------------------------------
void XItemContainer::SetVScrollBar(XScrollBar* pBar)
{
	m_pVScrollBar = pBar;
	if (m_pVScrollBar)
	{
		m_pVScrollBar = static_cast<XScrollBar*>(pBar);
		CONN_posChanged(m_pVScrollBar, XItemContainer, OnVScrollPosChanged);
		m_pVScrollBar->Refresh();
	}
}
//------------------------------------------------------------------------
void XItemContainer::OnHScrollPosChanged(DWORD dwParam)
{
	m_ptOffset.x = -*(int*)dwParam;
	InvalidRect();
}
//------------------------------------------------------------------------
void XItemContainer::OnVScrollPosChanged(DWORD dwParam)
{
	m_ptOffset.y = -*(int*)dwParam;
	InvalidRect();
}
//------------------------------------------------------------------------
void XItemContainer::AdjustHScroll()
{
	if (m_pHScrollBar)
	{
		int nLimitedWidth = GetWidth() - 2*GetMargin();
		
		// 最长一行的x向偏移
		int nNeedScrollValue = GetMaxValidWidth() - nLimitedWidth;
		if (nNeedScrollValue > 0)
		{
			//m_pHScrollBar->SetEnabled(TRUE);
			m_pHScrollBar->SetVisible(TRUE);
			m_pHScrollBar->SetVisibleCount(nLimitedWidth);
			m_pHScrollBar->SetScrollRange(0, nNeedScrollValue, TRUE);
			m_pHScrollBar->Refresh();
		}
		else
		{
			//m_pHScrollBar->SetEnabled(FALSE);
			m_pHScrollBar->SetVisible(FALSE);
			m_pHScrollBar->SetScrollRange(0, 0);
			m_pHScrollBar->SetVisibleCount(0);
			m_pHScrollBar->SetScrollPos(0);
			m_pHScrollBar->Refresh();
		}
	}
}
//------------------------------------------------------------------------
void XItemContainer::AdjustVScroll()
{
	if (GetLineCount() <= 0)
	{
		if (m_pVScrollBar)
		{
			m_pVScrollBar->SetVisible(FALSE);
			m_pVScrollBar->SetScrollRange(0, 0);
			m_pVScrollBar->SetVisibleCount(0);
			m_pVScrollBar->SetScrollPos(0);
			m_pVScrollBar->Refresh();
		}
		m_ptOffset.y = 0;
		return;
	}
	else
	{
		int nLimitedHeight = GetHeight() - 2*GetMargin();
		// 最后一行的y向偏移
		XLine* pLastLine = GetLine(GetLineCount() - 1);
		int nOffy = pLastLine->GetOffsetY() + pLastLine->GetHeight() + pLastLine->GetLineSpace();
		int nNeedScrollValue = nOffy - nLimitedHeight;
		if (m_pVScrollBar)
		{
			if (nNeedScrollValue > 0)
			{
				m_pVScrollBar->SetVisible(TRUE);
				m_pVScrollBar->SetVisibleCount(nLimitedHeight);
				m_pVScrollBar->SetScrollRange(0, nNeedScrollValue, TRUE);
				m_pVScrollBar->Refresh();
			}
			else // 容器里面还没有填满有效区域
			{
				m_pVScrollBar->SetVisible(FALSE);
				m_ptOffset.y = 0;
			}
		}
		if (-m_ptOffset.y > nNeedScrollValue && nNeedScrollValue > 0)
		{
			if (m_pVScrollBar)
				m_pVScrollBar->SetScrollPos(nNeedScrollValue);
			else
				OnVScrollPosChanged((DWORD)&nNeedScrollValue);
		}
	}
}
//------------------------------------------------------------------------
void XItemContainer::SetWindowPos(RECT& rc)
{
	XWindow::SetWindowPos(rc);
	SetMaxWidth(rc.right - rc.left - 2*m_nMargin);
	SetMaxHeight(rc.bottom - rc.top - 2*m_nMargin);
	AdjustHScroll();
	AdjustVScroll();
}
//------------------------------------------------------------------------
void GetColorString(char* color_buf, WPixel color);
BOOL XItemContainer::SaveToHtml(LPCSTR szFileName, LPCSTR szTitle, WPixel backcolor)
{
	char buf_head[] = 
		"<html>\r\n\r\n"
		"<head>\r\n"
		"<meta http-equiv=\"Content-Language\" content=\"zh-cn\">\r\n"
		"<meta name=\"GENERATOR\" content=\"Microsoft FrontPage 5.0\">\r\n<meta name=\"ProgId\" content=\"FrontPage.Editor.Document\">\r\n"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\r\n";
	char buf_head1[512];
	char color[8];
	GetColorString(color, backcolor);
	wsprintf(buf_head1, "<title>%s</title>\r\n</head>\r\n\r\n<body bgcolor=\"#%s\">\r\n",
		((szTitle)?szTitle:"聊天记录"), color);
	char buf_trail[] = "\r\n</body>\r\n\r\n</html>";

	if (!szFileName || *szFileName == '\0')
		return FALSE;
	FILE* hFile = fopen(szFileName, "w");
	if (!hFile)
		return FALSE;
	fwrite(buf_head, sizeof(buf_head), 1, hFile);
	fwrite(buf_head1, strlen(buf_head1), 1, hFile);

	int nLineCount = GetLineCount();
	for (int i=0; i<nLineCount; i++)
	{
		XLine* pLine = GetLine(i);
		if (pLine)
			pLine->SaveToHtml(hFile);
	}
	
	fwrite(buf_trail, sizeof(buf_trail), 1, hFile);
	fclose(hFile);
	return TRUE;
}
//------------------------------------------------------------------------