/**
*	created:		2012-7-1   5:53
*	filename: 		MapView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKCanvas/IFKCanvas.h"
#include "../../FKGame/MapView/IMapView.h"
#include "../../FKGame/MapView/MapItemView.h"
#include "ScrollSheet.h"
#include "MapCoordinate.h"
#include "GroundView.h"
#include "MapContainer.h"
#include "MapArchiveMgr.h"
#include "MiniMap.h"
#include "TilesView.h"
#include "Weather.h"
#include <MMSystem.h>
#include <list>
using std::list;
//------------------------------------------------------------------------
#ifdef _DEBUG
#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
struct IItemClassFactory;
//------------------------------------------------------------------------
#define MAX_SHORT_NAME	32
#define CMP_LT	-1
#define CMP_EQ	0
#define CMP_GT	1
//------------------------------------------------------------------------
#ifdef _USE
#define SAVE_CANVAS_W	1280
#define SAVE_CANVAS_H	1024
#else
#define SAVE_CANVAS_W	SCR_W  * 1.5
#define SAVE_CANVAS_H	SCR_H  * 1.5
#endif
//------------------------------------------------------------------------
struct CMapItemViewPtr
{
	CMapItemView* pItem;
	int				nCount;
	CMapItemViewPtr() : pItem(NULL), nCount(0) {}
	BOOL operator == (const CMapItemViewPtr& item)
	{
		return pItem == item.pItem;
	}
	BOOL operator < (const CMapItemViewPtr& item)
	{
		return *pItem < *item.pItem;
	}
	CMapItemView* operator ->()
	{
		return pItem;
	}
	BOOL IsPoint()
	{
		return (pItem->m_ptLeft.x == pItem->m_ptRight.x && pItem->m_ptLeft.y == pItem->m_ptRight.y);
	}
	int Compare(CMapItemViewPtr& item)
	{
		return 0;
	}
	BOOL IsValid()
	{
		return (pItem != NULL);
	}
};
//------------------------------------------------------------------------
class CMapView : public IMapView
{
private:
	// 网格、占位相关
	DWORD				m_dwDrawFlag;			// 一些其他如网格、占位块等的绘制标志
	IBitmap*			m_pOccupantBitmap;		// 占位块对象
	// 容器相关
	CMapContainer		m_Map;					// 地图容器
	// 坐标系统相关
	CMapCoordinate		m_MapCo;				// 地图的坐标系统
	// 视图相关
	CGroundView			m_GroundView;			// 地表视图
	CTilesView			m_TilesView;			// 处理地图的动态加载
	// 绘图列表相关
	typedef list<CMapItemViewPtr> DisplayList;
	typedef list<CMapItemViewPtr>::iterator DisplayListPtr;
	DisplayList			m_DisplayList;			// 当前视图多占位块对象显示列表
	DisplayListPtr		m_iteratorCurItem;	// 全屏显示扫描时，跟可视多占位块对象列表中的对比较，记住当前的位置 
	BOOL				m_bReSortDrawItemList;	// 显示时是否需要将队列中的所有对象排序

	// 主角相关
	CMapItemView*		m_pPlayerItem;			// 主角对象
	POINT				m_ptPlayerTile;			// 主角所有的位置
	// 地图文档相关
	CMapArchiveMgr		m_MapArchiveMgr;		// 地图文档管理器
	char				m_szGrondFileDirName[MAX_SHORT_NAME]; // 记住创建和load地图时传入的ground一级文件目录名，如"grass"
	char				m_szMapName[MAX_SHORT_NAME];
	CWeather			m_Weather;				// 天气

	// 小地图相关
	CMiniMap			m_MiniMap;
	BOOL				m_bMouseInMiniMap;
	POINT				m_ptMousePos;
	BOOL				m_bOptimizedScrollScreen;	// 优化的缓冲移屏

	DisplayList			m_VisableListGround;
	DisplayList			m_VisableListObj;
	DisplayList			m_VisableListMultiOcc;

	IBitmap*			m_pSaveBmp;
	ICanvas*			m_pSaveCanvas;

	IBitmap*			m_pMengBmp;
	IBitmap*			m_pWngBmp;

	IBitmap*			m_pMengBmpBig;
	IBitmap*			m_pWngBmpBig;

	IBitmap*			m_pMiniBmp;
	ICanvas*			m_pMiniCanvas;

	IBitmap*			m_pMiniBmpBig;
	ICanvas*			m_pMiniCanvasBig;

	list<RECT>			m_DirtyRectList;

	POINT				m_ptOldSave;
	POINT				m_ptSave;

public:
	// ---------------------------------------------------------------------------------
	// 小地图需要知道地图上的逻辑对象列表
	// 包括怪物,NPC,玩家,不包括LOAD地图时加载的树木，房子
	typedef list<CMapItemView*> LogicList;
	typedef list<CMapItemView*>::iterator LogicListPtr;
	LogicList			m_LogicItemList;

public:
	BOOL IsItemInViewArea(const POINT &ptTile, CMapItemView *pItemView);
	BOOL IsItemInLogicArea(const POINT &ptTile, CMapItemView *pItemView);
	BOOL IsItemVisable(const POINT& ptTile,CMapItemView *pItemView);
	// 关闭当前地图、创建一个空地图
	BOOL CreateNewMap(int nMapWidth,int nMapHeight,
		LPRECT lprcViewport, LPRECT lprcMiniMapViewport,
		LPCSTR szGroundJpgFileDir, IFileObject* pGridIdxArrayFile, ITrace* pTrace);
	BOOL LoadMapView(IFileObject* pMapFile, IFileObject* pGridIdxArrayFile, IFileObject* pTinyMapFile,
		LPCSTR szGroundJpgFileDir, LPCSTR szMapName, LPRECT lprcViewport, 
		LPRECT lprcMiniMapViewport, LPCSTR szMiniMapJpgFileDir, BOOL bUseDynamicMap, int nMiniMapRate, 
		IItemClassFactory* pItemCF, ITrace* pTrace);
	// 用CreateNewMap()创建一个新地图之后，或者OnSerialize(sRead)之后，调用此函数创建可见地表视图层
	// szGroundJpgFileDir为地表jpg文件所在的目录
	// szGridIndexArray为地表块拼接索引文件
	BOOL CreateGroundView(LPRECT lprcViewport, IItemClassFactory* pItemCF, 
		LPCSTR szGroundJpgFileDir, IFileObject* pGridIdxArrayFile=NULL);

	virtual BOOL FindPath(POINTS ptFrom , POINTS ptTo , POINTS** ppBuffer , int& nPathLen);
	virtual BOOL FindSimplePath(POINTS ptFrom , POINTS ptTo , POINTS** ppBuffer , int& nPathLen);

	virtual int GetMapWidth()const		{return m_Map.GetMapWidth();}
	virtual int GetMapHeight()const		{return m_Map.GetMapHeight();}

	// 可视窗口矩阵，返回绝对象素坐标
	virtual const RECT& GetViewportRect()const{return m_GroundView.GetViewportRect();};
	// 视口大小改变
	virtual void OnSizeViewport(int nViewWidth,int nViewHeight);

	// 滚动屏幕
	virtual BOOL ScrollViewport(int dx, int dy);

	// 绘制地图,rc为地图屏幕区域，暂不用
	virtual void OnDraw(ICanvas* pCanvas, const void* pParam);
	// 画矩形网格
	virtual void DrawRectangleGrid(ICanvas* pCanvas, int nGridWidth, int nGridHeight, WPixel clrLine, WPixel clrText);

	// 设置是否在地表上画网格
	virtual void SetDrawFlag(DWORD dwFlag, DWORD dwClearFlag, void* pParam = 0)
	{
		m_dwDrawFlag |= dwFlag;
		if(dwClearFlag)
			m_dwDrawFlag &= ~dwClearFlag;

		if(dwFlag & maskDrawOccupant)
			m_pOccupantBitmap = (IBitmap*)pParam;
	}

	virtual DWORD GetDrawFlag(DWORD dwFlag)
	{
		return m_dwDrawFlag & dwFlag;
	}

	virtual BOOL AddItem(const POINT& ptTile, CMapItemView* pItem, DWORD dwParam=0);
	virtual BOOL MoveItem(const POINT& ptTileFrom, const POINT& ptTileTo, CMapItemView* pItem, DWORD dwParam=0);
	virtual BOOL RemoveItem(const POINT& ptTile, CMapItemView* pItem, DWORD dwParam=0);

	// 加入对象占位信息
	virtual BOOL AddItemOccupant(const POINT& ptTile, CMapItemView* pItem);

	// 删除对象占位信息
	virtual BOOL RemoveItemOccupant(const POINT& ptTile, CMapItemView* pItem);

	virtual BOOL SetPlayerItem(CMapItemView* pItem)
	{
		m_pPlayerItem = pItem;
		return TRUE;
	}
	virtual CMapItemView* GetPlayerItem()
	{
		return m_pPlayerItem;
	}

	virtual BOOL SnapshotTileByWorldRect(const RECT& rcWorld, IN OUT int& nListCount, OUT SNAP_TILE_INFO* pListBuf)
	{
		return m_Map.SnapshotTileByWorldRect(rcWorld,nListCount, (SNAP_TILE*)pListBuf);
	}

	virtual BOOL SnapshotItemByWorldRect(const RECT& rcWorld, IN OUT int& nListCount, OUT SNAP_ITEM_INFO* pListBuf)
	{
		return m_Map.SnapshotItemByWorldRect(rcWorld,nListCount, (SNAP_ITEM*)pListBuf);
	}

	virtual CMapItemView* HitTest(const RECT& rcScreen, POINT* pPointFound = NULL, DWORD dwHitFlag = 0);

	// 读写地图存档
	// bServerMap:
	// 		用bServerMap标志区分是保存服务器专用地图还是客户端地图，
	//		两者的区别时：服务器端地图不需要地上物，只需要CTile数据即可
	//		只对图存档有效
	virtual BOOL OnSerialize(BOOL bWrite, IArchive* pArchive, IItemClassFactory *pItemCF, LPRECT lprcViewport, ITrace* pTrace);

	// 计算鼠标位置所在的块坐标
	virtual void ScreenToTile(const POINT& ptScreen, POINT &ptTile) const
	{
		m_GroundView.PixelToTile(ptScreen,ptTile);
	}
	// 返回块的左上角的坐标
	virtual void TileToScreen(const POINT &ptTile, POINT &ptTileCenter) const
	{
		m_GroundView.TileToPixel(ptTile,ptTileCenter);
	}

	// 返回块的世界坐标
	virtual void TileToWorld(const POINT &ptTile, POINT &ptTileCenter) const
	{
		m_MapCo.TileToPixel(ptTile,ptTileCenter);
	}
	// 返回世界坐标对应的块坐标
	virtual void WorldToTile(const POINT& ptWorld, POINT &ptTile) const
	{
		m_MapCo.PixelToTile(ptWorld,ptTile);
	}

	// 根据块坐标返回地表块CTile
	virtual CTile* GetTile(const POINT& ptTile) const
	{
		return &m_Map.GetTile(ptTile);
	}

	// 根据屏幕象素坐标取得块指针
	// 总是返回有效地址的CTile，根据CTile.IsValid()判断此tile是否有效
	virtual CTile* GetTileFromScreen(POINT ptScreen) const
	{
		POINT ptTile;
		m_GroundView.PixelToTile(ptScreen,ptTile);
		return &m_Map.GetTile(ptTile);
	}

	// 检查tile坐标是否为有效坐标
	virtual BOOL IsValidTile(const POINT& ptTile)
	{
		return m_Map.GetTile(ptTile).IsValid();
	}
	POINT GetPlayerTile()		{return m_ptPlayerTile;}

	virtual void Release()
	{
		Close();
#ifdef _DEBUG
		OutputDebugString("IMapView::Release()");
#endif
		delete this;
	}

	virtual int GetMapTileWidth()
	{
		return m_MapCo.GetMatrixWidth();
	}

	virtual int GetMapTileHeight()
	{
		return m_MapCo.GetMatrixHeight();
	}

	virtual BOOL ScrollToCenter(POINT ptScreenCenter);

	virtual int GetViewTopLeftX() const
	{
		return m_GroundView.GetViewTopX();
	}
	virtual int GetViewTopLeftY() const
	{
		return m_GroundView.GetViewTopY();
	}

	CMapContainer& GetMapContainer()
	{
		return m_Map;
	}

	CMapCoordinate& GetMapCoordinate()
	{
		return m_MapCo;
	}

	CMapView();
	~CMapView();
	void Close();

	virtual int OnDrawOccupant(const POINT& ptTile,CTile* pTile,void* pParam);
	virtual void OnMapItemEnterViewport(const POINT& ptTile,CMapItem* pItem,void* pParam);
	virtual void OnMapItemLeaveViewport(const POINT& ptTile,CMapItem* pItem,void* pParam);

	// 小地图相关
	virtual void SetMiniMapVisible(BOOL bVisible);
	virtual void SetMiniMapViewport(LPRECT lprcViewport);
	virtual void SetMiniMapTransparent(int nAlpha);
	virtual BOOL GetTinyMapVisible();
	virtual void SetTinyMapVisible(BOOL bVisible);

	// 设置小地图大小
	virtual void SetMiniMapBigOrSmall(BOOL bBig);

	// 改变地图天气
	virtual void ChangeWeather(SWeatherInfo info);

	virtual BOOL QuickFillOccupant(POINT& ptTileInRect, int nGridWidth, int nGridHeight, int nTileRows, int nTileCols);

	// 缓冲移屏的支持
	virtual void SetOptimizedScrollScreen(BOOL bOptimized)	{m_bOptimizedScrollScreen = bOptimized;}
	virtual BOOL GetOptimizedScrollScreen()					{return m_bOptimizedScrollScreen;}

	// 当有空余时间时，让地图预先处理一些东西
	virtual void OnHandleIdle(DWORD dwParam);

	// 为了放止不停的更新接口，这里加入一个抽象的方法，当需要加入功能而不希望修改接口时，可以暂时用这个方法来实现
	virtual int OnSomeHandle(DWORD dwParam1, DWORD dwParam2);
private:
	void AddItemToDisplayList(const POINT& ptTile, CMapItemView* pItem);
	void UpdateItemInDisplayList(const POINT& ptTile, CMapItemView* pItem, BOOL bVisible);
	void RemoveItemFromDisplayList(CMapItemView* pItem);
	void AddItemToLogicList(CMapItemView* pItem);
	void RemoveItemFromLogicList(CMapItemView* pItem);
public:
	void DrawObjectInMiniMap(ICanvas *pCanvas, ITTFont* pFont);
	IArchive* GetMapArchive();
	BOOL IsIntersect(CMapItemView& item1, CMapItemView& item2, RECT& rc);
	int CmpPointPoint(CMapItemView& item1, CMapItemView& item2);
	int CmpPointLine(CMapItemView& item1, CMapItemView& item2);
	int CmpLineLine(CMapItemView& item1, CMapItemView& item2);
	int Compare(CMapItemView& item1, CMapItemView& item2);
	void AddSortItem0(CMapItemView* pItem, DisplayList& _DisplayList);
	void UpdateSortItem0(CMapItemView* pItem, DisplayList& _DisplayList);
	void AddSortItem1(CMapItemView* pItem, DisplayList& _DisplayList);
	void UpdateSortItem1(CMapItemView* pItem, DisplayList& _DisplayList);
	void AddSortItem2(CMapItemView* pItem, DisplayList& _DisplayList);
	void UpdateSortItem2(CMapItemView* pItem, DisplayList& _DisplayList);
	void AddSortItemPoint(CMapItemView* pItem, DisplayList& _DisplayList);
	void AddSortItem3(CMapItemView* pItem, DisplayList& _DisplayList);
	void AddSortItem31(CMapItemView* pItem, DisplayList& _DisplayList);
	void UpdateSortItem3(CMapItemView* pItem, DisplayList& _DisplayList);
	void AddPointToList();

private:
	BOOL SaveMapBitmapToFile(LPCSTR szFileName, int nStartRow, int nStartCol, int nRows, int nCols, 
		float fRate, BOOL bDrawGround, BOOL bCombine);
	BOOL StretchBitmap(LPCSTR szFileName, int nNewWidth, int nNewHeight);

#ifdef _DEBUG
private:
	void DrawRect(ICanvas *pCanvas, RECT& rc, WPixel clr);
#endif
};
//------------------------------------------------------------------------
