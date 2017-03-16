/**
*	created:		2012-7-1   1:31
*	filename: 		ScrollSheet
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
#ifndef STRUCT_AREA
#define STRUCT_AREA
typedef struct tagAREA
{
	int x,y,w,h;
} AREA, * LPAREA;
#endif
//------------------------------------------------------------------------
#ifndef PixelCood2GridCood
#define PixelCood2GridCood(PixelPos, GridLength)	((PixelPos + GridLength - 1) / GridLength)
#endif
//------------------------------------------------------------------------
// 当移动时发现新的脏块或发现丢失块，通知接收器事件发生
struct IScrollSheetSink
{
	virtual void OnGridChanged(int nMoveDirection, int nDx, int nDy) = NULL; // 网格区域改变
	virtual void OnDirtyGridFound(int nGridX,int nGridY) = NULL;	// 发现一个脏块
	virtual void OnDirtyGridLost(int nGridX,int nGridY) = NULL;	// 发现一个脏块已经太远，因此可以考虑删除此脏块
};
//------------------------------------------------------------------------
class CScrollSheet  
{
public:
	BOOL ScrollToCenter(const RECT& rcView);
	CScrollSheet()
	{
		m_pScrollSheetSink = NULL;

		m_nMapWidth = 0;
		m_nMapHeight = 0;
		m_nViewWidth = 0;
		m_nViewHeight = 0;
		m_nViewTopX = 0;
		m_nViewTopY = 0;


		m_nGridHeight = 0;
		m_nGridWidth = 0;

		memset(&m_rcCurViewGrid,0,sizeof(RECT));
		memset(&m_rcMapGrid,0,sizeof(RECT));
		memset(&m_rcLastDirty,0,sizeof(RECT));
		memset(&m_rcViewportRect,0,sizeof(RECT));
		memset(&m_rcMap,0,sizeof(RECT));
	}
	~CScrollSheet()
	{

	}

	int GetViewTopX() const
	{
		return m_nViewTopX;
	}

	int GetViewTopY() const
	{
		return m_nViewTopY;
	}

	int GetViewWidth() const
	{
		return m_nViewWidth;
	}
	int GetViewHeight() const
	{
		return m_nViewHeight;
	}

	// 当前视口所覆盖的所有块的象素矩阵，基于世界坐标
	void GetViewportCoveredGridRect(RECT& rcCoveredRect)
	{
		rcCoveredRect.left = m_rcCurViewGrid.left * m_nGridWidth;
		rcCoveredRect.right = m_rcCurViewGrid.right * m_nGridWidth;
		rcCoveredRect.top = m_rcCurViewGrid.top * m_nGridHeight;
		rcCoveredRect.bottom = m_rcCurViewGrid.bottom * m_nGridHeight;
	}
	RECT& GetCurViewGrid() {return m_rcCurViewGrid;}

	BOOL Create(IScrollSheetSink* pSink,int nMapWidth,int nMapHeight,
		int nGridWidth,int nGridHeight,LPRECT lprcViewport);
	void Close();

	BOOL MoveViewportTo(int x,int y);
	BOOL ScrollViewport(int dx,int dy);
	BOOL SizeViewport(int nWidth,int nHeight);
	RECT GetViewportRect()
	{
		return m_rcViewportRect;
	}

private:
	void SetViewTileRect(RECT rc);
	IScrollSheetSink *m_pScrollSheetSink;
private:
	int m_nMapWidth;		// 地图大小(像素)
	int m_nMapHeight;
	int m_nViewWidth;		// 视口大小(像素)
	int m_nViewHeight;
	int m_nViewTopX;		// 视口左上角坐标(像素)
	int m_nViewTopY;
	RECT m_rcViewportRect;	// 视口矩形(像素)
	RECT m_rcMap;			// 地图矩形(像素)
	int m_nGridWidth;		// 一个单元格的大小(像素)
	int m_nGridHeight;		
	RECT m_rcCurViewGrid;	// 当前可见视口所在的所有块(块坐标)
	RECT m_rcMapGrid;		// 全图矩阵格(块坐标)
	RECT m_rcLastDirty;		// 上一次脏矩阵范围，当垂直或水平移动时，现优化为仅扩展前进方向上的块，上面和侧面不再设置为脏
};
//------------------------------------------------------------------------