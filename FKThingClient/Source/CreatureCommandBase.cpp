/**
*	created:		2012-7-3   2:26
*	filename: 		CreatureCommandBase
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../MsgCommon/MsgKeySubAction.h"
#include "../Include/CCreatureActStateMachine.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureCommandBase.h"
#include "../Include/CreatureViewWrapper.h"
#include "../Include/ThingClientCommon.h"
#include "../../MsgCommon/MsgFormat.h"
#include "../../FKGame/Network/IAppNetworkLink.h"
//------------------------------------------------------------------------
INSTANTIATION_SINGLE_COMMAND(CCreatureCommand_Null)
INSTANTIATION_SINGLE_COMMAND(CCreatureCommand_Stand)
INSTANTIATION_SINGLE_COMMAND(CCreatureCommand_ChanageMoveStyle)
//------------------------------------------------------------------------
CCreatureCommand_Null::CCreatureCommand_Null(void)
{

}
//------------------------------------------------------------------------
DWORD CCreatureCommand_Null::GetCommandID(void)
{
	return CREATURE_COMMANDID_INVALID;
}
//------------------------------------------------------------------------
int CCreatureCommand_Null::OnCommand(ICreature * pMaster, LPCSTR pContext, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
CCreatureCommand_Stand::CCreatureCommand_Stand(void)
{

}
//------------------------------------------------------------------------
DWORD CCreatureCommand_Stand::GetCommandID(void)
{
	return CREATURE_COMMANDID_STAND;
}
//------------------------------------------------------------------------
int CCreatureCommand_Stand::OnCommand(ICreature * pMaster, LPCSTR pContext, int nLen)
{	
	if(pMaster == NULL && pMaster == g_pGlobal->m_pPlayer)
	{
		return FALSE;
	}

	CCreatureViewWrapper creatureviewwrapper(pMaster->GetMapItem());

	// 转到站立状态
	pMaster->GotoActState(CREATURE_STATEID_STAND);

	// 让显示层停止
	POINTS ptCur = pMaster->GetLoc();
	POINT  ptPos = { ptCur.x, ptCur.y };
	creatureviewwrapper.Stand(ptPos);

	// 发给服务器验证
	WORD wMsgID = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_STAND);
	CS_ACTION_HEAD head(wMsgID);
	g_pGlobal->m_pAppNetworkLink->Send((LPCSTR)&head, sizeof(CS_ACTION_HEAD));

	return TRUE;
}
//------------------------------------------------------------------------
CCreatureCommand_ChanageMoveStyle::CCreatureCommand_ChanageMoveStyle(void)
{

}
//------------------------------------------------------------------------
DWORD CCreatureCommand_ChanageMoveStyle::GetCommandID(void)
{
	return CREATURE_COMMANDID_CHANGEMOVESTYLE;
}
//------------------------------------------------------------------------
int CCreatureCommand_ChanageMoveStyle::OnCommand(ICreature* pMaster, LPCSTR pContext, int nLen)
{
	if(pMaster == NULL	|| pContext == NULL || nLen != sizeof(int))
	{
		return FALSE;
	}

	// 发到服务端
	char szBuf[1024];
	CDataArchive dataarchive(szBuf, sizeof(szBuf));
	CS_ACTION_HEAD head(MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_CHANGEMOVESTYLE));
	dataarchive.Write(&head, sizeof(CS_ACTION_HEAD));	
	g_pGlobal->m_pAppNetworkLink->Send(szBuf, dataarchive.GetBufferOffset());

	return TRUE;
}
//------------------------------------------------------------------------