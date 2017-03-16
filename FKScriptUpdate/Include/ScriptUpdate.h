/**
*	created:		2012-6-30   23:57
*	filename: 		ScriptUpdate
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		脚本更新模块接口实现头文件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
#include <list>
#include <map>
#include "../../FKGame/Script/IScriptUpdate.h"
#include "../../FKTrace/IFKTrace.h"
#include "../../FKResManager/IFKResManager.h"
//------------------------------------------------------------------------
using std::list;
using std::map;
//------------------------------------------------------------------------
extern CFKTrace				g_DefaultTrace;
#define g_TraceHelper		g_DefaultTrace	
#ifndef TraceLn
	#ifdef _DEBUG
		#define TraceLn g_DefaultTrace.ThreadSafeDebugPrintLn
		#define TraceColorLn g_DefaultTrace.DebugPrintColorLn
	#else
		#define TraceLn __noop
		#define TraceColorLn __noop
	#endif
#endif
//------------------------------------------------------------------------
extern IResMgr*             g_pResMgr;
//------------------------------------------------------------------------
extern _VARIANT             empty_variant;
//------------------------------------------------------------------------
#define SAFE_RELEASE(p) \
	if ((p) != NULL) \
{ \
	(p)->Release(); \
}
//------------------------------------------------------------------------
struct SCRIPT_INFO
{
	char	szFileName[ MAX_PATH ];		
	list< IScriptUpdateSink * >	sinkList;	// 一个脚本可以支持多个回调处理	
};
//------------------------------------------------------------------------
/*
 *	脚本更新管理器具体实现类
 */
class ScriptUpdateManager : public IScriptUpdateManager
{
public:
	// 描  述：读取脚本，会自动注册
	// 参  数：[输入] LPCSTR szFile - 脚本文件名
	// 参  数：[输入] IScriptUpdateSink *pSink - 处理脚本的回调接口
	// 返回值：BOOL - 成功返回TRUE，否则为FALSE
	virtual BOOL LoadScript(LPCSTR szFile, IScriptUpdateSink *pSink);
	
	// 描  述：更新脚本
	// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
	// 返回值：BOOL - 成功返回TRUE，否则为FALSE
	virtual BOOL UpdateScript(LPCSTR szFile);	
	
	// 描  述：注册脚本回调接口
	// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
	// 参  数：[输入] IScriptUpdateSink * pSink - 脚本更新回调接口
	// 返回值：无
	virtual void RegisterSink(LPCSTR szFile, IScriptUpdateSink *pSink);
	
	// 描  述：取消注册脚本回调接口
	// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
	// 参  数：[输入] IScriptUpdateSink * pSink - 脚本更新回调接口
	// 返回值：无
	virtual void UnRegisterSink(LPCSTR szFile, IScriptUpdateSink *pSink);
	
	// 释放管理器
	virtual void Release();
public:
	virtual ~ScriptUpdateManager();
	static ScriptUpdateManager* GetInstance();
private:
	IScriptIterator* GetIteratorFromCSV(LPCSTR szFileName);
	IScriptIterator* GetIteratorFromCSE(LPCSTR szFileName);
	BOOL Load(LPCSTR szFileName, IScriptIterator **ppScriptIterator, IResScript **ppResScript);
private:	
	ScriptUpdateManager();	
	// 描  述：获得绝对路径
	// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
	// 参  数：[输入] char *szPath  - Buf
	// 参  数：[输入] int nSize		- Buf大小
	// 返回值：Buf
	char* GetAbsolutePath(LPCSTR szFile, char *szPath, int nSize);
private:
	static ScriptUpdateManager* s_pInstance;
	static int s_nRefCount;
	typedef map< int , SCRIPT_INFO >	SCRIPT_TABLE;
	
	SCRIPT_TABLE	  m_ScriptTable;
	int m_nCallNum;
};
//------------------------------------------------------------------------




