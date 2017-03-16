/**
*	created:		2012-7-1   5:12
*	filename: 		GridMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		网格管理器，用于类九宫格的管理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/MapContainer.h"
//------------------------------------------------------------------------
#ifndef STRUCT_AREA
#define STRUCT_AREA
typedef struct tagAREA
{
	int x,y,w,h;
} AREA, * LPAREA;
#endif
//------------------------------------------------------------------------
struct IScrollSheetSink;
//------------------------------------------------------------------------
class CGridMgr  
{
	RECT				m_rcMapGrid;		// 整个地图的网格矩形(网格坐标)
	RECT				m_rcPlayerGrid;		// 玩家活动的网格(在这个网格范围内不会导致地图网格的滚动)
	POINT				m_ptPlayerGrid;		// 玩家所在网格
	RECT				m_rcOldPreReadGrid;	// 先前的预读区域所在的网格矩形(网格坐标)
	RECT				m_rcPreReadGrid;	// 预读区域(网格坐标)
	RECT				m_rcOldViewGrid;	// 先前的可视区域所在的网格矩形(网格坐标)
	RECT				m_rcViewGrid;		// 可视区域所在的网格矩形(网格坐标)
	BOOL				m_bUseDynamicMap;	// 是否使用动态地图
	IScrollSheetSink*	m_pScrollSheetSink; // 用于对新增(丢弃)的网格块进行处理
public:
	CGridMgr();
	~CGridMgr() {}

	BOOL Create(IScrollSheetSink* pSink, int nMapWidth, int nMapHeight, 
		POINT& ptPlayerPos, BOOL bUseDynamicMap = FALSE);
	void Close();

	BOOL ScrollViewport(int dx, int dy);
	BOOL SizeViewport(int nWidth, int nHeight);
	BOOL ScrollToCenter(POINT& ptPlayerPos);

	void GetGridRectBelowViewport(RECT& rcCoveredRect);
	void GetGridRectBelowPreRead(RECT& rcCoveredRect);
	RECT& GetPreReadGrid()			{return m_rcPreReadGrid;}
	RECT& GetViewGrid()				{return m_rcViewGrid;}
	RECT& GetMapGrid()				{return m_rcMapGrid;}
	POINT GetPlayerGrid()			{return m_ptPlayerGrid;}
	BOOL  IsUseDynamicMap()			{return m_bUseDynamicMap;}
	
	void SnapshotLeftTop(int& nRows, int& nCols, BLOCK_INFO* pBlockInfo, int nMoveDirection);
	int GetBlockIndex(POINT& ptLeftTop);
private:
	void SetPreReadTileRect(RECT rc);
	void ClosePreReadTileRect();
	void SetViewTileRect(RECT rc);
	void CloseViewTileRect();
	int  GetMoveDirection(POINT& ptFrom, POINT& ptTo, int& nDx, int& nDy);
	void LimitRect(RECT& rcSmall, const RECT& rcBig);
	void ResetPlayerRect();
};
//------------------------------------------------------------------------