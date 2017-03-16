/**
*	created:		2012-6-29   14:16
*	filename: 		IThingPart
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		逻辑对像部件接口, 所有逻辑对像部件的接口必须继承此接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
typedef __int64 UID;
//------------------------------------------------------------------------
// 部件序列号（THINGPARTID + 应用到逻辑对像层 + 具体应用）
enum
{
	// 无效的部件序列号
	THINGPARTID_THING_INVALID = 0,
	
	// 生物移动部件 = 1
	THINGPARTID_CREATURE_MOVE,
	
	// 生物路径管理器部件 = 2
	THINGPARTID_CREATURE_PATHMANAGER,
	
	// 玩家广播部件 = 3
	THINGPARTID_ACTOR_BROADCAST,

	// 生物技能控制器部件 = 4
	THINGPARTID_CREATURE_SKILLCONTROL,

	// 逻辑对像广播部件 = 5
	THINGPARTID_THING_BROADCAST,

	// 生物AI部件 = 6
	THINGPARTID_CREATURE_AI,

	// 生物死亡部件 = 7
	THINGPARTID_CREATURE_DIE,

	// 生物的冷却系统 = 8
	THINGPARTID_ACTOR_COLDTIMESYSTERM,

	// 重生部件 = 9
	THINGPARTID_ACTOR_RELIVE,

	// 生物状态部件 = 10
	THINGPARTID_CREATURE_STATE,

	// 人物装备栏 = 11
	THINGPARTID_ACTOR_EQUIP,

	// 人物物品栏 = 12
	THINGPARTID_ACTOR_PACKET,

	// PK部件 = 13
	THINGPARTID_CREATURE_PK,

	// 生活技能部件ID = 14
	THINGPARTID_ACTOR_WORKSKILL,

	// 仓库部件 = 15
	THINGPARTID_ACTOR_WAREHOUSE,

	// 召唤部件 = 16
	THINGPARTID_CREATURE_CONJURE,

	// 时间戳管理部件 = 17
	THINGPARTID_ACTOR_TIMEMARKING,

	// 仇恨系统管理部件 = 18
	THINGPARTID_MONSTER_ENMITY,	

	// 摆摊部件 = 19
	THINGPARTID_ACTOR_STALL,

	// 奖赏部件 = 20
	THINGPARTID_MONSTER_PRIZEHOLD,

	// 氏族头顶部件 = 21
	THINGPARTID_ACTOR_CLANPATE,

	// 非玩家广播部件 = 22
	THINGPARTID_NONACTOR_BROADCAST,

	// 骑乘部件 = 23
	THINGPARTID_ACTOR_RIDE,

	// 验证部件 = 24
	THINGPARTID_ACTOR_VALIDATION,

	// 最大的部件序列号
	/****************************************************************
	******不要变更部件ID，因在DB中使用了物品栏ID和装备栏ID***********
	****************************************************************/
	THINGPARTID_THING_MAXID
};
//------------------------------------------------------------------------
// 此宏是发数据给客户端协议的属性ID
#define THINGPARTID_PROPID_LOWID		4000
//------------------------------------------------------------------------
// 申明
struct IThing;
//------------------------------------------------------------------------
// 接口定义
struct IThingPart
{
	// 创建
	virtual BOOL		Create(IThing * pMaster, char * pContext, int nLen) = NULL;

	// 释放
	virtual void		Release(void) = NULL;

	// 取得部件ID
	virtual DWORD		GetPartID(void) = NULL;

	// 取得本身生物
	virtual IThing *	GetMaster(void) = NULL;

	// 激活
	virtual BOOL		Activation(LPCSTR pContext, int nLen) = NULL;

	// 冻结
	virtual BOOL		Freeze(void) = NULL;

	// 取得私有对象现场
	virtual BOOL		OnGetPrivateContext(LPSTR buf, int &nLen) = NULL;
	
	// 取得公用对象现场
	virtual BOOL		OnGetPublicContext(LPSTR buf, int &nLen) = NULL;	

	// 取得数据库现场
	virtual BOOL		OnGetDBContext(LPSTR buf, int &nLen) = NULL;

	// Message
	virtual	int			OnMessage(DWORD dwMsg, LPCSTR buf, int nLen) = NULL;

	// 初始化客户端
	virtual BOOL		InitClient(UID uidClient) = NULL;
};
//------------------------------------------------------------------------