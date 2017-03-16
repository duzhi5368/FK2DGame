/**
*	created:		2012-7-1   5:16
*	filename: 		ObjectList
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		对象列表管理
*/
//------------------------------------------------------------------------
/*
 *	客户端的视区和预读区域可以设置得很小，但是list网格区域必须比较大，保证
 服务器进行对象add，remove和move时不会出问题。我们初步定list网格大小为16*16，
 每个list网格上的对象操作都记录下来，当地图滚动而需要更新逻辑对象时，可以从
 这些列表里面读取并创建出来。

  由于网格处理比较麻烦，而且牵涉到服务器的调用顺序，这里暂时将多列表弄成一个
  单列表，对整个地图起作用。至于能加进来的对象，服务器会进行9宫格的控制。
 */
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/MapInfo/MapItem.h"
#include <list>
using std::list;
//------------------------------------------------------------------------
enum ErrType{ERR_INVALID=-1,ERR_FAILED=0,ERR_SUCCESS=1};
//------------------------------------------------------------------------
struct stMapItemInfo
{
	CMapItem*	pItem;
	POINT		ptTile;
	POINT		ptGrid;
	stMapItemInfo(CMapItem* p, const POINT& tile, const POINT& grid)
		: pItem(p), ptTile(tile), ptGrid(grid) {}
	BOOL operator == (const stMapItemInfo& info)
	{
		return (info.pItem == pItem && 
			info.ptTile.x == ptTile.x && info.ptTile.y == ptTile.y);
	}
};
//------------------------------------------------------------------------
typedef list<stMapItemInfo>				MapItemInfoList;
typedef list<stMapItemInfo>::iterator	MapItemInfoListPtr;
//------------------------------------------------------------------------
class CObjectList  
{
protected:
	MapItemInfoList		m_ObjList;
	MapItemInfoListPtr	m_pCurObj;

public:
	CObjectList()
	{
		Clear();
	}
	virtual ~CObjectList()
	{
		Clear();
	}
	bool IsEmpty()
	{
		return m_ObjList.empty();
	}
	void Clear()
	{
		m_ObjList.clear();
	}
	int GetCount()
	{
		return m_ObjList.size();
	}
	MapItemInfoList* GetListPtr()
	{
		return &m_ObjList;
	}
	void Add(CMapItem* pItem, const POINT& ptTile, const POINT& ptGrid)
	{
		if (pItem == NULL) return;
		m_ObjList.push_back(stMapItemInfo(pItem, ptTile, ptGrid));
	}
	int Remove(CMapItem* pItem, const POINT& ptTile)
	{
		if (pItem == NULL) return ERR_INVALID;
		MapItemInfoListPtr first = m_ObjList.begin();
		MapItemInfoListPtr last = m_ObjList.end();
		while (first != last)
		{
			MapItemInfoListPtr next = first;
			++next;
			if ((*first).pItem == pItem)
			{
				m_ObjList.erase(first);
				return ERR_SUCCESS;
			}
			first = next;
		}
		return ERR_FAILED;
	}
	int Move(CMapItem* pItem, const POINT& ptTileFrom, const POINT& ptTileTo, const POINT& ptGridTo)
	{
		if (pItem == NULL) return ERR_INVALID;
		MapItemInfoListPtr first = m_ObjList.begin();
		MapItemInfoListPtr last = m_ObjList.end();
		while (first != last)
		{
			MapItemInfoListPtr next = first;
			++next;
			if ((*first).pItem == pItem)
			{
				(*first).ptTile = ptTileTo;
				(*first).ptGrid = ptGridTo;
				return ERR_SUCCESS;
			}
			first = next;
		}
		Add(pItem, ptTileTo, ptGridTo);
		return ERR_FAILED;
	}

	stMapItemInfo* FindFirstItem(int nGridX, int nGridY)
	{
		m_pCurObj = m_ObjList.begin();
		MapItemInfoListPtr last = m_ObjList.end();
		while (m_pCurObj != last)
		{
			MapItemInfoListPtr next = m_pCurObj;
			++next;
			stMapItemInfo& info = (*m_pCurObj);
			if (info.ptGrid.x == nGridX && info.ptGrid.y == nGridY)
				return &(*m_pCurObj);
			m_pCurObj = next;
		}
		return NULL;
	}
	stMapItemInfo* FindNextItem(int nGridX, int nGridY)
	{
		++m_pCurObj;
		MapItemInfoListPtr last = m_ObjList.end();
		while (m_pCurObj != last)
		{
			MapItemInfoListPtr next = m_pCurObj;
			++next;
			stMapItemInfo& info = (*m_pCurObj);
			if (info.ptGrid.x == nGridX && info.ptGrid.y == nGridY)
				return &(*m_pCurObj);
			m_pCurObj = next;
		}
		return NULL;
	}
};
//------------------------------------------------------------------------