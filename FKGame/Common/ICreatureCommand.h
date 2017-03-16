/**
*	created:		2012-7-2   22:03
*	filename: 		ICreatureCommand
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		所有生物的命令,当需要生物执行某一操作时，直接调用此接口相关于调生物类的方法
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../SceneObj/ICreature.h"
//------------------------------------------------------------------------
// 命令序列号
enum
{
	// 无效命令序列号
	CREATURE_COMMANDID_INVALID = 0,

	// 命令生物站立之命令序列号
	CREATURE_COMMANDID_STAND,

	CREATURE_COMMANDID_CHANGEMOVESTYLE,

	// 最大的命令序列号
	CREATURE_COMMANDID_MAXID
};
//------------------------------------------------------------------------
// 生物命令接口
struct ICreatureCommand
{	
	// 取得部件ID
	virtual DWORD		GetCommandID(void) = NULL;

	// 执行命令
	virtual int			OnCommand(ICreature * pCreature, LPCSTR pContext = NULL, int nLen = 0) = NULL;	
};
//------------------------------------------------------------------------