/**
*	created:		2012-7-2   23:04
*	filename: 		ThingClientHelper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		ThingClient.dll加载Helper
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IThingFactory.h"
//------------------------------------------------------------------------
// thingclient.dll接口版本
#define VERSION_THING_FACTORY 2
//------------------------------------------------------------------------
// 定义
class CClientTingFactoryHelper
{
	typedef BOOL (*ProcCreateClientClassFactory)(DWORD dwVersion,
												 IClientGlobal * pGlobal,
												 IThingFactory ** pThingFactory);
public:	
	CClientTingFactoryHelper(void)
	{
		m_hdll = NULL;
		m_pTCF = NULL;
	}
	
	~CClientTingFactoryHelper(void)
	{
		Close();
	}

	void Close(void)
	{
		if(m_pTCF != NULL)
		{
			m_pTCF->Release();
			m_pTCF = NULL;
		}

		if(m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}
	
	IThingFactory * operator ->(void)
	{
		return m_pTCF;	
	}
	
	BOOL Create(IClientGlobal * pGlobal)
	{
		try
		{
			LoadDLL();
			
			ProcCreateClientClassFactory proc;
			proc = (ProcCreateClientClassFactory)GetProcAddress(m_hdll, "CreateClientThingFactory");
			
			if(proc == NULL)
			{
				throw "Can not GetProAddress('CreateClientThingFactory')";
			}
			
			if(!proc(VERSION_THING_FACTORY, pGlobal, &m_pTCF))
			{
				throw "CreateClientThingFactory() catch error!";
			}

			return m_pTCF != NULL;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
			MessageBox(NULL, szMsg, "ERROR!! ThingClassFactory Create()!", MB_OK);
			return NULL;
		}
		catch(...)
		{
			OutputDebugString("CreateClientThingFactory() catch error\r\n");
			LPVOID lpMsgBuf;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
						  FORMAT_MESSAGE_FROM_SYSTEM |
						  FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL,
						  GetLastError(),
						  0, // Default language
						  (LPTSTR)&lpMsgBuf,
						  0,
						  NULL);

			MessageBox(NULL,
					   (LPCTSTR)lpMsgBuf,
					   "ThingClassFactory Create Error", 
					   MB_OK | MB_ICONINFORMATION);

			// Free the buffer.
			LocalFree(lpMsgBuf);

			return NULL;
		}
	}

	void LoadDLL(void)
	{
		if(m_hdll == NULL)
		{
#ifdef _DEBUG
			m_hdll = LoadLibrary("FKThingClient_D.dll");
			if(m_hdll == NULL)
			{
				throw "无法加载 FKThingClient_D.dll";
			}
#else
			m_hdll = LoadLibrary("FKThingClient.dll");
			if(m_hdll == NULL)
			{
				throw "无法加载 FKThingClient.dll";
			}
#endif
		}
	}
		
public:
	HINSTANCE		m_hdll;
	IThingFactory * m_pTCF;
};
//------------------------------------------------------------------------