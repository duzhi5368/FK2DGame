/**
*	created:		2012-7-2   1:56
*	filename: 		SkillCreate
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/SkillCreate.h"
#include "../Include/ResLoader.h"
#include "../Include/Magic.h"
#include "../Include/CreatureView.h"
#include "../Include/MagicMgrCirculDir.h"
#include "../Include/MagicMgrSingleDir.h"
#include "../Include/MagicMgrMultiDir.h"
#include "../Include/MagicMgrWanJian.h"
#include "../Include/MagicMgrGeneral.h"
#include "../Include/MagicMgrMultiUnit.h"
#include "../Include/MagicMgrOnePart.h"
#include "../Include/MagicMgrCommonAttack.h"
#include "../Include/MagicMgrChangeLoc.h"
#include "../Include/MagicMgrMultiUnitSingleTile.h"
#include "../Include/MagicMgrShadow.h"
//------------------------------------------------------------------------
CSkillCreate::CSkillCreate()
{

}
//------------------------------------------------------------------------
CSkillCreate::~CSkillCreate()
{

}
//------------------------------------------------------------------------
void* CSkillCreate::getSkillScpObj(int nAniType, int nIndex)
{
	IResObject* pScpSkill = g_ResLoader.m_pScpSkill->
		GetResObjectRoot()->
		GetSubObjectByIndex(nAniType)->
		GetSubObjectByIndex(nIndex);
	return pScpSkill;
}
//------------------------------------------------------------------------
void* CSkillCreate::getSkillLevelScpObj(int nAniType, int nIndex, int nLevel)
{	
	IResObject* pScpSkill = g_ResLoader.m_pScpSkill->
		GetResObjectRoot()->
		GetSubObjectByIndex(nAniType)->
		GetSubObjectByIndex(nIndex);
	if(pScpSkill == NULL)
		return NULL;

	IResObject* pScpSkillLv = pScpSkill->GetSubObjectByIndex(nLevel - 1);

	return pScpSkillLv;
}
//------------------------------------------------------------------------
// 创建通用技能
BOOL CSkillCreate::CreateGenericSkill(SSkillCreateParam* pParam, CCreatureView *pMaster)
{
	// 读取脚本
	IResObject* pScpSkill = (IResObject *)getSkillLevelScpObj(pParam->nAniType, pParam->nIndex, pParam->nLevel);
	if (!pScpSkill)
		return FALSE;

	// 获取魔法动画ID
	DWORD dwMagicID[2];
	pScpSkill->GetPropertyLong("Ex_MagicIDRun"		,(long&)dwMagicID[0], 0);
	pScpSkill->GetPropertyLong("Ex_MagicIDExplode"	,(long&)dwMagicID[1], 0);

	// 读入运行态的魔法创建参数（注意这个运行态的不能配飞行类魔法）
	IResObject* pMagicObj = NULL;
	SMagicArchive_General stRun;
	if (dwMagicID[0])
	{
		pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID[0]);
		if (!pMagicObj)
			return FALSE;

		memset(&stRun, 0, sizeof(stRun));
		g_ResLoader.ReadGeneralMagic(pMagicObj, &stRun);
		stRun.dwResID = pMagicObj->GetID();
	}

	// 读入爆炸态的魔法创建参数
	SMagicArchive_General stExplore;
	if (dwMagicID[1])
	{
		pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID[1]);
		if (!pMagicObj) return FALSE;
		memset(&stExplore, 0, sizeof(stExplore));
		g_ResLoader.ReadGeneralMagic(pMagicObj, &stExplore);
		stExplore.dwResID = pMagicObj->GetID();
	}

	// 构造创建参数
	CMagicMgrGeneral::SMagicMgrArchive_General st;
	st.ptTile = pParam->ptTile;
	st.ptAttackTile = pParam->ptAttack;
	st.bNeedAttackDelay = TRUE;
	st.pRun		= (dwMagicID[0] > 0) ? &stRun		: NULL;
	st.pExplode = (dwMagicID[1] > 0) ? &stExplore	: NULL;
	pScpSkill->GetPropertyLong("Ex_nLoopTime", (long&)st.nLoopTime, 0);
	pScpSkill->GetPropertyLong("Ex_nCanCtrl", (long&)st.bCanCtrl, 0);
	pScpSkill->GetPropertyLong("Ex_IsOnceAttack", (long&)st.bOnceAttack, 0);

	// 创建魔法管理器
	CMagicMgrGeneral* pMagicMgr = new CMagicMgrGeneral;
	// 增加被击光效
	DWORD dwMagicBeHitID;
	pScpSkill->GetPropertyLong("Base_MagicIDBeHit", (long&)dwMagicBeHitID, 0);
	pMagicMgr->SetMagicIDBeHit(dwMagicBeHitID);
	pMagicMgr->SetMasterView(pMaster);
	pMagicMgr->SetAttackView(pParam->pAttackView);
	pMagicMgr->SetCheckID(pParam->nCheckId);
	if (!pMagicMgr->Create(&st, sizeof(st)))
	{
		delete pMagicMgr;
		return FALSE;
	}

	// 把魔法管理器和生物绑定
	pMaster->AddMagic(pMagicMgr);
	return TRUE;
}
//------------------------------------------------------------------------
// 普通攻击的创建过程
BOOL CSkillCreate::CreateCommonAttackSkill(SSkillCreateParam* pParam, CCreatureView *pMaster)
{
	// 创建魔法管理器
	CMagicMgrCommonAttack* pMagicMgr = new CMagicMgrCommonAttack;
	pMagicMgr->SetMasterView(pMaster);
	pMagicMgr->SetCheckID(pParam->nCheckId);
	if (!pMagicMgr->Create(NULL, 0))
	{
		delete pMagicMgr;
		return FALSE;
	}

	// 把魔法管理器和生物绑定
	pMaster->AddMagic(pMagicMgr);
	return TRUE;
}
//------------------------------------------------------------------------
// 单向远程攻击类型技能的创建过程(如火球、冰矢等)
BOOL CSkillCreate::CreateSingleDirSkill(SSkillCreateParam* pParam, CCreatureView *pMaster)
{
	// 读取脚本
	IResObject* pScpSkill = (IResObject*)getSkillLevelScpObj(pParam->nAniType, pParam->nIndex, pParam->nLevel);
	if (!pScpSkill)
		return FALSE;

	// 获取魔法动画ID
	DWORD dwMagicID[2];
	pScpSkill->GetPropertyLong("Ex_MagicIDRun", (long&)dwMagicID[0], 0);
	pScpSkill->GetPropertyLong("Ex_MagicIDExplode", (long&)dwMagicID[1], 0);

	// 读入移动阶段的魔法数据
	IResObject* pMagicObj = NULL;
	SMagicArchive_SingleDir st1;
	if(dwMagicID[0] == 0)
		return FALSE;

	pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID[0]);
	if (!pMagicObj)
		return FALSE;

	memset(&st1, 0, sizeof(st1));
	g_ResLoader.ReadSingleDirMagic(pMagicObj, &st1);
	st1.dwResID = pMagicObj->GetID();
	st1.ptStartTile = pParam->ptTile;
	st1.pAttackView = pParam->pAttackView;
	if(st1.pAttackView != NULL)
		st1.ptAttackTile = st1.pAttackView->GetTile();
	else
		st1.ptAttackTile = pParam->ptAttack;
	st1.ptTileOffset = pMaster->m_ptTileOffset;
	pScpSkill->GetPropertyLong("Ex_FlyTileTime", (long&)(st1.nTimeTile), 60);

	// 读入爆炸阶段的魔法数据
	SMagicArchive_General st2;
	if (dwMagicID[1])
	{
		pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID[1]);
		if (!pMagicObj)
			return FALSE;

		memset(&st2, 0, sizeof(st2));
		g_ResLoader.ReadGeneralMagic(pMagicObj, &st2);
		st2.dwResID = pMagicObj->GetID();
	}

	// 构造创建参数
	CMagicMgrSingleDir::SMagicMgrArchive_SingleDir st;
	st.pRun = (dwMagicID[0]) ? &st1 : NULL;
	st.pExplode = (dwMagicID[1]) ? &st2 : NULL;

	// 创建魔法管理器
	CMagicMgrSingleDir* pMagicMgr = new CMagicMgrSingleDir;
	pMagicMgr->SetMasterView(pMaster);
	pMagicMgr->SetCheckID(pParam->nCheckId);
	if (!pMagicMgr->Create(&st, sizeof(st)))
	{
		delete pMagicMgr;
		return FALSE;
	}

	// 把魔法管理器和生物绑定
	pMaster->AddMagic(pMagicMgr);

	return TRUE;
}
//------------------------------------------------------------------------
// 特殊的多单元类型技能的创建过程（象暴烈火球，是多方向的，但是目的地是一个区域）
BOOL CSkillCreate::CreateMultiDirSkillEx(SSkillCreateParam* pParam, CCreatureView *pMaster)
{
	// 读取脚本
	IResObject* pScpSkill = (IResObject*)getSkillLevelScpObj(pParam->nAniType, pParam->nIndex, pParam->nLevel);
	if (!pScpSkill)
		return FALSE;

	// 获取魔法动画ID
	DWORD dwMagicID[2];
	pScpSkill->GetPropertyLong("Ex_MagicIDRun"		,(long&)dwMagicID[0], 0);
	pScpSkill->GetPropertyLong("Ex_MagicIDExplode"	,(long&)dwMagicID[1], 0);

	// 读入移动阶段的魔法数据
	IResObject* pMagicObj = NULL;
	SMagicArchive_SingleDir st1;
	if(dwMagicID[0] == 0)
		return FALSE;

	pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID[0]);
	if (!pMagicObj)
		return FALSE;

	memset(&st1, 0, sizeof(st1));
	g_ResLoader.ReadSingleDirMagic(pMagicObj, &st1);
	st1.dwResID = pMagicObj->GetID();
	st1.ptStartTile = pParam->ptTile;
	st1.pAttackView = pParam->pAttackView;
	if(st1.pAttackView != NULL)
		st1.ptAttackTile = st1.pAttackView->GetTile();
	else
		st1.ptAttackTile = pParam->ptAttack;
	st1.ptTileOffset = pMaster->m_ptTileOffset;
	pScpSkill->GetPropertyLong("Ex_FlyTileTime", (long&)(st1.nTimeTile), 60);

	// 读入爆炸阶段的魔法数据
	SMagicArchive_General st2;
	if (dwMagicID[1])
	{
		pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID[1]);
		if (!pMagicObj)
			return FALSE;

		memset(&st2, 0, sizeof(st2));
		g_ResLoader.ReadGeneralMagic(pMagicObj, &st2);
		st2.dwResID = pMagicObj->GetID();
	}

	// 构造创建参数
	CMagicMgrMultiDirEx::SMagicMgrArchive_MultiDirEx st;
	st.ptAttackTile = pParam->ptAttack;
	pScpSkill->GetPropertyLong("Ex_Width", (long&)st.nWidth, 0);
	pScpSkill->GetPropertyLong("Ex_Delay", (long&)st.nDelay, 0);
	st.pRun	= (dwMagicID[0]) ? &st1 : NULL;
	st.pExplode = (dwMagicID[1]) ? &st2 : NULL;

	// 创建魔法管理器
	CMagicMgrMultiDirEx* pMagicMgr = new CMagicMgrMultiDirEx;
	DWORD dwMagicBeHitID;
	pScpSkill->GetPropertyLong("Base_MagicIDBeHit", (long&)dwMagicBeHitID, 0);
	pMagicMgr->SetMagicIDBeHit(dwMagicBeHitID);

	// 把魔法管理器和生物绑定
	pMagicMgr->SetMasterView(pMaster);
	pMagicMgr->SetCheckID(pParam->nCheckId);
	if (!pMagicMgr->Create(&st, sizeof(st)))
	{
		delete pMagicMgr;
		return FALSE;
	}

	// 增加被击光效
	pMaster->AddMagic(pMagicMgr);
	return TRUE;
}
//------------------------------------------------------------------------
// 多单元雨类型技能的创建过程(如流星雨、冰风暴、狂雷等)
BOOL CSkillCreate::CreateRainSkill(SSkillCreateParam* pParam, CCreatureView *pMaster)
{
	// 读取脚本
	IResObject* pScpSkill = (IResObject*)getSkillLevelScpObj(pParam->nAniType, pParam->nIndex, pParam->nLevel);
	if (!pScpSkill)
		return FALSE;

	// 获取魔法动画ID
	DWORD dwMagicID;
	pScpSkill->GetPropertyLong("Ex_MagicIDRun", (long&)dwMagicID, 0);	

	if(dwMagicID == 0)
		return FALSE;

	// 读入雨中单元魔法的数据
	IResObject* pMagicObj;
	SMagicArchive_Rain st1;
	pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID);
	if (!pMagicObj)
		return FALSE;

	memset(&st1, 0, sizeof(st1));
	g_ResLoader.ReadRainMagic(pMagicObj, &st1);
	st1.bNeedRandomOffset =	TRUE;
	st1.dwResID = pMagicObj->GetID();

	// 构造创建参数
	CMagicMgrMultiUnit::SMagicMgrArchive_MultiUnit st;
	st.ptAttackTile = pParam->ptAttack;
	pScpSkill->GetPropertyLong("Ex_Width", (long&)st.nWidth, 0);
	pScpSkill->GetPropertyLong("Ex_Delay", (long&)st.nDelay, 0);
	pScpSkill->GetPropertyLong("Ex_nLoopTime", (long&)st.nLoopTime, -1);
	st.pUnit = &st1;

	// 创建魔法管理器
	CMagicMgrMultiUnit* pMagicMgr = new CMagicMgrMultiUnit;
	// 增加被击光效
	DWORD dwMagicBeHitID;
	pScpSkill->GetPropertyLong("Base_MagicIDBeHit", (long&)dwMagicBeHitID, 0);
	pMagicMgr->SetMagicIDBeHit(dwMagicBeHitID);
	pMagicMgr->SetMasterView(pMaster);
	pMagicMgr->SetCheckID(pParam->nCheckId);
	if (!pMagicMgr->Create(&st, sizeof(st)))
	{
		delete pMagicMgr;
		return FALSE;
	}

	// 把魔法管理器和生物绑定
	pMaster->AddMagic(pMagicMgr);

	return TRUE;
}
//------------------------------------------------------------------------
// 万剑朝宗类技能的创建过程
BOOL CSkillCreate::CreateWanJianSkill(SSkillCreateParam* pParam, CCreatureView *pMaster)
{
	// 读取脚本
	IResObject* pScpSkill = (IResObject*)getSkillLevelScpObj(pParam->nAniType, pParam->nIndex, pParam->nLevel);
	if (!pScpSkill)
		return FALSE;

	// 构造创建参数
	CMagicMgrWanJian::SMagicMgrArchive_WanJian st;
	st.ptTile = pParam->ptTile;
	pScpSkill->GetPropertyLong("Ex_Delay", (long&)st.nDelay, 0);
	pScpSkill->GetPropertyLong("Ex_nLoopTime", (long&)st.nLoopTime, -1);

	// 创建魔法管理器
	CMagicMgrWanJian* pMagicMgr = new CMagicMgrWanJian;
	// 增加被击光效
	DWORD dwMagicBeHitID;
	pScpSkill->GetPropertyLong("Base_MagicIDBeHit", (long&)dwMagicBeHitID, 0);
	pMagicMgr->SetMagicIDBeHit(dwMagicBeHitID);
	pMagicMgr->SetMasterView(pMaster);
	pMagicMgr->SetCheckID(pParam->nCheckId);
	if (!pMagicMgr->Create(&st, sizeof(st)))
	{
		delete pMagicMgr;
		return FALSE;
	}

	// 把魔法管理器和生物绑定
	pMaster->AddMagic(pMagicMgr);

	return TRUE;
}
//------------------------------------------------------------------------
// 对同个目标的多个单元魔法（如连冰箭等）
BOOL CSkillCreate::CreateMultiUnitSingleTileSkill(SSkillCreateParam* pParam, CCreatureView *pMaster)
{
	// 读取脚本
	IResObject* pScpSkill = (IResObject*)getSkillLevelScpObj(pParam->nAniType, pParam->nIndex, pParam->nLevel);
	if (!pScpSkill)
		return FALSE;

	// 获取魔法动画ID
	DWORD dwMagicID;
	pScpSkill->GetPropertyLong("Ex_MagicIDRun", (long&)dwMagicID, 0);	

	if(dwMagicID == 0)
		return FALSE;

	// 读入雨中单元魔法的数据
	IResObject* pMagicObj;
	SMagicArchive_Rain st1;
	pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID);
	if (!pMagicObj)
		return FALSE;

	memset(&st1, 0, sizeof(st1));
	g_ResLoader.ReadRainMagic(pMagicObj, &st1);
	st1.bNeedRandomOffset =	FALSE;
	st1.dwResID = pMagicObj->GetID();

	// 构造创建参数
	CMagicMgrMultiUnitSingleTile::SMagicMgrArchive_MultiUnitSingleTile st;
	st.ptAttackTile = pParam->ptAttack;
	pScpSkill->GetPropertyLong("Ex_Delay", (long&)st.nDelay, 0);
	pScpSkill->GetPropertyLong("Ex_Time",	(long&)st.nTime,  0);
	st.pUnit = &st1;

	// 创建魔法管理器
	CMagicMgrMultiUnitSingleTile* pMagicMgr = new CMagicMgrMultiUnitSingleTile;

	// 增加被击光效
	DWORD dwMagicBeHitID;
	pScpSkill->GetPropertyLong("Base_MagicIDBeHit", (long&)dwMagicBeHitID, 0);
	pMagicMgr->SetMagicIDBeHit(dwMagicBeHitID);
	// 把魔法管理器和生物绑定
	pMagicMgr->SetMasterView(pMaster);
	pMagicMgr->SetAttackView(pParam->pAttackView);
	pMagicMgr->SetCheckID(pParam->nCheckId);
	if (!pMagicMgr->Create(&st, sizeof(st)))
	{
		delete pMagicMgr;
		return FALSE;
	}

	pMaster->AddMagic(pMagicMgr);

	return TRUE;
}
//------------------------------------------------------------------------
// 8方向的扇形技能的创建过程
BOOL CSkillCreate::CreateCirculDirSkill(SSkillCreateParam* pParam, CCreatureView *pMaster, int nDir)
{
	// 读取脚本
	IResObject* pScpSkill = (IResObject*)getSkillLevelScpObj(pParam->nAniType, pParam->nIndex, pParam->nLevel);
	if (!pScpSkill)
		return FALSE;

	// 获取魔法动画ID
	DWORD dwMagicID;
	pScpSkill->GetPropertyLong("Ex_MagicIDRun", (long&)dwMagicID, 0);

	if(dwMagicID == 0)
		return FALSE;

	// 读入移动阶段的魔法数据
	SMagicArchive_EightDir st1;
	IResObject *pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID);
	if (!pMagicObj)
		return FALSE;
	memset(&st1, 0, sizeof(st1));
	g_ResLoader.ReadEightDirMagic(pMagicObj, &st1);
	st1.dwResID = pMagicObj->GetID();
	st1.ptTile = pParam->ptTile;

	// 构造创建参数
	CMagicMgrCirculDir::SMagicMgrArchive_CirculDir st;
	st.ptTile		= pParam->ptTile;
	st.pRun			= &st1;
	st.nDirNum		= nDir;
	pScpSkill->GetPropertyLong("Ex_bRunMagicRealTime", (long&)st.bRealTime, 0);

	// 创建魔法管理器
	CMagicMgrCirculDir* pMagicMgr = new CMagicMgrCirculDir;
	// 增加被击光效
	DWORD dwMagicBeHitID;
	pScpSkill->GetPropertyLong("Base_MagicIDBeHit", (long&)dwMagicBeHitID, 0);
	pMagicMgr->SetMagicIDBeHit(dwMagicBeHitID);
	pMagicMgr->SetMasterView(pMaster);
	pMagicMgr->SetAttackView(pParam->pAttackView);
	pMagicMgr->SetCheckID(pParam->nCheckId);
	if (!pMagicMgr->Create(&st, sizeof(st)))
	{
		delete pMagicMgr;
		return FALSE;
	}

	// 把魔法管理器和生物绑定
	pMaster->AddMagic(pMagicMgr);
	return TRUE;
}
//------------------------------------------------------------------------
// 只播放八个方向中某个方向的技能的创建过程
BOOL CSkillCreate::CreateChangeLoc(POINT pt, CCreatureView *pMaster)
{
	// 创建魔法管理器
	CMagicMgrChangeLoc* pMagicMgr = new CMagicMgrChangeLoc;
	if (!pMagicMgr->Create(pt, pMaster))
	{
		delete pMagicMgr;
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 拖影效果
BOOL CSkillCreate::CreateShadow(CCreatureView *pMaster)
{
	if(!pMaster)
		return FALSE;

	SAFE_DELETE(pMaster->m_pMagicShadow);

	// 创建魔法管理器
	pMaster->m_pMagicShadow = new CMagicMgrShadow;
	if (!pMaster->m_pMagicShadow->Create(pMaster))
	{
		delete pMaster->m_pMagicShadow;
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 创建通用动画
int CSkillCreate::CreateGenericMagic(SSkillCreateParam* pParam)
{
	// 读取脚本
	IResObject* pScpSkill = (IResObject*)getSkillScpObj(pParam->nAniType, pParam->nIndex);
	if (!pScpSkill)
		return FALSE;

	// 获取魔法动画ID
	DWORD dwMagicID[] = {0,0};
	int nLen = pScpSkill->GetArrayLong("MagicID");
	if (nLen != pScpSkill->GetArrayLong("MagicID", (long*)dwMagicID, nLen))
		return FALSE;

	// 读入运行态的魔法创建参数
	IResObject* pMagicObj = NULL;
	SMagicArchive_General stRun;
	if (dwMagicID[0])
	{
		pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID[0]);
		if (!pMagicObj) return FALSE;
		memset(&stRun, 0, sizeof(stRun));
		g_ResLoader.ReadGeneralMagic(pMagicObj, &stRun);
		stRun.dwResID = pMagicObj->GetID();
	}

	// 读入爆炸态的魔法创建参数
	SMagicArchive_General stExplore;
	if (dwMagicID[1])
	{
		pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID[1]);
		if (!pMagicObj) return FALSE;
		memset(&stExplore, 0, sizeof(stExplore));
		g_ResLoader.ReadGeneralMagic(pMagicObj, &stExplore);
		stExplore.dwResID = pMagicObj->GetID();
	}

	// 构造创建参数
	CMagicMgrGeneral::SMagicMgrArchive_General st;
	st.ptTile = pParam->ptTile;
	st.ptAttackTile = pParam->ptAttack;
	st.pRun = (dwMagicID[0]) ? &stRun : NULL;
	st.pExplode = (dwMagicID[1]) ? &stExplore : NULL;
	st.bNeedAttackDelay = FALSE;
	pScpSkill->GetPropertyLong("Ex_nLoopTime", (long&)st.nLoopTime, 0);
	pScpSkill->GetPropertyLong("Ex_nCanCtrl", (long&)st.bCanCtrl, 0);

	// 创建魔法管理器
	CMagicMgrGeneral* pMagicMgr = new CMagicMgrGeneral;
	if (!pMagicMgr->Create(&st, sizeof(st)))
	{
		delete pMagicMgr;
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 创建脚印动画
int CSkillCreate::CreateFootprintAnimate(SSkillCreateParam* pParam, int nDirection, int nLeftRight)
{
	if (nDirection < 0 || nDirection > CrtDirMaxDirCount)
		return FALSE;
	// 读取脚本
	IResObject* pScpSkill = (IResObject*)getSkillScpObj(pParam->nAniType, pParam->nIndex);
	if (!pScpSkill) return FALSE;

	// 获取魔法动画ID
	DWORD dwMagicID = 0;
	pScpSkill->GetPropertyLong("MagicID", (long&)dwMagicID);
	if (dwMagicID == 0)
		return FALSE;
	POINT ptOffset[CrtDirMaxDirCount];
	if (nLeftRight == 0) // left
		pScpSkill->GetArrayLong("LLegOffset", (long*)ptOffset, CrtDirMaxDirCount*2);
	else
		pScpSkill->GetArrayLong("RLegOffset", (long*)ptOffset, CrtDirMaxDirCount*2);

	// 读入运行态的魔法创建参数
	IResObject* pMagicObj = NULL;
	SMagicArchive_FootPrint stRun;
	pMagicObj = g_ResLoader.m_pScpMagic->GetResObject(dwMagicID);
	if (!pMagicObj) return FALSE;
	memset(&stRun, 0, sizeof(stRun));
	g_ResLoader.ReadLifeMagic(pMagicObj, &stRun);
	stRun.dwResID = pMagicObj->GetID();
	stRun.nDirection = nDirection;
	stRun.ptOffset = ptOffset[nDirection];

	// 构造创建参数
	CMagicMgrOnePart<CMagicFootprint, SMagicArchive_FootPrint>::SMagicMgrArchive_OnePart ar;
	ar.ptTile = pParam->ptTile;
	ar.ptAttackTile = pParam->ptAttack;
	ar.pCreate = &stRun;

	// 创建魔法管理器
	CMagicMgrOnePart<CMagicFootprint, SMagicArchive_FootPrint>* pMagicMgr = 
		new CMagicMgrOnePart<CMagicFootprint, SMagicArchive_FootPrint>;
	if (!pMagicMgr->Create(&ar, sizeof(ar)))
	{
		delete pMagicMgr;
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------