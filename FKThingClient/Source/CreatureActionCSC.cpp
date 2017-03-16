/**
*	created:		2012-7-4   0:02
*	filename: 		CreatureActionCSC
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		响应服务器发下来CSC类型消息
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../Include/CreatureActionCSC.h"
#include "../Include/IBox.h"
#include "../Include/IRidePart.h"
#include "../Include/CCreatureActStateMachine.h"
#include "../Include/CreatureActionCSC.h"
#include "../Include/CreatureViewWrapper.h"
#include "../../FKGame/Skill/ISkillClient.h"
#include "../Include/CThingClientFacade.h"
//------------------------------------------------------------------------
void CCreatureAction_Creature_Move_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	int    nLen = 0;
	char * pBuf = (char *)pEvent->GetContext(nLen);
	if(pBuf == NULL || nLen < sizeof(int) + sizeof(POINTS))
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}
	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());

	int nPathLen = *((int *)pBuf);
	pBuf += sizeof(int);
	POINTS * pPath = (POINTS *)pBuf;
	POINTS   ptCur = pCreature->GetLoc();
	
	// 长度为1的一般是服务器的校正数据
	if(nPathLen == 1)
	{
		POINT ptTile = { ((POINTS *)pBuf)->x, ((POINTS *)pBuf)->y };		
		viewwrapper.Stand(ptTile);
		return;
	}
	
	// 如果路径超长，则认为是非法的
	if(nPathLen >= MAX_PATH_LEN)
	{
		return;
	}

	// 如果误差太大直接跳过去
	if(abs(pPath[0].x - ptCur.x) > 3 || abs(pPath[0].y - ptCur.y) > 3)
	{
		viewwrapper.Move(pPath, nPathLen);
		return;
	}

	// 否则搜索一条最好的路过去
	POINTS * pPathBuffer = NULL;
	int      nPathFind = 0;
	if(g_pGlobal->m_pMapView->FindPath(ptCur, pPath[nPathLen - 1], &pPathBuffer, nPathFind))
	{
		viewwrapper.Move(pPathBuffer, nPathFind);
		return;
	}
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_ChangeLocation_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	int    nLen = 0;
	char * pBuf = (char *)pEvent->GetContext(nLen);
	if(pBuf == NULL || nLen < sizeof(POINTS))
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}
	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());		

	POINT ptTile = { ((POINTS *)pBuf)->x, ((POINTS *)pBuf)->y };
	viewwrapper.Stand(ptTile);

	// 死亡复活
	pCreature->SetDying(FALSE);

	// 让主角转到站立状态
	if(g_pGlobal->m_pPlayer == pCreature)
	{		
		pCreature->GotoActState(CREATURE_STATEID_STAND);
	}	
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_ChangeLocationForSkill_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	int nLen = 0;
	char * pBuf = (char *)pEvent->GetContext(nLen);
	if(pBuf == NULL || nLen < sizeof(POINTS))
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}
	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());		

	POINT ptTile = { ((POINTS *)pBuf)->x, ((POINTS *)pBuf)->y };
	viewwrapper.ChangeLocForSkill(ptTile);

	// 让主角转到站立状态
	if(g_pGlobal->m_pPlayer == pCreature)
	{		
		pCreature->GotoActState(CREATURE_STATEID_STAND);
	}
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_ChangeMoveStyle_CSC::OnAction(IEvent* pEvent)
{
	/*
	 * int  // 移动方式 
	 */
	if(pEvent == NULL)
	{
		return;
	}

	ICreature* pCreature = (ICreature*)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}

	int   nLen = 0;
	char* pBuf = (char *)pEvent->GetContext(nLen);
	if(pBuf == NULL || nLen != sizeof(int))
	{
		return;
	}
	int nMoveStyle = *(int*)pBuf;

	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());
	if(viewwrapper.GetState() == CrtStateSit && nMoveStyle != CREATURE_MOVESTYLE_SIT)
	{
		POINTS pts = pCreature->GetLoc();
		POINT  pt  = { pts.x, pts.y };
		viewwrapper.Stand(pt);
	}
	else if(nMoveStyle == CREATURE_MOVESTYLE_SIT)
	{
		viewwrapper.Sit();
	}

	if(nMoveStyle != CREATURE_MOVESTYLE_SIT)
	{	
		int nSetStyle = ActMoveStyleRun;
		if(nMoveStyle == CREATURE_MOVESTYLE_WALK)
		{
			nSetStyle = ActMoveStyleWalk;
		}

		viewwrapper.SetMoveStyle(nSetStyle);
	}
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_StopAttack_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}
	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());
	
	viewwrapper.StopAttack();
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_PreAttack_CSC::OnAction(IEvent * pEvent)
{
	/*消息格式	
	struct MSG_FORMAT
	{	
		SUseSkillContext *
	};//*/

	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}
	
	int	nContextLen = 0;
	char * _pContext = (char *)pEvent->GetContext(nContextLen);
	if(_pContext == NULL || nContextLen != sizeof(SUseSkillContext))
	{
		return;
	}

	SUseSkillContext *pContext = (SUseSkillContext *)_pContext;

	CCreatureView * pTargetView = NULL;
	if(pContext->uidTarget != INVALID_UID)
	{
		IThing * pThing = g_pThingClientFacade->GetThing(pContext->uidTarget);
		if(pThing == NULL)
		{
			// 地图上，找不到此物件
			// TraceLn("攻击时，地图上，找不到此物件");
			return;
		}
		else
		{
			pTargetView = (CCreatureView *)(((IMapThing *)(pThing))->GetMapItem());
		}
	}

	SSkillCreateParam context;
	context.nIndex = pContext->SkillData.wSkillID;
	context.nLevel = g_pThingClientFacade->GetSkillManager()->GetSkillInfo(pContext->SkillData, enSKILL_INFO_VIEWLEVEL);
	context.ptAttack.x = pContext->ptTargetTile.x;
	context.ptAttack.y = pContext->ptTargetTile.y;
	context.pAttackView = pTargetView;

	CCreatureViewWrapper ViewWrapper(pCreature->GetMapItem());
	ViewWrapper.PrepAttack((DWORD)&context);
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_Attack_CSC::OnAction(IEvent * pEvent)
{
		/*消息格式	
	struct MSG_FORMAT
	{	
		SUseSkillContext *
	};//*/
	if(pEvent == NULL)
	{
		return;
	}
	
	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	int	nContextLen = 0;
	char * _pContext = (char *)pEvent->GetContext(nContextLen);
	if(_pContext == NULL || nContextLen != sizeof(SUseSkillContext))
	{
		return;
	}

	SUseSkillContext *pContext = (SUseSkillContext *)_pContext;

	CCreatureView * pTargetView = NULL;
	if(pContext->uidTarget != INVALID_UID)
	{
		IThing * pThing = g_pThingClientFacade->GetThing(pContext->uidTarget);
		if(pThing == NULL)
		{
			// 地图上，找不到此物件
			// TraceLn("攻击时，地图上，找不到此物件");
			return;
		}
		else
		{
			pTargetView = (CCreatureView *)(((IMapThing *)(pThing))->GetMapItem());
		}
	}

	SSkillCreateParam context;
	context.nIndex = pContext->SkillData.wSkillID;
	context.nLevel = g_pThingClientFacade->GetSkillManager()->GetSkillInfo(pContext->SkillData, enSKILL_INFO_VIEWLEVEL);
	context.nCheckId = pContext->wCheckId;
	context.ptAttack.x = pContext->ptTargetTile.x;
	context.ptAttack.y = pContext->ptTargetTile.y;
	context.pAttackView = pTargetView;

	CCreatureViewWrapper ViewWrapper(pCreature->GetMapItem());
	ViewWrapper.Attack((DWORD)&context);
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_StopPreAttack_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}
	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());
	
	viewwrapper.StopPrepAttack();
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_Damage_Rehearsal_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}

	int	nContextLen = 0;
	SC_DAMAGE_REHEARSAL_HEAD *pContext = (SC_DAMAGE_REHEARSAL_HEAD *)(pEvent->GetContext(nContextLen));
	if(pContext == NULL || nContextLen != sizeof(SC_DAMAGE_REHEARSAL_HEAD))
	{
		return;
	}

	CCreatureView * pSourceView = NULL;
	if(pContext->uidSource != INVALID_UID)
	{
		IThing * pThing = g_pThingClientFacade->GetThing(pContext->uidSource);
		if(pThing != NULL)
			pSourceView = (CCreatureView *)(((IMapThing *)(pThing))->GetMapItem());
	}

	SBehitParam param;
	param.nCheckId = pContext->wCheckId;
	param.bDoAct = 0;
	param.FlowParam.nFlowType = (enFlowType)pContext->wFlowType;
	param.FlowParam.nHP = -pContext->nDamHP;
	param.pSourceView = pSourceView;

	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());
	viewwrapper.BeHit(&param);  	
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_Cure_Rehearsal_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}

	int	nContextLen = 0;
	SC_CURE_REHEARSAL_HEAD *pContext = (SC_CURE_REHEARSAL_HEAD *)(pEvent->GetContext(nContextLen));
	if(pContext == NULL || nContextLen != sizeof(SC_CURE_REHEARSAL_HEAD))
	{
		return;
	}

	CCreatureView * pSourceView = NULL;
	if(pContext->uidSource != INVALID_UID)
	{
		IThing * pThing = g_pThingClientFacade->GetThing(pContext->uidSource);
		if(pThing != NULL)
			pSourceView = (CCreatureView *)(((IMapThing *)(pThing))->GetMapItem());
	}

	SBehitParam param;
	param.nCheckId = pContext->wCheckId;
	param.bDoAct = 0;
	param.FlowParam.nFlowType = FlowTypeCommon;
	param.FlowParam.nHP = pContext->nCureHP;
	param.pSourceView = pSourceView;

	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());
	viewwrapper.BeHit(&param);
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_Die_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}

	// 设为死亡态
	pCreature->SetDying(TRUE);

	int	nContextLen = 0;
	char * pContext = (char *)pEvent->GetContext(nContextLen);
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_DIE);
	pCreature->OnMessage(wMsgID, pContext, nContextLen);	
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_Upgrade_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}

	// 升级动画
	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());
	viewwrapper.Upgrade();
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_BrushProData_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}

	int nLen = 0;
	char * pszContext = (char *)pEvent->GetContext(nLen);
	if(pszContext == NULL)
	{
		return;
	}

	DWORD dwDataType = *((DWORD *)(pszContext));
	pszContext += sizeof(DWORD);
	nLen -= sizeof(DWORD);

	if(dwDataType == 1)			// 1 :数字型
	{
		DWORD dwPropID = *((DWORD *)(pszContext));
		pszContext += sizeof(DWORD);
		nLen -= sizeof(DWORD);
		int nPropVal = *((int *)(pszContext));

		int nOldVal = pCreature->GetPropNum(dwPropID);
		pCreature->SetPropNum(dwPropID, nPropVal);
		int nNewVal = pCreature->GetPropNum(dwPropID);

		switch(dwPropID)
		{
		case CREATURE_PROPID_EXP_V1:
			{
				//BrushMessage_Exp(pCreature, nOldVal, nNewVal);
			}
			break;
		default:break;
		}	
	}
	else if(dwDataType == 2)	// 2 :字符型
	{
		DWORD dwPropID = *((DWORD *)(pszContext));
		pszContext += sizeof(DWORD);
		nLen -= sizeof(DWORD);

		pCreature->SetPropStr(dwPropID, pszContext);	
	}
	else if(dwDataType == 3)	// 3：全部更新
	{
		pCreature->OnSetPrivateContext(pszContext, nLen);
	}

	// 显示更新
	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());
	// 血条
	int nHPCur = pCreature->GetPropNum(CREATURE_PROPID_HPCUR);
	int nHPMax = pCreature->GetPropNum(CREATURE_PROPID_HPMAXCUR);
	viewwrapper.SetBloodInfo(nHPCur, max(nHPCur, nHPMax));

	// 移动类型
	int nWalkStyle = pCreature->GetPropNum(CREATURE_PROPID_WALKSTYLE);
	int nSetStyle = ActMoveStyleRun;
	IThingPart * pThingPart = pCreature->GetPart(THINGPARTID_ACTOR_RIDE);
	IRidePart * pRidePart = (IRidePart *)(pThingPart);
	if(pRidePart != NULL && pRidePart->IsRiding())
	{
		nSetStyle = ActMoveStyleRide;
		viewwrapper.SetMoveStyle(nSetStyle);
	}
	else if(nWalkStyle == CREATURE_MOVESTYLE_SIT)
	{
		viewwrapper.Sit();		
	}
	else
	{
		if(nWalkStyle == CREATURE_MOVESTYLE_RACE)
		{
			if(pCreature->GetThingClass()->GetThingClass() == THINGCLASS_MONSTER)
			{
				nSetStyle = ActMoveStyleRide;
			}
		}
		else if(nWalkStyle == CREATURE_MOVESTYLE_WALK)
		{
			nSetStyle = ActMoveStyleWalk;
		}
		viewwrapper.SetMoveStyle(nSetStyle);
	}
	

	// 移动速度
	int nMoveSpeed = pCreature->GetPropNum(CREATURE_PROPID_MOVESPEEDLEVEL);
	viewwrapper.SetMoveSpeedLevel(&nMoveSpeed);

	// 名字颜色（只针对玩家）
	/*if(pCreature->GetThingClass()->GetThingClass() == THINGCLASS_ACTOR)
	{
		int nNameColor = pCreature->GetPropNum(CREATURE_PROPID_NAMECOLOR);
		viewwrapper.SetNameColor((DWORD)nNameColor);
	}//*/

	// 名字
	LPCSTR pszName = pCreature->GetName();
	viewwrapper.SetName(pszName);

	// 让界面去更改
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_BRUSHPROPDATA);
	pCreature->OnResponse(wMsgID, NULL, 0);
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_BrushProData_CSC::BrushMessage_Exp(ICreature * pCreature, int nOldVal, int nNewVal)
{
	char szMsg[256];
	if(pCreature == g_pGlobal->m_pPlayer)
	{
		WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_SYSTEMMESSAGE);
		
		int nDiffVal = nNewVal - nOldVal;
		if(nDiffVal == 0)
		{
			return;
		}
		else if(nDiffVal > 0)
		{
			sprintf(szMsg, "获得经验%d", nDiffVal);
		}
		else
		{
			sprintf(szMsg, "失去经验%d", nDiffVal);
		}
		
		pCreature->OnMessage(wMsgID, szMsg, sizeof(szMsg));
	}
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_OpenBox_CSC::OnAction(IEvent * pEvent)
{
	// 盒子id+正在打开状态+打开时间
	// 盒子id+打开状态+容器的个数+容器1id+容器1大小+容器2id+容器2大小+...
	// 盒子id+关闭状态	
	if (pEvent == NULL)
	{
		return;
	}

	char* pBuf;
	int nLen;
	pBuf = (char*)pEvent->GetContext(nLen);
	if (pBuf == NULL)
	{
		return;
	}

	// 盒子id
	UID uid = *(UID*)pBuf;
	pBuf += sizeof(UID);
	nLen -= sizeof(UID);

	// 盒子对象
	IBox* pBox = (IBox*)g_pThingClientFacade->GetThing(uid);
	if (pBox == NULL)
	{
		return;
	}

	pBox->Respond(pBuf, nLen);
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_Relive_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}
	
	// 让底层复位
	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());
	viewwrapper.Born();

	// 发个响应，复活了
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_RELIVE);
	pCreature->OnResponse(wMsgID, NULL, 0);
}
//------------------------------------------------------------------------
void CCreatureAction_Creature_Stand_CSC::OnAction(IEvent * pEvent)
{
	if(pEvent == NULL)
	{
		return;
	}

	ICreature * pCreature = (ICreature *)pEvent->GetSrcPointer();
	if(pCreature == NULL)
	{
		return;
	}
	int    nLen = 0;
	char * pBuf = (char *)pEvent->GetContext(nLen);
	CCreatureViewWrapper viewwrapper(pCreature->GetMapItem());		

	POINT ptTile = { ((POINTS *)pBuf)->x, ((POINTS *)pBuf)->y };
	viewwrapper.Stand(ptTile);

	// 死亡复活
	pCreature->SetDying(FALSE);
}
//------------------------------------------------------------------------