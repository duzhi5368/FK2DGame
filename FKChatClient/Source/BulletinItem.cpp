/**
*	created:		2012-6-28   2:51
*	filename: 		BulletinItem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		维护公告项
*/
//------------------------------------------------------------------------
#include "../Include/BulletinItem.h"
//------------------------------------------------------------------------
// 两个公告之间的间隔
const int CBulletinItem::ITEM_SPACE = 20;
//------------------------------------------------------------------------
CBulletinItem::CBulletinItem(void)
: m_pFont(NULL)
, m_clrText(WPixel(255,255,0))
, m_nLength(0)
, m_bClicked(FALSE)
, m_nScrollCount(1)
, m_nScrolledCount(0)
, m_nState(STATE_NORMAL)
{
}
//------------------------------------------------------------------------
CBulletinItem::~CBulletinItem(void)
{
}
//-------------------------------------------------------------------
// 描  述：设置绘制时使用的字体 
//-------------------------------------------------------------------
void CBulletinItem::SetFont(ITTFont * pFont)
{
	m_pFont = pFont;
}

//-------------------------------------------------------------------
// 描  述：设置公告的文本 
//-------------------------------------------------------------------
void CBulletinItem::SetText(string strText)
{
	m_strText = strText;

	if (m_pFont != NULL)
	{
		m_nLength = m_pFont->GetLength(strText.c_str()) + ITEM_SPACE;
	}
}

void CBulletinItem::SetTextColor(WPixel clrText)
{
	m_clrText = clrText;
}

//-------------------------------------------------------------------
// 描  述：设置链接 
//-------------------------------------------------------------------
void CBulletinItem::SetUrl(string strUrl)
{
	m_strUrl = strUrl;
}

//-------------------------------------------------------------------
// 描  述：设置滚动的次数 
//-------------------------------------------------------------------
void CBulletinItem::SetScrollCount(int nCount)
{
	m_nScrollCount = nCount;
}

//-------------------------------------------------------------------
// 描  述：获取公告的长度 
//-------------------------------------------------------------------
int CBulletinItem::GetLength(void)
{
	return m_nLength;
}

//-------------------------------------------------------------------
// 描  述：是否完成了指定次数的滚动
//-------------------------------------------------------------------
BOOL CBulletinItem::IsFinished(void)
{
	return (m_nScrolledCount >= m_nScrollCount);
}

//-------------------------------------------------------------------
// 描  述：鼠标移到上面 
//-------------------------------------------------------------------
void CBulletinItem::MouseOver(BOOL bOver)
{
	if (bOver)
	{
		m_nState = STATE_MOUSEOVER;
	}
	else
	{
		if (m_bClicked)
		{
			m_nState = STATE_CLICKED;
		}
		else
		{
			m_nState = STATE_NORMAL;
		}
	}
}

//-------------------------------------------------------------------
// 描  述：点击这个条目 
//-------------------------------------------------------------------
void CBulletinItem::Click(void)
{
	m_bClicked = TRUE;
	m_nState = STATE_CLICKED;

	if (!(m_strUrl.empty()))
	{
		::ShellExecute(NULL, "open", m_strUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

//-------------------------------------------------------------------
// 描  述：完成一次滚动
//-------------------------------------------------------------------
void CBulletinItem::Scrolled(void)
{
	m_nScrolledCount++;
}

//-------------------------------------------------------------------
// 描  述：绘制自己
//-------------------------------------------------------------------
void CBulletinItem::OnDraw(ICanvas* pCanvas, int x, int y, RECT& rcClip)
{
	if (pCanvas == NULL)
	{
		return;
	}

	switch (m_nState)
	{
	case STATE_NORMAL:
		pCanvas->DrawClipText(m_pFont, x, y, m_strText.c_str(), m_clrText, rcClip, FONT_NOALPHA);
		break;

	case STATE_MOUSEOVER:
		pCanvas->DrawClipText(m_pFont, x, y, m_strText.c_str(), COLOR_MOUSEOVER, rcClip, FONT_NOALPHA);
		break;

	case STATE_CLICKED:
		pCanvas->DrawClipText(m_pFont, x, y, m_strText.c_str(), COLOR_CLICKED, rcClip, FONT_NOALPHA);
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------