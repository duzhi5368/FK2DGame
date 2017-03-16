/**
*	created:		2012-7-1   2:10
*	filename: 		GroundView
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
#define GROUND_CACHE_BITMAP_COUNT	60
#else
#define GROUND_CACHE_BITMAP_COUNT	60
#endif
//------------------------------------------------------------------------
class CGroundView : public IScrollSheetSink
{
private:
	int m_nMapWidth;		// 地图大小
	int m_nMapHeight;
	RECT m_rcMapRect;		// 整个地图象素矩形，用于视口是否越界检查
	int m_nViewWidth;		// 视口大小
	int m_nViewHeight;
	int m_nGridWidth;		// 一个单元格的大小
	int m_nGridHeight;		
	int	m_nTotalGridX;		// x,y方向上格子总数，经常用到，所以计算好
	int	m_nTotalGridY;
	RECT m_rcViewportRect;		// 视口的象素矩阵，绝对坐标
	RECT m_rcViewportTileRect;	// 视口的tile矩阵
	CJpegLoader m_jpgLoader;
	CMapCoordinate	m_MapCo;	// 坐标转换
	CScrollSheet	m_ScrollSheet;
	CFIFOCache<IBitmap*,int,GROUND_CACHE_BITMAP_COUNT> m_BitmapRecycle;	

public:
	int GetIndexByPoint(POINT& pt);
	void DrawTileLine(ICanvas* pCanvas);
	void PixelToTile(const POINT &ptScreen, POINT &ptTile) const;
	void PixelToViewTop(const POINT &ptTile, POINT &ptTileCenter) const;
	void TileToPixel(const POINT &ptTile, POINT &ptTileCenter) const;
	void ScrollViewport(int dx, int dy);
	void OnDraw(ICanvas* pCanvas);


	const RECT& GetViewportRect() const
	{
		return m_rcViewportRect;
	}
	const RECT& GetViewportTileRect() const
	{
		return m_rcViewportTileRect;
	}
	const CMapCoordinate& GetMapCoordinary() const
	{
		return m_MapCo;
	}
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
	void Close();
	BOOL Create(int nMapWidth,int nMapHeight,int nGridWidth,int nGridHeight,
		LPRECT lprcViewport, const char* szJpgFileDir = NULL, IFileObject* pGridIdxArrayFile = NULL, ITrace*pTrace = NULL);
	BOOL MoveViewportTo(int x,int y);
	BOOL SizeViewport(int nWidth,int nHeight);
	CGroundView()
	{
		m_nMapWidth = 0;
		m_nMapHeight = 0;
		m_nViewWidth = 0;
		m_nViewHeight = 0;
		m_nTotalGridX = 0;
		m_nTotalGridY = 0;
		m_nGridHeight = 400;
		m_nGridWidth = 300;
		m_pMapGrid = NULL;

		memset(&m_rcViewportTileRect,0,sizeof(m_rcViewportTileRect));
		memset(&m_rcMapRect,0,sizeof(m_rcMapRect));
	}

	~CGroundView()
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
	public:
		IBitmap*	pBmp;
		WORD		status;
		WORD		wBmpIndex;
	};
	MAP_GRID*		m_pMapGrid;	// 记录某块的bitmap是否已经读进来

protected:
	void CalcViewTileRect();
};
//------------------------------------------------------------------------
