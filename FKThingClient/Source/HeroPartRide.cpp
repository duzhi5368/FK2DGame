/**
*	created:		2012-7-4   1:12
*	filename: 		HeroPartRide
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		主角骑乘部件
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/HeroPartRide.h"
//------------------------------------------------------------------------
CHeroPart_Ride::CHeroPart_Ride(void)
{

}
//------------------------------------------------------------------------
CHeroPart_Ride::~CHeroPart_Ride(void)
{

}
//------------------------------------------------------------------------
void CHeroPart_Ride::Mount(SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA horse)
{
	// 切换状态
	m_pMaster->GotoActState(CREATURE_STATEID_RIDE, NULL, 0);
	
	// 变换外观
	CActorPart_Ride::Mount(horse);
}
//------------------------------------------------------------------------
void CHeroPart_Ride::Dismount(void)
{
	// 切换状态
	int nWalkStyle = m_pMaster->GetPropNum(CREATURE_PROPID_WALKSTYLE);
	if(nWalkStyle == CREATURE_MOVESTYLE_SIT)
	{
		m_pMaster->GotoActState(CREATURE_STATEID_SIT, NULL, 0);
	}
	else
	{
		m_pMaster->GotoActState(CREATURE_STATEID_MOVE, NULL, 0);
	}

	// 变换外观
	CActorPart_Ride::Dismount();
}
//------------------------------------------------------------------------