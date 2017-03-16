/**
*	created:		2012-6-30   21:18
*	filename: 		EventServer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		事件系统接口头文件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Event/IEvent.h"
#include "../../FKTrace/IFKTrace.h"
#include "EventServerTemplate.h"
//------------------------------------------------------------------------
extern CFKTrace		g_DefaultTrace;
//------------------------------------------------------------------------
struct OnVote
{
	BOOL operator() (IEventVoteListener *pListener, IEvent *pEvent)
	{
		if (pListener == NULL)
		{
			return FALSE;
		}
		return pListener->OnVote(pEvent);		
	}
};
//------------------------------------------------------------------------
struct OnAction
{
	BOOL operator() (IEventActionListener *pListener, IEvent *pEvent)
	{
		if (pListener == NULL)
		{
			return FALSE;
		}
		pListener->OnAction(pEvent);		

		return TRUE;
	}
};
//------------------------------------------------------------------------
struct OnResponse
{
	BOOL operator() (IEventResponseListener *pListener, IEvent *pEvent)
	{
		if (pListener == NULL)
		{
			return FALSE;
		}
		pListener->OnResponse(pEvent);		

		return TRUE;
	}
};
//------------------------------------------------------------------------
struct IResMgr;
struct IResObject;
//------------------------------------------------------------------------
class CEventServer : public IEventServer
{
public:
	CEventServer();
	virtual ~CEventServer();
public:
	virtual BOOL FireVote(IEvent *pEvent);
	virtual BOOL FireVote(BYTE *pContext, int nLen,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer);
	virtual void FireAction(IEvent *pEvent);
	virtual void FireAction(BYTE *pContext, int nLen,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer);
	virtual void FireResponse(IEvent *pEvent);
	virtual void FireResponse(BYTE *pContext, int nLen,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer);
	virtual BOOL AddListener(IEventVoteListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0,
		const char *pDescription = NULL);
	virtual BOOL AddListener(IEventActionListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0,
		const char *pDescription = NULL);
	virtual BOOL AddListener(IEventResponseListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0,
		const char *pDescription = NULL);
	virtual BOOL RemoveListener(IEventVoteListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0);
	virtual BOOL RemoveListener(IEventActionListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0);		
	virtual BOOL RemoveListener(IEventResponseListener *pListener,
		WORD wEventID, BYTE bSrcTypeID, DWORD dwSrcPointer = 0);
protected:
	virtual void Release();
public:	
	BOOL Create(IResMgr *pResMgr, const char *pFileName, BOOL bEncrypt);
private:
	//解析脚本,将类型ID填充到m_bTypeIDStructure中
	BOOL Parse(IResObject *pResObject, int nParentTypeID);
private:
	class CMiniEvent : public IEvent
	{		
	public:
		CMiniEvent():m_wID(0),m_bSrcTypeID(0),m_dwSrcPointer(0),m_pContext(NULL),m_nLen(0){}
		virtual WORD GetID(){return m_wID;}	
		virtual BYTE GetSrcTypeID(){return m_bSrcTypeID;}
		virtual DWORD GetSrcPointer(){return m_dwSrcPointer;}
		virtual BYTE* GetContext(int &nLen){nLen = m_nLen; return m_pContext;}
		void SetParams(BYTE *pContext, int nLen, WORD wID, BYTE bSrcTypeID, DWORD dwSrcPointer)
		{
			m_pContext = pContext;
			m_nLen = nLen;
			m_wID = wID;
			m_bSrcTypeID = bSrcTypeID;
			m_dwSrcPointer = dwSrcPointer;
		}
	private:
		WORD m_wID;
		BYTE m_bSrcTypeID;
		DWORD m_dwSrcPointer;
		BYTE *m_pContext;
		int m_nLen;
	};
private:
	typedef TEventServer< IEventVoteListener, OnVote > VOTE_SERVER;
	VOTE_SERVER m_VoteServer;
	typedef TEventServer< IEventActionListener, OnAction > ACTION_SERVER;
	ACTION_SERVER m_ActionServer;
	typedef TEventServer< IEventResponseListener, OnResponse > RESPONSE_SERVER;
	RESPONSE_SERVER m_ResponseServer;

	enum
	{
		MAX_TYPE_ID = 255,
	};
	BYTE m_bTypeIDStructure[MAX_TYPE_ID];	//保存类型ID 的结构										 
	BYTE m_bTypeIDCheck[MAX_TYPE_ID];		//用来检测类型ID是否重复	
};
//------------------------------------------------------------------------