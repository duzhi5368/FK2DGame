/**
*	created:		2012-6-18   20:12
*	filename: 		FKResManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../IFKResManager.h"
#include "../Include/ResMgrCommon.h"
#include "../Include/StdFileObject.h"
#include "../Include/ResMgr.h"
#include "../Include/ResScript.h"
#include "../Include/BinScript.h"
//------------------------------------------------------------------------
CFKTrace g_DefaultTrace;
//------------------------------------------------------------------------
void* _data_xor(BYTE* buf, int nLen)
{
	int nTotalLen = nLen;
	if (nLen%4 != 0)
		nTotalLen = ((nLen>>2) + 1)<<2;
	BYTE* data = new BYTE[nTotalLen];
	memcpy(data, buf, nLen);
	memset(data + nLen, 0, nTotalLen - nLen);
	DWORD* ptr = (DWORD*)data;
	for (int i=0; i<(nTotalLen>>2); i++)
		*ptr++ ^= 0xffffffff;
	memcpy(buf, data, nLen);
	delete[] data;
	return (void*)buf;
}
//------------------------------------------------------------------------
IFileObject* _create_fileobject(LPCSTR szFileName)
{
	CStdFileObject* pFileObject = new CStdFileObject;
	Assert(pFileObject);
	pFileObject->SetFilePath(szFileName);
	return static_cast<IFileObject*>(pFileObject);
}
//------------------------------------------------------------------------
int _find_string(string& str, string& str1, int nStart/* = 0*/)
{
	return str.find(str1.c_str(), nStart) + 1;
}
//------------------------------------------------------------------------
int _find_string_no_case(string str, string str1, int nStart/* = 0*/)
{
	_make_lower(str);
	_make_lower(str1);
	return str.find(str1.c_str(), nStart) + 1;
}
//------------------------------------------------------------------------
int _find_char(string& str, char c, int nStart/* = 0*/)
{
	return str.find(c, nStart) + 1;
}
//------------------------------------------------------------------------
int _find_char_no_case(string str, char c, int nStart/* = 0*/)
{
	_make_lower(str);
	if ((c >= 'A') && (c <= 'Z'))
		c |= 32;
	return str.find(c, nStart) + 1;
}
//------------------------------------------------------------------------
const char* _make_lower(string& str)
{
	//char* st = str.begin();
	//while (*st != 0)
	//{
	//	char val = *st;
	//	if ((val >= 'A') && (val <='Z')) 
	//		*st =(char)(val | 32);
	//	st ++;
	//}

	transform(str.begin(), str.end(), str.begin(), toupper);
	return str.c_str();
}
//------------------------------------------------------------------------
const char* _trim_left(string& str)
{
	//char* s = str.begin();
	//while(*s == ' ' || *s == '\t' || *s == '\n' || *s == ';')
	//	s++;
	//if (s != str.begin())
	//	str.erase(str.begin(), s);

	str.erase(0, str.find_first_not_of(" \t\n;"));
	return str.c_str();
}
//------------------------------------------------------------------------
const char* _trim_right(string& str)
{
	//char* s = str.end() - 1;
	//while(*s == ' ' || *s == '\t' || *s == '\n' || *s == ';')// || *s == '\0'
	//	s--;
	//if (s != str.end() - 1)
	//	str.erase(s + 1, str.end());

	str.erase(str.find_last_not_of(" \t\n;") + 1);
	return str.c_str();
}
//------------------------------------------------------------------------
const char* _trim(string& str)
{
	_trim_left(str);
	return _trim_right(str);
}
//------------------------------------------------------------------------
const char* _erase_left(string& str, int nSize)
{
	if (nSize < 0)
		nSize = 0;
	if (nSize >= str.size())
		nSize = str.size();
	return (str.erase(0, nSize)).c_str();
}
//------------------------------------------------------------------------
const char* _erase_right(string& str, int nSize)
{
	return _left(str, str.size() - nSize);
}
//------------------------------------------------------------------------
const char* _mid(string& str, int nStart, int nEnd)
{
	return _mid_len(str, nStart, nEnd - nStart + 1);
}
//------------------------------------------------------------------------
const char* _mid_len(string& str, int nStart, int nLen)
{
	_erase_left(str, nStart);
	return (str.erase(nLen, str.size())).c_str();
}
//------------------------------------------------------------------------
const char* _left(string& str, int nSize)
{
	if (nSize < 0)
		nSize = 0;
	if (nSize > str.size())
		nSize = str.size();
	return (str.erase(nSize, str.size())).c_str();
}
//------------------------------------------------------------------------
// 得到某个字符左边的串
const char* _left_c(string& str, char c)
{
	int pos = str.find(c, 0);
	if (pos)
		return _left(str, pos);
	return str.c_str();
}
//------------------------------------------------------------------------
const char* _right(string& str, int nSize)
{
	return _erase_left(str, str.size() - nSize);
}
//------------------------------------------------------------------------
// 得到某个字符右边的串
const char* _right_c(string& str, char c)
{
	int pos = str.find(c, 0);
	if (pos)
		return _erase_left(str, pos + 1);
	return NULL;
}
//------------------------------------------------------------------------
const char* _file_path(string& str)
{
	int pos = str.find_last_of('\\');
	if (pos != -1)
		return _left(str, pos);
	return "";
}
//------------------------------------------------------------------------
const char* _file_name(string& str)
{
	int pos = str.find_last_of('\\');
	if (pos != -1) // 有路径名,去掉
		return _erase_left(str, pos + 1);
	return str.c_str();
}
//------------------------------------------------------------------------
const char* _file_title(string& str)
{
	_file_name(str);
	int pos = str.find_last_of('.');
	if (pos != -1) // 有扩展名,去掉
		_left(str, pos);
	return str.c_str();
}
//------------------------------------------------------------------------
const char* _file_ext(string& str)
{
	int pos = str.find_last_of('.');
	if (pos != -1)
		return _erase_left(str, pos + 1);
	return "";
}
//------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		_CrtDumpMemoryLeaks();
		break;
	}
	return TRUE;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateResMgr(DWORD dwExpectVersion,
												   ITrace *pTrace,
												   IResMgr **ppResMgr)
{
	g_DefaultTrace.Create(pTrace);

	if(dwExpectVersion != VERSION_QUERY_RESOURCE_MANAGER)
	{
		FKResTraceColorLn(MY_TRACE_COLOR,"CreateResMgr() : Version error!");
		return FALSE;
	}

	CResMgr* pResMgr = new CResMgr();
	Assert(pResMgr);
	*ppResMgr = static_cast<IResMgr*>(pResMgr);
	return *ppResMgr != NULL;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateResScript(DWORD dwExpectVersion, LPCSTR szResFileName, BOOL bIgnoreID,
													  ITrace* pTrace, IResScript** ppResScript)
{
	g_DefaultTrace.Create(pTrace);
	if (dwExpectVersion != VERSION_QUERY_RESOURCE_MANAGER)
	{
		FKResTraceColorLn(MY_TRACE_COLOR, "CreateResScript(): Version error!");
		return FALSE;
	}
	CResScript* pResScript = new CResScript;
	Assert(pResScript);
	IFileObject* pFileObject = _create_fileobject(szResFileName);
	Assert(pFileObject);
	if (!pResScript->LoadScript(pFileObject, bIgnoreID, true))
	{
		SAFE_RELEASE(pResScript);
		return FALSE;
	}

	*ppResScript = static_cast<IResScript*>(pResScript);
	TraceLn("CreateResScript()......OK");
	return ppResScript != NULL;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateBinScript(DWORD dwExpectVersion, LPCSTR szBinFileName,
													  ITrace* pTrace, IBinScript** ppBinScript)
{
	g_DefaultTrace.Create(pTrace);
	if (dwExpectVersion != VERSION_QUERY_RESOURCE_MANAGER)
	{
		FKResTraceColorLn(MY_TRACE_COLOR, "CreateBinScript(): Version error!");
		return FALSE;
	}
	CBinScript* pBinScript = new CBinScript;
	Assert(pBinScript);
	IFileObject* pFileObject = _create_fileobject(szBinFileName);
	Assert(pFileObject);
	if (!pBinScript->LoadBinScript(pFileObject, true))
	{
		SAFE_RELEASE(pBinScript);
		return FALSE;
	}

	*ppBinScript = static_cast<IBinScript*>(pBinScript);
	TraceLn("CreateBinScript()......OK");
	return ppBinScript != NULL;
}
//------------------------------------------------------------------------
void DebugPrintLn(LPCSTR lpszFormat, ...)
{	
	va_list args;
	va_start(args, lpszFormat);
	int nBuf;char szBuffer[MAX_PATH];
	nBuf = _vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
	if(nBuf < 0)
	{
		// 字符串太长,切断!
		if (strlen(szBuffer) >= MAX_PATH-1)
			szBuffer[MAX_PATH-1] = 0;
	}
	OutputDebugString(szBuffer);OutputDebugString("\r\n");
	va_end(args);
}
//------------------------------------------------------------------------