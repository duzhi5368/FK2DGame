/**
*	created:		2012-6-28   12:22
*	filename: 		XChatEx
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		聊天信息显示框扩展
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "ChatItemFactory.h"
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
// 聊天信息显示框扩展
class XChatEx : public XChat  
{
public:
	XChatEx(XWindow * pParent);
	virtual ~XChatEx();

public:
	RECT GetScrollBarRect(void) const;
	RECT GetContainerRect(void) const;
	void SetItemFactory(IItemFactory * pFactory);
	BOOL DisplayMessage(const CHtmlNode * pContent);
	BOOL AddItem(XItem * pItem, BOOL bNewline);
	virtual void SetWindowPos(RECT & rc);
	virtual void SetWindowPos(RECT & rcScrollBar, RECT & rcContainer);	
	void OnVScrollBarChanged(DWORD dwParam);

private:
	void AutoScroll(void);
	
private:
	RECT m_rcContainer;
	RECT m_rcScrollBar;
	IItemFactory * m_pItemFactory;
};
//------------------------------------------------------------------------