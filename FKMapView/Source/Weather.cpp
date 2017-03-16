/**
*	created:		2012-7-1   5:52
*	filename: 		Weather
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/Weather.h"
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/MapViewCommon.h"
//------------------------------------------------------------------------
CWeather::CWeather()
{
	m_nWeatherType = enWeather_Type_None;

	m_nNextParticleNum = 100;	
	m_nWindPower = 0;	
	m_nDownSpeed = 5;
	m_nCurParticleNum = 0;	
	m_bRandom = TRUE;

	for(int i = 0; i < MAX_WEATHER_PARTICLE_NUM; i++)
	{
		m_RainParticle[i].Create(this);
		m_SnowParticle[i].Create(this);
		m_MeiGuiParticle[i].Create(this);
	}

	for(int i = 0; i < 5; i++)
	{
		m_pMeiGui[i] = NULL;
	}
}
//------------------------------------------------------------------------
CWeather::~CWeather()
{
 
}
//------------------------------------------------------------------------
BOOL CWeather::Create(LPCSTR szDir)
{
	char szTempPath [MAX_PATH];
	for(int i = 0; i < 6; i++)
	{
		sprintf(szTempPath, "%s\\MeiGui0%1d.tga", szDir, i+1);
		m_pMeiGui[i] = CImageLoader::GetInstance()->LoadImage(szTempPath);
	}

	return TRUE;
}
//------------------------------------------------------------------------
void CWeather::ChangeWeather(SWeatherInfo info)
{
	if(info.nWeatherType != enWeather_Type_None)
	{
		for(int i = 0; i < MAX_WEATHER_PARTICLE_NUM; i++)
		{
			m_RainParticle[i].Stop();		
			m_SnowParticle[i].Stop();	
			m_MeiGuiParticle[i].Stop();
		}
		m_nCurParticleNum = 0;

		m_nWeatherType = info.nWeatherType;
		m_bRandom = info.bRandom;
		if(m_bRandom)
		{
			m_nNextParticleNum = g_RandGen.GetDWord() % MAX_WEATHER_PARTICLE_NUM;
			if(m_nNextParticleNum < 50)
				m_nNextParticleNum = 50;

			m_nWindPower = 100 - g_RandGen.GetDWord() % 201;
			m_nDownSpeed = 1 + g_RandGen.GetDWord() % 10;
		}
		else
		{
			m_nNextParticleNum = info.nParticleNum;
			m_nWindPower = info.nWindPower;
			m_nDownSpeed = info.nSpeed;

			if(m_nNextParticleNum < 50)
				m_nNextParticleNum = 50;
			else if(m_nNextParticleNum > MAX_WEATHER_PARTICLE_NUM)
				m_nNextParticleNum = MAX_WEATHER_PARTICLE_NUM;

			if(m_nWindPower < -100)
				m_nWindPower = -100;
			else if(m_nWindPower > 100)
				m_nWindPower = 100;

			if(m_nDownSpeed < 1)
				m_nDownSpeed = 1;
			else if(m_nDownSpeed > 10)
				m_nDownSpeed = 10;
		}
	}
	else
	{
		m_bRandom = FALSE;
		m_nNextParticleNum = 0;
	}
}
//------------------------------------------------------------------------
BOOL CWeather::NeedDraw()
{
	if(m_nWeatherType == enWeather_Type_None)
		return FALSE;

	if(m_nCurParticleNum <= 0 && m_nNextParticleNum <= 0)
		return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------
void CWeather::OnDraw(ICanvas* pCanvas, int nLocX, int nLocY)
{
	// 定好下个天气情况
	if(m_bRandom && g_RandGen.GetDWord() % 1000 == 0)
	{
		m_nNextParticleNum = g_RandGen.GetDWord() % MAX_WEATHER_PARTICLE_NUM;
		if(m_nNextParticleNum < 50)
			m_nNextParticleNum = 50;

		if(g_RandGen.GetDWord() % 2 == 0)
		{
			m_nWindPower += 20;
			if(m_nWindPower > 100)
				m_nWindPower = 100;
		}
		else
		{
			m_nWindPower -= 20;
			if(m_nWindPower < -100)
				m_nWindPower = -100;
		}

		if(g_RandGen.GetDWord() % 2 == 0)
		{
			m_nDownSpeed ++;
			if(m_nDownSpeed > 10)
				m_nDownSpeed = 10;
		}
		else
		{
			m_nDownSpeed --;
			if(m_nDownSpeed < 1)
				m_nDownSpeed = 1;
		}
	}

	if(m_nCurParticleNum > m_nNextParticleNum)
	{
		m_nCurParticleNum--;
		m_RainParticle[m_nCurParticleNum].Stop();
		m_SnowParticle[m_nCurParticleNum].Stop();
		m_MeiGuiParticle[m_nCurParticleNum].Stop();
	}
	else if(m_nCurParticleNum < m_nNextParticleNum)
	{
		m_RainParticle[m_nCurParticleNum].Start();
		m_SnowParticle[m_nCurParticleNum].Start();
		m_MeiGuiParticle[m_nCurParticleNum].Start();
		m_nCurParticleNum++;
	}

	int i;
	switch(m_nWeatherType)
	{
	case enWeather_Type_Rain:
		for(i = 0; i < MAX_WEATHER_PARTICLE_NUM; i++)
		{
			m_RainParticle[i].OnDraw(pCanvas, nLocX, nLocY, m_nWindPower, m_nDownSpeed);
		}
		break;

	case enWeather_Type_Snow:
		for(i = 0; i < MAX_WEATHER_PARTICLE_NUM; i++)
		{
			m_SnowParticle[i].OnDraw(pCanvas, nLocX, nLocY, m_nWindPower, m_nDownSpeed);
		}
		break;

	case enWeather_Type_MeiGui:
		for(i = 0; i < MAX_WEATHER_PARTICLE_NUM; i++)
		{
			m_MeiGuiParticle[i].OnDraw(pCanvas, nLocX, nLocY, m_nWindPower, m_nDownSpeed);
		}
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------