/**
*	created:		2012-7-1   5:17
*	filename: 		TilesView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "MapContainer.h"
#include "ScrollSheet.h"
#include "GridMgr.h"
#include "ObjectList.h"
//------------------------------------------------------------------------
struct IScrollSheetSink;
class CTile;
class CMapView;
//------------------------------------------------------------------------
class CTilesView  : public IScrollSheetSink
{
	CMapView*	m_pMapView;			// 地图
	CGridMgr	m_GridMgr;			// 处理地图的动态加载
	CObjectList m_ObjList;			// 逻辑对象列表

public:
	CTilesView();
	virtual ~CTilesView();

	BOOL Create(int nMapWidth, int nMapHeight, POINT& ptPlayerPos, 
		CMapView* pMapView, BOOL bUseDynamicMap);

	BOOL SizeViewport(int nWidth,int nHeight);
	BOOL ScrollToCenter(POINT& ptPlayerPos);
	void Close();

	virtual void OnGridChanged(int nMoveDirection, int nDx, int nDy);
	virtual void OnDirtyGridFound(int nGridX, int nGridY);
	virtual void OnDirtyGridLost(int nGridX, int nGridY);

	void GetPreReadCoveredGridRect(RECT& rcCoveredRect)
	{
		m_GridMgr.GetGridRectBelowPreRead(rcCoveredRect);
	}
	void GetViewCoveredGridRect(RECT& rcCoveredRect)
	{
		m_GridMgr.GetGridRectBelowViewport(rcCoveredRect);
	}

	void AddItemToList(CMapItem* pItem, const POINT& ptTile);
	void RemoveItemFromList(CMapItem* pItem, const POINT& ptTile);
	void MoveItemInList(CMapItem* pItem, const POINT& ptTileFrom, const POINT& ptTileTo);
	CObjectList* GetObjectList()		{return &m_ObjList;}
	RECT& GetPreReadGrid()				{return m_GridMgr.GetPreReadGrid();}
	RECT& GetViewGrid()					{return m_GridMgr.GetViewGrid();}
};
//------------------------------------------------------------------------