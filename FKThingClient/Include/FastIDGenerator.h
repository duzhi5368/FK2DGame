/**
*	created:		2012-7-2   19:43
*	filename: 		FastIDGenerator
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		以一种比较迅速的机制产生一个空余的ID
					给一个对象指定唯一ID的时候经常要写类似代码
					for( int i = 0; i < XXXMax ; i ++ )
						if( i 是空余的 ) 返回i

					这样每次从0开始遍历效率较低也很麻烦。
					这里提供两种解决方法
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <list>
using std::list;
//------------------------------------------------------------------------
//	1.用一个队列记录所有可用ID
//	  当可用ID的范围很大时不适用(例如服务器UID可从0x00-0xffffffff)
template< class __ID_TYPE__ >
class CIDgeneratorPool
{
public:
	// ------------------------------------------------------------------------
	// idMin ~ idMax : 合法ID范围
	// ------------------------------------------------------------------------
	CIDgeneratorPool( __ID_TYPE__ idMin , __ID_TYPE__ idMax )
	{
		for( __ID_TYPE__ i = idMin ; i < idMax ; ++ i )
		{
			m_idList.push_back( i );
		}
	}

	// ------------------------------------------------------------------------
	// 分配一个可用ID
	// ------------------------------------------------------------------------
	BOOL Allot( __ID_TYPE__ & id )
	{
		if( m_idList.empty() )
			return FALSE;

		id = * (m_idList.begin());
		m_idList.pop_front();
		return TRUE;
	}

	// ------------------------------------------------------------------------
	// 归还一个可用ID
	// ------------------------------------------------------------------------
	void Restore( __ID_TYPE__ & id )
	{
		m_idList.push_back( id );
	}

	// ------------------------------------------------------------------------
	// 直接霸占一个指定的ID，不用分配
	// ------------------------------------------------------------------------
	BOOL Forcibl(__ID_TYPE__ id)
	{
		if(m_idList.empty() == TRUE)
		{
			return FALSE;
		}

		list< __ID_TYPE__ >::iterator it = m_idList.begin();
		for( ; it != m_idList.end(); ++it)
		{
			__ID_TYPE__ itid = *it;
			if(itid == id)
			{
				m_idList.erase(it);
				return TRUE;
			}
		}

		return FALSE;
	}	

protected:
	CIDgeneratorPool()
	{
	}

private:
	list< __ID_TYPE__ > m_idList;
};
//------------------------------------------------------------------------
//	2.用一个值纪录分配的最后一个ID,每次分配的时候默认
//	  分配下一个ID.
template< class TYPE >
struct is_id_canuse
{
	bool operator()( const TYPE& d1 ) 
	{
		return false;
	}
};
//------------------------------------------------------------------------
template< class __ID_TYPE__ , class __CHECK_IDCAN_USE = is_id_canuse<__ID_TYPE__> >
class CIDgenerator
{
public:
	// ------------------------------------------------------------------------
	// idMin ~ idMax : 合法ID范围
	// __check_fun	 : 判断ID是否有效的函数
	// ------------------------------------------------------------------------
	CIDgenerator( __ID_TYPE__ idMin , __ID_TYPE__ idMax )
	{
		m_idLastAlloted = idMin;
		m_idMin = idMin;
		m_idMax = idMax;
	}

	// ------------------------------------------------------------------------
	// 分配一个可用ID
	// ------------------------------------------------------------------------
	BOOL Allot( __ID_TYPE__ & id )
	{
		for ( __ID_TYPE__ i = m_idLastAlloted ; i < m_idMax ; i ++ )
		{
			if( __CHECK_IDCAN_USE( i ) )
			{
				m_idLastAlloted = i + 1;
				id = i;
				return TRUE;
			}
		}

		for ( i = m_idMin ; i < m_idLastAlloted ; i ++ )
		{
			if( __CHECK_IDCAN_USE( i ) )
			{
				m_idLastAlloted = i + 1;
				id = i;
				return TRUE;
			}
		}

		return FALSE;
	}

	// ------------------------------------------------------------------------
	// 分配一个可用ID
	// ------------------------------------------------------------------------
	BOOL Allot( __ID_TYPE__ & id , __CHECK_IDCAN_USE & __check_fun )
	{
		for ( __ID_TYPE__ i = m_idLastAlloted ; i < m_idMax ; i ++ )
		{
			if( __check_fun( i ) )
			{
				m_idLastAlloted = i + 1;
				id = i;
				return TRUE;
			}
		}

		for ( i = m_idMin ; i < m_idLastAlloted ; i ++ )
		{
			if( __check_fun( i ) )
			{
				m_idLastAlloted = i + 1;
				id = i;
				return TRUE;
			}
		}

		return FALSE;
	}

protected:
	CIDgenerator()
	{
	}

private:
	__ID_TYPE__		m_idLastAlloted;
	__ID_TYPE__		m_idMin;
	__ID_TYPE__		m_idMax;
};
//------------------------------------------------------------------------