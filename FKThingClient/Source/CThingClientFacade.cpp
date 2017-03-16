/**
*	created:		2012-7-2   23:54
*	filename: 		CThingClientFacade
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		ThingClientµÄÍâ¹Û
*/
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IGoods.h"
#include "../Include/ThingClientCommon.h"
#include "../Include/CThingClientFacade.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../../FKGame/ClientWorld/IClientWorld.h"
#include "../../FKGame/ClientWorld/IThingTable.h"
//------------------------------------------------------------------------
CThingClientFacade * g_pThingClientFacade;
//------------------------------------------------------------------------
IScriptUpdateManager * CThingClientFacade::GetScriptUpdateManager(void)
{	
	return g_pGlobal->m_pScriptUpdateManager;	
}
//------------------------------------------------------------------------
IResMgr	* CThingClientFacade::GetResMgr(void)
{
	return g_pGlobal->m_ResMgrHelper.m_pResMgr;
}
//------------------------------------------------------------------------
char * CThingClientFacade::GetWorkDir(void)
{
	return g_pGlobal->m_szWorkDir;
}
//------------------------------------------------------------------------
IEventServer * CThingClientFacade::GetEventServer(void)
{
	return g_pGlobal->m_pEventServer;
}
//------------------------------------------------------------------------
CAccountSpace<ACCOUNTSPACE_WIDTH, ACCOUNTSPACE_HIGH> * CThingClientFacade::GetAccountSpace(void)
{
	return &m_pAccountSpace;
}
//------------------------------------------------------------------------
BOOL CThingClientFacade::SetTimer(DWORD dwEventID, ITimerSink * pTimerSink, DWORD dwElapse, char *szTimerDesc)
{
	return g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->SetTimer(dwEventID, pTimerSink, dwElapse, szTimerDesc);
}
//------------------------------------------------------------------------
void CThingClientFacade::KillTimer(DWORD dwEventID, ITimerSink * pTimerSink)
{
	g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->KillTimer(dwEventID, pTimerSink);
}
//------------------------------------------------------------------------
IConfigDataCenter * CThingClientFacade::GetConfigDataCenter(void)
{
	return g_pGlobal->m_pDataCenter;
}
//------------------------------------------------------------------------
IMapView *	CThingClientFacade::GetMapView(void)
{
	return g_pGlobal->m_pMapView;
}
//------------------------------------------------------------------------
IThingTable * CThingClientFacade::GetThingTable(void)
{
	IClientWorld * pClientWorld = GetClientWorld();
	if(pClientWorld == NULL)
	{
		return NULL;
	}

	return pClientWorld->GetThingTable();	
}
//------------------------------------------------------------------------
IClientWorld * CThingClientFacade::GetClientWorld(void)
{
	return g_pGlobal->m_pClientWorld;
}
//------------------------------------------------------------------------
IItemClassFactory *	CThingClientFacade::GetItemFactory(void)
{
	return g_pGlobal->m_pItemClassFactory;
}
//------------------------------------------------------------------------
DWORD CThingClientFacade::GetMsgTimeTick(void)
{
	return (g_pGlobal->m_TimerAxisHelper.m_pTimeAxis->GetTimeTick() - g_pGlobal->m_dwLoginTimeTick);
}
//------------------------------------------------------------------------
DWORD CThingClientFacade::GetRandomNumber(void)
{
	static BOOL bInit = FALSE;
	if(bInit == FALSE)
	{
		m_RandomGenerator.Seed();
		bInit = TRUE;
	}

	return m_RandomGenerator.GetDWord();
}
//------------------------------------------------------------------------
IThing * CThingClientFacade::GetThing(UID uidThing)
{
	if(g_pGlobal->m_pClientWorld == NULL || g_pGlobal->m_pClientWorld->GetThingTable() == NULL)
	{
		return NULL;
	}

	return g_pGlobal->m_pClientWorld->GetThingTable()->Find(uidThing);
}
//------------------------------------------------------------------------
IColdTimeSystem * CThingClientFacade::GetColdTimeSystem(void)
{
	return NULL;
	//return g_pGlobal->GetColdTimeSystem();
}
//------------------------------------------------------------------------
IContainerMgr *	CThingClientFacade::GetContainerMgr(void)
{
	return NULL;
	//return g_pGlobal->GetContainerMgr();
}
//------------------------------------------------------------------------
IGoods * CThingClientFacade::GetGoods(UID uidGoods)
{
	IThing * pThing = GetThing(uidGoods);
	if(pThing == NULL || pThing->GetThingClass()->GetThingClass() != THINGCLASS_GOODS)
	{
		return NULL;
	}

	return (IGoods *)pThing;
}
//------------------------------------------------------------------------
IStatusClient *	CThingClientFacade::GetStatusClient(void)
{
	return NULL;
	// return g_pGlobal->GetStateClient();
}
//------------------------------------------------------------------------
IClientPKSystem * CThingClientFacade::GetPKSystem(void)
{
	return NULL;
	//return g_pGlobal->GetPKSystem();
}
//------------------------------------------------------------------------
IEffectFactory * CThingClientFacade::GetEffectFactory(void)
{
	return g_pGlobal->GetEffectFactory();
}
//------------------------------------------------------------------------
IWorkSkillSystem * CThingClientFacade::GetWorkSkillSystem(void)
{
	return NULL;
	//return g_pGlobal->GetWorkSkillSystem();
}
//------------------------------------------------------------------------
ISkillManager *	CThingClientFacade::GetSkillManager(void)
{
	return NULL;
	//return g_pGlobal->m_pSkillManager;
}
//------------------------------------------------------------------------
IEquipment * CThingClientFacade::GetEquipment(UID uidEquipment)
{
	IGoods * pGoods = GetGoods(uidEquipment);
	if(pGoods == NULL || !pGoods->GetThingClass()->IsEquipment())
	{
		return NULL;
	}

	return (IEquipment *)(pGoods);
}
//------------------------------------------------------------------------
IClanClient * CThingClientFacade::GetClanClient(void)
{
	return NULL;
	//return g_pGlobal->GetClanClient();
}
//------------------------------------------------------------------------