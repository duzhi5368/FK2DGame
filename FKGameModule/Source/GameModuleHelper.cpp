/**
*	created:		2012-6-26   0:02
*	filename: 		GameModuleHelper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../CGameModuleHelper.h"
#include "../Include/GameModuleCommon.h"
#include "../Include/GameModuleFacade.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
//------------------------------------------------------------------------
#define VERSION_GAME_MODULE 2
//------------------------------------------------------------------------
BOOL InitProject(void)
{
	g_pGameModuleFacade = new CGameModuleFacade();
	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：输出函数 - 创建物件工厂
extern "C" __declspec(dllexport) BOOL CreateGameModule(DWORD dwVersion, IClientGlobal * pGlobal)
{
	if(dwVersion != VERSION_GAME_MODULE)
	{
		return FALSE;
	}
	
	if(pGlobal == NULL)
	{
		return FALSE;
	}
	
	// 初始化全局变量
	g_pGlobal = pGlobal;
	
	// 初始化
	//g_TraceHelper.ThreadSafeDebugPrint("Start init GameModule.dll!");
	if(InitProject() == FALSE)
	{	
		g_TraceHelper.ThreadSafeDebugPrint("Init GameModule.dll Failed!");
		return FALSE;
	}
	//g_TraceHelper.ThreadSafeDebugPrintLn("Finish init GameModule.dll!");
		
	return TRUE;
}
//------------------------------------------------------------------------