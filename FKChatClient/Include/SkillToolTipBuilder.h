/**
*	created:		2012-6-30   4:47
*	filename: 		SkillToolTipBuilder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		这个类用来产生技能的TOOLTIP
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IToolTipGenerator.h"
//------------------------------------------------------------------------
class CSkillToolTipBuilder : public CToolTipGenerator  
{
public:
	CSkillToolTipBuilder();
	virtual ~CSkillToolTipBuilder();
	
	//用一个新对象重置TOOLTIP生成器
	BOOL SetObject(DWORD dwSkillID);
	
	//为物品的TOOLTIP添加一个项
	void AddItem(int nItemID);	
};
//------------------------------------------------------------------------