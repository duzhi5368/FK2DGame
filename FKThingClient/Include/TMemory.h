/**
*	created:		2012-7-2   21:48
*	filename: 		TMemory
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		对频繁new , delete的内存应用stl的内存优化方案
					说明: 实际上小对象也是可以用 __NEW_POOL_ALLOC 的
						  它和__NEW的唯一差别是 它是为每个类维护一个
						  缓冲池的，假设Class A 和 Class B都是 5 bytes
						  那么使用__NEW的STL机制的话 A和B是通用一个缓冲
						  池的，B释放的内存可以继续给A用,而__NEW_POOL_
						  ALLOC不能通用

						  __NEW_POOL_ALLOC 不是线程安全的
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <list>
using std::list;
//------------------------------------------------------------------------
// 大内存管理类，其实只是负责大内存的释放
class CChunkManager
{
public:
	void AddChunk( void * pChunk )
	{
		m_ChunkList.push_back( pChunk );
	}

	~CChunkManager()
	{
		list< void * >::iterator it = m_ChunkList.begin();
		for( ; it != m_ChunkList.end() ; it ++ )
		{
			delete ( * it );
		}

		m_ChunkList.clear();
	}
private:
	list< void * >	m_ChunkList;
};
//------------------------------------------------------------------------
extern CChunkManager g_ChunkManager;
//------------------------------------------------------------------------
// 这个是大于128子节对象的缓冲版本
template< class __T , int PoolSize = 32 >
class CMemPoolObject
{
public:	
	void * operator new(size_t size)
	{
		__poolNode * & pPoolList =  GetPoolFirst();
		if( pPoolList ) 
		{
			__poolNode * pTemp = pPoolList;
			pPoolList = pPoolList->pNext;
			return pTemp;
		}else
		{
			union __poolNodeAux
			{
				char data[ sizeof( __T ) ];
				__poolNodeAux * pNext;
			};

			__poolNodeAux * pChunk =  ( __poolNodeAux * )
				malloc( sizeof( __poolNodeAux ) * PoolSize );
			g_ChunkManager.AddChunk( pChunk );

			if( pChunk == NULL ) return NULL;

			for( int i = 1 ; i < PoolSize - 1 ; i ++ )
			{
				pChunk[ i ].pNext = &pChunk[ i + 1 ];
			}

			pChunk[ PoolSize - 1 ].pNext = NULL;
			pPoolList = ( __poolNode *)(&pChunk[ 1 ]);
			return &pChunk[0];
		}		
	}

	void operator delete(void *p)
	{
		__poolNode * & pPoolList =  GetPoolFirst();
		__poolNode * pTemp = pPoolList;
		pPoolList = (__poolNode *)p;
		pPoolList->pNext = pTemp;
	}

private:
	union __poolNode{
		__poolNode * pNext;
	};

	static __poolNode * & GetPoolFirst()
	{
		static __poolNode * s_PoolList = NULL;
		return s_PoolList;
	}
};
//------------------------------------------------------------------------