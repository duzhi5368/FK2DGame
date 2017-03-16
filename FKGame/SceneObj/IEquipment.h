/**
*	created:		2012-6-29   20:21
*	filename: 		IEquipment
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IGoods.h"
//------------------------------------------------------------------------
// 申明
struct IEffect;
struct ICreature;
//------------------------------------------------------------------------
// 定义
struct IEquipment : public IGoods
{
	// 取得耐久相关数值
	virtual void		GetEndure(int &nCur, int &nCurMax) = NULL;

	// 取得效果
	virtual IEffect *	GetEffect(int nIndex) = NULL;

	// 检查是否能装备该项装备
	virtual	BOOL		CanEquip(ICreature * pCreature, int nEquipPos) = NULL;

	// 取得精练等级
	virtual int			GetSmeltLevel(void) = NULL;

	// 取得装备的插槽数
	virtual DWORD		GetSlotLeft(void) = NULL;
};
//------------------------------------------------------------------------