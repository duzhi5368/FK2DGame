/**
*	created:		2012-7-1   20:08
*	filename: 		BasicCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
//------------------------------------------------------------------------
CCriticalSectionLock	g_AniFileLock;
CCriticalSectionLock	g_AniGroupFileLock;
//------------------------------------------------------------------------
CAccountSpace<1024, 1024, 1, 1> g_space;
IClientGlobal* gp = NULL;
//------------------------------------------------------------------------