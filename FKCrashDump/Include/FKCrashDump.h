/**
*	created:		2012-6-14   18:18
*	filename: 		FKCrashDump
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <windows.h>
#include <dbghelp.h>
#include <tchar.h>
#include <string>
#include <list>
//------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------
#include "../IFKCrashDumpHelper.h"
//------------------------------------------------------------------------
enum BasicType
{
    btNoType		= 0,
    btVoid			= 1,
    btChar			= 2,
    btWChar			= 3,
    btInt			= 6,
    btUInt			= 7,
    btFloat			= 8,
    btBCD			= 9,
    btBool			= 10,
    btLong			= 13,
    btULong			= 14,
    btCurrency		= 25,
    btDate			= 26,
    btVariant		= 27,
    btComplex		= 28,
    btBit			= 29,
    btBSTR			= 30,
    btHresult		= 31
};
//------------------------------------------------------------------------
#define WUNKNOWNSTR		_T("unknown Windows version")
#define W95STR			_T("Windows 95")
#define W95SP1STR		_T("Windows 95 SP1")
#define W95OSR2STR		_T("Windows 95 OSR2")
#define W98STR			_T("Windows 98")
#define W98SP1STR		_T("Windows 98 SP1")
#define W98SESTR		_T("Windows 98 SE")
#define WMESTR			_T("Windows ME")
#define WNT351STR		_T("Windows NT 3.51")
#define WNT4STR			_T("Windows NT 4")
#define W2KSTR			_T("Windows 2000")
#define WXPSTR			_T("Windows XP")
#define W2003SERVERSTR	_T("Windows 2003 Server")
#define WCESTR			_T("Windows CE")
//------------------------------------------------------------------------
#define WUNKNOWN	0
#define W9XFIRST	1
#define W95			1
#define W95SP1		2
#define W95OSR2		3
#define W98			4
#define W98SP1		5
#define W98SE		6
#define WME			7
#define W9XLAST		99
#define WNTFIRST	101
#define WNT351		101
#define WNT4		102
#define W2K			103
#define WXP			104
#define W2003SERVER	105
#define WNTLAST		199
#define WCEFIRST	201
#define WCE			201
#define WCELAST		299
//------------------------------------------------------------------------
class CCrashDump : public ICrashDump
{
public:
	virtual BOOL	AttachCrashDumpSink(ICrashDumpSink *);
	virtual BOOL	DetachCrashDumpSink(ICrashDumpSink *);
	virtual void	SetLogFileName(LPCSTR szLogFileName);
	virtual void	SetDumpFileName(LPCSTR szDumpFileName);
	virtual void	SetDumpType(long lDumpType);
	virtual void	SetDumpOn(int iEnum,BOOL bOn);
	virtual DWORD	GetExceptionCode(PEXCEPTION_POINTERS pExceptionInfo);
	virtual LPCTSTR	GetExceptionDesc(PEXCEPTION_POINTERS pExceptionInfo);
	virtual DWORD	GetCrashFileOffset(PEXCEPTION_POINTERS pExceptionInfo);
	virtual LPCTSTR	GetCrashModuleName(PEXCEPTION_POINTERS pExceptionInfo);
	virtual void	DumpLn(LPCTSTR szMsg);
	void			GetCrashModuleProductVer(PEXCEPTION_POINTERS pExceptionInfo,WORD pwVer[4]);
	string			GetCallStack(CONTEXT& Context,HANDLE hThread);
protected:
    CCrashDump();
public:
	static CCrashDump *Instance()
	{
		static CCrashDump cd;
		return &cd;
	}
    ~CCrashDump();
protected:
	void 			DumpModuleList(PEXCEPTION_POINTERS pExceptionInfo);
	void 			DumpWindowList(PEXCEPTION_POINTERS pExceptionInfo);
	void 			DumpProcessList(PEXCEPTION_POINTERS pExceptionInfo);
	void 			DumpRegister(PEXCEPTION_POINTERS pExceptionInfo);
	void 			DumpCallStack(PCONTEXT,HANDLE);
	void 			DumpBasicInfo(PEXCEPTION_POINTERS pExceptionInfo);
	void 			DumpMiniDump(PEXCEPTION_POINTERS pExceptionInfo);
	void 			DumpLog(PEXCEPTION_POINTERS pExceptionInfo);
private:
	void			DumpMiniDump(PEXCEPTION_POINTERS pExceptionInfo,HANDLE hFile);
	LPCTSTR			GetExceptionDescription(DWORD ExceptionCode);
	LPTSTR			GetFilePart(LPCTSTR);
	LPTSTR			lstrrchr(LPCTSTR,int ch);
	void			DumpModuleInfo(HINSTANCE ModuleHandle);
	void			FormatTime(LPTSTR output, FILETIME TimeToPrint);
	void			WriteStackDetails(PCONTEXT pContext,HANDLE);
	BOOL			GetLogicalAddress(PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset );
	BOOL			IsWindows98();
private:
	static LONG		WINAPI UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
	static BOOL		CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO  pSymInfo,ULONG SymbolSize,PVOID UserContext);
	static bool		FormatSymbolValue(PSYMBOL_INFO pSym,STACKFRAME * sf,char * pszBuffer,unsigned cbBuffer);
	static char *	DumpTypeIndex(char * pszCurrBuffer,DWORD64 modBase,DWORD dwTypeIndex,unsigned nestingLevel,DWORD_PTR offset,bool & bHandled);
	static BasicType GetBasicType( DWORD typeIndex, DWORD64 modBase );
	static char *	FormatOutputValue(   char * pszCurrBuffer,BasicType basicType,DWORD64 length,PVOID pAddress);
	static void 	OnDumpStart();
	static LONG 	OnDumpFinish(PEXCEPTION_POINTERS pExceptionInfo);
	static BOOL		GetWinVer(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild);

	static void		Dump(LPCTSTR szMsg,...);
private:
	static FILE*	m_fpLog;
	static string	m_szRpt;
	BOOL			m_pbOn[enDumpCount];
	long			m_lCrashDumpType;
    TCHAR			m_szLogFileName[MAX_PATH];
	TCHAR			m_szDumpFileName[MAX_PATH];
    LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
public:
	static ICrashDumpSink *	m_pSink;
};
//------------------------------------------------------------------------