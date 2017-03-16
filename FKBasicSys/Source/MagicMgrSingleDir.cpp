/**
*	created:		2012-7-2   3:28
*	filename: 		MagicMgrSingleDir
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrSingleDir.h"
#include "../Include/CreatureView.h"
//------------------------------------------------------------------------
CMagicMgrSingleDir::CMagicMgrSingleDir()
{
	m_pRun			= NULL;
	m_pExplode		= NULL;
}
//------------------------------------------------------------------------
CMagicMgrSingleDir::~CMagicMgrSingleDir()
{
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrSingleDir::Create(void* buf, int nSize)
{
	SMagicMgrArchive_SingleDir* pArchive = (SMagicMgrArchive_SingleDir*)buf;

	if(pArchive->pRun == NULL)
		return FALSE;

	// 运行魔法
	m_pRun = new CMagicSingleDir;
	{
		POINT ptAttackCenter, ptStartCenter;
		Tile_To_Screen(pArchive->pRun->ptStartTile, ptStartCenter);
		Tile_To_Screen(pArchive->pRun->ptAttackTile, ptAttackCenter);
		ptStartCenter.x += pArchive->pRun->ptTileOffset.x;
		ptStartCenter.y += pArchive->pRun->ptTileOffset.y;
		pArchive->pRun->fDir = Fun_CalcDirection32(&ptStartCenter, &ptAttackCenter);
		pArchive->pRun->fDirInc = 32;
	}

	if (!m_pRun->OnCreateItem((const char*)pArchive->pRun, sizeof(SMagicArchive_SingleDir)))
	{
		SAFE_RELEASE(m_pRun);
		return FALSE;
	}
	m_pRun->SetMgr(this);

	// 爆炸魔法
	if (pArchive->pExplode != NULL)
	{
		m_pExplode = new CMagicGeneral;
		if (!m_pExplode->OnCreateItem((const char*)pArchive->pExplode, sizeof(SMagicArchive_General)))
		{
			SAFE_RELEASE(m_pExplode);

			if (m_pRun)
				SAFE_RELEASE(m_pRun);

			return FALSE;
		}
		m_pExplode->SetMgr(this);
	}

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY,
		static_cast<ITimerSink*>(this),
		GetAttackDelay(),
		"BasicSys!!CMagicMgrSingleDir::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrSingleDir::OnPlayDone(void* pThis, POINT Pt)
{
	if (m_pRun)
	{
		BeginDamage();

		m_pRun = NULL;

		if (!m_pExplode ||
			!m_pExplode->AddToMapView(Pt) || 
			!m_pExplode->AddToTimerAxis())
			return;

		m_pExplode->m_ptTile = Pt;
	}
	else
	{
		if (m_pExplode)
		{ 
			g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY,
				static_cast<ITimerSink*>(this),
				MAGIC_MGR_DESTROY_TIME,
				"BasicSys!!CMagicMgrSingleDir::OnPlayDone");
		}
	}
}
//------------------------------------------------------------------------
void CMagicMgrSingleDir::OnTimer(DWORD dwEventID)
{
	if(dwEventID == TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
		SetBeginAttack();
		m_pRun->AddToMapView(m_pRun->m_ptTile);
		m_pRun->AddToTimerAxis();
	}
	else
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
		delete this;
	}
}
//------------------------------------------------------------------------
void CMagicMgrSingleDir::BeginAttack()
{
	if(!IsBeginAttack())
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY);
}
//------------------------------------------------------------------------