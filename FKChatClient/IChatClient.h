/**
*	created:		2012-6-28   0:46
*	filename: 		IChatClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		聊天模块的接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
struct IPaoPao;
struct IClientGlobal;
struct ICanvas;
//------------------------------------------------------------------------
typedef __int64 UID;
//------------------------------------------------------------------------
// 聊天模块接口
struct IChatClient
{
	//-------------------------------------------------------------------
	// 描  述：用户发言 
	// 参  数：[输入] int nChannel - 聊天频道
	// 参  数：[输入] const char * pszReceiver - 信息的接收者
	// 参  数：[输入] const char * pszText - 发言的文本
	// 返回值：BOOL - 想要说的话如果有效且可以发送返回TRUE，否则返回FALSE
	//-------------------------------------------------------------------
	virtual	BOOL Speak(int nChannel, const char * pszReceiver, const char * pszText) = 0;

	//-------------------------------------------------------------------
	// 描  述： 设置聊天频道的开闭
	// 参  数：[输入] int nChannel - 聊天频道
	// 参  数：[输入] BOOL bOnOff - 是否打开
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void ChannelOn(int nChannel, BOOL bOnOff) = 0;

	//-------------------------------------------------------------------
	// 描  述：设定是否显示聊天信息窗口 
	// 参  数：[输入] int nType - 显示方式
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void ShowWindow(int nType) = 0;

	//-------------------------------------------------------------------
	// 描  述：设置信息显示窗口的透明度 
	// 参  数：[输入] int nLevel - 透明度
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void SetWindowTrans(int nLevel) = 0;	

	//-------------------------------------------------------------------
	// 描  述：释放聊天模块使用的资源
	// 参  数：void
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void Release(void) = 0;
	
	//-------------------------------------------------------------------
	// 描  述：显示系统频道信息
	// 参  数：const char * pszText - 提示文本
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void DisplaySysMessage(const char * pszText) = 0;
	
	//-------------------------------------------------------------------
	// 描  述：创建一个泡泡 
	// 参  数：UID uid - 创建泡泡的生物UID
	// 返回值：void
	//-------------------------------------------------------------------
	virtual IPaoPao * CreatePaoPao(UID uid) = 0;

	//-------------------------------------------------------------------
	// 描  述：提示器画图
	//-------------------------------------------------------------------
	virtual void OnSysMsgDraw(ICanvas * pCanvas) = 0;

	//-------------------------------------------------------------------
	// 描  述：显示浮动提示信息
	// 参  数：int nType - 显示的类型（确定显示的位置）(SysMessageDef.h)
	// 参  数：const char * lpszText - 显示的文本
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void DisplayFloatMessage(int nType, const char * lpszText) = 0;

	//-------------------------------------------------------------------
	// 描  述：清空浮动提示信息
	// 参  数：int nType - 显示的类型（确定显示的位置）(SysMessageDef.h),-1为所有的的
	// 返回值：void
	//-------------------------------------------------------------------
	virtual void ClearFloatMessage(int nType) = 0;	

	//-------------------------------------------------------------------
	// 显示称号窗口
	//-------------------------------------------------------------------	
	virtual void ShowTitleWnd(BOOL bTrue,BOOL bTurn = TRUE) = 0;
};
//------------------------------------------------------------------------
#ifdef FKCHATCLIENT_EXPORTS
#define CHATCLIENT_API extern "C" __declspec(dllexport)
#else
#define CHATCLIENT_API extern "C" __declspec(dllimport)
#endif
//------------------------------------------------------------------------
#define CHATCLIENT_VERSION 1	// 聊天模块的版本号
//------------------------------------------------------------------------
// 描  述：创建聊天模块
// 参  数：[输入] DWORD dwVersion - 聊天模块的版本号
// 参  数：[输入] IClientGlobal * pGlobal - 全局管理器的指针
// 参  数：[输出] IChatClient ** ppModule - 聊天模块生成后地址的指针
// 返回值：BOOL - 模块创建成功返回TRUE,否则返回FALSE
//-------------------------------------------------------------------
CHATCLIENT_API BOOL CreateChatClient(DWORD dwVersion, IClientGlobal * pGlobal, IChatClient ** ppModule);
//------------------------------------------------------------------------
// 聊天模块的助手
class CChatClientHelper  
{
public:
	typedef BOOL (*ProcCreateChatClient)(DWORD dwVersion, 
		IClientGlobal * pGlobal, IChatClient ** ppModule); 
	
public:
	CChatClientHelper(void)
	{
	}
	virtual ~CChatClientHelper(void)
	{
		Close();
	}
	
public:
	// 创建模块
	BOOL Create(IClientGlobal * pGlobal)
	{
		try
		{	
			Close();
			
			// 载入动态库
#ifdef _DEBUG
			m_hDll = LoadLibrary("FKChatClient_D.dll");
			if (m_hDll == NULL)
			{
				throw "无法加载 FKChatClient_D.dll";
			}
#else
			m_hDll = LoadLibrary("FKChatClient.dll");
			if (m_hDll == NULL)
			{
				throw "无法加载 FKChatClient.dll";
			}
#endif
			
			// 获取函数地址
			ProcCreateChatClient createProc;
			createProc = (ProcCreateChatClient)GetProcAddress(m_hDll, "CreateChatClient");
			if(createProc == NULL)
			{
				throw "Can not GetProAddress('CreateChatClient')";
			}
			
			// 创建模块
			if(!createProc(CHATCLIENT_VERSION, pGlobal, &m_pModule))
			{
				throw "CreateChatClient() catch error!";
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
			OutputDebugString("CreateChatClient() catch error\r\n");
			return FALSE;
		}
	}
	
	// 获取模块
	IChatClient * operator ->(void) const
	{
		return m_pModule;
	}

	IChatClient * GetChatClient(void) const
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
	
private:
	HINSTANCE m_hDll;
	IChatClient * m_pModule; // 聊天模块指针
};
//------------------------------------------------------------------------
