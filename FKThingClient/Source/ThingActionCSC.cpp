/**
*	created:		2012-7-4   1:48
*	filename: 		ThingActionCSC
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		响应服务器发下来CSC类型消息
*/
//------------------------------------------------------------------------
#include "../Include/ThingActionCSC.h"
#include "../../FKGame/SceneObj/ICreature.h"
#include "../../FKGame/Common/ICreatureCommand.h"
#include "../../FKGame/SceneObj/IThingPart.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CCreatureActStateMachine.h"
#include "../Include/CreatureViewWrapper.h"
//------------------------------------------------------------------------
void CThingAction_Destroy_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	int nLen = 0;
	char * pBuf = (char *)pEvent->GetContext(nLen);
	if(pBuf == NULL || nLen != 0)
	{
		return;
	}

	IThing * pThing = (IThing *)pEvent->GetSrcPointer();
	if(pThing == NULL)
	{
		return;
	}
	
	SAFE_RELEASE(pThing);	
}
//------------------------------------------------------------------------