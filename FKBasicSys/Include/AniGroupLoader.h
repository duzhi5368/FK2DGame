/**
*	created:		2012-7-2   1:17
*	filename: 		AniGroupLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Thread/IThreadRoute.h"
//------------------------------------------------------------------------
class CAniGroup;
//------------------------------------------------------------------------
struct stAniGroupNode
{
	DWORD		dwResID;
	CAniGroup	*pAniGroup;
	int			nState;
};
//------------------------------------------------------------------------
class CAniGroupLoader : public IEventSink
{
public:
	CAniGroupLoader();
	virtual ~CAniGroupLoader();

	virtual void OnEvent(DWORD dwEventID);

	BOOL	Create();
	void	AddNodeLoad(stAniGroupNode node);
	void	RemoveNode(DWORD dwResID);
	void	LoadRes(stAniGroupNode node);
	
private:
	CThreadRouteHelper		m_Thread;
	HANDLE					m_hEventWakeDecode;
	CCriticalSectionLock	m_NodeListLock;
	CCriticalSectionLock	m_NodeLoadLock;

	CList<stAniGroupNode,stAniGroupNode&> m_NodeList;
	DWORD					m_dwCurLoadResID;
};
//------------------------------------------------------------------------	