/**
*	created:		2012-6-28   9:33
*	filename: 		TitleMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		当前玩家的称号
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <list>
#include <hash_map>
#include "../Include/TitleDef.h"
#include "../../FKResManager/IFKResManager.h"
//------------------------------------------------------------------------
using std::list;
using stdext::hash_map;
//------------------------------------------------------------------------
typedef hash_map<long,STitle *>  Title_Map;
typedef hash_map<long,STitle *>::iterator Title_Map_Itr;
typedef list<SActorTitle2Ex *> Actor_Title_Map;
typedef list<SActorTitle2Ex *>::iterator  Actor_Title_Map_Itr;
//------------------------------------------------------------------------
class CTitleMgr  
{
public:
	CTitleMgr();
	virtual ~CTitleMgr();
public:
    BOOL LoadScript(IResScript *pResScript);
    STitle* GetTitle(long lTitleId);
	void AddActorTitle(SActorTitle2Ex *pActorTitle);
	SActorTitle2Ex* GetActorTitle(long lTitleId);
	void DeleteActorTitle(long lTitleId);
	Actor_Title_Map& GetActorTitleMap();
	void IntialActorTitleMap();
public:
	void FiltTitleName(string& str);
private:
    Title_Map m_TitleMap;				// 称号库
	Actor_Title_Map m_ActorTitleMap;	// 玩家所有的称号
};
//------------------------------------------------------------------------