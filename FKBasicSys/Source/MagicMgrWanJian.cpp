/**
*	created:		2012-7-2   3:29
*	filename: 		MagicMgrWanJian
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrWanJian.h"
#include "../Include/CreatureView.h"
#include "../Include/StaticObj.h"
#include "../../FKGame/MapInfo/Tile.h"
#include <math.h>
//------------------------------------------------------------------------
CMagicMgrWanJian::CMagicMgrWanJian()
{
	m_nDir		= 0;
	m_bStop		= FALSE;
	m_nDelay	= 0;
	m_nLoopTime = -1;
}
//------------------------------------------------------------------------
CMagicMgrWanJian::~CMagicMgrWanJian()
{
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrWanJian::Create(void* buf, int nSize)
{
	SMagicMgrArchive_WanJian* pArchive = (SMagicMgrArchive_WanJian*)buf;

	if(pArchive->nDelay < 1)
		return FALSE;

	m_nLoopTime = pArchive->nLoopTime;
	m_nDelay = pArchive->nDelay;
	m_ptTile = pArchive->ptTile;

	m_nDir = m_pMasterView->GetDirection();

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY,
		static_cast<ITimerSink*>(this),
		GetAttackDelay(),
		"BasicSys!!CMagicMgrWanJian::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrWanJian::OnPlayDone(void* pThis, POINT Pt)
{
}
//------------------------------------------------------------------------
void CMagicMgrWanJian::OnTimer(DWORD dwEventID)
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
				"BasicSys!!CMagicMgrWanJian::OnTimer");

			if(m_nLoopTime > 0)
			{
				g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP,
					static_cast<ITimerSink*>(this),
					m_nLoopTime,
					"BasicSys!!CMagicMgrWanJian::OnTimer");
			}

			OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE);
		}
		break;

	case TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE:		// 粒子间隔
		{
			if(m_pMasterView == NULL)
			{
				return;
			}

			CStaticObjActEx *obj = new CStaticObjActEx();
			SStaticObjActExArchive archive;
			if(m_pMasterView->GetWeaponStyle() == ActWeaponStyleDouble)
			{
				int StateList[3] = {CrtStateJuqi2, CrtStateDoubleAttack1, CrtStateDoubleAttack2};
				archive.nState = StateList[rand() % 3];
			}
			else
			{
				int StateList[3] = {CrtStateJuqi2, CrtStateSingleAttack1, CrtStateSingleAttack2};
				archive.nState = StateList[rand() % 3];
			}

			archive.nDir = m_nDir;
			if(!obj->CreateItem(m_pMasterView, &archive))
			{
				g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
				return;
			}
			POINT ptTile;
			ptTile.x = m_ptTile.x + 2 - rand() % 5;
			ptTile.y = m_ptTile.y + 2 - rand() % 5;
			if(abs(ptTile.x - m_ptTile.x) == 2 && abs(ptTile.y - m_ptTile.y) == 2)
			{
				ptTile = m_ptTile;
			}
			obj->AddToMapView(ptTile);
			obj->AddToTimerAxis();
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
void CMagicMgrWanJian::BeginAttack()
{
	if(!IsBeginAttack())
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY);
}
//------------------------------------------------------------------------
void CMagicMgrWanJian::Stop()
{
	m_bStop = TRUE;
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_LOOP, static_cast<ITimerSink *>(this));

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY,
		static_cast<ITimerSink*>(this),
		MAGIC_MGR_DESTROY_TIME,
		"BasicSys!!CMagicMgrWanJian::Stop");

	return;
}
//------------------------------------------------------------------------
BOOL CMagicMgrWanJian::IsRunning()
{
	if(m_nLoopTime > 0)
		return FALSE;

	return !m_bStop;
}
//------------------------------------------------------------------------