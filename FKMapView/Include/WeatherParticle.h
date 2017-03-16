/**
*	created:		2012-7-1   5:47
*	filename: 		WeatherParticle
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		天气系统粒子
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKCanvas/IFKCanvas.h"
//------------------------------------------------------------------------
#define P_WHITE_COLOR		WPixel(255,255,255)
//------------------------------------------------------------------------
enum
{
	enWeather_Particle_State_None = 0,				// 未生成阶段
	enWeather_Particle_State_Run,					// 运行阶段
	enWeather_Particle_State_Over,					// 结束阶段
};
//------------------------------------------------------------------------
class CWeather;
//------------------------------------------------------------------------
// 雨粒子
class CRainParticle
{
public:
	CRainParticle();
	virtual ~CRainParticle();

	void	Create(CWeather *pWeather)	{m_pWeather = pWeather;}
	void	Stop()		{m_bRun = FALSE;}
	void	Start()		{m_bRun = TRUE;}
	void	OnDraw(ICanvas* pCanvas, int nLocX, int nLocY, int nWindPower, int nDownSpeed);

	void	OnTimer();

public:
	CWeather *m_pWeather;

	BOOL	m_bRun;				// 是否运行
	float	m_fSpeedX;			// X轴速度
	float	m_fSpeedY;			// Y轴速度
	int		m_nLengthX;			// X轴长度
	int		m_nLengthY;			// Y轴长度
	int		m_nEndY;			// Y轴结束位置
	DWORD	m_dwCreateTime;		// 出生时间
	DWORD	m_dwLastTime;		// 上次时间
	DWORD	m_dwCurLife;		// 当前生存时间
	POINT	m_ptLoc;			// 在哪里生成的
	POINT	m_ptBeginLoc;		// 开始位置
	POINT	m_ptDrawFrm;		// 画线前端
	POINT	m_ptDrawTo;			// 画线后端
	int		m_nAlpha;			// 透明值
	int		m_nState;			// 粒子当前所处阶段
	int		m_nOverTime;		// 结束控制
};
//------------------------------------------------------------------------
// 雪粒子
class CSnowParticle
{
public:
	CSnowParticle();
	virtual ~CSnowParticle();

	void	Create(CWeather *pWeather)	{m_pWeather = pWeather;}
	void	Stop()		{m_bRun = FALSE;}
	void	Start()		{m_bRun = TRUE;}
	void	DrawSnow(ICanvas* pCanvas, int x, int y);
	void	OnDraw(ICanvas* pCanvas, int nLocX, int nLocY, int nWindPower, int nDownSpeed);

	void	OnTimer();

public:
	CWeather *m_pWeather;

	BOOL	m_bRun;				// 是否运行
	float	m_fSpeedX;			// X轴速度
	float	m_fSpeedY;			// Y轴速度
	int		m_nEndY;			// Y轴结束位置
	float	m_fFlowX;			// 当前飘落的偏移
	float	m_fAddFlowX;		// 当前飘落的偏移
	DWORD	m_dwCreateTime;		// 出生时间
	DWORD	m_dwCurLife;		// 当前生存时间
	POINT	m_ptLoc;			// 在哪里生成的
	POINT	m_ptBeginLoc;		// 开始位置
	int		m_nAlpha;			// 透明值
	int		m_nState;			// 粒子当前所处阶段
	int		m_nType;			// 当前是那种类型的雪花
	BOOL	m_bFirst;			// 是否显示过
};
//------------------------------------------------------------------------
// 玫瑰花瓣
class CMeiGuiParticle
{
public:
	CMeiGuiParticle();
	virtual ~CMeiGuiParticle();

	void	Create(CWeather *pWeather)	{m_pWeather = pWeather;}
	void	Stop()		{m_bRun = FALSE;}
	void	Start()		{m_bRun = TRUE;}
	void	DrawSnow(ICanvas* pCanvas, int x, int y);
	void	OnDraw(ICanvas* pCanvas, int nLocX, int nLocY, int nWindPower, int nDownSpeed);

	void	OnTimer();

public:
	CWeather *m_pWeather;

	BOOL	m_bRun;				// 是否运行
	float	m_fSpeedX;			// X轴速度
	float	m_fSpeedY;			// Y轴速度
	int		m_nEndY;			// Y轴结束位置
	float	m_fFlowX;			// 当前飘落的偏移
	float	m_fAddFlowX;		// 当前飘落的偏移
	DWORD	m_dwCreateTime;		// 出生时间
	DWORD	m_dwCurLife;		// 当前生存时间
	POINT	m_ptLoc;			// 在哪里生成的
	POINT	m_ptBeginLoc;		// 开始位置
	int		m_nAlpha;			// 透明值
	int		m_nState;			// 粒子当前所处阶段
	int		m_nType;			// 当前是那种类型的雪花
	BOOL	m_bFirst;			// 是否显示过
	POINT	m_ptOverLoc;		// 在哪里生成的
	int		m_nOverTime;		// 结束控制
};
//------------------------------------------------------------------------