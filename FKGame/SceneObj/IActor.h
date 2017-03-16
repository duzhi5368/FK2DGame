/**
*	created:		2012-6-23   0:32
*	filename: 		IActor
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "ICreature.h"
//------------------------------------------------------------------------
// 接口定义
struct IActor : public ICreature
{
	// 取得帐号的DB ID
	virtual DWORD	GetUserDBID(void) = NULL;
	// 取得角色的DB ID
	virtual DWORD	GetActorDBID(void) = NULL;
	// 取得角色的氏族ID
	virtual DWORD	GetActorClanID(void) = NULL;
};
//------------------------------------------------------------------------