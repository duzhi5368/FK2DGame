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

#include "../Include/FKInsideStableHeader.h"
#include "../Include//FKString.h"
#include "../Include/FKStringVertor.h"

//-------------------------------------------------------------------
namespace FKCommon
{
	//-------------------------------------------------------------------
	const String CStringUtil::ms_StringUtilTemp;
	//-------------------------------------------------------------------
	void CStringUtil::Trim( OUT String& p_str, bool p_bLeft, bool p_bRight )
	{
		static const String s_Delims = " \t\r";
		if( p_bRight )
		{
			p_str.erase( p_str.find_last_not_of( s_Delims ) + 1 );
		}
		if( p_bLeft )
		{
			p_str.erase( 0, p_str.find_first_not_of( s_Delims ) );
		}
	}
	//-------------------------------------------------------------------
	std::vector< String > CStringUtil::Split( const String& p_str, const String& p_Delims, unsigned int p_unMaxSplits )
	{
		std::vector< String > retVec;

		// 预分配一定空间，默认就定义为 10
		retVec.reserve( p_unMaxSplits ? p_unMaxSplits + 1 : 10 );

		unsigned int unNumSplits = 0;

		size_t ulStart = 0;
		size_t ulPos = 0;

		do 
		{
			ulPos = p_str.find_first_of( p_Delims, ulStart );
			if( ulPos == ulStart )
			{
				ulStart = ulPos + 1;
			}
			else if( ( ulPos == String::npos ) || ( ( p_unMaxSplits != 0 ) && ( unNumSplits == p_unMaxSplits) ) )
			{
				retVec.push_back( p_str.substr( ulStart ) );
				break;
			}
			else
			{
				retVec.push_back( p_str.substr( ulStart, ulPos - ulStart ) );
				ulStart = ulPos + 1;
			}
			ulStart = p_str.find_first_not_of( p_Delims, ulStart );
		} while ( ulPos != String::npos );

		return retVec;
	}
	//-------------------------------------------------------------------
	void CStringUtil::ToLower( OUT String& p_Str )
	{
		std::transform( p_Str.begin(), p_Str.end(), p_Str.begin(), tolower );
	}
	//-------------------------------------------------------------------
	void CStringUtil::ToUpper( OUT String& p_Str )
	{
		std::transform( p_Str.begin(), p_Str.end(), p_Str.begin(), toupper );
	}
	//-------------------------------------------------------------------
	bool CStringUtil::IsStartWith( const String& p_Str, const String& p_Pattern, bool p_bLowerCase )
	{
		size_t ulSrcLen = p_Str.length();
		size_t ulPatternLen = p_Pattern.length();
		if(( ulSrcLen < ulPatternLen ) || ( ulPatternLen == 0 ))
		{
			return false;
		}
		String szTrueStart = p_Str.substr( 0, ulPatternLen );
		if( p_bLowerCase )
		{
			CStringUtil::ToLower( szTrueStart );
		}

		return ( szTrueStart == p_Pattern );
	}
	//-------------------------------------------------------------------
	bool CStringUtil::IsEndWith( const String& p_Str, const String& p_Pattern, bool p_bLowerCase )
	{
		size_t ulSrcLen = p_Str.length();
		size_t ulPatternLen = p_Pattern.length();
		if(( ulSrcLen < ulPatternLen ) || ( ulPatternLen == 0 ))
		{
			return false;
		}
		String szTrueEnd = p_Str.substr( ulSrcLen - ulPatternLen, ulPatternLen );
		if( p_bLowerCase )
		{
			CStringUtil::ToLower( szTrueEnd );
		}

		return ( szTrueEnd == p_Pattern );
	}
	//-------------------------------------------------------------------
	String CStringUtil::StandardPath( const String& p_Str )
	{
		String szPath = p_Str;
		std::replace( szPath.begin(), szPath.end(), '\\', '/' );
		if( szPath[szPath.length() - 1] != '/' )
		{
			szPath += '/';
		}
		return szPath;
	}
	//-------------------------------------------------------------------
	void CStringUtil::SplitFileName( const String& p_WholeName, OUT String& p_BaseName, OUT String& p_Path )
	{
		String szPath = p_WholeName;
		std::replace( szPath.begin(), szPath.end(), '\\', '/' );
		size_t i = szPath.find_last_of('/');

		if( i == String::npos )
		{
			p_Path.clear();
			p_BaseName = p_WholeName;
		}
		else
		{
			p_BaseName = szPath.substr( i + 1, szPath.size() - i - 1 );
			p_Path = szPath.substr( 0, i + 1 );
		}
	}
	//-------------------------------------------------------------------
	bool CStringUtil::MatchFile( const String& p_Str, const String& p_Pattern, bool p_bCaseSensitive )
	{
		String tmpStr = p_Str;
		String tmpPattern = p_Pattern;
		if( !p_bCaseSensitive )
		{
			CStringUtil::ToLower( tmpStr );
			CStringUtil::ToLower( tmpPattern );
		}

		String::const_iterator strIte = tmpStr.begin();
		String::const_iterator patIte = tmpPattern.begin();
		String::const_iterator lastWildIte = tmpPattern.end(); // 通配符迭代器

		while( ( strIte != tmpStr.end() ) && ( patIte != tmpPattern.end() ) )
		{
			if( *patIte == '*' )
			{
				lastWildIte = patIte;	// 记录通配符所在位置
				++patIte;				// 略过下一个字符
				if( patIte == tmpPattern.end() )
				{
					strIte = tmpStr.end();
				}
				else
				{
					while(( strIte != tmpStr.end() ) && ( *strIte != *patIte ))
					{
						++strIte;
					}
				}
			}
			else
			{
				if( *patIte != *strIte )
				{
					if( lastWildIte != tmpPattern.end() )		// 存在通配符并且找到了其位置
					{
						// 通配符和参照字符串顺序不符，重新查找
						patIte = lastWildIte;
						lastWildIte = tmpPattern.end();
					}
					else
					{
						return false;
					}
				}
				else
				{
					++patIte;
					++strIte;
				}
			}
		}

		// 若源字符串和参照字符串都到了结尾，查找成功了
		if(( patIte == tmpPattern.end() ) && ( strIte == tmpStr.end() ))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	//-------------------------------------------------------------------
}