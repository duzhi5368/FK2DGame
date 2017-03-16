/**
*	created:		2012-7-1   1:32
*	filename: 		ScrollSheet
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
//------------------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//------------------------------------------------------------------------
BOOL CScrollSheet::Create(IScrollSheetSink* pSink,
						  int nMapWidth,int nMapHeight,
						  int nGridWidth,int nGridHeight,
						  LPRECT lprcViewport)
{
	m_pScrollSheetSink = pSink,
	m_nMapWidth = nMapWidth;
	m_nMapHeight = nMapHeight;
	SetRect(&m_rcMap,0,0,nMapWidth,nMapHeight);

	m_nViewWidth = lprcViewport->right - lprcViewport->left;
	m_nViewHeight = lprcViewport->bottom - lprcViewport->top;
	m_nViewTopX = lprcViewport->left;
	m_nViewTopY = lprcViewport->top;
	SetRect(&m_rcViewportRect,m_nViewTopX,m_nViewTopY,m_nViewTopX + m_nViewWidth,m_nViewTopY + m_nViewHeight);

	m_nGridWidth = nGridWidth;
	m_nGridHeight = nGridHeight;
	SetRect(&m_rcMapGrid,0,0,(m_nMapWidth + nGridWidth - 1) / nGridWidth,(m_nMapHeight + nGridHeight - 1) / nGridHeight);

	RECT rc;
	SetRect(&rc, m_nViewTopX / nGridWidth, m_nViewTopY / nGridHeight,
		(m_nViewTopX + m_nViewWidth - 1) / nGridWidth + 1, (m_nViewTopY + m_nViewHeight - 1) / nGridHeight + 1);
	SetViewTileRect(rc);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CScrollSheet::SizeViewport(int nWidth, int nHeight)
{
	if(nWidth == 0 || nHeight == 0)
		return FALSE;
	m_nViewWidth = nWidth;
	m_nViewHeight = nHeight;

	SetRect(&m_rcCurViewGrid,m_nViewTopX / m_nGridWidth,m_nViewTopY / m_nGridHeight,
		(m_nViewTopX + m_nViewWidth - 1) / m_nGridWidth + 1,
		(m_nViewTopY + m_nViewHeight - 1) / m_nGridHeight + 1);

	// 更新新的脏格?
	//.....
	return TRUE;
}
//------------------------------------------------------------------------
void CScrollSheet::Close()
{

}
//------------------------------------------------------------------------
BOOL CScrollSheet::MoveViewportTo(int x, int y)
{
	ScrollViewport(x - m_nViewTopX,y - m_nViewTopY);
	return TRUE;
}
//------------------------------------------------------------------------
// 强制设置视口某区域，所有覆盖块被设置为脏
void CScrollSheet::SetViewTileRect(RECT rc)
{
	IntersectRect(&rc,&rc,&m_rcMapGrid);
	m_rcCurViewGrid = rc;
	InflateRect(&rc,1,1);
	IntersectRect(&rc,&rc,&m_rcMapGrid);
	for(int y = rc.top; y < rc.bottom; y++)
		for(int x = rc.left; x < rc.right; x++)
			m_pScrollSheetSink->OnDirtyGridFound(x,y);

	m_rcLastDirty = rc;
}
//------------------------------------------------------------------------
// 滚动视口
// 第一次启动时默认是在最左边
// bForce : 强制更新，因为一般没有移动到新块时不更新。一般在第一次显示时需强制更新
// 返回：如果有滚动返回TRUE，否则返回FALSE
BOOL CScrollSheet::ScrollViewport(int dx, int dy)
{
	if(m_nViewTopX + dx < 0)
		dx = -m_nViewTopX;
	if(m_nViewTopX + m_nViewWidth + dx > m_nMapWidth)
		dx = m_nMapWidth - m_nViewTopX - m_nViewWidth;
	if(m_nViewTopY + dy < 0)
		dy = - m_nViewTopY;
	if(m_nViewTopY + m_nViewHeight + dy > m_nMapHeight)
		dy = m_nMapHeight - m_nViewTopY - m_nViewHeight;
	if(dx == 0 && dy == 0)
		return FALSE;

	RECT rcNew,rcOffset,rcNewDirty;
	SetRect(&rcNew,(m_nViewTopX + dx) / m_nGridWidth,(m_nViewTopY + dy) / m_nGridHeight,
		(m_nViewTopX + m_nViewWidth - 1 + dx) / m_nGridWidth + 1,
		(m_nViewTopY + m_nViewHeight - 1 + dy) / m_nGridHeight + 1);
	if(EqualRect(&rcNew,&m_rcCurViewGrid))
	{
		m_nViewTopX += dx;
		m_nViewTopY += dy;
		SetRect(&m_rcViewportRect,m_nViewTopX,m_nViewTopY,m_nViewTopX + m_nViewWidth,m_nViewTopY + m_nViewHeight);
		return TRUE;
	}

	rcOffset = rcNew;
	OffsetRect(&rcOffset,dx > 0 ? 1 : dx < 0 ? -1 : 0,dy > 0 ? 1 : dy < 0 ? -1 : 0);
	UnionRect(&rcNewDirty,&rcNew,&rcOffset);
	IntersectRect(&rcNewDirty,&rcNewDirty,&m_rcMapGrid);

	m_nViewTopX += dx;
	m_nViewTopY += dy;

	SetRect(&m_rcViewportRect,m_nViewTopX,m_nViewTopY,m_nViewTopX + m_nViewWidth,m_nViewTopY + m_nViewHeight);
	
	// 丢弃旧的脏块，以便回收内存
	for(int y = m_rcLastDirty.top; y < m_rcLastDirty.bottom; y++)
		for(int x = m_rcLastDirty.left; x < m_rcLastDirty.right; x++)
		{
			POINT pt;
			pt.x = x; pt.y = y;
			if(!PtInRect(&rcNewDirty,pt) && !PtInRect(&rcNew,pt))
				m_pScrollSheetSink->OnDirtyGridLost(x,y);
		}

	// 增加新的脏块
	for(int y = rcNewDirty.top; y < rcNewDirty.bottom; y++)
		for(int x = rcNewDirty.left; x < rcNewDirty.right; x++)
		{
			POINT pt;
			pt.x = x; pt.y = y;
			if(!PtInRect(&rcNewDirty/*m_rcLastDirty*/,pt))
				m_pScrollSheetSink->OnDirtyGridFound(x,y);
		}

	m_rcCurViewGrid = rcNew;
	m_rcLastDirty = rcNewDirty;
	return TRUE;
}
//------------------------------------------------------------------------
// 可见视图移动屏幕时，使预读视图把可见视图包围在正中间
// 算法：总是尽可能保证地图视口与预读视口的中点是对齐的
// 返回：有滚动返回TRUE
BOOL CScrollSheet::ScrollToCenter(const RECT& rcView)
{
	int x = (rcView.left + rcView.right) / 2  - m_nViewWidth / 2;
	int y = (rcView.top + rcView.bottom) / 2 - m_nViewHeight / 2;

	return ScrollViewport(x - m_nViewTopX,y - m_nViewTopY);
}
//------------------------------------------------------------------------