/**
*	created:		2012-6-29   14:09
*	filename: 		ITeamClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		组队模块接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#ifdef FKTEAMCLIENT_EXPORTS
#define TEAMCLIENT_API extern "C" __declspec(dllexport)
#else
#define TEAMCLIENT_API extern "C" __declspec(dllimport)
#endif
//------------------------------------------------------------------------
#define TEAMCLIENT_VERSION 1
//------------------------------------------------------------------------
// 组队模块接口
struct ITeamClient
{	
	//-------------------------------------------------------------------
	// 描  述：释放模块的资源 
	//-------------------------------------------------------------------
	virtual void Release(void) = 0;	

	//-------------------------------------------------------------------
	// 描  述：显示组队菜单
	// 参  数：[输入] IActor * pActor - 玩家指针
	// 参  数：[输入] const POINT * pPos - 显示位置
	// 参  数：[输入] BOOL bShow - 是否显示
	// 返回值：void
	//-------------------------------------------------------------------	
	virtual void ShowTeamMenu(IActor * pActor, const POINT * pPos, BOOL bShow) = 0;
	
	//-------------------------------------------------------------------
	// 描  述：检查一个玩家是不是队友
	// 参  数：[输入] DWORD dwPlayerid - 玩家ID
	// 返回值：BOOL - 如果是队友返回TRUE，否则返回FALSE
	//-------------------------------------------------------------------
	virtual BOOL IsTeammate(DWORD dwPlayerid) = 0;
	
	//-------------------------------------------------------------------
	// 描  述：主动离开队伍 
	//-------------------------------------------------------------------
	virtual void LeaveTeam(void) = 0;
	
	//-------------------------------------------------------------------
	// 描  述：获取队伍的所有队员的ID
	// 参  数：[输出] int & nCount - 队员个数
	// 返回值：const DWORD * - 返回ID的数组指针
	//-------------------------------------------------------------------
	virtual const DWORD * GetTeamIds(int & nCount) const = 0;

	//-------------------------------------------------------------------
	// 描  述：获取队长ID
	// 参  数：void
	// 返回值：返回队长的DB ID
	//-------------------------------------------------------------------
	virtual DWORD GetCaptainId(void)  = 0;

	//-------------------------------------------------------------------
	// 描  述：获取组队选项
	// 参  数：void
	// 返回值：TRUE　同意组队，FALSE 拒绝组队
	//-------------------------------------------------------------------
	virtual BOOL  GetTeamOption(void)  = 0;

	//-------------------------------------------------------------------
	// 描  述：设置组队选项
	// 参  数：BOOL bNotRefuse ,TRUE　同意组队，FALSE 拒绝组队
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void  SetTeamOption(BOOL bNotRefuse)  = 0;

	//-------------------------------------------------------------------
	// 描  述：邀请加入队伍
	// 参  数：DWORD dwInviteeID  被邀请人的DB ID
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void  InviteJionTeam(DWORD dwInviteeID)  = 0;

	
	//-------------------------------------------------------------------
	// 描  述：邀请加入队伍
	// 参  数：LPCSTR lpszName  被邀请人的名字
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void  InviteJionTeam(LPCSTR lpszName)  = 0;	

	//-------------------------------------------------------------------
	// 描  述：获得队伍ID
	// 参  数：
	// 返回值：没有队伍的时候返回 0
	//-------------------------------------------------------------------	
	virtual DWORD GetTeamID() = 0;
	
};
//------------------------------------------------------------------------
// 组队模块的助手类
class CTeamClientHelper
{
private:
	typedef BOOL (*ProcCreateTeamClient)(DWORD dwVersion, 
										 IClientGlobal * pGlobal, 
										 ITeamClient ** ppModule); 

private:
	HINSTANCE m_hDll;
	ITeamClient * m_pModule; // 组队模块指针

public:
	CTeamClientHelper(void) : m_hDll(NULL), m_pModule(NULL)
	{		
	}

	~CTeamClientHelper(void)
	{
		Close();
	}
	
	// 创建组队模块
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
			m_hDll = LoadLibrary("FKTeamClient_D.dll");
			if (m_hDll == NULL)
			{
				throw "无法加载 FKTeamClient_D.dll";
			}
#else
			m_hDll = LoadLibrary("FKTeamClient.dll");
			if (m_hDll == NULL)
			{
				throw "无法加载 FKTeamClient.dll";
			}
#endif
			// 获取函数地址
			ProcCreateTeamClient createProc;
			createProc = (ProcCreateTeamClient)GetProcAddress(m_hDll, "CreateTeamClient");
			if(createProc == NULL)
			{
				throw "Can not GetProAddress('CreateTeamClient')";
			}
			
			if(!createProc(TEAMCLIENT_VERSION, pGlobal, &m_pModule))
			{
				throw "CreateTeamClient() catch error!";
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
			OutputDebugString("CreateTeamClient() catch error\r\n");
			return FALSE;
		}
	}

	// 获取模块指针
	ITeamClient * GetModule(void) const
	{
		return m_pModule;
	}

	// 获取模块指针
	ITeamClient * operator ->(void) const
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
//-----------------------------------------------------------------------------
// 描  述：创建组队模块
// 参  数：[输入] DWORD dwVersion - 模块的版本号
// 参  数：[输入] IClientGlobal * pGlobal - 全局管理器的指针
// 参  数：[输出] ITeamClient ** ppModule - 指向模块生成后地址的指针
// 返回值：BOOL - 模块创建成功返回TRUE,否则返回FALSE
//-----------------------------------------------------------------------------
TEAMCLIENT_API BOOL CreateTeamClient(DWORD dwVersion, 
									 IClientGlobal * pGlobal, 
									 ITeamClient ** ppModule);
//------------------------------------------------------------------------