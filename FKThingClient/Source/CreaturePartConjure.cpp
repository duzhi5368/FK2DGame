/**
*	created:		2012-7-4   0:50
*	filename: 		CreaturePartConjure
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		召唤部件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CreaturePartConjure.h"
#include "../Include/CThingClientFacade.h"
//------------------------------------------------------------------------
CCreaturePart_Conjure::CCreaturePart_Conjure(void)
{
	m_uidChief = INVALID_UID;
	m_pMaster = NULL;
	m_listPawn.clear();
}
//------------------------------------------------------------------------
CCreaturePart_Conjure::~CCreaturePart_Conjure(void)
{

}
//------------------------------------------------------------------------
void CCreaturePart_Conjure::Release(void)
{
	// 订阅事件
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer != NULL)
	{	
		/*
		WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CONJURE);
		pEventServer->RemoveListener(static_cast<IEventActionListener *>(this),
									wMsgID,
									SRC_TYPE_ID_CREATURE,
									(DWORD)m_pMaster);
		*/
	}
	
	delete this;
}
//------------------------------------------------------------------------
BOOL CCreaturePart_Conjure::Create(IThing * pMaster, char * pContext, int nLen)
{
	if(pMaster == NULL || !pMaster->GetThingClass()->IsCreature())
	{
		return FALSE;
	}

	m_pMaster = static_cast<ICreature *>(pMaster);

	// 订阅事件
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{			
		return FALSE;
	}

	/*
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CONJURE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(this),
							  wMsgID,
							  SRC_TYPE_ID_CREATURE,
							  (DWORD)m_pMaster,
							  "CCreaturePart_Conjure::Create");
	*/
	return TRUE;
}
//------------------------------------------------------------------------
DWORD CCreaturePart_Conjure::GetPartID(void)
{
	return THINGPARTID_CREATURE_CONJURE;
}
//------------------------------------------------------------------------
IThing * CCreaturePart_Conjure::GetMaster(void)
{
	return m_pMaster;
}
//------------------------------------------------------------------------
BOOL CCreaturePart_Conjure::Activation(LPCSTR pContext, int nLen)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CCreaturePart_Conjure::Freeze(void)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CCreaturePart_Conjure::OnGetPrivateContext(LPSTR buf, int &nLen)
{
	nLen = 0;
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CCreaturePart_Conjure::OnGetPublicContext(LPSTR buf, int &nLen)
{
	nLen = 0;
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CCreaturePart_Conjure::OnGetDBContext(LPSTR buf, int &nLen)
{
	nLen = 0;
	return FALSE;
}
//------------------------------------------------------------------------
int CCreaturePart_Conjure::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
BOOL CCreaturePart_Conjure::InitClient(UID uidClient)
{
	return TRUE;
}
//------------------------------------------------------------------------
UID	CCreaturePart_Conjure::GetChief(void)
{
	return m_uidChief;
}
//------------------------------------------------------------------------
BOOL CCreaturePart_Conjure::FindPawn(UID uidPawm)
{
	list< UID >::iterator it = m_listPawn.begin();
	for( ; it != m_listPawn.end(); ++it)
	{
		UID _uidPawn = (*it);
		if(_uidPawn == uidPawm)
		{	
			return TRUE;
		}
	}

	return FALSE;
}
//------------------------------------------------------------------------
list< UID >	CCreaturePart_Conjure::GetPawn(void)
{
	return m_listPawn;
}
//------------------------------------------------------------------------
void CCreaturePart_Conjure::OnAction(IEvent *pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	int nContextLen = 0;
	char * pContext = (char *)pEvent->GetContext(nContextLen);
	if(nContextLen < sizeof(SC_MSG_CONJURE_CONTEXT))
	{
		return;
	}

	SC_MSG_CONJURE_CONTEXT * pConjureContext = (SC_MSG_CONJURE_CONTEXT *)(pContext);
	m_uidChief = pConjureContext->uidChief;

	m_listPawn.clear();
	char * pPawnBuf = (char *)(pContext + sizeof(SC_MSG_CONJURE_CONTEXT));
	for(int i = 0; i < pConjureContext->dwPawnCount; i++)
	{
		UID uidPawn = *((UID *)(pPawnBuf));
		m_listPawn.push_back(uidPawn);

		pPawnBuf += sizeof(UID);
	}
}
//------------------------------------------------------------------------