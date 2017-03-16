/**
*	created:		2012-7-4   0:34
*	filename: 		ThingPart
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		部件定义
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 装备部件
// 攻击之时计算耐久
#define PART_MESSAGE_EQUIP_ENDURE_ATTACK		1
// 伤害之时计算耐久
#define PART_MESSAGE_EQUIP_ENDURE_DAMAGE		2
// 死亡之时计算耐久
#define	PART_MESSAGE_EQUIP_ENDURE_DIE			3
// 被人观察
#define PART_MESSAGE_EQUIP_LOOK					4
//------------------------------------------------------------------------
// 召唤部件
// 取召唤信息
#define PART_MESSAGE_CONJURE_GETINFOR			1
// 爪牙死忘后，通知主人的召唤部件删除
#define PART_MESSAGE_CONJURE_PAWNDIE			2	
//------------------------------------------------------------------------
// 骑乘部件
// 取得部件数据
#define PART_MESSAGE_RIDE_GETINFOR				1
//------------------------------------------------------------------------
// 更新人物外观
#define PART_MESSAGE_RIDE_SHOW					2
//------------------------------------------------------------------------