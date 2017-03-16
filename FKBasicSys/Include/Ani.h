/**
*	created:		2012-7-1   20:17
*	filename: 		Ani
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		动画包
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "BasicCommon.h"
#include "ObjPool.h"
#include "../../FKGame/SceneObj/IItemClassFactory.h"
#include "Structs.h"
#include "AniLoader.h"
#include <map>
//------------------------------------------------------------------------
class CMultiParticleMask;
class CParticle;
class CParticleMask;
//------------------------------------------------------------------------
class CAni : public IPoolNodeObject // 32bytes
{
public:
	typedef std::map<int,CMultiParticleMask*> MaskSet;
	typedef MaskSet::iterator mask_iterator;
	
	enLoadState			m_nLoadState;
	DWORD				m_dwResID;
	DWORD				m_dwSize;

	stAniInfoHeader		m_AniInfoHdr;				// 动画信息头
	stFrameIndex*		m_paFrameIndex;				// 动画帧索引
	IFileObject*		m_pFile;					// 文件对象
	IBitmap**			m_ppaIBitmap;				// 帧图形数据
	BYTE*				m_pPal;						// 公用调色板
	POINT				m_ptLowOffset;

public:
	CAni();
	virtual ~CAni();
	
	BOOL Open(IFileSystem* pFileSystem, LPCSTR szName, DWORD dwResID);
	void ParticleDraw(ICanvas *pCanvas, int nX, int nY, int nFrame, int nFlag, BYTE* pPal=NULL, BYTE nAlpha = 255);
	void Draw(ICanvas *pCanvas, int nX, int nY, int nFrame, int nFlag, BYTE* pPal=NULL, CParticle * pParticle=NULL);
	void Draw(ICanvas *pCanvas, int nX, int nY, int nFrame, int nFlag, BYTE nAlpha, BYTE* pPal=NULL, CParticle * pParticle=NULL);

	// 所有帧属性
	int GetTotalFrames(){return m_AniInfoHdr.total_frame;}

	// 单帧属性
	IBitmap* GetBitmap(int nFrame);
	int GetWidth(int nFrame);
	int GetHeight(int nFrame);
	void GetOffset(int nFrame, int& nOffsetX,int& nOffsetY);
	void GetLowOffset(int nFrame, int& nOffsetX,int& nOffsetY);

	BYTE* GetPal() {return m_pPal;}
	int GetUseColor() {return m_AniInfoHdr.use_color;}
	
	virtual DWORD GetSize(){return m_dwSize;}
	virtual void Release(){delete this;}
	
	BOOL GetData(BYTE* pBuffer, DWORD dwBufferSize);
};
//------------------------------------------------------------------------