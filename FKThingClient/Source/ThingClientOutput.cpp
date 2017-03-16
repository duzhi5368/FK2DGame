/**
*	created:		2012-7-2   23:22
*	filename: 		ThingClientOutput
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		工程输入文件
*/
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IThingFactory.h"
#include "../Include/CClientThingFactory.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureActionCSCManager.h"
#include "../Include/CreatureActionManager.h"
#include "../Include/ThingActionCSCManager.h"
#include "../Include/CCreatureActStateMachine.h"
#include "../Include/ThingClientCommon.h"
#include "../Include/CActSitStateAction.h"
#include "../Include/CActStandStateAction.h"
#include "../Include/CActDieStateAction.h"
#include "../Include/CActMoveStateAction.h"
#include "../Include/CActRideStateAction.h"
//------------------------------------------------------------------------
CFKTrace g_DefaultTrace;
//------------------------------------------------------------------------
#define VERSION_THING_FACTORY 2
//------------------------------------------------------------------------
// 输出函数 - 创建物件工厂
extern "C" __declspec(dllexport) BOOL CreateClientThingFactory(DWORD dwVersion,
															   IClientGlobal * pGlobal,
															   IThingFactory ** ppThingFactory)
{
	if(pGlobal == NULL) 
	{
		return FALSE;
	}
	
	g_pGlobal = pGlobal;	
	BOOL bResult = TRUE;
	PrintEventLn("Start to create thingclassfactory project!");

	if(dwVersion != VERSION_THING_FACTORY)
	{
		PrintEventLn("VERSION_THING_FACTORY Version error!");		
		return FALSE;
	}

	// 实例化外观类
	g_pThingClientFacade = new CThingClientFacade();

	// 创建工厂
	PrintEventLn("Start Buid client factory!");
	CThingFactory * pThingFactory = new CThingFactory();
	*ppThingFactory = static_cast<IThingFactory *>(pThingFactory);	
	PrintEventLn("Finish Buid client factory!");	

	CCreatureActSitState::InitFilterMessageProc();
	CCreatureActStandState::InitFilterMessageProc();
	CCreatureActDieState::InitFilterMessageProc();
	CCreatureActMoveState::InitFilterMessageProc();
	CCreatureActRideState::InitFilterMessageProc();	

	PrintEventLn("Start build action manager!");
	if(g_CreatureActionManager.Build() == FALSE)
	{
		PrintEventLn("Failed build action manager!");
		return FALSE;
	}
	PrintEventLn("Finish build action manager!");

	PrintEventLn("Start build action CSC manager!");
	if(g_CreatureActionCSCManager.Build() == FALSE)
	{
		PrintEventLn("Failed build action CSC manager!");
		return FALSE;
	}
	PrintEventLn("Finish build action CSC manager!");

	PrintEventLn("Start build thing action CSC manager!");
	if(g_ThingActionCSCManager.Build() == FALSE)
	{
		PrintEventLn("Failed build thing action CSC manager!");
		return FALSE;
	}
	PrintEventLn("Finish build thing action CSC manager!");

	return *ppThingFactory != NULL;
}
//------------------------------------------------------------------------