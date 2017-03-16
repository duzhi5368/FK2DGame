/**
*	created:		2012-7-2   3:25
*	filename: 		MagicMgrMultiUnit
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrMultiUnit.h"
#include "../Include/CreatureView.h"
//------------------------------------------------------------------------
CMagicMgrMultiUnit::CMagicMgrMultiUnit()
{
	m_bStop		= FALSE;
	m_nDelay	= 0;
	m_nLoopTime = -1;
}
//------------------------------------------------------------------------
CMagicMgrMultiUnit::~CMagicMgrMultiUnit()
{
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrMultiUnit::Create(void* buf, int nSize)
{
	SMagicMgrArchive_MultiUnit* pArchive = (SMagicMgrArchive_MultiUnit*)buf;
	if(pArchive->nWidth > 20 || pArchive->nWidth < 1)
		return FALSE;

	if(pArchive->pUnit == NULL)
		return FALSE;

	if(pArchive->nDelay < 1)
		return FALSE;

	memcpy(&m_pUnit, pArchive->pUnit, sizeof(SMagicArchive_Rain));

	m_nLoopTime = pArchive->nLoopTime;
	m_nDelay = pArchive->nDelay; // 创建间隙
	m_nWidth = pArchive->nWidth;
	int nHalfCount = pArchive->nWidth >> 1;
	m_ptAttackLT.x = pArchive->ptAttackTile.x - nHalfCount;
	m_ptAttackLT.y = pArchive->ptAttackTile.y - nHalfCount;

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY,
		static_cast<ITimerSink*>(this),
		GetAttackDelay(),
		"BasicSys!!CMagicMgrMultiUnit::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrMultiUnit::OnTimer(DWORD dwEventID)
{
	switch(dwEventID)
	{
	case TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY:		// 攻击延迟
		{
			g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
			SetBeginAttack();
			BeginDamage();

			g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE,
				static_cast<ITimerSink*>(this),
				m_nDelay,
				"BasicSys!!CMagicMgrMultiUnit::OnTimer");

			if(m_nLoopTime > 0)
			{
				g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP,
					static_cast<ITimerSink*>(this),
					m_nLoopTime,
					"BasicSys!!CMagicMgrMultiUnit::OnTimer");
			}

			OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE);
		}
		break;

	case TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE:		// 粒子间隔
		{
			POINT ptTile;
			ptTile.x = m_ptAttackLT.x + rand() % m_nWidth;
			ptTile.y = m_ptAttackLT.y + rand() % m_nWidth;

			CTile* pTile = g_pMapView->GetTile(ptTile);

			int nCount = pTile->GetLayerCount();
			for(int i = 0; i < nCount; i++)
			{
				CMapItem* pItem = pTile->GetLayer(i);
				if(pItem->GetClassID() == miMagicObj)
				{
					return;
				}
			}

			CMagicRain *pRain = new CMagicRain;
			if (!pRain->OnCreateItem((const char*)&m_pUnit, sizeof(SMagicArchive_Rain)))
			{
				pRain->Release();
				g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
				return;
			}

			pRain->m_ptTile = ptTile;
			pRain->SetMgr(this);

			pRain->AddToMapView(pRain->m_ptTile);
			pRain->AddToTimerAxis();
		}
		break;

	case TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP:		// 循环结束时间
		{
			Stop();
			return;
		}
		break;

	case TIME_EVENT_ID_MAGIC_MGR_DESTROY:			// 销毁时间
		{
			delete this;
			return;
		}
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------
void CMagicMgrMultiUnit::BeginAttack()
{
	if(!IsBeginAttack())
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY);
}
//------------------------------------------------------------------------
void CMagicMgrMultiUnit::OnPlayDone(void* pThis, POINT Pt)
{
}
//------------------------------------------------------------------------
void CMagicMgrMultiUnit::Stop()
{
	m_bStop = TRUE;
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP, static_cast<ITimerSink *>(this));

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY,
		static_cast<ITimerSink*>(this),
		MAGIC_MGR_DESTROY_TIME,
		"BasicSys!!CMagicMgrMultiUnit::Stop");

	return;
}
//------------------------------------------------------------------------
BOOL CMagicMgrMultiUnit::IsRunning()
{
	if(m_nLoopTime > 0)
		return FALSE;

	return !m_bStop;
}
//------------------------------------------------------------------------