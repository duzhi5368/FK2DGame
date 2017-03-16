/**
*	created:		2012-6-30   1:07
*	filename: 		IConjurePart
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		召唤部件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <list>
using std::list;
//------------------------------------------------------------------------
// 申明
struct IThingPart;

// 定义
struct IConjurePart : public IThingPart
{
	// 取得首领
	virtual UID					GetChief(void) = NULL;

	// 能否找到
	virtual BOOL				FindPawn(UID uidPawm) = NULL;

	// 取得爪牙列表
	virtual list< UID >			GetPawn(void) = NULL;
};
//------------------------------------------------------------------------