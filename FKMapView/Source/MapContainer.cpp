/**
*	created:		2012-7-1   2:45
*	filename: 		MapContainer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
/*
 * 功能：
		地图对象容器类。
		server和client通用，所有对象支持CMapItem接口
 		负责地表阻挡信息、地上物、NPC、人物等对象的增删查；路径有效性和路径查找功能
   实现：
		 游戏对象创建器创建所有对象，将对象纳入CMapContainer管理
   算法说明：
		一、地图上的一块，此块之上可以包含了其它对象，可以称之为柱状容器
			对象层管理示意图如下：
			  |	  <- CMapItem	   \
			  |	  <- CMapItem		\  管理类为
			  |	  <- CMapItem       /  CLayerList
			  |   <- CMapItem	   /
			 ---  CTile
		二、上面的层次图在具体实现时作了优化：
			只有一个结点时,CTile::m_pLayerList作为CMapItem结点指针，通过maskIsMapItem标志位来区分
			二个以上的结点才需要分配CLayerList进行多层管理
	    三、  范围查找时，为了提高速度，提供一种类似于接口的回调机制，如绘图循环、服务器计算魔法攻击范围等
			  执行过程如下：
				  1、外部程序传一个区域给CMapContainer，包括矩形、圆、线段(未实现)等
				  2、CMapContainer依次取得这个区域的每个有效点，
				   回调外部程序IMapItemEnumSink::OnEnumMapItem()
			  优点是：减少了大量检查坐标有效性的代码；避免了矩阵坐标的乘法运算
			  注意：当查找范围较小时，可以使用GetTile()逐步查找
		四、提供命令码广播方式替代范围查找
			同(三)的方法类似，直接将命令发给地图中的对象
		增加Snapshot系统广播函数，为了解决数组访问困境问题
 */
//------------------------------------------------------------------------
#include "../Include/MapContainer.h"
#include "../../FKGame/SceneObj/IItemClassFactory.h"
#include "../Include/Snapshot.h"
#include "../Include/MapViewCommon.h"
//------------------------------------------------------------------------
CTile CMapContainer::m_EmptyTile;
//------------------------------------------------------------------------
CMapContainer::ProcScrollTilesInfo CMapContainer::m_fnScrollTileInfo[mdMaxUpdateType] =
{
	&CMapContainer::_MovePreReadRect_Top,
	&CMapContainer::_MovePreReadRect_RightTop,
	&CMapContainer::_MovePreReadRect_Right,
	&CMapContainer::_MovePreReadRect_RightDown,
	&CMapContainer::_MovePreReadRect_Down,
	&CMapContainer::_MovePreReadRect_LeftDown,
	&CMapContainer::_MovePreReadRect_Left,
	&CMapContainer::_MovePreReadRect_LeftTop,
	&CMapContainer::_MovePreReadRect_All,
};
//------------------------------------------------------------------------
CMapContainer::CMapContainer()
{
	m_ptTileOrigin.x = m_ptTileOrigin.y = 0;
	m_nMatrixWidth = m_nMatrixHeight = 0;
	memset(&m_rcMapTileRect,0,sizeof(m_rcMapTileRect));
	m_pMultiValueTable = NULL;
	m_szGround[0] = 0;

	m_pTiles = NULL;
	m_pMapTable = NULL;
	m_pArchive = NULL;
	m_pItemCF = NULL;

	m_bUseDynamicMap = FALSE;
	
	m_nMapWidth = 0;
	m_nMapHeight = 0;
	memset(&m_rcMap,0,sizeof(m_rcMap));
}
//------------------------------------------------------------------------
CMapContainer::~CMapContainer()
{
	Close();
}
//------------------------------------------------------------------------
// nMapWidth和nMapHeight是地图的象素大小，经过了64(32)对齐
// rcPreRead是经过地图大小的限制和进行64(32)对齐之后的预读区域矩形
BOOL CMapContainer::Create(int nMapWidth, int nMapHeight, RECT& rcPreRead, ITrace* pTrace)
{
	Close();
	g_DefaultTrace.Create(pTrace);

	m_nMapWidth = nMapWidth;
	m_nMapHeight = nMapHeight;
	SetRect(&m_rcMap, 0, 0, nMapWidth, nMapHeight);

	m_MapCo.Create(nMapWidth, nMapHeight);

	m_LayerListCache.AddCacheList(64);

	// 更新原点
	if (IsUseDynamicMap())
	{
		POINT ptWorld, ptTileOrigin;
		ptWorld.x = rcPreRead.left;
		ptWorld.y = rcPreRead.top;
		m_MapCo.PixelToTile(ptWorld, ptTileOrigin);
		ptTileOrigin.y -= (rcPreRead.bottom - rcPreRead.top)/32 - 1;
		SetOriginTile(ptTileOrigin);
	}

	if (!CreateMapArray(rcPreRead))
	{
		return FALSE;
	}

	// 创建寻路模块
	CPathFinder_R<CMapContainer>::Create(this, m_MapCo.GetMatrixWidth(), m_MapCo.GetMatrixHeight());
	return TRUE;
}
//------------------------------------------------------------------------
// 创建地图菱形矩阵
// nWidth和nHeight是地图的象素大小
BOOL CMapContainer::CreateMapArray(RECT& rcPreRead)
{
	// 计算行列TILE数以及菱形格边长TILE数
	int nCols = PixelCood2GridCood(rcPreRead.right - rcPreRead.left, 64);
	int nRows = PixelCood2GridCood(rcPreRead.bottom - rcPreRead.top, 32);
	m_nMatrixWidth = nCols + nRows;
	m_nMatrixHeight = m_nMatrixWidth - 1;

	// 最大TILE矩形
	m_rcMapTileRect.left = m_rcMapTileRect.top = 0;
	m_rcMapTileRect.right = m_nMatrixWidth;
	m_rcMapTileRect.bottom = m_nMatrixHeight;

	// TILE数组
	m_pTiles = new CTile[m_nMatrixWidth * m_nMatrixHeight]();
	TraceLn("本地图Tile数量为 = %d",m_nMatrixWidth * m_nMatrixHeight);
	if(m_pTiles == NULL)
		return FALSE;

	// 构造乘法值
	m_pMultiValueTable = new DWORD[m_nMatrixHeight];
	if (m_pMultiValueTable == NULL)
		return FALSE;
	for (int k=0,nVal=0; k<m_nMatrixHeight; k++,nVal+=m_nMatrixWidth)
		m_pMultiValueTable[k] = nVal;

	// 初始化有效的TILE
	InitBlockInfo((POINT&)rcPreRead, nRows, nCols*2);

	return TRUE;
}
//------------------------------------------------------------------------
void CMapContainer::SetOriginTile(POINT& ptTileOrigin)
{
	m_ptTileOrigin = ptTileOrigin;
	TraceLn("本地图起点位置为 = [ %d,%d ]", m_ptTileOrigin.x, m_ptTileOrigin.y);
}
//------------------------------------------------------------------------
void CMapContainer::ScrollTilesInfo(BLOCK_INFO* pBlockInfo, int nRows, int nCols, 
		int nTileRow, int nTileCol, int nDirection, POINT& ptNewTileOrigin, int dx, int dy)

{
	(this->*m_fnScrollTileInfo[nDirection])(pBlockInfo, nRows, nCols, nTileRow, nTileCol, ptNewTileOrigin, dx, dy);
}
//------------------------------------------------------------------------
void CMapContainer::InitBlockInfo(POINT& ptLeftTop, int nTileRow, int nTileCol)
{
	POINT ptTileLeftTop;
	m_MapCo.PixelToTile(ptLeftTop, ptTileLeftTop);
	POINT ptCurTile;
	for (int row=0; row<nTileRow; row++) // 一个“之”字行
	{
		ptCurTile.x = ptTileLeftTop.x + row;
		ptCurTile.y = ptTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++) // 列数
		{
			CTile& tile = _GetTile(ptCurTile);
			tile.Create();
			
			ptCurTile.x += !(col&1); // odd
			ptCurTile.y += col&1;
		}
	}
}
//------------------------------------------------------------------------
void CMapContainer::LoadBlockInfo(POINT& ptLeftTop, int nIndex, int nTileRow, int nTileCol)
{
	if (ptLeftTop.x + GRID_WIDTH > m_nMapWidth)
		nTileCol = PixelCood2GridCood(m_nMapWidth - ptLeftTop.x, 32);
	if (ptLeftTop.y + GRID_HEIGHT > m_nMapHeight)
		nTileRow = PixelCood2GridCood(m_nMapHeight - ptLeftTop.y, 32);
	InitBlockInfo(ptLeftTop, nTileRow, nTileCol);
	m_pArchive->SeekTo(m_pMapTable[nIndex]);
	POINT ptTileLeftTop;
	m_MapCo.PixelToTile(ptLeftTop, ptTileLeftTop);
	_LoadBlock(m_pArchive, ptTileLeftTop, nTileRow, nTileCol, m_pItemCF);
}
//------------------------------------------------------------------------
void CMapContainer::CopyBlockInfo(POINT& ptSrcLeftTop, POINT& ptDstLeftTop, int nTileRow, int nTileCol)
{
	POINT ptSrcTileLeftTop, ptDstTileLeftTop;
	m_MapCo.PixelToTile(ptSrcLeftTop, ptSrcTileLeftTop);
	m_MapCo.PixelToTile(ptDstLeftTop, ptDstTileLeftTop);
	int dx = ptDstTileLeftTop.x - ptSrcTileLeftTop.x;
	int dy = ptDstTileLeftTop.y - ptSrcTileLeftTop.y;
	for (int row=0; row<nTileRow; row++) // 一个“之”字行
	{
		POINT ptCurTile;
		ptCurTile.x = ptSrcTileLeftTop.x + row;
		ptCurTile.y = ptSrcTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++) // 列数
		{
			GetTile(ptCurTile.x + dx, ptCurTile.y + dy) = GetTile(ptCurTile);
			
			ptCurTile.x += !(col&1); // odd
			ptCurTile.y += col&1;
		}
	}
}
//------------------------------------------------------------------------
void CMapContainer::CopyBlockInfoForSurface(POINT& ptSrcLeftTop, POINT& ptDstLeftTop, int nTileRow, int nTileCol)
{
	POINT ptSrcTileLeftTop, ptDstTileLeftTop;
	m_MapCo.PixelToTile(ptSrcLeftTop, ptSrcTileLeftTop);
	m_MapCo.PixelToTile(ptDstLeftTop, ptDstTileLeftTop);
	int dx = ptDstTileLeftTop.x - ptSrcTileLeftTop.x;
	int dy = ptDstTileLeftTop.y - ptSrcTileLeftTop.y;
	if (dx == 0 && dy == 0)
		return;
	for (int row=0; row<nTileRow; row++) // 一个“之”字行
	{
		POINT ptCurTile;
		ptCurTile.x = ptSrcTileLeftTop.x + row;
		ptCurTile.y = ptSrcTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++) // 列数
		{
			// 只将源TILE的地表信息添加到目的TILE
			CTile& srcTile = GetTile(ptCurTile);
			DWORD dwFlag = srcTile.GetFlag();
			dwFlag = srcTile.IsOccupant_Surface();

			CTile& dstTile = GetTile(ptCurTile.x + dx, ptCurTile.y + dy);
			DWORD dwFlagDstItem = dstTile.GetFlag();
			dwFlagDstItem &= ~ CTile::maskOccupant_Surface;
			dstTile.GetLayerCount();
			dwFlagDstItem |= dwFlag;
			dstTile.SetFlag(dwFlagDstItem);
			
			ptCurTile.x += !(col&1); // odd
			ptCurTile.y += col&1;
		}
	}
}
//------------------------------------------------------------------------
void CMapContainer::Close()
{
	if (m_pMultiValueTable)
	{
		delete[] m_pMultiValueTable;
		m_pMultiValueTable = NULL;
	}
	if(m_pTiles)
	{
		delete[] m_pTiles;
		m_pTiles = NULL;
		m_nMatrixWidth = 0;
	}

	//销毁地图资源
	MAP_ITEM_LIST::iterator it   =  m_MapItemList.begin();
	MAP_ITEM_LIST::iterator end  =  m_MapItemList.end();
	for(;it != end; ++it)
	{
		SAFE_RELEASE((*it));
	}
	m_MapItemList.clear();
	m_LayerListCache.Close();
}
//------------------------------------------------------------------------
BOOL CMapContainer::MoveItem(const POINT& ptFrom, const POINT& ptTo, CMapItem* pItem, DWORD dwParam)
{
	if (!RemoveItem(ptFrom, pItem))
		return FALSE;
	return AddItem(ptTo, pItem);
}
//------------------------------------------------------------------------
// 暂不实现排序
BOOL CMapContainer::AddLayerItem(const POINT &pt, CMapItem *pItem)
{
	return AddItem(pt,pItem);
}
//------------------------------------------------------------------------
BOOL CMapContainer::AddItem(const POINT& ptTile, CMapItem* pItem, DWORD dwParam)
{
	if (pItem == NULL)
		return NULL;
	CTile& tile = GetTile(ptTile);
	if (!tile.IsValid()) // tile无效
	{
		POINT pt;
		m_MapCo.TileToPixel(ptTile, pt);
		int nGridX = pt.x >> SHIFT_WIDTH;
		int nGridY = pt.y >> SHIFT_HEIGHT;
		return FALSE;	// 由于某些对象可以从某个动态地图边缘走到无效区域，所以这里忽略无效，仍然返回TRUE，以保证对象的逻辑坐标正确
	}

	// 测试占位块是否重叠
#ifdef _USE
	if (dwParam == 1)
	{
		if (pItem->GetOccupantTileList() == NULL)
		{
			CTile& tile = GetTile(ptTile);
			if ( tile.IsValid() &&tile.IsOccupant_Item())
				return FALSE;
		}
		else
		{
			if (!TestIntersect(ptTile, pItem))
				return FALSE;
		}
	}
#endif
	try
	{
		
	// 添加对象
	if (tile.GetMapItem() == NULL) // tile上原来没有对象
		tile.SetOneLayer(pItem);
	else // tile上原来有对象
	{ 
		// 当前只有一个结点时再增加一个结点，此时将第一个结点迁到m_pLayerList中
		if (tile.IsMapItemValid())
		{
			CMapItem *pFirst = tile.GetMapItem();
			tile.SetNoLayer();	// 清除当前m_pMapItem
			//....... 根据pItem占位情况改变地表flag
			if (!m_LayerListCache.AddData(tile.GetLayerListPointer(), pFirst))
				return FALSE;
		}
		// 添加对象到列表中
		if (!m_LayerListCache.AddData(tile.GetLayerListPointer(), pItem))
		{
			return FALSE;
		}
	}

	// 更新占位块
	if (pItem->GetOccupantTileList() == NULL)
		tile.Set_Occupant_Item(pItem->GetOccupantValue());
	else
		AddMultiOccupantValue(ptTile, pItem);
	}
	catch (...)
	{
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 如果一个CTile上有很多层相同的对象，应该先删除顶上的？
BOOL CMapContainer::RemoveItem(const POINT& ptTile, CMapItem* pItem, DWORD dwParam)
{
	CTile& tile = GetTile(ptTile);
	if (!tile.IsValid()) // tile无效
	{
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
		{
			POINT pt, ptLeftTop, ptTileLeftTop = m_ptTileOrigin;
			m_MapCo.TileToPixel(ptTile, pt);
			ptTileLeftTop.y -= TILES_PREREAD_HEIGHT / 32 - 1;
			m_MapCo.TileToPixel(ptTileLeftTop, ptLeftTop);
			RECT rcPreRead;
			rcPreRead.left = ptLeftTop.x;
			rcPreRead.top = ptLeftTop.y;
			rcPreRead.right = rcPreRead.left + TILES_PREREAD_WIDTH;
			rcPreRead.bottom = rcPreRead.top + TILES_PREREAD_HEIGHT;
			if (PtInRect(&rcPreRead, pt)) // 在预读区域内
			{
				int nGridX = pt.x >> SHIFT_WIDTH;
				int nGridY = pt.y >> SHIFT_HEIGHT;
				return FALSE;
			}
			return TRUE; // 在预读菱形格的无效区域!
		}
		return TRUE; // 如果不在预读区域，我们略过!
	}

	try{

	// 移除对象
	if (tile.GetMapItem() == NULL) // tile上没有对象
	{
		POINT pt;
		m_MapCo.TileToPixel(ptTile, pt);
		int nGridX = pt.x >> SHIFT_WIDTH;
		int nGridY = pt.y >> SHIFT_HEIGHT;
		return FALSE;
	}
	else if (tile.IsMapItemValid()) // tile上有一个对象
	{
		if (tile.GetMapItem() != pItem) // 但不是指定的对象
		{
			return FALSE;
		}
		tile.SetNoLayer();
	}
	else if (!m_LayerListCache.RemoveData(tile.GetLayerListPointer(), pItem)) // tile上有多个对象
	{
		return FALSE;
	}
	
	// 更新占位块
	if (pItem->GetOccupantTileList() == NULL)
		tile.RemoveItemOccupant(pItem->GetOccupantValue());
	else
		RemoveMultiOccupantValue(ptTile, pItem);
	}
	catch (...)
	{
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::AddItemOccupant(const POINT& ptTile, CMapItem* pItem)
{
	if(pItem == NULL)
		return FALSE;

	CTile& tile = GetTile(ptTile);
	if (!tile.IsValid()) // tile无效
		return FALSE;

	// 更新占位块
	if (pItem->GetOccupantTileList() == NULL)
		tile.Set_Occupant_Item(pItem->GetOccupantValue());
	else
		AddMultiOccupantValue(ptTile, pItem);

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::RemoveItemOccupant(const POINT& ptTile, CMapItem* pItem)
{
	if(pItem == NULL)
		return FALSE;

	CTile& tile = GetTile(ptTile);
	if (!tile.IsValid()) // tile无效
		return FALSE;

	// 更新占位块
	if (pItem->GetOccupantTileList() == NULL)
		tile.RemoveItemOccupant(pItem->GetOccupantValue());
	else
		RemoveMultiOccupantValue(ptTile, pItem);
	
	return TRUE;
}
//------------------------------------------------------------------------
void CMapContainer::AddMultiOccupantValue(POINT pt, CMapItem *pItem)
{
	COccupantTileList* pList = pItem->GetOccupantTileList();
	pt.x += pList->GetPreData().nOffAnchorX; // pt由放置点(锚点)偏移到占位块矩形的基点(矩形左下角)
	pt.y += pList->GetPreData().nOffAnchorY;
	int sw = pList->GetPreData().wWidth;
	int sh = pList->GetPreData().wHeight;
	if(pList->GetCount() == 0)
	{// 整个矩阵都是占位块
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			for(int x = sw; x > 0 ;x--,pt.x++)
			{
				CTile& tile = GetTile(pt);
				if(tile.IsValid()) // 此块是有可能出界的
					tile.Set_Occupant_Item();
			}
			pt.x = xFrom;
		}
	}
	else
	{ // 按标志字节来标识占位情况，即有一些不是占位块
		char* pL = pList->GetDataBuffer();;
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			char*p = pL;
			for(int x = sw; x > 0 ;x--,p++,pt.x++)
				if(*p)
				{
					CTile& tile = GetTile(pt);
					if(tile.IsValid()) // 此块是有可能出界的
						tile.Set_Occupant_Item_Byte(*p);
				}
			pt.x = xFrom;
			pL += sw;
		}
	}
}
//------------------------------------------------------------------------
void CMapContainer::RemoveMultiOccupantValue(POINT pt, CMapItem *pItem)
{
	COccupantTileList* pList = pItem->GetOccupantTileList();
	pt.x += pList->GetPreData().nOffAnchorX; // pt由放置点(锚点)偏移到占位块矩形的基点(矩形左下角)
	pt.y += pList->GetPreData().nOffAnchorY;
	int sw = pList->GetPreData().wWidth;
	int sh = pList->GetPreData().wHeight;
	if(pList->GetCount() == 0)
	{// 整个矩阵都是占位块
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			for(int x = sw; x > 0 ;x--,pt.x++)
			{
				CTile& tile = GetTile(pt);
				if(tile.IsValid()) // 此块是有可能出界的
				{
					tile.Clear_Occupant_Item_Byte();
					tile.OnUpdateItems();
				}
			}
			pt.x = xFrom;
		}
	}
	else
	{ // 按标志字节来标识占位情况，即有一些不是占位块
		char* pL = pList->GetDataBuffer();;
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			char*p = pL;
			for(int x = sw; x > 0 ;x--,p++,pt.x++)
				if(*p)
				{
					CTile& tile = GetTile(pt);
					if(tile.IsValid()) // 此块是有可能出界的
					{
						tile.Clear_Occupant_Item_Byte(*p);
						tile.OnUpdateItems();
					}
				}
			pt.x = xFrom;
			pL += sw;
		}
	}
}
//------------------------------------------------------------------------
// 测试将要添加的对象的占位块是否与地上已有的占位块有重复
BOOL CMapContainer::TestIntersect(POINT pt, CMapItem* pItem)
{
	COccupantTileList* pList = pItem->GetOccupantTileList();
	pt.x += pList->GetPreData().nOffAnchorX; 
	pt.y += pList->GetPreData().nOffAnchorY;
	int sw = pList->GetPreData().wWidth;
	int sh = pList->GetPreData().wHeight;
	if(pList->GetCount() == 0)
	{// 整个矩阵都是占位块
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			for(int x = sw; x > 0 ;x--,pt.x++)
			{
				CTile& tile = GetTile(pt);
				if(tile.IsValid() &&tile.IsBlock())
					return FALSE;
			}
			pt.x = xFrom;
		}
	}
	else
	{ // 按标志字节来标识占位情况，即有一些不是占位块
		char* pL = pList->GetDataBuffer();;
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			char*p = pL;
			for(int x = sw; x > 0 ;x--,p++,pt.x++)
				if(*p)
				{
					CTile& tile = GetTile(pt);
					if(tile.IsValid() &&tile.IsBlock())
						return FALSE;
				}
				pt.x = xFrom;
				pL += sw;
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 测试多占位块物体的移动性
BOOL CMapContainer::IsMovableForMultiOcc(CMapItem* pItem, POINT ptFrom, POINT ptTo)
{
	COccupantTileList* pList = pItem->GetOccupantTileList();
	POINT pt = ptFrom;
	pt.x += pList->GetPreData().nOffAnchorX; 
	pt.y += pList->GetPreData().nOffAnchorY;
	int sw = pList->GetPreData().wWidth;
	int sh = pList->GetPreData().wHeight;
	int dx = ptTo.x - ptFrom.x;
	int dy = ptTo.y - ptFrom.y;

	char* pL = pList->GetDataBuffer();
	int nOffset = sw*dy + dx;
	for (; pt.y<ptTo.y; pt.y++)
	{
		char* p = pL;
		for (; pt.x<ptTo.x; pt.x++,p++)
		{
			if (*p)
			{
				CTile& tile = GetTile(pt.x + dx, pt.y + dy);
				if (tile.IsValid() && tile.IsBlock() && *(pL+nOffset) == 0)
					return FALSE;
			}
		}
		pL += sw;
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 调整块坐标矩阵，使之每块都在有效坐标系m_rcMapTileRect之内
// 这里的rc定义成{tx,ty,col,row},(tx,ty)表示左上角tile坐标，(col,row)分别表示屏幕横向和纵向的格子数
// 返回
//	FALSE : 如果rc跟m_rcMapTileRect无交集或全部在无效区域
BOOL CMapContainer::AdjustTileRect(RECT &rc)
{
	if(rc.right < 0 || rc.left >= m_nMatrixWidth || rc.bottom < 0 || rc.top < 0 || rc.top >= m_nMatrixWidth)
	{
		TraceLn("Error : CMapContainer::AdjustTileRect() : invalid TileRect(%d,%d),(%d,%d)",rc.left,rc.top,rc.top,rc.bottom);
		return FALSE;
	}
	if(rc.left < 0)
	{
		rc.right +=  rc.left / 2;	// 扣除
		rc.bottom +=  rc.left / 2;	// 扣除 
		rc.left = 0;
	}
	if(rc.left + rc.right > m_nMatrixWidth)
		rc.right = 	m_nMatrixWidth - rc.left;

	if(rc.top + rc.right > m_nMatrixWidth)
		rc.right = m_nMatrixWidth - rc.top;
	if(rc.top - rc.bottom + 1 < 0)
		rc.bottom = rc.top + 1;

	// 右下角
	if(rc.left + rc.right + rc.bottom - 1 > m_nMatrixWidth)
		rc.bottom = m_nMatrixWidth - rc.left - rc.right + 1;
	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::SnapshotRectItem(const RECT& rc,/*in out*/int& nListCount,/*out*/SNAP_ITEM* pListBuf)
{
	if(pListBuf == NULL || nListCount < 1)
	{
		TraceLn("Error : CMapContainer::SnapshotRectItem() : invalid param!");
		return FALSE;
	}
	RECT rcArea;
	if(!IntersectRect(&rcArea,&rc,&m_rcMapTileRect))
		return FALSE;
	int nFoundCount = 0;

	int txEnd = rcArea.right;
	int tyEnd = rcArea.bottom;
	CTile* pTileLeft = &m_pTiles[rcArea.top * m_nMatrixWidth + rcArea.left];
	CTile* pTile = pTileLeft;
	for(int ty = rcArea.top; ty < tyEnd; ty++)
	{
		for(int tx = rcArea.left; tx < txEnd; tx++,pTile++)
		{
			int nCount = pTile->GetLayerCount();
			if(nCount)
			{
				for(int i = 0; i < nCount; i++)
				{
					if(nFoundCount >= nListCount)
					{
						TraceLn("Error : CMapContainer::SnapshotRectItem() : to %d items found,but %d max",nFoundCount,nListCount);
						goto END_SnapshotRectItem;
					}
					pListBuf[nFoundCount].ptTile.x = tx;
					pListBuf[nFoundCount].ptTile.y = ty;
					pListBuf[nFoundCount].pItem = pTile->GetLayer(i);
					nFoundCount++;
				}
			}
		}
		pTileLeft += m_nMatrixWidth;
		pTile = pTileLeft;
	}

END_SnapshotRectItem:
	nListCount = nFoundCount;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::SnapshotItemByWorldRect(const RECT &rcWorld, IN OUT int& nListCount, OUT SNAP_ITEM* pListBuf)
{
	if (pListBuf == NULL || nListCount < 1)
	{
		TraceLn("Error : CMapContainer::SnapshotItemByWorldRect() : invalid param!");
		return FALSE;
	}
	CSnapshotItemByWorldRect snap(this, rcWorld, nListCount, pListBuf);
	nListCount = snap.m_nFoundCount;
	return nListCount > 0;
}
//------------------------------------------------------------------------
BOOL CMapContainer::SnapshotTileByWorldRect(const RECT &rcWorld, IN OUT int& nListCount, OUT SNAP_TILE* pListBuf)
{
	if (pListBuf == NULL || nListCount < 1)
	{
		TraceLn("Error : CMapContainer::SnapshotTileByWorldRect() : invalid param!");
		return FALSE;
	}
	CSnapshotTileByWorldRect snap(this, rcWorld, nListCount, pListBuf);
	nListCount = snap.m_nFoundCount;
	return nListCount > 0;
}
//------------------------------------------------------------------------