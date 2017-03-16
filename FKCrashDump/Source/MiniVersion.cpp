/**
*	created:		2012-6-14   18:44
*	filename: 		MiniVersion
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../include/MiniVersion.h"
//------------------------------------------------------------------------
#pragma message("automatic link to VERSION.LIB")
#pragma comment(lib, "version.lib")
//------------------------------------------------------------------------
CMiniVersion::CMiniVersion(LPCTSTR lpszPath)
{
	ZeroMemory(m_szPath, sizeof(m_szPath));

	if (lpszPath && lpszPath[0] != 0)
	{
		lstrcpyn(m_szPath, lpszPath, sizeof(m_szPath)-1);
	}
	else
	{
	}

	m_pData = NULL;
	m_dwHandle = 0;

	for (int i = 0; i < 4; i++)
	{
		m_wFileVersion[i] = 0;
		m_wProductVersion[i] = 0;
	}

	m_dwFileFlags = 0;
	m_dwFileOS = 0;
	m_dwFileType = 0;
	m_dwFileSubtype = 0;

	ZeroMemory(m_szCompanyName, sizeof(m_szCompanyName));
	ZeroMemory(m_szProductName, sizeof(m_szProductName));
	ZeroMemory(m_szFileDescription, sizeof(m_szFileDescription));

	Init();
}
//------------------------------------------------------------------------
BOOL CMiniVersion::Init()
{
	DWORD dwHandle;
	DWORD dwSize;
	BOOL rc;

	dwSize = ::GetFileVersionInfoSize((LPSTR)(LPCTSTR)m_szPath, &dwHandle);
	if (dwSize == 0)
		return FALSE;

	m_pData = new BYTE [dwSize + 1];	
	ZeroMemory(m_pData, dwSize+1);

	rc = ::GetFileVersionInfo((LPSTR)(LPCTSTR)m_szPath, dwHandle, dwSize, m_pData);
	if (!rc)
		return FALSE;

	VS_FIXEDFILEINFO FixedInfo;

	if (GetFixedInfo(FixedInfo))
	{
		m_wFileVersion[0] = HIWORD(FixedInfo.dwFileVersionMS);
		m_wFileVersion[1] = LOWORD(FixedInfo.dwFileVersionMS);
		m_wFileVersion[2] = HIWORD(FixedInfo.dwFileVersionLS);
		m_wFileVersion[3] = LOWORD(FixedInfo.dwFileVersionLS);

		m_wProductVersion[0] = HIWORD(FixedInfo.dwProductVersionMS);
		m_wProductVersion[1] = LOWORD(FixedInfo.dwProductVersionMS);
		m_wProductVersion[2] = HIWORD(FixedInfo.dwProductVersionLS);
		m_wProductVersion[3] = LOWORD(FixedInfo.dwProductVersionLS);

		m_dwFileFlags   = FixedInfo.dwFileFlags;
		m_dwFileOS      = FixedInfo.dwFileOS;
		m_dwFileType    = FixedInfo.dwFileType;
		m_dwFileSubtype = FixedInfo.dwFileSubtype;
	}
	else
		return FALSE;

	GetStringInfo(_T("CompanyName"),     m_szCompanyName);
	GetStringInfo(_T("FileDescription"), m_szFileDescription);
	GetStringInfo(_T("ProductName"),     m_szProductName);

	return TRUE;		
}
//------------------------------------------------------------------------
void CMiniVersion::Release()
{
	if (m_pData)
		delete [] m_pData;
	m_pData = NULL;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetFileVersion(WORD * pwVersion)
{
	for (int i = 0; i < 4; i++)
		*pwVersion++ = m_wFileVersion[i];
	return TRUE;
}					 	 
//------------------------------------------------------------------------
BOOL CMiniVersion::GetProductVersion(WORD * pwVersion)
{
	for (int i = 0; i < 4; i++)
		*pwVersion++ = m_wProductVersion[i];
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetFileFlags(DWORD& rdwFlags)
{
	rdwFlags = m_dwFileFlags;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetFileOS(DWORD& rdwOS)
{
	rdwOS = m_dwFileOS;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetFileType(DWORD& rdwType)
{
	rdwType = m_dwFileType;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetFileSubtype(DWORD& rdwType)
{
	rdwType = m_dwFileSubtype;
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetCompanyName(LPTSTR lpszCompanyName, int nSize)
{
	if (!lpszCompanyName)
		return FALSE;
	ZeroMemory(lpszCompanyName, nSize);
	lstrcpyn(lpszCompanyName, m_szCompanyName, nSize-1);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetFileDescription(LPTSTR lpszFileDescription, int nSize)
{
	if (!lpszFileDescription)
		return FALSE;
	ZeroMemory(lpszFileDescription, nSize);
	lstrcpyn(lpszFileDescription, m_szFileDescription, nSize-1);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetProductName(LPTSTR lpszProductName, int nSize)
{
	if (!lpszProductName)
		return FALSE;
	ZeroMemory(lpszProductName, nSize);
	lstrcpyn(lpszProductName, m_szProductName, nSize-1);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo)
{
	BOOL rc;
	UINT nLength;
	VS_FIXEDFILEINFO *pFixedInfo = NULL;

	if (!m_pData)
		return FALSE;

	if (m_pData)
		rc = ::VerQueryValue(m_pData, _T("\\"), (void **) &pFixedInfo, &nLength);
	else
		rc = FALSE;

	if (rc)
		memcpy (&rFixedInfo, pFixedInfo, sizeof (VS_FIXEDFILEINFO));	

	return rc;
}
//------------------------------------------------------------------------
BOOL CMiniVersion::GetStringInfo(LPCSTR lpszKey, LPTSTR lpszReturnValue)
{
	BOOL rc;
	DWORD *pdwTranslation;
	UINT nLength;
	LPTSTR lpszValue;

	if (m_pData == NULL)
		return FALSE;

	if (!lpszReturnValue)
		return FALSE;

	if (!lpszKey)
		return FALSE;

	*lpszReturnValue = 0;

	rc = ::VerQueryValue(m_pData, _T("\\VarFileInfo\\Translation"), 
		(void**) &pdwTranslation, &nLength);
	if (!rc)
		return FALSE;

	char szKey[2000];
	wsprintf(szKey, _T("\\StringFileInfo\\%04x%04x\\%s"),
		LOWORD (*pdwTranslation), HIWORD (*pdwTranslation),
		lpszKey);

	rc = ::VerQueryValue(m_pData, szKey, (void**) &lpszValue, &nLength);

	if (!rc)
		return FALSE;

	lstrcpy(lpszReturnValue, lpszValue);

	return TRUE;
}
//------------------------------------------------------------------------