/**
*	created:		2012-6-28   8:55
*	filename: 		InvalidThingMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/InvalidThingMgr.h"
//------------------------------------------------------------------------
CInvalidThingMgr::CInvalidThingMgr()
{

}
//------------------------------------------------------------------------
CInvalidThingMgr::~CInvalidThingMgr()
{

}
//------------------------------------------------------------------------
void CInvalidThingMgr::AddThing(IThing *pThing)
{
	if( pThing )
	{
		m_ThingList.push_back(pThing);
	}
}
//------------------------------------------------------------------------
void CInvalidThingMgr::ReleaseAll()
{
	list<IThing *>::iterator iter = m_ThingList.begin();
	for(;iter!=m_ThingList.end();iter++)
	{
		(*iter)->Release();
	}
	m_ThingList.clear();
}
//------------------------------------------------------------------------