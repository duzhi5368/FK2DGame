/**
*	created:		2012-6-16   0:44
*	filename: 		FKTempl
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
/*
 说明：1、此模板库来自于vc6.0的afxtempl.h。
       2、afxtempl.h库的性能比stl优秀；其简单的垃圾回收机制也很实用
	   3、改造过的库的接口跟afxtempl.h相同，可以方便地在MFC和非MFC中切换
	   4、改造的原则是不改变源代码，源代码所需要的宏已经作了重新定义
	      改造后有不同的地方用 //> 注释
	   5、新加了CCriticalSectionLock类
	   6、增加了RemoveHeadOnly()，与RemoveHead()不同之处在于不返回node
	   7、2008.5.27,增加CFIFOCache，实现先进先出的缓冲策略，用于较大的相同大小的对象的缓冲，如IBitmap
	   8、2008.7.6，增加两个新类，CMultLenArray管理可变长数组，CMultLenArrayCache缓冲大量使用的可变长数组对象
	   9、2008.8.7，CStaticStack：静态栈管理，用一个FILO数组实现，当队列已满时，新加入的对象被抛弃
 */
//------------------------------------------------------------------------
// 如果已经使用了MFC模板库，则禁用以下改造的CList、CArray等模板，扩充床继续有效
#if !defined(__AFXTEMPL_H__)
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
#if !defined(ASSERT)
	#if defined(_DEBUG)
		#define ASSERT(f) {if (!(f))__asm int 3}
	#else
		#define ASSERT(f)	 ((void*)0)
	#endif
#endif
//------------------------------------------------------------------------
#define ASSERT_VALID(x)		((void)0)		//>> 使此宏不起作用
//------------------------------------------------------------------------
#ifndef __PLACEMENT_NEW_INLINE
	#define __PLACEMENT_NEW_INLINE
	inline void *__cdecl operator new(size_t, void *_P)
			{return (_P); }
	#if _MSC_VER >= 1200
		inline void __cdecl operator delete(void *, void *)
			{return; }
	#endif
#endif
//------------------------------------------------------------------------
template<class TYPE>
inline void __stdcall ConstructElements(TYPE* pElements, int nCount)
{
	ASSERT(nCount == 0 || IsValidAddress(pElements, nCount * sizeof(TYPE)));
	memset((void*)pElements, 0, nCount * sizeof(TYPE));
	// 调用构造
	for (; nCount--; pElements++)
		::new((void*)pElements) TYPE;
}
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
BOOL __stdcall CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	return *pElement1 == *pElement2;
}
//------------------------------------------------------------------------
class CDumpContext
{
public:
};
//------------------------------------------------------------------------
class CObject
{
public:
	virtual ~CObject() = 0{};
protected:
	CObject(){}
private:
	CObject(const CObject& objectSrc){}
public:
#if defined(_DEBUG) 
	// Diagnostic Support
	virtual void AssertValid() const{}
	virtual void Dump(CDumpContext& dc) const{}
#endif
};
//------------------------------------------------------------------------
struct CPlex     // warning variable length structure
{
	CPlex* pNext;
	DWORD dwReserved[1];    // align on 8 byte boundary
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CPlex* PASCAL Create(CPlex*& pHead, UINT nMax, UINT cbElement)
			// like 'calloc' but no zero fill
			// may throw memory exceptions
	{
		ASSERT(nMax > 0 && cbElement > 0);
		CPlex* p = (CPlex*) new BYTE[sizeof(CPlex) + nMax * cbElement];
				// may throw exception
		p->pNext = pHead;
		pHead = p;  // change head (adds in reverse order for simplicity)
		return p;
	}

	void FreeDataChain()       // free this one and links
	{
		CPlex* p = this;
		while (p != NULL)
		{
			BYTE* bytes = (BYTE*) p;
			CPlex* pNext = p->pNext;
			delete[] bytes;
			p = pNext;
		}
	}
};

struct __POSITION { };
typedef __POSITION* POSITION;
//------------------------------------------------------------------------
// 以上定义替代原库中使用的一些东西
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
class CList : public CObject
{
protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		TYPE data;
	};
public:
	CList(int nBlockSize = 10);

	int GetCount() const;
	BOOL IsEmpty() const;

	TYPE& GetHead();
	TYPE GetHead() const;
	TYPE& GetTail();
	TYPE GetTail() const;

	TYPE RemoveHead();
	void RemoveHeadOnly();
	TYPE RemoveTail();
	void RemoveAll();

	POSITION AddHead(ARG_TYPE newElement);
	POSITION AddTail(ARG_TYPE newElement);
	void AddHead(CList* pNewList);
	void AddTail(CList* pNewList);

	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	TYPE& GetNext(POSITION& rPosition);			// return *Position++
	TYPE GetNext(POSITION& rPosition) const;	// return *Position++
	TYPE& GetPrev(POSITION& rPosition);			// return *Position--
	TYPE GetPrev(POSITION& rPosition) const;	// return *Position--

	TYPE& GetAt(POSITION position);
	TYPE GetAt(POSITION position) const;
	void SetAt(POSITION pos, ARG_TYPE newElement);
	void RemoveAt(POSITION position);

	POSITION InsertBefore(POSITION position, ARG_TYPE newElement);
	POSITION InsertAfter(POSITION position, ARG_TYPE newElement);

	POSITION Find(ARG_TYPE searchValue, POSITION startAfter = NULL) const;
	POSITION FindIndex(int nIndex) const;

protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
	CNode* m_pNodeFree;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;
protected:
	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);
public:
	~CList();
#ifdef _DEBUG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif
};
//------------------------------------------------------------------------
// CList<TYPE, ARG_TYPE> 内联函数
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
inline int CList<TYPE, ARG_TYPE>::GetCount() const
	{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
inline BOOL CList<TYPE, ARG_TYPE>::IsEmpty() const
	{ return m_nCount == 0; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetHead()
	{ ASSERT(m_pNodeHead != NULL);
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetHead() const
	{ ASSERT(m_pNodeHead != NULL);
		return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetTail()
	{ ASSERT(m_pNodeTail != NULL);
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetTail() const
	{ ASSERT(m_pNodeTail != NULL);
		return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
inline POSITION CList<TYPE, ARG_TYPE>::GetHeadPosition() const
	{ return (POSITION) m_pNodeHead; }
template<class TYPE, class ARG_TYPE>
inline POSITION CList<TYPE, ARG_TYPE>::GetTailPosition() const
	{ return (POSITION) m_pNodeTail; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		rPosition = (POSITION) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) const // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		rPosition = (POSITION) pNode->pNext;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		rPosition = (POSITION) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) const // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		rPosition = (POSITION) pNode->pPrev;
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE& CList<TYPE, ARG_TYPE>::GetAt(POSITION position)
	{ CNode* pNode = (CNode*) position;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline TYPE CList<TYPE, ARG_TYPE>::GetAt(POSITION position) const
	{ CNode* pNode = (CNode*) position;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		return pNode->data; }
template<class TYPE, class ARG_TYPE>
inline void CList<TYPE, ARG_TYPE>::SetAt(POSITION pos, ARG_TYPE newElement)
	{ CNode* pNode = (CNode*) pos;
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		pNode->data = newElement; }
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
CList<TYPE, ARG_TYPE>::CList(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveAll()
{
	ASSERT_VALID(this);

	CNode* pNode;
	for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		// 析构函数 <TYPE>(&pNode->data, 1);
		pNode->data.~TYPE();		//> 学习vc7.0的做法

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}
//------------------------------------------------------------------------
template<class TYPE, class ARG_TYPE>
CList<TYPE, ARG_TYPE>::~CList()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}
//------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Node helpers
//
// Implementation note: CNode's are stored in CPlex blocks and
//  chained together. Free blocks are maintained in a singly linked list
//  using the 'pNext' member of CNode with 'm_pNodeFree' as the head.
//  Used blocks are maintained in a doubly linked list using both 'pNext'
//  and 'pPrev' as links and 'm_pNodeHead' and 'm_pNodeTail'
//   as the head/tail.
//
// We never free a CPlex block unless the List is destroyed or RemoveAll()
//  is used - so the total number of CPlex blocks may grow large depending
//  on the maximum past size of the list.
//

template<class TYPE, class ARG_TYPE>
typename CList<TYPE, ARG_TYPE>::CNode*
CList<TYPE, ARG_TYPE>::NewNode( CNode* pPrev, CNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		CPlex* pNewBlock = CPlex::Create(m_pBlocks, m_nBlockSize,
				 sizeof(CNode));

		// chain them into free list
		CNode* pNode = (CNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
	ASSERT(m_pNodeFree != NULL);  // we must have something

	CList::CNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow

	ConstructElements<TYPE>(&pNode->data, 1);

	return pNode;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::FreeNode( CNode* pNode)
{
	// 析构 <TYPE>(&pNode->data, 1); //>学习vc7.0
	pNode->data.~TYPE();

	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	CNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	CNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::AddHead(CList* pNewList)
{
	ASSERT_VALID(this);

	ASSERT(pNewList != NULL);
	ASSERT_VALID(pNewList);

	// add a list of same elements to head (maintain order)
	POSITION pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::AddTail(CList* pNewList)
{
	ASSERT_VALID(this);
	ASSERT(pNewList != NULL);
	ASSERT_VALID(pNewList);

	// add a list of same elements
	POSITION pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

template<class TYPE, class ARG_TYPE>
TYPE CList<TYPE, ARG_TYPE>::RemoveHead()
{
	ASSERT_VALID(this);
	ASSERT(m_pNodeHead != NULL);  // don't call on empty list !!!
	ASSERT(IsValidAddress(m_pNodeHead, sizeof(CNode)));

	CNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveHeadOnly()
{
	ASSERT_VALID(this);
	ASSERT(m_pNodeHead != NULL);  // don't call on empty list !!!
	ASSERT(IsValidAddress(m_pNodeHead, sizeof(CNode)));

	CNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
TYPE CList<TYPE, ARG_TYPE>::RemoveTail()
{
	ASSERT_VALID(this);
	ASSERT(m_pNodeTail != NULL);  // don't call on empty list !!!
	ASSERT(IsValidAddress(m_pNodeTail, sizeof(CNode)));

	CNode* pOldNode = m_pNodeTail;
	TYPE returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::InsertBefore(POSITION position, ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	if (position == NULL)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
	{
		ASSERT(IsValidAddress(pOldNode->pPrev, sizeof(CNode)));
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeHead);
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::InsertAfter(POSITION position, ARG_TYPE newElement)
{
	ASSERT_VALID(this);

	if (position == NULL)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	ASSERT(IsValidAddress(pOldNode, sizeof(CNode)));
	CNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
	{
		ASSERT(IsValidAddress(pOldNode->pNext, sizeof(CNode)));
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeTail);
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveAt(POSITION position)
{
	ASSERT_VALID(this);

	CNode* pOldNode = (CNode*) position;
	ASSERT(IsValidAddress(pOldNode, sizeof(CNode)));

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
	}
	else
	{
		ASSERT(IsValidAddress(pOldNode->pPrev, sizeof(CNode)));
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
		ASSERT(IsValidAddress(pOldNode->pNext, sizeof(CNode)));
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::FindIndex(int nIndex) const
{
	ASSERT_VALID(this);

	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	CNode* pNode = m_pNodeHead;
	while (nIndex--)
	{
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		pNode = pNode->pNext;
	}
	return (POSITION) pNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, POSITION startAfter) const
{
	ASSERT_VALID(this);

	CNode* pNode = (CNode*) startAfter;
	if (pNode == NULL)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
		ASSERT(IsValidAddress(pNode, sizeof(CNode)));
		pNode = pNode->pNext;  // start after the one specified
	}

	for (; pNode != NULL; pNode = pNode->pNext)
		if (CompareElements<TYPE>(&pNode->data, &searchValue))
			return (POSITION)pNode;
	return NULL;
}

inline bool IsValidAddress(const void* p, size_t nBytes,bool bReadWrite = true)
{
	return ((p != NULL) && !IsBadReadPtr(p, nBytes) &&
		(!bReadWrite || !IsBadWritePtr(const_cast<void*>(p), nBytes)));
}

#ifdef _DEBUG
	template<class TYPE, class ARG_TYPE>
	void CList<TYPE, ARG_TYPE>::Dump(CDumpContext& dc) const
	{
		CObject::Dump(dc);
	}

	template<class TYPE, class ARG_TYPE>
	void CList<TYPE, ARG_TYPE>::AssertValid() const
	{
		if (m_nCount == 0)
		{
			// empty list
			ASSERT(m_pNodeHead == NULL);
			ASSERT(m_pNodeTail == NULL);
		}
		else
		{
			// non-empty list
			ASSERT(IsValidAddress(m_pNodeHead, sizeof(CNode)));  //> 原为IsValidAddress() 
			ASSERT(IsValidAddress(m_pNodeTail, sizeof(CNode)));
		}
	}
#endif //_DEBUG
#endif // #if !defined(__AFXTEMPL_H__)
//------------------------------------------------------------------------
/*
	功能：静态栈管理，用一个FILO数组实现，当队列已满时，新加入的对象被抛弃
	说明：CList虽然没有长度限制，但动态分配结点速度太慢、管理队列的消耗也太大
		  TYPE一般为对象指针
	用例：地图上会大量对象的增删，删除对象时将对象指针放到此栈中以便下次重用
*/
//------------------------------------------------------------------------
template<class TYPE,int nStackSize = 256>
class CStaticStack
{
public:
	CStaticStack()
	{
		m_nCount = 0;
	}
	~CStaticStack()
	{
		Close();
	};

	BOOL IsEmpty() const{return m_nCount == 0;}
	BOOL IsFull() const{return m_nCount == nStackSize;}

	int  GetCount() const{return m_nCount;}

	// 取得一个结点，要确保栈不为空
	TYPE Pop()
	{
		m_nCount--;
		return m_stack[m_nCount];
	}

	// 如果栈已满，返回FALSE，此结点不加到栈中，应用层可能要删除此对象
	BOOL Push(TYPE node)
	{
		if(m_nCount >= nStackSize)
			return FALSE;
		m_stack[m_nCount++] = node;
		return TRUE;
	}

	// 直接获取栈中的对象，一般可用于删除关闭栈时删除栈中保留的对象
	TYPE GetData(int nIndex){return m_stack[nIndex];}

	void Close()
	{
		m_nCount = 0;
	}
private:
	int		m_nCount;
	TYPE	m_stack[nStackSize];
};
//------------------------------------------------------------------------
class CCriticalSectionLock
{
// Constructor
public:
	CCriticalSectionLock()
	{
		InitializeCriticalSection(&m_sect);
	}

// Attributes
public:
	CRITICAL_SECTION m_sect;
// Operations
public:
	inline void Lock()
	{
		EnterCriticalSection(&m_sect);
	}
	inline void Unlock()
	{
		LeaveCriticalSection(&m_sect);
	}
	
// Implementation
public:
	virtual ~CCriticalSectionLock(){}
};
//------------------------------------------------------------------------
/*
	功能：
		简单先进先出缓冲模板类，实现简单的先进先出队列
	说明：
		一个KEY绑定一个对象，通过KEY查找对象
		把最近的对象缓冲起来，下次再用的时候优先在队列里查找，如果找不到才从硬盘上读取
		缓冲大小在创建时定义的，而后不可改变
	实现：
		常用的一般是刚加到队列未尾的，查找时从后向前查找
	参数：
		TYPE : 缓冲对象类型，需要支持TYPE->Release()方法，如IBitmap*。必须为指针，因为有些返回值为NULL
		KEY  : 标识对象，一般为索引
		nCacheSize : 缓冲数量，如地表缓冲图片为48个格图片
 */
//------------------------------------------------------------------------
template<class TYPE,class KEY,int nCacheSize = 48>
class CFIFOCache
{
public:
	CFIFOCache()
	{
	}
	~CFIFOCache()
	{
		Close();
	}
	// 取得缓冲的对象，并从缓冲队列中删除
	TYPE GetObj(KEY key);

	// 查找对象是否存在，如果找不到，返回NULL
	TYPE Find(KEY key);

	// 增加一个对象，相当于放到回收站，如果发现回收站中已经有相同的对象，则删除旧对象
	void AddTail(KEY key,TYPE obj);

	// 有些对象已经不跟key绑定，仅是一个有用的内存而已，如一个已经跟key无关的IBitmap内存
	// 将这种对象放到队列开头，key取一个无效值即可。此函数的实现跟GetTail()不同，并不删除重复ID的对象
	// 因为是空对象，所以当队列满时不加入到队列中
	void AddGarbageToHead(KEY key,TYPE obj);

	// 垃圾再用时，取出头结点，如果队列为空，返回NULL
	TYPE PopHead()
	{
		if(m_CacheList.IsEmpty())
			return NULL;
		return m_CacheList.RemoveHead().obj;
	}

	// 队列是否为空
	BOOL IsEmpty()
	{
		return m_CacheList.IsEmpty();
	}

	BOOL IsFull()
	{
		return m_CacheList.GetCount() >= nCacheSize;
	}

	// 回收不再使用的所有对象，并负责删除CNode.obj
	void Close();

#ifndef _DEBUG
	private:
#endif

	struct CNode
	{
		KEY			key;
		TYPE		obj;
	};

	CList<CNode,CNode&> m_CacheList;
};
//------------------------------------------------------------------------
template<class TYPE,class KEY,int nCacheSize>
TYPE CFIFOCache<TYPE,KEY,nCacheSize>::Find(KEY key)
{
	POSITION pos = m_CacheList.GetTailPosition();
	while(pos)
	{
		CNode& node = m_CacheList.GetPrev(pos);
		if(node.key == key)
			return node.obj;
	}
	return NULL;
}

template<class TYPE,class KEY,int nCacheSize>
TYPE CFIFOCache<TYPE,KEY,nCacheSize>::GetObj(KEY key)
{
	POSITION pos = m_CacheList.GetTailPosition();
	while(pos)
	{
		POSITION posLast = pos;
		CNode node = m_CacheList.GetPrev(pos);
		if(node.key == key)
		{
			m_CacheList.RemoveAt(posLast);
			return node.obj;
		}
	}
	return NULL;
}

template<class TYPE,class KEY,int nCacheSize>
void CFIFOCache<TYPE,KEY,nCacheSize>::AddTail(KEY key,TYPE obj)
{
	TYPE oldobj = GetObj(key);
	if(oldobj)
		oldobj->Release();

	CNode node;
	if(m_CacheList.GetCount() > nCacheSize)
	{
		node = m_CacheList.RemoveHead();
		node.obj->Release();
	}
	node.key = key;
	node.obj = obj;
	m_CacheList.AddTail(node);
}

template<class TYPE,class KEY,int nCacheSize>
void CFIFOCache<TYPE,KEY,nCacheSize>::AddGarbageToHead(KEY key,TYPE obj)
{
	if(m_CacheList.GetCount() >= nCacheSize)
		return;
	CNode node;
	node.key = key;
	node.obj = obj;
	m_CacheList.AddHead(node);
}

template<class TYPE,class KEY,int nCacheSize>
void CFIFOCache<TYPE,KEY,nCacheSize>::Close()
{
	POSITION pos = m_CacheList.GetHeadPosition();
	while(pos)
	{
		CNode node = m_CacheList.GetNext(pos);
		node.obj->Release();
	}
}
//------------------------------------------------------------------------
/*
	功能：对大量使用的可变长数组的缓冲管理。释放对象时，按几种固定大小分类放到各自的队列中，
		  下次使用时优先重用这些对象
		  如：当分配长度为2、3、4的数组时，实际分配的都是长度为4的数组
		  CMultLenArray管理可变长数组，CMultLenArrayCache缓冲变长数组对象
	用途：
		如：地图上每块可能包含多个对象，数量不定，所以建立若干个缓冲不同长度数组的队列，
		如支持1、4、8、32等长度的可变长数组
		一般通过CMultLenArray::Create()静态函数创建CMultLenArray，因为CMultLenArray长度不固定

	对象定义如下：
		struct CNode
		{
			TYPE pre;		// 一些自定义信息
			DATA data[];	// 对象数组，长度不一
			// 这后面跟不同长度的数组
		}
	使用：
		  一、用法1
			  1、通过CMultLenArrayCache.AddCacheList()创建不同长度类型的数组队列，如1,4,8,16,32等
			  2、CMultLenArrayCache.AddData()用于添加数组中的数据，AddData决定是否需要换一个更长的数组
			  3、当变长数组不再需要时，通过2、CMultLenArrayCache.FreeArray()回收数组
		  二、用法2，例：
				char buf[4096];
				CMultLenArray<DWORD,DWORD> *p = (CMultLenArray<DWORD,DWORD>*)(buf);
				p->Init(100); // 100数组单元个数限制
				p->Add(3);
				p->Add(4);
				int nBufLen = p->GetMemSize();
				......
*/
//------------------------------------------------------------------------
#pragma warning(disable : 4200)
//------------------------------------------------------------------------
template<class TYPE,class DATA>
class CMultLenArray
{
public:
	CMultLenArray(int nArraySize)
	{
		wCount = 0;
		this->wArraySize = nArraySize;
	}
	~CMultLenArray()
	{
		for(int i = 0; i < wArraySize; i++)
			data[i].~DATA();
		wCount = 0;
		wArraySize = 0;
	}
	void Init(int nArraySize = 32)
	{
		wCount = 0;
		this->wArraySize = nArraySize;
	}
	static CMultLenArray<TYPE,DATA>* Create(int nArraySize)
	{
		if(nArraySize < 0)
			return NULL;
		CMultLenArray<TYPE,DATA>* p = (CMultLenArray<TYPE,DATA>*)new char[sizeof(CMultLenArray<TYPE,DATA>) + sizeof(DATA) * nArraySize];
		if(p == NULL)
			return NULL;
		p->wCount = 0;
		p->wArraySize = nArraySize;
		//for(int i = 0; i < nArraySize; i++)
		//	p->data[i].DATA();		// 本来想调用一下构造函数，但当DATA为int型时编译不通过
		return p;
	}

	// 检查数组是否已满，当数组满员时，CMultLenArrayCache会删除此数组，放入一个更大的数组中
	inline BOOL IsFull()const{return wCount >= wArraySize;}
	inline BOOL IsEmpty()const{return wCount == 0;}
	inline BOOL IsValid()const {return wArraySize >= wCount;}

	// 增加一个data单元
	inline BOOL Add(const DATA& data)
	{
		if(wCount >= wArraySize)
			return FALSE;
		this->data[wCount++] = data;
		return TRUE;
	}

	// 同Add()，不作有效性检查
	inline void _Add(const DATA& data)
	{
		this->data[wCount++] = data;
	}

	//删除有效结点，其它所有结点向前移
	BOOL RemoveAt(int index)
	{
		ASSERT(index < (int)wCount);
		memmove(&data[index],&data[index + 1],(wCount - index - 1) * sizeof(DATA));
		wCount--;
		return TRUE;
	}

	// 删除数组中所有相同的对象
	BOOL Remove(const DATA& data)
	{
		for(int index = 0; index < (int)wCount; index++)
			if(this->data[index] == data)
			{
				memmove(&this->data[index],&this->data[index + 1],(wCount - index - 1) * sizeof(DATA));
				wCount--;
				return TRUE;
			}
		return FALSE;
	}

	// 将item清0，表示此对象已经被删除
	// 说明：为解决循环数组时数组对象已经被现象，提供此函数
	//       要求DATA结构的第一个变量为清除标志，一般此时DATA的结构是一个指针
	BOOL ZeroItem(const DATA& data)
	{
		for(int index = 0; index < (int)wCount; index++)
			if(this->data[index] == data)
			{
				// 将第一个DWORD清为0
				(DWORD&)this->data[index] = 0;
				return TRUE;
			}
		return FALSE;
	}

	// 检查是否数组中所有对象都已经无效
	BOOL IsFullZero()
	{
		// 检查是不是所有
		for(int index = 0; index < (int)wCount; index++)
		{
			if((DWORD&)this->data[index] != 0)
				return FALSE;
		}
		return index == (int)wCount;
	}

	// 注意：如果是p此类的指针，必须这样访问：(*p)[0] = 1;
	// 我都犯了这个错误了，呵, p[0] = 1;(X)
	inline DATA& operator[](int i )
	{
		ASSERT(i < (int)wCount);
		return data[i];
	}
	
	// 取得用户数据地址，用户数据定义为PreData + data[]
	inline void* GetBuffer(){return &PreData;}

	// 取得data数组的地址
	inline DATA* GetDataBuffer(){return data;}
	inline TYPE& GetPreData(){return PreData;}

	inline int GetCount()const{return wCount;}
	inline int GetArraySize()const{return wArraySize;}

	// 获取有效数据内存占用字节数，用于通讯时发送若干元素很有用
	int GetMemSize()const{return sizeof(CMultLenArray<TYPE,DATA>) + wCount * sizeof(DATA);}

	//friend CMultLenArrayCache<TYPE,DATA>;	// 模板类无法申明成友元?
protected:
	WORD wCount;		// 当前结点数
	WORD wArraySize;	// 最大结点数
	TYPE PreData;
	DATA data[];		// 这后面跟不同长度的数组
};
//------------------------------------------------------------------------
template<class TYPE,class DATA>
class CMultLenArrayCache
{
public:
	enum{maxArrayKind = 16};
	typedef CMultLenArray<TYPE,DATA>		ArrayType;
	typedef CStaticStack<ArrayType*,512>	ArrayList;

	CMultLenArrayCache()
	{
		memset(&m_list[0],0,sizeof(m_list));
		memset(&m_SizeList[0],0,sizeof(m_SizeList));
		m_nCount = 0;
	}
	~CMultLenArrayCache(){Close();}
	void Close();
	
	// 增加一个缓冲队列，如分别加入支持1、4、8、32等长度对象的缓冲队列
	// 注意：按从小到大的顺序加入，我不写排序操作了
	BOOL AddCacheList(int nArraySize);

	// 分配一个新的不定长数组；复制pOldArray中旧的内容；pOldArray被回收到垃圾站
	// 说明：优先检查cache队列中是否可重用结点；如果创建更大的结点失败，pOldArray
	ArrayType* AllocArray(int nItemCount,ArrayType* pOldArray = NULL);

	// 删除一个不定长数组
	// 说明：删除时将此结点放到缓冲起来
	void FreeArray(ArrayType* pArray);

	// 添加一个data，如果*ppArray为空，则分配一个最小的Array
	// *ppArray返回新的pArray，因为旧数组可能已经换成一个更长的数组了
	// 如果添加失败，返回FALSE，旧数组不变
	BOOL AddData( ArrayType** ppArray,DATA& data);

	// 删除一个data，如果数组中已经无新的数据，则自动删除此数组，将此数组入到垃圾回收站中
	BOOL RemoveData(ArrayType** ppArray,DATA& data);
	
	// 将数组中的对象设置为0，表示已经对象已经被删除
	BOOL ZeroItem(ArrayType** ppArray,DATA& data);

protected:
	int			m_nCount;
	ArrayList*	m_list[maxArrayKind];
	int			m_SizeList[maxArrayKind];	// m_list各项分别对应的长度
};
//------------------------------------------------------------------------
template<class TYPE,class DATA>
void CMultLenArrayCache<TYPE,DATA>::Close()
{
	for(int i = 0; i < m_nCount; i++)
	{
		if(m_list[i])
		{
			int nCount = m_list[i]->GetCount();
			for(int j = 0; j < nCount; j++)
				delete m_list[i]->GetData(j);
			delete m_list[i];
			m_list[i] = NULL;
		}
		m_nCount = 0;
	}
}

template<class TYPE,class DATA>
BOOL CMultLenArrayCache<TYPE,DATA>::AddCacheList(int nSize)
{
	if(m_nCount >= maxArrayKind)
		return FALSE;
	ArrayList* pList = new ArrayList();
	if(pList == NULL)
		return FALSE;
	m_SizeList[m_nCount] = nSize;
	m_list[m_nCount++] = pList;
	return TRUE;	
};

template<class TYPE,class DATA>
BOOL CMultLenArrayCache<TYPE,DATA>::AddData(ArrayType** ppArray,DATA& data)
{
	ASSERT(ppArray != NULL);
	ArrayType* pArr;
	if(*ppArray != NULL)
	{
		if((*ppArray)->Add(data))
			return TRUE;
		if(NULL == (pArr = AllocArray((*ppArray)->GetCount() + 1,*ppArray)))
			return FALSE;
	}
	else
	{
		if(NULL == (pArr = AllocArray(m_SizeList[0])))
			return FALSE;
	}
	pArr->_Add(data);	// 肯定成功
	*ppArray = pArr;
	return TRUE;
}

template<class TYPE,class DATA>
BOOL CMultLenArrayCache<TYPE,DATA>::RemoveData(ArrayType** ppArray,DATA& data)
{
	ASSERT(ppArray != NULL);
	ArrayType* pArr = *ppArray;
	if(pArr == NULL)
		return NULL;
	BOOL retv = pArr->Remove(data);
	if(pArr->IsEmpty())
	{
		FreeArray(pArr);
		*ppArray = NULL;
	}
	return retv;
}

template<class TYPE,class DATA>
BOOL CMultLenArrayCache<TYPE,DATA>::ZeroItem(ArrayType** ppArray,DATA& data)
{
	ASSERT(ppArray != NULL);
	ArrayType* pArr = *ppArray;
	if(pArr == NULL)
		return NULL;
	BOOL retv = pArr->ZeroItem(data);
	if(pArr->IsEmpty())
	{
		FreeArray(pArr);
		*ppArray = NULL;
	}
	return retv;
}


template<class TYPE,class DATA>
CMultLenArray<TYPE,DATA>* CMultLenArrayCache<TYPE,DATA>::AllocArray(int nItemCount,ArrayType* pOldArray)
{
	int i = 0;
	for(; m_SizeList[i] < nItemCount && i < m_nCount;i++);
	if(i >= m_nCount)	// 找不到一个可以容纳这么多结点的数组列队
		return NULL;
	ArrayType* pArray;
	if(!m_list[i]->IsEmpty())
	{
		pArray = m_list[i]->Pop();
		pArray->Init(m_SizeList[i]);
	}
	else if(NULL == (pArray = ArrayType::Create(m_SizeList[i])))
		return NULL;

	// 将旧数据复制到新数组中
	if(pOldArray)
	{
		int nCount = pOldArray->GetCount();
		for(int j = 0; j < nCount; j++)
			pArray->_Add((*pOldArray)[j]);
		FreeArray(pOldArray);
	}
	return pArray;
}

template<class TYPE,class DATA>
void CMultLenArrayCache<TYPE,DATA>::FreeArray(ArrayType* pArray)
{
	if(pArray == NULL)
		return;
	int nSize = pArray->GetArraySize();
	int i = 0;
	for(; m_SizeList[i] != nSize && i < m_nCount;i++);
	ASSERT(i < m_nCount);

	if(!m_list[i]->Push(pArray))	// 如果已经满，删除此变长数组。些时pArray已经不包含有效数据，所以可直接删除
		delete pArray;
}
//------------------------------------------------------------------------
typedef CMultLenArray<DWORD,void*>		PointerArray;
typedef CMultLenArray<DWORD,DWORD>		DwordArray;
typedef CMultLenArrayCache<DWORD,void*> PointerArrayCache;
typedef CMultLenArrayCache<DWORD,DWORD> DwordArrayCache;
//------------------------------------------------------------------------
#if !defined(__AFXTEMPL_H__)

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMap : public CObject
{
protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		KEY key;
		VALUE value;
	};
public:
// Construction
	CMap(int nBlockSize = 10);

// Attributes
	// number of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(ARG_KEY key, VALUE& rValue) const;

// Operations
	// Lookup and add if not there
	VALUE& operator[](ARG_KEY key);

	// add a new (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(ARG_KEY key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(ARG_KEY, UINT&) const;

public:
	~CMap();
#ifdef _DEBUG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif
};
//------------------------------------------------------------------------
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline int CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetCount() const
	{ return m_nCount; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::IsEmpty() const
	{ return m_nCount == 0; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
	{ (*this)[key] = newValue; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline POSITION CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
inline UINT CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetHashTableSize() const
	{ return m_nHashTableSize; }
//------------------------------------------------------------------------
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMap(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
//
// Used to force allocation of a hash table or to override the default
//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new CAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
	ASSERT_VALID(this);

	if (m_pHashTable != NULL)
	{
		// destroy elements (values and keys)
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			  pAssoc = pAssoc->pNext)
			{
				//DestructElements<VALUE>(&pAssoc->value, 1);
				//DestructElements<KEY>(&pAssoc->key, 1);
				pNode->value.~TYPE();		//> 学习vc7.0的做法
				pNode->key.~TYPE();			//> 学习vc7.0的做法
			}
		}
	}

	// free hash table
	delete[] m_pHashTable;
	m_pHashTable = NULL;

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::~CMap()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CPlex* newBlock = CPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CMap::CAssoc));
		// chain them into free list
		CMap::CAssoc* pAssoc = (CMap::CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something

	CMap::CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
	ConstructElements<KEY>(&pAssoc->key, 1);
	ConstructElements<VALUE>(&pAssoc->value, 1);   // special construct values
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc( CAssoc* pAssoc)
{
	DestructElements<VALUE>(&pAssoc->value, 1);
	DestructElements<KEY>(&pAssoc->key, 1);
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = HashKey<ARG_KEY>(key) % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (CompareElements(&pAssoc->key, &key))
			return pAssoc;
	}
	return NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	ASSERT_VALID(this);

	UINT nHash;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);

	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	CAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey<ARG_KEY>(key) % m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (CompareElements(&pAssoc->key, &key))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(POSITION& rNextPosition,
	KEY& rKey, VALUE& rValue) const
{
	ASSERT_VALID(this);
	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);

	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
		ASSERT(pAssocRet != NULL);  // must find something
	}

	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		  nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}

#ifdef _DEBUG
	template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
	void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Dump(CDumpContext& dc) const
	{
		CObject::Dump(dc);

		dc << "with " << m_nCount << " elements";
		if (dc.GetDepth() > 0)
		{
			// Dump in format "[key] -> value"
			KEY key;
			VALUE val;

			POSITION pos = GetStartPosition();
			while (pos != NULL)
			{
				GetNextAssoc(pos, key, val);
				dc << "\n\t[";
				DumpElements<KEY>(dc, &key, 1);
				dc << "] = ";
				DumpElements<VALUE>(dc, &val, 1);
			}
		}

		dc << "\n";
	}

	template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
	void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::AssertValid() const
	{
		CObject::AssertValid();

		ASSERT(m_nHashTableSize > 0);
		ASSERT(m_nCount == 0 || m_pHashTable != NULL);
			// non-empty map should have hash table
	}
#endif //_DEBUG
#endif // #if !defined(__AFXTEMPL_H__)
//------------------------------------------------------------------------