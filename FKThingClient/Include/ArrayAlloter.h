/**
*	created:		2012-7-2   19:38
*	filename: 		ArrayAlloter
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		数组分配器
					快速取得一个数组中可用的元素方便的统计已分配的元素个数
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <list>
using std::list;
//------------------------------------------------------------------------
template< int nArraySize , class Element >
class CArrayAlloter
{
public:
	// ------------------------------------------------------------------------
	Element & operator[]( int nIndex )
	{
		ASSERT( nIndex >= 0 && nIndex < nArraySize );
		return m_element[nIndex];
	}

	// ------------------------------------------------------------------------
	// 分配一个可用的元素
	// ------------------------------------------------------------------------
	int  Allot()
	{
		if( m_UsableList.empty() )
			return -1;

		int nIndex = m_UsableList.front();
		m_UsableList.pop_front();

		m_nAllotedCount ++;
		return nIndex;
	}

	// ------------------------------------------------------------------------
	// 归还一个元素
	// ------------------------------------------------------------------------
	void Restore( int nIndex )
	{
		m_UsableList.push_back( nIndex );
		m_nAllotedCount --;
	}

	// ------------------------------------------------------------------------
	// 返回已分配的个数
	// ------------------------------------------------------------------------
	int  CountAlloted()
	{
		return m_nAllotedCount;
	}

	// ------------------------------------------------------------------------
	CArrayAlloter() : m_nAllotedCount( 0 )
	{
		for( int i = 0 ; i < nArraySize ; i ++ )
		{
			m_UsableList.push_back(i);
		}
	}

private:
	Element		m_element[ nArraySize ];
	list< int > m_UsableList;
	int			m_nAllotedCount;
};
//------------------------------------------------------------------------
// 下面这个模版不需要显示的调用Restore
// 但要提供一个仿函数判断某个元素是否可用
template< class TYPE >
struct is_element_canuse
{
	bool operator()( const TYPE& d1 ) const
	{
		return false;
	}
};
//------------------------------------------------------------------------
template< int nArraySize , class Element , class __CHECK_CAN_USE2 >
class CArrayAlloter2
{
public:
	// ------------------------------------------------------------------------
	Element & operator[]( int nIndex )
	{
		ASSERT( nIndex >= 0 && nIndex<nArraySize );
		return m_element[nIndex];
	}

	// ------------------------------------------------------------------------
	// 分配一个可用的元素
	// ------------------------------------------------------------------------
	int  Allot()
	{
		for ( int i = m_nLastAllotIndex ; i < nArraySize ; i ++ )
		{
			if( __CHECK_CAN_USE2()( m_element[i] ) )
			{
				m_nLastAllotIndex = i + 1;
				return i;
			}
		}

		for ( i = 0 ; i < m_nLastAllotIndex ; i ++ )
		{
			if( __CHECK_CAN_USE2()( m_element[i] ) )
			{
				m_nLastAllotIndex = i + 1;
				return i;
			}
		}

		return -1;
	}

	// ------------------------------------------------------------------------
	CArrayAlloter2() : m_nLastAllotIndex( 0 )
	{
	}

private:
	Element		m_element[ nArraySize ];
	int			m_nLastAllotIndex;
};
//------------------------------------------------------------------------