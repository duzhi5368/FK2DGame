/**
*	created:		2012-6-28   9:15
*	filename: 		TitleClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../IChatClient.h"
#include "../Include/SysMessageDef.h"
#include "../Include/TitleClient.h"
#include "../Include/ChatClient.h"
#include "../../MsgCommon/MsgKeySubAction.h"
#include "../../MsgCommon/MsgKeySystem.h"
#include "../../MsgCommon/MsgKeyMainThing.h"
#include "../../MsgCommon/MsgKeyRoot.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../../FKGame/WndSys/WndDef.h"
#include "../../FKGame/MapView/MapItemView.h"
#include "../../FKGame/ClientWorld/IClientWorld.h"
#include "../../FKGame/ClientWorld/IThingTable.h"
#include "../../FKGame/SceneObj/ThingDef.h"
//------------------------------------------------------------------------
CTitleClient* CTitleClient::s_pInstance = NULL;
//------------------------------------------------------------------------
CTitleClient::CTitleClient()
:m_pGlobal(NULL)
,m_pTitleWnd(NULL)
{

}
//------------------------------------------------------------------------
CTitleClient::~CTitleClient()
{

}
//------------------------------------------------------------------------
CTitleClient* CTitleClient::Instance()
{
	if( s_pInstance == NULL)
	{
		s_pInstance = new CTitleClient;
	}
	return s_pInstance;
}
//------------------------------------------------------------------------
BOOL CTitleClient::Create(IClientGlobal *pGlobal,IResScript *pRescript)
{
	m_pGlobal = pGlobal;

	// ◊∞‘ÿ≥∆∫≈
	BYTE pKey[] = "abcd";
	IResScript *pResScript1  = m_pGlobal->m_ResMgrHelper.m_pResMgr->LoadResScript("Data\\Scp\\chenghao.res", FALSE, pKey);

	if( pResScript1 == NULL )
	{
		return FALSE;
	}
	if( !m_TitleMgr.LoadScript(pResScript1) )
	{
		return FALSE;
	}
	m_pTitleWnd = new CTitleWnd;
	if( m_pTitleWnd == NULL
		|| !m_pTitleWnd->Create(pRescript->GetResObject(TITLE_WND),TITLE_WND))
	{
		return FALSE;
	}

	// ∂©‘ƒ≥∆∫≈œ˚œ¢
	IEventServer* pEventServer = (g_pGlobal == NULL) ? NULL : g_pGlobal->m_pEventServer;
	if (pEventServer == NULL)
	{
		return FALSE;
	}

	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_TITLE);
	pEventServer->AddListener(static_cast<IEventActionListener *>(&m_Controller), wMsgID, SRC_TYPE_ID_ACTOR, 0, "CTitleController::OnAction");	

	// ∂©‘ƒ–°ÕÀ
	if (pEventServer == NULL
		|| !pEventServer->AddListener(static_cast<IEventActionListener *>(&m_Controller), MSG_SYSTEM_CLIENT_BACKSELEHERO, SRC_TYPE_ID_SYSTEM,0,"CNpcTradeClient::OnAction"))
	{
		return FALSE;
	}
	// ∂©‘ƒ«–ªªµÿÕº
	IController * pController = m_pGlobal->GetController();
	if (pController == NULL 
		|| !pController->AddReceiver(static_cast<IDataReceiver *>(&m_Controller), CMD_ROOT_THING, ROOTTHING_SC_MAIN_CHANGEMAP))
	{
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
void CTitleClient::SetPlayerTitle(UID uid,long lTitleId,LPCSTR lpName)
{
	IActor *pActor = GetActor(uid);
	if( pActor == NULL)
	{
		return ;
	}

	CMapItemView *pMapItem = pActor->GetMapItem();

	if( pMapItem == NULL)
	{
		return;
	} 
	if( lTitleId == -1 )
	{
		pMapItem->OnCommand(CrtCmdClearTitle,0,0);     
		return;
	}
	//…Ë÷√≥∆∫≈√˚◊÷
	if( lpName!= NULL )
	{
		string str = lpName;
		//≤È’“,
		int nPos = str.find(",");
		if( nPos <0 )
		{
			pMapItem->OnCommand(CrtCmdClearTitle,0,0);
			pMapItem->OnCommand(CrtCmdAddTitle,(DWORD)lpName,0);
		}else{
			string strFormat = str.substr(0,nPos);
			string strEffect = str.substr(nPos+1,str.length()-nPos-1);
			int nEffectID = 0;
			if( strEffect.size() > 0)
			{
				nEffectID = atoi(strEffect.c_str());
			}
			pMapItem->OnCommand(CrtCmdClearTitle,0,0);
			pMapItem->OnCommand(CrtCmdAddTitle,(DWORD)strFormat.c_str(),nEffectID);	  
		}

	} 
}
//------------------------------------------------------------------------
IActor *CTitleClient::GetActor(UID uid)
{
	IThing *pThing = NULL;
	if( g_pGlobal->m_pClientWorld && g_pGlobal->m_pClientWorld->GetThingTable() )
	{
		pThing = g_pGlobal->m_pClientWorld->GetThingTable()->Find(uid);
		if(pThing == NULL || pThing->GetThingClass()->GetThingClass() != THINGCLASS_ACTOR)
		{
			return NULL;
		}
		return static_cast<IActor*>(pThing);
	}
	return NULL;
}
//------------------------------------------------------------------------
void CTitleClient::Speak(LPCSTR lpStr)
{
	g_pGlobal->GetChatClient()->DisplayFloatMessage(SMT_FLOAT,lpStr);
}
//------------------------------------------------------------------------
CTitleMgr& CTitleClient::GetTitleMgr()
{
	return m_TitleMgr;
}
//------------------------------------------------------------------------
CTitleController& CTitleClient::GetTitleController()
{
	return m_Controller;
}
//------------------------------------------------------------------------