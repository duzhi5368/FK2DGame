/**
*	created:		2012-6-18   22:17
*	filename: 		ResMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CResMgr : public IResMgr
{
public:
	CResMgr();
	virtual ~CResMgr();

public:
	IResScript* CreateResScript(int nStartID = 0);
	IResScript* LoadResScript(LPCSTR szResFileName, BOOL bIgnoreID = FALSE, BYTE* pKey = NULL); 
	IResScript* LoadResScript(IFileObject* pFileObject, BOOL bIgnoreID = FALSE, BYTE* pKey = NULL); 
	IBinScript* LoadBinScript(LPCSTR szBinFileName);
	IBinScript* LoadBinScript(IFileObject* pFileObject);

	BOOL MergeResourceScriptByFileList(LPCSTR szListFileName, LPCSTR szMergeFileName);
	BOOL Release();
};
//------------------------------------------------------------------------