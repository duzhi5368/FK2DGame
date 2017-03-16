/**
*	created:		2012-7-4   0:14
*	filename: 		CreatureActionCSCManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		CSC生物行为管理器
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CreatureActionCSCManager.h"
#include "../Include/CThingClientFacade.h"
//------------------------------------------------------------------------
CreatureActionCSCManager g_CreatureActionCSCManager;
//------------------------------------------------------------------------
BOOL CreatureActionCSCManager::Build(void)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_MOVE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_Move_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CHANGELOCATION);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_ChangeLoc_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CHANGELOCATION_FOR_SKILL);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_ChangeLocForSkill_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CHANGEMOVESTYLE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_ChangeMoveStyle_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_STOP_ATTACK);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_StopAttack_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_PRE_ATTACK);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_PreAttack_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_ATTACK);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_Attack_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_STOP_PRE_ATTACK);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_StopPreAttack_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");	
	
	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DAMAGE_REHEARSAL);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_Damage_Rehearsal_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");	

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CURE_REHEARSAL);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_Cure_Rehearsal_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");	

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DIE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_Die_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");
	
	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_UPGRADE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_Upgrade_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");
	
	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_BRUSHPROPDATA);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_BrushProData_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_OPENBOX);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_OpenBox_CSC),
							  wMsgID,
							  SRC_TYPE_ID_THING,
							  0,
							  "CreatureActionCSCManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_RELIVE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_Relive_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");	

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_STAND);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_Stand_CSC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CreatureActionCSCManager::Create");	
	*/
	return TRUE;
}
//------------------------------------------------------------------------