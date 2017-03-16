/**
*	created:		2012-6-21   0:20
*	filename: 		IClientGlobal
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Timer/TimerAxisHelper.h"
#include "../WndSys/IWndMgr.h"
#include "../SceneObj/IActor.h"
#include "../../FKTrace/IFKTrace.h"
#include "../../FKFileSystem/IFKFileSystem.h"
#include "../../FKResManager/IFKResManager.h"
//------------------------------------------------------------------------
struct ICanvas;
struct IMapView;
struct IAppNetworkLink;
struct IGameApp;
struct IConfigDataCenter;
struct IItemClassFactory;
struct IClientWorld;
struct IThingFactory;
struct IKeywordFilter;
struct ITeamClient;
struct IEffectFactory;
class IEventServer;
struct IController;
struct IChatClient;
struct IFriendClient;
struct ISound;
struct IScriptUpdateManager;
//------------------------------------------------------------------------
// 定义
struct IClientGlobal
{
public:
	// 工作目录
	char							m_szWorkDir[MAX_PATH];
	// 登陆时间, 用来防加速
	DWORD							m_dwLoginTimeTick;
public:
	// 时间轴辅助类
	CTimerAxisHelper				m_TimerAxisHelper;
	// 绘图屏幕
	ICanvas *						m_pScreen;
	// 地图视口
	IMapView *						m_pMapView;
	// 程序主窗口
	HWND							m_hMainWnd;	
	// 通讯层接口
	IAppNetworkLink *				m_pAppNetworkLink;
	// 窗口管理器具
	IWndMgr *						m_pWndMgr;
	// 由CGameApp设置此变量
	IGameApp *						m_pGameApp;
	// 主角
	IActor *						m_pPlayer;
	// 全局的trace对象
	CFKTrace						m_TraceHelper;
	// 图形管理对象创建器
	ICanvasHelper					m_CanvasHelper;
	// 全局默认字体
	ITTFont *						m_pDefFont;
	// 全局默认带轮廓的字体
	ITTFont *						m_pDefFontWithOutline;
	// 文件系统对象创建器
	CIFileSystemHelper				m_FileSystemHelper;
	// 脚本对象的创建器
	CIResMgrHelper					m_ResMgrHelper;
	// 游戏数值管理
	IConfigDataCenter  *			m_pDataCenter;
	// CMapItemView创建器
	IItemClassFactory *				m_pItemClassFactory;
	// 客户端世界
	IClientWorld *					m_pClientWorld;
	// THING创建工厂
	IThingFactory *					m_pThingFactory;
	// 事件服务器
	IEventServer *					m_pEventServer;
	// CSV脚本处理器
	IScriptUpdateManager *			m_pScriptUpdateManager;
public:
	// 聊天模块
	virtual IChatClient *			GetChatClient(void) = NULL;
	// 关键字过滤
	virtual IKeywordFilter *		GetKeywordFilter(void) = NULL;
	// 组队
	virtual ITeamClient *			GetTeamClient(void) = NULL;
	// 效果工厂
	virtual IEffectFactory *		GetEffectFactory(void) = NULL;
	// 客户端控制器
	virtual IController *			GetController(void) = NULL;
	// 好友模块
	virtual IFriendClient *			GetFriendClient(void) = NULL;
	// 声音引擎
	virtual ISound *				GetSoundEngine(void) = NULL;
};
//------------------------------------------------------------------------