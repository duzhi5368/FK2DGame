/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKPlatform
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
#include "FKInsideErrorString.h"
//-------------------------------------------------------------------
#if defined( _MSC_VER )
	#if _MSC_VER <= 1300
		#pragma error ERR_NO_SUPPORT_VC_VERSION
	#endif
#elif defined( __GNUC__ )
	#pragma error ERR_NO_SUPPORT_COMPILER
#elif defined( __BORLANDC__ )
	#pragma error ERR_NO_SUPPORT_COMPILER
#else
	#pragma error ERR_NO_SUPPORT_COMPILER
#endif
//-------------------------------------------------------------------
#if defined( __WIN32__ ) || defined ( _WIN32 )
#elif defined( __APPLE_CC__ )
	#pragma error ERR_NO_SUPPORT_PLATFORM
#else
	#pragma error ERR_NO_SUPPORT_PLATFORM
#endif
//-------------------------------------------------------------------
#if defined(__x86_64__) || defined( _M_X64_ )
	#pragma warning WRN_DANGEROUS_PLATFORM
#endif
//-------------------------------------------------------------------
