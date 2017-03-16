/**
*	created:		2012-6-30   4:57
*	filename: 		IFriendClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		好友模块接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifdef FRIENDCLIENT_EXPORTS
#define FRIENDCLIENT_API extern "C" __declspec(dllexport)
#else
#define FRIENDCLIENT_API extern "C" __declspec(dllimport)
#endif
//------------------------------------------------------------------------
#define FRIENDCLIENT_VERSION 1
//------------------------------------------------------------------------
// 好友模块接口
struct IFriendClient
{	
	//-------------------------------------------------------------------
	// 描  述：释放模块的资源 
	// 参  数：void	
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void Release(void) = 0;

	//-------------------------------------------------------------------
	// 描  述：显示好友窗口
	// 参  数：[输入] BOOL bShow - 是否显示
	// 参  数：[输入] BOOL bTurn - 是否轮流显示/隐藏
	// 返回值：void 
	//-------------------------------------------------------------------	
	virtual void ShowFriendWnd(BOOL bShow, BOOL bTurn = FALSE) = 0;

	//-------------------------------------------------------------------
	// 描  述：小退的时候调用做清除工作
	// 返回值：void 
	//-------------------------------------------------------------------	
	virtual void Close() = 0;
	
	//-------------------------------------------------------------------
	// 描  述：添加好友功能
	// 参  数：[输入] long lFriendId - 好友DB ID
	// 返回值：void 
	//-------------------------------------------------------------------

	virtual void AddFriend(long lFriendId) = 0;

	//-------------------------------------------------------------------
	// 描  述：添加好友功能
	// 参  数：[输入] LPCSTR lpszName - 好友名字
	// 返回值：void 
	//-------------------------------------------------------------------	
	virtual void AddFriend(LPCSTR lpszName) = 0;
};
//------------------------------------------------------------------------
// 好友模块的助手类
class CFriendClientHelper
{
private:
	typedef BOOL (*ProcCreateFriendClient)(DWORD dwVersion, 
										   IClientGlobal * pGlobal, 
										   IFriendClient ** ppModule); 

private:
	HINSTANCE m_hDll;
	IFriendClient * m_pModule; // 好友模块指针

public:
	CFriendClientHelper(void) : m_hDll(NULL), m_pModule(NULL)
	{		
	}

	~CFriendClientHelper(void)
	{
		Close();
	}
	
	// 创建好友模块
	BOOL Create(IClientGlobal * pGlobal)
	{
		if (m_pModule != NULL)
		{
			return TRUE;
		}
		
		try
		{
			// 载入动态库
#ifdef _DEBUG
			m_hDll = LoadLibrary("FKFriendClient_D.dll");
			if (m_hDll == NULL)
			{
				throw "无法加载 FKFriendClient_D.dll";
			}
#else
			m_hDll = LoadLibrary("FKFriendClient.dll");
			if (m_hDll == NULL)
			{
				throw "无法加载 FKFriendClient.dll";
			}
#endif
			
			// 获取函数地址
			ProcCreateFriendClient createProc;
			createProc = (ProcCreateFriendClient)GetProcAddress(m_hDll, "CreateFriendClient");
			if(createProc == NULL)
			{
				throw "Can not GetProAddress('CreateFriendClient')";
			}
			
			if(!createProc(FRIENDCLIENT_VERSION, pGlobal, &m_pModule))
			{
				throw "CreateFriendClient() catch error!";
			}
			
			return (m_pModule != NULL);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
			return FALSE;
		}
		catch(...)
		{
			OutputDebugString("CreateFriendClient() catch error\r\n");
			return FALSE;
		}
	}

	// 获取模块指针
	IFriendClient * GetModule(void) const
	{
		return m_pModule;
	}

	// 获取模块指针
	IFriendClient * operator ->(void) const
	{
		return m_pModule;
	}

	// 销毁模块
	void Close(void)
	{
		if (m_pModule != NULL)
		{
			m_pModule->Release();
			m_pModule = NULL;
		}

		if (m_hDll != NULL)
		{
			FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}
};
//------------------------------------------------------------------------
// 描  述：创建好友模块
// 参  数：[输入] DWORD dwVersion - 模块的版本号
// 参  数：[输入] IClientGlobal * pGlobal - 全局管理器的指针
// 参  数：[输出] IFriendClient ** ppModule - 指向模块生成后地址的指针
// 返回值：BOOL - 模块创建成功返回TRUE,否则返回FALSE
//-----------------------------------------------------------------------------
FRIENDCLIENT_API BOOL CreateFriendClient(DWORD dwVersion, 
										 IClientGlobal * pGlobal, 
										 IFriendClient ** ppModule);
//------------------------------------------------------------------------