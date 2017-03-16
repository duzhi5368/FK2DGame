/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKCommonPre
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
#include "FKPlatform.h"
#include "FKStdHeader.h"
#include "FKInsideMacro.h"
#include "FKTypedef.h"
#include "FKRecursiveMutex.h"
//-------------------------------------------------------------------
namespace FKCommon
{
	#define FK_COMMON_MAHOR_VERSION 1
	#define FK_COMMON_MINOR_VERSION 0
	#define FK_COMMON_PATCH_VERSION 1
	#define FK_COMMON_VERSION (( FK_COMMON_MAHOR_VERSION << 16 ) | ( FK_COMMON_MINOR_VERSION << 8 )| ( FK_COMMON_PATCH_VERSION ) )
}
//-------------------------------------------------------------------