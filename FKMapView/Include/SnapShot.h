/**
*	created:		2012-7-1   2:49
*	filename: 		SnapShot
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Geometry.h"
#include "MapContainer.h"
//------------------------------------------------------------------------
/*
  功能：取得一个线段(不包括线段的第一点)上各点的快照
  参数：
	输入：
		(x1,y1),(x2,y2)	: 起始点和终点，游戏世界象素坐标系
		nListCount	: pListBuf的最大维数
	输出：
		m_nListCount	: 返回 m_pListBuf 中有效的对象数量
		m_pListBuf		: 返回所有对象，不包括(x1,y1)中可能存在的对象
  使用范例：
		CSnapshotLine<4096> line(&m_MapContainer,x1,y1,x2,y2);
		// line.m_nFoundCount和line.m_ListBuf中包含了返回结果
*/
//------------------------------------------------------------------------
template<int nMaxItemCount>
class CSnapshotLine : public CLineDiffuse
{
public:
	CSnapshotLine(CMapContainer* pMap,int x1, int y1, int x2, int y2)
	{
		Create(pMap->GetMapArrayWidth(),pMap->GetMapArrayWidth());

		m_pMap = pMap;
		m_nFoundCount = 0;
		Line(x1,y1,x2,y2);
	}
	virtual int OnFoundTile(int x,int y)
	{
		if(m_nFoundCount >= nMaxItemCount)
		{
			TraceLn("Error : CSnapshotLine::OnFoundTile() : to %d items found,but %d max",m_nFoundCount,nMaxItemCount);
			return retStop;
		}
		CTile& tile = m_pMap->GetTile(x,y);
		if(!tile.IsValid())	// 此块为用于填充数组用的无效块，不理
			return retTrue;
		m_ListBuf[m_nFoundCount].x = x;
		m_ListBuf[m_nFoundCount].y = y;
		m_nFoundCount++;
		return retTrue;
	}
public:
	int		m_nFoundCount;					// 返回找到的item数量
	POINT	m_ListBuf[nMaxItemCount];		// 返回item数组
private:
	CMapContainer*	m_pMap;
};
//------------------------------------------------------------------------
/*
	功能： 返回沿着此线段(不包括线段的第一点)上行走或飞行时遇到第一个符合要求的tile和item
	说明：
		dwStopFlag为tile和item需要满足的标志位，参见CTile标志位定义，一般可取：maskBlock、maskFly、maskOccupant
		如果发现块阻挡，但没发现item，*ppItem 值会被设置为 NULL，此时一般是因为地表阻挡，但地表上没有对象
	输入：
		dwStopFlag : 遇到符合此标志的块或对象即止搜索
	输出：
		m_bFoundTile : 如果找到阻挡块，返回TRUE，否则返回FALSE
		m_ptStopTile : 返回结束块
		m_pStopItem	 : 返回找到的item，如要没找到item，此m_pStopItem被设置为NULL
	使用范例：
		CSnapshotShoot line(&m_MapContainer,CTile::maskFly,x1,y1,x2,y2)
		line.Line(x1,y1,x2,y2);
		if(line.m_bFoundTile)
		{
			//line.m_ptStopTile和line.m_pStopItem包含了返回结果
		}
*/
class CSnapshotShoot : public CLineDiffuse
{
public:
	CSnapshotShoot(CMapContainer* pMap,DWORD dwStopFlag,int x1, int y1, int x2, int y2)
	{
		Create(pMap->GetMapArrayWidth(),pMap->GetMapArrayWidth());

		m_pMap = pMap;
		m_dwStopFlag = dwStopFlag;

		m_bFoundTile = FALSE;
		m_pStopItem = NULL;

		Line(x1,y1,x2,y2);
	}
	virtual int OnFoundTile(int x,int y)
	{
		CTile& tile = m_pMap->GetTile(x,y);
		if(!tile.IsValid())	// 此块为用于填充数组用的无效块，不理
			return retTrue;
		if(tile.GetFlag() & m_dwStopFlag)
		{
			m_bFoundTile = TRUE;
			m_ptStopTile.x = x; m_ptStopTile.y = y;
			int nCount = tile.GetLayerCount();				
			for(int i = 0; i < nCount; i++)					
			{												
				CMapItem* pItem = tile.GetLayer(i);			
				if(pItem->GetOccupantValue() & m_dwStopFlag)	
					m_pStopItem = pItem;					
			}												
			return retStop;										
		}
		return retTrue;
	}

public:
	BOOL		m_bFoundTile;	// 返回是否找到tile
	POINT		m_ptStopTile;	// 返回找到的tile
	CMapItem*	m_pStopItem;	// 返回找到的item

private:
	CMapContainer*	m_pMap;
	DWORD			m_dwStopFlag;
};
//------------------------------------------------------------------------
/* 功能：返回类似于火焰攻击时，火焰在遇到障碍前所覆盖的所有对象(不包括线段的第一点)
   说明：
		dwTileFlag为tile和item需要满足的标志位，参见CTile标志位定义，一般可取：maskBlock、maskFly、maskOccupant
		如果发现块阻挡，但没发现item，*ppItem 值会被设置为 NULL，此时一般是因为地表阻挡，但地表上没有对象
	输入：
		dwStopFlag : 遇到符合此标志的块或对象即止搜索
		nListCount : pListBuf的最大维数
	输出：
		如果找到对象，返回TRUE，否则返回FALSE
		如果成功，nListCount和pListBuf返回对象列表
	使用范例：
		CSnapshotLine<4096> line(&m_MapContainer,x1,y1,x2,y2);
		// line.m_nFoundCount和line.m_ListBuf中包含了返回结果
			
*/
template<int nMaxItemCount>
class CSnapshotShootItemCovered : public CLineDiffuse
{
public:
	CSnapshotShootItemCovered(CMapContainer* pMap,int x1, int y1, int x2, int y2)
	{
		Create(pMap->GetMapArrayWidth(),pMap->GetMapArrayWidth());
		m_pMap = pMap;
		m_nFoundCount = 0;

		Line(x1,y1,x2,y2);
	}
	virtual int OnFoundTile(int x,int y)
	{
		if(m_nFoundCount >= nMaxItemCount)
		{
			TraceLn("Error : CSnapshotShootItemCovered::OnFoundTile() : to %d items found,but %d max",m_nFoundCount,nMaxItemCount);
			return retStop;
		}
		CTile& tile = m_pMap->GetTile(x,y);
		if(!tile.IsValid())	// 此块为用于填充数组用的无效块，不理
			return retTrue;
		m_ListBuf[m_nFoundCount].x = x;
		m_ListBuf[m_nFoundCount].y = y;
		m_nFoundCount++;
		return retTrue;
	}
public:
	int		m_nFoundCount;					// 返回找到的item数量
	POINT	m_ListBuf[nMaxItemCount];		// 返回item数组
private:
	CMapContainer*	m_pMap;
};
//------------------------------------------------------------------------
/*
  功能：取得一个线段(不包括线段的第一点)上所有对象的快照
  参数：
	输入：
		(x1,y1),(x2,y2)	: 起始点和终点，游戏世界象素坐标系
		nListCount	: pListBuf的最大维数
	输出：
		m_nListCount	: 返回 m_pListBuf 中有效的对象数量
		m_pListBuf		: 返回所有对象，不包括(x1,y1)中可能存在的对象
  使用范例：
		CSnapshotLine<4096> line(&m_MapContainer,x1,y1,x2,y2);
		// line.m_nFoundCount和line.m_ListBuf中包含了返回结果
*/
template<int nMaxItemCount>
class CSnapshotItemOnLine : public CLineDiffuse
{
public:
	CSnapshotItemOnLine(CMapContainer* pMap,int x1, int y1, int x2, int y2)
	{
		Create(pMap->GetMapArrayWidth(),pMap->GetMapArrayWidth());
		m_pMap = pMap;
		m_nFoundCount = 0;
		Line(x1,y1,x2,y2);
	}
	virtual int OnFoundTile(int x,int y)
	{
		CTile& tile = m_pMap->GetTile(x,y);
		if(!tile.IsValid())	// 此块为用于填充数组用的无效块，不理
			return retTrue;
		int nItemCount = tile.GetLayerCount();
		for(int tt = 0; tt < nItemCount; tt++)
		{
			if(m_nFoundCount >= nMaxItemCount)
			{
				TraceLn("Error : CSnapshotItemOnLine::OnFoundTile() : to %d items found,but %d max",m_nFoundCount,nMaxItemCount);
				return retStop;
			}
			m_ListBuf[m_nFoundCount].ptTile.x = x;
			m_ListBuf[m_nFoundCount].ptTile.y = y;
			m_ListBuf[m_nFoundCount].pItem = tile.GetLayer(tt);
			m_nFoundCount++;
		}
		return retTrue;
	}
public:
	int			m_nFoundCount;					// 返回找到的item数量
	SNAP_ITEM	m_ListBuf[nMaxItemCount];		// 返回item数组
private:
	CMapContainer*	m_pMap;
};
//------------------------------------------------------------------------
class CSnapshotItemByWorldRect : public CEnumTileArea
{
public:
	CSnapshotItemByWorldRect(CMapContainer* pMap, const RECT &rcWorld, 
		int nMaxItemCount, SNAP_ITEM* pListBuf)
	{
		Create(pMap->GetMapArrayWidth());
		m_pMap = pMap;
		m_nMaxItemCount = nMaxItemCount;
		m_pListBuf = pListBuf;
		m_nFoundCount = 0;
		EnumTileArea(pMap->GetMapCoordinary().PixelRectToAreaTileRect(rcWorld));
	}
	virtual int OnFoundTile(int x, int y)
	{
		try{
		CTile& tile = m_pMap->_GetTile(x,y);
		int nItemCount = tile.GetLayerCount();
		for (int tt=0; tt<nItemCount; tt++)
		{
			if (m_nFoundCount >= m_nMaxItemCount)
			{
				return retStop;
			}
			m_pListBuf[m_nFoundCount].ptTile.x = x;
			m_pListBuf[m_nFoundCount].ptTile.y = y;
			m_pListBuf[m_nFoundCount].pItem = tile.GetLayer(tt);
			m_nFoundCount++;
		}
		}catch (...) {
		}
		return retTrue;
	}
public:
	int			m_nMaxItemCount;
	int			m_nFoundCount;	// 返回找到的item数量
	SNAP_ITEM*	m_pListBuf;		// 返回item数组
private:
	CMapContainer*	m_pMap;
};
//------------------------------------------------------------------------
class CSnapshotTileByWorldRect : public CEnumTileArea
{
public:
	CSnapshotTileByWorldRect(CMapContainer* pMap, const RECT &rcWorld,
		int nMaxTileCount, SNAP_TILE* pListBuf)
	{
		Create(pMap->GetMapArrayWidth());
		m_pMap = pMap;
		m_nMaxTileCount = nMaxTileCount;
		m_pListBuf = pListBuf;
		m_nFoundCount = 0;
		EnumTileArea(pMap->GetMapCoordinary().PixelRectToAreaTileRect(rcWorld));
	}
	virtual int OnFoundTile(int x, int y)
	{
		CTile& tile = m_pMap->_GetTile(x, y);
		ASSERT(tile.IsValid());
		if (m_nFoundCount >= m_nMaxTileCount)
		{
			return retStop;
		}
		m_pListBuf[m_nFoundCount].ptTile.x = x;
		m_pListBuf[m_nFoundCount].ptTile.y = y;
		m_pListBuf[m_nFoundCount].pTile = &tile;
		m_nFoundCount++;
		
		return retTrue;
	}
public:
	int			m_nMaxTileCount;
	int			m_nFoundCount;	// 返回找到的TILE数量
	SNAP_TILE*	m_pListBuf;		// 返回TILE数组
private:
	CMapContainer*	m_pMap;
};
//------------------------------------------------------------------------