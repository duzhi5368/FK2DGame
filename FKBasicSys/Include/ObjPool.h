/**
*	created:		2012-7-1   19:48
*	filename: 		ObjPool
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		对象缓冲池
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifdef _DEBUG
#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
#include <hash_map>
#include <list>
#include <map>
using std::map;
using std::list;
using stdext::hash_map;
//------------------------------------------------------------------------
// 所有需要缓冲的对象必须实现的接口
struct IPoolNodeObject
{
	virtual DWORD GetSize() = NULL;
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
// 池节点
struct CPoolNode
{
	IPoolNodeObject* m_pObj;
	DWORD			 m_dwID;
	int				 m_nRefCount;

	bool operator==( const CPoolNode& p_Other )const
	{
		if( p_Other.m_dwID == this->m_dwID )
		{
			return true;
		}
		return false;
	}

	CPoolNode(): m_pObj(0), m_dwID(0), m_nRefCount(0){}
	virtual ~CPoolNode(){}
	CPoolNode(const CPoolNode& apn): m_pObj(apn.m_pObj), m_dwID(apn.m_dwID), m_nRefCount(apn.m_nRefCount) {}
	CPoolNode(IPoolNodeObject* pObj, DWORD dwID): m_pObj(pObj), m_dwID(dwID){m_nRefCount=0;}
	CPoolNode& operator = (const CPoolNode& pn){m_pObj=pn.m_pObj;m_dwID=pn.m_dwID;m_nRefCount=pn.m_nRefCount;return *this;}
	// 得到对象
	IPoolNodeObject* GetPoolNodeObject(){return m_pObj;}
	// 获取对象的大小
	virtual DWORD GetSize(){if (m_pObj) return m_pObj->GetSize();return 0;}
	// 每个对象都有一个键值与之对应
	virtual DWORD GetKey(){return m_dwID;}
	// 得到节点的引用次数
	virtual int GetRefCount() {return m_nRefCount;}
	// 添加一次引用
	virtual int AddRef() {return ++m_nRefCount;}
	// 减去一次引用(考虑到缓冲池的可回收性，这里允许引用计数为0时不进行Release操作)
	virtual int SubRef() {if(--m_nRefCount<=0) m_nRefCount=0;return m_nRefCount;}
	// 释放对象(不考虑引用计数)
	virtual void ReleasePoolNodeObject(){if(m_pObj){m_pObj->Release();m_pObj=0;}}
};
//------------------------------------------------------------------------
/*
 * 输入：
 * 　	PoolNodeType		：必须继承自CPoolNode或实现CPoolNode的所有功能
 *		key_type			：键类型，如DWORD,__int64等

 * 思路：
 * 采用引用计数, 缓冲池的容量可由外部动态设置(默认128M)
 * 两个 list : 一个标准列表，记录正在使用的节点；一个可回收列表，记录被扔掉的节点, 利用了list的快速增删操作
 * 节点的查询通过hash_map快速查询，hash_map记录了各节点在两个list中的位置，利用了hash_map的快速搜索操作
 * 当计数为0时，不要删除，放到可回收list中
*/
//------------------------------------------------------------------------
#define KBYTE     1024
#define MBYTE     KBYTE * KBYTE
#define GBYTE     MBYTE * MBYTE
//------------------------------------------------------------------------
template <class PoolNodeType = CPoolNode, class key_type = DWORD>
class CObjectPool
{
private:
	typedef list<PoolNodeType>							NodeList;
	//typedef list<PoolNodeType>::iterator				NodeListPtr;
	typedef hash_map<key_type, PoolNodeType>			HashMap;
	//typedef hash_map<key_type, PoolNodeType>::iterator	HashMapPtr;

	DWORD		m_dwMemSize;		// 当前所有对象所占的内存数
	NodeList	m_NodeList;			// 标准节点列表
	NodeList	m_RecycledNodeList;	// 可回收节点列表
	HashMap		m_HashMap;			// 哈希表
	DWORD		m_dwMaxPoolSize;	// 缓冲池能够容纳的最大内存数

public:
	CObjectPool()
	{
		Close();
		m_dwMaxPoolSize = 0 * MBYTE;
	}

	virtual ~CObjectPool(){Close();}
	void Close()
	{
		// 调用当前列表中的每个对象的Release
		for ( list<PoolNodeType>::iterator it=m_NodeList.begin(); it!=m_NodeList.end(); it++)
			(*it).ReleasePoolNodeObject();
		for ( list<PoolNodeType>::iterator it=m_RecycledNodeList.begin(); it!=m_RecycledNodeList.end(); it++)
			(*it).ReleasePoolNodeObject();

		m_NodeList.clear();
		m_RecycledNodeList.clear();
		m_HashMap.clear();
		m_dwMemSize = 0;
	}
	
	void Recycle()
	{
		list<PoolNodeType>::iterator rlist_it=m_RecycledNodeList.begin(); 
		while (rlist_it!=m_RecycledNodeList.end())
		{
			m_dwMemSize -= (*rlist_it).GetSize();
			(*rlist_it).ReleasePoolNodeObject();
			m_HashMap.erase((*rlist_it).GetKey());
			m_RecycledNodeList.erase(rlist_it);
			rlist_it=m_RecycledNodeList.begin();
		}
	}
	
	void AddSize(DWORD dwSize)
	{
		m_dwMemSize += dwSize;
	}

	// 判断缓冲池是否已经溢出
	BOOL IsOverflow()
	{
		MEMORYSTATUS ms;
		GlobalMemoryStatus(&ms);
		if(ms.dwAvailPhys < 100 * MBYTE)
		{
			if(m_dwMaxPoolSize >= 10 * MBYTE)
				m_dwMaxPoolSize -= 10 * MBYTE;
		}
		else
		{
			if(m_dwMaxPoolSize <= 90 * MBYTE)
				m_dwMaxPoolSize += 10 * MBYTE;
		}

		return (m_dwMemSize > m_dwMaxPoolSize);
	}

	BOOL AddObjectRef(PoolNodeType& node)
	{
		if (IsOverflow())
		{
			if(m_RecycledNodeList.size() > 1)
			{
				list<PoolNodeType>::iterator rlist_it=m_RecycledNodeList.begin(); 
				while (rlist_it!=m_RecycledNodeList.end())
				{
					m_dwMemSize -= (*rlist_it).GetSize();
					(*rlist_it).ReleasePoolNodeObject();
					m_HashMap.erase((*rlist_it).GetKey());
					m_RecycledNodeList.erase(rlist_it);
					rlist_it=m_RecycledNodeList.begin();
					if (!IsOverflow())
						break;
				}
			}
		}
		// 添加一个对象,并记录到哈希表
		node.AddRef();
		m_NodeList.push_back(node);
		list<PoolNodeType>::iterator itTmp = m_NodeList.end();
		--itTmp;
		m_HashMap[node.GetKey()] = (*itTmp); //--m_NodeList.end();

		return TRUE;
	}
	
	// 移除节点
	// 如果引用计数为0，但缓冲池还有空间则将对象扔到垃圾桶，不进行真正的删除
	BOOL RemoveObjectRef(key_type idKey)
	{
		hash_map<key_type, PoolNodeType>::iterator it = m_HashMap.find(idKey);
		if (it != m_HashMap.end()) // exist!
		{
			PoolNodeType list_it = (*it).second;

			if (list_it.GetRefCount() >0 && list_it.SubRef() == 0) // 扔到可回收列表的尾部
			{
				m_RecycledNodeList.push_back(list_it);
				list<PoolNodeType>::iterator itTmp = m_RecycledNodeList.end();
				--itTmp;
				m_HashMap[idKey] = (*itTmp);
				//m_NodeList.erase(list_it);
				m_NodeList.remove( list_it );

				// 如果溢出，则释放可回收列表头部的节点，直到缓冲池不溢出
				if (IsOverflow() && m_RecycledNodeList.size() > 1)
				{
					list<PoolNodeType>::iterator rlist_it=m_RecycledNodeList.begin(); 
					while (rlist_it!=m_RecycledNodeList.end())
					{
						m_dwMemSize -= (*rlist_it).GetSize();
						(*rlist_it).ReleasePoolNodeObject();
						m_HashMap.erase((*rlist_it).GetKey());
						m_RecycledNodeList.erase(rlist_it);
						rlist_it=m_RecycledNodeList.begin();
						if (!IsOverflow())
							break;
					}					
				}
			}
			return TRUE;
		}
		return FALSE;
	}

	// 查找对应键值的节点
	// 找到则添加引用计数，如果在回收列表中，则移入标准列表中
	BOOL Find(key_type idKey, PoolNodeType** ppNode)
	{
		hash_map<key_type, PoolNodeType>::iterator it = m_HashMap.find(idKey);
		if (it != m_HashMap.end()) // exist!
		{
			PoolNodeType list_it = (*it).second;
			
			// 如果在可回收列表中，则放到标准列表中
			if (list_it.GetRefCount() == 0)
			{
				m_NodeList.push_back(list_it);			// push to back

				// m_HashMap[idKey] = --m_NodeList.end();	// reset pointer
				list<PoolNodeType>::iterator itTmp = m_NodeList.end();
				--itTmp;
				m_HashMap[idKey] = (*itTmp);

				// m_RecycledNodeList.erase(list_it);		// delete node
				m_RecycledNodeList.remove( list_it );

				list_it = (*itTmp);
				// list_it = --m_NodeList.end();			// update pointer
			}

			list_it.AddRef();

			*ppNode = &(list_it);
			return (ppNode != NULL);
		}

		return FALSE;
	}
};
//------------------------------------------------------------------------