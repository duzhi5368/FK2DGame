/**
*	created:		2012-7-4   1:23
*	filename: 		CActMoveStateAction
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		响应在此状态下，各种行为事件的发生
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CActMoveStateAction.h"
#include "../Include/CThingClientFacade.h"
#include "../../FKGame/Common/ICreatureCommand.h"
//------------------------------------------------------------------------
CCreatureActMoveState::FILTER_MESSAGE_PROC	CCreatureActMoveState::m_FilterMessageProc[0xFFFF + 1];
//------------------------------------------------------------------------
void CCreatureActMoveState::InitFilterMessageProc(void)
{
	ZeroMemory(m_FilterMessageProc, sizeof(m_FilterMessageProc));

	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_IC, MAINACTION_MSGID_MOVE);
	m_FilterMessageProc[wMsgID] = &CCreatureActMoveState::OnMove_IC_Message;

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DIE);
	m_FilterMessageProc[wMsgID] = &CCreatureActMoveState::OnDie_CSC_Message;
}
//------------------------------------------------------------------------
CCreatureActMoveState::CCreatureActMoveState(void)
{
	m_pMaster = NULL;
}
//------------------------------------------------------------------------
CCreatureActMoveState::~CCreatureActMoveState(void)
{
	
}
//------------------------------------------------------------------------
BOOL CCreatureActMoveState::Create(ICreature * pCreature)
{
	if(pCreature == NULL)
	{
		return FALSE;
	}

	m_pMaster = pCreature;

	return TRUE;
}
//------------------------------------------------------------------------
DWORD CCreatureActMoveState::GetStateID(void)
{
	return CREATURE_STATEID_MOVE;
}
//------------------------------------------------------------------------
BOOL CCreatureActMoveState::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
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
void CCreatureActMoveState::Start(LPCSTR pContext, int nContextLen)
{

}
//------------------------------------------------------------------------
void CCreatureActMoveState::End(LPCSTR pContext, int nContextLen)
{

}
//------------------------------------------------------------------------
BOOL CCreatureActMoveState::OnMove_IC_Message(LPCSTR buf, int nLen)
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
BOOL CCreatureActMoveState::OnDie_CSC_Message(LPCSTR buf, int nLen)
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
BOOL CCreatureActMoveState::OnMoveItem(ICreature * pCreature, POINT ptTile)
{
	if(m_pMaster == NULL)
	{
		return FALSE;
	}

	m_pMaster->OnCommand(CREATURE_COMMANDID_STAND, NULL, 0);

	return TRUE;
}
//------------------------------------------------------------------------