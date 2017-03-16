/**
*	created:		2012-7-4   1:51
*	filename: 		ThingActionCSCManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		CSCÂß¼­¶ÔÏñ¹ÜÀíÆ÷
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/ThingActionCSCManager.h"
#include "../Include/CThingClientFacade.h"
//------------------------------------------------------------------------
CThingActionCSCManager	g_ThingActionCSCManager;
//------------------------------------------------------------------------
BOOL CThingActionCSCManager::Build(void)
{
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{
		return FALSE;
	}

	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DESTROY);
	/*
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_ThingAction_Destroy_CSC),
							  wMsgID,
							  SRC_TYPE_ID_THING,
							  0,
							  "CThingActionCSCManager::Build");
	*/
	return TRUE;
}
//------------------------------------------------------------------------