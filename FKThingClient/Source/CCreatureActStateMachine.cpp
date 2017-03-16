/**
*	created:		2012-7-4   1:18
*	filename: 		CCreatureActStateMachine
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CCreatureActStateMachine.h"
//------------------------------------------------------------------------
CCreatureActStateMachine::CCreatureActStateMachine(void)
{
	m_pMaster = NULL;
	m_pCurCreatureActState = NULL;
}
//------------------------------------------------------------------------
CCreatureActStateMachine::~CCreatureActStateMachine(void)
{

}
//------------------------------------------------------------------------
BOOL CCreatureActStateMachine::Create(ICreature * pMaster)
{
	m_pMaster = pMaster;

	m_CreatureActSitState.Create(m_pMaster);
	m_CreatureActStandState.Create(m_pMaster);
	m_CreatureActDieState.Create(m_pMaster);
	m_CreatureActMoveState.Create(m_pMaster);
	m_CreatureActRideState.Create(m_pMaster);

	// 初始化为站立态
	GotoActState(CREATURE_STATEID_STAND, NULL, 0);

	return TRUE;	
}
//------------------------------------------------------------------------
BOOL CCreatureActStateMachine::GotoActState(DWORD dwStateID, LPCSTR pContext, int nLen)
{
	ICreatureActState * pOldCreatureActState = m_pCurCreatureActState;
	switch(dwStateID) 
	{
	case CREATURE_STATEID_MOVE:
		{			
			m_pCurCreatureActState = static_cast< ICreatureActState *>(&m_CreatureActMoveState);			
		}
		break;		
	case CREATURE_STATEID_SIT:
		{
			m_pCurCreatureActState = static_cast< ICreatureActState *>(&m_CreatureActSitState);
		}
		break;
	case CREATURE_STATEID_STAND:
		{		
			m_pCurCreatureActState = static_cast< ICreatureActState *>(&m_CreatureActStandState);
		}
		break;
	case CREATURE_STATEID_DIE:
		{
			m_pCurCreatureActState = static_cast< ICreatureActState *>(&m_CreatureActDieState);
		}
		break;
	case CREATURE_STATEID_RIDE:
		{
			m_pCurCreatureActState = static_cast< ICreatureActState *>(&m_CreatureActRideState);
		}
	default:return FALSE;
	}

	if(pOldCreatureActState != NULL)
	{
		pOldCreatureActState->End(NULL, 0);
	}

	if(m_pCurCreatureActState != NULL)
	{
		m_pCurCreatureActState->Start(pContext, nLen);
	}

	return TRUE;
}
//------------------------------------------------------------------------
ICreatureActState * CCreatureActStateMachine::GetCurActState(void)
{
	return m_pCurCreatureActState;
}
//------------------------------------------------------------------------
DWORD CCreatureActStateMachine::GetCurActStateID(void)
{
	return m_pCurCreatureActState->GetStateID();
}
//------------------------------------------------------------------------