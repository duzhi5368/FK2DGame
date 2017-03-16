/**
*	created:		2012-7-1   2:56
*	filename: 		MiniMapView
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
#include "../Include/MiniMapView.h"
#include "../Include/MapViewCommon.h"
//------------------------------------------------------------------------
#define _NO_USE_THREAD
#ifndef _NO_USE_THREAD
	#if defined(_USE) || defined(_DEBUG) || defined(_TEST)
		#define _NO_USE_THREAD
	#endif
#endif
//------------------------------------------------------------------------
// szGridIndexArray为地表块拼接索引文件
BOOL CMiniMapView::Create(int nMapWidth, int nMapHeight, int nGridWidth, int nGridHeight,
						 LPRECT lprcViewport, 
						 const char* szJpgFileDir, ITrace*pTrace)
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

	if(!m_jpgLoader.Create(szJpgFileDir,pTrace))
		return FALSE;

	int nTotalGrid = m_nTotalGridX * m_nTotalGridY;
	TraceLn("本地图小地图Grid数量 = %d", nTotalGrid);
	m_pMapGrid = new MAP_GRID[nTotalGrid];
	if(m_pMapGrid == NULL)
		return FALSE;

	for(int i = 0; i < nTotalGrid; i++)
		m_pMapGrid[i].wBmpIndex = (WORD)i;

	if (!m_ScrollSheet.Create(static_cast<IScrollSheetSink*>(this),
		nMapWidth, nMapHeight, nGridWidth, nGridHeight, lprcViewport))
		return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------
void CMiniMapView::Close()
{
	m_jpgLoader.Close();
	if(m_pMapGrid)
	{
		MAP_GRID *p = m_pMapGrid;
		for(int y = 0; y < m_nTotalGridY; y++)
			for(int x = 0; x < m_nTotalGridX; x++)
			{
				if(p->pBmp != NULL)
					p->pBmp->Release();
				p++;
			}
		delete m_pMapGrid;
		m_pMapGrid = NULL;
	}
}
//------------------------------------------------------------------------
BOOL CMiniMapView::SizeViewport(int nWidth, int nHeight)
{
	if(nWidth == 0 || nHeight == 0)
		return FALSE;
	m_nViewWidth = nWidth;
	m_nViewHeight = nHeight;
	m_ScrollSheet.SizeViewport(nWidth, nHeight);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniMapView::MoveViewportTo(int x, int y)
{
	m_ScrollSheet.MoveViewportTo(x,y);
	return TRUE;
}
//------------------------------------------------------------------------
// 把可见块显示出来，如果没发现相关块的IBitmap，则填成黑色
void CMiniMapView::OnDraw(ICanvas* pCanvas)
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
#ifdef _NO_USE_THREAD
				// 从垃圾回收站里找找看
				int index = (nGridTopY + y) * m_nTotalGridX + nGridTopX + x;
				IBitmap* pBmp = m_BitmapRecycle.GetObj(m_pMapGrid[index].wBmpIndex);
				if (pBmp)
					m_pMapGrid[index].SetLoaded(pBmp);
				else
				{
					// 找不到只好去解码了
					if (m_BitmapRecycle.IsFull())
						m_BitmapRecycle.PopHead();
					pBmp = m_jpgLoader.LoadJpgNoUseThread(m_pMapGrid[index].wBmpIndex);
					if (pBmp)
						m_pMapGrid[index].SetLoaded(pBmp);
				}
#else
				if (px->IsNeedLoad())
					OnDirtyGridFound(nGridTopX + x,nGridTopY + y);
				if (px->IsRequesting())
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
				if(px->IsLoadError())
				{
					char str[1024];
					wsprintf(str,"Warn : CMiniMapView::OnDraw() : can not load grid(%d,%d),load error\r\n",nGridTopX + x,nGridTopY + y);
					OutputDebugString(str);
				}
				else if(px->IsEmptyBitmap())
				{
					char str[1024];
					wsprintf(str,"Warn : CMiniMapView::OnDraw() : can not load grid(%d,%d),wait time out\r\n",nGridTopX + x,nGridTopY + y);
					OutputDebugString(str);
				}
#endif
			}
			if(px->pBmp)
			{
				DWORD dwStyle = BLIT_NORMAL;
				if (m_nAlpha >0 && m_nAlpha < 256)
				{
					dwStyle |= BLIT_TRANS;
					px->pBmp->SetUserData(m_nAlpha);
				}
				px->pBmp->Draw(pCanvas, 
					rcGridInScreen.left - nViewTopX, 
					rcGridInScreen.top - nViewTopY,
					rcGridInScreen.right - rcGridInScreen.left, // width
					rcGridInScreen.bottom - rcGridInScreen.top, // height
					rcGridInScreen.left - rcDirtyGrid.left,
					rcGridInScreen.top  - rcDirtyGrid.top, dwStyle);
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
void CMiniMapView::GetBackgroundLoadingBitmap()
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
void CMiniMapView::OnDirtyGridFound(int nGridX,int nGridY)
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
void CMiniMapView::OnDirtyGridLost(int nGridX,int nGridY)
{
#ifndef _NO_USE_THREAD
	int index = nGridY * m_nTotalGridX + nGridX;
	if (m_pMapGrid[index].IsEmptyBitmap()) return;
	if(m_pMapGrid[index].IsLoaded())
		m_BitmapRecycle.AddTail(m_pMapGrid[index].wBmpIndex,m_pMapGrid[index].pBmp);
	else if(m_pMapGrid[index].IsRequesting())
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
void CMiniMapView::ScrollViewport(int dx, int dy)
{
	if (!m_ScrollSheet.ScrollViewport(dx, dy))
		return;
}
//------------------------------------------------------------------------