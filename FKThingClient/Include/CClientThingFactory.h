/**
*	created:		2012-7-2   22:22
*	filename: 		CClientThingFactory
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		传入一个逻辑对象的类型，和现场，返回一个逻辑对象指针
*					逻辑对象包括地图上的人,怪物,NPC,装备,传送点等
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IThingFactory.h"
//------------------------------------------------------------------------
class CThingFactory : public IThingFactory
{
public:
	//////////////////////////////////////////////////////////
	/******************** IThingFactory *********************/
	// 每一个逻辑对象你可以选择是用详细的现场创建还是用粗略的现场创建
	// 典型应用是游戏中的人物，你可能需要看到自己的攻击力防御力等各种属性
	// 包括物品栏的每一件物品，而你看别人只能看到他们的角色类型,性别
	// 装备等， 他们的详细属性或者物品栏的物品服务器是不会发下来的
	// 所以需要支持两种方式创建
	virtual IThing *	CreatePrivateThing(DWORD dwThingClassID, UID uid, const char * buf, int nLen);

	virtual IThing *	CreatePublicThing(DWORD dwThingClassID, UID uid, const char * buf, int nLen);

	virtual void		Release(void);

private:
	// 创建人物
	IThing *			CreateActor_private(UID uid, LPCSTR buf, int nLen);
	IThing *			CreateActor_public(UID uid, LPCSTR buf, int nLen);

	// 创建怪物
	IThing *			CreateMonster_public(UID uid, LPCSTR buf, int nLen);

	// 创建物品
	IThing *			CreateGoods_private(UID uid, LPCSTR buf, int nLen);
	IThing *			CreateGoods_public(UID uid, LPCSTR buf, int nLen);

	// 创建地图通道
	IThing *			CreateChunnel_public(UID uid, LPCSTR buf, int nLen);
	
	// 创建盒子
	IThing *            CreateBox_public(UID uid, LPCSTR buf, int nLen);

	// 创建摊位
	IThing *			CreateStall_public(UID uid, LPCSTR buf, int nLen);
};
//------------------------------------------------------------------------