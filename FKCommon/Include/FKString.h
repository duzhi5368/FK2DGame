/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKString
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
#include "FKCommonPre.h"
//-------------------------------------------------------------------
namespace FKCommon
{
	/** 字符串常用类 */
	class FK_DLL CStringUtil
	{
	public:
		/**
		* Comment: 	删除字符串的空白字节和TAB制表符
		* @param:
		* @return:
		* @remarks:	参数可以指定是删除字符串前或后的空白。（默认是两者都删除）
		* @note:
		*/
		static void Trim( OUT String& p_str, bool p_bLeft = true, bool p_bRight = true );
		/**
		* Comment: 	将一个字符串根据分隔符分割为一个字符串数组
		* @param:	p_Delims	分割符列表
		* @param:	p_unMaxSplits	最大分割次数。若此值为0，则表示无限制分割；
					若>0，则表示分割指定次数后部再进行分割
		* @return:
		* @remarks:
		* @note:
		*/
		static std::vector< String > Split( const String& p_str, const String& p_Delims = "\t\n ", unsigned int p_unMaxSplits = 0 );
		/**
		* Comment: 	将一个字符串内所有字符转换为小写字符
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static void ToLower( OUT String& p_Str );
		/**
		* Comment: 	将一个字符串所有字符转换为大写字符
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static void ToUpper( OUT String& p_Str );
		/**
		* Comment: 	判断一个字符串是否是以指定参考字符串为开头的
		* @param:	p_Pattern	参考字符串
		* @param:	p_bLowerCase	若该参数为true，则意味着需要进行比较的字符串在比较前将被转化为小写字符，
					当然，参考字符串也必须小写。
		* @return:
		* @remarks:
		* @note:
		*/
		static bool	IsStartWith( const String& p_Str, const String& p_Pattern, bool p_bLowerCase = true );
		/**
		* Comment: 	判断一个字符串是否是以指定参考字符串为结尾的
		* @param:	p_Pattern	参考字符串
		* @param:	p_bLowerCase	若该参数为true，则意味着需要进行比较的字符串在比较前将被转化为小写字符，
					当然，参考字符串也必须小写。
		* @return:
		* @remarks:
		* @note:
		*/
		static bool IsEndWith( const String& p_Str, const String& p_Pattern, bool p_bLowerCase = true );
		/**
		* Comment: 	标准化路径。（标准化后的路径仅用前向斜线，也将以前斜线作为结尾）
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static String StandardPath( const String& p_Str );
		/**
		* Comment: 	将一个完整的文件和路径分割为基本文件名和路径两部分。
		* @param:
		* @return:
		* @remarks:	注意：路径将被标准化处理
		* @note:
		*/
		static void SplitFileName( const String& p_WholeName, OUT String& p_BaseName, OUT String& p_Path );
		/**
		* Comment: 	比对文件名是否属于某个类型文件（仅检查后缀）
		* @param:	p_Pattern	比对参数字符串。它可以包含“*”通配符
		* @param:	p_bCaseSensitive	是否对大小写敏感
		* @return:
		* @remarks:
		* @note:
		*/
		static bool MatchFile( const String& p_Str, const String& p_Pattern, bool p_bCaseSensitive = false );
	private:
		///	StringUtil常量空白字符串
		static const String			ms_StringUtilTemp;
	};
}