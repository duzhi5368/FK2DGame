/**
*	created:		2012-7-4   1:20
*	filename: 		CActDieStateAction
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		生物死亡走路状态，响应事件
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CActDieStateAction.h"
#include "../Include/CreatureViewWrapper.h"
//------------------------------------------------------------------------
CCreatureActDieState::FILTER_MESSAGE_PROC CCreatureActDieState::m_FilterMessageProc[0xFFFF + 1];
//------------------------------------------------------------------------
void CCreatureActDieState::InitFilterMessageProc(void)
{
	ZeroMemory(m_FilterMessageProc, sizeof(m_FilterMessageProc));

	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_IC, MAINACTION_MSGID_SKILL_ATTACK);
	m_FilterMessageProc[wMsgID] = &CCreatureActDieState::OnGeneral_Vote_Message;

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_IC, MAINACTION_MSGID_MOVE);
	m_FilterMessageProc[wMsgID] = &CCreatureActDieState::OnGeneral_Vote_Message;

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DIE);
	m_FilterMessageProc[wMsgID] = &CCreatureActDieState::OnGeneral_Vote_Message;
}
//------------------------------------------------------------------------
CCreatureActDieState::CCreatureActDieState(void)
{
	m_pMaster = NULL;
}
//------------------------------------------------------------------------
CCreatureActDieState::~CCreatureActDieState(void)
{
	
}
//------------------------------------------------------------------------
BOOL CCreatureActDieState::Create(ICreature * pCreature)
{
	if(pCreature == NULL)
	{
		return FALSE;
	}

	m_pMaster = pCreature;

	return TRUE;
}
//------------------------------------------------------------------------
DWORD CCreatureActDieState::GetStateID(void)
{
	return CREATURE_STATEID_DIE;
}
//------------------------------------------------------------------------
BOOL CCreatureActDieState::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
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
void CCreatureActDieState::Start(LPCSTR pContext, int nContextLen)
{

}
//------------------------------------------------------------------------
void CCreatureActDieState::End(LPCSTR pContext, int nContextLen)
{

}
//------------------------------------------------------------------------
BOOL CCreatureActDieState::OnGeneral_Vote_Message(LPCSTR buf, int nLen)
{
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CCreatureActDieState::OnMoveItem(ICreature * pCreature, POINT ptTile)
{
	return FALSE;
}
//------------------------------------------------------------------------