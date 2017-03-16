/**
*	created:		2012-6-28   7:15
*	filename: 		ChatMenu
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		组队菜单
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
#include <string>
#include "../../FKGame/WndSys/WndSys.h"
using std::string;
//------------------------------------------------------------------------
const int MENU_ITEM_HEIGHT = 15;
//------------------------------------------------------------------------
// 菜单事件接收者
struct IMenuEventReceiver
{
	// 单击事件
	virtual void Clicked(int nId) = 0;
};
//------------------------------------------------------------------------
// 组队菜单项
class CChatMenuItem
{
public:
	int		m_nId;					// 菜单项的ID
	string	m_strName;				// 菜单项的名称
	BOOL    m_bEnabled;				// 是否Enabled
public:
	CChatMenuItem(int nId, string strName,BOOL bEnabled = TRUE);
};
//------------------------------------------------------------------------
// 组队菜单
class CChatMenu : public XMainWnd
{
public:
	typedef list<CChatMenuItem *> MenuItemList;
	typedef list<CChatMenuItem *>::iterator MenuItemListItr;

public:
	CChatMenu(void);
	virtual ~CChatMenu(void);
	
Signals:
	Signal1 s_selChanged;
	
public:
	BOOL Create(IResObject * pResObject);
	void DrawMenu(ICanvas * pCanvas, RECT & rcClip);
	virtual void OnPaint(ICanvas * pCanvas);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual void OnActivate(BOOL bActive);
	virtual BOOL IsTransparent(POINT& pt);
	void Show(const POINT * pPos, BOOL bShow); 	
	void AddItem(int nId, string strName);
	void RemoveItem(int nId);
	void RemoveAll(void);
	void SetEventReceiver(IMenuEventReceiver * pReceiver);
    void SetMenuItemEnable(int nMenuId,BOOL bEnabled = TRUE);  
    void SetMenuItemsDisabled(const int nMenuIds[],int nCount);

private:
	XEdit * m_pFrame;						// 框架
	IMenuEventReceiver * m_pEventReceiver;	// 事件接收者
	int m_nSelected;						// 选中的项
	WPixel m_clrSelected;					// 选中项的颜色
	WPixel m_clrText;						// 菜单项的文本颜色
	MenuItemList m_items;					// 菜单项列表	
};
//------------------------------------------------------------------------