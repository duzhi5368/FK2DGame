/**
*	created:		2012-7-4   0:19
*	filename: 		CreatureActionManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物行为管理器
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CreatureActionManager.h"
#include "../Include/CThingClientFacade.h"
///------------------------------------------------------------------------
CCreatureActionManager   g_CreatureActionManager;
//------------------------------------------------------------------------
BOOL CCreatureActionManager::Build(void)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	/*
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_MOVE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Actor_Move_CC),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  0,
							  "CCreatureActionManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_PRE_ATTACK);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Actor_PrepAttack_CC),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  0,
							  "CCreatureActionManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_ATTACK);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Actor_Attack_CC),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  0,
							  "CCreatureActionManager::Create");	

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_STOP_ATTACK);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Actor_StopAttack_CC),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  0,
							  "CCreatureActionManager::Create");	
	
	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_STOP_PRE_ATTACK);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Actor_StopPreAttack_CC),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  0,
							  "CCreatureActionManager::Create");	

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_CHANGEMOVESTYLE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Actor_ChangeMoveStyle_CC),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  0,
							  "CCreatureActionManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_DIE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Actor_Die_CC),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  0,
							  "CCreatureActionManager::Create");
	
	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_DIE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Monster_Die_CC),
							 wMsgID,
							 SRC_TYPE_ID_MONSTER,
							 0,
							 "CCreatureActionManager::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_BORN);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Creature_Born_CC),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  0,
							  "CCreatureActionManager::Create");	

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_SYSTEMMESSAGE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_CreatureAction_Actor_SystemMessage_CC),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  0,
							  "CCreatureActionManager::Create");	
	*/
	return TRUE;
}
//------------------------------------------------------------------------