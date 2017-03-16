/**
*	created:		2012-6-24   22:45
*	filename: 		CreatureSignal
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		用来实现生物消息的订阅
*/
//------------------------------------------------------------------------
#pragma once
///------------------------------------------------------------------------
#include <list>
using std::list;
//------------------------------------------------------------------------
#ifndef _BASIC_CONNECTION__
#define _BASIC_CONNECTION__
	//------------------------------------------------------------------------
	template< class ARG1 , class ARG2 , class ARG3 >
	struct __BasicConnection3
	{
		virtual void Emit( ARG1 , ARG2 , ARG3 )
		{
		}
	};

	//------------------------------------------------------------------------
	template< class TYPE , class ARG1 , class ARG2 , class ARG3 >
	struct __SignalConnection3 : public __BasicConnection3< ARG1 , ARG2 , ARG3 >
	{
	public:
		typedef void (TYPE::*CONNECTFUN)( ARG1 , ARG2 , ARG3 );

		void Emit( ARG1 a , ARG2 b , ARG3 c )
		{
			( m_pConnection->*m_ConnectFun )( a , b , c );
		}

	public:
		TYPE * m_pConnection;
		CONNECTFUN m_ConnectFun;
	};

	//------------------------------------------------------------------------
	template< class ARG1 , class ARG2 , class ARG3 >
	class __SignalSender3
	{
	public:
		void AddConnection( __BasicConnection3< ARG1 , ARG2 , ARG3 > * pConnect )
		{
			m_ConnectList.push_back( pConnect );
		}

		void DelConnection( __BasicConnection3< ARG1 , ARG2 , ARG3 > * pConnect )
		{
			m_ConnectList.remove( pConnect );
		}

		void Emit( ARG1 a, ARG2 b, ARG3 c )
		{
			list< __BasicConnection3< ARG1 , ARG2 , ARG3 > * >::iterator it ;
			for( it = m_ConnectList.begin() ; it != m_ConnectList.end() ; /*it ++*/ )
			{
				(*( it ++ ))->Emit( a , b , c );
			}
		}
	private:
		typedef  list< __BasicConnection3< ARG1 , ARG2 , ARG3 > * > ConnectionList;
		ConnectionList m_ConnectList;
	};
	//------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------
// 管理消息订阅及消息派送的类
template< int MAX_MESSAGE = 16 >
class CMessageSender
{
public:
	// 订阅消息  消息号为dwMsg , pConnect为接受端连接对象指针
	BOOL Subscibe( DWORD dwMsg , __BasicConnection3< LPCSTR , int , BOOL & > * pConnect )
	{
		if( dwMsg >= 0 && dwMsg < MAX_MESSAGE )
		{
			m_Sender[ dwMsg ].AddConnection( pConnect );
			return TRUE;
		}

		return FALSE;
	}

	// 退订消息  消息号为dwMsg , pConnect为接受端连接对象指针
	BOOL UnSubscibe( DWORD dwMsg , __BasicConnection3< LPCSTR , int , BOOL & > * pConnect )
	{
		if( dwMsg >= 0 && dwMsg < MAX_MESSAGE )
		{
			m_Sender[ dwMsg ].DelConnection( pConnect );
			return TRUE;
		}

		return FALSE;		
	}

	// 派送消息号为dwMsg的所有消息给订阅用户
	void Emit( DWORD dwMsg , LPCSTR buf , int nLen , BOOL & bReturn )
	{
		ASSERT( dwMsg >= 0 && dwMsg < MAX_MESSAGE );
		m_Sender[ dwMsg ].Emit( buf , nLen , bReturn );
	}

protected:
	__SignalSender3< LPCSTR , int , BOOL & >  m_Sender[ MAX_MESSAGE ];
};

//------------------------------------------------------------------------
// 接受端连接对象
// TYPE	:	接收类的类名
template< class TYPE >
class CMessageConnection : public __SignalConnection3< TYPE , LPCSTR , int , BOOL & >
{
public:
	CMessageConnection( TYPE * p , void ( TYPE::*member_fun)(LPCSTR , int , BOOL & ) )
	{
		m_pConnection = p;
		m_ConnectFun = member_fun;
	}

	CMessageConnection()
	{
		m_pConnection = 0;
		m_ConnectFun = 0;
	}

	// 设置回调方式 p为连接端this指针 , member_fun为接受消息的成员函数
	// member_fun的定义为 void member_fun(LPCSTR buf, int nLen, BOOL & bReturn )
	// 例: 	m_Connect.SetCallBack( this , &CActor::Accept );
	void SetCallBack( TYPE * p , void ( TYPE::*member_fun)(LPCSTR , int , BOOL & ) )
	{
		m_pConnection = p;
		m_ConnectFun = member_fun;
	}
};
//------------------------------------------------------------------------