/**
*	created:		2012-6-28   2:43
*	filename: 		IGoods
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		物品接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IMapThing.h"
//------------------------------------------------------------------------
// 定义接口
struct IGoods : public IMapThing
{
	// 取得物品ID
	virtual DWORD		GetGoodsID(void) = NULL;

	// 取得物品名字
	virtual LPCSTR		GetName(void) = NULL;

	// 取得物品的数字型属性
	virtual int			GetPropNum(DWORD dwPropID) = NULL;

	// 取得物品的字符型属性
	virtual LPCSTR		GetPropStr(DWORD dwPropID) = NULL;

	// 取得物品数量
	virtual DWORD		GetNumber(void) = NULL;

	// 取得物品的使用锁定
	virtual DWORD		GetUseLock(void) = NULL;

	// 取得绑定标志，单独查看绑定标志
	virtual  BOOL		GetBindFlag(DWORD dwFlag) = NULL;  
};
//------------------------------------------------------------------------

