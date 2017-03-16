/**
*	created:		2012-7-3   0:40
*	filename: 		BoxProp
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		盒子属性定义
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Macro.h"
//------------------------------------------------------------------------
enum
{
	BOX_CREATE  , // 创建状态
	BOX_OPENING , // 正在打开状态
	BOX_OPENED  , // 已经打开
	BOX_CLOSED  , // 处于关闭状态
	BOX_COMMAND   // 发送命令
};
//------------------------------------------------------------------------
enum
{
	BOX_COMMAND_CLEARRIGHT = 0,
	BOX_COMMAND_ADDRIGHT      ,
	BOX_COMMAND_DELRIGHT      ,
	BOX_COMMAND_MAX
};
//------------------------------------------------------------------------
enum
{
	BOX_TYPE_MINE     = 1, // 矿
	BOX_TYPE_MEDICINE    , // 草药
	BOX_TYPE_TREASURE    , // 宝箱
	BOX_TYPE_TASK        , // 任务盒子
	BOX_TYPE_DYNAMIC       // 动态生成的掉装盒子
};
//------------------------------------------------------------------------
enum
{
	BOX_ID_UNKNOWN           = 0, // 未知或者不用关心的
	BOX_ID_CREATE_BY_MONSTER    , // 怪物杀人生成
	BOX_ID_CREATE_BY_ACTOR1     , // 人杀怪生成
	BOX_ID_CREATE_BY_ACTOR2     , // 人杀人生成
	BOX_ID_CREATE_BY_TEAM1      , // 人杀怪生成(组队)
	BOX_ID_CREATE_BY_TEAM2        // 人杀人生成(组队)
};
//------------------------------------------------------------------------
struct SBOX_CONTEXT
{
	UID uidBox; // 盒子
	DWORD dwState; // 盒子状态
};
//------------------------------------------------------------------------
struct SBOX_PROPERTY_PUBLIC
{
	char	szName[MAX_NAME_LEN]; // 名字
	POINTS  ptLoc; // 位置
	DWORD	dwResID1; // 盒子ID
	DWORD   dwResID2; // 星星id
	DWORD   dwTypeId; // 盒子的类型，矿、草药、地上宝箱、怪物掉装
	DWORD   dwBoxId; // 类型下的id，怪物掉装的需要动态分配
	DWORD   dwState; // 状态，关闭、正在打开(进度条)、打开
};
//------------------------------------------------------------------------
struct SBOX_PROPERTY_LIST : public SBOX_PROPERTY_PUBLIC
{
	DWORD   dwOpenTime; // 打开时间，打开进度条的需要
	DWORD   dwCloseTime; // 强制关闭时间，防止玩家打开不关闭
	DWORD   dwRefresh; // 重生时间，如果为-1不重生
	DWORD   dwRightTime; // 动态盒子的权限消息时间
};
//------------------------------------------------------------------------