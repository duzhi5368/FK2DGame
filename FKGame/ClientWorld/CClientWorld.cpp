/**
*	created:		2012-6-26   0:56
*	filename: 		CClientWorld
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		客户端逻辑对像管理列表
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "CClientWorld.h"
#include "../GameGlobal/ClientGlobal.h"
#include "../GameApp/CommonFunc.h"
#include "../GameApp/GameApp.h"
#include "../SceneObj/IThingFactory.h"
#include "../../MsgCommon/MsgKeyRoot.h"
#include "../../MsgCommon/MsgKeyMainThing.h"
#include "../../MsgCommon/MsgKeySystem.h"
//------------------------------------------------------------------------
BOOL CMainActionDataReceiver::OnReceived(const char* pData, int nLen)
{
	if(pData == NULL || nLen < sizeof(UID) + sizeof(WORD))
	{
		TraceLn( "Error : CMainActionDataReceiver::OnReceived() : buffer size error!");
		return FALSE;
	}

	UID uidThing = *((UID *)(pData));
	pData += sizeof(UID);
	nLen  -= sizeof(UID);

	WORD wKeySub = *((WORD *)(pData));
	pData += sizeof(WORD);
	nLen  -= sizeof(WORD);
	
	IThingTable * pThingTable = g_ClientGlobal.m_pClientWorld->GetThingTable();
	if(pThingTable == NULL)
	{
		return FALSE;
	}

	IThing * pThing = pThingTable->Find(uidThing);
	if(pThing != NULL)
	{
		pThing->OnMessage(wKeySub, pData, nLen);
	}
	else
	{
		TraceLn( "Error : CMainActionDataReceiver::OnReceived() : ROOTTHING_SC_MAIN_ACTION, can not find uid = %08X%08X, MSG=%d", uidThing, wKeySub);
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CPrivateContextDataReceiver::OnReceived(const char* pData, int nLen)
{
	if(pData == NULL || nLen < sizeof(UID) + sizeof(WORD))
	{
		TraceLn( "Error : CPrivateContextDataReceiver::OnReceived() : buffer size error!");
		return FALSE;
	}

	UID uidThing = *((UID *)(pData));
	pData += sizeof(UID);
	nLen  -= sizeof(UID);

	WORD wThingClassID = *((WORD *)(pData));
	pData += sizeof(WORD);
	nLen  -= sizeof(WORD);

	WORD wReserved = *((WORD *)(pData));
	pData += sizeof(WORD);
	nLen  -= sizeof(WORD);
	
	IThingTable * pThingTable = g_ClientGlobal.m_pClientWorld->GetThingTable();
	if(pThingTable == NULL)
	{
		return FALSE;
	}

	IThing * pThing = pThingTable->Find(uidThing);
	if(pThing != NULL)
	{
		pThing->OnSetPrivateContext(pData, nLen);
	}
	else
	{
		IThingFactory * pThingFactory = g_ClientGlobal.m_pThingFactory;
		if(pThingFactory == NULL)
		{
			return FALSE;
		}
		
		pThing = pThingFactory->CreatePrivateThing(wThingClassID, uidThing, pData, nLen);
		if(pThing != NULL)
		{
			pThingTable->Add(pThing);
		}
		else
		{
			TraceLn( "Error : CPrivateContextDataReceiver::OnReceived() : can not create uid = %08X%08X, ThingClassID = %d", uidThing, wThingClassID);
			return FALSE;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CPublicContextDataReceiver::OnReceived(const char* pData, int nLen)
{	
	if(pData == NULL || nLen < sizeof(UID) + sizeof(WORD))
	{
		TraceLn( "Error : CPublicContextDataReceiver::OnReceived() : buffer size error!");
		return FALSE;
	}

	UID uidThing = *((UID *)(pData));
	pData += sizeof(UID);
	nLen  -= sizeof(UID);

	WORD wThingClassID = *((WORD *)(pData));
	pData += sizeof(WORD);
	nLen  -= sizeof(WORD);

	WORD wReserved = *((WORD *)(pData));
	pData += sizeof(WORD);
	nLen  -= sizeof(WORD);
	
	IThingTable * pThingTable = g_ClientGlobal.m_pClientWorld->GetThingTable();
	if(pThingTable == NULL)
	{
		return FALSE;
	}

	IThing * pThing = pThingTable->Find(uidThing);

	if(pThing != NULL)
	{
		pThing->OnSetPublicContext(pData, nLen);
	}
	else
	{
		IThingFactory * pThingFactory = g_ClientGlobal.m_pThingFactory;
		if(pThingFactory == NULL)
		{
			return FALSE;
		}

		pThing = pThingFactory->CreatePublicThing(wThingClassID, uidThing, pData, nLen);
		if(pThing != NULL)
		{
			pThingTable->Add(pThing);
		}
		else
		{
			TraceLn( "Error : CPublicContextDataReceiver::OnReceived() : can not create uid = %08X%08X, ThingClassID = %d", uidThing, wThingClassID);
			return FALSE;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CChangeMapDataReceiver::OnReceived(const char* pData, int nLen)
{
	if(pData == NULL || nLen < sizeof(DWORD) + sizeof(int) + sizeof(POINT))
	{
		TraceLn( "Error : CChangeMapDataReceiver::OnReceived() : buffer size error!");
		return FALSE;
	}
	
	DWORD dwChangeMode = *((DWORD *)(pData));
	pData += sizeof(DWORD);
	nLen  -= sizeof(DWORD);

	if(dwChangeMode != 1)// 切换模式.1 : 本进程切换
	{
		return TRUE;
	}

	int nMapID = *((int *)(pData));
	pData += sizeof(int);
	nLen  -= sizeof(int);

	POINT ptWorld = *((POINT *)(pData));
	pData += sizeof(POINT);
	nLen  -= sizeof(POINT);

	DWORD dwStartTime = ::GetTickCount();
	if(g_CommonFunc.CreateNewWorld(nMapID, ptWorld) == FALSE)
	{
		char str[100];
		sprintf(str, "地图载入失败:MapID = %d, x = %d, y = %d", nMapID, ptWorld.x, ptWorld.y);
		MessageBox(g_ClientGlobal.m_hMainWnd, str, "载入地图失败",  MB_ICONWARNING | MB_OK);

		return FALSE;
	}

	TraceLn("载入地图耗时 %d", ::GetTickCount() - dwStartTime);

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CCountFeeDataReceiver::OnReceived(const char* pData, int nLen)
{
	if(pData == NULL)
	{
		TraceLn( "Error : CCountFeeDataReceiver::OnReceived() : buffer size error!");
		return FALSE;
	}

	pData += sizeof(DWORD) + sizeof(int) + sizeof(int);
	nLen  -= sizeof(DWORD) + sizeof(int) + sizeof(int);

	DWORD dwColor = WPixel(255, 210, 44);
	//g_GameApp.OnCommand(IDM_SET_CARD_INFO, (DWORD)pData, dwColor);

	return TRUE;
}
//------------------------------------------------------------------------
// 描  述：小退处理
void CReselectHeroActionListener::OnAction(IEvent * pEvent)
{
	g_CommonFunc.ClearMemoryResource();
}
//------------------------------------------------------------------------
CClientWorld::CClientWorld(void)
{	
}
//------------------------------------------------------------------------
CClientWorld::~CClientWorld(void)
{
	Close();
}
//------------------------------------------------------------------------
// 描  述：初始化
BOOL CClientWorld::Init(void)
{
	
	g_CommonFunc.m_ControllerHelper->AddReceiver(static_cast<IDataReceiver *>(&m_MainActionDataReceiver),
										  CMD_ROOT_THING,
										  ROOTTHING_SC_MAIN_ACTION);

	g_CommonFunc.m_ControllerHelper->AddReceiver(static_cast<IDataReceiver *>(&m_PrivateContextDataReceiver),
										  CMD_ROOT_THING,
										  ROOTTHING_SC_MAIN_PRIVATECONTEXT);

	g_CommonFunc.m_ControllerHelper->AddReceiver(static_cast<IDataReceiver *>(&m_PublicContextDataReceiver),
										  CMD_ROOT_THING,
										  ROOTTHING_SC_MAIN_PUBLICCONTEXT);

	g_CommonFunc.m_ControllerHelper->AddReceiver(static_cast<IDataReceiver *>(&m_ChangeMapDataReceiver),
										  CMD_ROOT_THING,
										  ROOTTHING_SC_MAIN_CHANGEMAP);

	g_CommonFunc.m_ControllerHelper->AddReceiver(static_cast<IDataReceiver *>(&m_CountFeeDataReceiver),
										  CMD_ROOT_THING,
										  ROOTTHING_SC_MAIN_COUNTFEE);

	g_ClientGlobal.m_pEventServer->AddListener(static_cast<IEventActionListener *>(&m_ReselectHeroActionListener),
								  MSG_SYSTEM_CLIENT_BACKSELEHERO,
								  SRC_TYPE_ID_SYSTEM,
								  0,
								  "CClientWorld::Create");

	
	g_ClientGlobal.m_dwLoginTimeTick = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();
	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CClientWorld::Create(void)
{
	g_ClientGlobal.m_dwLoginTimeTick = g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->GetTimeTick();

	return TRUE;
}
//------------------------------------------------------------------------
void CClientWorld::Close(void)
{
	m_ThingTable.Close();
}
//------------------------------------------------------------------------
void CClientWorld::OnMessage(DWORD dwKeyMain, LPCSTR buf, int nLen)
{		

}
//------------------------------------------------------------------------
// 取得逻辑对象管理器
IThingTable * CClientWorld::GetThingTable(void)
{
	return static_cast<IThingTable *>(&m_ThingTable);
}
//------------------------------------------------------------------------
// 描  述：取得当前静态地图ID
int	CClientWorld::GetCurStaticMapID(void)
{
	return g_CommonFunc.m_nMapID;
}
//------------------------------------------------------------------------