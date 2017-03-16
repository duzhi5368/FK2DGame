/**
*	created:		2012-7-4   1:50
*	filename: 		ThingActionCSCManager
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		CSC逻辑对像管理器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/ThingActionCSC.h"
//------------------------------------------------------------------------
class CThingActionCSCManager
{
public:
	// 建立
	BOOL								Build(void);

private:
	CThingAction_Destroy_CSC			m_ThingAction_Destroy_CSC;
};
//------------------------------------------------------------------------
extern CThingActionCSCManager			g_ThingActionCSCManager;
//------------------------------------------------------------------------