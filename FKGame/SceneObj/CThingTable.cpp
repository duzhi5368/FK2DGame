/**
*	created:		2012-6-26   1:07
*	filename: 		CThingTable
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "CThingTable.h"
//------------------------------------------------------------------------
CThingTable::CThingTable(void)
{
	m_TableEnd = m_table.end();
}
//------------------------------------------------------------------------
CThingTable::~CThingTable(void)
{
	Close();
}
//------------------------------------------------------------------------
// 说明：CThingTable的特点决定很难回收残留的对象
//	   因而依赖于地图容器和对象本身结束自身生命期
//		   以后改进CMap实现，性能也同时得到提升
void CThingTable::Close(void)
{	
	THASH_THINGTABLE_ITERATOR it;
	int nSize = m_table.size();
	while(m_table.size() > 0)
	{
		it = m_table.begin();
		IThing * pThing = (*it).second;//(*it)->second;	
		pThing->Release(); //只能在it++后面释放，因为it++会访问pThing
	}
}
//------------------------------------------------------------------------
IThing * CThingTable::Find(UID uid)
{
	THASH_THINGTABLE_ITERATOR it = m_table.find(uid);
	if(it == m_TableEnd)
	{
		return NULL;
	}
	return (*it).second;
}
//------------------------------------------------------------------------
// 返回：
//	  如果成功：如果hash表中已经有相同对象，返回表中的对象
//	  否则返回pThing
//      添加失败：返回NULL	
IThing * CThingTable::Add(IThing * pThing)
{
	THASH_THINGTABLE_ITERATOR it = m_table.find( pThing->GetUID() );
	if( it != m_table.end() )
	{
		return (*it).second;
	}
	m_table.insert( make_pair( pThing->GetUID(), pThing ) );
	return pThing;
}
//------------------------------------------------------------------------
// 移除
BOOL CThingTable::Remove(UID uid)
{
	int nTestDebug = m_table.size();
	BOOL bResult = m_table.erase(uid);
	nTestDebug = m_table.size();

	return bResult;
}
//------------------------------------------------------------------------
// 移除
BOOL CThingTable::Remove(IThing * pThing)
{
	return Remove(pThing->GetUID());
}
//------------------------------------------------------------------------
// 描  述：取得thing列表
TVECTOR_THING * CThingTable::GetThingList(void)
{
	// 清空
	m_vectorThing.clear();

	// 填充
	THASH_THINGTABLE_ITERATOR it = m_table.begin();
	for( ; it != m_table.end(); ++it)
	{
		m_vectorThing.push_back((*it).second);
	}

	return (&m_vectorThing);
}
//------------------------------------------------------------------------