/**
*	created:		2012-6-24   0:20
*	filename: 		XItem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/FKWndSysCommon.h"
//------------------------------------------------------------------------
void GetColorString(char* color_buf, WPixel color)
{
	DWORD dwColor = color;
	wsprintf(color_buf, "%02X%02X%02X", (dwColor>>16)&0xFF, (dwColor>>8)&0xFF, dwColor&0xFF);
}
//------------------------------------------------------------------------
XItem::XItem(XItem* pItem) :
m_nLinkStyle(pItem->m_nLinkStyle), m_nOffsetX(pItem->m_nOffsetX), m_nWidth(pItem->m_nWidth),
m_nHeight(pItem->m_nHeight), m_nParent(pItem->m_nParent), m_pContainer(pItem->m_pContainer)
{
}
//------------------------------------------------------------------------
// 文本条目
XItemText::XItemText(XItemText* pItem) : XItem(pItem),
m_clrText(pItem->m_clrText), m_strText(pItem->m_strText), m_pFont(pItem->m_pFont)
{
}
//------------------------------------------------------------------------
void XItemText::OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha)
{
	ASSERT(IsValid());
	pCanvas->DrawClipText(m_pFont, x, y, m_strText.c_str(), m_clrText, rcClip, FONT_NOALPHA);
}
//------------------------------------------------------------------------
// 计算文字所占的区域大小
void XItemText::CalcSize()
{
	if (IsValid())
	{
		m_nWidth = m_pFont->GetLength(m_strText.c_str());
		m_nHeight = m_pFont->GetHeight();
	}
}
//------------------------------------------------------------------------
int XItemText::CalcSplitPos(int nFromPos, int xDisc)
{
	ASSERT(IsValid());
	if (xDisc == -1)
		return m_strText.length();
    int nNewPos = nFromPos;
	int curDisc = 0;	// 当前累计距离
	WORD c;				// 当前操作字符
	int strlen = GetTextLength();
	int i = nFromPos;
	bool isDblByteChar;
	int charwidth;
	while (i<strlen && (BYTE)m_strText.at(i))
	{
		isDblByteChar = false;
		c = (BYTE)m_strText.at(i++);
		if (c > 0x80)	// 双字节文字
		{
			c = c*256 + (BYTE)m_strText.at(i++);
			isDblByteChar = true;
		}
		
		charwidth = m_pFont->GetCharLength(c); 
		curDisc += charwidth;
		if (curDisc <= xDisc)
		{
			nNewPos ++;
			if (isDblByteChar)
				nNewPos ++;
		}
		else
			break;
	}
	return nNewPos;
}
//------------------------------------------------------------------------
// 分割一个条目为两个子条目
void XItemText::SplitItem(int nLimitedWidth, XItem*& pItem1, XItem*& pItem2)
{
	int nSplitPos = CalcSplitPos(0, nLimitedWidth);
	if (nSplitPos > 0) // 如果nLimitedWidth小于一个字符的宽度，则nSplitPos=0
	{
		XItemText* pSubItem1 = new XItemText(this);
		XItemText* pSubItem2 = new XItemText(this);
		
		string substr1 = m_strText.substr(0, nSplitPos);
		string substr2 = m_strText.substr(nSplitPos, m_strText.length()-nSplitPos);
		pSubItem1->SetText(substr1.c_str());
		pSubItem1->CalcSize();
		pSubItem2->SetText(substr2.c_str());
		pSubItem2->CalcSize();
		pItem1 = static_cast<XItem*>(pSubItem1);
		pItem2 = static_cast<XItem*>(pSubItem2);	
	}
	else
	{
		pItem1 = NULL;
		pItem2 = static_cast<XItem*>(this);
	}
}
//------------------------------------------------------------------------
BOOL XItemText::SaveToHtml(FILE* hFile)
{
	XFontCreate::stFontInfo* info = XFontCreate::GetInstance()->GetFontInfo(m_pFont);
	if (!info) return FALSE;
	static int __size[] = 
	{
		0,1,1,1,1,1,1,1,1,1, // 0~9
		2,2, // 10,11
		3,3, // 12,13
		4,4,4,4, // 14~17
		5,5,5,5,5,5, // 18~23
		6,6,6,6,6,6,6,6,6,6,6,6, // 24~35
		7, // 36
	};
	int nSize = info->nSize;
	if (nSize > 36) nSize = 36;
	nSize = __size[nSize]-1;
	char buf_begin[256], buf_color[256];
	GetColorString(buf_color, m_clrText);
	wsprintf(buf_begin, "<font face=\"%s\" size=\"%d\" color=\"#%s\">", 
		info->szName, nSize, buf_color);
	
	fwrite(buf_begin, strlen(buf_begin), 1, hFile);
	if (info->bBlod) fwrite("<b>", 3, 1, hFile);
	if (info->bItalic) fwrite("<i>", 3, 1, hFile);
	if (info->bUnderline) fwrite("<u>", 3, 1, hFile);

	fwrite(m_strText.c_str(), m_strText.length(), 1, hFile);

	if (info->bUnderline) fwrite("</u>", 3, 1, hFile);
	if (info->bItalic) fwrite("</i>", 3, 1, hFile);
	if (info->bBlod) fwrite("</b>", 3, 1, hFile);
	fwrite("</font>", 7, 1, hFile);
	return TRUE;
}
//------------------------------------------------------------------------
// 带链接的文字
XItemLinkText::XItemLinkText(XItemLinkText* pItem) : XItemText(pItem),
m_strLinkText(pItem->m_strLinkText), m_clrNotVisited(pItem->m_clrNotVisited), 
m_clrVisited(pItem->m_clrVisited), m_clrRollOver(pItem->m_clrRollOver), 
m_bVisited(pItem->m_bVisited), m_bRollOver(pItem->m_bRollOver)
{
}

void XItemLinkText::OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha)
{
	if (x > rcClip.right || x + m_nWidth < rcClip.left)
		return;
	ASSERT(IsValid());
	pCanvas->DrawClipText(m_pFont, x, y, m_strText.c_str(), m_clrText, rcClip, FONT_NOALPHA);
	// 画下划线
	//if (m_bRollOver)
	/*{
		POINT ptFrom, ptTo;
		ptFrom.x = x;
		ptTo.x = x + m_nWidth;
		ptFrom.y = ptTo.y = y + m_nHeight;
		if (ptFrom.x < rcClip.left)
			ptFrom.x = rcClip.left;
		if (ptTo.x > rcClip.right)
			ptTo.x = rcClip.right;
		pCanvas->DrawLine(ptFrom, ptTo, m_clrText, BLIT_NORMAL, alpha);
	}*/
}
//------------------------------------------------------------------------
// 分割一个条目为两个子条目
void XItemLinkText::SplitItem(int nLimitedWidth, XItem*& pItem1, XItem*& pItem2)
{
	int nSplitPos = CalcSplitPos(0, nLimitedWidth);
	if (nSplitPos > 0) // 如果nLimitedWidth小于一个字符的宽度，则nSplitPos=0
	{
		XItemLinkText* pSubItem1 = new XItemLinkText(this);
		XItemLinkText* pSubItem2 = new XItemLinkText(this);
		
		string substr1 = m_strText.substr(0, nSplitPos);
		string substr2 = m_strText.substr(nSplitPos, m_strText.length()-nSplitPos);
		pSubItem1->SetText(substr1.c_str());
		pSubItem1->CalcSize();
		pSubItem2->SetText(substr2.c_str());
		pSubItem2->CalcSize();
		pItem1 = static_cast<XItem*>(pSubItem1);
		pItem2 = static_cast<XItem*>(pSubItem2);
	}
	else
	{
		pItem1 = NULL;
		pItem2 = static_cast<XItem*>(this);
	}
}
//------------------------------------------------------------------------
void XItemLinkText::OnMouseEnter()
{
	SetTextColor(m_clrRollOver);
	m_bRollOver = TRUE;
	SetDirty();
}
//------------------------------------------------------------------------
void XItemLinkText::OnMouseExit()
{
	if (IsVisited())
		SetTextColor(m_clrVisited);
	else
		SetTextColor(m_clrNotVisited);
	m_bRollOver = FALSE;
	SetDirty();
}
//------------------------------------------------------------------------
DWORD XItemLinkText::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{
	if (nButton == LeftButton)
	{
		SetVisited(TRUE);
		if (m_nParent == XItem::NotParent)
			Emit();
	}	
	return 0;
}
//------------------------------------------------------------------------
BOOL XItemLinkText::SaveToHtml(FILE* hFile)
{
	return XItemText::SaveToHtml(hFile);
}
//------------------------------------------------------------------------
BOOL XItemBookmarkText::SaveToHtml(FILE* hFile)
{
	return XItemText::SaveToHtml(hFile);
}
//------------------------------------------------------------------------
// 图片条目
void XItemImage::OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha)
{
	ASSERT(IsValid());
	DWORD style = BLIT_MASK;
	if (alpha < 256)
	{
		m_pImage->SetUserData(alpha);
		style |= BLIT_TRANS;
	}

	int w = GetWidth();
	int h = GetHeight();
	int sx = 0;
	int sy = 0;
	if (x < rcClip.left)
	{
		sx = rcClip.left - x;
		x = rcClip.left;
		w -= sx;
	}
	if (x + w > rcClip.right)
	{
		w = rcClip.right - x;
	}
	if (y < rcClip.top)
	{
		sy = rcClip.top - y;
		y = rcClip.top;
		h -= sy;
	} 
	if (y + h > rcClip.bottom)
	{
		h = rcClip.bottom - y;
	}	
	m_pImage->Draw(pCanvas, x, y, w, h, m_nCurFrame*GetWidth() + sx, sy, style);
}
//------------------------------------------------------------------------
void XItemImage::OnTimer(DWORD dwEventID)
{
	if (++m_nCurFrame >= m_nFrameCount)
		m_nCurFrame = 0;
	SetDirty();
}
//------------------------------------------------------------------------
void XItemImage::SetFrameCount(int nCount)
{
	m_nFrameCount = nCount;
	if (m_nFrameCount > 1)
		XDesktop::GetInstance()->GetTimeAxis()->SetTimer(0, static_cast<ITimerSink*>(this), m_nDelay);
}
//------------------------------------------------------------------------
void XItemImage::Release()
{
	if (m_nFrameCount > 1)
		XDesktop::GetInstance()->GetTimeAxis()->KillTimer(0, static_cast<ITimerSink*>(this));
	//SAFE_RELEASE_IMAGE(m_pImage); // 这里先注释，防止频繁发送表情符号，导致死机
	delete this;
}
//------------------------------------------------------------------------
BOOL XItemImage::SaveToHtml(FILE* hFile)
{
	return TRUE;
}
//------------------------------------------------------------------------
DWORD XItemLinkImage::OnMouseUp(UINT nButton, UINT nShift, POINT pt)
{	
	if (nButton == LeftButton
		&& m_nParent == XItem::NotParent)
		Emit();
	return 0;
}
//------------------------------------------------------------------------
BOOL XItemLinkImage::SaveToHtml(FILE* hFile)
{
	return TRUE;
}
//------------------------------------------------------------------------
// 一行条目管理
void XLine::OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip, int alpha)
{
	if (IsEmpty())
		return;
	
	int curX = x;
	int curY = y;

	RECT rcLine;
	SetRect(&rcLine, x, y, x + m_nWidth, y + m_nHeight);
	// 裁减
	RECT rc;
	IntersectRect(&rc, &rcLine, &rcClip);

	if (!IsRectEmpty(&rc))
	{
		// 计算对齐
		//...
		int nItemCount = GetItemCount();
		//int nStartX, nEndX;
		for (int i=0; i<nItemCount; i++)
		{
			XItem* pItem = GetItem(i);
			ASSERT(pItem);
			curY = y + m_nHeight - pItem->GetHeight();
			pItem->OnDraw(pCanvas, curX, curY, rc, alpha);
			curX += pItem->GetWidth();
		}
	}
}
//------------------------------------------------------------------------
// 在本行中添加一个条目(不进行条目是否能容纳下的检测)
void XLine::AddItem(XItem* pItem)
{
	m_vItems.push_back(pItem);
	pItem->SetContainer(static_cast<IItemContainer*>(this));
	pItem->SetOffsetX(GetWidth());
	// 更新当前所有条目的宽度
	SetWidth(GetWidth() + pItem->GetWidth());
	// 更新最大高度
	if (pItem->GetHeight() > m_nHeight)
	{
		SetHeight(pItem->GetHeight());
	}
}
//------------------------------------------------------------------------
void XLine::SetDirty(XItem* pItem)
{
	if (m_pContainer)
		m_pContainer->SetDirty(static_cast<XLine*>(this), pItem);
}
//------------------------------------------------------------------------
void XLine::Clear()
{
	if (!m_vItems.empty())
	{
		ItemsPtr _end = m_vItems.end();
		for (ItemsPtr it=m_vItems.begin(); it!=_end; ++it)
			(*it)->Release();
		m_vItems.clear();
	}
}
//------------------------------------------------------------------------
BOOL XLine::SaveToHtml(FILE* hFile)
{
	char align[3][8] = {"left","center","right"};
	char buf[256];
	wsprintf(buf, "\r\n<p align=\"%s\">", &align[m_nAlign][0]);
	fwrite(buf, strlen(buf), 1, hFile);

	int nItemCount = GetItemCount();
	for (int i=0; i<nItemCount; i++)
	{
		XItem* pItem = GetItem(i);
		if (pItem)
			pItem->SaveToHtml(hFile);
	}

	fwrite("</p>", 4, 1, hFile);
	return TRUE;
}
//------------------------------------------------------------------------