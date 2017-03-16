/**
*	created:		2012-7-2   1:05
*	filename: 		Particle
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/Particle.h"
#include "../../FKCanvas/IFKCanvas.h"
#include "../Include/ParticleSetting.h"
#include <sys/timeb.h>
//------------------------------------------------------------------------
CParticle::CParticle()
{	
	m_pPoolNodeObj = NULL;

	m_nPartAlpha = 255;

	m_nLife = 10;
	m_nPerPtcNum = 3;
	m_dwResID = 0;

	m_nXSpeedMin = 0;
	m_nXSpeedMax = 0;
	m_nYSpeedMin = 0;
	m_nYSpeedMax = 0;

	m_nXAddSpeed = 0;
	m_nYAddSpeed = 0;

	m_nXDrawOffset = 0;
	m_nYDrawOffset = 0;

	m_fNowCreateNum = 0.0f;

	m_nXCaiYang = 3;
	m_nYCaiYang = 3;

	m_nAlphaBegin = 255;
	m_nAlphaEnd = 255;

	m_nCurParticleNum = 0;

	memset(m_particles,0,sizeof(m_particles));
}
//------------------------------------------------------------------------
CParticle::CParticle(const CParticle & particle)
{
	m_pPoolNodeObj = particle.m_pPoolNodeObj;

	m_nPartAlpha = particle.m_nPartAlpha;

	m_nLife = particle.m_nLife;
	m_nPerPtcNum = particle.m_nPerPtcNum;
	m_dwResID = particle.m_dwResID;

	m_nXSpeedMin = particle.m_nXSpeedMin;
	m_nXSpeedMax = particle.m_nXSpeedMax;
	m_nYSpeedMin = particle.m_nYSpeedMin;
	m_nYSpeedMax = particle.m_nYSpeedMax;

	m_nXAddSpeed = particle.m_nXAddSpeed;
	m_nYAddSpeed = particle.m_nYAddSpeed;

	m_nXCaiYang = particle.m_nXCaiYang;
	m_nYCaiYang = particle.m_nYCaiYang;

	m_nXDrawOffset = particle.m_nXDrawOffset;
	m_nYDrawOffset = particle.m_nYDrawOffset;

	m_nAlphaBegin = particle.m_nAlphaBegin;
	m_nAlphaEnd = particle.m_nAlphaEnd;

	m_fNowCreateNum = 0.0f;
	m_nCurParticleNum = 0;
	memset(m_particles,0,sizeof(m_particles));
}
//------------------------------------------------------------------------
CParticle::~CParticle()
{
}
//------------------------------------------------------------------------
void CParticle::CloseRes()
{
	if (m_pPoolNodeObj)
	{
		g_ResLoader.ReleaseRes(ioMagic + m_dwResID);
		m_pPoolNodeObj = NULL;
	}
}
//------------------------------------------------------------------------
CAni* CParticle::RequestRes()
{
	if (m_pPoolNodeObj)
		return m_pPoolNodeObj;

	m_pPoolNodeObj = static_cast<CAni*>(g_ResLoader.RequestMagic(m_dwResID));

	return m_pPoolNodeObj;
}
//------------------------------------------------------------------------
void CParticle::AddNewParticle(CParticleMask * pMask,POINT ptPos)
{
	if(m_nCurParticleNum >= (MAX_PARTICLE_COUNT-1))
		return;

	//生产新的粒子
	int nSize = pMask->Size();
	if(nSize == 0)
		return;

	PARTICLE * pPos = m_particles + m_nCurParticleNum;

	int dvx = m_nXSpeedMax - m_nXSpeedMin;
	int dvy = m_nYSpeedMax - m_nYSpeedMin;

	POINTS ptTemp;
	POINT ptTile;
	POINT ptTileCenter;
	POINT ptOffset;
	g_pMapView->ScreenToTile(ptPos,ptTile);
	g_pMapView->TileToScreen(ptTile,ptTileCenter);
	ptOffset.x = ptPos.x - ptTileCenter.x;
	ptOffset.y = ptPos.y - ptTileCenter.y;
	for(int index = 0;
		index < m_nPerPtcNum && m_nCurParticleNum < MAX_PARTICLE_COUNT;
		++index, ++pPos, ++m_nCurParticleNum)
	{
		ptTemp			= pMask->GetPoint(rand()%nSize);
		pPos->x			= ptOffset.x+ptTemp.x;
		pPos->y			= ptOffset.y+ptTemp.y;
		pPos->ptTile	= ptTile;
		pPos->vx		= dvx > 0 ? (((rand()%dvx)+m_nXSpeedMin) / 100.f) : 0;
		pPos->vy		= dvy > 0 ? (((rand()%dvy)+m_nYSpeedMin) / 100.f) : 0;
		pPos->life		= m_nLife;
		pPos->Curlife	= 0;
		pPos->alpha		= (float)m_nAlphaBegin;
		pPos->valpha	= (float)(m_nAlphaBegin - m_nAlphaEnd) / m_nLife / 3;
	}
}
//------------------------------------------------------------------------
BOOL CParticle::GenNext()
{
	//迭代现有的粒子
	PARTICLE * pPos = m_particles;
	PARTICLE * pCur = m_particles;
	int newSize = m_nCurParticleNum;
	for( ; pCur != m_particles + m_nCurParticleNum; ++pCur)
	{
		if(pCur->Curlife >= pCur->life * 3)
		{
			//粒子死亡
			--newSize;
			continue;
		}else
		{
			pPos->x			= pCur->x + pCur->vx;
			pPos->y			= pCur->y + pCur->vy;
			pPos->vx		= pCur->vx + (float)m_nXAddSpeed / 100.f;
			pPos->vy		= pCur->vy + (float)m_nYAddSpeed / 100.f;
			pPos->ptTile	= pCur->ptTile;
			pPos->alpha		= pCur->alpha - pCur->valpha;

			//粒子成长
			pPos->Curlife = pCur->Curlife + 1;

			++pPos;
		}
	}
	m_nCurParticleNum = newSize;

	return true;
}
//------------------------------------------------------------------------
void CParticle::Draw(ICanvas * pCanvas,int x,int y,int alpha)
{
	if (!pCanvas || !RequestRes())
		return;

	POINT ptScreen;
	POINT ptTile;
	for(int index=0;index<m_nCurParticleNum;++index)
	{
		int offx, offy;
		m_pPoolNodeObj->GetLowOffset(m_particles[index].Curlife / 3, offx, offy);
		ptTile.x = m_particles[index].ptTile.x;
		ptTile.y = m_particles[index].ptTile.y;
		g_pMapView->TileToScreen(ptTile,ptScreen);
		m_pPoolNodeObj->ParticleDraw(pCanvas,
			ptScreen.x + offx + m_particles[index].x - m_nXDrawOffset,
			ptScreen.y + offy + m_particles[index].y - m_nYDrawOffset,
			m_particles[index].Curlife / 3, BLIT_MASK | BLIT_TRANS, m_pPoolNodeObj->GetPal(), (int)m_particles[index].alpha);
	}
}
//------------------------------------------------------------------------
void CParticle::GetSimpling(POINTS & pt)
{
	pt.x = m_nXCaiYang;
	pt.y = m_nYCaiYang;
}
//------------------------------------------------------------------------
CParticle * CParticle::Create(int nParticle)
{
	CParticle * pPartcileTemp = CParticleSetting::GetInstance()->GetParticle(nParticle);
	if(!pPartcileTemp)
		return NULL;
	std::auto_ptr<CParticle> pPartcile(new CParticle(*pPartcileTemp));
	return pPartcile.release();
}
//------------------------------------------------------------------------
BOOL CParticle::Release()
{
	delete this;
	return true;
}
//------------------------------------------------------------------------
CParticleMask::CParticleMask()
{
 
}
//------------------------------------------------------------------------
BOOL CParticleMask::Create(int dx,int dy,IRleBitmap * pBitmap,BYTE * pPal)
{
	m_MaskPoints.clear();
	int height = pBitmap->GetHeight(),
		width = pBitmap->GetWidth();
	IBitmap * pTempBitmap = NULL;

	if(!g_CanvasHelper.CreateIBitmap(&pTempBitmap) )
	{
		return false;
	}
	if(!pTempBitmap->Create(width,height))
	{
		pTempBitmap->Release();
		return false;
	}

	pTempBitmap->Clear(WPixel(255,255,255));

	pTempBitmap->DrawRle(0,0,width,height,pBitmap,pPal,0,0);

	POINT pt;
	for(pt.y = 0; pt.y < height; pt.y += dy)
	{
		WPixel * pPixel = (*pTempBitmap)[pt.y];
		for(pt.x=0; pt.x<width; pt.x+=dx)
		{
			if(!pBitmap->IsTransparent(pt))
			{
				COLORREF cr = pPixel[pt.x];
				if(!isGray565(pPixel[pt.x].red,pPixel[pt.x].green,pPixel[pt.x].blue))
				{
					POINTS ptMask={pt.x,pt.y};
					m_MaskPoints.push_back(ptMask);
				}
			}
		}
	}
	pTempBitmap->Release();
	return true;
}
//------------------------------------------------------------------------
CMultiParticleMask::CMultiParticleMask():m_pMask(NULL),m_nCount(0)
{
}
//------------------------------------------------------------------------
CMultiParticleMask::~CMultiParticleMask()
{
	if(m_pMask)
	{
		delete [] m_pMask;
		m_pMask = NULL;
	}
}
//------------------------------------------------------------------------
BOOL CMultiParticleMask::Create(CParticle * pParticle,IRleBitmap ** ppBitmap,int cBitmap,BYTE * pPal)
{
	_timeb begin;
	_ftime(&begin);
	if(cBitmap<=0)
		return false;

	if(m_pMask)
	{
		delete m_pMask;
		m_pMask = NULL;
		m_nCount = 0;
	}

	m_pMask = new CParticleMask[cBitmap];
	POINTS ptSimpling ;
	pParticle->GetSimpling(ptSimpling);
	for(int index = 0; index<cBitmap;++index)
	{
		if(ppBitmap[index]&&!m_pMask[index].Create(ptSimpling.x,ptSimpling.y,ppBitmap[index],pPal))
			return false;
	}
	m_nCount = cBitmap;
	_timeb end;
	_ftime(&end);

	int nTime = (end.time-begin.time)*1000 + (end.millitm-begin.millitm);

	return true;
}
//------------------------------------------------------------------------
CParticleMask * CMultiParticleMask::GetMask(int index)
{
	if(index>=m_nCount)
		return NULL;
	return &m_pMask[index];
}
//------------------------------------------------------------------------