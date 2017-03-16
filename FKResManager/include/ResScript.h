/**
*	created:		2012-6-18   20:16
*	filename: 		ResScript
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
class CResScript  : public IResScript
{	
private:
	int							m_nCount;				// 所有对象总数
	CResObject*					m_pResObjectRoot;		// 根对象
	IFileObject*				m_pFileObject;			// 文件对象指针
	hash_map<UINT, CResObject*>	m_HashMap;				// 对象的ID映射表
	hash_map<string, TYPEINFO*>	m_TypeList;				// 类型模板映射表
	bool						m_bReleaseFileObject;	// 文件对象是否需要释放的标志
public:
	CResScript();
	virtual ~CResScript();

	// IResScript
	// 新建空的资源脚本(返回最上层对象列表指针)
	BOOL Create(int nStartID = 0);
	// 加载资源脚本(返回最上层对象列表指针)
	BOOL LoadScript(IFileObject* pFileObject, BOOL bIgnoreID, bool bReleaseFileObject, BYTE* pKey = NULL);
	BOOL LoadTypeList(LPCSTR szTypeListFileName);
	// 保存资源脚本
	BOOL SaveScript(LPCSTR szResScriptFileName = NULL, BYTE* pKey = NULL);
	// 编译
	BOOL Compile(LPCSTR szFileName = NULL);
	// 访问指定ID的资源对象(内部通过哈希表快速访问)
	IResObject* GetResObject(UINT uID);
	IResObject* GetResObjectRoot();
	// 获得本脚本中所有对象总数
	int GetObjectCount();
	// 获得本脚本的文件名
	LPCSTR GetScriptFileName();
	UINT GetMaxID();
	// 目前仅用于文件服务器的下载等操作
	BOOL ParseObjectServer(UINT uID, char* pData);
	IResObject* ParseObjectClient(char* pData, int nSize);
	// 释放资源脚本
	BOOL Release();

	BOOL AddToHashMap(UINT uID, IResObject* pResObject);
	BOOL EraseFromHashMap(UINT uID);
	BOOL WriteResObject(IResObject* pResObject, FILE* fp);
	LPCSTR GetPath();
	UINT NextMaxID();
	void Empty();
	BOOL WriteBinObject(IResObject* pResObject, vector<ID_OFFSET>& idOffsetTable, int& nFilePos, FILE* fp);
	BOOL WriteRootObject(IResObject* pResObject, vector<ID_OFFSET>& idOffsetTable, int& nFilePos, FILE* fp);
#ifdef _DEBUG
	BOOL Dump();
#endif
};
//------------------------------------------------------------------------