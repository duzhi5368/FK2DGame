/**
*	created:		2012-6-28   10:32
*	filename: 		XListBoxEx
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/XListBoxEx.h"
#include "../Include/ChatClient.h"
//------------------------------------------------------------------------
XListBoxEx::XListBoxEx()
:m_nVisibleCount(0)
,m_nVisibleStartIndex(0)
,m_nSel(-1)
,m_nItemHeight(17)
,m_clrSelected(255,0,0)
{

}
//------------------------------------------------------------------------
XListBoxEx::~XListBoxEx()
{

}
//------------------------------------------------------------------------
BOOL XListBoxEx::Create(XWindow *pParent,IResObject *pRes,int nWndId)
{
	if (pParent == NULL || pRes == NULL)
	{
		return FALSE;
	}

	// 创建QQ控件主界面
	SetParent(pParent);
	g_wndHelper.CreateStockProperty(this, pRes, nWndId);
	pParent->AddWindow(this);

	m_nVisibleCount = (m_rcWnd.bottom-m_rcWnd.top-4)/m_nItemHeight;
	if( m_nVisibleCount <0 )
	{
		m_nVisibleCount = 0;
	}
	return TRUE;
}
//------------------------------------------------------------------------
DWORD XListBoxEx::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{
	// 计算选中的项
	int m_nOldSel = m_nSel;
	int y = pt.y - m_rcWnd.top-2;
	m_nSel = y/m_nItemHeight+m_nVisibleStartIndex;
	if( m_nSel <0  )
	{
		m_nSel = 0;
	}
	if( m_nSel >= m_ItemList.size())
	{
		m_nSel = m_ItemList.size()-1;
	}
	ItemListItr Itr = LocateItem(m_nSel);
	if( Itr == m_ItemList.end() )
	{
		m_nSel = -1;
	}
	if( m_nOldSel!= m_nSel)
	{
		OnSelChanage((DWORD)&m_nSel);
	}

	return XWindow::OnMouseDown(nButton,nShift,pt);
}
//------------------------------------------------------------------------
void XListBoxEx::OnDraw(ICanvas *pCanvas,RECT& rcClip)
{  
	ItemListItr itr = LocateItem(m_nVisibleStartIndex);
	int i = 0;
	RECT rc;
	rc.right = m_rcWnd.right;

	for(;itr!=m_ItemList.end() && i<m_nVisibleCount;i++,itr++)
	{

		// 绘制文字

		int x = m_rcWnd.left + 2;
		int y = m_rcWnd.top + i * m_nItemHeight + 2;				

		rc.left = x;
		rc.top = y-2;		
		rc.bottom = rc.top + m_nItemHeight;

		// 画出选中标志
		if (i+m_nVisibleStartIndex == m_nSel)
		{
			pCanvas->DrawBox(rc, m_clrSelected, BLIT_NORMAL, 255);
		}

		// 画出名字
		pCanvas->DrawClipText(GetFont(), x,y, 
			(*itr).str.c_str(), (*itr).wColor, rc, FONT_NOALPHA);
	}
}
//------------------------------------------------------------------------
void XListBoxEx::AddItem(LPCSTR lpStr,WPixel wColor,DWORD data)
{
	SListItem item;
	item.str = lpStr;
	item.wColor = wColor;
	item.dwData = data;
	m_ItemList.push_back(item);
}
//------------------------------------------------------------------------
void XListBoxEx::SetVisibleStart(int nIndex)
{
	m_nVisibleStartIndex = nIndex;
}
//------------------------------------------------------------------------
void XListBoxEx::SetVisibleCount(int nCount)
{
	m_nVisibleCount = nCount;
}
//------------------------------------------------------------------------
int XListBoxEx::GetSel()
{
	return m_nSel;
}
//------------------------------------------------------------------------
void XListBoxEx::OnSelChanage(DWORD dwParam)
{
	s_selChanged.emit(dwParam);
}
//------------------------------------------------------------------------
int  XListBoxEx::GetVisibleCount()
{
	return m_nVisibleCount;
}
//------------------------------------------------------------------------
void XListBoxEx::Clear()
{
	m_nVisibleStartIndex = 0;
	m_nSel = -1;
	m_ItemList.clear();
}
//------------------------------------------------------------------------
XListBoxEx::ItemListItr XListBoxEx::LocateItem(int index)
{
	ItemListItr itr = m_ItemList.begin();
	while (itr != m_ItemList.end())
	{
		if (index <= 0)
		{
			break;
		}
		itr++;
		index--;
	}
	return itr;  
}
//------------------------------------------------------------------------
BOOL XListBoxEx::IsTransparent(POINT& pt)
{
	return !PtInRect(&m_rcWnd,pt);
}
//------------------------------------------------------------------------
void XListBoxEx::ScrollUp()
{
	if (m_nVisibleStartIndex >= (m_ItemList.size()-m_nVisibleCount)) 
	{
		return ;
	}
	m_nVisibleStartIndex++;

}
//------------------------------------------------------------------------
void XListBoxEx::ScrollDown()
{
	if (m_nVisibleStartIndex <=0 ) 
	{
		return ;
	}
	m_nVisibleStartIndex--;
}
//------------------------------------------------------------------------
int  XListBoxEx::GetSize()
{
	return m_ItemList.size();
}
//------------------------------------------------------------------------
DWORD XListBoxEx::GetItemData(int Index)
{
	ItemListItr Itr = LocateItem(Index);
	return (*Itr).dwData;
}
//------------------------------------------------------------------------
void XListBoxEx::RemoveItem(int nIndex)
{
	ItemListItr Itr = LocateItem(nIndex);
	if( Itr != m_ItemList.end())
	{
		m_ItemList.erase(Itr);
	}
}
//------------------------------------------------------------------------
DWORD XListBoxEx::OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt)
{
	if (zDelta < 0)
	{
		int nDistance = -zDelta;
		for(int i = 0; i < nDistance; i++)
		{
			ScrollUp();
		}
	}
	else
	{
		int nDistance = zDelta;
		for(int i = 0;i < nDistance; i++)
		{
			ScrollDown();
		}
	}
	return XWindow::OnMouseWheel(nButton, nShift, zDelta, pt);	
}
//------------------------------------------------------------------------