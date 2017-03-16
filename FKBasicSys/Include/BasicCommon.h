/**
*	created:		2012-7-1   19:59
*	filename: 		BasicCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//------------------------------------------------------------------------
#ifdef _DEBUG
#define __USE_MALLOC
#endif
//------------------------------------------------------------------------
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <hash_map>
#include <list>
#include <vector>
#include <map>
#include "../../FKTrace/IFKTrace.h"
using std::map;
using std::vector;
using std::list;
using stdext::hash_map;
//------------------------------------------------------------------------
#include "../../FKGame/DataArchive/IDataArchive.h"
#include "../../FKGameModule/Include/AccountSpace.h"
#include "../../FKGame/MapView/IMapView.h"
#include "../../FKGame/MapView/MapItemView.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../../FKGame/Sound/ISound.h"
#include "../../FKGame/Common/WinMsgDef.h"
#include "GlobalFunc.h"
//------------------------------------------------------------------------
#ifndef SAFE_DELETE
#	define SAFE_DELETE(p)		 {if(p){delete (p);(p)=NULL;}}
#endif
//------------------------------------------------------------------------
#ifndef SAFE_DELETE_ARRAY
#	define SAFE_DELETE_ARRAY(p) {if(p){delete [](p);(p)=NULL;}}
#endif
//------------------------------------------------------------------------
#ifndef SAFE_RELEASE
#	ifdef _DEBUG
#		define SAFE_RELEASE(p)      {if(p){(p)->Release();(p)=NULL;}}
#	else
#		define SAFE_RELEASE(p)      {if(p){try{(p)->Release();}catch(...){};(p)=NULL;}}
#	endif
#endif
//------------------------------------------------------------------------

#define _TraceOnce(staticflag, isMsgBox, szErrorInfo) \
	static int staticflag = 0; \
	if (staticflag == 0) \
{ \
	staticflag = 1; \
	if (isMsgBox) \
	::MessageBox(NULL, szErrorInfo, "错误", MB_OK); \
	else \
	PrintEventLn(szErrorInfo); \
}

//------------------------------------------------------------------------
typedef POINT	TILE;
//------------------------------------------------------------------------
#define TRANS_VALUE				50 //(96~127,50% alpha)
#define MAX_ITEM_NAME_LENGTH	36 // item 最大名字长度
#define	MAX_PATH_LENGTH			32
//------------------------------------------------------------------------
#ifdef	g_DefaultTrace
#undef	g_DefaultTrace
#endif

#define g_DefaultTrace			gp->m_TraceHelper
#define g_pResMgr				gp->m_ResMgrHelper.m_pResMgr
#define g_fsHelper				gp->m_FileSystemHelper
#define g_CanvasHelper			((IClientGlobal*)gp)->m_CanvasHelper
#define g_pMapView				((IClientGlobal*)gp)->m_pMapView
#define g_pTimeAxis				gp->m_TimerAxisHelper.m_pTimeAxis
#define g_pFont					((IClientGlobal*)gp)->m_pDefFont
#define g_pFontWithOutline		((IClientGlobal*)gp)->m_pDefFontWithOutline
#define g_pSound				((IClientGlobal*)gp)->GetSoundEngine()
#define g_pPlayer				((IClientGlobal*)gp)->m_pPlayer

#define Tile_To_Screen	g_pMapView->TileToScreen
#define Screen_To_Tile	g_pMapView->ScreenToTile
#define Tile_To_World	g_pMapView->TileToWorld
#define World_To_Tile	g_pMapView->WorldToTile
//------------------------------------------------------------------------
extern CCriticalSectionLock	g_AniFileLock;
extern CCriticalSectionLock	g_AniGroupFileLock;
//------------------------------------------------------------------------
// 全局变量
extern CAccountSpace<1024, 1024, 1, 1> g_space;
extern IClientGlobal* gp;
//------------------------------------------------------------------------
enum ThreeState
{
	stateCreate = 0,
	stateLoop,
	stateRelease
};
enum AniType 
{
	anitypeSkillAni = 0,			// 技能动画
	anitypeGenericAni,				// 通用动画
	anitypeAppearanceAni,			// 生物外观
	anitypeHeadAni,					// 头顶光效
};
enum GenericAni 
{
	aniBorn = 0,					// 出生			
	aniUpgrade,						// 升级
	aniBeLock_Red,					// 被选中（红圈）
	aniBeLock_Green,				// 被选中（绿圈）
	aniBeLock_Yellow,				// 被选中（黄圈）
	aniBeLock_Blue,					// 被选中（蓝圈）
	aniMouse,						// 鼠标点击动画

	aniFootprint,
};
enum HeadAni 
{
	aniTaskType_Get_Can = 0,		// 小卷轴-亮		（有任务，并满足接受条件）	
	aniTaskType_Get_NoCanGet,		// 小卷轴-灰		（有任务，但未满足接受条件）
	aniTaskType_Give_Complete,		// 闭合小卷轴-亮	（有可交付任务，并已完成）
	aniTaskType_Give_NoComplete,	// 闭合小卷轴-灰	（有可交付任务，但还未完成）	

	aniNpcTypeTrans,				// 魔法门	（传送员）	
	aniNpcTypeSkill,				// 魔法球	（技能学习师）
	aniNpcTypeMedicinal,			// 大药瓶	（药品商）
	aniNpcTypeWeapon,				// 刀剑		（武器商）
	aniNpcTypeArmor,				// 铠甲		（装备商）
	aniNpcTypeRide,					// 马		（骑乘商）
	aniNpcTypeSmith,				// 铁锤		（铁匠）
	aniNpcTypeGrocer,				// 包裹		（杂货商）
};
enum DrawOrder 
{
	doFirstDrawAni = 0,
	doFirstDrawCrt
};
enum enNumType
{
	enNUM_TYPE_YELLOW = 0,		// 黄色字 （玩家自己对其他生物造成伤害）
	enNUM_TYPE_ORANGE,			// 橙色字 （其他生物之间造成伤害）
	enNUM_TYPE_RED,				// 红色字 （其他生物对玩家自己造成伤害）
	enNUM_TYPE_POWDER,			// 粉色字 （其他特殊性质造成伤害）
	enNUM_TYPE_GREEN,			// 绿色字 （恢复生命）
	
	enNUM_TYPE_MAX_NUM,			// 最大可能数目
};

enum enLoadState
{
	enLOAD_NONE = 0,	// 未读取
	enLOAD_ING,			// 正在读取
	enLOAD_OVER			// 读取完毕
};
//------------------------------------------------------------------------