/**
*	created:		2012-6-25   23:59
*	filename: 		IThingTable
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		客户端逻辑对像管理列表
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <vector>
using namespace std;
//------------------------------------------------------------------------
// 申明
struct  IThing;
typedef vector<IThing *>	TVECTOR_THING;
//------------------------------------------------------------------------
// 定义
struct IThingTable
{
	// 关闭
	virtual void			Close(void) = NULL;

	// 搜索
	virtual IThing *		Find(UID uid) = NULL;

	// 添加
	virtual IThing *		Add(IThing * pThing) = NULL;

	// 移除
	virtual BOOL			Remove(UID uid) = NULL;

	// 移除
	virtual BOOL			Remove(IThing* pThing) = NULL;

	// 取得thing列表
	virtual TVECTOR_THING *	GetThingList(void) = NULL;
};
//------------------------------------------------------------------------
