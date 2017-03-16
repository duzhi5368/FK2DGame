/**
*	created:		2012-7-2   3:50
*	filename: 		ActorTitle
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/ActorTitle.h"
//------------------------------------------------------------------------
CTitleEffectFactory	g_tef;
ColorList CActorTitle::m_ColorList;
//------------------------------------------------------------------------
// 称号效果 基类
void CTitleEffect::DrawRect(ICanvas* pCanvas, RECT& rc, WPixel clr)
{
	if (!pCanvas) return;
	POINT ptFrom,ptTo;
	ptFrom.x=rc.left, ptFrom.y=rc.top, ptTo.x=rc.right-1, ptTo.y=rc.top;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 上横
	ptFrom.x=rc.right-1, ptFrom.y=rc.top, ptTo.x=rc.right-1, ptTo.y=rc.bottom;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 右竖
	ptFrom.x=rc.right-1, ptFrom.y=rc.bottom-1, ptTo.x=rc.left, ptTo.y=rc.bottom-1;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 下横
	ptFrom.x=rc.left, ptFrom.y=rc.bottom-1, ptTo.x=rc.left, ptTo.y=rc.top;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 左竖
}
//------------------------------------------------------------------------
void CTitleEffect::DrawBox(ICanvas* pCanvas, RECT& rc, WPixel clr, int alpha)
{
	if (!pCanvas) return;
	pCanvas->DrawBox(rc, clr, BLIT_TRANS, alpha);
}
//------------------------------------------------------------------------
// 称号效果 效果0:无效果
CTitleEffect0::CTitleEffect0()
{
	m_nWidth = m_nHeight = m_nCharCount = 0;
	m_pFont = g_pFontWithOutline;
}
//------------------------------------------------------------------------
BOOL CTitleEffect0::Create(NodeList& CharList)
{
	m_nHeight = m_pFont->GetHeight();
	m_nCharCount = CharList.size();
	for (int i=0; i<m_nCharCount; i++)
	{
		CharList[i].nWidth = m_pFont->GetLength(CharList[i].wChar);
		m_nWidth += CharList[i].nWidth;
	}
	return TRUE;
}
//------------------------------------------------------------------------
void CTitleEffect0::OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter, NodeList& CharList)
{
	int x = ptBottomCenter.x - m_nWidth / 2;
	int y = ptBottomCenter.y - m_nHeight - 1;
	for (int i=0; i<m_nCharCount; i++)
	{
		pCanvas->DrawText(m_pFont, x, y, CharList[i].wChar, CharList[i].color, FONT_NOALPHA);
		x += CharList[i].nWidth;
	}
}
//------------------------------------------------------------------------
// 称号效果 效果1:带背景
CTitleEffect1::CTitleEffect1()
{
	m_pFont = g_pFont;
}
//------------------------------------------------------------------------
void CTitleEffect1::OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter, NodeList& CharList)
{
	int x = ptBottomCenter.x - m_nWidth / 2;
	int y = ptBottomCenter.y - m_nHeight - 1;

	RECT rc;
	rc.left = x-1;
	rc.top = y-1;
	rc.right = x + m_nWidth;
	rc.bottom = ptBottomCenter.y - 1;
	pCanvas->DrawBox(rc, WPixel(255,255,0), BLIT_TRANS, 150);

	for (int i=0; i<m_nCharCount; i++)
	{
		pCanvas->DrawText(m_pFont, x, y, CharList[i].wChar, CharList[i].color, FONT_NOALPHA);
		x += CharList[i].nWidth;
	}
}
//------------------------------------------------------------------------
// 称号效果 效果2:文字陆续出现
CTitleEffect2::CTitleEffect2()
{
	m_pFont = g_pFontWithOutline;
	m_nTimerCount = 0;
	m_nState = 0;
}
//------------------------------------------------------------------------
CTitleEffect2::~CTitleEffect2()
{
}
//------------------------------------------------------------------------
BOOL CTitleEffect2::Create(NodeList& CharList)
{
	CTitleEffect0::Create(CharList);

	g_pTimeAxis->SetTimer(0,
		static_cast<ITimerSink*>(this),
		200,
		"BasicSys!!CTitleEffect2::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CTitleEffect2::Close()
{
	g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
}
//------------------------------------------------------------------------
void CTitleEffect2::OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter, NodeList& CharList)
{
	int x = ptBottomCenter.x - m_nWidth / 2;
	int y = ptBottomCenter.y - m_nHeight - 1;
	switch (m_nState) 
	{
	case 0:
		{
			for (int i=0; i<m_nTimerCount; i++)
			{
				pCanvas->DrawText(m_pFont, x, y, CharList[i].wChar, CharList[i].color, FONT_NOALPHA);
				x += CharList[i].nWidth;
			}
		}
		break;/*
	case 1:
		{
			for (int i=0; i<m_nCharCount; i++)
			{
				pCanvas->DrawText(m_pFont, x, y, CharList[i].wChar, CharList[i].color, FONT_NOALPHA);
				x += CharList[i].nWidth;
			}
		}
		break;*/
	case 2:
		{
			for (int i=0; i<m_nTimerCount; i++)
				x += CharList[i].nWidth;
			for (int i=m_nTimerCount; i<m_nCharCount; i++)
			{
				pCanvas->DrawText(m_pFont, x, y, CharList[i].wChar, CharList[i].color, FONT_NOALPHA);
				x += CharList[i].nWidth;
			}
		}
		break;
	}
}
//------------------------------------------------------------------------
void CTitleEffect2::OnTimer(DWORD dwEventID)
{
	switch (m_nState)
	{
	case 0:
		{
			if (++m_nTimerCount > m_nCharCount)
			{
				m_nState = 2;
				m_nTimerCount = 0;
			}
		}
		break;/*
	case 1:
		{
			if (++m_nTimerCount > m_nCharCount)
			{
				m_nState = 2;
				m_nTimerCount = m_nCharCount;
			}
		}
		break;*/
	case 2:
		{
			if (++m_nTimerCount > m_nCharCount)
			{
				m_nState = 0;
				m_nTimerCount = 0;
			}
		}
		break;
	}
}
//------------------------------------------------------------------------
// 称号效果 效果3:文字陆续变色效果
CTitleEffect3::CTitleEffect3()
{
	m_pFont = g_pFontWithOutline;
	m_nTimerCount = 0;
}
//------------------------------------------------------------------------
CTitleEffect3::~CTitleEffect3()
{
}
//------------------------------------------------------------------------
BOOL CTitleEffect3::Create(NodeList& CharList)
{
	CTitleEffect0::Create(CharList);

	g_pTimeAxis->SetTimer(0,
		static_cast<ITimerSink*>(this),
		200,
		"BasicSys!!CTitleEffect3::Create");

	return TRUE;
}
//------------------------------------------------------------------------
void CTitleEffect3::Close()
{
	g_pTimeAxis->KillTimer(0, static_cast<ITimerSink*>(this));
}
//------------------------------------------------------------------------
void CTitleEffect3::OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter, NodeList& CharList)
{
	int x = ptBottomCenter.x - m_nWidth / 2;
	int y = ptBottomCenter.y - m_nHeight - 1;
	WPixel color;
	color.red = 31 - CharList[m_nTimerCount].color.red;
	color.green = 63 - CharList[m_nTimerCount].color.green;
	color.blue = 31 - CharList[m_nTimerCount].color.blue;

	for (int i=0; i<m_nTimerCount; i++)
	{
		pCanvas->DrawText(m_pFont, x, y, CharList[i].wChar, CharList[i].color, FONT_NOALPHA);
		x += CharList[i].nWidth;
	}
	pCanvas->DrawText(m_pFont, x, y, CharList[m_nTimerCount].wChar, 
		color, FONT_NOALPHA);
	x += CharList[m_nTimerCount].nWidth;
	for (int i=m_nTimerCount+1; i<m_nCharCount; i++)
	{
		pCanvas->DrawText(m_pFont, x, y, CharList[i].wChar, CharList[i].color, FONT_NOALPHA);
		x += CharList[i].nWidth;
	}

}
//------------------------------------------------------------------------
void CTitleEffect3::OnTimer(DWORD dwEventID)
{
	if (++m_nTimerCount >= m_nCharCount)
		m_nTimerCount = 0;
}
//------------------------------------------------------------------------
// 称号对象
void CTitleObj::Close()
{
	CharList.clear();
	SAFE_RELEASE(pTitleEffect);
}
//------------------------------------------------------------------------
void CTitleObj::OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter)
{
	pTitleEffect->OnDraw(pCanvas, ptBottomCenter, CharList);	
}
//------------------------------------------------------------------------
// 角色称号
CActorTitle::CActorTitle()
{
}
//------------------------------------------------------------------------
CActorTitle::~CActorTitle()
{
	ClearTitle();
}
//------------------------------------------------------------------------
int CActorTitle::AddTitle(LPCSTR szFormatStr, DWORD dwEffectID)
{
	if (!szFormatStr || szFormatStr[0] == '\0') return FALSE;
	int nIndex = m_TitleList.size();
	CTitleObj to;
	to.pTitleEffect = g_tef.CreateTitleEffect(dwEffectID);
	m_TitleList.push_back(to);
	int nRtl = ParseFormatString(szFormatStr, m_TitleList[nIndex]);
	if (!nRtl)
	{
		SAFE_RELEASE(to.pTitleEffect);
		m_TitleList.erase(m_TitleList.end() - 1);
		return FALSE;
	}
	nRtl = to.pTitleEffect->Create(m_TitleList[nIndex].CharList);
	if (!nRtl)
	{
		SAFE_RELEASE(to.pTitleEffect);
		m_TitleList.erase(m_TitleList.end() - 1);
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
void CActorTitle::ClearTitle()
{
	int nSize = m_TitleList.size();
	for (int i=0; i<nSize; i++)
		m_TitleList[i].Close();
	m_TitleList.clear();
}
//------------------------------------------------------------------------
void CActorTitle::OnDraw(ICanvas* pCanvas, POINT& ptBottomCenter)
{
	int nTitleCount = m_TitleList.size();
	for (int i=0; i<nTitleCount; i++)
	{
		m_TitleList[i].OnDraw(pCanvas, ptBottomCenter);
		ptBottomCenter.y -= m_TitleList[i].pTitleEffect->GetHeight();
	}
}
//------------------------------------------------------------------------
//    #12#这个是称号的格式化字符串例子#22#呵呵
int CActorTitle::ParseFormatString(LPCSTR szFormatStr, CTitleObj& to)
{
	unsigned char* p = (unsigned char*)szFormatStr;
	int i = 0, j = 0;
	bool isColorID = false;
	char szID[32];
	int nColorID = 0;
	int nColorCount = m_ColorList.size();
	BOOL bRtn = FALSE;
	while (*p)
	{
		if (*p == COLORSIGN) // find '#'
		{
			unsigned char* pp = p + 1;
			while (*pp && isdigit(*pp))
				pp++;

			if (pp > p + 1 && *pp)
			{
				memset(szID, 0, sizeof(szID));
				strncpy(szID, (const char*)(p+1), pp-p-1);
				nColorID = atoi(szID);
				p = _mbsinc(pp);
				continue;
			}
		}
		
		SCharConext cc;
		cc.wChar[1] = cc.wChar[2] = '\0';
		_mbsncpy((unsigned char*)&cc.wChar[0], p, 1);
		if (nColorID >= nColorCount)
			nColorID = 0;
		cc.color = m_ColorList[nColorID];
		to.CharList.push_back(cc);
		p = _mbsinc(p);
		bRtn = TRUE;
	}
	return bRtn;
}
//------------------------------------------------------------------------
// 效果工厂
CTitleEffectFactory::ProcCreator CTitleEffectFactory::m_fnCreateEffect[] =
{
	&CTitleEffectFactory::CreateEffect0,
	&CTitleEffectFactory::CreateEffect1,
	&CTitleEffectFactory::CreateEffect2,
	&CTitleEffectFactory::CreateEffect3,
};
//------------------------------------------------------------------------
CTitleEffect* CTitleEffectFactory::CreateTitleEffect(DWORD dwEffectID)
{
	if (dwEffectID >= sizeof(CTitleEffectFactory::m_fnCreateEffect)/sizeof(void*))	// 当前未支持的dwEffectID
	{
		PrintEventLn("ERROR: CTitleEffectFactory::CreateTitleEffect(%d), unsupport effect ID", dwEffectID);
		dwEffectID = 0;
	}
	
	if (m_fnCreateEffect[dwEffectID] == NULL)
	{
		PrintEventLn("ERROR: CTitleEffectFactory::CreateTitleEffect(%d), m_fnCreateEffect[%d] == NULL", dwEffectID, dwEffectID);		
		return NULL;
	}
	
	return (this->*m_fnCreateEffect[dwEffectID])();
}
//------------------------------------------------------------------------
CTitleEffect* CTitleEffectFactory::CreateEffect0()
{
	return (CTitleEffect*) new CTitleEffect0;
}
//------------------------------------------------------------------------
CTitleEffect* CTitleEffectFactory::CreateEffect1()
{
	return (CTitleEffect*) new CTitleEffect1;
}
//------------------------------------------------------------------------
CTitleEffect* CTitleEffectFactory::CreateEffect2()
{
	return (CTitleEffect*) new CTitleEffect2;
}
//------------------------------------------------------------------------
CTitleEffect* CTitleEffectFactory::CreateEffect3()
{
	return (CTitleEffect*) new CTitleEffect3;
}
//------------------------------------------------------------------------