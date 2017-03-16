/**
*	created:		2012-7-1   5:51
*	filename: 		Weather
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		天气系统
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKCanvas/IFKCanvas.h"
#include "../../FKGame/MapView/IMapView.h"
#include "../../FKGame/Thread/IThreadRoute.h"
#include "WeatherParticle.h"
//------------------------------------------------------------------------
#define MAX_WEATHER_PARTICLE_NUM	1000
//------------------------------------------------------------------------
// 天气控制类
class CWeather
{
public:
	CWeather();
	virtual ~CWeather();
public:
	BOOL	Create(LPCSTR szDir);
	void	Release() {delete this;}

	void	ChangeWeather(SWeatherInfo info);
	BOOL	NeedDraw();
	void	OnDraw(ICanvas* pCanvas, int nLocX, int nLocY);
public:
	enWeatherType		m_nWeatherType;								// 当前天气类型

	BOOL				m_bRandom;									// 是否随机
	int					m_nNextParticleNum;							// 当前粒子数
	int					m_nCurParticleNum;							// 当前粒子数
	int					m_nWindPower;								// 风力
	int					m_nDownSpeed;								// 下降速度
	CRainParticle		m_RainParticle[MAX_WEATHER_PARTICLE_NUM];	// 下雨粒子
	CSnowParticle		m_SnowParticle[MAX_WEATHER_PARTICLE_NUM];	// 雪粒子
	CMeiGuiParticle		m_MeiGuiParticle[MAX_WEATHER_PARTICLE_NUM];	// 玫瑰花瓣

	IBitmap*			m_pMeiGui[5];
};
//------------------------------------------------------------------------