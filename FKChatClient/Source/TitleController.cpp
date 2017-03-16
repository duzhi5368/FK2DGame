/**
*	created:		2012-6-28   9:19
*	filename: 		TitleController
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/TitleClient.h"
#include "../Include/TitleController.h"
#include "../Include/ChatClient.h"
#include "../../FKGame/DataArchive/IDataArchive.h"
#include "../../MsgCommon/MsgKeySubAction.h"
#include "../../MsgCommon/MsgKeySystem.h"
#include "../../FKGame/WndSys/WndDef.h"
//------------------------------------------------------------------------
CTitleController::CTitleController()
{

}
//------------------------------------------------------------------------
CTitleController::~CTitleController()
{

}
//------------------------------------------------------------------------
// 获取称号
void CTitleController::GetTitle()
{
	//char buf[256];
	//int nLen = sizeof(buf);
	//CDataArchive archive(buf, nLen);

	//CS_ACTION_HEAD actionhead;	
	//actionhead.m_wKeyRoot = CMD_ROOT_THING;
	//actionhead.m_wKeyMain = ROOTTHING_CS_MAIN_ACTION;
	//actionhead.m_wKeySub  = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_TITLE);

	//// 头
	//archive.Write(&actionhead, sizeof(CS_ACTION_HEAD));

	//// 命令
	//int nCmd = TITLE_CMD_GET;
	//archive.Write(&nCmd, sizeof(int));

	//// 发送
	//g_pGlobal->m_pAppNetworkLink->Send(buf, archive.GetBufferOffset());    
}
//------------------------------------------------------------------------
// 使用称号
void CTitleController::UserTitle(long lTitleId)
{
	//char buf[256];
	//int nLen = sizeof(buf);
	//CDataArchive archive(buf, nLen);

	//CS_ACTION_HEAD actionhead;	
	//actionhead.m_wKeyRoot = CMD_ROOT_THING;
	//actionhead.m_wKeyMain = ROOTTHING_CS_MAIN_ACTION;
	//actionhead.m_wKeySub  = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_TITLE);

	//// 头
	//archive.Write(&actionhead, sizeof(CS_ACTION_HEAD));

	//// 命令
	//int nCmd = TITLE_CMD_USE;
	//archive.Write(&nCmd, sizeof(int));

	//// 称号ID
	//archive.Write(&lTitleId, sizeof(long));

	//// 发送
	//g_pGlobal->m_pAppNetworkLink->Send(buf, archive.GetBufferOffset()); 	
}
//------------------------------------------------------------------------
// 不使用称号
void CTitleController::UnUseTitle(long lTitleId)
{
	//char buf[256];
	//int nLen = sizeof(buf);
	//CDataArchive archive(buf, nLen);

	//CS_ACTION_HEAD actionhead;	
	//actionhead.m_wKeyRoot = CMD_ROOT_THING;
	//actionhead.m_wKeyMain = ROOTTHING_CS_MAIN_ACTION;
	//actionhead.m_wKeySub  = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_TITLE);

	//// 头
	//archive.Write(&actionhead, sizeof(CS_ACTION_HEAD));

	//// 命令
	//int nCmd = TITLE_CMD_UNUSE;
	//archive.Write(&nCmd, sizeof(int));

	//// 称号ID
	//archive.Write(&lTitleId, sizeof(long));

	//// 发送
	//g_pGlobal->m_pAppNetworkLink->Send(buf, archive.GetBufferOffset()); 
}
//------------------------------------------------------------------------
// 删除称号
void CTitleController::DeleteTitle(long lTitleId)
{
	//char buf[256];
	//int nLen = sizeof(buf);
	//CDataArchive archive(buf, nLen);

	//CS_ACTION_HEAD actionhead;	
	//actionhead.m_wKeyRoot = CMD_ROOT_THING;
	//actionhead.m_wKeyMain = ROOTTHING_CS_MAIN_ACTION;
	//actionhead.m_wKeySub  = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_TITLE);

	//// 头
	//archive.Write(&actionhead, sizeof(CS_ACTION_HEAD));

	//// 命令
	//int nCmd = TITLE_CMD_DELETE;
	//archive.Write(&nCmd, sizeof(int));

	//// 称号ID
	//archive.Write(&lTitleId, sizeof(long));

	//// 发送
	//g_pGlobal->m_pAppNetworkLink->Send(buf, archive.GetBufferOffset()); 	
}
//------------------------------------------------------------------------
// 获取其它人的称号
void CTitleController::GetOtherTitle(UID uid)
{
	//char buf[256];
	//int nLen = sizeof(buf);
	//CDataArchive archive(buf, nLen);

	//CS_ACTION_HEAD actionhead;	
	//actionhead.m_wKeyRoot = CMD_ROOT_THING;
	//actionhead.m_wKeyMain = ROOTTHING_CS_MAIN_ACTION;
	//actionhead.m_wKeySub  = MAKE_MSGID(MAINACTION_CIRCULTYPE_CSC, MAINACTION_MSGID_TITLE);

	//// 头
	//archive.Write(&actionhead, sizeof(CS_ACTION_HEAD));

	//// 命令
	//int nCmd = TITLE_CMD_GETOTHER;
	//archive.Write(&nCmd, sizeof(int));

	//// UID
	//archive.Write(&uid, sizeof(UID));

	//// 发送
	//g_pGlobal->m_pAppNetworkLink->Send(buf, archive.GetBufferOffset());
}
//------------------------------------------------------------------------
void CTitleController::OnAction(IEvent *pEvent)
{
	typedef void (CTitleController::*TITLE_PROC)(LPCSTR, int);
	static TITLE_PROC __proc__[TITLE_CMD_MAX] = {
		&CTitleController::GetTitleResponse,
		&CTitleController::UseTitleResponse,
		&CTitleController::UnUseTitleResponse,
		&CTitleController::DeleteTitleResponse,
		&CTitleController::GetOtherTitleResponse,
		&CTitleController::TitleStatusChange,
		&CTitleController::AddTitleResponse
	};		

	// 先判断是什么消息
	WORD  wMsgID = pEvent->GetID();
	if( wMsgID == MAKE_MSGID(MAINACTION_CIRCULTYPE_CC, MAINACTION_MSGID_BORN))
	{
		// 如果是人物出生消息，去获取这个人的称号
		IActor *pActor = (IActor *)pEvent->GetSrcPointer();
		if(pActor != NULL)
		{
			GetOtherTitle(pActor->GetUID());
		}
		return;
	}else if(wMsgID == MSG_SYSTEM_CLIENT_BACKSELEHERO)
	{
		// 小退:
		g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,404,0,0);
		return;
	}
	// 如果是称号消息
	int nLen;
	char* pBuf = (char*)pEvent->GetContext(nLen);
	if (pBuf == NULL
		|| nLen < sizeof(int))
	{
		return;
	}	

	// 得到操作命令
	int nCmd = *(int*)pBuf;	
	pBuf += sizeof(int);
	nLen -= sizeof(int);
	if (nCmd < 0 || nCmd >= TITLE_CMD_MAX)
	{
		return;
	}
	// 处理
	if (__proc__[nCmd] != NULL)
	{
		(this->*__proc__[nCmd])(pBuf, nLen);
	}
}
//------------------------------------------------------------------------
BOOL CTitleController::OnReceived(const char* pData, int nLen)
{
	g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,404,0,0);
	return TRUE;   
}
//------------------------------------------------------------------------
// 获取称号返回
void CTitleController::GetTitleResponse(LPCSTR pBuf,int nLen)
{
	if (nLen < sizeof(int))
	{
		return;
	}
	// 称号个数
	int  nSize = *(int*)pBuf;
	pBuf += sizeof(int);
	nLen -= sizeof(int);

	for(int i=0;i<nSize;i++)
	{
		if( nLen < sizeof(SActorTitle2)) 
		{
			break;
		}
		g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,400,(WPARAM)pBuf,0);
		pBuf += sizeof(SActorTitle2);
		nLen -= sizeof(SActorTitle2);

	}
	// 显示称号窗口
	g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,406,0,0);
}
//------------------------------------------------------------------------
// 使用称号返回
void CTitleController::UseTitleResponse(LPCSTR pBuf,int nLen)
{
	// 称号ID;
	if (nLen < sizeof(int)+sizeof(int))
	{
		return;
	}	
	long  lTitleId = *(long*)pBuf;
	pBuf += sizeof(long);
	nLen -= sizeof(long);

	int nTrue = *(int*)pBuf; 
	pBuf += sizeof(int);
	nLen -= sizeof(int);

	if( nTrue )
	{
		g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,402,(WPARAM)lTitleId,0);
	}
}
//------------------------------------------------------------------------
// 不使用称号
void CTitleController::UnUseTitleResponse(LPCSTR pBuf,int nLen)
{
	// 称号ID;
	if (nLen < sizeof(int)+sizeof(int))
	{
		return;
	}	
	long  lTitleId = *(long*)pBuf;
	pBuf += sizeof(long);
	nLen -= sizeof(long);

	int nTrue = *(int*)pBuf; 
	pBuf += sizeof(int);
	nLen -= sizeof(int);

	if( nTrue )
	{
		g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,403,(WPARAM)lTitleId,0);
	}
}
//------------------------------------------------------------------------
//删除称号响应
void CTitleController::DeleteTitleResponse(LPCSTR pBuf,int nLen)
{
	// 称号ID;
	if (nLen < sizeof(int)+sizeof(int))
	{
		return;
	}	
	long  lTitleId = *(long*)pBuf;
	pBuf += sizeof(long);
	nLen -= sizeof(long);

	int nTrue = *(int*)pBuf; 
	pBuf += sizeof(int);
	nLen -= sizeof(int);

	if( nTrue )
	{
		g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,401,(WPARAM)lTitleId,0);
	}
}
//------------------------------------------------------------------------
void CTitleController::GetOtherTitleResponse(LPCSTR pBuf,int nLen)
{
	//消息格式玩家UID+称号类型+称号名字
	if( nLen < sizeof(UID)+sizeof(long))
	{
		return;
	}

	UID uid = *(UID *)pBuf;
	pBuf += sizeof(UID);
	nLen -= sizeof(UID);

	long lTitleId = *(long *)pBuf;
	pBuf += sizeof(long);
	nLen -= sizeof(long);
	if( lTitleId <0 )
	{
		CTitleClient::Instance()->SetPlayerTitle(uid,lTitleId);
	}else{
		if( nLen < 32)
		{
			return;
		}
		CTitleClient::Instance()->SetPlayerTitle(uid,lTitleId,pBuf);
	}
}
//------------------------------------------------------------------------
// 状态改变通知
void CTitleController::TitleStatusChange(LPCSTR pBuf,int nLen)
{
	if(nLen<sizeof(long)+sizeof(int))
	{
		return;
	}

	long lTitleId = *(long *)pBuf;
	pBuf +=sizeof(long);
	nLen -=sizeof(long);

	int nStatus = *(int *)pBuf;
	pBuf +=sizeof(int);
	nLen -=sizeof(int);
	g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,407,(WPARAM)lTitleId,nStatus);   
}
//------------------------------------------------------------------------
// 增加一个称号
void CTitleController::AddTitleResponse(LPCSTR pBuf,int nLen)
{
	if (nLen < sizeof(int))
	{
		return;
	}
	// 称号个数
	int  nSize = *(int*)pBuf;
	pBuf += sizeof(int);
	nLen -= sizeof(int);

	for(int i=0;i<nSize;i++)
	{
		if( nLen < sizeof(SActorTitle2)) 
		{
			break;
		}
		g_pGlobal->m_pWndMgr->OnEvent(TITLE_WND,400,(WPARAM)pBuf,0);
		pBuf += sizeof(SActorTitle2);
		nLen -= sizeof(SActorTitle2);

	}
}
//------------------------------------------------------------------------