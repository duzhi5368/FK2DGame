/**
*	created:		2012-7-1   5:13
*	filename: 		GridMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
/*
 * 功能：类九宫格管理，不同之处在于管理的矩阵可能更大
 * 用途：动态地表块加载、地图上可见大物体扫描
 * 描述：
		将一个世界分成很多单元格，用户可见视口可能比一个单元格大，也可能比一个单元格小
 * 名词解释:
		脏块：  当视口移动时，预测在一个方向上移动时将要到达的块，定义此块为脏块
		丢失块：当视口移动时，预测在一个方向上移动时，已经远离的相反的方向的脏块称为丢失块
   实现：
		当移动时发现新的脏块或发现丢失块，通知接收器事件发生
		接收器一般在此时预读或删除下一个块的图片(动态地表加载)、扫描新发现的大物体
	说明：
		1、考虑定义一些无块，当这些块被发现时不处理，因为地图会有很多空白区域
		2、单元格必须比视口小？ 太多会加载太多多余的图
		3、脏矩形发现策略改成只将运动方向上对应的块设置为脏？以后考虑按此优化实现
 */
//------------------------------------------------------------------------
#include "../Include/ScrollSheet.h"
#include "../Include/GridMgr.h"
//------------------------------------------------------------------------
CGridMgr::CGridMgr()
{
	m_pScrollSheetSink = NULL;
	m_ptPlayerGrid.x = m_ptPlayerGrid.y = 0;
	memset(&m_rcMapGrid, 0, sizeof(RECT));
	memset(&m_rcPlayerGrid, 0, sizeof(RECT));
	memset(&m_rcOldPreReadGrid,0,sizeof(RECT));
	memset(&m_rcPreReadGrid, 0, sizeof(RECT));
	memset(&m_rcOldViewGrid, 0, sizeof(RECT));
	memset(&m_rcViewGrid, 0, sizeof(RECT));
	m_bUseDynamicMap = FALSE;
}
//------------------------------------------------------------------------
BOOL CGridMgr::Create(IScrollSheetSink* pSink, int nMapWidth, int nMapHeight, 
					  POINT& ptPlayerPos, BOOL bUseDynamicMap)
{
	ASSERT(pSink);
	m_pScrollSheetSink = pSink;
	m_bUseDynamicMap = bUseDynamicMap;
	
	// 地图网格矩形
	m_rcMapGrid.left	= m_rcMapGrid.top = 0;
	m_rcMapGrid.right	= (nMapWidth + GRID_WIDTH - 1) >> SHIFT_WIDTH;
	m_rcMapGrid.bottom	= (nMapHeight + GRID_HEIGHT - 1) >> SHIFT_HEIGHT;

	// 玩家(屏幕中心点)所在网格
	m_ptPlayerGrid.x = ptPlayerPos.x >> SHIFT_WIDTH;
	m_ptPlayerGrid.y = ptPlayerPos.y >> SHIFT_HEIGHT;

	// 预读区域
	if (m_bUseDynamicMap)
	{
		m_rcPreReadGrid.left	= m_ptPlayerGrid.x - (TILE_GRID_COL >> 1);
		m_rcPreReadGrid.top		= m_ptPlayerGrid.y - (TILE_GRID_ROW >> 1);
		m_rcPreReadGrid.right	= m_rcPreReadGrid.left + TILE_GRID_COL;
		m_rcPreReadGrid.bottom	= m_rcPreReadGrid.top + TILE_GRID_ROW;
		LimitRect(m_rcPreReadGrid, m_rcMapGrid);
		m_rcOldPreReadGrid = m_rcPreReadGrid;
	}
	else
	{
		m_rcOldPreReadGrid = m_rcPreReadGrid = m_rcMapGrid;
	}

	// 可视区域
	m_rcViewGrid.left	= m_ptPlayerGrid.x - (OBJECT_GRID_COL >> 1);
	m_rcViewGrid.top	= m_ptPlayerGrid.y - (OBJECT_GRID_ROW >> 1);
	m_rcViewGrid.right	= m_rcViewGrid.left + OBJECT_GRID_COL;
	m_rcViewGrid.bottom	= m_rcViewGrid.top + OBJECT_GRID_ROW;
	LimitRect(m_rcViewGrid, m_rcMapGrid);
	m_rcOldViewGrid = m_rcViewGrid;
	ResetPlayerRect();
	
	if (m_bUseDynamicMap)
		SetPreReadTileRect(m_rcPreReadGrid);
	SetViewTileRect(m_rcViewGrid);

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CGridMgr::SizeViewport(int nWidth, int nHeight)
{
	return TRUE;
}
//------------------------------------------------------------------------
void CGridMgr::Close()
{
	CloseViewTileRect();
	ClosePreReadTileRect();
}
//------------------------------------------------------------------------
// 强制设置预读某区域，所有覆盖块被设置为脏
void CGridMgr::SetPreReadTileRect(RECT rc)
{
	int nDx, nDy;
	int nMoveDir = GetMoveDirection((POINT&)m_rcOldPreReadGrid, (POINT&)m_rcPreReadGrid, nDx, nDy);
	m_pScrollSheetSink->OnGridChanged(nMoveDir, nDx, nDy);
}
//------------------------------------------------------------------------
// 强制设置预读某区域，所有覆盖块被设置为脏
void CGridMgr::ClosePreReadTileRect()
{
}
//------------------------------------------------------------------------
// 强制设置视口某区域，所有覆盖块被设置为脏
void CGridMgr::SetViewTileRect(RECT rc)
{
	for (int y=m_rcViewGrid.top; y<m_rcViewGrid.bottom; y++)
		for (int x=m_rcViewGrid.left; x<m_rcViewGrid.right; x++)
			m_pScrollSheetSink->OnDirtyGridFound(x, y);
}
//------------------------------------------------------------------------
// 强制关闭视口某区域，所有覆盖块被设置为丢失
void CGridMgr::CloseViewTileRect()
{
	for (int y=m_rcViewGrid.top; y<m_rcViewGrid.bottom; y++)
		for (int x=m_rcViewGrid.left; x<m_rcViewGrid.right; x++)
			m_pScrollSheetSink->OnDirtyGridLost(x, y);
}
//------------------------------------------------------------------------
// 滚动视口
// 第一次启动时默认是在最左边
// bForce : 强制更新，因为一般没有移动到新块时不更新。一般在第一次显示时需强制更新
// 返回：如果有滚动返回TRUE，否则返回FALSE
BOOL CGridMgr::ScrollViewport(int dx, int dy)
{
	// 检测预加载网格矩形是否改变
	BOOL bPreReadGridChanged = FALSE;
	RECT rcNewPreReadGrid;
	if (m_bUseDynamicMap)
	{
		rcNewPreReadGrid = m_rcPreReadGrid;
		OffsetRect(&rcNewPreReadGrid, dx, dy);
		LimitRect(rcNewPreReadGrid, m_rcMapGrid);
		bPreReadGridChanged = !EqualRect(&rcNewPreReadGrid, &m_rcPreReadGrid);
	}

	// 检测视图网格矩形是否改变
	BOOL bViewGridChanged = FALSE;
	RECT rcNewViewGrid;
	rcNewViewGrid = m_rcViewGrid;
	OffsetRect(&rcNewViewGrid, dx, dy);
	LimitRect(rcNewViewGrid, m_rcMapGrid);
	bViewGridChanged = !EqualRect(&rcNewViewGrid, &m_rcViewGrid);
	
	// 丢弃旧的脏块(仅仅针对地图上的对象)
	if (bViewGridChanged)
	{
		POINT pt;
		for (int y=m_rcViewGrid.top; y<m_rcViewGrid.bottom; y++)
		{
			for (int x=m_rcViewGrid.left; x<m_rcViewGrid.right; x++)
			{
				pt.x = x, pt.y = y;
				if (!PtInRect(&rcNewViewGrid, pt))
					m_pScrollSheetSink->OnDirtyGridLost(x, y);
			}
		}
	
		m_rcOldViewGrid = m_rcViewGrid;
		m_rcViewGrid = rcNewViewGrid;
		ResetPlayerRect();
	}

	// 预读网格矩形有改变
	if (m_bUseDynamicMap && bPreReadGridChanged) // 仅仅针对地图上的地表信息
	{
		m_rcOldPreReadGrid = m_rcPreReadGrid;
		m_rcPreReadGrid = rcNewPreReadGrid;
		int nDx, nDy;
		int nMoveDirection = GetMoveDirection((POINT&)m_rcOldPreReadGrid, (POINT&)m_rcPreReadGrid, nDx, nDy);
		m_pScrollSheetSink->OnGridChanged(nMoveDirection, nDx, nDy);
	}

	// 增加新的脏块(仅仅针对地图上的对象)
	if (bViewGridChanged)
	{
		POINT pt;
		for (int y=m_rcViewGrid.top; y<m_rcViewGrid.bottom; y++)
		{ 
			for (int x=m_rcViewGrid.left; x<m_rcViewGrid.right; x++)
			{
				pt.x = x, pt.y = y;
				if (!PtInRect(&m_rcOldViewGrid, pt))
					m_pScrollSheetSink->OnDirtyGridFound(x, y);
			}
		}

	}
	return TRUE;
}
//------------------------------------------------------------------------
// 可见视图移动屏幕时，使预读视图把可见视图包围在正中间
// 算法：总是尽可能保证地图视口与预读视口的中点是对齐的
// 返回：有滚动返回TRUE
BOOL CGridMgr::ScrollToCenter(POINT& ptPlayerPos)
{
	// 屏幕中心点所在网格
	POINT ptPlayerGridNew;
	ptPlayerGridNew.x = ptPlayerPos.x >> SHIFT_WIDTH;
	ptPlayerGridNew.y = ptPlayerPos.y >> SHIFT_HEIGHT;
	if (m_ptPlayerGrid.x == ptPlayerGridNew.x && m_ptPlayerGrid.y == ptPlayerGridNew.y)
		return FALSE;
	
	if (PtInRect(&m_rcPlayerGrid, ptPlayerGridNew))
	{
		m_ptPlayerGrid = ptPlayerGridNew;
		return FALSE;
	}

	int dx, dy;
	if (m_rcPlayerGrid.right - m_rcPlayerGrid.left != 1 ||
		m_rcPlayerGrid.bottom - m_rcPlayerGrid.top != 1) // 说明玩家没有站在动态网格的中心网格上
	{
		dx = ptPlayerGridNew.x - ((m_rcViewGrid.right + m_rcViewGrid.left) >> 1);
		dy = ptPlayerGridNew.y - ((m_rcViewGrid.bottom + m_rcViewGrid.top) >> 1);
	}
	else
	{
		dx = ptPlayerGridNew.x - m_ptPlayerGrid.x;
		dy = ptPlayerGridNew.y - m_ptPlayerGrid.y;
	}
	m_ptPlayerGrid = ptPlayerGridNew;
	return ScrollViewport(dx, dy);
}
//------------------------------------------------------------------------
// 可视区域所在的网格矩形
void CGridMgr::GetGridRectBelowViewport(RECT& rcCoveredRect)
{
	rcCoveredRect.left   = m_rcViewGrid.left   << SHIFT_WIDTH;
	rcCoveredRect.top	 = m_rcViewGrid.top    << SHIFT_HEIGHT;
	rcCoveredRect.right  = m_rcViewGrid.right  << SHIFT_WIDTH;
	rcCoveredRect.bottom = m_rcViewGrid.bottom << SHIFT_HEIGHT;
	// 不需要与地图整个矩形作相交处理，因为预加载区域可以出界
}
//------------------------------------------------------------------------
// 预读区域所在的网格矩形
void CGridMgr::GetGridRectBelowPreRead(RECT& rcCoveredRect)
{
	rcCoveredRect.left   = m_rcPreReadGrid.left   << SHIFT_WIDTH;
	rcCoveredRect.top	 = m_rcPreReadGrid.top    << SHIFT_HEIGHT;
	rcCoveredRect.right  = m_rcPreReadGrid.right  << SHIFT_WIDTH;
	rcCoveredRect.bottom = m_rcPreReadGrid.bottom << SHIFT_HEIGHT;
	// 不需要与地图整个矩形作相交处理，因为预加载区域可以出界
}
//------------------------------------------------------------------------
// 传入预读区域的左上点的网格坐标
int CGridMgr::GetMoveDirection(POINT& ptFrom, POINT& ptTo, int& nDx, int& nDy)
{
	nDx = ptTo.x - ptFrom.x;
	nDy = ptTo.y - ptFrom.y;
	if (nDx > TILE_GRID_COL || nDx < -TILE_GRID_COL || nDy > TILE_GRID_ROW || nDy < -TILE_GRID_ROW)
		return mdAll;// 表示预读区域改变前后的两个预读区域不相交
	if (nDx > 0)
	{
		if (nDy > 0)		return mdEastSouth;	// 右下
		else if (nDy < 0)	return mdEastNorth;	// 右上
		else				return mdEast;		// 右
	}
	else if (nDx < 0)
	{
		if (nDy > 0)		return mdWestSouth;	// 左下
		else if (nDy < 0)	return mdWestNorth;	// 左上
		else				return mdWest;		// 左
	}
	else
	{
		if (nDy > 0)		return mdSouth;	// 下
		else if (nDy < 0)	return mdNorth;	// 上
	}
	return mdAll;
}
//------------------------------------------------------------------------
void CGridMgr::SnapshotLeftTop(int& nRows, int& nCols, BLOCK_INFO* pBlockInfo, int nMoveDirection)
{
	RECT rcUnion;
	if (nMoveDirection == mdAll)
		rcUnion = m_rcPreReadGrid;
	else
		UnionRect(&rcUnion, &m_rcOldPreReadGrid, &m_rcPreReadGrid);
	POINT ptLeftTop;
	ptLeftTop.x  = rcUnion.left << SHIFT_WIDTH;
	ptLeftTop.y	 = rcUnion.top << SHIFT_HEIGHT;
	nRows = rcUnion.bottom - rcUnion.top;
	nCols = rcUnion.right - rcUnion.left;
	
	int x, y = ptLeftTop.y;
	int nIdx = rcUnion.top * m_rcMapGrid.right + rcUnion.left;
	for (int row=0,i=0; row<nRows; row++,y+=GRID_HEIGHT,nIdx+=m_rcMapGrid.right)
	{
		x = ptLeftTop.x;
		for (int col=0; col<nCols; col++,x+=GRID_WIDTH,i++)
		{
			pBlockInfo[i].ptLeftTop.x = x;
			pBlockInfo[i].ptLeftTop.y = y;
			pBlockInfo[i].nIndex = nIdx + col;
		}
	}
}
//------------------------------------------------------------------------
int CGridMgr::GetBlockIndex(POINT& ptLeftTop)
{
	int nGridX = ptLeftTop.x >> SHIFT_WIDTH;
	int nGridY = ptLeftTop.y >> SHIFT_HEIGHT;
	return nGridY * m_rcMapGrid.right + nGridX;
}
//------------------------------------------------------------------------
// 必须保证rcSmall的尺寸比rcBig的小。
void CGridMgr::LimitRect(RECT& rcSmall, const RECT& rcBig)
{
	if (rcSmall.left < rcBig.left)
	{
		rcSmall.right += rcBig.left - rcSmall.left;
		rcSmall.left = rcBig.left;
		
		if (rcSmall.right > rcBig.right)
			rcSmall.right = rcBig.right;
	}
	if (rcSmall.top < rcBig.top)
	{
		rcSmall.bottom += rcBig.top - rcSmall.top;
		rcSmall.top = rcBig.top;

		if (rcSmall.bottom > rcBig.bottom)
			rcSmall.bottom = rcBig.bottom;
	}
	if (rcSmall.right > rcBig.right)
	{
		rcSmall.left -= rcSmall.right - rcBig.right;
		rcSmall.right = rcBig.right;
		
		if (rcSmall.left < rcBig.left)
			rcSmall.left = rcBig.left;
	}
	if (rcSmall.bottom > rcBig.bottom)
	{
		rcSmall.top -= rcSmall.bottom - rcBig.bottom;
		rcSmall.bottom = rcBig.bottom;
		
		if (rcSmall.top < rcBig.top)
			rcSmall.top = rcBig.top;
	}
}
//------------------------------------------------------------------------
// 根据视区矩形和地图矩形确定玩家活动的矩形范围
void CGridMgr::ResetPlayerRect()
{
	int dx = 0, dy = 0;
	if (m_rcViewGrid.left <= m_rcMapGrid.left)		// 左边界相接
		dx = - ((m_rcViewGrid.left + m_rcViewGrid.right) >> 1);
	if (m_rcViewGrid.top <= m_rcMapGrid.top)		// 上边界相接
		dy = - ((m_rcViewGrid.top + m_rcViewGrid.bottom) >> 1);
	if (m_rcViewGrid.right >= m_rcMapGrid.right)	// 右边界相接
		dx = ((m_rcViewGrid.right - m_rcViewGrid.left) >> 1);
	if (m_rcViewGrid.bottom >= m_rcMapGrid.bottom)	// 下边界相接
		dy = ((m_rcViewGrid.bottom - m_rcViewGrid.top) >> 1);

	m_rcPlayerGrid.left = (m_rcViewGrid.left + m_rcViewGrid.right) >> 1;
	m_rcPlayerGrid.top = (m_rcViewGrid.top + m_rcViewGrid.bottom) >> 1;
	m_rcPlayerGrid.right = m_rcPlayerGrid.left + 1;
	m_rcPlayerGrid.bottom = m_rcPlayerGrid.top + 1;
	if (dx < 0)
		m_rcPlayerGrid.left += dx;
	if (dx > 0)
		m_rcPlayerGrid.right += dx;
	if (dy < 0)
		m_rcPlayerGrid.top += dy;
	if (dy > 0)
		m_rcPlayerGrid.bottom += dy;
}
//------------------------------------------------------------------------