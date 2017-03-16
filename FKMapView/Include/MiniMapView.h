/**
*	created:		2012-7-1   2:55
*	filename: 		MiniMapView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "JpegLoader.h"
#include "ScrollSheet.h"
#include "MapCoordinate.h"
//------------------------------------------------------------------------
#ifdef _DEBUG	
	#define MINIMAP_CACHE_BITMAP_COUNT	60
#else
	#define MINIMAP_CACHE_BITMAP_COUNT	16
#endif
//------------------------------------------------------------------------
class CMiniMapView : public IScrollSheetSink
{
	int m_nMapWidth;		// 地图大小
	int m_nMapHeight;
	RECT m_rcMapRect;		// 整个地图象素矩形，用于视口是否越界检查
	int m_nViewWidth;		// 视口大小
	int m_nViewHeight;
	int m_nGridWidth;		// 一个单元格的大小
	int m_nGridHeight;		
	int	m_nTotalGridX;		// x,y方向上格子总数，经常用到，所以计算好
	int	m_nTotalGridY;
	int	m_nAlpha;			// 小地图绘制时的alpha值

	CJpegLoader m_jpgLoader;
	CScrollSheet	m_ScrollSheet;
	CFIFOCache<IBitmap*,int,MINIMAP_CACHE_BITMAP_COUNT> m_BitmapRecycle;	
	POINT		m_ptDstOffset;

public:
	void ScrollViewport(int dx, int dy);
	void OnDraw(ICanvas* pCanvas);

	int GetMapWidth() const
	{
		return m_nMapWidth;
	}
	int GetMapHeight() const
	{
		return m_nMapHeight;
	}

	const RECT& GetMapRect() const
	{
		return m_rcMapRect;
	}

	int GetViewTopX() const
	{
		return m_ScrollSheet.GetViewTopX();
	}

	int GetViewTopY() const
	{
		return m_ScrollSheet.GetViewTopY();
	}

	int GetViewWidth() const
	{
		return m_nViewWidth;
	}
	int GetViewHeight() const
	{
		return m_nViewHeight;
	}
	void SetOffset(POINT ptOffset)
	{
		m_ptDstOffset = ptOffset;
	}
	POINT& GetOffset()
	{
		return m_ptDstOffset;
	}
	void Close();
	BOOL Create(int nMapWidth,int nMapHeight,int nGridWidth,int nGridHeight,
		LPRECT lprcViewport, const char* szJpgFileDir = NULL, ITrace*pTrace = NULL);
	BOOL MoveViewportTo(int x,int y);
	BOOL SizeViewport(int nWidth,int nHeight);
	void SetAlpha(int nAlpha)	{m_nAlpha = nAlpha;}
	int  GetAlpha()				{return m_nAlpha;}
	CMiniMapView()
	{
		m_nMapWidth = 0;
		m_nMapHeight = 0;
		m_nViewWidth = 0;
		m_nViewHeight = 0;
		m_nTotalGridX = 0;
		m_nTotalGridY = 0;
		m_nGridHeight = 200;
		m_nGridWidth = 150;
		m_pMapGrid = NULL;
		m_nAlpha = 256;
		m_ptDstOffset.x = m_ptDstOffset.y = 0;
		memset(&m_rcMapRect,0,sizeof(m_rcMapRect));
	}

	~CMiniMapView()
	{
		Close();
	}


private:
	void GetBackgroundLoadingBitmap();
	virtual void OnGridChanged(int nMoveDirection, int nDx, int nDy){}
	virtual void OnDirtyGridFound(int nGridX,int nGridY);	// 发现一个脏块
	virtual void OnDirtyGridLost(int nGridX,int nGridY);	// 发现一个脏块已经太远，因此可以考虑删除此脏块

	struct MAP_GRID
	{
		enum STATUS{None = 0,Requesting = 1/*正在请求后台解码*/, Loaded = 2/*已经加载完毕*/,LoadError = 3/*解码出错*/};
		MAP_GRID()
		{
			Init();
			wBmpIndex = 0xFFFF;
		}
		void Init()
		{
			pBmp = NULL;
			status = None;
		}
		void SetReqesting(){status = Requesting;}
		void SetLoaded(IBitmap* pBmp)
		{
			this->pBmp = pBmp;
			status = Loaded;
		}
		void SetLoadError(){status = LoadError;pBmp = NULL;}
		
		BOOL IsLoaded(){return pBmp != NULL;}
		BOOL IsNeedLoad(){return status == None;}
		BOOL IsRequesting(){return status == Requesting;}
		BOOL IsLoadError(){return status == LoadError;}
		BOOL IsEmptyBitmap() {return wBmpIndex == 0xFFFF;} // 此块为空，编辑时未填图
	public:
		int GetViewTopY();
		int GetViewTopX();

		IBitmap*	pBmp;
		WORD		status;
		WORD		wBmpIndex;
	};
private:
	MAP_GRID*		m_pMapGrid;	// 记录某块的bitmap是否已经读进来
};
//------------------------------------------------------------------------