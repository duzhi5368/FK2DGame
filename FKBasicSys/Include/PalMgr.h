/**
*	created:		2012-7-2   1:12
*	filename: 		PalMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "ObjPool.h"
//------------------------------------------------------------------------
enum PaletteType
{
	PalType_Light = 0,		// 加亮
	PalType_Frozen,			// 冰冻
	PalType_Toxic,			// 中毒
	PalType_Berserk,		// 狂暴
	PalType_Stone,			// 石化

	PalType_MaxType,		// 最大类型
};
//------------------------------------------------------------------------
/*
 * 思路：
 * 以list保存节点信息，引用频繁的放list开头，不频繁的放于list尾部，利用了list的快速增删操作
 * 节点的查询通过hash_map快速查询，hash_map记录了各节点在list中的位置，利用了hash_map的快速搜索操作
*/
typedef __int64  __KEY;
//------------------------------------------------------------------------
class CPalette
{
	BYTE	m_pal[512];
	int		m_nUseColor;
public:
	CPalette(){};
	CPalette(BYTE* pPal, int nUseColor) {SetPalette(pPal, nUseColor);}
	virtual ~CPalette(){};
	
	BYTE* GetPalette() {return m_pal;}
	int GetUseColor() {return m_nUseColor;}
	int GetPalSize() {return m_nUseColor<<1;}
	void SetPalette(BYTE* pPal, int nUseColor);
	void Release() {delete this;}

	void Modify(int nPalTyle);
	
	void Light();
	void Light(int nBrightness, int nContrast);
	void Blue();
	void Green();
	void Red();
};
//------------------------------------------------------------------------
// 调色板节点
class CPalNode
{
	CPalette*	m_pPal;
	__KEY		m_dwID;
public:
	CPalNode() : m_pPal(NULL),m_dwID(0) {}
	CPalNode(CPalette* pPal, __KEY dwID) : m_pPal(pPal),m_dwID(dwID) {}
	virtual ~CPalNode() {}
	__KEY GetKey() {return m_dwID;}
	void Release() {m_pPal->Release();}
	CPalette* GetPal() {return m_pPal;}
};
//------------------------------------------------------------------------
template <DWORD dwMaxPalCount = 100>// 最大可存放调色板数
class CPalPool
{
private:
	typedef list<CPalNode>							NodeList;
	typedef list<CPalNode>::iterator				NodeListPtr;
	typedef hash_map<__KEY, NodeListPtr>			HashMap;
	typedef hash_map<__KEY, NodeListPtr>::iterator	HashMapPtr;

	NodeList	m_NodeList;		// 节点列表
	HashMap		m_HashMap;		// 哈希表

public:
	CPalPool(){Close();}
	virtual ~CPalPool(){Close();}
	void Close()
	{
		// 调用当前列表中的每个对象的Release
		for (NodeListPtr it=m_NodeList.begin(); it!=m_NodeList.end(); it++)
			(*it).Release();

		m_NodeList.clear();
		m_HashMap.clear();
	}
	
	// 添加一个节点,放于list的前端
	BOOL AddPal(CPalNode& pNode)
	{
		// 如果节点总尺寸或节点数目超出范围，则删除列表后面的节点
		while (m_NodeList.size() > dwMaxPalCount)
		{
#ifdef _DEBUG
			static int flag = 0;
			if (flag == 0)
			{
				flag = 1;
				TraceLn("Warning: pal pool overflow!");
			}
#endif
			// remove the back node
			NodeListPtr it = --m_NodeList.end();
			(*it).Release();
			m_HashMap.erase((*it).GetKey());
			m_NodeList.pop_back();
		}

		// 增加一个对象,并记录到哈希表
		m_NodeList.push_front(pNode);
		m_HashMap[pNode.GetKey()] = m_NodeList.begin();

		return TRUE;
	}
	
	// 删除节点。  注:节点并未Release, 仅仅是从池中移除,将节点的控制
	// 权交给调用方,rtnNode为返回节点的引用，是否释放由调用方自行决定
	BOOL RemovePal(__KEY dwID, CPalNode& rtnNode)
	{
		HashMapPtr it = m_HashMap.find(dwID);
		if (it != m_HashMap.end()) // exist!
		{
			NodeListPtr list_it = (*it).second;
			rtnNode = (*list_it);

			m_HashMap.erase(dwID);
			m_dwMemSize -= rtnNode.GetSize();
			m_NodeList.erase(list_it);
			return TRUE;
		}
		return FALSE;
	}

	// 查找对应键值的节点
	BOOL Find(__KEY dwID, CPalNode*& pNode)
	{
		HashMapPtr it = m_HashMap.find(dwID);
		if (it != m_HashMap.end())
		{
			NodeListPtr list_it = (*it).second;
			
			// erase from back and move to front
			CPalNode node = *list_it;
			m_NodeList.erase(list_it);
			m_NodeList.push_front(node);
			pNode = &m_NodeList.front();
			m_HashMap[dwID] = m_NodeList.begin();

			return (pNode != NULL);
		}
		return FALSE;
	}
};
//------------------------------------------------------------------------
class CPalMgr  
{
	CPalPool<>		m_PalPool;	// 调色板缓冲池
public:
	CPalMgr() {}
	virtual ~CPalMgr() {m_PalPool.Close();};

	// 请求指定ID指定类型的调色板，没有则创建
	CPalette* RequestPal(DWORD dwID, int nCurPalType, int nPalIndex, BYTE* pSrcPal, int nUseColor, int nPalType = PalType_Light)
	{
		CPalNode* pNode;
		__KEY dwNewID = (nPalType<<24) | (nCurPalType<<16) | (nPalIndex+1); // 因为nPalIndex=-1,0,...，所以加1
		dwNewID = (dwNewID <<32) | dwID;
		if (m_PalPool.Find(dwNewID, pNode))
			return pNode->GetPal();

		if (!pSrcPal)
			return NULL;
		
		CPalette* pPal = new CPalette(pSrcPal, nUseColor);
		if (!pPal)
			return NULL;
		pPal->Modify(nPalType);
		CPalNode node(pPal, dwNewID);
		m_PalPool.AddPal(node);
		return node.GetPal();
	}
};
//------------------------------------------------------------------------
extern CPalMgr	g_PalMgr;
//------------------------------------------------------------------------