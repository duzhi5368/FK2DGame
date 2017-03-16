/**
*	created:		2012-6-28   2:08
*	filename: 		IItemFactory
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		HTML的显示元素对象工厂接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
class XItem;
//------------------------------------------------------------------------
// HTML的显示元素对象工厂接口(使子类可以部分实现)
struct IItemFactory  
{
	// 创建文本
	virtual XItem * CreateTextItem(void * pNode)
	{
		return NULL;
	}

	// 创建链接
	virtual XItem * CreateLinkItem(void * pNode)
	{
		return NULL;
	}
	
	virtual XItem * CreateLinkItem2(void * pNode)
	{
		return NULL;
	}

	// 创建输入框
	virtual XItem * CreateInputItem(void * pNode)
	{
		return NULL;
	}

	// 创建图片
	virtual XItem * CreateImageItem(void * pNode)
	{
		return NULL;
	}
	
	// 创建新行
	virtual XItem * CreateNewlineItem(void * pNode)
	{
		return NULL;
	}	

};
//------------------------------------------------------------------------