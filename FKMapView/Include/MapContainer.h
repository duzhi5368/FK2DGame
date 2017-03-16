/**
*	created:		2012-7-1   2:38
*	filename: 		MapContainer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/MapInfo/MapItem.h"
#include "../../FKGame/MapInfo/Tile.h"
#include "../Include/MapCoordinate.h"
#include "../Include/Geometry.h"
#include "../Include/FindPath_R.h"
#include <list>
using std::list;
//------------------------------------------------------------------------
/*
#ifdef _USE_QUICK_SPEED
// 地图保存、加载时的网格块大小设定：(为了避免乘除运算，请遵循下面的设定，
// 既是64(32)的倍数，又是2的整数次幂)
	#ifndef TILES_GRID_SHIFT
		#define TILES_GRID_SHIFT		10	// (>=6)
	#endif
	#define TILES_GRID_WIDTH			(64*(2^(TILES_GRID_SHIFT-6)))	// = 64*2^4 = 64*16 = 1024
	#define TILES_GRID_HEIGHT			(32*(2^(TILES_GRID_SHIFT-6)))	// = 32*2^4 = 32*16 = 512

	#ifndef TILES_PREREAD_SHIFT
		#define TILES_PREREAD_SHIFT		3
	#endif
	#define PREREAD_GRID_ROW			(2^TILES_PREREAD_SHIFT)		// 8X8 GRID BLOCK
	#define PREREAD_GRID_COL			(2^TILES_PREREAD_SHIFT)
#else
	#ifndef TILES_GRID_WIDTH
		#define TILES_GRID_WIDTH		1024//(64*10)
	#endif
	#ifndef TILES_GRID_HEIGHT
		#define TILES_GRID_HEIGHT		512//(32*10)
	#endif
	#ifndef TILES_PREREAD_GRID_ROW
		#define TILES_PREREAD_GRID_ROW	12
	#endif
	#ifndef TILES_PREREAD_GRID_COL
		#define TILES_PREREAD_GRID_COL	12
	#endif
	#ifndef LIST_PREREAD_GRID_ROW
		#define LIST_PREREAD_GRID_ROW	15 // 保证与TILES_PREREAD_GRID_ROW的差是偶数，保持为2的幂
	#endif
	#ifndef LIST_PREREAD_GRID_COL
		#define LIST_PREREAD_GRID_COL	15
	#endif
#endif
#define TILES_PREREAD_WIDTH				TILES_GRID_WIDTH*TILES_PREREAD_GRID_COL
#define TILES_PREREAD_HEIGHT			TILES_GRID_HEIGHT*TILES_PREREAD_GRID_ROW
#define LIST_PREREAD_WIDTH				LIST_GRID_WIDTH*LIST_PREREAD_GRID_COL
#define LIST_PREREAD_HEIGHT				LIST_GRID_HEIGHT*LIST_PREREAD_GRID_ROW
#define OBJECT_GRID_WIDTH				TILES_GRID_WIDTH
#define OBJECT_GRID_HEIGHT				TILES_GRID_HEIGHT
#define LIST_GRID_WIDTH					TILES_GRID_WIDTH
#define LIST_GRID_HEIGHT				TILES_GRID_HEIGHT
#ifdef _USE
	#define OBJECT_PREREAD_WIDTH			OBJECT_GRID_WIDTH*4
	#define OBJECT_PREREAD_HEIGHT			OBJECT_GRID_HEIGHT*4
#else
	#define OBJECT_PREREAD_WIDTH			OBJECT_GRID_WIDTH*12
	#define OBJECT_PREREAD_HEIGHT			OBJECT_GRID_HEIGHT*12
#endif*/
//------------------------------------------------------------------------
#define SHIFT_WIDTH			11
#define SHIFT_HEIGHT		10
#define GRID_WIDTH			2048
#define GRID_HEIGHT			1024
#define TILE_GRID_ROW		5
#define TILE_GRID_COL		5
#define OBJECT_GRID_ROW		5
#define OBJECT_GRID_COL		5
#define LIST_GRID_ROW		15
#define LIST_GRID_COL		15
//------------------------------------------------------------------------
#define TILES_PREREAD_WIDTH		GRID_WIDTH*TILE_GRID_COL
#define TILES_PREREAD_HEIGHT	GRID_HEIGHT*TILE_GRID_ROW
//------------------------------------------------------------------------
#ifndef PixelCood2GridCood
#define PixelCood2GridCood(PixelPos, GridLength)	((PixelPos + GridLength - 1) / GridLength)
#endif
//------------------------------------------------------------------------
enum DynamicMapUpdateType{
	mdNorth=0,		// 动态地图网格向北移动一行
	mdEastNorth,
	mdEast,
	mdEastSouth,
	mdSouth,
	mdWestSouth,
	mdWest,
	mdWestNorth,
	mdAll,			// 当动态地图移动的步长不是1(行/列)时，用这个方向

	mdMaxUpdateType
};
//------------------------------------------------------------------------
struct IItemClassFactory;
//------------------------------------------------------------------------
struct SNAP_ITEM
{
	POINT		ptTile;
	CMapItem*	pItem;
};
//------------------------------------------------------------------------
struct SNAP_TILE
{
	POINT		ptTile;
	CTile*		pTile;
};
//------------------------------------------------------------------------
struct BLOCK_INFO
{
	POINT	ptLeftTop;	// 块的左上角TILE的中心点像素坐标
	int		nIndex;		// 块索引
};
//------------------------------------------------------------------------
#define MAX_SNAP_ITEM_COUNT		4096
// vc默认堆栈大小设置为1M，如果此局部变量超过此值，需改一下设置
#define MAX_SNAP_BUF_SIZE		(MAX_SNAP_ITEM_COUNT * sizeof(SNAP_ITEM))
#define MAP_ARCHIVE_VERSION		0x00010102
//------------------------------------------------------------------------
struct MAP_ARCHIVE_HEADER
{
	DWORD		dwSize;				// 从MAP_ARCHIVE_HEADER起始地址开始数据的总长度
	DWORD 		dwVersion;			// 版本信息
	int			nMapWidth;			// 象素宽度
	int			nMapHeight;			// 象素高度
	char		szGroundJpgDir[16];	// 地表jpg文件所在的目录名，如"草地"
	int			nGridWidth;			// 网格宽度
	int			nGridHeight;		// 网格高度
	BOOL		bSaveItemOcc;		// 是否保存对象的占位信息
	char		szFill[100];		// 填充信息

	BOOL IsValid()
	{
		return dwVersion <= MAP_ARCHIVE_VERSION && nMapWidth > 0 && nMapHeight > 0 && dwSize > sizeof(MAP_ARCHIVE_HEADER);
	}
	void Init(int w,int h,LPCSTR szGroundJpgDir)
	{
		memset(this,0,sizeof(MAP_ARCHIVE_HEADER));
		dwVersion = MAP_ARCHIVE_VERSION;
		nMapWidth = w;
		nMapHeight = h;
		lstrcpyn(this->szGroundJpgDir,szGroundJpgDir,sizeof(this->szGroundJpgDir)); // 注意：有可能占用szFile[0]
		dwSize = sizeof(MAP_ARCHIVE_HEADER);
	}
};
//------------------------------------------------------------------------
// 地图上的一行
struct MAP_ARHCIVE_LINE_HEADER
{
	int		nFromX;			// 起始块偏移
	int		nTileCount;		// 块数量
};
//------------------------------------------------------------------------
struct ITrace;
//------------------------------------------------------------------------
typedef list<CMapItem* >  MAP_ITEM_LIST;
//------------------------------------------------------------------------
class CMapContainer : public CPathFinder_R<CMapContainer>
{
private:
	POINT	m_ptTileOrigin;		// 菱形区域的原点在整个地图中的偏移
	int		m_nMatrixWidth;		// 地图TILE宽度，长宽肯定是相同的(都等于row + col)
	int		m_nMatrixHeight;
	RECT	m_rcMapTileRect;	// map块数组范围
	CTile*	m_pTiles;			// TILE数组
	DWORD*	m_pMapTable;		// 地图文档的映射表
	int		m_nMapWidth;		// 地图象素宽度
	int		m_nMapHeight;		// 地图象素高度
	RECT	m_rcMap;			// 整个map象素大小
	CMapCoordinate m_MapCo;		// 象素坐标与块坐标之间的计算
	BOOL	m_bUseDynamicMap;	// 是否使用动态地图来加载数据
	IArchive* m_pArchive;
	IItemClassFactory* m_pItemCF;
	CMultLenArrayCache<DWORD,CMapItem*> m_LayerListCache;
	DWORD* m_pMultiValueTable;	// 乘法表，当GetTile时避免乘法
	MAP_ITEM_LIST m_MapItemList; // 用于记录地图资源

	typedef void (CMapContainer::*ProcScrollTilesInfo)(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	static ProcScrollTilesInfo m_fnScrollTileInfo[mdMaxUpdateType];
public:
	char	m_szGround[256];
	
private:
	static CTile m_EmptyTile;	// 当输入了无效的坐标时，返回此TILE
public:
	BOOL IsBlock(POINTS ptPos) // 提供给寻路模块
	{
		CTile& tile = GetTile(ptPos.x, ptPos.y);
		return (!tile.IsValid() || tile.IsBlock());
	}
	BOOL IsMovableForMultiOcc(CMapItem* pItem, POINT ptFrom, POINT ptTo);	
	void SetUseDynamicMap(BOOL bUse)	{m_bUseDynamicMap = bUse;}
	BOOL IsUseDynamicMap()				{return m_bUseDynamicMap;}
	BOOL OnReadServerMap(IArchive* pArchive,ITrace* pTrace);
	BOOL OnReadServerMap112(IArchive* pArchive,ITrace* pTrace);
	int GetMapWidth()const		{return m_nMapWidth;}
	int GetMapHeight()const		{return m_nMapHeight;}
	void _MovePreReadRect_Top(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void _MovePreReadRect_RightTop(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void _MovePreReadRect_Right(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void _MovePreReadRect_RightDown(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void _MovePreReadRect_Down(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void _MovePreReadRect_LeftDown(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void _MovePreReadRect_Left(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void _MovePreReadRect_LeftTop(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void _MovePreReadRect_All(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void ScrollTilesInfo(BLOCK_INFO* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, int nDirection, POINT& ptNewTileOrigin, int dx, int dy);
	void InitBlockInfo(POINT& ptLeftTop, int nTileRow, int nTileCol);
	void LoadBlockInfo(POINT& ptLeftTop, int nIndex, int nTileRow, int nTileCol);
	void CopyBlockInfo(POINT& ptSrcLeftTop, POINT& ptDstLeftTop, int nTileRow, int nTileCol);
	void CopyBlockInfoForSurface(POINT& ptSrcLeftTop, POINT& ptDstLeftTop, int nTileRow, int nTileCol);
	void SetOriginTile(POINT& ptTileOrigin);
	POINT GetOriginTile()		{return m_ptTileOrigin;}

	// 是否已经初始化
	BOOL IsValid()
	{
		return m_pTiles != NULL;
	}

	// 调整块坐标矩阵，使之每块都在有效坐标系m_rcMap之内
	// 这里的rc定义成{tx,ty,col,row},(tx,ty)表示左上角tile坐标，(col,row)分别表示屏幕横向和纵向的格子数
	// 返回
	//	FALSE : 如果rc跟m_rcMap无交集或全部在无效区域
	BOOL AdjustTileRect(RECT &rc);

	// 块是否在地图数组中
	BOOL IsTileInMap(int tx,int ty) const
	{
		POINT ptOff = {tx - m_ptTileOrigin.x, ty - m_ptTileOrigin.y};
		return (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight);
	}

	// 块是否在地图数组中
	BOOL IsTileInMap(const POINT& ptTile) const
	{
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		return (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight);
	}

	// 是否为有效块，即坐标正确且块也是有效块
	BOOL IsValidTile(const POINT& ptTile)
	{
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
			return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x].IsValid();
		else
			return FALSE;
	}

	// 是否为有效块，即坐标正确且块也是有效块
	BOOL IsValidTile(int tx,int ty)
	{
		POINT ptOff = {tx - m_ptTileOrigin.x, ty - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
			return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x].IsValid();
		else
			return FALSE;
	}

	CTile& GetTile(const POINT& ptTile) const
	{
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
			return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
		else
			return m_EmptyTile;
	}

	CTile& GetTile(int tx, int ty) const
	{
		POINT ptOff = {tx - m_ptTileOrigin.x, ty - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
			return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
		else
			return m_EmptyTile;
	}

	// 不作坐标有效性检查，其它同GetTile()
	inline CTile& _GetTile(const POINT& ptTile) const
	{
		//return GetTile(ptTile);
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		ASSERT(ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight);
		return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
	}

	inline CTile& _GetTile(int tx, int ty) const
	{
		//return GetTile(tx, ty);
		POINT ptOff = {tx - m_ptTileOrigin.x, ty - m_ptTileOrigin.y};
		ASSERT(ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight);
		return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
	}

	CTile* GetTilePtr(const POINT& ptTile) const
	{
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
		{
			CTile* pTile = &m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
			if (pTile->IsValid())
				return pTile;
		}
		return NULL;
	}

	// 提供一个手段直接访问map二维数组
	inline CTile* GetMapArray()						{return m_pTiles;}
	int GetMapArrayWidth() const					{return m_nMatrixWidth;}
	int GetMapArrayHeight() const					{return m_nMatrixHeight;}
	const CMapCoordinate& GetMapCoordinary() const	{return m_MapCo;}

	// 创建一个新的空的map
	// nMapnWidth和nMapHeight为地图象素大小
	BOOL Create(int nMapWidth, int nMapHeight, RECT& rcPreRead, ITrace* pTrace);
	void Close();

	// 将当前地图容器现场保存或复盘
	// pItemCF用于创建对象
	BOOL OnSaveMap(IArchive* pArchive,LPCSTR szGroundJpgDir,ITrace* pTrace = NULL);
	BOOL OnLoadMap(IArchive* pArchive,IItemClassFactory* pItemCF, LPRECT lprcViewport, ITrace* pTrace = NULL);

	virtual BOOL AddItem(const POINT& ptTile, CMapItem* pItem, DWORD dwParam=0);
	virtual BOOL MoveItem(const POINT& ptFrom, const POINT& ptTo, CMapItem* pItem, DWORD dwParam=0);
	virtual BOOL RemoveItem(const POINT& ptTile, CMapItem* pItem, DWORD dwParam=0);

	virtual BOOL AddItemOccupant(const POINT& ptTile, CMapItem* pItem);
	virtual BOOL RemoveItemOccupant(const POINT& ptTile, CMapItem* pItem);

	// 增加一个需要按层排序的对象
	BOOL AddLayerItem(const POINT &pt, CMapItem *pItem);

	/* 说明：范围广播(基于快照的广播)
			快照式广播可以解决：广播过程中，范围内的现场已经发生改变，此时可能导致数组长度变化等
			快照方式广播适合于广播那些可能导致对象状态变化的消息
			快照取对象列表以后，向对象广播消息时，有些对象可能因某些消息而消失，
			因此需要检查此对象是否还有效，检查方法是pItem是否还在此tile上
	*/
	/*
	  功能：向逻辑矩形内广播，按从左到右从上到下的顺序。广泛用于范围广播，如攻击、聊天广播等
	  参数：
		输入：
			rc			: tile坐标系下的矩形
			nListCount	: pListBuf的最大维数
		输出：
			pListBuf	: 返回所有对象
			nListCount	: 返回 pListBuf 中有效的对象数量
	 */
	BOOL SnapshotRectItem(const RECT& rc,/*in out*/int& nListCount,/*out*/SNAP_ITEM* pListBuf);

	/*
	  功能：取得世界坐标系下的一个矩形范围内所有对象的快照，按从左到右从上到下的顺序
	  参数：
		输入：
			rcWorld		: 游戏世界象素坐标
			nListCount	: pListBuf的最大维数
		输出：
			pListBuf	: 返回所有对象
			nListCount	: 返回 pListBuf 中有效的对象数量
	 */

	// 功能同EnumItemByWorldRect()
	BOOL SnapshotItemByWorldRect(const RECT &rcWorld, /*in out*/int& nListCount,/*out*/SNAP_ITEM* pListBuf);

	/*
	  功能：取得世界坐标系下的一个矩形范围内所有tile的快照 
	  参数：
		输入：
			rcWorld		: 游戏世界象素坐标
			nListCount	: pListBuf的最大维数
		输出：
			pListBuf	: 返回所有tile
			nListCount	: 返回 pListBuf 中有效的对象数量
	 */
	// 功能同EnumTileByWorldRect()
	BOOL SnapshotTileByWorldRect(const RECT &rcWorld, /*in out*/int& nListCount,/*out*/SNAP_TILE* pListBuf);

	CMapContainer();
	virtual ~CMapContainer();

	BOOL OnLoadMap112(IArchive* pArchive,IItemClassFactory* pItemCF,LPRECT lprcViewport, ITrace* pTrace=NULL);
	BOOL OnAddMap112(IArchive* pArchive,IItemClassFactory* pItemCF,LPRECT lprcViewport, ITrace* pTrace=NULL);
	BOOL OnSaveMap112(IArchive* pArchive,LPCSTR szGroundJpgDir, int nGridWidth, int nGridHeight, ITrace* pTrace=NULL);
	void ViewRectToPreReadRect(RECT& rcView, RECT& rcPreRead, int nMapWidth, int nMapHeight);
	
private:
	BOOL _LoadBlock(IArchive* pArchive, POINT ptTileLeftTop, int nTileRow, int nTileCol, IItemClassFactory* pItemCF);
	BOOL _LoadTileInfo(IArchive* pArchive, CTile* pTile, POINT& ptTile, IItemClassFactory* pItemCF);
	BOOL _SaveBlock(IArchive* pArchive, POINT ptTileLeftTop, int nTileRow, int nTileCol);
	BOOL _SaveMultiEmptyTileInfo(IArchive* pArchive, int nCount);
	BOOL _SaveMultiTileInfo(IArchive* pArchive, POINT* ptTileList, int nCount);
	BOOL _SaveTileInfo(IArchive* pArchive, CTile* pTile);
	void AddMultiOccupantValue(POINT pt, CMapItem *pItem);
	void RemoveMultiOccupantValue(POINT pt, CMapItem *pItem);
	BOOL TestIntersect(POINT pt, CMapItem* pItem);
	
protected:
	BOOL CreateMapArray(RECT& rcPreRead);
};
//------------------------------------------------------------------------