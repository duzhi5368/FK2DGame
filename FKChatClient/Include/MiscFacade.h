/**
*	created:		2012-6-28   8:58
*	filename: 		MiscFacade
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		这个类用来实现一些常用功能
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WorkSkill/IWorkSkillClient.h"
#include "../../FKGame/Container/IContainerHolder.h"
#include "../../FKGame/Container/IContainerClient.h"
#include "../../FKGame/SceneObj/IMedicament.h"
#include "../../FKGame/SceneObj/ICreature.h"
//------------------------------------------------------------------------
namespace MISCFACADE
{
	//通过物品ID, 取物品名字
	LPCSTR GetGoodsName(DWORD dwGoodsID);
	
	//获得人物背包中, 某个物品的个数 (只返回能删除的)
	int GetGoodsNumFromPackage(DWORD dwGoodsID);
	
	//通过UID取某个Thing
	IThing *GetThingFromUID(UID uidThing);
	
	//取玩家某个技能的等级
	int GetPlayerSkillLevel(int nSkillID);
	
	//取人物身上,空练瓶的数量(没有开始炼化的)
	int GetEmptyBottle(int nBottleID);
	
	//取Thing的类型
	int GetThingClass(UID uidThing);
	
	//通过工作技能ID,,取工作技能, 如果没学会,返回NULL
	IWorkSkill * GetWorkSkill(int nSkillID);
	
	//获取某个生物是否死亡
	BOOL IsDead(UID uidCreature);

	//取生物部件
	void *GetThingPart(ICreature *pCreature, int nPartIndex);

	//判断是否掌握某技能
	BOOL MasteredSkill(int nSkillID);

	//判断UID是否召唤兽
	BOOL isSummonPet(UID uidCreature);

	//取当前人物背包
	IContainer *GetActorPackage(void);

	//判断物品是否一件耐久为0 的装备
	BOOL isBrokenEquipment(UID uidThing);

	//判断玩家召唤出来多少只狗
	int GetPetCount(IActor *pActor);

	//判断一个UID是否炼化瓶
	BOOL isBottle(UID uidThing);

	//判断一个炼化瓶是否在炼化中
	BOOL isBottleRefining(IMedicament *pMedicament);

	//判断一个物品能否丢弃
	BOOL GoodsCanDiscard(UID uidGoods);

	//判断一个UID是否元宝
	BOOL isPlayTimeTreasure(UID uidThing);

	//对一个数值范围进行修正
	int CorrectRange(int val, int low, int high);

	//将szSrc的内容插入到szDst的nPos个位置内(szDst要足够大)
	void StringInsert(char *szDst, char *szSrc, int nPos);

	//传入套装ID, 查询身上穿了多少件这样的套装
	int GetSuitEquipedCount(int nSuitID);
};
//------------------------------------------------------------------------