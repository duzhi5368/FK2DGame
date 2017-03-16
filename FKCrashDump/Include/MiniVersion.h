/**
*	created:		2012-6-14   18:42
*	filename: 		MiniVersion
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>
//------------------------------------------------------------------------
class CMiniVersion
{
public:
	CMiniVersion(LPCTSTR lpszPath = NULL);
	BOOL Init();
	void Release();
public:
	BOOL GetFileVersion(WORD *pwVersion);
	BOOL GetProductVersion(WORD* pwVersion);
	BOOL GetFileFlags(DWORD& rdwFlags);
	BOOL GetFileOS(DWORD& rdwOS);
	BOOL GetFileType(DWORD& rdwType);
	BOOL GetFileSubtype(DWORD& rdwType);	
	BOOL GetCompanyName(LPTSTR lpszCompanyName, int nSize);
	BOOL GetFileDescription(LPTSTR lpszFileDescription, int nSize);
	BOOL GetProductName(LPTSTR lpszProductName, int nSize);
protected:
	BOOL GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo);
	BOOL GetStringInfo(LPCTSTR lpszKey, LPTSTR lpszValue);
protected:
	BYTE*		m_pData;
	DWORD		m_dwHandle;
	WORD		m_wFileVersion[4];
	WORD		m_wProductVersion[4];
	DWORD		m_dwFileFlags;
	DWORD		m_dwFileOS;
	DWORD		m_dwFileType;
	DWORD		m_dwFileSubtype;
	TCHAR		m_szPath[MAX_PATH*2];
	TCHAR		m_szCompanyName[MAX_PATH*2];
	TCHAR		m_szProductName[MAX_PATH*2];
	TCHAR		m_szFileDescription[MAX_PATH*2];
};
//------------------------------------------------------------------------