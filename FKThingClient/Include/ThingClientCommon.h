/**
*	created:		2012-7-2   22:36
*	filename: 		ThingClientCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
struct IClientGlobal;
extern	IClientGlobal * g_pGlobal;
//------------------------------------------------------------------------
#include <windows.h>
#include <wtypes.h>
#include <stdio.h>
#include <mmsystem.h>
#include <hash_map>
#include <map>
#include <vector>
#include <list>
using std::list;
using std::map;
using std::vector;
using stdext::hash_map;
//------------------------------------------------------------------------
#include "../../FKGame/Common/CreaturePropertyDef.h"
#include "../../FKGame/Common/Macro.h"
#include "../../MsgCommon/MsgKeyMainThing.h"
#include "../../MsgCommon/MsgKeyRoot.h"
#include "../../MsgCommon/MsgKeySubAction.h"
#include "../../MsgCommon/MsgKeySystem.h"
#include "../../MsgCommon/MsgFormat.h"
#include "../../FKGame/SceneObj/IThingFactory.h"
#include "../../FKGame/SceneObj/ThingDef.h"
#include "../../FKGame/WndSys/WndSys.h"
#include "../../FKGame/WndSys/WndDef.h"
#include "../../FKGame/Common/WinMsgDef.h"
#include "../../FKGame/SceneObj/IThingClass.h"
#include "../../FKGame/SceneObj/IThingPart.h"
#include "../../FKGame/SceneObj/IItemClassFactory.h"
#include "../../FKGame/ClientWorld/IClientWorld.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../../FKGame/Common/MapItemDef.h"
#include "../../FKGame/ClientWorld/IThingTable.h"
#include "../../FKGame/MapView/IMapView.h"
#include "../../FKGame/MapView/MapItemView.h"
//------------------------------------------------------------------------
#ifdef TraceLn
#undef TraceLn
#endif
#ifdef _DEBUG
#define TraceLn			g_pGlobal->m_TraceHelper.ThreadSafeDebugPrintLn
#else
#define TraceLn			__noop
#endif
//------------------------------------------------------------------------
#ifndef SAFE_CLOSE
#define SAFE_CLOSE(p) if(p) { (p)->Close(); (p) = NULL; }
#endif
//------------------------------------------------------------------------
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) if(p) { (p)->Release(); (p) = NULL; }
#endif
//------------------------------------------------------------------------