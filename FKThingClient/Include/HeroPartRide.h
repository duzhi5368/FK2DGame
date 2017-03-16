/**
*	created:		2012-7-4   1:10
*	filename: 		HeroPartRide
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		主角骑乘部件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "ActorPartRide.h"
//------------------------------------------------------------------------
class CHeroPart_Ride : public CActorPart_Ride
{
public:
	// 骑
	virtual void			Mount(SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA horse);
	// 下
	virtual void			Dismount(void);
public:
	CHeroPart_Ride(void);
	virtual ~CHeroPart_Ride(void);
};
//------------------------------------------------------------------------