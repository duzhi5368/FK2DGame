/**
*	created:		2012-7-1   6:15
*	filename: 		MapViewCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKTrace/IFKTrace.h"
#include "../../FKGameModule/Include/Random.h"
//------------------------------------------------------------------------
extern CFKTrace				g_DefaultTrace;
#define g_TraceHelper		g_DefaultTrace
extern CRandom				g_RandGen;
//------------------------------------------------------------------------
#ifndef TraceLn
	#ifdef _DEBUG
		#define TraceLn g_DefaultTrace.ThreadSafeDebugPrintLn
		#define TraceColorLn g_DefaultTrace.DebugPrintColorLn
	#else
		#define TraceLn __noop
		#define TraceColorLn __noop
	#endif
#endif
//------------------------------------------------------------------------
#ifndef _DEBUG
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#else
#define SAFE_RELEASE(p)      { if(p) { try {(p)->Release();}catch(...){}; (p)=NULL; } }
#endif
//------------------------------------------------------------------------