/**
*	created:		2012-6-30   23:35
*	filename: 		IScriptUpdate
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		脚本更新模块接口文件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
//------------------------------------------------------------------------
struct IScriptIterator;
//------------------------------------------------------------------------
// 复合变量类型
struct _VARIANT
{
	enum _TYPE_
	{
		enType_EMPTY =  0,
		enType_LONG		 ,	// 支持整型数据
		enType_FLOAT	 ,  // 支持浮点数据
		enType_STRING	 ,  // 支持字符数据
		enType_ITERATOR  ,  // 支持多维脚本
		enType_MAX		 ,
	}m_Type;
	
	union
	{
		long	lValue;		// 可能是整型类型
		double	dValue;		// 可能是浮点类型
		LPCSTR  string;		// 可能是字符串类型
		IScriptIterator * it; // 可能是子脚本 用来实现任意多维的脚本
	}m_Data;
	
};
//------------------------------------------------------------------------
// 脚本迭代器用来实现脚本类型无关性
struct IScriptIterator
{
	virtual int Size() const = NULL;
	
	virtual _VARIANT& GetData(int nIndex) = NULL;
	
	virtual _VARIANT& operator[] (int nIndex) = NULL;
	
	virtual _VARIANT& GetData(LPCSTR szName) = NULL;
	
	virtual _VARIANT& operator [](LPCSTR szName) = NULL;
	
	virtual void Release() = NULL;	// 迭代器由管理器释放!!
};
//------------------------------------------------------------------------
struct IResScript;
//------------------------------------------------------------------------
/*
 *	脚本更新回调接口
 */
struct IScriptUpdateSink
{
	// 脚本载入成功时通知,对于csv脚本，用it读取，对于scp/res脚本，用pResScript读取
	virtual BOOL OnScriptLoad(IScriptIterator *it, IResScript *pResScript, LPCSTR szFile) = NULL;	
	// 脚本动态更新时通知,对于csv脚本，用it读取，对于scp/res脚本，用pResScript读取
	// 不要在OnScriptUpdate中调用ReleaseScript方法,因为可能一个脚本对应多个Sink
	virtual BOOL OnScriptUpdate(IScriptIterator *it, IResScript *pResScript, LPCSTR szFile) = NULL;
};
//------------------------------------------------------------------------
/*
 *	脚本更新管理器
 */
struct IScriptUpdateManager
{
	// 描  述：读取脚本，会自动注册
	// 参  数：[输入] LPCSTR szFile - 脚本文件名
	// 参  数：[输入] IScriptUpdateSink *pSink - 处理脚本的回调接口
	// 返回值：BOOL - 成功返回TRUE，否则为FALSE
	virtual BOOL LoadScript(LPCSTR szFile, IScriptUpdateSink *pSink) = NULL;
	
	// 描  述：更新脚本
	// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
	// 返回值：BOOL - 成功返回TRUE，否则为FALSE
	virtual BOOL UpdateScript(LPCSTR szFile) = NULL;	
	
	// 描  述：注册脚本回调接口
	// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
	// 参  数：[输入] IScriptUpdateSink * pSink - 脚本更新回调接口
	// 返回值：无
	virtual void RegisterSink(LPCSTR szFile, IScriptUpdateSink *pSink) = NULL;
	
	// 描  述：取消注册脚本回调接口
	// 参  数：[输入] LPCSTR szFile - 更新脚本文件名
	// 参  数：[输入] IScriptUpdateSink * pSink - 脚本更新回调接口
	// 返回值：无
	virtual void UnRegisterSink(LPCSTR szFile, IScriptUpdateSink *pSink) = NULL;
	
	// 释放管理器
	virtual void Release() = NULL;
};
//------------------------------------------------------------------------
#pragma warning(disable : 4355)
#ifndef ASSERT
#		define ASSERT(p)	if(!(p)){ __asm{ int 3 }; }
#endif
//------------------------------------------------------------------------
// 使复合类型支持更多操作符，方便使用
class CVariant : protected _VARIANT
{
public:	
	CVariant( char * str ) : m_variant(*((_VARIANT *)this))
	{
		m_variant.m_Type = enType_STRING;
		m_variant.m_Data.string = str;
	}
	
	CVariant( long lValue ) : m_variant(*((_VARIANT *)this))
	{
		m_variant.m_Type = enType_LONG;
		m_variant.m_Data.lValue = lValue;
	}
	
	CVariant( double dValue ) : m_variant(*((_VARIANT *)this))
	{
		m_variant.m_Type = enType_FLOAT;
		m_variant.m_Data.dValue = dValue;
	}
	
	CVariant( IScriptIterator * it ) : m_variant(*((_VARIANT *)this))
	{
		m_variant.m_Type = enType_ITERATOR;
		m_variant.m_Data.it = it;
	}

	CVariant( _VARIANT & value ) : m_variant(value)
	{
	}

	CVariant() : m_variant(*((_VARIANT *)this))
	{
		m_variant.m_Type = enType_EMPTY;
		m_variant.m_Data.dValue = 0;
	}
	
	CVariant operator[](int nIndex)
	{
		ASSERT( m_variant.m_Type == enType_ITERATOR );
		
		return (*m_variant.m_Data.it)[nIndex];
	}

	CVariant operator[](LPCSTR szName)
	{
		ASSERT( m_variant.m_Type == enType_ITERATOR );
		
		return (*m_variant.m_Data.it)[szName];
	}

	int Bound()
	{
		ASSERT( m_variant.m_Type == enType_ITERATOR );
		return m_variant.m_Data.it->Size();
	}

	operator LPCSTR()
	{
		if( m_variant.m_Type != enType_STRING )
		ChangeType( enType_STRING);
		return m_variant.m_Data.string;
	}

	operator long()
	{
		if( m_variant.m_Type != enType_LONG )
		ChangeType( enType_LONG);
		return m_variant.m_Data.lValue;
	}

	operator double()
	{
		if( m_variant.m_Type != enType_FLOAT )
		ChangeType( enType_FLOAT);
		return m_variant.m_Data.dValue;
	}

	typedef void ( CVariant::*CHANGETYPEPOC)();
	void ChangeType( _TYPE_ type )
	{
		static CHANGETYPEPOC	s_change_poc[enType_MAX][enType_MAX] =
		{
			{	NULL,NULL,NULL,NULL,NULL, },
			{	NULL,NULL,&CVariant::ChangeType_long2float ,NULL,NULL },
			{	NULL,&CVariant::ChangeType_float2long,NULL,NULL,NULL  },
			{   NULL,&CVariant::ChangeType_str2long,&CVariant::ChangeType_str2float, NULL, NULL },
			{	NULL,NULL,NULL,NULL,NULL, }
		};

		if( m_variant.m_Type == enType_EMPTY )
			return;
		if( m_variant.m_Type == type )
			return;
		if( s_change_poc[m_variant.m_Type][type] == NULL )
			ASSERT(0);

		(this->*s_change_poc[m_variant.m_Type][type])();
	}

	// -------------------------------------------------------------------
	void ChangeType_long2float()
	{
		m_variant.m_Data.dValue = m_variant.m_Data.lValue;
		m_variant.m_Type = enType_FLOAT;
	}
	
	void ChangeType_float2long()
	{
		m_variant.m_Data.lValue = (long)m_variant.m_Data.dValue;
		m_variant.m_Type = enType_LONG;
	}
	
	void ChangeType_str2long()
	{
		long lTemp = 0;
		sscanf( m_variant.m_Data.string , "%lld" , &lTemp );
		m_variant.m_Data.lValue = lTemp;
		m_variant.m_Type = enType_LONG;
	}
	
	void ChangeType_str2float()
	{
		float dTemp = 0.0;
		sscanf( m_variant.m_Data.string , "%f" , &dTemp );
		m_variant.m_Data.dValue = dTemp;
		m_variant.m_Type = enType_FLOAT;
	}

public:
	_VARIANT	& m_variant;
};
//------------------------------------------------------------------------
#pragma pack()
//------------------------------------------------------------------------
struct IResMgr;
struct ITrace;
//------------------------------------------------------------------------
/*
 *	脚本更新管理器HELPER类
 */
class CScriptUpdateManagerHelper
{
private:
	HINSTANCE m_hdll;
	char m_szErrMsg[128];								//错误信息
public:
	IScriptUpdateManager* m_pManager;
public:
	// 创建类工厂
	typedef BOOL (*ProcCreateScriptUpdateManager)(
		IScriptUpdateManager ** ppManager, IResMgr *pResMgr, ITrace *pTrace);
	
	CScriptUpdateManagerHelper() : m_hdll( NULL ) ,m_pManager(NULL)
	{
		m_szErrMsg[0] = 0;
	}
	
	~CScriptUpdateManagerHelper()
	{
		Close();
	}
	
	IScriptUpdateManager* operator ->()
	{
		return m_pManager;
	}
	
	void Close()
	{
		if (m_pManager!=NULL)
		{
			m_pManager->Release();
			m_pManager = NULL;
		}
		
		if (m_hdll!=NULL)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;			
		}
	}
	
	BOOL Create(IResMgr *pResMgr, ITrace *pTrace)
	{
		Close();
		
		try
		{
			if( !LoadDLL() )
				return FALSE;
			
			ProcCreateScriptUpdateManager proc;
			proc = (ProcCreateScriptUpdateManager)GetProcAddress(m_hdll, "CreateScriptUpdateManager");
			
			if(proc == NULL)
				throw "Can't GetProcAddress('CreateScriptUpdateManager')";
			
			if(!proc(&m_pManager, pResMgr, pTrace))
				throw "CreateScriptUpdateManager() error!";

			return m_pManager != NULL;
		}
		catch (LPCSTR szMsg) 
		{
			lstrcpyn(m_szErrMsg, szMsg, sizeof(m_szErrMsg));
			return FALSE;
		}
		catch(...)
		{
			lstrcpyn(m_szErrMsg, "Unknown Error!", sizeof(m_szErrMsg));
			return FALSE;
		}
	}
	
	BOOL LoadDLL()
	{
		if(m_hdll == NULL)
		{
#ifdef _DEBUG
			m_hdll = LoadLibrary( "FKScriptUpdate_D.dll" );
			if(m_hdll == NULL)
				throw "无法加载 FKScriptUpdate_D.dll";
#else
			m_hdll = LoadLibrary( "FKScriptUpdate.dll" );
			if(m_hdll == NULL)
				throw "无法加载 FKScriptUpdate.dll";
#endif
			return TRUE;
		}
		return FALSE;
	}
	//判断接口指针是否有效
	BOOL IsValid()
	{
		return (m_pManager != NULL);
	}
	char* GetErrMsg()
	{
		return m_szErrMsg;
	}
	IScriptUpdateManager* GetScriptUpdateManager()
	{
		return m_pManager;
	}
	
};
//------------------------------------------------------------------------
