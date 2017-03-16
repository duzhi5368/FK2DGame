/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKStringVertor
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
#include "FKCommonPre.h"
#include "FKString.h"
#include "FKSharedPtr.h"
//-------------------------------------------------------------------
namespace FKCommon
{
	typedef std::vector< String >			StringVector;
	typedef TSharedPtr< StringVector >		StringVectorPtr;
}