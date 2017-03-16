/**
*	created:		2012-6-28   7:16
*	filename: 		ChatMenu
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/ChatMenu.h"
#include "../Include/WindowCreator.h"
#include "../../FKResManager/IFKResManager.h"
//------------------------------------------------------------------------
CChatMenuItem::CChatMenuItem(int nId, string strName,BOOL bEnabled)
: m_nId(nId)
, m_strName(strName)
, m_bEnabled(bEnabled)
{

}
//------------------------------------------------------------------------
CChatMenu::CChatMenu(void)
: m_pFrame(NULL)
, m_pEventReceiver(NULL)
, m_nSelected(-1)
{
	m_clrSelected = WPixel(100, 100, 0);
	m_clrText = WPixel(255, 245, 200);
}
//------------------------------------------------------------------------
CChatMenu::~CChatMenu(void)
{
	RemoveAll();
}

//-------------------------------------------------------------------
// 描  述：创建菜单窗口 
//-------------------------------------------------------------------
BOOL CChatMenu::Create(IResObject * pResObject)
{
	int i = 0;

	if (pResObject == NULL)
	{
		return FALSE;
	}

	// 创建菜单的主窗口
	if (!CWindowCreator::CreateMainWnd(NULL, this, pResObject))
	{
		return FALSE;
	}

	// 创建窗口的框架
	m_pFrame = new XEdit();
	if (!CWindowCreator::CreateControl(this, m_pFrame, pResObject->GetSubObjectByIndex(i++)))
	{
		return FALSE;
	}
	m_pFrame->SetWindowPos(m_rcWnd);

	SetVisible(FALSE);	
	return TRUE;
}
//------------------------------------------------------------------------
void CChatMenu::OnActivate(BOOL bActive)
{
	if (!bActive)
	{
		SetVisible(FALSE);		
	}
	else
	{
		SetVisible(TRUE);		
	}
}

//-------------------------------------------------------------------
// 描  述：画出菜单 
//-------------------------------------------------------------------
void CChatMenu::DrawMenu(ICanvas * pCanvas, RECT & rcClip)
{
	RECT rc;
	int x = m_rcWnd.left;
	int y = m_rcWnd.top;

	CopyRect(&rc, &m_rcWnd);
	rc.left += 2;
	rc.right -= 2;

	// 画出各个菜单项
	MenuItemListItr itr = m_items.begin();
	for (int i = 0; itr != m_items.end(); itr++, i++)
	{
		y = m_rcWnd.top + MENU_ITEM_HEIGHT * i;

		rc.top = y + 2;
		rc.bottom = rc.top + MENU_ITEM_HEIGHT;

		// 画出选中标志
		if (i == m_nSelected)
		{
			pCanvas->DrawBox(rc, m_clrSelected);
		}

		// 获取字符串的长度
		ITTFont * pFont = GetFont();
		if (pFont == NULL)
		{
			return;
		}				
		int nLen = pFont->GetLength((*itr)->m_strName.c_str());

		// 画出菜单项
		int nOffset = (rc.right - rc.left - nLen) / 2;
		// 如果激活
		if ((*itr)->m_bEnabled)
		{
			pCanvas->DrawClipText(pFont, x + nOffset, y + 4, (*itr)->m_strName.c_str(), m_clrText, rcClip, FONT_NOALPHA);
		}else{
			pCanvas->DrawClipText(pFont, x + nOffset, y + 4, (*itr)->m_strName.c_str(), WPixel(128,128,128), rcClip, FONT_NOALPHA);
		}

	}
}
//------------------------------------------------------------------------
void CChatMenu::OnPaint(ICanvas * pCanvas)
{
	XMainWnd::OnPaint(pCanvas);

	if (IsVisible())
	{
		DrawMenu(pCanvas, m_rcWnd);
	}	
}
//------------------------------------------------------------------------
DWORD CChatMenu::OnMouseMove(UINT nButton, UINT nShift, POINT pt)
{
	// 计算选中项
	int nIndex = (pt.y - m_rcWnd.top) / MENU_ITEM_HEIGHT;	

	if (nIndex < m_items.size())
	{
		m_nSelected = nIndex;
	}	

	return 0;	
}
//------------------------------------------------------------------------
DWORD CChatMenu::OnMouseDown(UINT nButton, UINT nShift, POINT pt)
{	
	SetVisible(FALSE);

	// 计算选中项
	int nClicked = (pt.y - m_rcWnd.top) / MENU_ITEM_HEIGHT;	

	if (nClicked < m_items.size())
	{
		m_nSelected = nClicked;		

		if (m_pEventReceiver != NULL)
		{
			MenuItemListItr itr = m_items.begin();
			advance(itr, m_nSelected);
			// 如果激活
			if((*itr)->m_bEnabled)
			{
				m_pEventReceiver->Clicked((*itr)->m_nId);
			}

		}
	}	

	return 0;
}

//-------------------------------------------------------------------
// 描  述：设置是否为透明
//-------------------------------------------------------------------
BOOL CChatMenu::IsTransparent(POINT& pt)
{
	return FALSE;
}

//-------------------------------------------------------------------
// 描  述：显示菜单
// 参  数：[输入] const POINT * pPos - 显示位置
// 参  数：[输出] BOOL bShow - 是否显示
// 返回值：void
//-------------------------------------------------------------------
void CChatMenu::Show(const POINT * pPos, BOOL bShow)
{ 
	if (!bShow)
	{
		SetVisible(FALSE);
		return;
	}

	if (pPos != NULL)
	{
		int dx = pPos->x + 30 - m_rcWnd.left;
		int dy = pPos->y - m_rcWnd.top;

		MoveWindow(dx, dy);	
	}

	SetVisible(TRUE);
	SetActive();	
}

//-------------------------------------------------------------------
// 描  述：添加一个菜单项
// 参  数：[输入] int nId - 菜单项ID
// 参  数：[输入] string strName - 菜单项名称
// 参  数：[输入] BOOL bEnabled - 是否Enabled
// 返回值：void
//-------------------------------------------------------------------
void CChatMenu::AddItem(int nId, string strName)
{
	CChatMenuItem * pItem = new CChatMenuItem(nId, strName,TRUE);
	if (pItem == NULL)
	{
		return;
	}

	m_items.push_back(pItem);

	m_rcWnd.bottom = m_rcWnd.top + m_items.size() * MENU_ITEM_HEIGHT + 4;
	m_pFrame->SetWindowPos(m_rcWnd);
}

//-------------------------------------------------------------------
// 描  述：设置菜单的激活状态
//-------------------------------------------------------------------
void CChatMenu::SetMenuItemEnable(int nMenuId,BOOL bEnabled)
{
	MenuItemListItr itr ;
	for(itr = m_items.begin(); itr != m_items.end();itr++)
	{
		if((*itr)->m_nId == nMenuId)
		{
			(*itr)->m_bEnabled = bEnabled;
			return;
		}
	}

}

//-------------------------------------------------------------------
// 描  述：设置菜单的激活状态
//-------------------------------------------------------------------
void CChatMenu::SetMenuItemsDisabled(const int nMenuIds[],int nCount)
{
	int i = 0;
	for(i=0 ;i<nCount ;i++)
	{
		SetMenuItemEnable(nMenuIds[i],FALSE);
	}
}

//-------------------------------------------------------------------
// 描  述：根据ID删除菜单项 
//-------------------------------------------------------------------
void CChatMenu::RemoveItem(int nId)
{
	for (MenuItemListItr itr = m_items.begin(); itr != m_items.end(); itr++)
	{
		if ((*itr)->m_nId == nId)
		{
			delete *itr;
			m_items.erase(itr);

			break;
		}		
	}
}

//-------------------------------------------------------------------
// 描  述：删除所有的菜单项 
//-------------------------------------------------------------------
void CChatMenu::RemoveAll(void)
{
	for (MenuItemListItr itr = m_items.begin(); itr != m_items.end(); itr++)
	{
		delete *itr;
	}
	m_items.clear();
}

//-------------------------------------------------------------------
// 描  述：设置菜单的事件接收者 
//-------------------------------------------------------------------
void CChatMenu::SetEventReceiver(IMenuEventReceiver * pReceiver)
{
	m_pEventReceiver = pReceiver;
}
//------------------------------------------------------------------------