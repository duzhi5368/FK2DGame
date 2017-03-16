/**
*	created:		2012-7-2   3:23
*	filename: 		MagicMgrMultiDir
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrMultiDir.h"
#include "../Include/CreatureView.h"
#include "../../FKGame/MapInfo/Tile.h"
#include <math.h>
//------------------------------------------------------------------------
CMagicMgrMultiDirEx::CMagicMgrMultiDirEx()
{
	m_paUnit	= NULL;
	m_paLeftObj	= NULL;
	m_nCurCreateCount	= 0;
	m_nCurDestroyCount	= 0;
	m_nDelay			= 0;
	m_nExplodeNum		= 0;
}
//------------------------------------------------------------------------
CMagicMgrMultiDirEx::~CMagicMgrMultiDirEx()
{
	SAFE_DELETE_ARRAY(m_paUnit);
	SAFE_DELETE_ARRAY(m_paLeftObj);
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrMultiDirEx::Create(void* buf, int nSize)
{
	SMagicMgrArchive_MultiDirEx* pArchive = (SMagicMgrArchive_MultiDirEx*)buf;
	if(pArchive->nWidth > 10 || pArchive->nWidth < 1)
		return FALSE;

	if(pArchive->pRun == NULL)
		return FALSE;

	if(pArchive->nDelay < 1)
		return FALSE;

	// 先判断鼠标点击处的TILE是否合法
	if (_test_tile_spread(pArchive->ptAttackTile) != 1)
		return FALSE;

	// 创建雨中的单元
	m_nDelay = pArchive->nDelay; // 创建间隙

	m_nWidth = pArchive->nWidth;
	int nHalfCount = pArchive->nWidth >> 1;
	m_ptAttackLT.x = pArchive->ptAttackTile.x - nHalfCount;
	m_ptAttackLT.y = pArchive->ptAttackTile.y - nHalfCount;

	// 计算每个飞行对象的方向
	int nDirs[MAX_DIR_COUNT];

	POINT ptAttackCenter, ptStartCenter;
	Tile_To_Screen(pArchive->pRun->ptStartTile, ptStartCenter);
	Tile_To_Screen(pArchive->ptAttackTile, ptAttackCenter);

	int nDir = Fun_CalcDirection32_45(&ptStartCenter, &ptAttackCenter);
	nDir += 16;

	float fDirInc = 32 / MAX_DIR_COUNT;
	for(int i = 0; i < MAX_DIR_COUNT; i++) 
	{
		nDirs[i] = nDir + (int)(((i % 2 == 0)? 1 : -1) * fDirInc * i / 2);
		if(nDirs[i] >= 32)
			nDirs[i] -= 32;
		else if(nDirs[i] < 0)
			nDirs[i] += 32;
		nDirs[i] = nDirs[i] % 32;
	}

	// NEED CHECK
	int dx = ptAttackCenter.x - ptStartCenter.x;
	int dy = ptAttackCenter.y - ptStartCenter.y;
	float fLength = (float)(sqrt( static_cast<double>(dy * dy + dx * dx) ) );

	// 固定有效TILE中创建雨单元(如果只有一个TILE有效，则也创建一个)
	m_paUnit = new CMagicSingleDir*[MAX_DIR_COUNT];
	for (int i = 0; i < MAX_DIR_COUNT; i++)
	{
		m_paUnit[i] = new CMagicSingleDir;

		pArchive->pRun->fDir = rand() % 32;//nDirs[MAX_DIR_COUNT - i - 1];
		pArchive->pRun->fDirInc = 1;//fDirInc;

		pArchive->pRun->ptAttackTile.x = m_ptAttackLT.x + rand() % m_nWidth;
		pArchive->pRun->ptAttackTile.y = m_ptAttackLT.y + rand() % m_nWidth;

		if (!m_paUnit[i]->OnCreateItem((const char*)pArchive->pRun, sizeof(SMagicArchive_SingleDir)))
		{
			for(int k = 0; k <= i; k++)
				SAFE_RELEASE(m_paUnit[k]);
			return FALSE;
		}
	}

	if (pArchive->pExplode != NULL)
	{
		// 爆炸效果
		m_paLeftObj = new CMagicGeneral*[MAX_DIR_COUNT];
		for (int i = 0; i < MAX_DIR_COUNT; i++)
		{
			m_paLeftObj[i] = new CMagicGeneral;
			if (!m_paLeftObj[i]->OnCreateItem((const char *)pArchive->pExplode,	sizeof(SMagicArchive_General)))
			{
				int k;
				for(k = 0; k < MAX_DIR_COUNT; k++)
					SAFE_RELEASE(m_paUnit[k]);
				for(k = 0; k <= i; k++)
					SAFE_RELEASE(m_paLeftObj[k]);
				return FALSE;
			}
		}
	}

	for (int i = 0; i < MAX_DIR_COUNT; i++)
		m_paUnit[i]->SetMgr(this);

	if(m_paLeftObj != NULL)
		for ( int i = 0; i < MAX_DIR_COUNT; i++)
			m_paLeftObj[i]->SetMgr(this);

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY,
		static_cast<ITimerSink*>(this),
		GetAttackDelay(),
		"BasicSys!!CMagicMgrMultiDirEx::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrMultiDirEx::OnPlayDone(void* pThis, POINT Pt)
{
	for(int i = 0; i < MAX_DIR_COUNT; i++)
	{
		if (m_paUnit[i] == pThis)
		{
			if(m_nExplodeNum++ > MAX_DIR_COUNT / 2)
				BeginDamage();

			m_paUnit[i] = NULL;

			if (NULL != m_paLeftObj)
			{
				m_paLeftObj[i]->AddToMapView(Pt);
				m_paLeftObj[i]->AddToTimerAxis();
			}
			break;
		}
		if (m_paLeftObj[i] == pThis)
		{
			m_paLeftObj[i] = NULL;
			if(++m_nCurDestroyCount >= MAX_DIR_COUNT)
			{
				g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY,
					static_cast<ITimerSink*>(this),
					MAGIC_MGR_DESTROY_TIME,
					"BasicSys!!CMagicMgrMultiDirEx::OnPlayDone");
				return;
			}
		}
	}
}
//------------------------------------------------------------------------
void CMagicMgrMultiDirEx::OnTimer(DWORD dwEventID)
{
	if(dwEventID == TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY, static_cast<ITimerSink *>(this));
		SetBeginAttack();
		m_paUnit[m_nCurCreateCount]->AddToMapView(m_paUnit[m_nCurCreateCount]->m_ptTile);
		m_paUnit[m_nCurCreateCount]->AddToTimerAxis();
		m_nCurCreateCount++;
		m_paUnit[m_nCurCreateCount]->AddToMapView(m_paUnit[m_nCurCreateCount]->m_ptTile);
		m_paUnit[m_nCurCreateCount]->AddToTimerAxis();
		m_nCurCreateCount++;
		g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE,
			static_cast<ITimerSink*>(this),
			m_nDelay,
			"BasicSys!!CMagicMgrMultiDirEx::OnTimer");
	}
	else if(dwEventID == TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE)
	{
		if (m_nCurCreateCount < MAX_DIR_COUNT)
		{
			m_paUnit[m_nCurCreateCount]->AddToMapView(m_paUnit[m_nCurCreateCount]->m_ptTile);
			m_paUnit[m_nCurCreateCount]->AddToTimerAxis();
			m_nCurCreateCount++;
		}
		else
		{
			g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
			return;
		}

		if (m_nCurCreateCount < MAX_DIR_COUNT)
		{
			m_paUnit[m_nCurCreateCount]->AddToMapView(m_paUnit[m_nCurCreateCount]->m_ptTile);
			m_paUnit[m_nCurCreateCount]->AddToTimerAxis();
			m_nCurCreateCount++;
		}
		else
		{
			g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
		}
	}
	else
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_DESTROY, static_cast<ITimerSink *>(this));
		delete this;
	}
}
//------------------------------------------------------------------------
void CMagicMgrMultiDirEx::BeginAttack()
{
	if(!IsBeginAttack())
		OnTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_DELAY);
}
//------------------------------------------------------------------------