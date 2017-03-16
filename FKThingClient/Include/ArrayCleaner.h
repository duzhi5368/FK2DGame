/**
*	created:		2012-7-2   19:42
*	filename: 		ArrayCleaner
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		模版函数<清除数组中的无效对象>,数组中的对象必须为flat形式即不需要构造可以用memcpy搬运
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
template< class TYPE >
struct is_item_invalid
{
	bool operator()( const TYPE& d1 ) const 
	{
		return d1 == 0;
	}
};
//------------------------------------------------------------------------
#define __COMP_FUN	is_item_invalid< TYPE >
//------------------------------------------------------------------------
// 清理数组中的无效对象
// 输入	: 例 数组 0 1 0 3 5 0 8
// 输出 :    数组 1 3 5 8
// pArray	: 待处理数组 ( IN / OUT ) 
// nArrayLen: 数组长度 ( IN / OUT )   返回处理后数组的有效长度
// __is_item_invalid : 判断某个元素是否有效的仿函数
template< class TYPE >
void CleanArray( TYPE * pArray , int & nArrayLen , __COMP_FUN & __is_item_invalid )
{
	int nMovedIndex = 0;
	int nStartCopy = 0;
	for( nStartCopy = 0 ; nStartCopy< nArrayLen ; nStartCopy ++ )
	{
		if( !__is_item_invalid( pArray[ nStartCopy ] ) )
		{
			int nEndCopy = nStartCopy + 1;
			for( ; nEndCopy < nArrayLen ; nEndCopy ++ )
			{
				if( __is_item_invalid( pArray[ nEndCopy ] ) )
				{
					break;
				}
			}

			if( nMovedIndex != nStartCopy )
			{
				memcpy( &pArray[ nMovedIndex ] , 
					&pArray[ nStartCopy ] , 
					(nEndCopy-nStartCopy)*sizeof( TYPE ) );
			}

			nMovedIndex += nEndCopy-nStartCopy;
			nStartCopy = nEndCopy;
		}
	}

	nArrayLen = nMovedIndex;
}
//------------------------------------------------------------------------