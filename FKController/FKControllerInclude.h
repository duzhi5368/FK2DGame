/**
*	created:		2012-6-24   20:37
*	filename: 		FKControllerInclude
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
//------------------------------------------------------------------------
#ifdef	_DEBUG
#define	TestLn	PrintEventLn
#else
#define	TestLn	(void*)0
#endif
//------------------------------------------------------------------------
#ifndef SAFE_RELEASE
#define	SAFE_RELEASE(p)	{ if(p != NULL) { (p)->Release(); (p) = NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	{ if(p != NULL) { delete (p); (p) = NULL; } }
#endif
//------------------------------------------------------------------------
#include <windows.h>
#include <list>
#include <vector>
#include <algorithm>
#include <Nb30.h>
//------------------------------------------------------------------------
using std::list;
using std::vector;
//------------------------------------------------------------------------
#include "../FKGame/GameGlobal/IClientGlobal.h"
//------------------------------------------------------------------------
#define FloatTip(style, str)											       \
{																		       \
	if (g_pGlobal->GetChatClient())											   \
	{																	       \
	g_ClientGlobal->GetChatClient()->DisplayFloatMessage((style), (str));	       \
	}                                                                          \
}	                                       									   \

//------------------------------------------------------------------------
#define PLAYSOUND(id)                                     \
{                                                         \
	if (g_pGlobal->GetSoundEngine())					  \
	{													  \
	g_ClientGlobal->GetSoundEngine()->PlaySound(id);           \
	}                                                     \
}														  \

//------------------------------------------------------------------------
#define PLAYMUSIC(id)                                     \
{                                                         \
	if (g_pGlobal->GetSoundEngine())					  \
	{													  \
	g_ClientGlobal->GetSoundEngine()->PlayMusic(id);           \
	}                                                     \
}														  \

//------------------------------------------------------------------------
#ifdef	g_TraceHelper
#undef	g_TraceHelper
#endif
#ifdef _DEBUG
#define	g_TraceHelper	g_ClientGlobal->m_TraceHelper
#else
#define g_TraceHelper	__noop
#endif

#ifdef tPrintEventLn
#undef tPrintEventLn
#endif
#ifdef _DEBUG
#define tPrintEventLn	g_ClientGlobal->m_TraceHelper.DebugPrintLnWithTime
#else
#define tPrintEventLn	__noop
#endif

#ifdef TraceLn
#undef TraceLn
#endif
#ifdef _DEBUG
#define TraceLn			g_ClientGlobal->m_TraceHelper.ThreadSafeDebugPrintLn
#else
#define TraceLn			__noop
#endif
//------------------------------------------------------------------------
// 全局客户端对象指针
extern IClientGlobal *g_ClientGlobal;
//------------------------------------------------------------------------