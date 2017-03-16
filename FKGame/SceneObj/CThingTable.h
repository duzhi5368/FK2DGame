/**
*	created:		2012-6-26   0:49
*	filename: 		CThingTable
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		客户端逻辑对像管理列表
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IThing.h"
#include "IThingTable.h"
#include <hash_map>
//------------------------------------------------------------------------
#define THING_TABLE_SIZE	1024
typedef IThing *			THINGPTR;
//------------------------------------------------------------------------
typedef stdext::hash_map< UID, THINGPTR >	THASH_THINGTABLE;
typedef THASH_THINGTABLE::iterator	THASH_THINGTABLE_ITERATOR;
//------------------------------------------------------------------------
// 定义
class CThingTable : public IThingTable
{
public:
	// 关闭
	virtual void				Close(void);

	// 搜索
	virtual IThing *			Find(UID uid);

	// 添加
	virtual IThing *			Add(IThing * pThing);

	// 移除
	virtual BOOL				Remove(UID uid);

	// 移除
	virtual BOOL				Remove(IThing* pThing);

	// 取得thing列表
	virtual TVECTOR_THING *		GetThingList(void);
public:
	CThingTable(void);
	virtual ~CThingTable(void);
private:
	THASH_THINGTABLE			m_table;
	THASH_THINGTABLE_ITERATOR	m_TableEnd;
	TVECTOR_THING				m_vectorThing;
};
//------------------------------------------------------------------------