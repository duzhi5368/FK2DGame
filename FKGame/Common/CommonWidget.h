/**
*	created:		2012-6-20   23:49
*	filename: 		CommonWidget
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKCrashDump/IFKCrashDumpHelper.h"
#include "../../FKTrace/IFKTrace.h"
#include "../../FKCanvas/IFKCanvas.h"
#include "../../FKGameModule/CGameModuleHelper.h"
//------------------------------------------------------------------------
#include "../GameGlobal/ClientGlobal.h"
#include "../CrashHelper/CrashReport.h"
#include "../GameApp/GameApp.h"
#include "../GameApp/CommonFunc.h"
#include "../FKFileSystem/Include/FKTempl.h"
#include "../PerformanceCheck/Performance.h"
#include "../Team/ITeamClient.h"
#include "../KeywordFilter/IKeywordFilter.h"
#include "../Effect/IEffectClient.h"
#include "../FKChatClient/IChatClient.h"
#include "../Friend/IFriendClient.h"
#include "../Event/IEvent.h"
#include "../Sound/ISound.h"
#include "../Config/CConfigDataCenter.h"
#include "../Script/IScriptUpdate.h"
#include "../FKBasicSys/CBasicSysHelper.h"
#include "../FKThingClient/ThingClientHelper.h"
//------------------------------------------------------------------------
extern CCrashDumpHelper		g_CrashDumpHelper;
//------------------------------------------------------------------------
extern CClientGlobal		g_ClientGlobal;
extern CCrashReport			g_CrashReport;
extern CGameApp				g_GameApp;
extern CCommonFunc			g_CommonFunc;
extern CProfileManager		g_ProfileManager;
extern CGameModuleHelper	g_GameModuleHelper;
extern CTeamClientHelper	g_TeamClientHelper;
extern CKeywordFilterHelper	g_KeywordFilterHelper;
extern CEffectFactoryHelper	g_EffectFactoryHelper;
extern CChatClientHelper	g_ChatClientHelper;
extern CFriendClientHelper	g_FriendClientHelper;
extern CSoundHelper			g_SoundHelper;
extern CEventServerHelper	g_EventServerHelper;
extern CConfigDataCenter	g_ConfigDataCenter;
extern CBasicSysHelper		g_BasicSysHelper;
extern CScriptUpdateManagerHelper	g_ScriptUpdateManagerHelper;
extern CClientTingFactoryHelper		g_ThingFactoryHelper;
//------------------------------------------------------------------------