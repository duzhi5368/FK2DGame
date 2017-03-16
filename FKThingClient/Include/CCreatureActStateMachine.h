/**
*	created:		2012-7-4   1:16
*	filename: 		CCreatureActStateMachine
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物行为状态机.
					生物行为状态机器, 包括走路,跑步,死亡,站立,坐下
					包括各种状态下的下一状态，事件的订阅，事件的屏蔽
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "CActSitStateAction.h"
#include "CActStandStateAction.h"
#include "CActDieStateAction.h"
#include "CActRideStateAction.h"
#include "CActMoveStateAction.h"
//------------------------------------------------------------------------
class CCreatureActStateMachine
{
public:
	// 创建
	BOOL							Create(ICreature * pMaster);
	// 设置当前行动状态
	BOOL							GotoActState(DWORD dwActStateID, LPCSTR pContext, int nLen);
	// 取得当前行动状态
	ICreatureActState *				GetCurActState(void);
	// 取得当前行动状态ID
	DWORD							GetCurActStateID(void);
public:
	CCreatureActStateMachine(void);
	~CCreatureActStateMachine(void);
private:
	// 坐下状态
	CCreatureActSitState			m_CreatureActSitState;	
	// 站立状态
	CCreatureActStandState			m_CreatureActStandState;
	// 死亡状态
	CCreatureActDieState			m_CreatureActDieState;
	// 移动状态
	CCreatureActMoveState			m_CreatureActMoveState;
	// 骑马状态
	CCreatureActRideState			m_CreatureActRideState;

private:
	// 主人
	ICreature *						m_pMaster;
	// 当前行为状态
	ICreatureActState *				m_pCurCreatureActState;	
};
//------------------------------------------------------------------------