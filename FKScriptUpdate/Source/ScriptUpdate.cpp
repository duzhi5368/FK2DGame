/**
*	created:		2012-6-30   23:59
*	filename: 		ScriptUpdate
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/ScriptUpdate.h"
#include "../Include/ScriptAdapter_csv.h"
#include "../Include/ScriptAdapter_cse.h"
#include "../Include/HashFunction.h"
//------------------------------------------------------------------------
ScriptUpdateManager*		ScriptUpdateManager::s_pInstance=NULL;
int							ScriptUpdateManager::s_nRefCount=0;
CFKTrace					g_DefaultTrace;	
IResMgr*                    g_pResMgr = NULL;
_VARIANT                    empty_variant = { _VARIANT::enType_EMPTY, 0 };
//------------------------------------------------------------------------
ScriptUpdateManager::ScriptUpdateManager()
{
	m_nCallNum = 0;
}
//------------------------------------------------------------------------
ScriptUpdateManager::~ScriptUpdateManager()
{

}
//------------------------------------------------------------------------
//接口函数
// 描  述：读取脚本，会自动注册
// 参  数：[输入] LPCSTR szFile - 脚本文件名
// 参  数：[输入] IScriptUpdateSink *pSink - 处理脚本的回调接口
// 返回值：BOOL - 成功返回TRUE，否则为FALSE
BOOL ScriptUpdateManager::LoadScript(LPCSTR szFile, IScriptUpdateSink *pSink)
{
	// 注意为了提高效率和避免太多麻烦，这里不能被嵌套调用，也就是在回调中再装载
	// 脚本，如果要处理请参考事件系统
	if (m_nCallNum  > 0)
	{
		memcpy(NULL, NULL, -1);
	}

	if (g_pResMgr == NULL || szFile == NULL || pSink == NULL)
	{
		return FALSE;
	}

	//注册
	RegisterSink(szFile, pSink);	

	// 转换成绝对路径
	char szPath[MAX_PATH] = {0};
	GetAbsolutePath(szFile, szPath, MAX_PATH);

	unsigned int unHashKey = SDBMHash( szPath );
	SCRIPT_TABLE::iterator it = m_ScriptTable.find( unHashKey );
	if (it == m_ScriptTable.end())
	{
		return FALSE;
	}

	IScriptIterator *pSriptIterator = NULL;
	IResScript *pResSript = NULL;
	if (!Load(szFile, &pSriptIterator, &pResSript))
	{
		return FALSE;
	}

	m_nCallNum ++;

	//通知脚本LOAD
	SCRIPT_INFO &info = (*it).second;
	BOOL bSucceed = TRUE;
	list< IScriptUpdateSink* >::iterator itList = info.sinkList.begin();
	for(; itList != info.sinkList.end(); ++itList)
	{
		IScriptUpdateSink *pSink = *itList;
		if (pSink != NULL)
		{
			if (!pSink->OnScriptLoad(pSriptIterator, pResSript, szFile))
			{				
				bSucceed = FALSE;
				TraceLn("脚本文件%s更新失败", szPath);
				break;
			}			
		}
	}	

	if (pResSript != NULL)
	{
		pResSript->Release();
		pResSript = NULL;
	}

	if (pSriptIterator != NULL)
	{
		pSriptIterator->Release();
		pSriptIterator = NULL;
	}	

	m_nCallNum --;

	return bSucceed;
}
//------------------------------------------------------------------------
// 描  述：更新脚本
// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
// 返回值：BOOL - 成功返回TRUE，否则为FALSE
BOOL ScriptUpdateManager::UpdateScript(LPCSTR szFile) 
{
	if (szFile == NULL)
	{
		return FALSE;
	}

	// 转换成绝对路径
	char szPath[MAX_PATH] = {0};
	GetAbsolutePath(szFile, szPath, MAX_PATH);

	unsigned int unHashKey = SDBMHash( szPath );
	SCRIPT_TABLE::iterator it = m_ScriptTable.find(unHashKey);
	if (it == m_ScriptTable.end())
	{
		return FALSE;
	}

	IScriptIterator *pSriptIterator = NULL;
	IResScript *pResSript = NULL;
	if (!Load(szFile, &pSriptIterator, &pResSript))
	{
		return FALSE;
	}

	SCRIPT_INFO &info = (*it).second;	
	// 开始更新脚本
	BOOL  bSucceed = TRUE;
	list< IScriptUpdateSink* >::iterator itList = info.sinkList.begin();
	for(; itList != info.sinkList.end(); ++itList)
	{
		IScriptUpdateSink *pSink = *itList;
		if (pSink != NULL)
		{
			if (!pSink->OnScriptUpdate(pSriptIterator, pResSript, szFile))
			{				
				bSucceed = FALSE;
				TraceLn("脚本文件%s更新失败", szPath);
				break;
			}			
		}
	}	
	if (pResSript != NULL)
	{
		pResSript->Release();
		pResSript = NULL;
	}

	if (pSriptIterator != NULL)
	{
		pSriptIterator->Release();
		pSriptIterator = NULL;
	}	

	return bSucceed;
}
//------------------------------------------------------------------------
// 描  述：注册脚本回调接口
// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
// 参  数：[输入] IScriptUpdateSink * pSink - 脚本更新回调接口
// 返回值：无
void ScriptUpdateManager::RegisterSink(LPCSTR szFile, IScriptUpdateSink* pSink)
{
	// 转换成绝对路径
	char szPath[MAX_PATH]={0};
	GetAbsolutePath(szFile, szPath, MAX_PATH);

	unsigned int unHashKey = SDBMHash( szPath );
	SCRIPT_TABLE::iterator it = m_ScriptTable.find(unHashKey);
	if (it != m_ScriptTable.end())
	{
		//已经存在该文件
		SCRIPT_INFO &info = (*it).second;
		info.sinkList.push_back(pSink);//添加新的接收器
		info.sinkList.unique();//保持唯一		
	}
	else
	{
		//不存在该文件，则添加新的
		SCRIPT_INFO info;
		lstrcpyn(info.szFileName, szPath, MAX_PATH);		
		info.sinkList.push_back(pSink);
		m_ScriptTable.insert( std::make_pair( unHashKey, info ) );
	}	
}
//------------------------------------------------------------------------
// 描  述：取消注册脚本回调接口
// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
// 参  数：[输入] IScriptUpdateSink * pSink - 脚本更新回调接口
// 返回值：无
void ScriptUpdateManager::UnRegisterSink(LPCSTR szFile, IScriptUpdateSink * pSink)
{
	// 转换成绝对路径
	char szPath[MAX_PATH] = {0};	
	GetAbsolutePath(szFile, szPath, MAX_PATH);

	unsigned int unHashKey = SDBMHash( szPath );
	SCRIPT_TABLE::iterator it = m_ScriptTable.find(unHashKey);
	if (it != m_ScriptTable.end())
	{
		SCRIPT_INFO & info = (*it).second;
		info.sinkList.remove(pSink);
	}
}
//------------------------------------------------------------------------
// 释放管理器
void ScriptUpdateManager::Release() 
{
	s_nRefCount--;
	if (s_nRefCount <= 0)
	{
		m_ScriptTable.empty();
		delete this;
	}
}
//------------------------------------------------------------------------
ScriptUpdateManager* ScriptUpdateManager::GetInstance()
{
	if (s_pInstance == NULL)
	{
		s_pInstance = new ScriptUpdateManager;
	}

	s_nRefCount++;
	return s_pInstance;
}
//------------------------------------------------------------------------
IScriptIterator* ScriptUpdateManager::GetIteratorFromCSV(LPCSTR szFileName)
{	
	CScriptAdapter_csv *pScriptAdapter = new CScriptAdapter_csv();
	if(!pScriptAdapter->LoadScript(szFileName))
	{
		return NULL;
	}
	return pScriptAdapter->GetScriptIterator();	
}
//------------------------------------------------------------------------
IScriptIterator* ScriptUpdateManager::GetIteratorFromCSE(LPCSTR szFileName)
{	
	CScriptAdapter_cse* pScriptAdapter = new CScriptAdapter_cse();
	if(!pScriptAdapter->LoadScript(szFileName))
	{
		return NULL;
	}

	return pScriptAdapter->GetScriptIterator();	
}
//------------------------------------------------------------------------
BOOL ScriptUpdateManager::Load(LPCSTR szFileName, 
							   IScriptIterator **ppScriptIterator, IResScript **ppResScript)
{
	int nLen = strlen(szFileName);
	if( nLen < 3 )
	{
		return FALSE;
	}

	LPCSTR pExternName = szFileName + nLen - 3;
	if(stricmp( pExternName , "SCP" ) == 0)
	{		
		*ppResScript = g_pResMgr->LoadResScript(szFileName);
		return (*ppResScript != NULL) ? TRUE : FALSE;
	}
	else if (stricmp( pExternName , "RES" ) == 0)
	{
		BYTE Key[] = "abcd";
		*ppResScript = g_pResMgr->LoadResScript(szFileName, FALSE, Key);
		return (*ppResScript != NULL) ? TRUE : FALSE;
	}
	else if (stricmp( pExternName , "CSV" ) == 0)
	{		
		*ppScriptIterator = GetIteratorFromCSV(szFileName);
		return (*ppScriptIterator != NULL) ? TRUE : FALSE;
	}
	else if (stricmp(pExternName, "CSE") == 0)
	{
		// add by wangzq
		*ppScriptIterator = GetIteratorFromCSE(szFileName);
		return (*ppScriptIterator != NULL) ? TRUE : FALSE;
	}
	return FALSE;
}
//------------------------------------------------------------------------
// 转换成绝对路径	
char* ScriptUpdateManager::GetAbsolutePath(LPCSTR szFile, char *szPath, int nSize)
{
	if (szFile[1] != ':')
	{
		//相对路径
		GetCurrentDirectory(nSize, szPath);
		strcat(szPath, "\\");
		strcat(szPath, szFile);		
	}
	else
	{
		//绝对路径
		lstrcpyn(szPath, szFile, nSize);
	}

	strupr(szPath);//将其中的小写字母转换成大写

	return szPath;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) BOOL CreateScriptUpdateManager(
	IScriptUpdateManager **ppManager, IResMgr *pResMgr, ITrace *pTrace)
{	
	if (ppManager == NULL || pResMgr == NULL)
	{
		return FALSE;
	}

	g_pResMgr = pResMgr;

	if (!g_DefaultTrace.Create(pTrace))
	{
		return FALSE;
	}	

	*ppManager = ScriptUpdateManager::GetInstance();
	return *ppManager != NULL;
}
//------------------------------------------------------------------------