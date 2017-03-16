/**
*	created:		2012-6-29   21:16
*	filename: 		IContainerHolder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		容器拥有者（诸如装备栏，物品栏，仓库都需实现）
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../SceneObj/IThingPart.h"
//------------------------------------------------------------------------
// 申明
struct IContainer;
//------------------------------------------------------------------------
// 定义
struct IContainerHolder : public IThingPart
{
	// 取得容器
	virtual IContainer *	GetContainer(void) = NULL;

	// 是否存在（能通过物品id，容器内是否达到了要求的个数）
	virtual BOOL			IsExistByGoodsID(DWORD dwGoodsID, DWORD dwNum) = NULL;

	// 获取此物品id容器内的个数
	virtual DWORD			GetNumByGoodsID(DWORD dwGoodsID) = NULL;

	// 此容器内是否能销毁个数
	virtual BOOL			IsRemoveByGoodsID(DWORD dwGoodsID, DWORD dwNum) = NULL;

	// 此容器内能销毁的个数
	virtual DWORD			GetRemoveNumByGoodsID(DWORD dwGoodsID) = NULL;
};
//------------------------------------------------------------------------