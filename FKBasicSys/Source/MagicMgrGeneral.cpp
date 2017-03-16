/**
*	created:		2012-7-2   3:21
*	filename: 		MagicMgrGeneral
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrGeneral.h"
#include "../Include/CreatureView.h"
//------------------------------------------------------------------------
CMagicMgrGeneral::CMagicMgrGeneral()
{
	m_pRun			= NULL;
	m_pExplode		= NULL;
	m_nLoopTime		= 0;
	m_bIsRunning	= FALSE;
	m_bCanCtrl		= FALSE;
	m_bBegin		= FALSE;
	m_bOnceAttack	= FALSE;
}
//------------------------------------------------------------------------
CMagicMgrGeneral::~CMagicMgrGeneral()
{
	if(m_pRun)
	{
		m_pRun->SetMgr(NULL);
		m_pRun->Release();
	}
	if(m_pExplode)
	{
		m_pExplode->SetMgr(NULL);
		m_pExplode->Release();
	}

	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrGeneral::Create(void* buf, int nSize)
{
	SMagicMgrArchive_General* pArchive = (SMagicMgrArchive_General*)buf;

	m_bOnceAttack = pArchive->bOnceAttack;
	// 运行态魔法
	if (pArchive->pRun != NULL)
	{
		m_pRun = new CMagicGeneral;
		if (!m_pRun->OnCreateItem((const char*)pArchive->pRun, sizeof(SMagicArchive_General)))
		{
			SAFE_RELEASE(m_pRun);
			return FALSE;
		}
		m_pRun->m_ptTile = pArchive->ptAttackTile;
		m_nLoopTime = pArchive->nLoopTime;
		m_bCanCtrl = pArchive->bCanCtrl;

		if(m_bCanCtrl)
			m_bIsRunning = TRUE;

		if(m_nLoopTime > 0)
		{
			m_pRun->SetLoop(TRUE);
		}
	}

	// 爆炸态魔法
	if (pArchive->pExplode != NULL)
	{
		m_pExplode = new CMagicGeneral;
		if (!m_pExplode->OnCreateItem((const char*)pArchive->pExplode, sizeof(SMagicArchive_General)))
		{
			SAFE_RELEASE(m_pRun);
			SAFE_RELEASE(m_pExplode);
			return FALSE;
		}
		m_pExplode->m_ptTile = pArchive->ptAttackTile;
	}

	if(m_pRun != NULL)
		m_pRun->SetMgr(this);

	if(m_pExplode != NULL)
		m_pExplode->SetMgr(this);

	if(pArchive->bNeedAttackDelay)
	{
		g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY,
			static_cast<ITimerSink*>(this),
			GetAttackDelay(),
			"BasicSys!!CMagicMgrGeneral::Create");
	}
	else
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY);
	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrGeneral::OnPlayDone(void* pThis, POINT Pt)
{
	if (m_pRun)
	{
		m_pRun = NULL;
		m_bIsRunning = FALSE;
		if (m_pExplode)
		{
			BeginDamage();
			if(m_pAttackView != NULL)
			{
				m_pExplode->m_ptTile = m_pAttackView->GetTile();
			}
			m_pExplode->AddToMapView(m_pExplode->m_ptTile);
			m_pExplode->AddToTimerAxis();
		}
		else
		{
			g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY,
				static_cast<ITimerSink*>(this),
				MAGIC_MGR_DESTROY_TIME,
				"BasicSys!!CMagicMgrGeneral::OnPlayDone");

			return;
		}
	}
	else
	{
		m_pExplode = NULL;
		g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY,
			static_cast<ITimerSink*>(this),
			MAGIC_MGR_DESTROY_TIME,
			"BasicSys!!CMagicMgrGeneral::OnPlayDone");

		return;
	}
}
//------------------------------------------------------------------------
void CMagicMgrGeneral::OnTimer(DWORD dwEventID)
{
	if(dwEventID == TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
		m_bBegin = TRUE;

		if(m_pRun)
		{
			if(m_pAttackView != NULL)
			{
				m_pRun->m_ptTile = m_pAttackView->GetTile();
			}
			m_pRun->AddToMapView(m_pRun->m_ptTile);
			m_pRun->AddToTimerAxis();

			if(!m_bOnceAttack)
				BeginDamage();

			if(m_nLoopTime > 0)
				g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP,
				static_cast<ITimerSink*>(this),
				m_nLoopTime,
				"BasicSys!!CMagicMgrGeneral::OnTimer");
		}
		else if(m_pExplode)
		{
			BeginDamage();
			if(m_pAttackView != NULL)
			{
				m_pExplode->m_ptTile = m_pAttackView->GetTile();
			}
			m_pExplode->AddToMapView(m_pExplode->m_ptTile);
			m_pExplode->AddToTimerAxis();
		}
		else
		{
			BeginDamage();
			delete this;
		}
	}
	else if(dwEventID == TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP, static_cast<ITimerSink *>(this));
		if(m_pRun)
			m_pRun->SetLoop(FALSE);
	}
	else
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
		delete this;
	}
}
//------------------------------------------------------------------------
void CMagicMgrGeneral::BeginAttack()
{
	if(!IsBeginAttack())
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY);
}
//------------------------------------------------------------------------
void CMagicMgrGeneral::Stop()
{
	if(!m_bIsRunning)
		return;

	if(!m_bBegin)
	{
		delete this;
		return;
	}
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP, static_cast<ITimerSink *>(this));
	m_bIsRunning = FALSE;
	if(m_pRun)
		m_pRun->SetLoop(FALSE);

	return;
}
//------------------------------------------------------------------------
BOOL CMagicMgrGeneral::IsRunning()
{
	return m_bIsRunning;
}
//------------------------------------------------------------------------
void CMagicMgrGeneral::AddDamage(CCreatureView *pView, SFlowParam FlowParam)
{
	CMagicMgrBase::AddDamage(pView, FlowParam);
	if(m_bOnceAttack && !IsBeginAttack())
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP);
}
//------------------------------------------------------------------------