/**
*	created:		2012-6-24   23:11
*	filename: 		MonsterFinder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <set>
#include <Windows.h>
using std::set;
//------------------------------------------------------------------------
struct IThing;
//------------------------------------------------------------------------
typedef  __int64 UID;
//------------------------------------------------------------------------
//最大查找半径
const int MAX_SEARCHRADIUS = 13;
//------------------------------------------------------------------------
class CMonsterFinder  
{
private:
	set<UID> setUIDMonster;
	//取附近未被选过的Thing (第二参数表示,是否选择没有归属的怪)
	IThing * __GetFreeThing(POINTS ptActor, BOOL bHPMustFull, BOOL bExcludeNPC);
	//找到的第一只怪的UID
	UID m_uidFirstMonster;
public:
	CMonsterFinder();
	virtual ~CMonsterFinder();

	IThing * GetNextMonster(BOOL bHPMustFull = FALSE, BOOL bExcludeNPC = FALSE);
};
//------------------------------------------------------------------------
