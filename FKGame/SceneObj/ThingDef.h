/**
*	created:		2012-6-29   15:26
*	filename: 		ThingDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// thing类型
enum
{
	THINGCLASS_NONE	= 0,			// 0 = 未明确
	THINGCLASS_ACTOR,				// 1 = 人物
	THINGCLASS_MONSTER,				// 2 = 怪物
	THINGCLASS_CONTAINER,			// 3 = 容器
	THINGCLASS_GOODS,				// 4 = 物品
	THINGCLASS_CHUNNEL,				// 5 = 地图通道
	THINGCLASS_BOX,					// 6 = 盒子
	THINGCLASS_STALL,				// 7 = 摊位
	THINGCLASS_MAX					// 最多
};
//------------------------------------------------------------------------
// 人物职业
enum
{
	ACTOR_OCCUPATION_NONE = 0,		// 0 = 未明确的
	ACTOR_OCCUPATION_WARRIOR,		// 1 = 战士
	ACTOR_OCCUPATION_RABBI,			// 2 = 法师
	ACTOR_OCCUPATION_PRIEST,		// 3 = 幻师
	ACTOR_OCCUPATION_NECROMANCER,	// 4 = 暗巫
	ACTOR_OCCUPATION_MAX
};
//------------------------------------------------------------------------
// 种族
enum
{
	CREATURE_PHYLE_NONE = 0,		// 0 = 未明确的
	CREATURE_PHYLE_HUMAN,			// 1 = 人族
	CREATURE_PHYLE_PERI,			// 2 = 妖族
	CREATURE_PHYLE_PSYCHE,			// 3 = 亡灵
	CREATURE_PHYLE_NATURE,			// 4 = 自然
	CREATURE_PHYLE_ORGAN,			// 5 = 机关
	CREATURE_PHYLE_WILDANIMAL,		// 6 = 野兽
	CREATURE_PHYLE_MAX
};
//------------------------------------------------------------------------
// 怪物类型
enum
{
	MONSTER_TYPE_NONE = 0,			// 0 = 未明确的
	MONSTER_TYPE_GENERAL,			// 1 = 普通怪
	MONSTER_TYPE_EXCELLENT,			// 2 = 精英怪
	MONSTER_TYPE_RARE,				// 3 = 稀有怪
	MONSTER_TYPE_BOSS,				// 4 = BOSS
	MONSTER_TYPE_BEASTIE,			// 5 = 自然界的怪物（小猫、小狗等）
	MONSTER_TYPE_SAFEGUARD,			// 6 = 肉盾型召唤兽
	MONSTER_TYPE_HATCHETMAN,		// 7 = 攻击型召唤兽
	MONSTER_TYPE_NOATTACKNPC,		// 8 = 不可攻击的NPC
	MONSTER_TYPE_ATTACKNPC,			// 9 = 可攻击NPC
	MONSTER_TYPE_GUARD,				// 10 = 守卫
	MONSTER_TYPE_MPAWN_GEN,			// 11 = 怪物召唤兽（普通AI）
	MONSTER_TYPE_MPAWN_HIGH,		// 12 = 怪物召唤兽（宠物AI）
	MONSTER_TYPE_ATTACK_TOTEM,		// 13 = 可攻击的图腾
	MONSTER_TYPE_SAFE_TOTEM,		// 14 = 不可攻击的图腾
	MONSTER_TYPE_BATTLE_BEAST,		// 15 = 战争兽(用于城战)
};
//------------------------------------------------------------------------
// 人物性别
enum
{
	ACTOR_SEX_MAN = 1,				// 1 = 男
	ACTOR_SEX_WOMAN,				// 2 = 女
	ACTOR_SEX_OTHER,				// 3 = 其他
};
//------------------------------------------------------------------------
// 生物移动类型
enum
{
	CREATURE_MOVESTYLE_WALK = 0,	// 0 = 走路
	CREATURE_MOVESTYLE_RUN,			// 1 = 跑步
	CREATURE_MOVESTYLE_SIT,			// 2 = 坐下	

	CREATURE_MOVESTYLE_RACE,		// 3 = 骑马（ 非人物才有效）
};
//------------------------------------------------------------------------
// 策略ID
enum
{
	PRIZEHOLD_STRATEGY_SIMPLE = 0,	// 简单策略
	PRIZEHOLD_STRATEGY_PAWN,		// 爪牙策略，就是什么好处都没有
	PRIZEHOLD_STRATEGY_LAST,		// 最后一次攻击权
	PRIZEHOLD_STRATEGY_CONTINUAL,	// 持续攻击权
	PRIZEHOLD_STRATEGY_BORN,		// 出生归属
};
//------------------------------------------------------------------------