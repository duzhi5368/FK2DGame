/**
*	created:		2012-7-2   1:55
*	filename: 		SkillCreate
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		技能通用创建函数
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Common/CreatureViewDef.h"
#include "Magic.h"
//------------------------------------------------------------------------
class CSkillCreate  
{
public:
	CSkillCreate();
	virtual ~CSkillCreate();
	
public:
	// 创建通用技能
	static BOOL CreateGenericSkill(SSkillCreateParam* pParam, CCreatureView *pMaster = NULL);

	// 普通攻击的创建过程
	static BOOL CreateCommonAttackSkill(SSkillCreateParam* pParam, CCreatureView *pMaster = NULL);

	// 单向远程攻击类型技能的创建过程
	static BOOL CreateSingleDirSkill(SSkillCreateParam* pParam, CCreatureView *pMaster = NULL);

	// 多个单向远程攻击类型技能的创建过程
	static BOOL CreateMultiDirSkillEx(SSkillCreateParam* pParam, CCreatureView *pMaster = NULL);

	// 万剑朝宗类技能的创建过程
	static BOOL CreateWanJianSkill(SSkillCreateParam* pParam, CCreatureView *pMaster = NULL);

	// 多单元雨类型技能的创建过程
	static BOOL CreateRainSkill(SSkillCreateParam* pParam, CCreatureView *pMaster = NULL);

	// 对同个目标的多个单元魔法（如连冰箭等）
	static BOOL CreateMultiUnitSingleTileSkill(SSkillCreateParam* pParam, CCreatureView *pMaster = NULL);

	// 8方向技能的创建过程
	static BOOL CreateCirculDirSkill(SSkillCreateParam* pParam, CCreatureView *pMaster = NULL, int nDir = 8);
	
	// 顺移
	static BOOL CreateChangeLoc(POINT pt, CCreatureView *pMaster = NULL);

public:
	// 创建脚印动画
	static int CreateFootprintAnimate(SSkillCreateParam* pParam, int nDirection, int nLeftRight);

	// 创建通用动画
	static int CreateGenericMagic(SSkillCreateParam* pParam);

	// 拖影效果
	static BOOL CreateShadow(CCreatureView *pMaster = NULL);

public:
	static void* getSkillScpObj(int nAniType, int nIndex);
	static void* getSkillLevelScpObj(int nAniType, int nIndex, int nLevel);
};
//------------------------------------------------------------------------