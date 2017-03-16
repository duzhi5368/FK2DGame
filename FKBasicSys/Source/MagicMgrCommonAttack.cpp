/**
*	created:		2012-7-2   3:20
*	filename: 		MagicMgrCommonAttack
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrCommonAttack.h"
#include "../Include/CreatureView.h"
#include "../Include/StaticObj.h"
//------------------------------------------------------------------------
CMagicMgrCommonAttack::CMagicMgrCommonAttack()
{
}
//------------------------------------------------------------------------
CMagicMgrCommonAttack::~CMagicMgrCommonAttack()
{
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrCommonAttack::Create(void* buf, int nSize)
{
	if(m_pMasterView != NULL)
	{
		SetSoundIDBeHit(m_pMasterView->m_nCommonAttackBeHitSoundID);
	}

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY,
		static_cast<ITimerSink*>(this),
		GetAttackDelay(),
		"BasicSys!!CMagicMgrCommonAttack::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrCommonAttack::OnTimer(DWORD dwEventID)
{
	if(dwEventID == TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
		SetBeginAttack();
		BeginDamage();
		g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY,
			static_cast<ITimerSink*>(this),
			MAGIC_MGR_DESTROY_TIME,
			"BasicSys!!CMagicMgrCommonAttack::OnTimer");
	}
	else
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
		delete this;
	}
}
//------------------------------------------------------------------------
void CMagicMgrCommonAttack::BeginAttack()
{
	if(!IsBeginAttack())
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY);
}
//------------------------------------------------------------------------