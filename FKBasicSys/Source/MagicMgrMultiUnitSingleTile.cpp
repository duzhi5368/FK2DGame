/**
*	created:		2012-7-2   3:26
*	filename: 		MagicMgrMultiUnitSingleTile
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrMultiUnitSingleTile.h"
#include "../Include/CreatureView.h"
//------------------------------------------------------------------------
CMagicMgrMultiUnitSingleTile::CMagicMgrMultiUnitSingleTile()
{
	m_paUnit			= NULL;
	m_nUnitCount		= 0;
	m_nCurCreateCount	= 0;
	m_nCurDestroyCount	= 0;
	m_nDelay			= 0;
}
//------------------------------------------------------------------------
CMagicMgrMultiUnitSingleTile::~CMagicMgrMultiUnitSingleTile()
{
	SAFE_DELETE_ARRAY(m_paUnit);
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrMultiUnitSingleTile::Create(void* buf, int nSize)
{
	SMagicMgrArchive_MultiUnitSingleTile* pArchive = (SMagicMgrArchive_MultiUnitSingleTile*)buf;
	if(pArchive->pUnit == NULL)
		return FALSE;

	// 创建雨中的单元
	m_nDelay = pArchive->nDelay; // 创建间隙
	m_nUnitCount = pArchive->nTime;
	m_nCurCreateCount = 0;
	if(m_pAttackView != NULL)
		m_ptAttackTile = m_pAttackView->GetTile();
	else
		m_ptAttackTile = pArchive->ptAttackTile;

	m_paUnit = new CMagicRain*[m_nUnitCount];
	for (int j = 0; j < m_nUnitCount; j++)
	{
		m_paUnit[j] = new CMagicRain;
		if (!m_paUnit[j]->OnCreateItem((const char*)pArchive->pUnit, sizeof(SMagicArchive_Rain)) && j < 50)
		{
			for(int k = 0; k <= j; k++)
				SAFE_RELEASE(m_paUnit[k]);
			return FALSE;
		}

		m_paUnit[j]->m_ptTile = m_ptAttackTile;
	}

	// 如果创建成功为每个魔法元素设置管理器
	for (int i = 0; i < m_nUnitCount; i++)
		m_paUnit[i]->SetMgr(this);

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY,
		static_cast<ITimerSink*>(this),
		GetAttackDelay(),
		"BasicSys!!CMagicMgrMultiUnitSingleTile::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrMultiUnitSingleTile::OnTimer(DWORD dwEventID)
{
	if(dwEventID == TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
		SetBeginAttack();

		if(m_pAttackView != NULL)
			m_ptAttackTile = m_pAttackView->GetTile();
		m_paUnit[0]->AddToMapView(m_ptAttackTile);
		m_paUnit[0]->AddToTimerAxis();
		m_nCurCreateCount = 1;
		m_nFlowDamNum = 0;

		g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE,
			static_cast<ITimerSink*>(this),
			m_nDelay,
			"BasicSys!!CMagicMgrMultiUnitSingleTile::OnTimer");
	}
	else if(dwEventID == TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE)
	{
		if (m_nCurCreateCount < m_nUnitCount)
		{
			if(m_pAttackView != NULL)
				m_ptAttackTile = m_pAttackView->GetTile();

			m_paUnit[m_nCurCreateCount]->AddToMapView(m_ptAttackTile);
			m_paUnit[m_nCurCreateCount]->AddToTimerAxis();
			m_nCurCreateCount++;
		}
		else
			g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
	}
	else
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
		delete this;
	}
}
//------------------------------------------------------------------------
void CMagicMgrMultiUnitSingleTile::OnPlayExplode(void* pThis)
{
	m_nFlowDamNum++;
	if(m_nFlowDamNum == m_nUnitCount)
	{
		BeginDamage();
	}
	else
	{
		FlowDamage(m_nFlowDamNum - GetFlowDamageNum());
	}
}
//------------------------------------------------------------------------
void CMagicMgrMultiUnitSingleTile::OnPlayDone(void* pThis, POINT Pt)
{
	m_paUnit[m_nCurDestroyCount] = NULL;
	if(++m_nCurDestroyCount >= m_nUnitCount)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));

		g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY,
			static_cast<ITimerSink*>(this),
			MAGIC_MGR_DESTROY_TIME,
			"BasicSys!!CMagicMgrMultiUnitSingleTile::OnPlayDone");

		return;
	}
}
//------------------------------------------------------------------------
void CMagicMgrMultiUnitSingleTile::BeginAttack()
{
	if(!IsBeginAttack())
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY);
}
//------------------------------------------------------------------------