/**
*	created:		2012-6-18   21:35
*	filename: 		BinScript
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../include/ResMgrCommon.h"
#include "../include/BitTool.h"
#include "../include/BinObj.h"
#include "../include/BinScript.h"
//------------------------------------------------------------------------
CBinScript::CBinScript()
{
	m_pFileObject = NULL;
	Empty();
}
//------------------------------------------------------------------------
CBinScript::~CBinScript()
{
}
//------------------------------------------------------------------------
HANDLE CBinScript::GetObject(UINT uID)
{
	// 映射表空
	VerifyR(m_OffsetMap.empty(), NULL, 
		"CBinScript::GetObject() 出现异常。ID map表为空！");

	VerifyR(!m_pFileObject, NULL, 
		"CBinScript::GetObject() 出现异常。文件名为空！");

	if (!m_HashMap.empty())// 曾经创建过!
	{
		hash_map<UINT, CBinObject*>::iterator it = m_HashMap.find(uID);
		if (it != m_HashMap.end())
		{
			return (HANDLE)((*it).second);
		}
	}

	DWORD dwOffset = 0;
	hash_map<UINT, DWORD>::iterator it = m_OffsetMap.find(uID);
	if (it != m_OffsetMap.end())
	{
		dwOffset = (*it).second;
	}
	else
	{
		FKResTraceColorLn(MY_TRACE_COLOR, "错误： CBinScript::GetObject() 出现错误。无法找到指定ID的对象。ID：%d！", uID );
		return NULL;
	}

	CBinObject* pBinObject = new CBinObject;
	Assert(pBinObject);

	// 打开文件对象
	m_pFileObject->Open();
	m_pFileObject->Seek(dwOffset);

	// 读对象信息
	BINOBJECTINFO boi;
	m_pFileObject->Read(&boi, sizeof(BINOBJECTINFO));
	Assert(boi.IsValid());
	pBinObject->SetSize(boi.GetSize());
	pBinObject->SetUnFixPropertyOffset(boi.GetUnFixPropertyOffset());
	
	// 读子对象列表
	DWORD dwObjectCount = boi.GetSubObjectCount();
	if (dwObjectCount)
	{
		UINT* pIDList = new UINT[dwObjectCount];
		Assert(pIDList);
		m_pFileObject->Read(pIDList, dwObjectCount*sizeof(UINT));
		pBinObject->SetIDList(pIDList, dwObjectCount*sizeof(UINT));
		delete[] pIDList;
	}

	// 读不固定属性的长度列表
	DWORD dwUnFixCount = boi.cbUnFixCount;
	if (dwUnFixCount)
	{
		WORD* pUnFixList = new WORD[dwUnFixCount];
		m_pFileObject->Read(pUnFixList, dwUnFixCount*sizeof(WORD));
		pBinObject->SetUnFixSizeList(pUnFixList, dwUnFixCount*sizeof(WORD));
		delete[] pUnFixList;
	}
	
	// 读对象数据缓冲
	DWORD dwRemainSize = pBinObject->GetSize();
	BYTE* pData = new BYTE[dwRemainSize];
	Assert(pData);
	m_pFileObject->Read(pData, dwRemainSize);
	pBinObject->SetObjectBuffer(pData, dwRemainSize);
	delete[] pData;
	
	m_pFileObject->SeekToBegin();
	AddToHashMap(uID, pBinObject);

	return (HANDLE)pBinObject;
}
//------------------------------------------------------------------------
// 释放资源脚本
BOOL CBinScript::Release()
{
	Empty();

	delete this;
	return TRUE;
}
//------------------------------------------------------------------------
BYTE* CBinScript::GetBuffer(HANDLE hBinObject)
{
	Assert(hBinObject);
	return ((CBinObject*)hBinObject)->GetBuffer();
}
//------------------------------------------------------------------------
int CBinScript::GetSize(HANDLE hBinObject)
{
	Assert(hBinObject);
	return ((CBinObject*)hBinObject)->GetSize();
}
//------------------------------------------------------------------------
int CBinScript::GetUnFixPropertyCount(HANDLE hBinObject)
{
	Assert(hBinObject);
	return ((CBinObject*)hBinObject)->GetUnFixPropertyCount();
}
//------------------------------------------------------------------------
int CBinScript::GetUnFixPropertyOffset(HANDLE hBinObject, int nIndexEx)
{
	Assert(hBinObject);
	return ((CBinObject*)hBinObject)->GetUnFixPropertyOffset(nIndexEx);
}
//------------------------------------------------------------------------
int CBinScript::GetUnFixPropertyLength(HANDLE hBinObject, int nIndexEx)
{
	Assert(hBinObject);
	return ((CBinObject*)hBinObject)->GetUnFixPropertyLength(nIndexEx);
}
//------------------------------------------------------------------------
int CBinScript::GetSubObjectCount(HANDLE hBinObject)
{
	Assert(hBinObject);
	return ((CBinObject*)hBinObject)->GetSubObjectCount();
}
//------------------------------------------------------------------------
HANDLE CBinScript::GetSubObject(HANDLE hBinObject, int nIndex)
{
	Assert(hBinObject);
	UINT uID = ((CBinObject*)hBinObject)->GetSubObject(nIndex);
	return GetObject(uID);
}
//------------------------------------------------------------------------
BOOL CBinScript::Delete(UINT uID)
{
	return EraseFromHashMap(uID);
}
//------------------------------------------------------------------------
BOOL CBinScript::AddToHashMap(UINT uID, CBinObject* pBinObject)
{
	Assert(pBinObject);

	m_HashMap[uID] = pBinObject;
	return pBinObject != NULL;
}
//------------------------------------------------------------------------
BOOL CBinScript::EraseFromHashMap(UINT uID)
{
	CBinObject* pBinObject = NULL;
	hash_map<UINT, CBinObject*>::iterator it = m_HashMap.find(uID);
	if (it != m_HashMap.end())
	{
		pBinObject = (*it).second;
		Assert(pBinObject);
		pBinObject->Release();
		m_HashMap.erase(uID);
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
void CBinScript::Empty()
{
	m_dwObjectCount = 0;
	if (m_bReleaseFileObject)
	{
		SAFE_RELEASE(m_pFileObject);
	}

	if (!m_OffsetMap.empty())
		m_OffsetMap.clear();
	if (!m_HashMap.empty())
	{
		hash_map<UINT, CBinObject*>::iterator it;
		for (it=m_HashMap.begin(); it!=m_HashMap.end(); it++)
			((*it).second)->Release();
		m_HashMap.clear();
	}
}
//------------------------------------------------------------------------
BOOL CBinScript::LoadBinScript(IFileObject* pFileObject, bool bReleaseFileObject)
{
	Assert(pFileObject);

	m_pFileObject = pFileObject;
	m_bReleaseFileObject = bReleaseFileObject;

	BINOBJECTHEADER bohd;
	ID_OFFSET* pID_Offset = NULL;

	m_pFileObject->Open();
	m_pFileObject->SeekToBegin();
	m_pFileObject->Read(&bohd, sizeof(BINOBJECTHEADER));
	Assert(bohd.IsValid());

	m_dwObjectCount = bohd.dwObjectCount;
	m_pFileObject->Seek(bohd.dwMapOffset);
	pID_Offset = new ID_OFFSET[bohd.dwObjectCount];
	Assert(pID_Offset);
	m_pFileObject->Read(pID_Offset, bohd.dwObjectCount*sizeof(ID_OFFSET));

	m_pFileObject->SeekToBegin();

	// 构造映射表
	for(int i=0; i<bohd.dwObjectCount; i++)
		m_OffsetMap[pID_Offset[i].id] = pID_Offset[i].offset;

	delete[] pID_Offset;
	return TRUE;
}
//------------------------------------------------------------------------