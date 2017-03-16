/**
*	created:		2012-7-1   5:49
*	filename: 		WeatherParticle
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/WeatherParticle.h"
#include "../Include/Weather.h"
#include <mmsystem.h>
#include "../Include/MapViewCommon.h"
//------------------------------------------------------------------------
// 雨粒子
CRainParticle::CRainParticle()
{
	m_pWeather = NULL;

	m_bRun = FALSE;
	
	m_fSpeedX = 0;
	m_fSpeedY = 0;

	m_nLengthX = 0;
	m_nLengthY = 0;

	m_nEndY = 0;

	m_dwCreateTime = 0;
	m_dwLastTime = 0;
	m_dwCurLife = 0;
	m_nAlpha = 0;

	m_ptBeginLoc.x = m_ptBeginLoc.y = 0;
	m_ptDrawFrm.x = m_ptDrawFrm.y = 0;
	m_ptDrawTo.x = m_ptDrawTo.y = 0;
	m_ptLoc.x = m_ptLoc.y = 0;

	m_nState = enWeather_Particle_State_None;
}
//------------------------------------------------------------------------
CRainParticle::~CRainParticle()
{
}
//------------------------------------------------------------------------
void CRainParticle::OnTimer()
{
}
//------------------------------------------------------------------------
void CRainParticle::OnDraw(ICanvas* pCanvas, int nLocX, int nLocY, int nWindPower, int nDownSpeed)
{
	switch(m_nState)
	{
	case enWeather_Particle_State_None:
		{
			if(!m_bRun)
				return;

			m_nState = enWeather_Particle_State_Run;

			m_ptLoc.x = nLocX;
			m_ptLoc.y = nLocY;
			
			m_dwCreateTime = ::timeGetTime();
			m_dwLastTime = m_dwCreateTime;

			m_dwCurLife = 0;

			m_nEndY = 1 + g_RandGen.GetDWord() % 800;

			// X轴起始位置
			m_ptBeginLoc.x = g_RandGen.GetDWord() % 1600 - 400;

			// Y轴起始位置
			m_ptBeginLoc.y = g_RandGen.GetDWord() % m_nEndY;
			m_ptBeginLoc.y -= 500;
			if(m_ptBeginLoc.y < 0)
				m_ptBeginLoc.y = 0;

			// X轴速度
			m_fSpeedX = nWindPower * 0.15 / 100;
			m_fSpeedX = m_fSpeedX * (1 + (float)m_nEndY / 800);

			// Y轴速度
			m_fSpeedY = 0.28 + (float)nDownSpeed / 100;
			m_fSpeedY = m_fSpeedY * (1 + (float)m_nEndY / 800);

			// 雨滴长度
			int nLength = g_RandGen.GetDWord() % 7 + 7;
			m_nLengthX = nLength * m_fSpeedX * 2.5;
			m_nLengthY = nLength * m_fSpeedY * 2.5;

			// alpha 150~255
			m_nAlpha = 20 + m_nEndY * 20 / 800 + g_RandGen.GetDWord() % 30;
		}
		break;
	
	case enWeather_Particle_State_Run:
		{
			DWORD dwTime = ::timeGetTime();
			if(dwTime - m_dwLastTime > 1000)
			{
				m_dwCreateTime += dwTime - m_dwLastTime;

			}
			m_dwLastTime = dwTime;
			
			m_dwCurLife = dwTime - m_dwCreateTime;

			m_ptDrawFrm.x = m_ptBeginLoc.x + m_dwCurLife * m_fSpeedX + m_ptLoc.x - nLocX;
			m_ptDrawFrm.y = m_ptBeginLoc.y + m_dwCurLife * m_fSpeedY + m_ptLoc.y - nLocY;

			m_ptDrawTo.x = m_ptDrawFrm.x + m_nLengthX;
			m_ptDrawTo.y = m_ptDrawFrm.y + m_nLengthY;

			pCanvas->DrawLine(m_ptDrawFrm, m_ptDrawTo, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			if(m_ptDrawTo.y > m_nEndY)
			{
				m_nOverTime = 3;
				m_nState = enWeather_Particle_State_Over;
				return;
			}
		}
		break;
	
	case enWeather_Particle_State_Over:
		{
			m_ptDrawFrm.x = m_ptBeginLoc.x + m_dwCurLife * m_fSpeedX + m_ptLoc.x - nLocX;
			m_ptDrawFrm.y = m_ptBeginLoc.y + m_dwCurLife * m_fSpeedY + m_ptLoc.y - nLocY;

			m_ptDrawTo.x = m_ptDrawFrm.x + m_nLengthX;
			m_ptDrawTo.y = m_ptDrawFrm.y + m_nLengthY;

			if(m_nOverTime > 1)
			{
				m_ptDrawFrm.x += m_nLengthX / m_nOverTime;
				m_ptDrawFrm.y += m_nLengthY / m_nOverTime;
				
				pCanvas->DrawLine(m_ptDrawFrm, m_ptDrawTo, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			}

			int nAlpha = m_nAlpha + 120;
			if(nAlpha > 255)
				nAlpha = 255;
			pCanvas->DrawPixel(m_ptDrawTo.x, m_ptDrawTo.y, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
			
			if(m_nOverTime-- < 0)
				m_nState = enWeather_Particle_State_None;
		}
		break;
		
	default:
		break;
	}
}
//------------------------------------------------------------------------
// 雪粒子
CSnowParticle::CSnowParticle()
{
	m_pWeather = NULL;

	m_bRun = FALSE;
	
	m_fSpeedX = 0;
	m_fSpeedY = 0;

	m_nEndY = 0;

	m_dwCreateTime = 0;
	m_dwCurLife = 0;
	m_nAlpha = 0;

	m_ptBeginLoc.x = m_ptBeginLoc.y = 0;
	m_ptLoc.x = m_ptLoc.y = 0;

	m_nType = 0;
	
	m_bFirst = FALSE;

	m_nState = enWeather_Particle_State_None;
}
//------------------------------------------------------------------------
CSnowParticle::~CSnowParticle()
{
}
//------------------------------------------------------------------------
void CSnowParticle::OnTimer()
{
}
//------------------------------------------------------------------------
void CSnowParticle::DrawSnow(ICanvas* pCanvas, int x, int y)
{
	int nAlpha = m_nAlpha - 100;
	if(nAlpha < 1)
		nAlpha = 1;
	switch(m_nType)
	{
//OO
// O
// O
	case 0:
		{
			pCanvas->DrawPixel(x, y, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 1, y, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 1, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 1, y + 2, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			
			pCanvas->DrawPixel(x, y + 1, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
		}
		break;
// O
//OOO
// O
	case 1:
		{
			pCanvas->DrawPixel(x + 1, y, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 1, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 2, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 1, y + 2, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);

			
			pCanvas->DrawPixel(x, y, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
			pCanvas->DrawPixel(x + 2, y, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
			pCanvas->DrawPixel(x, y + 2, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
			pCanvas->DrawPixel(x + 2, y + 2, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
		}
		break;	
//
//OOO
//
	case 2:
		{
			pCanvas->DrawPixel(x, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 1, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 2, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			
			pCanvas->DrawPixel(x + 1, y, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
			pCanvas->DrawPixel(x + 1, y + 2, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
		}
		break;	
//  00
//0O0
	case 3:
		{
			pCanvas->DrawPixel(x + 2, y, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 3, y, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 1, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 2, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);

			
			pCanvas->DrawPixel(x + 1, y, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
			pCanvas->DrawPixel(x + 3, y + 1, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
		}
		break;	
//   00
//OO0
//  00	
	case 4:
		{
			pCanvas->DrawPixel(x + 3, y, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 4, y, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 1, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 2, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 2, y + 2, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 3, y + 2, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			
			pCanvas->DrawPixel(x + 2, y, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
			pCanvas->DrawPixel(x + 3, y + 1, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
			pCanvas->DrawPixel(x + 1, y + 2, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
		}
		break;
//OO
//  000
	case 5:
		{
			pCanvas->DrawPixel(x, y, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 1, y, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 2, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 3, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			pCanvas->DrawPixel(x + 4, y + 1, P_WHITE_COLOR, BLIT_TRANS, m_nAlpha);
			
			pCanvas->DrawPixel(x + 2, y, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
			pCanvas->DrawPixel(x + 1, y + 1, P_WHITE_COLOR, BLIT_TRANS, nAlpha);
		}
		break;
		
	default:
		break;
	}
}
//------------------------------------------------------------------------
void CSnowParticle::OnDraw(ICanvas* pCanvas, int nLocX, int nLocY, int nWindPower, int nDownSpeed)
{
	switch(m_nState)
	{
	case enWeather_Particle_State_None:
		{
			if(!m_bRun)
				return;

			m_nState = enWeather_Particle_State_Run;

			m_ptLoc.x = nLocX;
			m_ptLoc.y = nLocY;

			m_nEndY = 1 + g_RandGen.GetDWord() % 800;

			m_ptBeginLoc.x = g_RandGen.GetDWord() % 2400 - 800;

			m_ptBeginLoc.y = g_RandGen.GetDWord() % m_nEndY;
			m_ptBeginLoc.y -= 100;
			if(m_ptBeginLoc.y < 0)
				m_ptBeginLoc.y = 0;

			// X轴速度-0.15~0.15
			m_fSpeedX = nWindPower * 0.2 / 100;
			m_fSpeedX = m_fSpeedX * (0.2 + m_nEndY * 0.8 / 800);

			// Y轴速度0.01~0.10 - 0.03~0.30
			m_fSpeedY = 0.015 + (float)nDownSpeed / 500;
			m_fSpeedY = m_fSpeedY + m_nEndY * m_fSpeedY * 10 / 800;

			m_dwCurLife = 0;

			m_dwCreateTime = ::timeGetTime();

			// alpha
			m_nAlpha = 100 + m_nEndY * 120 / 800 + g_RandGen.GetDWord() % 30;

			m_nType = m_nEndY * 6 / 800;

			m_fFlowX = 0;
			if(g_RandGen.GetDWord() % 2 == 0)
				m_fAddFlowX = 0.91f; 
			else
				m_fAddFlowX = -0.91f;
			m_bFirst = FALSE;
		}
		break;
	
	case enWeather_Particle_State_Run:
		{
			m_dwCurLife = ::timeGetTime() - m_dwCreateTime;

			int nBeginX = m_ptBeginLoc.x + m_ptLoc.x - nLocX;
			int nBeginY = m_ptBeginLoc.y + m_ptLoc.y - nLocY;
			int nDrawX = nBeginX + m_dwCurLife * m_fSpeedX;
			int nDrawY = nBeginY + m_dwCurLife * m_fSpeedY;
			
			if(!m_bFirst && nDrawX > 0 && nDrawX < 800 && nDrawY > 0 && nDrawY < 600)
			{
				m_bFirst = TRUE;
			}

			m_fFlowX += m_fAddFlowX;
			if(m_fFlowX > 0 && g_RandGen.GetDWord() % 2 == 0)
				m_fAddFlowX -= 0.1f;
			else if(m_fFlowX < 0 && g_RandGen.GetDWord() % 2 == 0)
				m_fAddFlowX += 0.1f;

			DrawSnow(pCanvas, nDrawX + m_fFlowX, nDrawY);

			if(nDrawY < -10)
			{
				m_nState = enWeather_Particle_State_None;
				return;
			}
			if(m_bFirst && (nDrawX < -10 || nDrawX > 810 || nDrawY > 610))
			{
				m_nState = enWeather_Particle_State_None;
				return;
			}

			if(nDrawY > m_nEndY)
			{
				m_nState = enWeather_Particle_State_None;
				return;
			}
		}
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------
// 玫瑰花瓣
CMeiGuiParticle::CMeiGuiParticle()
{
	m_pWeather = NULL;

	m_bRun = FALSE;
	
	m_fSpeedX = 0;
	m_fSpeedY = 0;

	m_nEndY = 0;

	m_dwCreateTime = 0;
	m_dwCurLife = 0;
	m_nAlpha = 0;

	m_ptBeginLoc.x = m_ptBeginLoc.y = 0;
	m_ptLoc.x = m_ptLoc.y = 0;

	m_nType = 0;
	
	m_bFirst = FALSE;

	m_nState = enWeather_Particle_State_None;
}
//------------------------------------------------------------------------
CMeiGuiParticle::~CMeiGuiParticle()
{
}
//------------------------------------------------------------------------
void CMeiGuiParticle::OnTimer()
{
}
//------------------------------------------------------------------------
void CMeiGuiParticle::DrawSnow(ICanvas* pCanvas, int x, int y)
{
	if(m_pWeather == NULL)
		return;
	
	if(m_pWeather->m_pMeiGui[m_nType] != NULL)
	{
		m_pWeather->m_pMeiGui[m_nType]->Draw(pCanvas, x, y);
	}
}
//------------------------------------------------------------------------
void CMeiGuiParticle::OnDraw(ICanvas* pCanvas, int nLocX, int nLocY, int nWindPower, int nDownSpeed)
{
	switch(m_nState)
	{
	case enWeather_Particle_State_None:
		{
			if(!m_bRun)
				return;

			m_nState = enWeather_Particle_State_Run;

			m_ptLoc.x = nLocX;
			m_ptLoc.y = nLocY;

			m_nEndY = 1 + g_RandGen.GetDWord() % 800;

			m_ptBeginLoc.x = g_RandGen.GetDWord() % 1600 - 400;

			m_ptBeginLoc.y = g_RandGen.GetDWord() % m_nEndY;
			m_ptBeginLoc.y -= 100;
			if(m_ptBeginLoc.y < 0)
				m_ptBeginLoc.y = 0;

			// X轴速度-0.15~0.15
			m_fSpeedX = nWindPower * 0.2 / 100;
			m_fSpeedX = m_fSpeedX * (0.2 + m_nEndY * 0.8 / 800);

			// Y轴速度0.01~0.10 - 0.03~0.30
			m_fSpeedY = 0.015 + (float)nDownSpeed / 500;
			m_fSpeedY = m_fSpeedY + m_nEndY * m_fSpeedY * 10 / 800;

			m_dwCurLife = 0;

			m_dwCreateTime = ::timeGetTime();

			// alpha
			m_nAlpha = 100 + m_nEndY * 120 / 800 + g_RandGen.GetDWord() % 30;

			m_nType = m_nEndY * 7 / 800;
			m_nType = m_nType + 1 - g_RandGen.GetDWord() % 3;
			if(m_nType > 5)
				m_nType = 5;
			else if(m_nType < 0)
				m_nType = 0;

			m_fFlowX = 0;
			if(g_RandGen.GetDWord() % 2 == 0)
				m_fAddFlowX = 0.91f; 
			else
				m_fAddFlowX = -0.91f;
			m_bFirst = FALSE;
		}
		break;
	
	case enWeather_Particle_State_Run:
		{
			m_dwCurLife = ::timeGetTime() - m_dwCreateTime;

			int nBeginX = m_ptBeginLoc.x + m_ptLoc.x - nLocX;
			int nBeginY = m_ptBeginLoc.y + m_ptLoc.y - nLocY;
			int nDrawX = nBeginX + m_dwCurLife * m_fSpeedX;
			int nDrawY = nBeginY + m_dwCurLife * m_fSpeedY;
			
			if(!m_bFirst && nDrawX > 0 && nDrawX < 800 && nDrawY > 0 && nDrawY < 600)
			{
				m_bFirst = TRUE;
			}

			m_fFlowX += m_fAddFlowX;
			if(m_fFlowX > 0 && g_RandGen.GetDWord() % 2 == 0)
				m_fAddFlowX -= 0.1f;
			else if(m_fFlowX < 0 && g_RandGen.GetDWord() % 2 == 0)
				m_fAddFlowX += 0.1f;

			DrawSnow(pCanvas, nDrawX + m_fFlowX, nDrawY);

			if(nDrawY < -10)
			{
				m_nState = enWeather_Particle_State_None;
				return;
			}
			if(m_bFirst && (nDrawX < -10 || nDrawX > 810 || nDrawY > 610))
			{
				m_nState = enWeather_Particle_State_None;
				return;
			}

			if(nDrawY > m_nEndY)
			{
				m_nOverTime = 50 + g_RandGen.GetDWord() % 200;
				m_ptOverLoc.x = nDrawX + m_fFlowX + nLocX;
				m_ptOverLoc.y = nDrawY + nLocY;
				m_nState = enWeather_Particle_State_Over;
				return;
			}
		}
		break;

	
	case enWeather_Particle_State_Over:
		{
			DrawSnow(pCanvas, m_ptOverLoc.x - nLocX, m_ptOverLoc.y - nLocY);

			if(m_nOverTime-- < 0)
				m_nState = enWeather_Particle_State_None;
		}
		break;
		
	default:
		break;
	}
}
//------------------------------------------------------------------------