/**
*	created:		2012-6-28   10:30
*	filename: 		XListBoxEx
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		扩展的ListBox
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <string>
#include "../../FKGame/WndSys/WndSys.h"
using std::string;
//------------------------------------------------------------------------
class XListBoxEx:public XWindow
{
public:
	struct SListItem
	{
		string  str;     // 内容
		WPixel  wColor;  // 文本颜色
		DWORD   dwData;  // 内容

	};

	typedef list<SListItem>  ItemList;
    typedef list<SListItem>::iterator ItemListItr;
Signals:
	Signal1	  s_selChanged;
public:
	XListBoxEx();
	virtual ~XListBoxEx();
public:
	BOOL Create(XWindow *pParent,IResObject *pRes,int nWndId);
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseWheel(UINT nButton, UINT nShift, short zDelta, POINT pt);
	virtual void OnDraw(ICanvas *pCanvas,RECT& rcClip);
	void AddItem(LPCSTR lpStr,WPixel wColor,DWORD data);
    void SetVisibleStart(int nIndex);
    void SetVisibleCount(int nCount);
	int  GetSel();
	void OnSelChanage(DWORD dwParam);
	void Clear();
    BOOL   IsTransparent(POINT& pt);
	int    GetVisibleCount();
	int    GetSize();
	DWORD  GetItemData(int nIndex);
    void RemoveItem(int nIndex);
protected:
	ItemListItr LocateItem(int index);
	void ScrollUp();
    void ScrollDown();
private:
	ItemList m_ItemList;
	int      m_nVisibleCount;        // 可见的数目
	int      m_nVisibleStartIndex ;  // 第一个可见的Item的索引
    int      m_nSel;                 // 当前选中的条目
	int      m_nItemHeight;        
	WPixel   m_clrSelected;
};
//------------------------------------------------------------------------
