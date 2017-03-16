/**
*	created:		2012-7-2   1:03
*	filename: 		Particle
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <list>
#include <vector>
#include <string>
#include "Ani.h"
#include "ResLoader.h"
using std::list;
using std::vector;
using std::string;
//------------------------------------------------------------------------
#define MAX_PARTICLE_COUNT 500
//------------------------------------------------------------------------
struct IRleBitmap;
struct IBitmap;
struct ICanvas;
//------------------------------------------------------------------------
struct PARTICLE
{
	float x;				// 初始位置
	float y;				// 初始位置
	float vx;				// 水平速度
	float vy;				// 垂直速度
	POINT ptTile;			// 世界坐标
	unsigned char life;		// 生命
	unsigned char Curlife;	
	float alpha;			// 透明度
	float valpha;			// 透明度衰减
};
//------------------------------------------------------------------------
class CParticleMask
{
public:
	CParticleMask();
	//对图片采样，采样的结果用于粒子生成的基准，dx,dy分别对应采样的密度
	//即水平和垂直方向每隔多少个像素进行一次采样。
	BOOL Create(int dx,int dy,IRleBitmap * pBitmap,BYTE *pPal);

	int Size()const
	{
		return m_MaskPoints.size();
	}

	POINTS GetPoint(int index)const
	{
		return m_MaskPoints[index];
	}

	inline bool isGray565(short r,short g,short b)
	{
		return (abs((r<<1)-(g>>1)-b)+abs(g-r-b)+abs((b<<1)-(g>>1)-r))<6;
	}

private:
	std::vector<POINTS> m_MaskPoints;
};
//------------------------------------------------------------------------
class CParticle
{
public:
	CParticle();
	CParticle(const CParticle & particle);
	virtual ~CParticle();

	void CloseRes();
	CAni *RequestRes();
	void AddNewParticle(CParticleMask * pMask,POINT ptOffset);
	BOOL GenNext();
	void Draw(ICanvas * pCanvas,int x,int y,int alpha);
	void GetSimpling(POINTS & ptSimpling);
	static CParticle* Create(int nParticle);
	BOOL Release();

private:
	float	m_fNowCreateNum;

public:
	DWORD	m_dwResID;			// 资源ID，用于向资源加载器请求图象资源
	CAni*	m_pPoolNodeObj;		// 图片内存，当此图片移出可见区域时将被丢进缓存里

	char m_szName[250];
	int m_nPartAlpha;
	int m_nLife;
	int m_nPerPtcNum;

	int m_nXSpeedMin;
	int m_nXSpeedMax;
	int m_nYSpeedMin;
	int m_nYSpeedMax;
	int m_nXAddSpeed;
	int m_nYAddSpeed;
	int m_nXCaiYang;
	int m_nYCaiYang;
	int m_nXDrawOffset;
	int m_nYDrawOffset;
	int m_nAlphaBegin;
	int m_nAlphaEnd;

	int m_nCurParticleNum;
	PARTICLE m_particles[MAX_PARTICLE_COUNT];
	friend class CParticleSetting;
};
//------------------------------------------------------------------------
class CMultiParticleMask
{
public:
	CMultiParticleMask();
	virtual ~CMultiParticleMask();
	BOOL Create(CParticle * pParticle,IRleBitmap ** ppBitmap,int cBitmap,BYTE * pPal);

	CParticleMask * GetMask(int index);
private:
	CParticleMask * m_pMask;
	int m_nCount;
};
//------------------------------------------------------------------------