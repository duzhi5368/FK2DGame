/**
*	created:		2012-7-4   0:12
*	filename: 		CreatureActionCSCManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		CSC生物行为管理器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/CreatureActionCSC.h"
//------------------------------------------------------------------------
class CreatureActionCSCManager
{
public:
	// 建立
	BOOL												Build(void);

private:
	CCreatureAction_Creature_Move_CSC					m_CreatureAction_Creature_Move_CSC;
	CCreatureAction_Creature_ChangeLocation_CSC			m_CreatureAction_Creature_ChangeLoc_CSC;
	CCreatureAction_Creature_ChangeLocationForSkill_CSC	m_CreatureAction_Creature_ChangeLocForSkill_CSC;
	CCreatureAction_Creature_ChangeMoveStyle_CSC		m_CreatureAction_Creature_ChangeMoveStyle_CSC;
	CCreatureAction_Creature_StopAttack_CSC				m_CreatureAction_Creature_StopAttack_CSC;
	CCreatureAction_Creature_PreAttack_CSC				m_CreatureAction_Creature_PreAttack_CSC;
	CCreatureAction_Creature_Attack_CSC					m_CreatureAction_Creature_Attack_CSC;
	CCreatureAction_Creature_StopPreAttack_CSC			m_CreatureAction_Creature_StopPreAttack_CSC;
	CCreatureAction_Creature_Damage_Rehearsal_CSC		m_CreatureAction_Creature_Damage_Rehearsal_CSC;
	CCreatureAction_Creature_Cure_Rehearsal_CSC			m_CreatureAction_Creature_Cure_Rehearsal_CSC;
	CCreatureAction_Creature_Die_CSC					m_CreatureAction_Creature_Die_CSC;
	CCreatureAction_Creature_Upgrade_CSC				m_CreatureAction_Creature_Upgrade_CSC;
	CCreatureAction_Creature_BrushProData_CSC			m_CreatureAction_Creature_BrushProData_CSC;
	CCreatureAction_Creature_OpenBox_CSC				m_CreatureAction_Creature_OpenBox_CSC;
	CCreatureAction_Creature_Relive_CSC					m_CreatureAction_Creature_Relive_CSC;
	CCreatureAction_Creature_Stand_CSC					m_CreatureAction_Creature_Stand_CSC;
};
//------------------------------------------------------------------------
extern CreatureActionCSCManager						g_CreatureActionCSCManager;
//------------------------------------------------------------------------