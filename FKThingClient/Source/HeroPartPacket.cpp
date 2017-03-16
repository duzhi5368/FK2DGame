/**
*	created:		2012-7-4   1:07
*	filename: 		HeroPartPacket
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		主角的背包栏
*/
//------------------------------------------------------------------------
#include "../Include/ThingClientCommon.h"
#include "../../FKGame/SceneObj/IGoods.h"
#include "../Include/HeroPartPacket.h"
#include "../Include/CThingClientFacade.h"
#include "../Include/CreatureViewWrapper.h"
#include "../../FKGame/Container/IContainerClient.h"
#include "../../FKGame/Common/SContainer.h"
//------------------------------------------------------------------------
CHeroPart_Packet::CHeroPart_Packet(void)
{
	m_pMaster = NULL;	
	m_pContainer = NULL;
}
//------------------------------------------------------------------------
CHeroPart_Packet::~CHeroPart_Packet(void)
{
	
}
//------------------------------------------------------------------------
BOOL CHeroPart_Packet::Create(IThing * pMaster, char * pContext, int nLen)
{
	if(pMaster == NULL || pMaster->GetThingClass()->GetThingClass() != THINGCLASS_ACTOR)
	{
		return FALSE;
	}

	m_pMaster = (IActor *)pMaster;

	// 创建包裹栏容器
	IContainerMgr * pContainerMgr = g_pThingClientFacade->GetContainerMgr();
	if(pContainerMgr == NULL)
	{
		return FALSE;
	}

	SContainerCreateContext containercreatecontext;
	containercreatecontext.dwID = m_pMaster->GetPropNum(CREATURE_PROPID_PACKETCONTAINERID);
	containercreatecontext.nMaxSize = PACKET_OBJECT_INITCOUNT;
	containercreatecontext.bCanAdd = TRUE;
	containercreatecontext.bCanRemove = TRUE;
	containercreatecontext.dwType = GOODS_TYPE_REALY;	
	m_pContainer = pContainerMgr->CreateContainer((LPCSTR)&containercreatecontext, sizeof(SContainerCreateContext));
	if(m_pContainer == NULL)
	{
		return FALSE;
	}
	PrintEventLn("创建容器 = %d", m_pContainer->GetID());

	// 向服务器添加一个观察者
	m_pContainer->Open();

	return TRUE;
}
//------------------------------------------------------------------------
void CHeroPart_Packet::Release(void)
{
	if(m_pContainer != NULL)
	{
		// 删掉物品
		for(int i = 0; i < m_pContainer->GetSize(); i++)
		{
			IContainerGoods * pContainerGoods = m_pContainer->GetGoods(i);
			if(pContainerGoods == NULL)
			{
				continue;
			}

			UID uidGoods = pContainerGoods->GetUID();
			IGoods * pGoods = g_pThingClientFacade->GetGoods(uidGoods);
			SAFE_RELEASE(pGoods);
		}

		// 向服务器退订一个观察者
		m_pContainer->Close();

		PrintEventLn("释放容器 = %d", m_pContainer->GetID());
	}

	// 释放容器	
	SAFE_RELEASE(m_pContainer);

	Freeze();

	delete this;
}
//------------------------------------------------------------------------
DWORD CHeroPart_Packet::GetPartID(void)
{
	return THINGPARTID_ACTOR_PACKET;
}
//------------------------------------------------------------------------
IThing * CHeroPart_Packet::GetMaster(void)
{
	return m_pMaster;
}
//------------------------------------------------------------------------
BOOL CHeroPart_Packet::Activation(LPCSTR pContext, int nLen)
{	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CHeroPart_Packet::Freeze(void)
{
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CHeroPart_Packet::OnGetPrivateContext(LPSTR buf, int &nLen)
{
	nLen = 0;
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CHeroPart_Packet::OnGetPublicContext(LPSTR buf, int &nLen)
{
	nLen = 0;
	return FALSE;
}
//------------------------------------------------------------------------
BOOL CHeroPart_Packet::OnGetDBContext(LPSTR buf, int &nLen)
{
	nLen = 0;
	return FALSE;
}
//------------------------------------------------------------------------
int CHeroPart_Packet::OnMessage(DWORD dwMsg, LPCSTR buf, int nLen)
{
	return 0;
}
//------------------------------------------------------------------------
BOOL CHeroPart_Packet::InitClient(UID uidClient)
{
	return TRUE;
}
//------------------------------------------------------------------------
IContainer * CHeroPart_Packet::GetContainer(void)
{
	return m_pContainer;
}
//------------------------------------------------------------------------
BOOL CHeroPart_Packet::IsExistByGoodsID(DWORD dwGoodsID, DWORD dwNum)
{
	if(m_pContainer == NULL)
	{
		return FALSE;
	}

	int nTotalNum = 0;
	IContainerGoods * pContainerGoods = NULL;
	for(int i = 0; i < m_pContainer->GetSize(); i++)
	{
		pContainerGoods = m_pContainer->GetGoods(i);
		if(pContainerGoods == NULL)
		{
			continue;
		}

		if(pContainerGoods->GetGoodsID() == dwGoodsID)
		{
			nTotalNum += pContainerGoods->GetNumber();

			if(nTotalNum >= dwNum)
			{
				return TRUE;			
			}
		}	
	}

	return FALSE;
}
//------------------------------------------------------------------------
DWORD CHeroPart_Packet::GetNumByGoodsID(DWORD dwGoodsID)
{
	if(m_pContainer == NULL)
	{
		return 0;
	}

	int nTotalNum = 0;
	IContainerGoods * pContainerGoods = NULL;
	for(int i = 0; i < m_pContainer->GetSize(); i++)
	{
		pContainerGoods = m_pContainer->GetGoods(i);
		if(pContainerGoods == NULL)
		{
			continue;
		}

		if(pContainerGoods->GetGoodsID() == dwGoodsID)
		{
			nTotalNum += pContainerGoods->GetNumber();
		}
	}

	return nTotalNum;
}
//------------------------------------------------------------------------
BOOL CHeroPart_Packet::IsRemoveByGoodsID(DWORD dwGoodsID, DWORD dwNum)
{
	if(m_pContainer == NULL)
	{
		return FALSE;
	}

	int nTotalNum = 0;
	IContainerGoods * pContainerGoods = NULL;
	for(int i = 0; i < m_pContainer->GetSize(); i++)
	{
		pContainerGoods = m_pContainer->GetGoods(i);
		if(pContainerGoods == NULL)
		{
			continue;
		}

		if(pContainerGoods->GetGoodsID() == dwGoodsID && m_pContainer->CanRemove(pContainerGoods, pContainerGoods->GetLocation()))
		{
			nTotalNum += pContainerGoods->GetNumber();

			if(nTotalNum >= dwNum)
			{
				return TRUE;
			}
		}		
	}

	return FALSE;
}
//------------------------------------------------------------------------
DWORD CHeroPart_Packet::GetRemoveNumByGoodsID(DWORD dwGoodsID)
{
	if(m_pContainer == NULL)
	{
		return FALSE;
	}

	int nTotalNum = 0;
	IContainerGoods * pContainerGoods = NULL;
	for(int i = 0; i < m_pContainer->GetSize(); i++)
	{
		pContainerGoods = m_pContainer->GetGoods(i);
		if(pContainerGoods == NULL)
		{
			continue;
		}

		if(pContainerGoods->GetGoodsID() == dwGoodsID && m_pContainer->CanRemove(pContainerGoods, pContainerGoods->GetLocation()))
		{
			nTotalNum += pContainerGoods->GetNumber();
		}
	}

	return nTotalNum;
}
//------------------------------------------------------------------------