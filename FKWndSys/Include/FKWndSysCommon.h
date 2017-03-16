/**
*	created:		2012-6-23   20:53
*	filename: 		FKWndSysCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
#include <zmouse.h>
#include <Mmsystem.h>
#include "../../FKTrace/IFKTrace.h"
//------------------------------------------------------------------------
#define g_TraceHelper g_DefaultTrace
//------------------------------------------------------------------------
#ifndef PrintEventLn
#ifdef _DEBUG
#define PrintEventLn g_DefaultTrace.ThreadSafeDebugPrintLn
#else
#define PrintEventLn g_DefaultTrace.ThreadSafeDebugPrintLn
#endif
#endif
//------------------------------------------------------------------------
// 输入法支持
#pragma comment(lib,"Imm32.lib")
//------------------------------------------------------------------------
#ifndef SAFE_RELEASE
	#ifdef _DEBUG
	#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
	#else
	#define SAFE_RELEASE(p)      { if(p) { try {(p)->Release();}catch(...){}; (p)=NULL; } }
	#endif
#endif
//------------------------------------------------------------------------
#define SAFE_DELETE(p)		 { if(p) { delete (p); (p)=NULL; } }
#define SAFE_RELEASE_IMAGE(p) {CImageLoader::GetInstance()->FreeImage((p));(p)=NULL;}
//------------------------------------------------------------------------
#ifndef ASSERT
	#	ifdef _DEBUG
	#		define ASSERT(expr)	{if (!(expr))__asm int 3}
	#	else
	#		define ASSERT(expr)	 ((void*)0)
	#	endif
#endif
//------------------------------------------------------------------------
