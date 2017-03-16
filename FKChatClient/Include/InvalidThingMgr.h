/**
*	created:		2012-6-28   8:48
*	filename: 		InvalidThingMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <list>
#include "../../FKGame/SceneObj/IThing.h"
//------------------------------------------------------------------------
using std::list;
//------------------------------------------------------------------------
class CInvalidThingMgr  
{
public:
	CInvalidThingMgr();
	virtual ~CInvalidThingMgr();
public:
	void AddThing(IThing *pThing);
	void ReleaseAll();
private:
	list<IThing *> m_ThingList;
};
//------------------------------------------------------------------------