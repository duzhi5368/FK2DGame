/**
*	created:		2012-6-21   22:26
*	filename: 		IMapView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifndef VERSION_MAPVIEW
#define VERSION_MAPVIEW			1
#endif
//------------------------------------------------------------------------
#include "../../FKCanvas/IFKCanvas.h"
//------------------------------------------------------------------------
class CTile;
class CMapItem;
class CMapItemView;
struct IArchive;
struct IItemClassFactory;
struct IFileObject;
//------------------------------------------------------------------------
// 与CMapContainer中的SNAP_TILE结构是一样的
struct SNAP_TILE_INFO  
{
	POINT		ptTile;
	CTile*		pTile;
};
//------------------------------------------------------------------------
// 与CMapContainer中的SNAP_ITEM结构是一样的
struct SNAP_ITEM_INFO 
{
	POINT		ptTile;
	CMapItem*	pItem;
};
//------------------------------------------------------------------------
// 天气类型
enum enWeatherType
{
	enWeather_Type_None = 0,		// 无天气效果
	enWeather_Type_Rain,			// 下雨
	enWeather_Type_Snow,			// 下雪
	enWeather_Type_MeiGui,			// 下玫瑰花瓣
};
//------------------------------------------------------------------------
// 天气信息
struct SWeatherInfo 
{
	enWeatherType	nWeatherType;	// 天气类型
	BOOL			bRandom;		// 是否随机，如果为TRUE的话下面的控制不起作用
	int				nParticleNum;	// 粒子数量		50～500 (玫瑰花瓣可以配到1000)
	int				nWindPower;		// 风力			-100～100
	int				nSpeed;			// 下落速度		1～10
};
//------------------------------------------------------------------------
// 供地图编辑器使用的地图显示接口
struct IMapView
{
	// 增加一个对象
	virtual BOOL AddItem(const POINT& ptTile, CMapItemView* pItem, DWORD dwParam=0) = NULL;
	//将对象移到一个新块
	virtual BOOL MoveItem(const POINT& ptFrom, const POINT& ptTo, CMapItemView* pItem, DWORD dwParam=0) = NULL;
	// 删除对象
	virtual BOOL RemoveItem(const POINT& ptTile, CMapItemView* pItem, DWORD dwParam=0) = NULL;
	// 加入对象占位信息
	virtual BOOL AddItemOccupant(const POINT& ptTile, CMapItemView* pItem) = NULL;
	// 删除对象占位信息
	virtual BOOL RemoveItemOccupant(const POINT& ptTile, CMapItemView* pItem) = NULL;
	/* 
		测试光鼠标下所指的对象
		ptScreen ：屏幕坐标，相对于地图左上角的偏移
		pItemFoundAnchorTile : 如果找到对象，返回此对象落脚点所在的tile的坐标和对象指针
		dwFlag : 
			指定满足此标志的对象才能被选中。
			当dwFlag == 0时，表示只要点对对象的可见部位即为选中，适合于地图编辑器
			当dwFlag == htCanSel时，表示检查是否曾通过CMapItemView::AddFlag()
			设置对象的flagCanbeSelected标志，适合于游戏主程序
	 */
	enum {htCanSel = 2/*只选择指定可选中的对象，即虽然点中对象可见区域，但没设置可选中标志也选不中*/};
	virtual CMapItemView* HitTest(IN const RECT& rcScreen, OUT POINT* pItemFoundAnchorTile, IN DWORD dwHitFlag = 0) = NULL;
	/* 
		读写地图存档
		bWrite为TRUE时，表示保存，pItemCF可以为NULL
		bWrite为FALSE时，表示读取地图，并把恢复地图上的所有对象现场，pItemCF为对象创建器
		//	暂不定此参数
		//	bServerMap:
	 	//	用bServerMap标志区分是保存服务器专用地图还是客户端地图，
		//	两者的区别是：服务器端地图不需要地上物，只需要CTile数据即可
		//	只对地图存档有效
	*/
	virtual BOOL OnSerialize(BOOL bWrite, IArchive* pArchive, IItemClassFactory* pItemCF, LPRECT lprcViewport, ITrace* pTrace) = NULL;
	// 地图创建管理
	// 保存地图图片(原理：按照 1024*768 的大小一屏一屏来保存)
	// nStartRow,nStartCol,nRows,nCols决定保存的区域
	// fRate:保存后的图片与保存前图片大小的比值
	// bDrawGround:是否绘制地表
	// bCombine:是否将生成的每屏图片合并到一起
	virtual BOOL SaveMapBitmapToFile(LPCSTR szFileName, int nStartRow, int nStartCol, int nRows, int nCols, 
		float fRate = 1.0f, BOOL bDrawGround = TRUE, BOOL bCombine = TRUE) = NULL;	
	// 屏幕相关
	virtual int GetMapWidth() const = NULL;
	virtual int GetMapHeight() const = NULL;
	/* 功能：获取地图tile数组的宽度。地图tile数组长宽不是相等的:width=height+1
	   说明：
			搜跑模块需要建立与地图相同的大小的tile数组，所以提供此函数
	*/
	virtual int GetMapTileWidth() = NULL;
	virtual int GetMapTileHeight() = NULL;
	// 获取当前可见视图左上角world坐标
	virtual int GetViewTopLeftX() const  = NULL;
	virtual int GetViewTopLeftY() const = NULL;
	// 可视窗口矩阵，返回绝对象素坐标
	virtual const RECT& GetViewportRect() const = NULL;
	// 视口大小改变，刚创建地图时默认大小为1024 x 768
	virtual void OnSizeViewport(int nViewWidth, int nViewHeight) = NULL;
	// 滚动屏幕
	virtual BOOL ScrollViewport(int dx, int dy) = NULL;
	// 滚动屏幕，尽可能把ptScreenCenter定位到屏幕中心。以主角居中模式需要这个函数
	virtual BOOL ScrollToCenter(POINT ptScreenCenter) = NULL;
	// 绘制地图,rc为地图屏幕区域，暂不用
	virtual void OnDraw(ICanvas* pCanvas, const void* pParam) = NULL;
	virtual void DrawRectangleGrid(ICanvas* pCanvas, int nGridWidth, int nGridHeight, WPixel clrLine, WPixel clrText) = NULL;
	// 返回块的中心点的屏幕坐标
	virtual void ScreenToTile(IN const POINT& ptScreen, OUT POINT& ptTile) const = NULL;
	// 返回屏幕点对应的tile坐标
	virtual void TileToScreen(IN const POINT& ptTile, OUT POINT& ptTileCenter) const = NULL;
	// 返回世界坐标对应的块坐标
	virtual void WorldToTile(IN const POINT& ptWorld, OUT POINT& ptTile) const = NULL;
	// 返回块的世界坐标
	virtual void TileToWorld(IN const POINT& ptTile, OUT POINT& ptTileCenter) const = NULL;
	// 返回块对象
	// 总是返回CTile，可通过CTile::IsValid()检查此tile是否有效
	virtual CTile* GetTile(const POINT& ptTile) const = NULL;
	// 根据屏幕象素坐标取得块指针
	// 总是返回有效地址的CTile，根据CTile.IsValid()判断此tile是否有效
	virtual CTile* GetTileFromScreen(POINT ptScreen) const = NULL;
	// 检查tile坐标是否有效、tile是否有效
	virtual BOOL IsValidTile(const POINT& ptTile) = NULL;
	// 设置是否在地表上画网格、占位块
	enum{maskDrawGrid = 1,maskDrawOccupant = 2,
		maskClearDrawGrid = 0xFFFFFFFE,maskClearDrawOccupant = 0xFFFFFFFD};
	// 设置是否显示地表标志图，pParam一般为相应标记图图像
	//	注：同时设置多个标记时，只能有一个设置pParam
	// 当dwFlag == maskDrawOccupant时，dwParam为占位块图片pBitmap，图片大小为64 x 32
	virtual void SetDrawFlag(DWORD dwFlag, DWORD dwClearFlag, void* pParam = 0) = NULL;
	virtual DWORD GetDrawFlag(DWORD dwFlag) = NULL;
	virtual void Release() = NULL;
	virtual BOOL SetPlayerItem(CMapItemView* pItem) = NULL;
	virtual CMapItemView* GetPlayerItem() = NULL;
	/*
	  功能：向逻辑矩形内广播，按从左上角到右下角的顺序。广泛用于范围广播，如攻击、聊天广播等
	  参数：
		输入：
			rcWorld		: 游戏世界象素坐标
			nListCount	: pListBuf的最大维数
		输出：
			pListBuf	: 返回所有对象
			nListCount	: 返回 pListBuf 中有效的对象数量
	 */
	// 功能同EnumTileByWorldRect()
	virtual BOOL SnapshotTileByWorldRect(IN const RECT& rcWorld, IN OUT int& nListCount, OUT SNAP_TILE_INFO* pListBuf) = NULL;
	virtual BOOL SnapshotItemByWorldRect(IN const RECT& rcWorld, IN OUT int& nListCount, OUT SNAP_ITEM_INFO* pListBuf) = NULL;
	// 设置小地图是否可见
	virtual void SetMiniMapVisible(BOOL bVisible) = NULL;
	// 设置小地图的视图矩形
	virtual void SetMiniMapViewport(LPRECT lprcViewport) = NULL;
	// 设置小地图的半透明值
	virtual void SetMiniMapTransparent(int nAlpha) = NULL;
	// 得到示意图的显示状态
	virtual BOOL GetTinyMapVisible() = NULL;
	// 设置示意图的显示状态
	virtual void SetTinyMapVisible(BOOL bVisible) = NULL;
	// 寻路相关
	// 寻路 , 如果出错程序抛出错误 类型LPCTSTR
	// ptFrom	:   路径的起始点
	// ptTo		:   路径的目标点
	// ...		
	virtual BOOL FindPath(POINTS ptFrom, POINTS ptTo, POINTS** ppBuffer, int& nPathLen) = NULL;
	// 寻找一个简单路径
	// 有时候搜路不需要很精确只需要返回一个模糊路径就可
	virtual BOOL FindSimplePath(POINTS ptFrom, POINTS ptTo, POINTS** ppBuffer, int& nPathLen) = NULL;
	// 快速填充地表的占位块(仅仅用于地图编辑器)
	// 原理：在地图编辑器上定义一张图元的占位块，然后将其信息复制到地图中使用了同样图元的地方
	// ptTileInRect:模板区域内的某个Tile坐标(系统会自动校正到左上点)
	// nGridWidth,nGridHeight:图元网格的宽高，即图元的宽高
	// nTileRows,nTileCols:要填充的区域的行数、列数(从左上点开始算)
	virtual BOOL QuickFillOccupant(POINT& ptTileInRect, int nGridWidth, int nGridHeight, int nTileRows, int nTileCols) = NULL;
	// 缓冲移屏的支持
	virtual void SetOptimizedScrollScreen(BOOL bOptimized) = NULL;
	virtual BOOL GetOptimizedScrollScreen() = NULL;
	// 当有空余时间时，让地图预先处理一些东西
	virtual void OnHandleIdle(DWORD dwParam) = NULL;
	// 为了放止不停的更新接口，这里加入一个抽象的方法，当需要加入功能而不希望修改接口时，可以暂时用这个方法来实现
	virtual int OnSomeHandle(DWORD dwParam1, DWORD dwParam2) = NULL;
	// 设置小地图大小
	virtual void SetMiniMapBigOrSmall(BOOL bBig) = NULL;
	// 改变地图天气
	virtual void ChangeWeather(SWeatherInfo info) = NULL;
};
//------------------------------------------------------------------------
extern IMapView* CreateNewMapView(DWORD dwVer, int nMapWidth, int nMapHeight,
							  LPRECT lprcViewport, LPRECT lprcMiniMapViewport,
							  LPCSTR szGroundJpgFileDir,
							  IFileObject* pGridIdxArrayFile, ITrace* pTrace);
// 注：szGroundJpgFileDir不再需要，可以设置为NULL，为兼容性考虑，应用层也可指定szGroundJpgFileDir
extern IMapView* LoadMapView(DWORD dwVer, IFileObject* pMapFile, IFileObject* pGridIdxArrayFile, IFileObject* pTinyMapFile,
						 LPCSTR szGroundJpgFileDir, LPCSTR szMapName, LPRECT lprcViewport, 
						 LPRECT lprcMiniMapViewport, LPCSTR szMiniMapJpgFileDir, BOOL bUseDynamicMap, int nMiniMapRate, 
						 IItemClassFactory *pItemCF, ITrace *pTrace);
//------------------------------------------------------------------------
// __LIB_USEMAPVIEW			// 定义这个宏指定通过lib方式调用地图视图接口
//------------------------------------------------------------------------
class CMapViewHelper
{
public:
	typedef IMapView* (*ProcCreateNewMapView)(DWORD dwVer, int nMapWidth, int nMapHeight,
		LPRECT lprcViewport, LPRECT lprcMiniMapViewport,
		LPCSTR szGroundJpgFileDir, IFileObject* pGridIdxArrayFile, ITrace* pTrace);

	typedef IMapView* (*ProcLoadMapView)(DWORD dwVer,
		IFileObject* pMapFile, IFileObject* pGridIdxArrayFile, IFileObject* pTinyMapFile,
		const char* szGroundJpgFileDir, LPCSTR szMapName, LPRECT lprcViewport, 
		LPRECT lprcMiniMapViewport, LPCSTR szMiniMapJpgFileDir, BOOL bUseDynamicMap, int nMiniMapRate, 
		IItemClassFactory *pItemCF,ITrace *pTrace);
	
	CMapViewHelper()
	{
		m_pMapView = NULL;
		m_hdll = NULL;
	}
	~CMapViewHelper()
	{
		Close();
	}

	BOOL CreateNewMap(int nMapWidth, int nMapHeight,
		LPRECT lprcViewport, LPRECT lprcMiniMapViewport,
		LPCSTR szGroundJpgFileDir, IFileObject* pGridIdxArrayFile, ITrace* pTrace)
	{
		Close();
		if(!LoadDll())
			return NULL;
#if !defined(__LIB_USEMAPVIEW)
		ProcCreateNewMapView proc = (ProcCreateNewMapView)GetProcAddress(m_hdll, "dllCreateNewMapView");
		if(proc == NULL)
		{
			OutputDebugString("Can not GetProcAddress() : dllCreateNewMapView\r\n");
			return FALSE;
		}
			
		m_pMapView = proc(VERSION_MAPVIEW, nMapWidth, nMapHeight,
			lprcViewport, lprcMiniMapViewport,
			szGroundJpgFileDir, pGridIdxArrayFile, pTrace);
#else
		m_pMapView = CreateNewMapView(VERSION_MAPVIEW,nMapWidth,nMapHeight,
			lprcViewport, lprcMiniMapViewport,
			szGroundJpgFileDir,szGridIndexArray,pTrace);
#endif
		return m_pMapView != NULL;
	}

	/*
	 *	pMapFile: 地图文件
	 *	pGridIdxArrayFile: 地表索引数组文件
	 *	pTinyMapFile: 地图框架图文件
	 *	szgroundJpgFiledir: 地表jpg文件目录
	 *	szMapName: 地图名称
	 *	lprcViewport: 地图当前的视口(宽=1024,高=768)
	 *	lprcMiniMapViewport: 小地图在大地图中的视口
	 *	szMiniMapJpgFileDir: 小地图jpg文件目录
	 *	bUseDynamicMap: 是否使用动态地图管理(当地图较小时，请不要使用动态地图管理)
	 *	nMiniMapRate: 小地图图片的缩放比例(默认为20)
	 *	pItemCF: item工厂
	 *	pTrace: Trace接口
	 */
	BOOL LoadMap(IFileObject* pMapFile, IFileObject* pGridIdxArrayFile, IFileObject* pTinyMapFile,
		LPCSTR szGroundJpgFileDir, LPCSTR szMapName, LPRECT lprcViewport, 
		LPRECT lprcMiniMapViewport, LPCSTR szMiniMapJpgFileDir, BOOL bUseDynamicMap, int nMiniMapRate, 
		IItemClassFactory* pItemCF, ITrace* pTrace)
	{
		Close();
		if(!LoadDll())
			return FALSE;
#if !defined(__LIB_USEMAPVIEW)
		ProcLoadMapView proc= (ProcLoadMapView)GetProcAddress(m_hdll, "dllLoadMapView");
		if(proc == NULL)
		{
			OutputDebugString("Can not GetProcAddress() : dllLoadMapView\r\n");
			return FALSE;
		}
		m_pMapView = proc(VERSION_MAPVIEW, pMapFile, pGridIdxArrayFile, pTinyMapFile,
			szGroundJpgFileDir, szMapName, lprcViewport, 
			lprcMiniMapViewport, szMiniMapJpgFileDir, bUseDynamicMap, nMiniMapRate, 
			pItemCF, pTrace);
#else
		m_pMapView = LoadMapView(VERSION_MAPVIEW, pMapFile, pGridIdxArrayFile, pTinyMapFile,
			szGroundJpgFileDir, szMapName, lprcViewport, 
			lprcMiniMapViewport, szMiniMapJpgFileDir, bUseDynamicMap, nMiniMapRate, 
			pItemCF, pTrace);
#endif
		return m_pMapView != NULL;
	}

	void Close()
	{
		if(m_pMapView)
		{
			m_pMapView->Release();
			m_pMapView = NULL;
		}	
		if(m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}
	BOOL LoadDll()
	{
		try
		{
//#ifdef _USE // 供地图编辑器用
//			if(m_hdll == NULL)
//				m_hdll = ::LoadLibrary("FKMapView_U.dll");
//			if(m_hdll == NULL)
//				throw "无法读取FKMapView_U.dll";
//#elif defined(_DEBUG)
#ifdef _DEBUG
			if(m_hdll == NULL)
				m_hdll = ::LoadLibrary("FKMapView_D.dll");
			if(m_hdll == NULL)
				throw "无法读取FKMapViewD.dll";
#else
			if(m_hdll == NULL)
				m_hdll = ::LoadLibrary("FKMapView.dll");
			if(m_hdll == NULL)
				throw "无法读取FKMapView.dll";
#endif
			return TRUE;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return FALSE;
		}
	}
	IMapView* operator->()
	{
		return m_pMapView;
	}
public:
	IMapView*	m_pMapView;
private:
	HINSTANCE		m_hdll;
};
//------------------------------------------------------------------------