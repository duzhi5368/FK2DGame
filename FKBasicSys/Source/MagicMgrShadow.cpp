/**
*	created:		2012-7-2   3:27
*	filename: 		MagicMgrShadow
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrShadow.h"
#include "../Include/CreatureView.h"
#include "../Include/StaticObj.h"
//------------------------------------------------------------------------
CMagicMgrShadow::CMagicMgrShadow()
{
}
//------------------------------------------------------------------------
CMagicMgrShadow::~CMagicMgrShadow()
{
	if(m_pMasterView != NULL)
		m_pMasterView->DelLinkView(&m_pMasterView);

	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrShadow::Create(CCreatureView *pMaster)
{
	if(pMaster == NULL)
		return FALSE;

	m_pMasterView = pMaster;
	m_pMasterView->AddLinkView(&m_pMasterView);

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE,
		static_cast<ITimerSink*>(this),
		15,
		"BasicSys!!CMagicMgrShadow::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrShadow::OnTimer(DWORD dwEventID)
{
	if(m_pMasterView == NULL)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
		return;
	}

	CStaticObjEx *obj = new CStaticObjEx();
	if(!obj->CreateItem(m_pMasterView))
	{
		delete this;
		return;
	}
	obj->AddToMapView(obj->m_ptTile);
	obj->AddToTimerAxis();
}
//------------------------------------------------------------------------