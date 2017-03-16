/**
*	created:		2012-6-26   0:01
*	filename: 		GameModuleFacade
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/GameModuleFacade.h"
#include "../Include/GameModuleCommon.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../../FKGame/ClientWorld/IClientWorld.h"
#include "../../FKGame/ClientWorld/IThingTable.h"
#include "../../FKGame/SceneObj/IThingClass.h"
//------------------------------------------------------------------------
CGameModuleFacade * g_pGameModuleFacade;
//------------------------------------------------------------------------
/////////////////////////////////////////////////////////
////
//// 获取脚本管理器
////
/////////////////////////////////////////////////////////
IScriptUpdateManager * CGameModuleFacade::GetScriptUpdateManager(void)
{	
	return NULL;
	//return g_pGlobal->m_pScriptUpdateManager;	
}

/////////////////////////////////////////////////////////
////
//// SCP脚本管理器
////
/////////////////////////////////////////////////////////
IResMgr	* CGameModuleFacade::GetResMgr(void)
{
	return g_pGlobal->m_ResMgrHelper.m_pResMgr;
}

/////////////////////////////////////////////////////////
////
////
////
/////////////////////////////////////////////////////////
char * CGameModuleFacade::GetWorkDir(void)
{
	return g_pGlobal->m_szWorkDir;
}

/////////////////////////////////////////////////////////
////
//// 事件服务器
////
/////////////////////////////////////////////////////////
IEventServer * CGameModuleFacade::GetEventServer(void)
{
	return NULL;
	//return g_pGlobal->m_pEventServer;
}

/////////////////////////////////////////////////////////
////
//// 空间计算器
////
/////////////////////////////////////////////////////////
CAccountSpace<ACCOUNTSPACE_WIDTH, ACCOUNTSPACE_HIGH> * CGameModuleFacade::GetAccountSpace(void)
{
	return &m_pAccountSpace;
}

/////////////////////////////////////////////////////////
////
//// 启用定时器
////
/////////////////////////////////////////////////////////
BOOL CGameModuleFacade::SetTimer(DWORD dwEventID, ITimerSink * pTimerSink, DWORD dwElapse, char *szTimerDesc)
{
	return g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->SetTimer(dwEventID, pTimerSink, dwElapse, szTimerDesc);
}

/////////////////////////////////////////////////////////
////
//// 销毁定时器
////
/////////////////////////////////////////////////////////
void CGameModuleFacade::KillTimer(DWORD dwEventID, ITimerSink * pTimerSink)
{
	g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->KillTimer(dwEventID, pTimerSink);
}

/////////////////////////////////////////////////////////
////
//// 取得数据设置中心
////
/////////////////////////////////////////////////////////
IConfigDataCenter * CGameModuleFacade::GetConfigDataCenter(void)
{
	return g_pGlobal->m_pDataCenter;
}

/////////////////////////////////////////////////////////
//// 
//// 取得当前地图容器
////
/////////////////////////////////////////////////////////
IMapView *	CGameModuleFacade::GetMapView(void)
{
	return g_pGlobal->m_pMapView;
}

/////////////////////////////////////////////////////////
//// 描  述：客户端逻辑对像管理列表
//// 参  数：
//// 返回值：
/////////////////////////////////////////////////////////
IThingTable * CGameModuleFacade::GetThingTable(void)
{
	IClientWorld * pClientWorld = GetClientWorld();
	if(pClientWorld == NULL)
	{
		return NULL;
	}

	return pClientWorld->GetThingTable();	
}

/////////////////////////////////////////////////////////
//// 描  述：客户端游戏世界
//// 参  数：
//// 返回值：
/////////////////////////////////////////////////////////
IClientWorld * CGameModuleFacade::GetClientWorld(void)
{
	return g_pGlobal->m_pClientWorld;
}

/////////////////////////////////////////////////////////
////
//// CMapItemView创建器
////
/////////////////////////////////////////////////////////
IItemClassFactory *	CGameModuleFacade::GetItemFactory(void)
{
	return g_pGlobal->m_pItemClassFactory;
}

/////////////////////////////////////////////////////////
////
//// 取得[检测时间戳时间，放在消息中发至客户端]
////
/////////////////////////////////////////////////////////
DWORD CGameModuleFacade::GetMsgTimeTick(void)
{
	return (g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->GetTimeTick() - g_pGlobal->m_dwLoginTimeTick);
}

/////////////////////////////////////////////////////////
////
//// 取得一个比系统随机的数
////
/////////////////////////////////////////////////////////
DWORD CGameModuleFacade::GetRandomNumber(void)
{
	static BOOL bInit = FALSE;
	if(bInit == FALSE)
	{
		m_RandomGenerator.Seed();
		bInit = TRUE;
	}
	
	return m_RandomGenerator.GetDWord();
}

/////////////////////////////////////////////////////////
//// 描  述：取得一个逻辑对像
//// 参  数：
//// 返回值：
/////////////////////////////////////////////////////////
IThing * CGameModuleFacade::GetThing(UID uidThing)
{
	if(g_pGlobal->m_pClientWorld == NULL || g_pGlobal->m_pClientWorld->GetThingTable() == NULL)
	{
		return NULL;
	}

	return g_pGlobal->m_pClientWorld->GetThingTable()->Find(uidThing);
}
