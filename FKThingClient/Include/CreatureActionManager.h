/**
*	created:		2012-7-4   0:18
*	filename: 		CreatureActionManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物行为管理器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "CreatureActionBase.h"
//------------------------------------------------------------------------
class CCreatureActionManager
{
public:
	// 建立
	BOOL											Build(void);

private:
	CCreatureAction_Actor_Move_CC				m_CreatureAction_Actor_Move_CC;
	CCreatureAction_Actor_PrepAttack_CC			m_CreatureAction_Actor_PrepAttack_CC;
	CCreatureAction_Actor_Attack_CC				m_CreatureAction_Actor_Attack_CC;	
	CCreatureAction_Actor_ChangeMoveStyle_CC	m_CreatureAction_Actor_ChangeMoveStyle_CC;
	CCreatureAction_Actor_StopAttack_CC			m_CreatureAction_Actor_StopAttack_CC;
	CCreatureAction_Actor_StopPreAttack_CC		m_CreatureAction_Actor_StopPreAttack_CC;
	CCreatureAction_Actor_Die_CC				m_CreatureAction_Actor_Die_CC;
	CCreatureAction_Monster_Die_CC				m_CreatureAction_Monster_Die_CC;
	CCreatureAction_Creature_Born_CC			m_CreatureAction_Creature_Born_CC;
	CCreatureAction_Actor_SystemMessage_CC		m_CreatureAction_Actor_SystemMessage_CC;
};
//------------------------------------------------------------------------
extern CCreatureActionManager						g_CreatureActionManager;
//------------------------------------------------------------------------