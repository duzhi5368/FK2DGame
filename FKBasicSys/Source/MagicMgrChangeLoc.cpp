/**
*	created:		2012-7-2   3:13
*	filename: 		MagicMgrChangeLoc
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/MagicMgrChangeLoc.h"
#include "../Include/CreatureView.h"
#include "../Include/StaticObj.h"
//------------------------------------------------------------------------
CMagicMgrChangeLoc::CMagicMgrChangeLoc()
{
	m_nCurNodeIndex = 0;
}
//------------------------------------------------------------------------
CMagicMgrChangeLoc::~CMagicMgrChangeLoc()
{
	if(m_pMasterView != NULL)
		m_pMasterView->DelLinkView(&m_pMasterView);

	g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
}
//------------------------------------------------------------------------
BOOL CMagicMgrChangeLoc::Create(POINT pt, CCreatureView *pMaster)
{
	if(pMaster == NULL)
		return FALSE;

	m_pMasterView = pMaster;
	m_pMasterView->AddLinkView(&m_pMasterView);

	POINT ptEndCenter, ptStartCenter;
	Tile_To_Screen(m_pMasterView->GetTile(), ptStartCenter);
	Tile_To_Screen(pt, ptEndCenter);

	ptStartCenter.x += m_pMasterView->m_ptTileOffset.x;
	ptStartCenter.y += m_pMasterView->m_ptTileOffset.y;

	if(pt.x != m_pMasterView->GetTile().x || pt.y != m_pMasterView->GetTile().y)
	{
		int nDir = Fun_CalcDirection8(&ptStartCenter, &ptEndCenter);
		m_pMasterView->SetDirection(nDir);
	}

	float IncX = (float)(ptEndCenter.x - ptStartCenter.x) / SHANDOW_STEP;
	float IncY = (float)(ptEndCenter.y - ptStartCenter.y) / SHANDOW_STEP;
	float TempX = ptStartCenter.x;
	float TempY = ptStartCenter.y;
	POINT ptTemp;
	POINT ptOffsetTile;
	for(int i = 0; i < SHANDOW_STEP; i++)
	{
		TempX += IncX;
		TempY += IncY;
		ptTemp.x = TempX;
		ptTemp.y = TempY;
		Screen_To_Tile(ptTemp, m_pPathList[i].ptTile);
		Tile_To_Screen(m_pPathList[i].ptTile, ptOffsetTile);
		m_pPathList[i].ptOffsetTile.x = TempX - ptOffsetTile.x;
		m_pPathList[i].ptOffsetTile.y = TempY - ptOffsetTile.y;
	}

	// 取消生物的占位
	g_pMapView->RemoveItem(m_pMasterView->GetTile(), m_pMasterView, 2);
	m_pMasterView->SetOccupant(FALSE);
	g_pMapView->AddItem(m_pMasterView->GetTile(), m_pMasterView, 2);

	g_pTimeAxis->SetTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE,
		static_cast<ITimerSink*>(this),
		15,
		"BasicSys!!CMagicMgrChangeLoc::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CMagicMgrChangeLoc::OnTimer(DWORD dwEventID)
{	
	if(m_pMasterView == NULL)
	{
		g_pTimeAxis->KillTimer(TIME_EVENT_ID_MAGIC_MGR_ATTACK_SPACE, static_cast<ITimerSink *>(this));
		return;
	}

	if(m_nCurNodeIndex > SHANDOW_NUM)
	{
		m_pMasterView->SetOccupant(TRUE);
		delete this;
		return;
	}

	CStaticObjEx *obj = new CStaticObjEx();
	if(!obj->CreateItem(m_pMasterView))
	{
		m_pMasterView->SetOccupant(TRUE);
		delete this;
		return;
	}
	obj->AddToMapView(obj->m_ptTile);
	obj->AddToTimerAxis();

	if(++m_nCurNodeIndex < SHANDOW_STEP)
	{
		if (g_pMapView->MoveItem(m_pMasterView->GetTile(), m_pPathList[m_nCurNodeIndex].ptTile, m_pMasterView))
		{
			m_pMasterView->SetTile(m_pPathList[m_nCurNodeIndex].ptTile, g_pMapView);
			m_pMasterView->m_ptTileOffset = m_pPathList[m_nCurNodeIndex].ptOffsetTile;
		}
	}
}
//------------------------------------------------------------------------