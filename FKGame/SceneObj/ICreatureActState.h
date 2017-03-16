/**
*	created:		2012-6-23   0:37
*	filename: 		ICreatureActState
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
// 申明
struct ICreature;
//------------------------------------------------------------------------
// 生物状态定义序列
// 未知状态
#define CREATURE_STATEID_UNKNOW			1
// 移动状态
#define CREATURE_STATEID_MOVE			2
// 坐下状态
#define CREATURE_STATEID_SIT			3
// 站立状态
#define CREATURE_STATEID_STAND			4
// 死亡状态
#define CREATURE_STATEID_DIE			5
// 骑马状态
#define CREATURE_STATEID_RIDE			6
//------------------------------------------------------------------------
// 定义
struct ICreatureActState
{
	// 创建
	virtual BOOL				Create(ICreature * pCreature) = NULL;
	// 取得本状态ID
	virtual DWORD				GetStateID(void) = NULL;
	// 消息过滤
	virtual BOOL				OnMessage(DWORD dwMsg, LPCSTR buf, int nLen) = NULL;
	// 状态开始,订阅事件
	virtual void				Start(LPCSTR pContext, int nContextLen) = NULL;
	// 状态结束,取消订阅事件
	virtual void				End(LPCSTR pContext, int nContextLen) = NULL;
	// 显示层事件
	virtual BOOL				OnMoveItem(ICreature * pCreature, POINT ptTile) = NULL;
};
//------------------------------------------------------------------------