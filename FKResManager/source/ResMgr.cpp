/**
*	created:		2012-6-18   22:18
*	filename: 		ResMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/ResMgrCommon.h"
#include "../Include/ResObject.h"
#include "../Include/ResScript.h"
#include "../Include/BinScript.h"
#include "../Include/ResMgr.h"
//------------------------------------------------------------------------
CResMgr::CResMgr()
{
}
//------------------------------------------------------------------------
CResMgr::~CResMgr()
{
}
//------------------------------------------------------------------------
// 创建IResScript(资源脚本接口)
IResScript* CResMgr::CreateResScript(int nStartID/* = 0*/)
{
	CResScript* pResScript = new CResScript();
	Assert(pResScript);
	if (!pResScript->Create(nStartID))
	{
		SAFE_RELEASE(pResScript);
		return NULL;
	}

	TraceLn("创建资源脚本 [nStartID=%d] 完成", nStartID);
	return static_cast<IResScript*>(pResScript);
}
//------------------------------------------------------------------------
// 加载资源脚本
IResScript* CResMgr::LoadResScript(LPCSTR szResFileName, BOOL bIgnoreID, BYTE* pKey)
{
	IFileObject* pFileObject = _create_fileobject(szResFileName);
	Assert(pFileObject);
	CResScript* pResScript = new CResScript();
	Assert(pResScript);
	if (!pResScript->LoadScript(pFileObject, bIgnoreID, true, pKey))
	{
		SAFE_RELEASE(pResScript);
		return NULL;
	}

	TraceLn("加载资源脚本 ['%s'] 完成", pFileObject->GetFilePath());
	return static_cast<IResScript*>(pResScript);
}
//------------------------------------------------------------------------
// 加载资源脚本
IResScript* CResMgr::LoadResScript(IFileObject* pFileObject, BOOL bIgnoreID, BYTE* pKey)
{
	CResScript* pResScript = new CResScript();
	Assert(pResScript);
	if (!pResScript->LoadScript(pFileObject, bIgnoreID, false, pKey))
	{
		SAFE_RELEASE(pResScript);
		return NULL;
	}

	TraceLn("加载资源脚本 ['%s'] 完成", pFileObject->GetFilePath());
	return static_cast<IResScript*>(pResScript);
}
//------------------------------------------------------------------------
// 加载二进制脚本文件
IBinScript* CResMgr::LoadBinScript(LPCSTR szBinFileName)
{
	IFileObject* pFileObject = _create_fileobject(szBinFileName);
	Assert(pFileObject);
	CBinScript* pBinScript = new CBinScript();
	Assert(pBinScript);
	if (!pBinScript->LoadBinScript(pFileObject, true))
	{
		SAFE_RELEASE(pBinScript);
		return NULL;
	}

	TraceLn("加载资源脚本 ['%s'] 完成", pFileObject->GetFilePath());
	return static_cast<IBinScript*>(pBinScript);
}
//------------------------------------------------------------------------
IBinScript* CResMgr::LoadBinScript(IFileObject* pFileObject)
{
	CBinScript* pBinScript = new CBinScript();
	Assert(pBinScript);
	if (!pBinScript->LoadBinScript(pFileObject, false))
	{
		SAFE_RELEASE(pBinScript);
		return NULL;
	}

	TraceLn("加载资源脚本 ['%s'] 完成", pFileObject->GetFilePath());
	return static_cast<IBinScript*>(pBinScript);
}
//------------------------------------------------------------------------
// 释放资源管理器(包括由他创建的所有IResScript*, IBinScript*, IResLoader*)
BOOL CResMgr::Release()
{
	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CResMgr::MergeResourceScriptByFileList(LPCSTR szListFileName, LPCSTR szMergeFileName)
{
	// 操作文件列表
	IResScript* pScript_FileList = LoadResScript(szListFileName, TRUE);
	Assert(pScript_FileList);
	IResObject* pRoot_FileList = pScript_FileList->GetResObjectRoot();
	DWORD dwFileCount = pRoot_FileList->GetSubObjectCount();

	// 构造每个脚本文件的IResScript*接口数组
	IResScript** ppScript = new IResScript*[dwFileCount];
	Assert(ppScript);

	// 将其他资源文件合并到第一个资源文件
	for (int i=0; i<dwFileCount; i++)
	{
		IResObject* pObject = NULL;									// 文件列表中的第n个文件对象
		long lIgnoreID = 0;
		pObject = pRoot_FileList->GetSubObjectByIndex(i);
		pObject->GetPropertyLong("ignoreID", lIgnoreID);
		string strFileName = pObject->GetPropertyString("src");

		ppScript[i] = LoadResScript(strFileName.c_str(), lIgnoreID); // 创建第n个资源脚本文件
		IResObject* pRoot = ppScript[i]->GetResObjectRoot();
		DWORD dwObjectCount = pRoot->GetSubObjectCount();
		if (i != 0)
		{
			for (int k=0; k<dwObjectCount; k++)						// 第n个资源脚本文件的Root对象中的子对象一个个添加
			{
				IResObject* obj = pRoot->GetSubObjectByIndex(k);
				ppScript[0]->GetResObjectRoot()->AddSubObject(obj);
				static_cast<CResScript*>(ppScript[0])->AddToHashMap(obj->GetID(), obj);
			}
		}
	}

	// 保存合并后的资源脚本
	ppScript[0]->SaveScript(szMergeFileName);

	// 释放1到n个脚本文件的资源
	for (int j=0; j<dwFileCount; j++)
		SAFE_RELEASE(ppScript[j]);
	delete[] ppScript;

	// 释放操作文件列表的IResScript接口
	SAFE_RELEASE(pScript_FileList);

	return TRUE;
}
//------------------------------------------------------------------------