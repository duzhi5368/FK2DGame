/**
*	created:		2012-7-1   2:12
*	filename: 		GroundView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
/*
 * 功能		:	管理地表显示，可能包含地表动画；类九宫格式的地表图管理，自己缓冲地表图像?；
				滚屏时地表的后台加载。
				坐标计算
   说明		:
				1、实现单元格IBitmap的缓存功能，把最常用的图片缓存起来，采取简单的先进先出的淘汰算法
				实现：
					1)、当脏块丢失时，将脏块的格块图片缓冲起来
					2)、当需要解码新的图片时，将缓冲中最旧的图片内存提交给CJpegLoader
					3)、CJpegLoader只有在没有IBitmap缓冲时才申请内存创建IBitmap
				2、jpg文件读取的问题，有一次性读取、需要哪张即读哪张、后台动态读取三种做法
						
 */
//------------------------------------------------------------------------
#include "../Include/GroundView.h"
#include "../Include/MapViewCommon.h"
//------------------------------------------------------------------------
#define _NO_USE_THREAD
#ifndef _NO_USE_THREAD
	#if defined(_USE) || defined(_TEST)
		#define _NO_USE_THREAD
	#endif
#endif
//------------------------------------------------------------------------
#define TILE_LINE_COLOR	WPixel(0,132,132)
//------------------------------------------------------------------------
#define MAX(i, j) ((i < j) ? j : i)
#define MIN(i, j) (( i < j) ? i : j)
//------------------------------------------------------------------------
// szGridIndexArray为地表块拼接索引文件
BOOL CGroundView::Create(int nMapWidth, int nMapHeight, int nGridWidth, int nGridHeight,
						 LPRECT lprcViewport, 
						 const char* szJpgFileDir, IFileObject* pGridIdxArrayFile, ITrace*pTrace)
{
	m_nMapWidth = nMapWidth;
	m_nMapHeight = nMapHeight;
	m_rcMapRect.right = nMapWidth;
	m_rcMapRect.bottom = nMapHeight;

	m_nGridWidth = nGridWidth;
	m_nGridHeight = nGridHeight;

	m_nViewWidth = lprcViewport->right - lprcViewport->left;
	m_nViewHeight = lprcViewport->bottom - lprcViewport->top;
	
	m_nTotalGridX = PixelCood2GridCood(m_nMapWidth, nGridWidth);
	m_nTotalGridY = PixelCood2GridCood(m_nMapHeight, nGridHeight);

	m_MapCo.Create(nMapWidth, nMapHeight);

	if(!m_jpgLoader.Create(szJpgFileDir,pTrace))
	{
		return FALSE;
	}

	int nTotalGrid = m_nTotalGridX * m_nTotalGridY;
	TraceLn("本地图Grid数量 = %d", nTotalGrid );
	m_pMapGrid = new MAP_GRID[nTotalGrid];
	if(m_pMapGrid == NULL)
		return FALSE;

	if (pGridIdxArrayFile && pGridIdxArrayFile->Open())
	{
		int nBufLen = pGridIdxArrayFile->GetLength();
		char* pBuf = new char[nBufLen
#ifdef _DEBUG
			+1024
#endif
			];
		if (pGridIdxArrayFile->Read(pBuf, nBufLen) != (UINT)nBufLen)
		{
			delete[] pBuf;
			return FALSE;
		}

		int nGridX = *(WORD*)pBuf;
		int nGridY = *((WORD*)pBuf+1);
		int nTrueGridCount = nGridX * nGridY;
		if (nTrueGridCount != nTotalGrid)
		{
			TraceLn("mai文件和map文件定义的地图大小不匹配!");
		}

		// 索引
		WORD *pIndex = (WORD*)(pBuf + 4); // 格式：wWidth + wHeight + wIndexArray[wWidth * wHeight]
		for(int i = 0; i < nTrueGridCount; i++)
		{
			m_pMapGrid[i].wBmpIndex = *pIndex++;
		}
		delete[] pBuf;
	}
	else
	{
		for(int i = 0; i < nTotalGrid; i++)
			m_pMapGrid[i].wBmpIndex = (WORD)i;
	}

	if (!m_ScrollSheet.Create(static_cast<IScrollSheetSink*>(this),
		nMapWidth, nMapHeight, nGridWidth, nGridHeight, lprcViewport))
		return FALSE;
	
	CalcViewTileRect();
	return TRUE;
}
//------------------------------------------------------------------------
void CGroundView::Close()
{
	m_jpgLoader.Close();
	if(m_pMapGrid)
	{
		MAP_GRID *p = m_pMapGrid;
		delete m_pMapGrid;
		m_pMapGrid = NULL;
	}
}
//------------------------------------------------------------------------
int CGroundView::GetIndexByPoint(POINT& pt)
{
	if (PtInRect(&m_rcMapRect, pt))
	{
		int nGridX = pt.x / m_nGridWidth;
		int nGridY = pt.y / m_nGridHeight;
		int nIdx = m_nTotalGridX * nGridY + nGridX;
		return m_pMapGrid[nIdx].wBmpIndex;
	}
	return -1;
}
//------------------------------------------------------------------------
BOOL CGroundView::SizeViewport(int nWidth, int nHeight)
{
	if(nWidth == 0 || nHeight == 0)
		return FALSE;
	m_nViewWidth = nWidth;
	m_nViewHeight = nHeight;
	m_ScrollSheet.SizeViewport(nWidth, nHeight);
	CalcViewTileRect();
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CGroundView::MoveViewportTo(int x, int y)
{
	m_ScrollSheet.MoveViewportTo(x,y);
	CalcViewTileRect();
	return TRUE;
}
//------------------------------------------------------------------------
// 把可见块显示出来，如果没发现相关块的IBitmap，则填成黑色
void CGroundView::OnDraw(ICanvas* pCanvas)
{
	int nViewTopX = m_ScrollSheet.GetViewTopX();
	int nViewTopY = m_ScrollSheet.GetViewTopY();
	int col = (nViewTopX + m_nViewWidth - 1) / m_nGridWidth - nViewTopX / m_nGridWidth + 1;
	int row = (nViewTopY + m_nViewHeight - 1) / m_nGridHeight - nViewTopY / m_nGridHeight + 1;
	if(col == 0 || row == 0)
		return;

	int nGridTopX = nViewTopX / m_nGridWidth;						// 新视口左上角格块坐标
	int nGridTopY = nViewTopY / m_nGridHeight;

	RECT rcView = {nViewTopX,nViewTopY,nViewTopX + m_nViewWidth,nViewTopY + m_nViewHeight};
	
	RECT rcDirtyGrid;	// 可能需要有一部分显示的脏块，用来步进每个脏块
	rcDirtyGrid.left = nGridTopX * m_nGridWidth;
	rcDirtyGrid.top = nGridTopY * m_nGridHeight;
	rcDirtyGrid.right = rcDirtyGrid.left + m_nGridWidth;
	rcDirtyGrid.bottom = rcDirtyGrid.top + m_nGridHeight;

	ASSERT(nGridTopY * m_nTotalGridX + nGridTopX >= 0);
	MAP_GRID* pLine = &m_pMapGrid[ nGridTopY * m_nTotalGridX + nGridTopX ];
	int nDirtyRectX0 = rcDirtyGrid.left;
	for(int y = 0; y < row; y++)
	{
		MAP_GRID* px = pLine;
		for(int x = 0; x < col; x++,px++)
		{
			RECT rcGridInScreen; // 一个grid在屏幕上可见的部分
			BOOL b = IntersectRect(&rcGridInScreen,&rcDirtyGrid,&rcView);
			
			// 如果少了一个IBitmap，检查是否有新的IBitmap解码出来
			if(px->pBmp == NULL)
			{
				if (!px->IsEmptyBitmap())
				{
#ifdef _NO_USE_THREAD
					// 从垃圾回收站里找找看
					int index = (nGridTopY + y) * m_nTotalGridX + nGridTopX + x;
					IBitmap* pBmp = m_BitmapRecycle.Find(m_pMapGrid[index].wBmpIndex);
					if (pBmp)
						m_pMapGrid[index].SetLoaded(pBmp);
					else
					{
						// 找不到只好去解码了
						if (m_BitmapRecycle.IsFull())
						{
							IBitmap* pRemoveBmp = m_BitmapRecycle.PopHead();
							
							if(pRemoveBmp)
							{
								int nTotalGrid = m_nTotalGridX * m_nTotalGridY;
								for(int i = 0; i < nTotalGrid; i++)
								{
									if(m_pMapGrid[i].pBmp == pRemoveBmp)
										m_pMapGrid[i].Init();
								}
								pRemoveBmp->Release();
							}
						}
						pBmp = m_jpgLoader.LoadJpgNoUseThread(m_pMapGrid[index].wBmpIndex);
						m_pMapGrid[index].SetLoaded(pBmp);
						if (pBmp)
						{
							m_BitmapRecycle.AddTail(m_pMapGrid[index].wBmpIndex,m_pMapGrid[index].pBmp);
						}
					}
#else
					if(px->IsNeedLoad())
						OnDirtyGridFound(nGridTopX + x,nGridTopY + y);
					if(px->IsRequesting())
					{
						// 等待很短时间，以便图片此时可能能加载进来
						DWORD dwLast = GetTickCount();
						do
						{
							GetBackgroundLoadingBitmap();
							if(!px->IsRequesting())
								break;
							else
								Sleep(1);
						#ifdef _DEBUG
						}while(GetTickCount() - dwLast < 100);
						#else
						}while(GetTickCount() - dwLast < 80);
						#endif
					}
#endif // _NO_USE_THREAD
				
#ifdef _DEBUG
					if (px->IsLoadError())
					{
						char str[1024];
						wsprintf(str,"Warn : CGroundView::OnDraw() : can not load grid(%d,%d),id=%d,load error\r\n",nGridTopX + x,nGridTopY + y,px->wBmpIndex);
						OutputDebugString(str);
					}
					else if(px->IsEmptyBitmap())
					{
						char str[1024];
						wsprintf(str,"Warn : CGroundView::OnDraw() : can not load grid(%d,%d),wait time out\r\n",nGridTopX + x,nGridTopY + y);
						OutputDebugString(str);
					}
#endif
				}
			}
			if(px->pBmp)
			{
				pCanvas->Draw(
					rcGridInScreen.left - nViewTopX/*DestX*/, 
					rcGridInScreen.top - nViewTopY/*DestY*/,
					rcGridInScreen.right - rcGridInScreen.left, // width
					rcGridInScreen.bottom - rcGridInScreen.top, // height
					px->pBmp,
					rcGridInScreen.left - rcDirtyGrid.left/*srcX*/,
					rcGridInScreen.top  - rcDirtyGrid.top/*srcY*/);
			}
			else
			{
				RECT rc;
				rc.left = rcGridInScreen.left - nViewTopX;
				rc.top = rcGridInScreen.top - nViewTopY;
				rc.right = rcGridInScreen.right - nViewTopX;
				rc.bottom = rcGridInScreen.bottom - nViewTopY;

				rc.left = MAX(0, rc.left);
				rc.top = MAX(0, rc.top);
				rc.right = MAX(0, rc.right);
				rc.bottom = MAX(0, rc.bottom);
				
				rc.left = MIN(1024, rc.left);
				rc.top = MIN(768, rc.top);
				rc.right = MIN(1024, rc.right);
				rc.bottom = MIN(768, rc.bottom);

				pCanvas->Clear(0, &rc);
			}

			rcDirtyGrid.left += m_nGridWidth;
			rcDirtyGrid.right = rcDirtyGrid.left + m_nGridWidth; 
		}
		rcDirtyGrid.left = nDirtyRectX0;
		rcDirtyGrid.right = rcDirtyGrid.left + m_nGridWidth;
		rcDirtyGrid.top += m_nGridHeight;
		rcDirtyGrid.bottom += m_nGridHeight;

		pLine += m_nTotalGridX;
	}
}
//------------------------------------------------------------------------
// 检查是否有新的IBitmap解码出来
void CGroundView::GetBackgroundLoadingBitmap()
{
	int nXY;IBitmap* pBitmap;
	while(m_jpgLoader.GetBmp(nXY,&pBitmap))
	{
		if(m_pMapGrid[nXY].pBmp == NULL)
		{
			if(pBitmap)
				m_pMapGrid[nXY].SetLoaded(pBitmap);
			else
				m_pMapGrid[nXY].SetLoadError();
		}
		else
		{
			if(pBitmap)
				pBitmap->Release();
		}
	}
}
//------------------------------------------------------------------------
void CGroundView::OnDirtyGridFound(int nGridX,int nGridY)
{
#ifndef _NO_USE_THREAD
	int index = nGridY * m_nTotalGridX + nGridX;
	if (m_pMapGrid[index].IsEmptyBitmap()) return;
	if(m_pMapGrid[index].IsNeedLoad())
	{
		// 从垃圾回收站里找找看
		IBitmap* pBmp = m_BitmapRecycle.GetObj(m_pMapGrid[index].wBmpIndex);
		if(pBmp)
		{
			m_pMapGrid[index].SetLoaded(pBmp);
			return;
		}

		// 找不到只好去解码了
		m_pMapGrid[index].SetReqesting();
		m_jpgLoader.AddJpg(m_pMapGrid[index].wBmpIndex,index,0,NULL,m_BitmapRecycle.IsFull() ? m_BitmapRecycle.PopHead() : NULL);
	}
#endif
}
//------------------------------------------------------------------------
void CGroundView::OnDirtyGridLost(int nGridX,int nGridY)
{
#ifndef _NO_USE_THREAD
	int index = nGridY * m_nTotalGridX + nGridX;
	if (m_pMapGrid[index].IsEmptyBitmap()) return;
	if (m_pMapGrid[index].IsLoaded())
		m_BitmapRecycle.AddTail(m_pMapGrid[index].wBmpIndex,m_pMapGrid[index].pBmp);
	else if (m_pMapGrid[index].IsRequesting())
	{
		IBitmap* pBmp = NULL;
		m_jpgLoader.RemoveOldJpg(m_pMapGrid[index].wBmpIndex,&pBmp); // 可能正在请求解码的jpg中就有这张就要被淘汰的
		if(pBmp)
			m_BitmapRecycle.AddGarbageToHead(-1,pBmp);
	}
	m_pMapGrid[index].Init();
#endif
}
//------------------------------------------------------------------------
void CGroundView::ScrollViewport(int dx, int dy)
{
	if (!m_ScrollSheet.ScrollViewport(dx,dy))
		return;
	CalcViewTileRect();
}
//------------------------------------------------------------------------
// 这里的rc定义成{tx,ty,col,row},(tx,ty)表示左上角tile坐标，(col,row)分别表示屏幕横向和纵向的格子数
void CGroundView::CalcViewTileRect()
{
	SetRect(&m_rcViewportRect,m_ScrollSheet.GetViewTopX(),m_ScrollSheet.GetViewTopY(),
		m_ScrollSheet.GetViewTopX() + m_ScrollSheet.GetViewWidth(),
		m_ScrollSheet.GetViewTopY() + m_ScrollSheet.GetViewHeight());

	m_rcViewportTileRect = m_MapCo.PixelRectToAreaTileRect(m_rcViewportRect);
}
//------------------------------------------------------------------------
// 返回块的左上角的坐标
void CGroundView::PixelToViewTop(const POINT &ptTile, POINT &ptTileCenter) const
{
	ptTileCenter = ptTile;
	ptTileCenter.x -= m_ScrollSheet.GetViewTopX();
	ptTileCenter.y -= m_ScrollSheet.GetViewTopY();
}
//------------------------------------------------------------------------
// 返回块的左上角的坐标
void CGroundView::TileToPixel(const POINT &ptTile, POINT &ptTileCenter) const
{
	m_MapCo.TileToPixel(ptTile,ptTileCenter);
	ptTileCenter.x -= m_ScrollSheet.GetViewTopX();
	ptTileCenter.y -= m_ScrollSheet.GetViewTopY();
}
//------------------------------------------------------------------------
void CGroundView::PixelToTile(const POINT &ptScreen, POINT &ptTile) const
{
	POINT ptWorld;
	ptWorld.x = ptScreen.x + m_ScrollSheet.GetViewTopX();
	ptWorld.y = ptScreen.y + m_ScrollSheet.GetViewTopY();
	m_MapCo.PixelToTile(ptWorld,ptTile);
}
//------------------------------------------------------------------------
void CGroundView::DrawTileLine(ICanvas *pCanvas)
{
/*
	|
	| A\
	+------+
	| B/   |
	|  \   |
	+------+
	| C/
	|

*/
	// 找到(kx,ky)
	int nViewTopX = m_ScrollSheet.GetViewTopX();
	int nViewTopY = m_ScrollSheet.GetViewTopY();
	POINT ktl = {nViewTopX / 64,nViewTopY / 32};

	int col = m_ScrollSheet.GetViewWidth() / 64 + 2;
	int row = m_ScrollSheet.GetViewHeight() / 32 + 2;
	int w =  col * 64;
	int h = col * 32;

	// A区只画\线
	int x = -(nViewTopX - ktl.x * 64);
	int y = -(nViewTopY - ktl.y * 32) - h + 16;
	for(int i = 0; i < col; i++)
	{
		POINT ptFrom = {x,y};
		POINT ptTo = {x + w, y + h};
		pCanvas->DrawLine(ptFrom,ptTo,TILE_LINE_COLOR);
		y += 32;
	}

	// B区画\/两种斜线
	for(int i = 0; i < row; i++)
	{
		POINT ptFrom = {x,y};
		POINT ptTo = {x + w, y + h};
		pCanvas->DrawLine(ptFrom,ptTo,TILE_LINE_COLOR);
		ptFrom.x = x; ptFrom.y = y;
		ptTo.x = x + w; ptTo.y = y - h;
		pCanvas->DrawLine(ptFrom,ptTo,TILE_LINE_COLOR);

		y += 32;
	}

	// C区画/线
	for(int i = 0; i < col; i++)
	{
		POINT ptFrom = {x,y};
		POINT ptTo = {x + w, y - h};
		pCanvas->DrawLine(ptFrom,ptTo,TILE_LINE_COLOR);

		y += 32;
	}
}
//------------------------------------------------------------------------