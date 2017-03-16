/**
*	created:		2012-7-1   5:18
*	filename: 		TilesView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/MapView.h"
#include "../Include/TilesView.h"
#include "../../FKGame/SceneObj/IItemClassFactory.h"
#include "../Include/MapViewCommon.h"
//------------------------------------------------------------------------
#define MAX_GRID_SNAP_ITEM_COUNT	1024
//------------------------------------------------------------------------
#ifdef _DEBUG
int g_nAddCount = 0;
int g_nDelCount = 0;
#endif
//------------------------------------------------------------------------
CTilesView::CTilesView()
{
	m_pMapView = NULL;
	m_ObjList.Clear();
}
//------------------------------------------------------------------------
CTilesView::~CTilesView()
{
}
//------------------------------------------------------------------------
// 对于服务器，如果希望创建整个地图的CTile数组，则可以将View尺寸设置成地图尺寸
// 即：nViewWidth=nMapWidth, nViewHeight=nMapHeight
BOOL CTilesView::Create(int nMapWidth, int nMapHeight, POINT& ptPlayerPos, CMapView* pMapView, BOOL bUseDynamicMap)
{
	ASSERT(pMapView);
	m_pMapView = pMapView;

	// 将预读区域进行校验并进行网格对齐
	if (bUseDynamicMap)
	{
	}	
	else
	{
		// 下面的做法是临时解决较小地图加载的问题。
		if (nMapWidth <= 800*20 && nMapHeight <= 600*20)
		{
			return m_GridMgr.Create(static_cast<IScrollSheetSink*>(this), 
				nMapWidth, nMapHeight, 
				ptPlayerPos, bUseDynamicMap);		
		}
	}

	m_GridMgr.Create(static_cast<IScrollSheetSink*>(this), 
		nMapWidth, nMapHeight, ptPlayerPos, bUseDynamicMap);

	return TRUE;
}
//------------------------------------------------------------------------
void CTilesView::Close()
{
	m_ObjList.Clear();
	m_GridMgr.Close();
}
//------------------------------------------------------------------------
BOOL CTilesView::SizeViewport(int nWidth,int nHeight)
{
	return m_GridMgr.SizeViewport(nWidth, nHeight);
}
//------------------------------------------------------------------------
BOOL CTilesView::ScrollToCenter(POINT& ptPlayerPos)
{
	return m_GridMgr.ScrollToCenter(ptPlayerPos);
}
//------------------------------------------------------------------------
void CTilesView::OnGridChanged(int nMoveDirection, int nDx, int nDy)	
{
	RECT rcPreRead;
	POINT ptTileOrigin;
	m_GridMgr.GetGridRectBelowPreRead(rcPreRead);
	m_pMapView->GetMapCoordinate().PixelToTile((const POINT&)rcPreRead, ptTileOrigin);
	ptTileOrigin.y -= (rcPreRead.bottom - rcPreRead.top)/32 - 1;

	BLOCK_INFO BlockInfo[TILE_GRID_ROW*TILE_GRID_COL*4];
	int nRows, nCols;
	m_GridMgr.SnapshotLeftTop(nRows, nCols, BlockInfo, nMoveDirection);
	RECT& rc = m_GridMgr.GetPreReadGrid();
	TraceLn("预读:[%d,%d]-(%d,%d)   朝向:%d,dx=%d,dy=%d   起点:%d,%d",rc.left,rc.top,rc.right,rc.bottom, nMoveDirection,nDx,nDy,ptTileOrigin.x,ptTileOrigin.y);
	RECT& rc1 = m_GridMgr.GetViewGrid();
	POINT ptPlayerGrid = m_GridMgr.GetPlayerGrid();
	TraceLn("可视:[%d,%d]-(%d,%d)   玩家:[%d,%d]",rc1.left,rc1.top,rc1.right,rc1.bottom, ptPlayerGrid.x, ptPlayerGrid.y);
	m_pMapView->GetMapContainer().ScrollTilesInfo(BlockInfo, nRows, nCols, 
		GRID_HEIGHT/32, GRID_WIDTH*2/64, nMoveDirection, ptTileOrigin, nDx, nDy);
}
//------------------------------------------------------------------------
void CTilesView::OnDirtyGridFound(int nGridX, int nGridY)
{
	RECT rc;
	rc.left = nGridX << SHIFT_WIDTH;
	rc.top = nGridY << SHIFT_HEIGHT;
	rc.right = rc.left + GRID_WIDTH;
	rc.bottom = rc.top + GRID_HEIGHT;
	SNAP_ITEM ListBuf[MAX_GRID_SNAP_ITEM_COUNT];
	int nListCount = MAX_GRID_SNAP_ITEM_COUNT;
	if (m_pMapView->SnapshotItemByWorldRect(rc, nListCount, (SNAP_ITEM_INFO*)ListBuf))
	{
		for (int i=0; i<nListCount; i++)
		{
			m_pMapView->OnMapItemEnterViewport(ListBuf[i].ptTile, ListBuf[i].pItem, NULL);
		}
	}

	if (!m_GridMgr.IsUseDynamicMap())
		return;

	// 添加逻辑对象
	stMapItemInfo* pInfo = m_ObjList.FindFirstItem(nGridX, nGridY);
	while (pInfo != NULL)
	{
		m_pMapView->AddItem(pInfo->ptTile, (CMapItemView*)pInfo->pItem, 1);
		pInfo = m_ObjList.FindNextItem(nGridX, nGridY);
	}
}
//------------------------------------------------------------------------
void CTilesView::OnDirtyGridLost(int nGridX, int nGridY)
{
	RECT rc;
	rc.left = nGridX << SHIFT_WIDTH;
	rc.top = nGridY << SHIFT_HEIGHT;
	rc.right = rc.left + GRID_WIDTH;
	rc.bottom = rc.top + GRID_HEIGHT;
	SNAP_ITEM ListBuf[MAX_GRID_SNAP_ITEM_COUNT];
	int nListCount = MAX_GRID_SNAP_ITEM_COUNT;
	if (m_pMapView->SnapshotItemByWorldRect(rc, nListCount, (SNAP_ITEM_INFO*)ListBuf))
	{
		for (int i=0; i<nListCount; i++)
		{
			m_pMapView->OnMapItemLeaveViewport(ListBuf[i].ptTile, ListBuf[i].pItem, NULL);
		}
	}
}
//------------------------------------------------------------------------
void CTilesView::AddItemToList(CMapItem* pItem, const POINT& ptTile)
{
	CMapCoordinate& coord = m_pMapView->GetMapCoordinate();
	POINT ptGrid, ptTileCenter;
	coord.TileToPixel(ptTile, ptTileCenter);
	ptGrid.x = ptTileCenter.x >> SHIFT_WIDTH;
	ptGrid.y = ptTileCenter.y >> SHIFT_HEIGHT;
	if (PtInRect(&m_GridMgr.GetMapGrid(), ptGrid))
	{
		m_ObjList.Add(pItem, ptTile, ptGrid);
	}
	else
	{
		TraceLn("grid(%d,%d) overflow (%d)0x%08x", 
			ptGrid.x, ptGrid.y, pItem->GetClassID(), (DWORD)pItem);
	}
}
//------------------------------------------------------------------------
void CTilesView::RemoveItemFromList(CMapItem* pItem, const POINT& ptTile)
{
	int nRtn = m_ObjList.Remove(pItem, ptTile);
	if (nRtn == ERR_SUCCESS)
	{
	}
	else if (nRtn == ERR_FAILED)
	{
		TraceLn("NoF(del)=(%d)0x%08x [%d,%d]", 
 			pItem->GetClassID(), (DWORD)pItem, ptTile.x, ptTile.y);
	}
}
//------------------------------------------------------------------------
void CTilesView::MoveItemInList(CMapItem* pItem, const POINT& ptTileFrom, const POINT& ptTileTo)
{
	CMapCoordinate& coord = m_pMapView->GetMapCoordinate();
	POINT ptGridTo, ptTileCenter;
	coord.TileToPixel(ptTileTo, ptTileCenter);
	ptGridTo.x = ptTileCenter.x >> SHIFT_WIDTH;
	ptGridTo.y = ptTileCenter.y >> SHIFT_HEIGHT;
	int nRtn = m_ObjList.Move(pItem, ptTileFrom, ptTileTo, ptGridTo);
	if (nRtn == ERR_SUCCESS)
	{
	}
	else if (nRtn == ERR_FAILED)
	{
		TraceLn("NoF(mov)=[%d,%d] (%d)0x%08x [%d,%d]", 
			ptGridTo.x, ptGridTo.y, pItem->GetClassID(), (DWORD)pItem, ptTileTo.x, ptTileTo.y);
	}
}
//------------------------------------------------------------------------