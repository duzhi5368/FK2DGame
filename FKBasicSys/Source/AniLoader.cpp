/**
*	created:		2012-7-1   20:16
*	filename: 		AniLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/AniLoader.h"
#include "../Include/Ani.h"
#include "../Include/ResLoader.h"
//------------------------------------------------------------------------
CAniLoader::CAniLoader()
{
	m_dwCurLoadResID = 0;
	m_hEventWakeDecode = NULL;
}
//------------------------------------------------------------------------
CAniLoader::~CAniLoader()
{
	m_Thread.Close();
}
//------------------------------------------------------------------------
BOOL CAniLoader::Create()
{
	m_Thread.Close();
	if(!m_Thread.Create(NULL))
	{
		return FALSE;
	}

	DWORD dwEventID;
	if(!m_Thread.m_pThreadRoute->AddEvent(static_cast<IEventSink*>(this),dwEventID))
		return FALSE;

	m_hEventWakeDecode = m_Thread.m_pThreadRoute->GetEventHandle(dwEventID);

	if(!m_Thread.m_pThreadRoute->Start())
	{
		m_Thread.Close();
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
void CAniLoader::OnEvent(DWORD dwEventID)
{	
	m_dwCurLoadResID = 0;
	m_NodeListLock.Lock();

	int nCount = m_NodeList.GetCount();
	if(nCount == 0)
	{
		m_NodeListLock.Unlock();
		return;
	}
	stAniNode node = m_NodeList.RemoveHead();

	m_dwCurLoadResID = node.dwResID;

	m_NodeLoadLock.Lock();

	m_NodeListLock.Unlock();

	LoadRes(node);	
	node.pAni->m_nLoadState = enLOAD_OVER;

	m_NodeLoadLock.Unlock();

	m_dwCurLoadResID = 0;
	SetEvent(m_hEventWakeDecode);
}
//------------------------------------------------------------------------
void CAniLoader::LoadRes(stAniNode node)
{
	// 只要用到就全都载入内存
	DWORD dwDataSize = node.pAni->m_AniInfoHdr.total_size;

	BYTE* pData = new BYTE[dwDataSize];
	if (!node.pAni->GetData(pData,dwDataSize))
	{
		SAFE_DELETE_ARRAY(pData);
		return;
	}

	node.pAni->m_ppaIBitmap = new IBitmap*[node.pAni->m_AniInfoHdr.total_frame];
	for (int i = 0; i < node.pAni->m_AniInfoHdr.total_frame; i++)
	{
		g_CanvasHelper.CreateIRleBitmap((IRleBitmap**)&node.pAni->m_ppaIBitmap[i]);
		if (node.pAni->m_ppaIBitmap[i])
		{
			node.pAni->m_ppaIBitmap[i]->Load(pData + node.pAni->m_paFrameIndex[i].data_offset);
			g_ResLoader.AddSize(node.pAni->m_ppaIBitmap[i]->GetSize());
			node.pAni->m_dwSize += node.pAni->m_ppaIBitmap[i]->GetSize();
		}
		if (node.pAni->m_pPal)
			((IRleBitmap*)node.pAni->m_ppaIBitmap[i])->SetPalette(node.pAni->m_pPal, 0);

	}

	SAFE_DELETE_ARRAY(pData);
}
//------------------------------------------------------------------------
void CAniLoader::AddNodeLoad(stAniNode node)
{
	m_NodeListLock.Lock();

	m_NodeList.AddTail(node);
	SetEvent(m_hEventWakeDecode);

	m_NodeListLock.Unlock();
}
//------------------------------------------------------------------------
void CAniLoader::RemoveNode(DWORD dwResID)
{	
	m_NodeListLock.Lock();

	POSITION pos = m_NodeList.GetHeadPosition();
	while(pos)
	{
		POSITION posLast = pos;
		stAniNode& node = m_NodeList.GetNext(pos);
		if(node.dwResID == dwResID)
		{
			m_NodeList.RemoveAt(posLast);
		}
	}

	if(m_dwCurLoadResID == dwResID)
	{
		m_NodeLoadLock.Lock();
		m_NodeLoadLock.Unlock();
	}
	m_NodeListLock.Unlock();
}
//------------------------------------------------------------------------