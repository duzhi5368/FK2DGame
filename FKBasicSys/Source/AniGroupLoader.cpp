/**
*	created:		2012-7-2   1:18
*	filename: 		AniGroupLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/AniGroup.h"
#include "../Include/AniGroupLoader.h"
#include "../Include/ResLoader.h"
//------------------------------------------------------------------------
CAniGroupLoader::CAniGroupLoader()
{
	m_dwCurLoadResID = 0;
	m_hEventWakeDecode = NULL;
}
//------------------------------------------------------------------------
CAniGroupLoader::~CAniGroupLoader()
{
	m_Thread.Close();
}
//------------------------------------------------------------------------
BOOL CAniGroupLoader::Create()
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
void CAniGroupLoader::OnEvent(DWORD dwEventID)
{	
	m_dwCurLoadResID = 0;
	m_NodeListLock.Lock();

	int nCount = m_NodeList.GetCount();
	if(nCount == 0)
	{
		m_NodeListLock.Unlock();
		return;
	}
	stAniGroupNode node = m_NodeList.RemoveHead();

	m_dwCurLoadResID = node.dwResID;

	m_NodeLoadLock.Lock();

	m_NodeListLock.Unlock();

	LoadRes(node);
	node.pAniGroup->m_LoadState[node.nState] = enLOAD_OVER;

	m_NodeLoadLock.Unlock();

	m_dwCurLoadResID = 0;
	SetEvent(m_hEventWakeDecode);
}
//------------------------------------------------------------------------
void CAniGroupLoader::LoadRes(stAniGroupNode node)
{
	if(node.pAniGroup->m_ppaState == NULL || node.pAniGroup->m_ppaIBitmap == NULL)
		return;

	int nAni = node.nState << 3;
	long lDataSize = node.pAniGroup->GetStateSize(node.nState);

	BYTE* pData = new BYTE[lDataSize];
	if (!node.pAniGroup->GetStateData(node.nState, pData, lDataSize))
	{
		SAFE_DELETE_ARRAY(pData);
		return;
	}

	DWORD dwDataOffset = node.pAniGroup->m_paAniIndex[nAni].ani_offset - node.pAniGroup->m_paAniIndex[0].ani_offset;
	for (int j = 0; j < 8; j++ ) // 8个ani(8个方向)
	{
		for (int i = 0; i < node.pAniGroup->m_paAniIndex[nAni+j].ani_frames; i++ ) // 每个帧
		{
			long lIndex = node.pAniGroup->m_paAniIndex[nAni+j].key_frame + i;
			g_CanvasHelper.CreateIRleBitmap((IRleBitmap**)&node.pAniGroup->m_ppaIBitmap[lIndex]);
			if (node.pAniGroup->m_ppaIBitmap[lIndex])
			{
				((IRleBitmap*)node.pAniGroup->m_ppaIBitmap[lIndex])->Load(
					pData + node.pAniGroup->m_paAniIndex[nAni+j].ani_offset - dwDataOffset + node.pAniGroup->m_paFrameIndex[lIndex].data_offset);

				g_ResLoader.AddSize(node.pAniGroup->m_ppaIBitmap[lIndex]->GetSize());
				node.pAniGroup->m_dwSize += node.pAniGroup->m_ppaIBitmap[lIndex]->GetSize();
			}
			if (node.pAniGroup->m_pPal)
				((IRleBitmap*)node.pAniGroup->m_ppaIBitmap[lIndex])->SetPalette(node.pAniGroup->m_pPal, 0);// 第二个参数无意义，所以没填
		}
	}

	node.pAniGroup->m_ppaState[node.nState] = node.pAniGroup->m_ppaIBitmap[node.pAniGroup->m_paAniIndex[nAni].key_frame];
	delete []pData;
	return;
}
//------------------------------------------------------------------------
void CAniGroupLoader::AddNodeLoad(stAniGroupNode node)
{
	m_NodeListLock.Lock();

	m_NodeList.AddTail(node);
	SetEvent(m_hEventWakeDecode);

	m_NodeListLock.Unlock();
}
//------------------------------------------------------------------------
void CAniGroupLoader::RemoveNode(DWORD dwResID)
{	
	m_NodeListLock.Lock();

	POSITION pos = m_NodeList.GetHeadPosition();
	while(pos)
	{
		POSITION posLast = pos;
		stAniGroupNode& node = m_NodeList.GetNext(pos);
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