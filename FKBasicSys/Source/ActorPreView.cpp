/**
*	created:		2012-7-2   3:59
*	filename: 		ActorPreView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		角色预显示类
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/ActorPreView.h"
//------------------------------------------------------------------------
CActPreView::CActPreView()
{
	for (int i = 0; i < CrtPartMax; i++)
	{
		m_pModule[i] = NULL;
		m_dwModuleID[i] = 0;
		m_bytePal[i] = 0;
		m_byteAlpha[i] = 0;		
		m_PartStateFlag[i] = NULL;
	}
	memset (&m_AniContext, 0, sizeof(SAniContext));
	m_AniContext.nDir = 4;
	m_nDelay = 0;
	m_dwDrawFlag = 0;
}
//------------------------------------------------------------------------
CActPreView::~CActPreView()
{
}
//------------------------------------------------------------------------
BOOL CActPreView::ChangeModule(int nIndex, DWORD dwResID, BYTE nPal, BYTE nAlpha)
{
	if(nIndex >= CrtPartMax || nIndex < 0)
		return FALSE;
	m_dwModuleID[nIndex] = dwResID;
	m_bytePal[nIndex] = nPal;
	m_byteAlpha[nIndex] = nAlpha;
	return TRUE;
}
//------------------------------------------------------------------------
CAniGroup* CActPreView::RequestRes(int nModuleType)
{
	if (m_pModule[nModuleType])
		return m_pModule[nModuleType];
	
	DWORD dwID = m_dwModuleID[nModuleType];
	if (dwID == 0)
		return NULL;
	m_pModule[nModuleType] = static_cast<CAniGroup*>(g_ResLoader.RequestCreature(dwID));	
	
	if (m_pModule[nModuleType] == NULL)
	{
		return NULL;
	}
	if (m_pModule[nModuleType]->GetUserPal(0) == NULL) // 加载用户调色板
	{
		int nCount = ptMaxPalTypeCount;
		BYTE* pPal = m_pModule[nModuleType]->GetPal();
		int nUseColor = m_pModule[nModuleType]->GetUseColor();
		m_pModule[nModuleType]->SetMaxUserPalCount(nUseColor);
		//设置原始调色板
		m_Pal[nModuleType].SetPal(pPal, nUseColor);
		//如果使用调色板，改变调色板
		if (m_bytePal[nModuleType] != 0)
		{
			int *p = g_ResLoader.GetHSLpal(dwID, m_bytePal[nModuleType]);
			if (!p)
				return NULL;
			m_Pal[nModuleType].OnHSLChanged(p[0], p[1], p[2]);
#ifndef _USE_BINSCRIPT		//这个是为了满足Debug版本的需求。两个版本的脚本读取不一样
			SAFE_DELETE_ARRAY(p);
#endif
		}
	}
	//加载部件状态标志
#ifndef _USE_BINSCRIPT		//这个是为了满足Debug版本的需求。两个版本的脚本读取不一样
	SAFE_DELETE_ARRAY(m_PartStateFlag[nModuleType]);
#endif
	if (NULL == (m_PartStateFlag[nModuleType] = g_ResLoader.GetStateFlag(nModuleType, dwID)))
		return NULL;
	
	//记录生物该状态共有几帧
	if (nModuleType == CrtPartHead || nModuleType == CrtPartBody || nModuleType == CrtPartWholeBody)
	{
		int nCurAni = GetCurAni(nModuleType);
		if (nCurAni == -1)//不用Release()，因为每次使用前都会先Release()掉
			return NULL;
		m_AniContext.nFrames = m_pModule[nModuleType]->GetTotalFrames(nCurAni);
	}
	return m_pModule[nModuleType];
}
//------------------------------------------------------------------------
void CActPreView::ReleasePreView()
{
	for (int i=0; i<CrtPartMax; i++)
	{
		if (m_pModule[i])
		{
			DWORD dwID = m_dwModuleID[i];
			if (dwID != 0)
				g_ResLoader.ReleaseRes(ioCreature + dwID);
			m_pModule[i] = NULL;	
		}
		m_bytePal[i] = 0;
		m_byteAlpha[i] = 0;
		m_PartStateFlag[i] = NULL;
	}
	memset (&m_AniContext, 0, sizeof(SAniContext));
	m_AniContext.nDir = 4;
	m_nDelay = 0;
	g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
}
//------------------------------------------------------------------------
void CActPreView::SetState(int nState)
{
	if(nState != CrtStateStand)
	{
		nState = CrtStateStand;
		m_dwDrawFlag = 0;
	}
	else
	{
		m_dwDrawFlag = BLIT_TRANS;
	}

	if (nState >= 0 && nState < CrtStateMax)
	{
		if (nState != m_AniContext.nState)
		{
			m_AniContext.nState = nState;
			int nModuleType = 0;
			if (m_pModule[CrtPartHead])
				nModuleType = CrtPartHead;
			else if (m_pModule[CrtPartBody])
				nModuleType = CrtPartBody;
			else if (m_pModule[CrtPartWholeBody])
				nModuleType = CrtPartWholeBody;
			else
				return;
			int nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				return;
			m_AniContext.nFrames = m_pModule[nModuleType]->GetTotalFrames(nCurAni);
			//重新设置帧延时和定时器
			m_nDelay = g_ResLoader.GetFrameDelay(m_AniContext.nType, nState);
			g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
			if (m_nDelay > 0)
			{
				g_pTimeAxis->SetTimer(0,
					static_cast<ITimerSink*>(this),
					m_nDelay,
					"BasicSys!!CActPreView::SetState");
			}
		}
	}
}
//------------------------------------------------------------------------
int CActPreView::GetCurAni(int nFind)
{
	int nState = m_AniContext.nState;
	if(m_PartStateFlag[nFind] == NULL)
		return -1;
	
	if (m_PartStateFlag[nFind][nState] == 1)
	{
		
		int nPos = -1;
		for (int i = 0; i <= m_AniContext.nState; i++)
		{
			if (m_PartStateFlag[nFind][i] == 1)
			{
				nPos++;
			}
		}
		return nPos * 8 + m_AniContext.nDir;
	}
	return -1;
}
//------------------------------------------------------------------------
void CActPreView::OnTimer(DWORD dwEventID)
{
	if (++m_AniContext.nCurFrame >= m_AniContext.nFrames)
	{
		m_AniContext.nCurFrame = 0;
		if (m_AniContext.nState != CrtStateStand)
		{
			SetState (CrtStateStand);
		}
	}
}
//------------------------------------------------------------------------
void CActPreView::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag)
{
	if(!pCanvas) return;
	
	int offx, offy;
	int nModuleType, nCurAni;
	CAniGroup* pCurObj;
	BYTE nDrawOrder[CrtPartMax/2];
	memset (nDrawOrder, 0, sizeof(BYTE) * CrtPartMax/2);
	int nCount = g_ResLoader.GetFrameSort(nDrawOrder, m_AniContext);
	for (int i = 0; i < nCount; i++)
	{
		nModuleType = nDrawOrder[i];
		if(nModuleType < 0 || nModuleType > CrtPartMax - 1)
			continue;
		pCurObj = RequestRes(nModuleType);

	
		if (pCurObj)		{

			nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;
			pCurObj->GetOffset(nCurAni, m_AniContext.nCurFrame, offx, offy);

			if (m_byteAlpha[nModuleType] == 0 || dwFlag == (BLIT_MASK | BLIT_TRANS))
			{
				pCurObj->Draw(pCanvas, 
					ptTileCenter.x + offx,
					ptTileCenter.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, 0, m_Pal[nModuleType].GetPal());
			}
			else
			{
				pCurObj->Draw(pCanvas, 
					ptTileCenter.x + offx,
					ptTileCenter.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, m_byteAlpha[nModuleType], m_Pal[nModuleType].GetPal());
			}
		}
   
		// 画特效
		nModuleType++;
		pCurObj = RequestRes(nModuleType);
		if (pCurObj)
		{
			int nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;
			pCurObj->GetOffset(nCurAni, m_AniContext.nCurFrame, offx, offy);
			if (m_byteAlpha[nModuleType] == 0 || dwFlag == (BLIT_MASK | BLIT_TRANS))
			{
				pCurObj->Draw(pCanvas,
					ptTileCenter.x + offx,
					ptTileCenter.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, 0, m_Pal[nModuleType].GetPal());
			}
			else
			{
				pCurObj->Draw(pCanvas,
					ptTileCenter.x + offx,
					ptTileCenter.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, m_byteAlpha[nModuleType], m_Pal[nModuleType].GetPal());
			}
		}
	}
	/*
	// draw actor
	int offx, offy;
	BYTE nDrawOrder[CrtPartMax/2];
	int nCount = g_ResLoader.GetFrameSort(nDrawOrder, m_AniContext);
	for (int i=0; i<nCount; i++)
	{
		CAniGroup* pCurObj;
		int nModuleType = nDrawOrder[i];
		pCurObj = RequestRes(nModuleType);
		
		if (pCurObj)
		{
			int nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;
			pCurObj->GetOffset(nCurAni, m_AniContext.nCurFrame, offx, offy);
			int nTemp = BLIT_MASK | BLIT_TRANS;
			if (m_byteAlpha[nModuleType] == 0 || dwFlag == (BLIT_MASK | BLIT_TRANS))
			{
				pCurObj->Draw(pCanvas, 
					ptTileCenter.x + offx,
					ptTileCenter.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, 0, m_Pal[nModuleType].GetPal());
			}
			else
			{
				pCurObj->Draw(pCanvas, 
					ptTileCenter.x + offx,
					ptTileCenter.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, m_byteAlpha[nModuleType], m_Pal[nModuleType].GetPal());
			}
		}
		//画特效，华夏2才有
		nModuleType++;
		CAniGroup* pEffect;
		pEffect = RequestRes(nModuleType);
		if (pEffect)
		{
			int nCurAni = GetCurAni(nModuleType);
			if (nCurAni == -1)
				continue;
			pEffect->GetOffset(nCurAni, m_AniContext.nCurFrame, offx, offy);
			if (m_byteAlpha[nModuleType] == 0 || dwFlag == (BLIT_MASK | BLIT_TRANS))
			{
				pEffect->Draw(pCanvas,
					ptTileCenter.x + offx,
					ptTileCenter.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, 0, m_Pal[nModuleType].GetPal());
			}
			else
			{
				pEffect->Draw(pCanvas,
					ptTileCenter.x + offx,
					ptTileCenter.y + offy,
					nCurAni, m_AniContext.nCurFrame, dwFlag, m_byteAlpha[nModuleType], m_Pal[nModuleType].GetPal());
			}
		}
	}	
*/
}
//------------------------------------------------------------------------