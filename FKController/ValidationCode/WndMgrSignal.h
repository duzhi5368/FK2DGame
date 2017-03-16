/**
*	created:		2012-6-24   22:56
*	filename: 		WndMgrSignal
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		用来实现窗口消息的订阅
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/IWndMgr.h"
#include "../../FKFileSystem/Include/FKTempl.h"
#include <list>
using std::list;
//------------------------------------------------------------------------
#ifndef _BASIC_CONNECTION__
#define _BASIC_CONNECTION__
//------------------------------------------------------------------------
	template< class ARG1, class ARG2, class ARG3 >
	struct __BasicConnection3
	{
		virtual void Emit(ARG1, ARG2, ARG3){}
	};
	//------------------------------------------------------------------------
	template< class TYPE, class ARG1, class ARG2, class ARG3 >
	struct __SignalConnection3 : public __BasicConnection3< ARG1, ARG2, ARG3 >
	{
	public:
		typedef void (TYPE::*CONNECTFUN)(ARG1, ARG2, ARG3);
		void Emit(ARG1 a, ARG2 b, ARG3 c)
		{
			(m_pConnection->*m_ConnectFun)(a, b, c);
		}

	public:
		TYPE *m_pConnection;
		CONNECTFUN m_ConnectFun;
	};
	//------------------------------------------------------------------------
	template< class ARG1, class ARG2, class ARG3 >
	class __SignalSender3
	{
	public:
		void AddConnection(__BasicConnection3< ARG1, ARG2, ARG3 > *pConnect)
		{
			m_ConnectionList.push_back(pConnect);
			m_ConnectionList.unique();
		}
		void DelConnection(__BasicConnection3< ARG1, ARG2, ARG3 > *pConnect)
		{
			m_ConnectionList.remove(pConnect);
		}
		void Emit(ARG1 a, ARG2 b, ARG3 c)
		{
			ConnectionList::iterator it;
			for(it = m_ConnectionList.begin(); it != m_ConnectionList.end(); ++it)
			{
				(*it)->Emit(a, b, c);
			}
		}
	private:
		typedef list< __BasicConnection3< ARG1, ARG2, ARG3 >* > ConnectionList;

		ConnectionList m_ConnectionList;
	};
//------------------------------------------------------------------------
#endif //_BASIC_CONNECTION__
//------------------------------------------------------------------------
//管理消息订阅以及消息派送的类
template< int MAX_MESSAGE=16 >
class CWMMessageSender
{
public:
	//订阅消息，消息号为dwMsg, pConnect为接受端连接对象指针
	BOOL Subscibe(DWORD dwMsg, __BasicConnection3< WPARAM, LPARAM, BOOL& > *pConnect)
	{
		if (dwMsg >= 0 && dwMsg < MAX_MESSAGE)
		{
			m_Sender[dwMsg].AddConnection(pConnect);
			return TRUE;
		}
		return FALSE;
	}
	//退订消息、消息号为dwMsg, pConnect为接受端连接对象指针
	BOOL UnSubscibe(DWORD dwMsg, __BasicConnection3< WPARAM, LPARAM, BOOL& > *pConnect)
	{
		if (dwMsg >=0 && dwMsg < MAX_MESSAGE)
		{
			m_Sender[dwMsg].DelConnection(pConnect);
			return TRUE;
		}
		return FALSE;
	}
	//派发消息号为dwMsg的所有消息给订阅用户
	void Emit(DWORD dwMsg, WPARAM wParam, LPARAM lParam, BOOL &bReturn)
	{
		ASSERT(dwMsg >=0 && dwMsg < MAX_MESSAGE);
		m_Sender[dwMsg].Emit(wParam, lParam, bReturn);
	}
protected:
	__SignalSender3< WPARAM, LPARAM, BOOL& > m_Sender[MAX_MESSAGE];
};
//------------------------------------------------------------------------
//接受端连接对象
template< class TYPE >
class CWMConnection : public __SignalConnection3< TYPE, WPARAM, LPARAM, BOOL& >
{
public:
	CWMConnection():m_pWndMgr(NULL), m_uWndID(0), m_uEventID(0)
	{
	}
	CWMConnection(IWndMgr *pWndMgr, UINT uWndID, UINT uEventID, TYPE *pConnection,
		void (TYPE::*member_fun)(WPARAM, LPARAM, BOOL&))
	{
		m_pConnection = pConnection;
		m_ConnectFun = member_fun;
		m_pWndMgr = pWndMgr;
		m_uWndID = uWndID;
		m_uEventID =uEventID;

		if (pWndMgr != NULL)
		{
			pWndMgr->SubscibeMessage(uWndID, uEventID, this);
		}
	}
	virtual ~CWMConnection()
	{
	}

	//如果不想在构造的时候自动连接，则调用下面2个函数
	void Connect(IWndMgr *pWndMgr, UINT uWndID, UINT uEventID, TYPE *p,
		void (TYPE::*member_fun)(WPARAM, LPARAM, BOOL&))
	{
		m_pConnection = p;
		m_ConnectFun = member_fun;
		m_pWndMgr = pWndMgr;
		m_uWndID = uWndID;
		m_uEventID = uEventID;
		if (pWndMgr != NULL)
		{
			pWndMgr->SubscibeMessage(uWndID, uEventID, this);
		}
	}
	void UnConnect()
	{
		if (m_pWndMgr != NULL)
		{
			m_pWndMgr->UnSubscibeMessage(m_uWndID, m_uEventID, this);
			m_pWndMgr = NULL;
		}		
	}
public:
	IWndMgr *m_pWndMgr;
	UINT    m_uWndID;
	UINT	m_uEventID;
};
//------------------------------------------------------------------------