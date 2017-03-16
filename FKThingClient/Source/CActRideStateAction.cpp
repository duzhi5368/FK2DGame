/**
*	created:		2012-7-4   1:27
*	filename: 		CActRideStateAction
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CActRideStateAction.h"
#include "../Include/CThingClientFacade.h"
#include "../../FKGame/Common/ICreatureCommand.h"
//------------------------------------------------------------------------
CCreatureActRideState::FILTER_MESSAGE_PROC	CCreatureActRideState::m_FilterMessageProc[0xFFFF + 1];
//------------------------------------------------------------------------
void CCreatureActRideState::InitFilterMessageProc(void)
{
	ZeroMemory(m_FilterMessageProc, sizeof(m_FilterMessageProc));

	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_IC, MAINACTION_MSGID_MOVE);
	m_FilterMessageProc[wMsgID] = &CCreatureActRideState::OnMove_IC_Message;

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DIE);
	m_FilterMessageProc[wMsgID] = &CCreatureActRideState::OnDie_CSC_Message;
}
//------------------------------------------------------------------------
CCreatureActRideState::CCreatureActRideState(void)
{
	m_pMaster = NULL;
}
//------------------------------------------------------------------------
CCreatureActRideState::~CCreatureActRideState(void)
{
	
}
//------------------------------------------------------------------------
BOOL CCreatureActRideState::Create(ICreature * pCreature)
{
	if(pCreature == NULL)
	{
		return FALSE;
	}

	m_pMaster = pCreature;

	return TRUE;
}
//------------------------------------------------------------------------
DWORD CCreatureActRideState::GetStateID(void)
{
	return CREATURE_STATEID_RIDE;
}
//------------------------------------------------------------------------
BOOL CCreatureActRideState::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	if(dwMsg < 0 || dwMsg > 0xFFFF)
	{
		return FALSE;
	}

	if(m_FilterMessageProc[dwMsg] != NULL)
	{
		return (this->*m_FilterMessageProc[dwMsg])(buf, nLen);
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
void CCreatureActRideState::Start(LPCSTR pContext, int nContextLen)
{

}
//------------------------------------------------------------------------
void CCreatureActRideState::End(LPCSTR pContext, int nContextLen)
{

}
//------------------------------------------------------------------------
BOOL CCreatureActRideState::OnMove_IC_Message(LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_IC, MAINACTION_MSGID_MOVE);
	/*
	if(!pEventServer->FireVote((BYTE *)buf, nLen, wMsgID, SRC_TYPE_ID_ACTOR, (DWORD)m_pMaster))
	{
		return FALSE;
	}
	*/

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_MOVE);
	m_pMaster->OnMessage(wMsgID, buf, nLen);

	return FALSE;
}
//------------------------------------------------------------------------
BOOL CCreatureActRideState::OnDie_CSC_Message(LPCSTR buf, int nLen)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DIE);
	/*
	if(!pEventServer->FireVote((BYTE *)buf, nLen, wMsgID, SRC_TYPE_ID_ACTOR, (DWORD)m_pMaster))
	{
		return FALSE;
	}
	*/

	// 转换到死亡态
	m_pMaster->GotoActState(CREATURE_STATEID_DIE);

	// 转到死亡模块去处理
	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_DIE);
	m_pMaster->OnMessage(wMsgID, buf, nLen);

	return FALSE;
}
//------------------------------------------------------------------------
BOOL CCreatureActRideState::OnMoveItem(ICreature * pCreature, POINT ptTile)
{
	if(m_pMaster == NULL)
	{
		return FALSE;
	}

	m_pMaster->OnCommand(CREATURE_COMMANDID_STAND, NULL, 0);

	return TRUE;
}
//------------------------------------------------------------------------