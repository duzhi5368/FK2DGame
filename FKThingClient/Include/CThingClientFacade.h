/**
*	created:		2012-7-2   23:43
*	filename: 		CThingClientFacade
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		ThingClient的外观.本类负责将外部接口打包,提出来一个,与游戏逻辑无关
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IThing.h"
#include "../../FKGame/Common/IConfigDataCenter.h"
#include "../../FKGameModule/Include/Random.h"
#include "../../FKGameModule/Include/AccountSpace.h"
//------------------------------------------------------------------------
// 申明
struct IScriptUpdateManager;
class  IEventServer;
struct IMapView;
struct IContainerMgr;
struct IGoods;
struct IEquipment;
struct IStatusClient;
struct IClientPKSystem;
struct ISkillManager;
struct IResMgr;
struct ITimerSink;
struct IThingTable;
struct IClientWorld;
struct IWorkSkillSystem;
struct IClanClient;
struct IEffectFactory;
struct IItemClassFactory;
struct IColdTimeSystem;
//------------------------------------------------------------------------
#define ACCOUNTSPACE_WIDTH		32
#define ACCOUNTSPACE_HIGH		32
//------------------------------------------------------------------------
// 定义
class CThingClientFacade
{
public:
	// 脚本载入及动态更新管理器
	virtual IScriptUpdateManager *	GetScriptUpdateManager(void);	

	// SCP脚本管理器
	virtual IResMgr	*				GetResMgr(void);

	// 工作路径
	virtual char *					GetWorkDir(void);

	// 事件服务器
	virtual IEventServer *			GetEventServer(void);

	// 启用定时器
	virtual BOOL					SetTimer(DWORD dwEventID, ITimerSink * pTimerSink, DWORD dwElapse, char *szTimerDesc = NULL);	

	// 销毁定时器
	virtual void					KillTimer(DWORD dwEventID, ITimerSink * pTimerSink);

	// 取得数据设置中心
	virtual IConfigDataCenter *		GetConfigDataCenter(void);	

	// 空间计算器
	virtual CAccountSpace<ACCOUNTSPACE_WIDTH, ACCOUNTSPACE_HIGH> *	GetAccountSpace(void);

	// 取得当前地图容器
	virtual IMapView *				GetMapView(void);

	// 客户端逻辑对像管理列表
	virtual IThingTable *			GetThingTable(void);

	// 客户端游戏世界
	virtual IClientWorld *			GetClientWorld(void);

	// CMapItemView创建器
	virtual IItemClassFactory *		GetItemFactory(void);

	// 取得[检测时间戳时间，放在消息中发至客户端]
	virtual DWORD					GetMsgTimeTick(void);

	// 取得一个比系统随机的数
	virtual DWORD					GetRandomNumber(void);

	// 取得一个逻辑对像
	virtual IThing *				GetThing(UID uidThing);

	// 取得冷却系统
	virtual IColdTimeSystem *		GetColdTimeSystem(void);

	// 取得容器管理器
	virtual IContainerMgr *			GetContainerMgr(void);

	// 取得物品
	virtual IGoods *				GetGoods(UID uidGoods);

	// 取得状态系统
	virtual IStatusClient *			GetStatusClient(void);

	// 取得pk系统
	virtual IClientPKSystem *		GetPKSystem(void);

	// 取得效果系统
	virtual IEffectFactory *		GetEffectFactory(void);

	// 取得工作技能系统
	virtual IWorkSkillSystem *		GetWorkSkillSystem(void);

	// 取得技能管理器
	virtual ISkillManager *			GetSkillManager(void);

	// 取得装备
	virtual IEquipment *			GetEquipment(UID uidEquipment);

	// 取得氏族部件
	virtual IClanClient *			GetClanClient(void);

private:
	CAccountSpace<ACCOUNTSPACE_WIDTH, ACCOUNTSPACE_HIGH>	m_pAccountSpace;
	CRandom													m_RandomGenerator;
};
//------------------------------------------------------------------------
extern CThingClientFacade *			g_pThingClientFacade;
//------------------------------------------------------------------------