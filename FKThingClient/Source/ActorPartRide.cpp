/**
*	created:		2012-7-4   0:40
*	filename: 		ActorPartRide
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		人物骑乘部件
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/ActorPartRide.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureViewWrapper.h"
#include "../../FKGame/Common/ThingPart.h"
//------------------------------------------------------------------------
CActorPart_Ride::CActorPart_Ride(void)
{
	m_pMaster = NULL;	
	m_listHorse.clear();
	m_nCurMountIndex = 0;
	m_nOldPart1_no = 0;
	m_nOldPart2_no = 0;
}
//------------------------------------------------------------------------
CActorPart_Ride::~CActorPart_Ride(void)
{
	
}
//------------------------------------------------------------------------
BOOL CActorPart_Ride::Create(IThing * pMaster, char * pContext, int nLen)
{
	if(pMaster == NULL || pMaster->GetThingClass()->GetThingClass() != THINGCLASS_ACTOR)
	{
		return FALSE;
	}

	m_pMaster = (IActor *)pMaster;

	// 订阅事件
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer == NULL)
	{			
		return FALSE;
	}

	/*
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_MOUNT);
	pEventServer->AddListener(static_cast<IEventActionListener *>(this),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  (DWORD)m_pMaster,
							  "CActorPart_Ride::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DISMOUNT);
	pEventServer->AddListener(static_cast<IEventActionListener *>(this),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  (DWORD)m_pMaster,
							  "CActorPart_Ride::Create");

	wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_GETRIDEDATA);
	pEventServer->AddListener(static_cast<IEventActionListener *>(this),
							  wMsgID,
							  SRC_TYPE_ID_ACTOR,
							  (DWORD)m_pMaster,
							  "CActorPart_Ride::Create");
	*/
	return TRUE;
}
//------------------------------------------------------------------------
void CActorPart_Ride::Release(void)
{
	// 订阅事件
	IEventServer * pEventServer = g_pThingClientFacade->GetEventServer();
	if(pEventServer != NULL)
	{		
		/*
		WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_MOUNT);
		pEventServer->RemoveListener(static_cast<IEventActionListener *>(this),
									 wMsgID,
									 SRC_TYPE_ID_ACTOR,
									 (DWORD)m_pMaster);

		wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DISMOUNT);
		pEventServer->RemoveListener(static_cast<IEventActionListener *>(this),
									 wMsgID,
									 SRC_TYPE_ID_ACTOR,
									 (DWORD)m_pMaster);

		wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_GETRIDEDATA);
		pEventServer->RemoveListener(static_cast<IEventActionListener *>(this),
									 wMsgID,
									 SRC_TYPE_ID_ACTOR,
									 (DWORD)m_pMaster);
		*/
	}

	delete this;
}
//------------------------------------------------------------------------
DWORD CActorPart_Ride::GetPartID(void)
{
	return THINGPARTID_ACTOR_RIDE;
}
//------------------------------------------------------------------------
IThing * CActorPart_Ride::GetMaster(void)
{
	return m_pMaster;
}
//------------------------------------------------------------------------
BOOL CActorPart_Ride::Activation(LPCSTR pContext, int nLen)
{	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CActorPart_Ride::Freeze(void)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CActorPart_Ride::OnGetPrivateContext(LPSTR buf, int &nLen)
{
	nLen = 0;
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CActorPart_Ride::OnGetPublicContext(LPSTR buf, int &nLen)
{
	nLen = 0;
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CActorPart_Ride::OnGetDBContext(LPSTR buf, int &nLen)
{
	nLen = 0;
	return FALSE;
}
//------------------------------------------------------------------------
int CActorPart_Ride::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	switch(dwMsg)
	{
	case PART_MESSAGE_RIDE_SHOW:
		{
			OnMessage_Show(buf, nLen);
		}
		break;
	default:break;
	}

	return 0;
}
//------------------------------------------------------------------------
BOOL CActorPart_Ride::InitClient(UID uidClient)
{
	return TRUE;
}
//------------------------------------------------------------------------
IHorse * CActorPart_Ride::GetHorse(int nIndex)
{
	TLIST_HORSE::iterator it = m_listHorse.begin();
	for( ; it != m_listHorse.end(); ++it)
	{
		HORSEINFOR * pHorseInfor = (HORSEINFOR *)(&(*it));
		if(pHorseInfor != NULL && pHorseInfor->nIndex == nIndex)
		{
			return (IHorse *)(&pHorseInfor->horse);
		}
	}

	return NULL;
}
//------------------------------------------------------------------------
BOOL CActorPart_Ride::IsRiding(void)
{
	return m_nCurMountIndex == 0 ? FALSE : TRUE;
}
//------------------------------------------------------------------------
void CActorPart_Ride::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	switch(pEvent->GetID())
	{
	case MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_MOUNT):
		{
			OnAction_Mount(pEvent);
		}
		break;
	case MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_DISMOUNT):
		{
			OnAction_Dismount(pEvent);
		}
		break;
	case MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_GETRIDEDATA):
		{
			OnAction_GetRideData(pEvent);
		}
		break;
	default:break;
	}
}
//------------------------------------------------------------------------
void CActorPart_Ride::OnAction_Mount(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	int nContextLen = 0;
	char * pContext = (char *)pEvent->GetContext(nContextLen);
	if(nContextLen != sizeof(SC_MSG_MOUNT_CONTEXT))
	{
		return;
	}
	SC_MSG_MOUNT_CONTEXT * _pContext = (SC_MSG_MOUNT_CONTEXT *)(pContext);

	IHorse * pHorse = GetHorse(_pContext->nIndex);
	if(pHorse != NULL)
	{
		SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA data;
		data.nIndex = _pContext->nIndex;
		data.nID = pHorse->GetID();
		
		// 骑上马
		Mount(data);
	}
}
//------------------------------------------------------------------------
void CActorPart_Ride::OnAction_Dismount(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	// 下马
	Dismount();
}
//------------------------------------------------------------------------
void CActorPart_Ride::OnAction_GetRideData(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	// 清零
	int nOldMountIndex = m_nCurMountIndex;
	m_listHorse.clear();
	m_nCurMountIndex = 0;

	int nContextLen = 0;
	char * pContext = (char *)(pEvent->GetContext(nContextLen));
	if(nContextLen < sizeof(SC_MSG_GETRIDEDATA_CONTEXT))
	{
		return;
	}

	SC_MSG_GETRIDEDATA_CONTEXT * _pContext = (SC_MSG_GETRIDEDATA_CONTEXT *)(pContext);
	m_nCurMountIndex = _pContext->nCurMountIndex;
	int nHorseCount = _pContext->nHorseCount;

	// 马匹数据
	char * pData = (pContext + sizeof(SC_MSG_GETRIDEDATA_CONTEXT));
	for(int i = 0; i < nHorseCount; i++)
	{
		SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA * pHorseData = (SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA *)(pData + i * sizeof(SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA));

		HORSEINFOR horseinfor;
		horseinfor.nIndex = pHorseData->nIndex;
		horseinfor.horse.SetID(pHorseData->nID);

		m_listHorse.push_back(horseinfor);
	}

	// 如果是第一次，则更新形像
	if(nOldMountIndex == 0 && m_nCurMountIndex != 0)
	{
		IHorse * pHorse = GetHorse(m_nCurMountIndex);
		if(pHorse != NULL)
		{
			SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA data;
			data.nIndex = m_nCurMountIndex;
			data.nID = pHorse->GetID();

			// 骑上马
			Mount(data);
		}
	}
}
//------------------------------------------------------------------------
void CActorPart_Ride::Mount(SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA horse)
{
	IConfigDataCenter * pDataCenter = g_pThingClientFacade->GetConfigDataCenter();
	if(pDataCenter == NULL)
	{
		return;
	}

	HORSE_CONFIG_INFOR * pHorseConfig = pDataCenter->GetHorseInfor(horse.nID);
	if(pHorseConfig == NULL)
	{
		return;
	}

	m_nCurMountIndex = horse.nIndex;
		
	// 先传入资源ID
	CCreatureViewWrapper viewwrapper(m_pMaster->GetMapItem());
	
	/*
	// 占领城后的氏族的氏族族长，在骑乘后，会获得一个特殊颜色的坐骑
	IThingPart * pThingPart = m_pMaster->GetPart(THINGPARTID_ACTOR_CLANPATE);
	BOOL bCastellanShaikh = FALSE;
	if(pThingPart != NULL)
	{
		IClanClientPate * pClanClientPart = (IClanClientPate *)(pThingPart);
		if(pClanClientPart->IsClanShaikh())
		{
			long lOccupation = m_pMaster->GetPropNum(CREATURE_PROPID_OCCUPATION);
			SCASTELLANGOWN * pCastellanGown = NULL;
			if(pClanClientPart->IsCastellanClan(HX_CASTLE))
			{
				pCastellanGown = pDataCenter->GetCastellanGown(HX_CASTLE, lOccupation);	
				
			}
			else if(pClanClientPart->IsCastellanClan(BG_CASTLE))
			{
				pCastellanGown = pDataCenter->GetCastellanGown(BG_CASTLE, lOccupation);	
			}
			else if(pClanClientPart->IsCastellanClan(JY_CASTLE))
			{
				pCastellanGown = pDataCenter->GetCastellanGown(JY_CASTLE, lOccupation);					
			}

			if(pCastellanGown != NULL)
			{
				bCastellanShaikh = TRUE;

				// 取得性别
				long lSex = m_pMaster->GetPropNum(CREATURE_PROPID_SEX);
				if(pCastellanGown->lHorsePartID1 != 0)
				{
					m_nOldPart1_no = pCastellanGown->lHorsePartID1;

					long lHorseResID = lSex == ACTOR_SEX_MAN ? pCastellanGown->lManHorsePartResID1 : pCastellanGown->lWomanHorsePartResID1;
					long lHorsePalID = lSex == ACTOR_SEX_MAN ? pCastellanGown->lManHorsePartPalID1 : pCastellanGown->lWomanHorsePartPalID1;
					
					viewwrapper.ChangeModulePal(pCastellanGown->lHorsePartID1, lHorsePalID);
					viewwrapper.ChangeModule(pCastellanGown->lHorsePartID1, lHorseResID);					
				}

				if(pCastellanGown->lHorsePartID2 != 0)
				{
					m_nOldPart2_no = pCastellanGown->lHorsePartID2;

					long lHorseResID = lSex == ACTOR_SEX_MAN ? pCastellanGown->lManHorsePartResID2 : pCastellanGown->lWomanHorsePartResID2;
					long lHorsePalID = lSex == ACTOR_SEX_MAN ? pCastellanGown->lManHorsePartPalID2 : pCastellanGown->lWomanHorsePartPalID2;
					
					viewwrapper.ChangeModulePal(pCastellanGown->lHorsePartID2, lHorsePalID);
					viewwrapper.ChangeModule(pCastellanGown->lHorsePartID2, lHorseResID);
				}				
			}
		}
	}
	

	if(!bCastellanShaikh)
	{
		if(pHorseConfig->lpart1_no != 0)
		{
			m_nOldPart1_no = pHorseConfig->lpart1_no;
			viewwrapper.ChangeModulePal(pHorseConfig->lpart1_no, pHorseConfig->lpart1_pal);
			viewwrapper.ChangeModule(pHorseConfig->lpart1_no, pHorseConfig->lpart1_resid);
		}

		if(pHorseConfig->lpart2_no != 0)
		{
			m_nOldPart2_no = pHorseConfig->lpart2_no;
			viewwrapper.ChangeModulePal(pHorseConfig->lpart2_no, pHorseConfig->lpart2_pal);
			viewwrapper.ChangeModule(pHorseConfig->lpart2_no, pHorseConfig->lpart2_resid);
		}
	}
	*/
	// 改变人物移动方式
	viewwrapper.SetMoveStyle(ActMoveStyleRide);
}
//------------------------------------------------------------------------
void CActorPart_Ride::Dismount(void)
{
	m_nCurMountIndex = 0;

	// 还原图包
	CCreatureViewWrapper viewwrapper(m_pMaster->GetMapItem());
	viewwrapper.ChangeModulePal(m_nOldPart1_no, 0);
	viewwrapper.ChangeModule(m_nOldPart1_no, 0);
	viewwrapper.ChangeModulePal(m_nOldPart2_no, 0);
	viewwrapper.ChangeModule(m_nOldPart2_no, 0);

	// 更改移动类型
	int nWalkStyle = m_pMaster->GetPropNum(CREATURE_PROPID_WALKSTYLE);
	if(nWalkStyle == CREATURE_MOVESTYLE_SIT)
	{
		viewwrapper.Sit();
	}
	else if(nWalkStyle == CREATURE_MOVESTYLE_WALK)
	{
		viewwrapper.SetMoveStyle(ActMoveStyleWalk);
	}
	else
	{		
		viewwrapper.SetMoveStyle(ActMoveStyleRun);
	}
}
//------------------------------------------------------------------------
void CActorPart_Ride::OnMessage_Show(LPCSTR buf, int nLen)
{
	if(m_nCurMountIndex != 0)
	{
		IHorse * pHorse = GetHorse(m_nCurMountIndex);
		if(pHorse != NULL)
		{
			SC_MSG_GETRIDEDATA_CONTEXT::HORSEDATA data;
			data.nIndex = m_nCurMountIndex;
			data.nID = pHorse->GetID();

			// 骑上马
			Mount(data);
		}
	}
}
//------------------------------------------------------------------------