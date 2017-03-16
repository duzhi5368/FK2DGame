/**
*	created:		2012-7-2   19:54
*	filename: 		HashTable
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
template<>
AFX_INLINE UINT AFXAPI HashKey(DWORD key)
{ // override HashKey for my usage
	return key;
}
//------------------------------------------------------------------------
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CHashTable  : public CMap<KEY, ARG_KEY,VALUE,ARG_VALUE>
{
public:
	class CMyAssoc : public CPair
	{
	public:
		CMyAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration

		CMyAssoc( ARG_KEY key ) : CPair( key ) {}
	};

	CHashTable(int nBlockSize = 10) 
		: CMap<KEY, ARG_KEY,VALUE,ARG_VALUE>(nBlockSize){};
	~CHashTable(){};

	// 查找
	BOOL Find(ARG_KEY key)	// 查找指定key是否存在，不象愚蠢的CMap::Lookup()返回结点的拷贝,CMap::operator[] 会自作聪明地加一个结点
	{
		UINT nHashBucket,nHashValue;
		CAssoc* pAssoc = GetAssocAt(key, nHashBucket,nHashValue);
		return pAssoc != NULL;
	};

	// 查找，如果找到，返回找到的结点的地地址，以便后面用GetAt()取得结点，这一切不就是为了速度吗
	BOOL Find(ARG_KEY key,POSITION& pos)
	{
		ASSERT_VALID(this);

		UINT nHashBucket,nHashValue;
		CAssoc* pAssoc = GetAssocAt(key, nHashBucket,nHashValue);

		if(pAssoc == NULL)
			return FALSE;
		pos = (POSITION)pAssoc;
		return TRUE;
	}
	
	// 根据Find(key,pos)返回的pos，取得值
	VALUE& GetAt(POSITION pos)
	{
		ASSERT_VALID(this);
		return ((CAssoc*)pos)->value;
	}

	// 随机查找第一个有效在线用户，从指定的一个序号开始
	BOOL FindRandomNextNode(ARG_KEY key,POSITION& posNext,VALUE **ppValue = NULL)
	{ 
		// 用于读取一组在线用户时，查得第一个结点，返回下一结点的位置
		if (m_pHashTable == NULL)
			return FALSE;
		UINT nHash = HashKey<ARG_KEY>(key) % m_nHashTableSize;
		UINT nEnumCount = 0;
		while(m_pHashTable[nHash] == NULL)
		{ 
			// 循环查找一个有效结点
			//.....注意此处可能存在性能问题，如人数极少且大于50 * m_nHashTableSize时
			nHash++;
			if(nHash >= m_nHashTableSize)
				nHash = 0;
			nEnumCount++;
			if(nEnumCount >= m_nHashTableSize)
				return FALSE;
		}
		posNext = (POSITION)m_pHashTable[nHash];
		if(ppValue != NULL)
			*ppValue = &((CAssoc*)posNext)->value;
		GetNextNode(posNext,NULL);
		return TRUE;
	}
	// 此函数与CMap::GetNextAssoc()一样，但可以不必返回结点值，也可以返回结点值指针
	void GetNextNode(POSITION& rNextPosition,VALUE** ppValue = NULL)
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
		CMyAssoc* pAssocNext;
		if ((pAssocNext = ((CMyAssoc*)pAssocRet)->pNext) == NULL)
		{
			// go to next bucket
			for (UINT nBucket = (((CMyAssoc*)pAssocRet)->nHashValue % m_nHashTableSize) + 1;
			  nBucket < m_nHashTableSize; nBucket++)
				if ((pAssocNext = (CMyAssoc*)m_pHashTable[nBucket]) != NULL)
					break;
		}

		rNextPosition = (POSITION) pAssocNext;

		// fill in return data
		if(ppValue != NULL)
			*ppValue = &pAssocRet->value;
	}
};
//------------------------------------------------------------------------