/**
*	created:		2012-6-26   1:02
*	filename: 		IThingFactory
*	author:			FreeKnight
*	Copyright (C): 	传入一个逻辑对象的类型，和现场，返回一个逻辑对象指针
*					逻辑对象包括地图上的人,怪物,NPC,装备,传送点等
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
#include "../Common/Macro.h"
//------------------------------------------------------------------------
// 申明
struct IThing;
//------------------------------------------------------------------------
// 定义
struct IThingFactory
{
	// 每一个逻辑对象你可以选择是用详细的现场创建还是用粗略的现场创建
	// 典型应用是游戏中的人物，你可能需要看到自己的攻击力防御力等各种属性
	// 包括物品栏的每一件物品，而你看别人只能看到他们的角色类型,性别
	// 装备等， 他们的详细属性或者物品栏的物品服务器是不会发下来的
	// 所以需要支持两种方式创建
	virtual IThing *	CreatePrivateThing(DWORD dwThingClassID, UID uid, const char * buf, int nLen) = NULL;

	virtual IThing *	CreatePublicThing( DWORD dwThingClassID, UID uid, const char * buf, int nLen) = NULL;

	virtual void		Release(void) = NULL;
};
//------------------------------------------------------------------------