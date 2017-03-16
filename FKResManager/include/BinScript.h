/**
*	created:		2012-6-18   21:34
*	filename: 		BinScript
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <hash_map>
using stdext::hash_map;
//------------------------------------------------------------------------
class CBinScript  : public IBinScript
{	
private:
	hash_map<UINT, DWORD>		m_OffsetMap;			// ID-Offset映射表
	IFileObject*				m_pFileObject;			// 文件对象
	DWORD						m_dwObjectCount;		// 对象总数
	hash_map<UINT, CBinObject*>	m_HashMap;				// 记录已经创建了的BinObjectInfo
	bool						m_bReleaseFileObject;	// 文件对象是否需要释放的标志
public:
	CBinScript();
	~CBinScript();
public:
	HANDLE GetObject(UINT uID);
	int GetObjectCount() {return m_dwObjectCount;}
	BOOL Release();

	BYTE* GetBuffer(HANDLE hBinObject);
	int GetSize(HANDLE hBinObject);
	int GetUnFixPropertyCount(HANDLE hBinObject);
	int GetUnFixPropertyOffset(HANDLE hBinObject, int nIndexEx);
	int GetUnFixPropertyLength(HANDLE hBinObject, int nIndexEx);
	int GetSubObjectCount(HANDLE hBinObject);
	HANDLE GetSubObject(HANDLE hBinObject, int nIndex);
	BOOL Delete(UINT uID);

	LPCSTR GetScriptFileName(){return m_pFileObject->GetFilePath();}
	void Empty();
	BOOL LoadBinScript(IFileObject* pFileObject, bool bReleaseFileObject);
	BOOL AddToHashMap(UINT uID, CBinObject* pBinObject);
	BOOL EraseFromHashMap(UINT uID);
};
//------------------------------------------------------------------------