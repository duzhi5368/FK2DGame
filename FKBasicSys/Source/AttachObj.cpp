/**
*	created:		2012-7-2   2:00
*	filename: 		AttachObj
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/AttachObj.h"
#include "../Include/ResLoader.h"
#include "../Include/CreatureView.h"
#include "../Include/StaticObj.h"
//------------------------------------------------------------------------
extern CMagicFlowText* g_pFlowNum[12][enNUM_TYPE_MAX_NUM][2];
extern CMagicFlowText* g_pCrazy[enNUM_TYPE_MAX_NUM];

extern CMagicFlowText* g_pResist;
extern CMagicFlowText* g_pHedge;

extern int g_CommonNumWidthHalf[3];
extern int g_CommonSignWidthHalf[3];
extern int g_CrazyNumWidthHalf[3];
extern int g_CrazySignWidthHalf[3];
extern int g_CharWidthHalf[3];
//------------------------------------------------------------------------
CAttachObj::CAttachObj(CCreatureView* pParent)
{
	m_dwResID = 0;
	m_dwAppearanceID = -1;
	m_nState = m_nFrame = m_nFrameCount = 0;
	(__int64&)m_ptAnchor = (__int64&)m_ptOffset = 0;
	m_pPoolNodeObj = NULL;
	m_pParent = pParent;
	m_pParent->AddLinkView(&m_pParent);

	for(int i = 0; i < MAX_DRAW_FRAMES ; i++)
	{
		m_bNeedSetOldDirty[i] = FALSE;
		m_nFrameOld[i] = -1;
		SetRectEmpty(&m_rcAreaOld[i]);
	}
	m_dwSoundResID = 0;
	m_bSoundLoop = FALSE;
	m_nSoundChunnelID = -1;
}
//------------------------------------------------------------------------
CAttachObj::~CAttachObj()
{
	Close();
}
//------------------------------------------------------------------------
void CAttachObj::Close()
{
	if(m_pParent != NULL)
		m_pParent->DelLinkView(&m_pParent);

	if (m_pPoolNodeObj)
	{
		g_ResLoader.ReleaseRes(ioMagic + m_dwResID);
		m_pPoolNodeObj = NULL;
	}
	g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));

	Fun_StopSound(m_nSoundChunnelID);
}
//------------------------------------------------------------------------
CAni* CAttachObj::RequestRes()
{
	if (m_pPoolNodeObj)
		return m_pPoolNodeObj;
	
	m_pPoolNodeObj = static_cast<CAni*>(g_ResLoader.RequestMagic(m_dwResID));
	if (m_pPoolNodeObj)
		m_nFrameCount = m_pPoolNodeObj->GetTotalFrames();
	return m_pPoolNodeObj;
}
//------------------------------------------------------------------------
// 创建吸附物
BOOL CAttachObj::Create(const char* buf, int nLen)
{
	SAttachObjBuffer* pBuffer = (SAttachObjBuffer*)buf;

	if(pBuffer->nDelay <= 0 )
		return FALSE;
	
	m_dwResID			= pBuffer->dwResID;
	m_ptAnchor			= pBuffer->ptAnchor;
	m_ptOffset			= pBuffer->ptOffset;
	if(m_pParent != NULL && pBuffer->bAddCrtHeight)
	{
		m_ptOffset.y += m_pParent->GetHeightOffset();
	}
	m_dwSoundResID		= pBuffer->dwSoundResID;
	m_bSoundLoop		= pBuffer->bSoundLoop;
	m_nDrawOrderSave	= pBuffer->nDrawOrder;
	for (int i = 0; i < MAX_DRAW_FRAMES; ++i)
		m_nDrawOrder[i] = m_nDrawOrderSave;
	m_dwAppearanceID = pBuffer->dwAppearanceID;
	
	if (!RequestRes())
		return FALSE;

	g_pTimeAxis->SetTimer(0,
		static_cast<ITimerSink*>(this),
		pBuffer->nDelay,
		"BasicSys!!CAttachObj::Create");

	m_nState = IAttachObj::stateCreate;

	if(m_pParent)
		m_nSoundChunnelID = Fun_PlaySound(m_pParent->GetTile(), m_dwSoundResID, m_bSoundLoop);

	return TRUE;
}
//------------------------------------------------------------------------
RECT CAttachObj::GetShowRect(int nLoopNumIndex)
{
	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_nFrame, offx, offy);
	RECT rc = {- m_ptAnchor.x + offx + m_ptOffset.x, - m_ptAnchor.y + offy + m_ptOffset.y, 0, 0};
	rc.right = rc.left + m_pPoolNodeObj->GetWidth(m_nFrame);
	rc.bottom = rc.top + m_pPoolNodeObj->GetHeight(m_nFrame);
	return rc;
}
//------------------------------------------------------------------------
void CAttachObj::GetOldDirtyRect(list<RECT> &DirtyRectList, int nLoopNumIndex)
{
	if(!m_bNeedSetOldDirty[nLoopNumIndex])
		return;

	m_bNeedSetOldDirty[nLoopNumIndex] = FALSE;
	
	DirtyRectList.push_back(m_rcAreaOld[nLoopNumIndex]);
}
//------------------------------------------------------------------------
void CAttachObj::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop, const POINT& ptOffset, int nLoopNumIndex)
{
	if(!pCanvas)
		return;
	
	RECT rcAreaNew = GetShowRect(nLoopNumIndex);
	OffsetRect(&rcAreaNew,
		ptOffset.x,
		ptOffset.y);

	// 处理旧脏
	if(m_bNeedSetOldDirty[nLoopNumIndex])
	{
		// 如果还是画的同一祯
		if(m_nFrameOld[nLoopNumIndex] == m_nFrame)
		{
			if(EqualRect(&rcAreaNew, &m_rcAreaOld[nLoopNumIndex]))	// 如果和上次画的位置一样则不用处理了						
				return;
		}
		else
		{
			m_nFrameOld[nLoopNumIndex] = m_nFrame;
		}

		// 如果画的不是同一祯则把上次和这次的矩阵设脏
		// 先把上次旧的画图区域设脏
		OffsetRect(&m_rcAreaOld[nLoopNumIndex],
			-ptViewTop.x,
			-ptViewTop.y);
		pCanvas->AddDirtyRect(m_rcAreaOld[nLoopNumIndex]);
	}

	if(!RequestRes())
		return;

	// 保存这次的数据
	m_rcAreaOld[nLoopNumIndex] = rcAreaNew;

	// 再把新的要画图区域设脏
	OffsetRect(&rcAreaNew,
		-ptViewTop.x,
		-ptViewTop.y);
	pCanvas->AddDirtyRect(rcAreaNew);
	
	// 表示这次这个对象画了，下次要把他画的地方设脏
	m_bNeedSetOldDirty[nLoopNumIndex] = TRUE;
}
//------------------------------------------------------------------------
void CAttachObj::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag, int nLoopNumIndex)
{
	if (!pCanvas || !RequestRes()) return;
	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_nFrame, offx, offy);
	m_pPoolNodeObj->Draw(pCanvas, 
		ptTileCenter.x - m_ptAnchor.x + offx + m_ptOffset.x, 
		ptTileCenter.y - m_ptAnchor.y + offy + m_ptOffset.y,
		m_nFrame, dwFlag);
}
//------------------------------------------------------------------------
// 漂浮的文字
CFlowText::CFlowText(CCreatureView* pParent)
{
	(__int64&)m_ptBeginPos = 0;
	(__int64&)m_ptPos = 0;
	m_nAlpha = 255;
	m_nFlowHeight = 0;
	m_a = 0.0f;
	m_t = 0;
	m_nDelay = 0;
	m_nNumSize = 0;
	m_nSign = 11;
	m_pParent = pParent;
	m_pParent->AddLinkView(&m_pParent);
	m_nFlowType = FlowTypeCommon;
	m_nNumType = enNUM_TYPE_YELLOW;
	m_bBeginDraw = TRUE;
}
//------------------------------------------------------------------------
CFlowText::~CFlowText()
{
	Close();
}
//------------------------------------------------------------------------
void CFlowText::Close()
{
	if(m_pParent != NULL)
		m_pParent->DelLinkView(&m_pParent);

	g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
	g_pTimeAxis->KillTimer(1, static_cast<ITimerSink*>(this));
}
//------------------------------------------------------------------------
BOOL CFlowText::Create(const char* buf, int nLen)
{
	if(!m_pParent)
		return FALSE;
	
	SFlowTextBuffer* pBuffer = (SFlowTextBuffer*)buf;

	if(pBuffer->nDelay <= 0 )
		return FALSE;
	
	m_nDelay = pBuffer->nDelay;
	m_ptBeginPos = pBuffer->ptPos;
	m_ptPos = m_ptBeginPos;
	m_nFlowType = pBuffer->nFlowType;

	m_a = 3;
	m_nAlpha = 0;
	m_nCurFrame = 0;
	m_t = 6;

	m_nNumType = pBuffer->nNumType;
	if(m_nNumType == enNUM_TYPE_GREEN)
		m_nSign = 10;
	else
		m_nSign = 11;

	int nNum = abs(pBuffer->nNum);
	char szMsg[32] = {0};
	itoa(nNum,szMsg,10);

	m_nNumSize = 0;
	for (int i = 0; szMsg[i]; i++, m_nNumSize++)
	{
		m_Num[i] = (((unsigned char)szMsg[i]) & 0x7f) - 48;
	}

	
	DWORD dwNowTime = ::GetTickCount();
	if(m_nFlowType == FlowTypeCrazy)
		dwNowTime += 180;
	DWORD dwLastTime = m_pParent->GetLastFlowTime();
	int nTemp = dwNowTime - dwLastTime;
	if(nTemp > -1000 && nTemp < 200)
	{
		m_bBeginDraw = FALSE;
		m_pParent->SetLastFlowTime(dwLastTime + 200);
		g_pTimeAxis->SetTimer( 0,
			static_cast<ITimerSink*>(this),
			200 - nTemp,
			"BasicSys!!CFlowText::Create");

	}
	else
	{
		m_pParent->SetLastFlowTime(dwNowTime);
		OnTimer(0);
	}

	return TRUE;
}
//------------------------------------------------------------------------
// ITimerSink
void CFlowText::OnTimer(DWORD dwEventID)
{
	if(dwEventID == 0)
	{
		m_bBeginDraw = TRUE;
		g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
		g_pTimeAxis->SetTimer(1,
			static_cast<ITimerSink*>(this),
			m_nDelay,
			"BasicSys!!CFlowText::OnTimer");
	}
	else
	{
		m_t ++;
		if(m_t > 6)
		{
			m_a -= (float)0.035;
			int dy = m_a;
			if(dy > 0)
				m_nFlowHeight += dy;

			m_ptPos.y = m_ptBeginPos.y - m_nFlowHeight;
			m_nAlpha -= 3;
		}
		else
		{
			switch(m_t) 
			{
			case 1:
			case 4:
				m_nCurFrame = 1;
				break;
			case 2:
				m_nCurFrame = 2;
				break;
			case 6:
				m_nCurFrame = 0;
				break;
			default:
				break;
			}
		}
		
		if (m_t >= 90)
		{
			if (m_pParent)
				m_pParent->OnFlowObjDestroy(this);
		}
	}
}
//------------------------------------------------------------------------
void CFlowText::OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag, int nLoopNumIndex)
{
	if(!m_bBeginDraw)
		return;
	
	int x = g_pMapView->GetViewTopLeftX();
	int y = g_pMapView->GetViewTopLeftY();
	
	POINT ptTemp = m_ptPos;
	ptTemp.x -= x;
	ptTemp.y -= y;
	int nBeginX;
	int i;
	switch(m_nFlowType)
	{
	case FlowTypeCommon:	// 正常飘
		{
			// 计算最左边位置
			nBeginX = m_ptPos.x - x	- g_CommonNumWidthHalf[m_nCurFrame] * m_nNumSize;

			// 画符号
			g_pFlowNum[m_nSign][m_nNumType][0]->DrawText(pCanvas,
				nBeginX,
				m_ptPos.y - y,
				m_nCurFrame, dwFlag, m_nAlpha);
			nBeginX += g_CommonSignWidthHalf[m_nCurFrame];

			// 画数字
			for(i = 0; i < m_nNumSize; i++)
			{
				nBeginX += g_CommonNumWidthHalf[m_nCurFrame];
				g_pFlowNum[m_Num[i]][m_nNumType][0]->DrawText(pCanvas,
					nBeginX,
					m_ptPos.y - y,
					m_nCurFrame, dwFlag, m_nAlpha);
				nBeginX += g_CommonNumWidthHalf[m_nCurFrame];
			}
			break;
		}
	case FlowTypeCrazy:		// 飘暴击
		{
			// 计算最左边位置
			nBeginX = m_ptPos.x - x
				- g_CrazySignWidthHalf[m_nCurFrame]
				- g_CrazyNumWidthHalf[m_nCurFrame] * m_nNumSize;
			
			// 画致命
			g_pCrazy[m_nNumType]->DrawText(pCanvas, nBeginX, m_ptPos.y - y, m_nCurFrame, dwFlag, m_nAlpha);
			nBeginX += g_CharWidthHalf[m_nCurFrame];
			
			// 画符号
			nBeginX += g_CrazySignWidthHalf[m_nCurFrame];
			g_pFlowNum[m_nSign][m_nNumType][1]->DrawText(pCanvas,
				nBeginX,
				m_ptPos.y - y,
				m_nCurFrame, dwFlag, m_nAlpha);
			nBeginX += g_CrazySignWidthHalf[m_nCurFrame];

			// 画数字
			for(i = 0; i < m_nNumSize; i++)
			{
				nBeginX += g_CrazyNumWidthHalf[m_nCurFrame];
				g_pFlowNum[m_Num[i]][m_nNumType][1]->DrawText(pCanvas,
					nBeginX,
					m_ptPos.y - y,
					m_nCurFrame, dwFlag, m_nAlpha);
				nBeginX += g_CrazyNumWidthHalf[m_nCurFrame];
			}
			break;
		}
	case FlowTypeResist:	// 飘抵抗
		g_pResist->DrawText(pCanvas, m_ptPos.x - x, m_ptPos.y - y, m_nCurFrame, dwFlag, m_nAlpha);
		break;
	case FlowTypeHedge:		// 飘躲闪
		g_pHedge->DrawText(pCanvas, m_ptPos.x - x, m_ptPos.y - y, m_nCurFrame, dwFlag, m_nAlpha);
		break;
	default:
		break;
	}
}
//------------------------------------------------------------------------
// 被击动画
CBehitMagic::CBehitMagic(CCreatureView* pParent) : CAttachObj(pParent)
{
}
//------------------------------------------------------------------------
BOOL CBehitMagic::Create(const char* buf, int nLen)
{
	return CAttachObj::Create(buf, nLen);
}
//------------------------------------------------------------------------
void CBehitMagic::OnTimer(DWORD dwEventID)
{
	if (++m_nFrame > m_nFrameCount)
	{
		if (m_pParent)
			m_pParent->OnAttachObjDestroy(this);
	}
}
//------------------------------------------------------------------------
// 带有3个状态的吸附对象
CAttachStateObj::CAttachStateObj(CCreatureView* pParent) : CAttachObj(pParent)
{
	m_nLoopTime = -1;
	m_nLoopStartIndex = m_nLoopEndIndex = -1;
	m_nLoopNum = 1;
	m_nNoShowBegin = m_nNoShowBegin = -1;
	m_nLoopNumSave = 0;
	memset(m_nLoopNumIndex, 0, sizeof(m_nLoopNumIndex));
}
//------------------------------------------------------------------------
CAttachStateObj::~CAttachStateObj()
{
}
//------------------------------------------------------------------------
// 创建吸附物
BOOL CAttachStateObj::Create(const char* buf, int nLen)
{
	SAttachStateObjBuffer* pBuffer = (SAttachStateObjBuffer*)buf;
	m_nLoopStartIndex = pBuffer->nLoopStartIndex - 1;
	m_nLoopEndIndex = pBuffer->nLoopEndIndex - 1;

	m_nLoopTime = pBuffer->nLoopTime;
	m_nNoShowBegin = pBuffer->m_nNoShowBegin;
	m_nNoShowEnd = pBuffer->m_nNoShowEnd;
	m_nLoopNum = max(1, min(MAX_DRAW_FRAMES, pBuffer->m_nLoopNum));

	m_nLoopNumSave = m_nFrame;
	
	if(!CAttachObj::Create(buf, nLen))
		return FALSE;

	if(m_nLoopStartIndex < 0 || m_nLoopStartIndex >= m_nLoopEndIndex)
	{
		m_nLoopStartIndex = 0;
		m_nLoopEndIndex = m_nFrameCount - 1;
	}
	return TRUE;
}
//------------------------------------------------------------------------
void CAttachStateObj::OnTimer(DWORD dwEventID)
{
	if(m_nLoopTime > 0)
	{
		if(m_nFrame == m_nLoopStartIndex)
			m_nLoopTime--;

		if(m_nLoopTime == 0)
			m_nState = IAttachObj::stateRelease;
	}

	if (m_nState == IAttachObj::stateCreate) // 启动阶段
	{
		if (++m_nFrame >= m_nLoopStartIndex) m_nState = IAttachObj::stateLoop;
	}
	else if (m_nState == IAttachObj::stateLoop) // 循环运行阶段
	{
		if (++m_nFrame >= m_nLoopEndIndex)
		{
			m_nFrame = m_nLoopStartIndex;
		}
	}
	else if (m_nState == IAttachObj::stateRelease) // 销毁阶段
	{
		if (++m_nFrame >= m_nFrameCount)
		{
			if (m_pParent)
			{
				m_pParent->OnAttachObjDestroy(this);
				return;
			}
		}
	}

	// 前后绘图判断
	for (int ni = 0; ni < MAX_DRAW_FRAMES; ++ni)
		m_nDrawOrder[ni] = m_nDrawOrderSave;

	if (m_nLoopNumSave != m_nFrame)
	{
		for (int i = 0; i < m_nLoopNum; ++i)
		{
			m_nFrameCount = max(1, m_nFrameCount);
			float fTmp = m_nFrameCount / m_nLoopNum + 0.5f;
			int nIncFrame =  fTmp * i;
			int nFrameCur = (m_nFrame + nIncFrame) % m_nFrameCount;

			m_nLoopNumIndex[i] = nFrameCur;
			
			for (int k = m_nNoShowBegin; k < m_nNoShowEnd; ++k)
			{
				if (k == nFrameCur)
				{
					m_nDrawOrder[i] = IAttachObj::draw_order_prev;
					break;
				}
			}
		}
		
		m_nLoopNumSave = m_nFrame;
	}
}
//------------------------------------------------------------------------
RECT CAttachStateObj::GetShowRect(int nLoopNumIndex)
{
	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_nLoopNumIndex[nLoopNumIndex], offx, offy);
	RECT rc = {- m_ptAnchor.x + offx + m_ptOffset.x, - m_ptAnchor.y + offy + m_ptOffset.y, 0, 0};
	rc.right = rc.left + m_pPoolNodeObj->GetWidth(m_nLoopNumIndex[nLoopNumIndex]);
	rc.bottom = rc.top + m_pPoolNodeObj->GetHeight(m_nLoopNumIndex[nLoopNumIndex]);
	return rc;
}
//------------------------------------------------------------------------
void CAttachStateObj::SetDirty(ICanvas* pCanvas, const POINT& ptViewTop, const POINT& ptOffset, int nLoopNumIndex)
{
	if(!pCanvas)
		return;
	
	RECT rcAreaNew = GetShowRect(nLoopNumIndex);
	OffsetRect(&rcAreaNew,
		ptOffset.x,
		ptOffset.y);

	// 处理旧脏
	if(m_bNeedSetOldDirty[nLoopNumIndex])
	{
		// 如果还是画的同一祯
		if(m_nFrameOld[nLoopNumIndex] == m_nLoopNumIndex[nLoopNumIndex])
		{
			if(EqualRect(&rcAreaNew, &m_rcAreaOld[nLoopNumIndex]))	// 如果和上次画的位置一样则不用处理了						
				return;
		}
		else
		{
			m_nFrameOld[nLoopNumIndex] = m_nFrame;
		}

		// 如果画的不是同一祯则把上次和这次的矩阵设脏
		// 先把上次旧的画图区域设脏
		OffsetRect(&m_rcAreaOld[nLoopNumIndex],
			-ptViewTop.x,
			-ptViewTop.y);
		pCanvas->AddDirtyRect(m_rcAreaOld[nLoopNumIndex]);
	}

	if(!RequestRes())
		return;

	// 保存这次的数据
	m_rcAreaOld[nLoopNumIndex] = rcAreaNew;

	// 再把新的要画图区域设脏
	OffsetRect(&rcAreaNew,
		-ptViewTop.x,
		-ptViewTop.y);
	pCanvas->AddDirtyRect(rcAreaNew);
	
	// 表示这次这个对象画了，下次要把他画的地方设脏
	m_bNeedSetOldDirty[nLoopNumIndex] = TRUE;
}
//------------------------------------------------------------------------
void CAttachStateObj::OnDraw(ICanvas* pCanvas,
							 const POINT& ptTileCenter, DWORD dwFlag, int nLoopNumIndex)
{
	if (!pCanvas || !RequestRes()) return;

	int offx, offy;
	m_pPoolNodeObj->GetOffset(m_nLoopNumIndex[nLoopNumIndex], offx, offy);
	
	m_pPoolNodeObj->Draw(pCanvas, 
		ptTileCenter.x - m_ptAnchor.x + offx + m_ptOffset.x, 
		ptTileCenter.y - m_ptAnchor.y + offy + m_ptOffset.y,
		m_nLoopNumIndex[nLoopNumIndex], dwFlag);
}
//------------------------------------------------------------------------