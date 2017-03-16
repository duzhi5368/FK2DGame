/**
*	created:		2012-7-1   10:16
*	filename: 		CBasicSysHelper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		地图对象创建的类工厂接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IItemClassFactory.h"
//------------------------------------------------------------------------
struct IClientGlobal;
//------------------------------------------------------------------------
#define VERSION_QUERY_ITEMCLASSFACTORY 3
//------------------------------------------------------------------------
class CBasicSysHelper
{
private:
	HINSTANCE m_hdll;
public:
	IItemClassFactory* m_pICF;
public:
	// 创建类工厂
	typedef BOOL (*ProcCreateItemClassFactory)(DWORD dwExpectVersion, IItemClassFactory** ppItemClassFactory, IClientGlobal* pGlobal);	
	
	CBasicSysHelper()
	{
		m_hdll = NULL;
		m_pICF = NULL;
	}
	
	~CBasicSysHelper()
	{
		Close();
	}

	void Close()
	{
		if (m_pICF)
		{
			m_pICF->Release();
			m_pICF = NULL;
		}
		
		if (m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}	
	}

	IItemClassFactory* operator ->()
	{
		return m_pICF;
	}
	
	BOOL CreateItemClassFactory(IClientGlobal* pGlobal)
	{
		try
		{
			Close();
			
			LoadDLL();
			
			ProcCreateItemClassFactory proc;
			proc = (ProcCreateItemClassFactory)GetProcAddress(m_hdll, "CreateItemClassFactory");
			
			if(proc == NULL)
				throw "Can not GetProAddress('CreateItemClassFactory')";
			
			if(!proc(VERSION_QUERY_ITEMCLASSFACTORY, &m_pICF, pGlobal))
				throw "CreateItemClassFactory() catch error!";
			return m_pICF != NULL;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);OutputDebugString("\r\n");
			return NULL;
		}
		catch(...)
		{
			OutputDebugString("CreateItemClassFactory() catch error\r\n");
			return NULL;
		}
	}
	
	// 加载BasicSys.dll
	void LoadDLL()
	{
		if(m_hdll == NULL)
		{
#ifdef _DEBUG
		m_hdll = LoadLibrary("FKBasicSys_D.dll");
		if(m_hdll == NULL)
			throw "无法加载 FKBasicSys_D.dll";
#else
		m_hdll = LoadLibrary("FKBasicSys.dll");
		if(m_hdll == NULL)
			throw "无法加载 FKBasicSys.dll";
#endif
		}
	}
};
//------------------------------------------------------------------------