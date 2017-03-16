/**
*	created:		2012-6-28   6:47
*	filename: 		ChatItemFactory
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		聊天框的显示元素的对象工厂
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../IItemFactory.h"
#include "HtmlNode.h"
#include "../../FKCanvas/IFKCanvas.h"
//------------------------------------------------------------------------
enum 
{
	LINK_PLAYER,	// 玩家链接
	LINK_THING,		// 装备链接
	LINK_SENTENCE	// 句子链接
};
//------------------------------------------------------------------------
// 聊天框的显示元素的对象工厂
class CChatItemFactory : public IItemFactory  
{
public:
	CChatItemFactory(void);
	virtual ~CChatItemFactory(void);

public:
	// 创建文本
	virtual XItem * CreateTextItem(void * pNode);
	// 创建链接
	virtual XItem * CreateLinkItem(void * pNode);
	// 创建输入框
	virtual XItem * CreateInputItem(void * pNode);
	// 创建图片
	virtual XItem * CreateImageItem(void * pNode);
	// 创建新行
	virtual XItem * CreateNewlineItem(void * pNode);
	// 创建链接
	virtual XItem * CreateLinkItem2(void * pNode);	

private:
	XItem * CreateSentenceLink2(CHtmlNode * pNode);
	XItem * CreateSentenceLink(CHtmlNode * pNode);
	XItem * CreateThingLink(CHtmlNode * pNode);
	XItem * CreatePlayerLink(CHtmlNode * pNode);
	BOOL GetColor(const CHtmlNode * pNode, WPixel & wpColor, string strAttribute = "color");
	void GetFont(const CHtmlNode * pNode, ITTFont ** pFont);
};
//------------------------------------------------------------------------