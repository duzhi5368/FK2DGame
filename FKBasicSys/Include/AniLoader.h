/**
*	created:		2012-7-1   20:15
*	filename: 		AniLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Thread/IThreadRoute.h"
//------------------------------------------------------------------------
class CAni;
//------------------------------------------------------------------------
struct stAniNode
{
	DWORD		dwResID;
	CAni		*pAni;
};
//------------------------------------------------------------------------
class CAniLoader : public IEventSink
{
public:
	CAniLoader();
	virtual ~CAniLoader();

	virtual void OnEvent(DWORD dwEventID);

	BOOL	Create();
	void	AddNodeLoad(stAniNode node);
	void	RemoveNode(DWORD dwResID);
	void	LoadRes(stAniNode node);
	
private:
	CThreadRouteHelper		m_Thread;
	HANDLE					m_hEventWakeDecode;
	CCriticalSectionLock	m_NodeListLock;
	CCriticalSectionLock	m_NodeLoadLock;

	CList<stAniNode,stAniNode&> m_NodeList;
	DWORD					m_dwCurLoadResID;
};
//------------------------------------------------------------------------