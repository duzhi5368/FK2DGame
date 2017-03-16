/**
*	created:		2012-6-30   6:02
*	filename: 		CreateChatClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		实现创建聊天模块
*/
//------------------------------------------------------------------------
#include "../IChatClient.h"
#include "../../FKTrace/IFKTrace.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../Include/ChatClient.h"
//------------------------------------------------------------------------
// 描  述：创建聊天模块
// 参  数：[输入] DWORD dwVersion - 聊天模块的版本号
// 参  数：[输入] IClientGlobal * pGlobal - 全局管理器的指针
// 参  数：[输出] IChatClient ** ppModule - 聊天模块生成后地址的指针
// 返回值：BOOL - 模块创建成功返回TRUE,否则返回FALSE
//-------------------------------------------------------------------
CHATCLIENT_API BOOL CreateChatClient(DWORD dwVersion, 
									 IClientGlobal * pGlobal, 
									 IChatClient ** ppModule)
{
	// 检查保存模块地址的指针是否有效
	if (ppModule == NULL)
	{
		return FALSE;
	}
	*ppModule = NULL;
	
	// 检查全局管理器是不是已经启动
    if (pGlobal == NULL)
	{
		return FALSE;
	}
	g_DefaultTrace.Create(pGlobal->m_TraceHelper.GetTraceObject());
	TraceLn("ChatClient - CreateChatClient");

	// 检查版本是不是正确
	if (dwVersion != CHATCLIENT_VERSION)
	{
		PrintEventLn("ChatClient - VERSION NOT MATCH");
		return FALSE;
	}	
	
	// 创建模块对象
	CChatClient * pChatClient = CChatClient::Instance();
	if (pChatClient == NULL)
	{
		PrintEventLn("ChatClient (FAIL) - new CChatClient()");		
		return FALSE;
	}
	if (!pChatClient->Create(pGlobal))
	{
		PrintEventLn("ChatClient (FAIL) - pChatClient->Create(pGlobal)");
		pChatClient->Release();
		return FALSE;
	}	
	
	// 已经成功创建了模块
	*ppModule = static_cast<CChatClient *>(pChatClient);	
	TraceLn("ChatClient - CreateChatClient OK");
	return TRUE;
}
//------------------------------------------------------------------------