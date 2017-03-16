/**
*	created:		2012-6-25   23:48
*	filename: 		GameModuleFacade
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IThing.h"
#include "../../FKGame/Common/IConfigDataCenter.h"
#include "Random.h"
#include "AccountSpace.h"
//------------------------------------------------------------------------
// 申明
struct IScriptUpdateManager;
class  IEventServer;
struct IMapView;
struct IResMgr;
struct ITimerSink;
struct IThingTable;
struct IClientWorld;
struct IItemClassFactory;
//------------------------------------------------------------------------
#define ACCOUNTSPACE_WIDTH		32
#define ACCOUNTSPACE_HIGH		32
//------------------------------------------------------------------------
// 定义
class CGameModuleFacade
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

private:
	CAccountSpace<ACCOUNTSPACE_WIDTH, ACCOUNTSPACE_HIGH>	m_pAccountSpace;
	CRandom													m_RandomGenerator;
};
//------------------------------------------------------------------------
extern CGameModuleFacade *			g_pGameModuleFacade;
//------------------------------------------------------------------------