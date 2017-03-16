/**
*	created:		2012-6-21   0:25
*	filename: 		ClientGlobal
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "ClientGlobal.h"
//------------------------------------------------------------------------
#define MAX_SOUND_CHANNEL		16

#define COLDTIMERES_CONFIG_FILE "data\\scp\\ColdTime.res"
//------------------------------------------------------------------------
CClientGlobal::CClientGlobal(void)
{
	ZeroMemory(m_szWorkDir, sizeof(m_szWorkDir));
	m_pScreen = NULL;
	m_pMapView = NULL;
	m_pAppNetworkLink = NULL;
	m_pPlayer = NULL;
	m_pWndMgr = NULL;
	m_pGameApp = NULL;
	m_pDefFont = NULL;
	m_pDefFontWithOutline = NULL;
	m_pEventServer = NULL;
	m_pScriptUpdateManager = NULL;
}
//------------------------------------------------------------------------
CClientGlobal::~CClientGlobal(void)
{
	Close();
}
//------------------------------------------------------------------------
BOOL CClientGlobal::Create(HINSTANCE hInstance, LPCSTR szWorkDir, HWND hMainWnd, ICanvas * pScreen, ITTFont * pDefFont, ITTFont * pDefFontWithOutline, ITrace * pTrace)
{
	m_hMainWnd = hMainWnd;
	m_pScreen = pScreen;

	// 工作目录
	lstrcpyn(m_szWorkDir, szWorkDir, MAX_PATH);
	
	// Trace
	char szTraceFileName[MAX_PATH];
	std::string strTraceFile = std::string(m_szWorkDir) + std::string( TRACE_LOG_FILE );
	wsprintf(szTraceFileName, strTraceFile.c_str() );
	if(pTrace != NULL)
	{
		if(m_TraceHelper.Create(pTrace) == FALSE)
		{
			MessageBox(NULL, "[game]Create Trace failed!", "Trace faile", 0);
			return FALSE;
		}
	}
#ifdef _DEBUG
	else
	{
		if (m_TraceHelper.Create(::GetModuleHandle(NULL), 
									   OS_WINDOW|OS_FILE | OS_OUTPUTDEBUGSTRING, 
									   NULL, NULL, szTraceFileName, FALSE) == FALSE)
		{
			MessageBox(NULL, "[game]First create Trace failed!", "Trace faile", NULL );
			return FALSE;
		}				
	}
#endif
	// 时间轴
	PrintEventLn("Start m_TimerAxisHelper.Create");	
	if(m_TimerAxisHelper.Create() == FALSE)
	{
		MessageBox(0, "Create Axis Failed!", "", 0);
		return FALSE;
	}	
	PrintEventLn("Finish m_TimerAxisHelper.Create");

	// 文件系统管理
	IFileSystem * pFileSystem = NULL;
	char szMapDir[MAX_PATH] = { 0 };
	sprintf(szMapDir,    MAP_RES_DIR,			m_szWorkDir);
	m_FileSystemHelper.Create(&pFileSystem, szMapDir, FSS_DIRECTORY, m_TraceHelper.m_pTrace);
	if(pFileSystem == NULL)
	{
		MessageBox(0, "创建文件系统失败", "", 0);
		return FALSE;
	}

	// 视图系统管理
	m_pMapView = g_CommonFunc.m_MapViewHelper.m_pMapView;

	// 资源创建器
	PrintEventLn("Start m_ResMgrHelper.Create");
	if(m_ResMgrHelper.Create(m_TraceHelper.GetTraceObject()) == FALSE)
	{
		MessageBox(0, "Create resmgr Failed!", "", 0);
		return FALSE;		
	}
	PrintEventLn("Finish m_ResMgrHelper.Create");

	// CSV脚本管理器
	PrintEventLn("Start ScriptUpdateManagerHelper.Create");	
	if(g_ScriptUpdateManagerHelper.Create(m_ResMgrHelper.m_pResMgr, m_TraceHelper.GetTraceObject()) == FALSE)
	{
		MessageBox(0, "Create script Update Failed!", "", 0);
		return FALSE;	
	}
	m_pScriptUpdateManager = g_ScriptUpdateManagerHelper.m_pManager;
	PrintEventLn("Finish ScriptUpdateManagerHelper.Create");	

	// 全局默认字体
	m_pDefFont = pDefFont;
	if(m_pDefFont == NULL)
	{
		return FALSE;
	}

	// 全局默认带轮廓的字体
	m_pDefFontWithOutline = pDefFontWithOutline;
	if(m_pDefFontWithOutline == NULL)
	{
		return FALSE;
	}

	// 事件服务器
	char szEventScpFileName[MAX_PATH];
	sprintf(szEventScpFileName, EVENT_RES_FILE, g_ClientGlobal.m_szWorkDir);
	PrintEventLn("Start m_EventServerHelper.Create");	
#ifdef _DEBUG
	if(!g_EventServerHelper.Create(m_ResMgrHelper.m_pResMgr, 
								  m_TraceHelper.m_pTrace,
								  szEventScpFileName,
								  FALSE))
	{
		MessageBox(0, "Create Event Server Failed!", "", 0);
		return FALSE;
	}
#else
	if(!g_EventServerHelper.Create(m_ResMgrHelper.m_pResMgr, 
									m_TraceHelper.m_pTrace,
									szEventScpFileName,
									TRUE))
	{
		MessageBox(0, "Create Event Server Failed!", "", 0);
		return FALSE;
	}
#endif
	m_pEventServer = g_EventServerHelper.m_pEventServer;
	PrintEventLn("Finish EventServerHelper.Create");		
/*	
	// 技能管理器
	PrintEventLn("Start m_SkillHelper.Create");	
	if(g_SkillHelper.CreateSkillManager(this) == FALSE)
	{
		MessageBox(0, "Create skill module Failed!", "", 0);
		return FALSE;
	}
	m_pSkillManager = g_SkillHelper.m_pSM;
	PrintEventLn("Finish g_SkillHelper.Create");	
*/

	// 读入脚本
	PrintEventLn("Start load all script");
	if(g_ConfigDataCenter.LoadScript() == FALSE)
	{
		MessageBox(0, "load all script failed!", "", 0);
		return FALSE;
	}
	m_pDataCenter = static_cast<IConfigDataCenter *>(&g_ConfigDataCenter);
	PrintEventLn("Finish load all script");

	// 创建ItemFactory
	PrintEventLn("Start item factory Create");
	if(g_BasicSysHelper.CreateItemClassFactory(this) == FALSE)
	{
		MessageBox(0, "Create item factory Failed!", "", 0);
		return FALSE;
	}
	m_pItemClassFactory = g_BasicSysHelper.m_pICF;
	PrintEventLn("Finish item factory Create");

	// THING创建工厂
	PrintEventLn("Start thing factory Create");
	if(g_ThingFactoryHelper.Create(this) == FALSE)
	{
		MessageBox(0, "Create thing factory Failed!", "", 0);
		return FALSE;
	}
	m_pThingFactory = g_ThingFactoryHelper.m_pTCF;
	PrintEventLn("Finish thing factory Create");
/*	
	// 加载冷却系统
	PrintEventLn("Start Cold Timer system Create");
	if(!g_ColdTimeSystemHelper.Create(m_TraceHelper.m_pTrace, m_pScriptUpdateManager, COLDTIMERES_CONFIG_FILE, static_cast<IClientGlobal *>(this)))
	{
		MessageBox(0, "Create Cold Timer system Failed!", "", 0);
		return FALSE;
	}
	PrintEventLn("Finish Cold Timer system Create");
	
	// 创建声音引擎
	PrintEventLn("Start sound system Create");
	if(!g_SoundHelper.Create(MAX_SOUND_CHANNEL, static_cast<IClientGlobal *>(this)))
	{
		PrintEventLn("Create sound system Failed!");
	}
	else
	{
		PrintEventLn("Finish sound system Create");
	}

	// 创建PK模块
	PrintEventLn("Start pk system Create");
	if(!g_PKSystemHelper.Create(static_cast<IClientGlobal *>(this)))
	{
		MessageBox(0, "Create pk system Failed!", "", 0);
		return FALSE;
	}
	PrintEventLn("Finish pk system Create");

	// 创建状态系统
	PrintEventLn("Start state system Create");
	if(!g_StatusClientHelper.Create(static_cast<IClientGlobal *>(this)))
	{
		MessageBox(0, "Create state system Failed!", "", 0);
		return FALSE;
	}
	PrintEventLn("Finish state system Create");

	// 创建效果系统 
	PrintEventLn("Start effect factory Create");
	if(!g_EffectFactoryHelper.Create(static_cast<IClientGlobal *>(this)))
	{
		MessageBox(0, "Create effect factory Failed!", "", 0);
		return FALSE;
	}
	PrintEventLn("Finish effect factory Create");

	// 创建药品系统 
	PrintEventLn("Start medicine client Create");
	if(!g_MedicineClientHelper.Create(static_cast<IClientGlobal *>(this)))
	{
		MessageBox(0, "Create medicine client Failed!", "", 0);
		return FALSE;
	}
	PrintEventLn("Finish medicine client Create");

	// 创建工作技能
	PrintEventLn("Start work skill system Create");
	if(!g_WorkSkillSystemHelper.Create(static_cast<IClientGlobal *>(this)))
	{
		MessageBox(0, "Create work skill system Failed!", "", 0);
		return FALSE;
	}
	PrintEventLn("Finish work skill system Create");

	// 创建关键字过滤模块
	PrintEventLn("Start keyword filter Create");
	if(!g_KeywordFilterHelper.Create())
	{
		MessageBox(0, "Create keyword filter Failed!", "", 0);
		return FALSE;
	}
	PrintEventLn("Finish keyword filter Create");	
*/
	// 世界
	m_pClientWorld = static_cast<IClientWorld *>(&m_ClientWorld);	

	return TRUE;
}
//------------------------------------------------------------------------
void CClientGlobal::Close(void)
{

	// 释放canvas
	//m_CanvasHelper.Close();

	// 释放文件系统
	//m_FileSystemHelper.Close();
/*
	// 释放npc支持客户端
	g_TradeClientHelper.Close();

	// 释放工作技能
	g_WorkSkillSystemHelper.Close();

	// 释放药品系统
	g_MedicineClientHelper.Close();

	// 释放效果系统
	g_EffectFactoryHelper.Close();

	// 释放状态系统
	g_StatusClientHelper.Close();

	// 释放组队模块
	g_TeamClientHelper.Close();

	// 释放PK模块
	g_PKSystemHelper.Close();

	// 释放声音引擎
	g_SoundHelper.Close();
*/
	// 释放客户端模块
	g_GameModuleHelper.Close();
/*
	// 释放冷却系统
	g_ColdTimeSystemHelper.Close();

	// 释放thing工厂
	g_ThingFactoryHelper.Close();

	// 释放basicsys
	g_BasicSysHelper.Close();

	// 释放技能模块
	g_SkillHelper.Close();
	
	// 释放事件服务器
	//g_EventServerHelper

	// 释放脚本管是器
	g_ScriptUpdateManagerHelper.Close();

	// 释放聊天模块
	g_ChatClientHelper.Close();

	// 释放交互模块
	g_InteractClientHelper.Close();
*/	
	g_CommonFunc.Close();

	// 释放时间轴
	m_TimerAxisHelper.Close();
}
//------------------------------------------------------------------------
// 描  述：关键字过滤
IKeywordFilter * CClientGlobal::GetKeywordFilter(void)
{
	return g_KeywordFilterHelper.m_pKeywordFilter;
}
//------------------------------------------------------------------------
// 描  述：组队
ITeamClient * CClientGlobal::GetTeamClient(void)
{
	return g_TeamClientHelper.GetModule();
}
//------------------------------------------------------------------------
// 描  述：效果工厂
IEffectFactory * CClientGlobal::GetEffectFactory(void)
{
	return g_EffectFactoryHelper.GetEffectFactory();
}
//------------------------------------------------------------------------
// 描  述：客户端控制器
IController * CClientGlobal::GetController(void)
{
	return g_CommonFunc.m_ControllerHelper.m_pInput;
}
//------------------------------------------------------------------------
// 描  述：聊天模块
IChatClient * CClientGlobal::GetChatClient(void)
{
	return g_ChatClientHelper.GetChatClient();
}
//------------------------------------------------------------------------
// 描  述：好友模块
IFriendClient *	CClientGlobal::GetFriendClient(void)
{
	return g_FriendClientHelper.GetModule();
}
//------------------------------------------------------------------------
// 描  述：声音引擎
ISound * CClientGlobal::GetSoundEngine(void)
{
	return g_SoundHelper.m_pSound;
}
//------------------------------------------------------------------------