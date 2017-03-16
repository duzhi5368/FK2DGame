/**
*	created:		2012-7-2   3:14
*	filename: 		MagicMgrCirculDir
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrCirculDir.h"
#include "../Include/CreatureView.h"
#include "../Include/StaticObj.h"
//------------------------------------------------------------------------
CMagicMgrCirculDir::CMagicMgrCirculDir()
{
	m_nDirNum = 0;
	m_nCurDestroyCount = 0;
	m_paRun	= NULL;
	m_bRealTime = FALSE;
}
//------------------------------------------------------------------------
CMagicMgrCirculDir::~CMagicMgrCirculDir()
{
	SAFE_DELETE_ARRAY(m_paRun);
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrCirculDir::Create(void* buf, int nSize)
{
	SMagicMgrArchive_CirculDir* pArchive = (SMagicMgrArchive_CirculDir*)buf;

	if (pArchive->pRun == NULL)
	{
		return FALSE;
	}

	if(m_pMasterView == NULL)
		return FALSE;

	if(pArchive->nDirNum <= 0)
		return FALSE;

	m_nDirNum = pArchive->nDirNum;
	m_ptTile = pArchive->ptTile;

	// ÔËÐÐÄ§·¨
	m_paRun = new CMagicEightDir*[m_nDirNum];
	int nDir = m_pMasterView->GetDirection() - (m_nDirNum / 2);
	if(nDir < 0)
		nDir += 8;
	for(int i = 0; i < m_nDirNum; i++)
	{
		m_paRun[i] = new CMagicEightDir;

		int nDirTemp = nDir + i;
		if(nDirTemp >= 8)
			nDirTemp -= 8;
		pArchive->pRun->nDir = nDirTemp;
		if(!m_paRun[i]->OnCreateItem((const char*)pArchive->pRun, sizeof(SMagicArchive_EightDir)))
		{
			for(int k = 0; k <= i; k++)
				SAFE_RELEASE(m_paRun[k]);
			return FALSE;
		}
	}

	for (int i = 0; i < m_nDirNum; i++)
		m_paRun[i]->SetMgr(this);

	m_bRealTime = pArchive->bRealTime;
	if(m_bRealTime)
	{
		m_nCurDestroyCount = 0;
		for(int i = 0; i < m_nDirNum; i++) 
		{
			m_paRun[i]->AddToMapView(m_ptTile);
			m_paRun[i]->AddToTimerAxis();
		}
	}

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY,
		static_cast<ITimerSink*>(this),
		GetAttackDelay(),
		"BasicSys!!CMagicMgrCirculDir::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrCirculDir::OnPlayDone(void* pThis, POINT Pt)
{
	if(++m_nCurDestroyCount >= m_nDirNum)
	{
		g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY,
			static_cast<ITimerSink*>(this),
			MAGIC_MGR_DESTROY_TIME,
			"BasicSys!!CMagicMgrCirculDir::OnPlayDone");
	}
}
//------------------------------------------------------------------------
void CMagicMgrCirculDir::OnTimer(DWORD dwEventID)
{
	if(dwEventID == TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
		SetBeginAttack();
		BeginDamage();

		if(!m_bRealTime)
		{
			m_nCurDestroyCount = 0;
			for(int i = 0; i < m_nDirNum; i++) 
			{
				m_paRun[i]->AddToMapView(m_ptTile);
				m_paRun[i]->AddToTimerAxis();
			}
		}
	}
	else
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
		delete this;
	}

}
//------------------------------------------------------------------------
void CMagicMgrCirculDir::BeginAttack()
{
	if(!IsBeginAttack())
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY);
}
//------------------------------------------------------------------------