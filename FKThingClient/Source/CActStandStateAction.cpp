/**
*	created:		2012-7-4   1:32
*	filename: 		CActStandStateAction
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		站立状态
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CActStandStateAction.h"
#include "../Include/CThingClientFacade.h"
#include "../../FKGame/Common/ICreatureCommand.h"
//------------------------------------------------------------------------
CCreatureActStandState::FILTER_MESSAGE_PROC	CCreatureActStandState::m_FilterMessageProc[0xFFFF + 1];
//------------------------------------------------------------------------
void CCreatureActStandState::InitFilterMessageProc(void)
{
	ZeroMemory(m_FilterMessageProc, sizeof(m_FilterMessageProc));

	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_IC, MAINACTION_MSGID_MOVE);
	m_FilterMessageProc[wMsgID] = &CCreatureActStandState::OnMove_IC_Message;

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DIE);
	m_FilterMessageProc[wMsgID] = &CCreatureActStandState::OnDie_CSC_Message;
}
//------------------------------------------------------------------------
CCreatureActStandState::CCreatureActStandState(void)
{
	m_pMaster = NULL;
}
//------------------------------------------------------------------------
CCreatureActStandState::~CCreatureActStandState(void)
{
	
}
//------------------------------------------------------------------------
BOOL CCreatureActStandState::Create(ICreature * pCreature)
{
	if(pCreature == NULL)
	{
		return FALSE;
	}

	m_pMaster = pCreature;

	return TRUE;
}//------------------------------------------------------------------------
DWORD CCreatureActStandState::GetStateID(void)
{
	return CREATURE_STATEID_STAND;
}
//------------------------------------------------------------------------
BOOL CCreatureActStandState::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
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
void CCreatureActStandState::Start(LPCSTR pContext, int nContextLen)
{

}
//------------------------------------------------------------------------
void CCreatureActStandState::End(LPCSTR pContext, int nContextLen)
{

}
//------------------------------------------------------------------------
BOOL CCreatureActStandState::OnMove_IC_Message(LPCSTR buf, int nLen)
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

	// 转换状态
	m_pMaster->GotoActState(CREATURE_STATEID_STAND);

	// 移动模块
	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_MOVE);
	m_pMaster->OnMessage(wMsgID, buf, nLen);

	return FALSE;
}
//------------------------------------------------------------------------
BOOL CCreatureActStandState::OnDie_CSC_Message(LPCSTR buf, int nLen)
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
BOOL CCreatureActStandState::OnMoveItem(ICreature * pCreature, POINT ptTile)
{
	return FALSE;
}
//------------------------------------------------------------------------