/**
*	created:		2012-7-3   2:24
*	filename: 		CreatureCommandBase
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物基本命令
					所有生物的命令,当需要生物执行某一操作时，直接调用此接口, 相关于调生物类的方法
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Common/ICreatureCommand.h"
//------------------------------------------------------------------------
// 申明
struct ICreature;
//------------------------------------------------------------------------
//  申明COMMON
#define DECLARE_SINGLE_COMMAND(class_name)						\
	private:													\
			class_name(void);									\
			static class_name			m_Instance;				\
	public:														\
			static ICreatureCommand *	GetInstance(void);		\

//------------------------------------------------------------------------
// 实例化COMMON
#define INSTANTIATION_SINGLE_COMMAND(class_name)				\
	class_name			class_name::m_Instance;					\
																\
	ICreatureCommand *	class_name::GetInstance(void)			\
	{															\
			return static_cast<ICreatureCommand *>(&m_Instance);\
	}															\

//------------------------------------------------------------------------
// 实现一个空对象 （命令序列号：CREATURE_COMMANDID_INVALID）
class CCreatureCommand_Null : public ICreatureCommand
{
public:	
	// 取得部件ID
	virtual DWORD		GetCommandID(void);
	
	// 执行命令
	virtual int			OnCommand(ICreature * pMaster, LPCSTR pContext = NULL, int nLen = 0);

	DECLARE_SINGLE_COMMAND(CCreatureCommand_Null)
};
//------------------------------------------------------------------------
// 命令生物站立之命令（命令序列号：CREATURE_COMMANDID_STAND）
class CCreatureCommand_Stand : public ICreatureCommand
{
public:
	// 取得部件ID
	virtual DWORD		GetCommandID(void);
	
	// 执行命令, TRUE : 成功，FALSE ：失败
	virtual int			OnCommand(ICreature * pMaster, LPCSTR pContext = NULL, int nLen = 0);

	DECLARE_SINGLE_COMMAND(CCreatureCommand_Stand)
};
//------------------------------------------------------------------------
// 改变移动方式，会自动进行状态转换/诸如：坐下 < -- > 跑步 < -- > 行走 （命令序列号：CREATURE_COMMANDID_CHANGEMOVESTYLE）
class CCreatureCommand_ChanageMoveStyle : public ICreatureCommand
{
public:
	// 取得部件ID
	virtual DWORD		GetCommandID(void);
	
	// 执行命令, TRUE : 成功，FALSE ：失败
	virtual int			OnCommand(ICreature * pMaster, LPCSTR pContext, int nLen);
	
	DECLARE_SINGLE_COMMAND(CCreatureCommand_ChanageMoveStyle)
};
//------------------------------------------------------------------------