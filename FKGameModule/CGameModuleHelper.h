/**
*	created:		2012-6-25   23:45
*	filename: 		CGameModuleHelper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
struct IClientGlobal;
//------------------------------------------------------------------------
// GameModule.dll接口版本
#define VERSION_GAME_MODULE 2
//------------------------------------------------------------------------
// 定义
class CGameModuleHelper
{
	typedef BOOL (*ProcCreateGameModule)(DWORD dwVersion, IClientGlobal * pGlobal);
public:	
	CGameModuleHelper(void)
	{
		m_hdll = NULL;
	}

	~CGameModuleHelper(void)
	{
		Close();
	}

	void Close(void)
	{
		if(m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}
	
	BOOL Create(IClientGlobal * pGlobal)
	{
		try
		{
			LoadDLL();
			
			ProcCreateGameModule proc;
			proc = (ProcCreateGameModule)GetProcAddress(m_hdll, "CreateGameModule");
			
			if(proc == NULL)
			{
				throw "Can not GetProAddress('CreateGameModule')";
			}
			
			if(!proc(VERSION_GAME_MODULE, pGlobal))
			{
				throw "CreateGameModule() catch error!";
			}

			return TRUE;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
			MessageBox(NULL, szMsg, "ERROR!! GameModule Create()!", MB_OK);
			return NULL;
		}
		catch(...)
		{
			OutputDebugString("CreateGameModule() catch error\r\n");
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
					   "GameModule Create Error", 
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
			m_hdll = LoadLibrary("FKGameModule_D.dll");
			if(m_hdll == NULL)
			{
				throw "无法加载 FKGameModule_D.dll";
			}
#else
			m_hdll = LoadLibrary("FKGameModule.dll");
			if(m_hdll == NULL)
			{
				throw "无法加载 FKGameModule.dll";
			}
#endif
		}
	}
		
public:
	HINSTANCE		m_hdll;
};
//------------------------------------------------------------------------