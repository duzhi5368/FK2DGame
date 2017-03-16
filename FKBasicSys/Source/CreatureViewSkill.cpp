/**
*	created:		2012-7-2   2:48
*	filename: 		CreatureViewSkill
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		和生物技能有关的
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/CreatureView.h"
#include "../Include/ResLoader.h"
#include "../Include/SkillCreate.h"
//------------------------------------------------------------------------
CCreatureView::ProcCreateSkill CCreatureView::m_funcSkill[SKILL_MAX_COUNT + 1] =
{
	&CCreatureView::skill_CommonAttack,					// 0	普通攻击

// 战士
	&CCreatureView::skill_SingleCirculDirSkill,			// 1	破风斩
	&CCreatureView::skill_SingleCirculDirSkill,			// 2	割裂
	&CCreatureView::skill_SingleCirculDirSkill,			// 3	凝击滞敌
	&CCreatureView::skill_GenericSkill,					// 4	怒吼
	
	&CCreatureView::skill_GenericSkill,					// 5	讥讽
	&CCreatureView::skill_HalfCirculDirSkill,			// 6	狂扫八方
	&CCreatureView::skill_None,							// 7	冲锋
	&CCreatureView::skill_SingleDir,					// 8	重击
	
	&CCreatureView::skill_SingleCirculDirSkill,			// 9	决灭
	&CCreatureView::skill_SingleCirculDirSkill,			// 10	必杀诀
	&CCreatureView::skill_GenericSkill,					// 11	怒火冲天
	&CCreatureView::skill_GenericSkill,					// 12	盾壁
	
	&CCreatureView::skill_GenericSkill,					// 13	羞辱
	&CCreatureView::skill_GenericSkill,					// 14	金钟御体
	&CCreatureView::skill_SingleCirculDirSkill,			// 15	劫杀
	&CCreatureView::skill_SingleCirculDirSkill,			// 16	逆血搏杀
	
	&CCreatureView::skill_GenericSkill,					// 17	战士形态切换
	
// 法师
	&CCreatureView::skill_SingleDir,					// 18	疾火珠
	&CCreatureView::skill_GenericSkill,					// 19	灵符御敌
	&CCreatureView::skill_SingleDir,					// 20	寒冰箭
	&CCreatureView::skill_GenericSkill,					// 21	灼火随身
	
	&CCreatureView::skill_None,							// 22	化影移形
	&CCreatureView::skill_MultiUnitSingleTileSkill,		// 23	冰瀑连环
	&CCreatureView::skill_GenericSkill,					// 24	神归
	&CCreatureView::skill_SingleDir,					// 25	炽火

	&CCreatureView::skill_GenericSkill,					// 26	法印
	&CCreatureView::skill_GenericSkill,					// 27	冰封
	&CCreatureView::skill_GenericSkill,					// 28	暴烈
	&CCreatureView::skill_GenericSkill,					// 29	火舞红尘
	
	&CCreatureView::skill_RainSkill,					// 30	暴雪寒流
	&CCreatureView::skill_SingleDir,					// 31	冰爆
	&CCreatureView::skill_GenericSkill,					// 32	地火焦熔
	&CCreatureView::skill_GenericSkill,					// 33	雾隐
	
	&CCreatureView::skill_GenericSkill,					// 34	法师形态切换
	
// 牧师
	&CCreatureView::skill_SingleDir,					// 35	紫电惊虹
	&CCreatureView::skill_GenericSkill,					// 36	玉泉疗伤
	&CCreatureView::skill_GenericSkill,					// 37	无量元气
	&CCreatureView::skill_GenericSkill,					// 38	荆棘丛生
	
	&CCreatureView::skill_GenericSkill,					// 39	净衣
	&CCreatureView::skill_GenericSkill,					// 40	灵光禁魔
	&CCreatureView::skill_GenericSkill,					// 41	还魂术
	&CCreatureView::skill_GenericSkill,					// 42	妙手回春
	
	&CCreatureView::skill_GenericSkill,					// 43	鼎力回天
	&CCreatureView::skill_GenericSkill,					// 44	斗气激昂
	&CCreatureView::skill_GenericSkill,					// 45	祥光普照
	&CCreatureView::skill_SingleCirculDirSkill,			// 46	幻身厉爪
	
	&CCreatureView::skill_SingleCirculDirSkill,			// 47	厉凿
	&CCreatureView::skill_SingleCirculDirSkill,			// 48	龙腾
	&CCreatureView::skill_GenericSkill,					// 49	寻衅
	&CCreatureView::skill_SingleCirculDirSkill,			// 50	合掌断魂
	
	&CCreatureView::skill_GenericSkill,					// 51	牧师形态切换

// 暗巫
	&CCreatureView::skill_GenericSkill,					// 52	魔兽奉召
	&CCreatureView::skill_SingleDir,					// 53	暗影
	&CCreatureView::skill_GenericSkill,					// 54	冥念蚀魂
	&CCreatureView::skill_GenericSkill,					// 55	化血
	
	&CCreatureView::skill_GenericSkill,					// 56	肝胆俱裂
	&CCreatureView::skill_GenericSkill,					// 57	缚敌
	&CCreatureView::skill_GenericSkill,					// 58	魅影相随
	&CCreatureView::skill_GenericSkill,					// 59	愈合
	
	&CCreatureView::skill_GenericSkill,					// 60	移伤
	&CCreatureView::skill_GenericSkill,					// 61	咆哮万状
	&CCreatureView::skill_GenericSkill,					// 62	暴烈狂魔
	&CCreatureView::skill_GenericSkill,					// 63	化功巫毒

	&CCreatureView::skill_GenericSkill,					// 64	蚀皮腐骨
	&CCreatureView::skill_GenericSkill,					// 65	毒网
	&CCreatureView::skill_GenericSkill,					// 66	催魂摄魄
	&CCreatureView::skill_GenericSkill,					// 67	缚龙索

	&CCreatureView::skill_GenericSkill,					// 68	暗巫形态切换

// 训练技能
	&CCreatureView::skill_NNNNNNNNNNNN,					// 69	物理攻击力增加
	&CCreatureView::skill_NNNNNNNNNNNN,					// 70	法术攻击力增加
	&CCreatureView::skill_NNNNNNNNNNNN,					// 71	物理防御力增加
	&CCreatureView::skill_NNNNNNNNNNNN,					// 72	法术防御力增加
	&CCreatureView::skill_NNNNNNNNNNNN,					// 73	生命值上限增加

// 宠物技能
	&CCreatureView::skill_GenericSkill,					// 74	炎灵兽的基本攻击
	&CCreatureView::skill_SingleDir,					// 75	魅影的基本攻击

// 怪物技能
	&CCreatureView::skill_GenericSkill,					// 76 	单体技能（普通技能）
	&CCreatureView::skill_SingleDir,					// 77 	单体技能（飞行系）
	&CCreatureView::skill_MultiUnitSingleTileSkill,		// 78	单体技能（对同个目标的多单元落雷型魔法）
	&CCreatureView::skill_SingleCirculDirSkill,			// 79	单体技能（以自己为中心8方向单方向技能）

	&CCreatureView::skill_GenericSkill,					// 80 	范围技能（一次性的、范围矩形）（普通技能）
	&CCreatureView::skill_SingleDir,					// 81 	范围技能（一次性的、范围矩形）（飞行系）
	&CCreatureView::skill_HalfCirculDirSkill,			// 82	范围技能（一次性的、范围扇形）（以自己为中心8方向扇形技能）
	&CCreatureView::skill_CirculDirSkill,				// 83	范围技能（一次性的、范围矩形）（以自己为中心8方向圆形技能）

	&CCreatureView::skill_GenericSkill,					// 84 	范围技能（持续性的，范围矩形）（普通技能）
	&CCreatureView::skill_RainSkill,					// 85 	范围技能（持续性的，范围矩形）（对地面范围的多单元落雷型类型）
	&CCreatureView::skill_HalfCirculDirSkill,			// 86	范围技能（持续性的，范围扇形）（以自己为中心8方向扇形技能）
	&CCreatureView::skill_CirculDirSkill,				// 87	范围技能（持续性的，范围矩形）（以自己为中心8方向圆形技能）

	&CCreatureView::skill_GenericSkill,					// 88	怪物召唤技能
	
	&CCreatureView::skill_SingleCirculDirSkill,			// 89	攻击前方9×9的圆形范围技能
	&CCreatureView::skill_SingleCirculDirSkill,			// 90	攻击前方5×6的圆形范围技能

// 资料片玩家技能
	&CCreatureView::skill_GenericSkill,					// 91 	战士：群体眩晕
	&CCreatureView::skill_WanJianSkill,					// 92 	战士：万剑朝宗
	
	&CCreatureView::skill_GenericSkill,					// 93 	法师：冰霜巨龙
	&CCreatureView::skill_MultiDirSkillEx,				// 94 	法师：火焰放射
	
	&CCreatureView::skill_GenericSkill,					// 95 	幻师：隐形
	&CCreatureView::skill_RainSkill,					// 96 	幻师：九天神雷
	
	&CCreatureView::skill_GenericSkill,					// 97 	暗巫：群体惊吓
	&CCreatureView::skill_GenericSkill,					// 98 	暗巫：自爆毒雾


//	skill_MultiDirSkillEx,				// 4	暴烈火球
//	skill_GenericSkill,					// 6	群体治疗
};

//------------------------------------------------------------------------
// 空技能还没做
BOOL CCreatureView::skill_NNNNNNNNNNNN(SSkillCreateParam* pParam)
{
	return FALSE;
}
//------------------------------------------------------------------------
// 无魔法效果技能
BOOL CCreatureView::skill_None(SSkillCreateParam* pParam)
{
	return TRUE;
}
//------------------------------------------------------------------------
// 普通攻击
BOOL CCreatureView::skill_CommonAttack(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateCommonAttackSkill(pParam, this);
}
//------------------------------------------------------------------------
// 单向飞行技能(如火球)
BOOL CCreatureView::skill_SingleDir(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateSingleDirSkill(pParam, this);
}
//------------------------------------------------------------------------
// 多单元雨类型技能的创建过程(如暴风雪)
BOOL CCreatureView::skill_RainSkill(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateRainSkill(pParam, this);
}
//------------------------------------------------------------------------
// 万剑朝宗类技能的创建过程
BOOL CCreatureView::skill_WanJianSkill(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateWanJianSkill(pParam, this);
}
//------------------------------------------------------------------------
// 对同个目标的多个单元魔法（如连冰箭等）
BOOL CCreatureView::skill_MultiUnitSingleTileSkill(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateMultiUnitSingleTileSkill(pParam, this);
}
//------------------------------------------------------------------------
// 多向飞行技能(如暴烈火球)
BOOL CCreatureView::skill_MultiDirSkillEx(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateMultiDirSkillEx(pParam, this);
}
//------------------------------------------------------------------------
// 8方向的圆形和扇形技能的创建过程(如扇面攻击)
BOOL CCreatureView::skill_SingleCirculDirSkill(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateCirculDirSkill(pParam, this, 1);
}
//------------------------------------------------------------------------
// 8方向的圆形和扇形技能的创建过程(如扇面攻击)
BOOL CCreatureView::skill_HalfCirculDirSkill(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateCirculDirSkill(pParam, this, 5);
}
//------------------------------------------------------------------------
// 8方向的圆形和扇形技能的创建过程(如扇面攻击)
BOOL CCreatureView::skill_CirculDirSkill(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateCirculDirSkill(pParam, this, 8);
}
//------------------------------------------------------------------------
// 通用技能创建
BOOL CCreatureView::skill_GenericSkill(SSkillCreateParam* pParam)
{
	return CSkillCreate::CreateGenericSkill(pParam, this);
}
//------------------------------------------------------------------------
// 顺移魔法
BOOL CCreatureView::skill_ChangeLoc(POINT pt)
{
	return CSkillCreate::CreateChangeLoc(pt, this);
}
//------------------------------------------------------------------------
// 创建技能选择光效
CMagic* CCreatureView::CreateSkillSelMagic(int nSkillID)
{
	return NULL;

	IResObject* pMagicObj = NULL;
	SMagicArchive_Mouse stRun;
	pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(32);
	if (!pMagicObj)
		return NULL;

	memset(&stRun, 0, sizeof(stRun));
	g_ResLoader.ReadMouseMagic(pMagicObj, &stRun);
	stRun.dwResID = 32;

	CMagicMouse *pMagic = new CMagicMouse;
	if(!pMagic->OnCreateItem((const char*)&stRun, sizeof(SMagicArchive_Mouse)))
	{
		SAFE_RELEASE(pMagic);
		return NULL;
	}
	pMagic->AddToMapView(m_ptTile);

	return pMagic;
}
//------------------------------------------------------------------------
/*
破风斩
割裂
凝击滞敌
怒吼
讥讽
狂扫八方
冲锋
重击
决灭
必杀诀
怒火冲天
盾壁
羞辱
金钟御体
劫杀
逆血搏杀
战士形态切换


疾火珠
灵符御敌
寒冰箭
灼火随身
化影移形
冰瀑连环
神归
炽火
法印
冰封
暴烈
火舞红尘
暴雪寒流
冰爆
地火焦熔
雾隐
法师形态切换

紫电惊虹
玉泉疗伤
无量元气
荆棘丛生
净衣
灵光禁魔
还魂术???
妙手回春
鼎力回天
斗气激昂
祥光普照
幻身厉爪
厉凿
龙腾
寻衅
合掌断魂
牧师形态切换


魔兽奉召???
暗影
冥念蚀魂
化血
肝胆俱裂
缚敌
魅影相随???
愈合???
移伤???
咆哮万状???
暴烈狂魔???
化功巫毒
蚀皮腐骨
毒网
催魂摄魄
缚龙索
暗巫形态切换
*/
//------------------------------------------------------------------------