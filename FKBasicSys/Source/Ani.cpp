/**
*	created:		2012-7-1   21:23
*	filename: 		Ani
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/Ani.h"
#include <stdio.h>
#include "../Include/Particle.h"
//------------------------------------------------------------------------
extern CAniLoader* g_pAniLoader;
//------------------------------------------------------------------------
CAni::CAni()
{
	m_pFile			= NULL;
	m_paFrameIndex	= NULL;	
	m_ppaIBitmap	= NULL;
	m_pPal			= NULL;
	m_dwResID		= 0;
	m_nLoadState	= enLOAD_NONE;
	m_dwSize		= 0;
}
//------------------------------------------------------------------------
CAni::~CAni()
{
	if(m_nLoadState == enLOAD_ING)
	{
		g_pAniLoader->RemoveNode(m_dwResID);
	}

	SAFE_RELEASE(m_pFile);
	SAFE_DELETE_ARRAY(m_paFrameIndex);
	SAFE_DELETE_ARRAY(m_pPal);
	if (!m_ppaIBitmap)
		return;
	for(int i=0; i<m_AniInfoHdr.total_frame; i++)
	{
		SAFE_RELEASE(m_ppaIBitmap[i]);
	}
	SAFE_DELETE_ARRAY(m_ppaIBitmap);
}
//------------------------------------------------------------------------
// 读入动画包头
BOOL CAni::Open(IFileSystem* pFileSystem, LPCSTR szName, DWORD dwResID)
{
	ASSERT(pFileSystem && szName && "file system is NULL or file name is NULL");

	m_dwResID = dwResID;
	// load ani
	SAFE_RELEASE(m_pFile);
	g_AniFileLock.Lock();
	m_pFile = pFileSystem->GetFile(szName);
	if (!m_pFile) 
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAni::Open(): get file [%s] failed!", szName);
		_TraceOnce(flag1, false, buf);
		g_AniFileLock.Unlock();
		return FALSE;
	}

	if (!m_pFile->Open())
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAni::Open(): open file [%s] failed!", m_pFile->GetFilePath());
		_TraceOnce(flag2, false, buf);
		SAFE_RELEASE(m_pFile);
		g_AniFileLock.Unlock();
		return FALSE;
	}

	// read file header
	stPackFileHeader	pfh;
	if (0 == m_pFile->Read(&pfh, sizeof(stPackFileHeader)))
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAni::Open(%d): read file [%s] failed!", __LINE__, m_pFile->GetFilePath());
		_TraceOnce(flag3, false, buf);
		SAFE_RELEASE(m_pFile);
		g_AniFileLock.Unlock();
		return FALSE;
	}

	// check validity
	if (!pfh.IsValid())
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAni::Open(): invalid pack file header!");
		_TraceOnce(flag4, false, buf);
		SAFE_RELEASE(m_pFile);
		g_AniFileLock.Unlock();
		return FALSE;
	}

	// read ani info header
	if (0 == m_pFile->Read(&m_AniInfoHdr, sizeof(stAniInfoHeader)))
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAni::Open(%d): read file [%s] failed!", __LINE__, m_pFile->GetFilePath());
		_TraceOnce(flag5, false, buf);
		SAFE_RELEASE(m_pFile);
		g_AniFileLock.Unlock();
		return FALSE;
	}

	if (pfh.pack_type == ptMultiFramePublicPalette)
	{
		int nPalSize = m_AniInfoHdr.use_color<<1;

		// 下面3行是对先前打包机留下的隐患做的补救处理
		m_pPal = new BYTE[512];
		WPixel* pPixel = (WPixel*)m_pPal;
		pPixel[255] = WPixel(255,0,255);

		if (0 == m_pFile->Read(m_pPal, nPalSize))
		{
			char buf[MAX_PATH];
			wsprintf(buf, "ERROR: CAni::Open(%d): read file [%s] failed!", __LINE__, m_pFile->GetFilePath());
			_TraceOnce(flag6, false, buf);			
			SAFE_DELETE_ARRAY(m_pPal);
			SAFE_RELEASE(m_pFile);
			g_AniFileLock.Unlock();
			return FALSE;
		}
	}

	// read frame index array
	m_paFrameIndex	= new stFrameIndex[m_AniInfoHdr.total_frame];
	if (0 == m_pFile->Read(m_paFrameIndex, sizeof(stFrameIndex)*m_AniInfoHdr.total_frame))
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAni::Open(%d): read file [%s] failed!", __LINE__, m_pFile->GetFilePath());
		_TraceOnce(flag7, false, buf);			
		SAFE_DELETE_ARRAY(m_pPal);
		SAFE_RELEASE(m_pFile);
		g_AniFileLock.Unlock();
		return FALSE;
	}
	g_AniFileLock.Unlock();

	m_ptLowOffset.x = 9999;
	m_ptLowOffset.y = 9999;
	for(int i = 0; i < (int)m_AniInfoHdr.total_frame; i++)
	{
		if(m_ptLowOffset.x > m_paFrameIndex[i].clip_offx)
			m_ptLowOffset.x = m_paFrameIndex[i].clip_offx;

		if(m_ptLowOffset.y > m_paFrameIndex[i].clip_offy)
			m_ptLowOffset.y = m_paFrameIndex[i].clip_offy;
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 显示帧
void CAni::ParticleDraw(ICanvas *pCanvas, int nX, int nY, int nFrame, int nFlag, BYTE* pPal, BYTE nAlpha)
{
	if (nFrame < 0)
		return;

	if (!GetBitmap(0))
		return;

	int nFF = nFrame % (int)m_AniInfoHdr.total_frame;	
	if (m_ppaIBitmap[nFF])
	{
		if (nFlag & BLIT_TRANS)
		{
			if(nAlpha >= 0 && nAlpha < 255)
				((IRleBitmap*)m_ppaIBitmap[nFF])->SetUserData(nAlpha);
			else
				nFlag = BLIT_NORMAL;
		}

		if (!pPal) 
			pPal = m_pPal;
		pCanvas->DrawRle(nX, nY, (IRleBitmap*)m_ppaIBitmap[nFF], pPal, nFlag);
	}
}
//------------------------------------------------------------------------
// 显示帧
void CAni::Draw(ICanvas *pCanvas, int nX, int nY, int nFrame, int nFlag, BYTE* pPal, CParticle * pParticle)
{
	if (nFrame<0 || nFrame>=m_AniInfoHdr.total_frame)
		return;

	if (!GetBitmap(0))
		return;

	if (m_ppaIBitmap[nFrame])
	{
		if (nFlag & BLIT_TRANS)
		{
			((IRleBitmap*)m_ppaIBitmap[nFrame])->SetUserData(TRANS_VALUE);
		}

		if (!pPal) 
			pPal = m_pPal;

		try
		{
			pCanvas->DrawRle(nX, nY, (IRleBitmap*)m_ppaIBitmap[nFrame], pPal, nFlag);
		}
		catch (...)
		{
		}
	}
}
//------------------------------------------------------------------------
// 显示帧
void CAni::Draw(ICanvas *pCanvas, int nX, int nY, int nFrame, int nFlag, BYTE nAlpha, BYTE* pPal, CParticle * pParticle)
{
	if (nFrame<0 || nFrame>=m_AniInfoHdr.total_frame)
		return;

	if (!GetBitmap(0))
		return;

	if (m_ppaIBitmap[nFrame])
	{
		if (nAlpha == 0)
		{
			if (nFlag & BLIT_TRANS)
				((IRleBitmap*)m_ppaIBitmap[nFrame])->SetUserData(TRANS_VALUE);
		}
		else
		{
			((IRleBitmap*)m_ppaIBitmap[nFrame])->SetUserData(nAlpha);
			nFlag = BLIT_MASK | BLIT_TRANS;
		}

		if (!pPal) 
			pPal = m_pPal;

		try
		{
			pCanvas->DrawRle(nX, nY, (IRleBitmap*)m_ppaIBitmap[nFrame], pPal, nFlag);
		}

		catch (...) 
		{
		}
	}
}
//------------------------------------------------------------------------
// 得到第几帧的IBitmap
IBitmap* CAni::GetBitmap(int nFrame)
{
	if (nFrame <0 || nFrame>=m_AniInfoHdr.total_frame)
		return NULL;

	switch(m_nLoadState)
	{
	case enLOAD_NONE:
		m_nLoadState = enLOAD_ING;
		stAniNode node;
		node.dwResID = m_dwResID;
		node.pAni = this;
		g_pAniLoader->AddNodeLoad(node);
		return NULL;

	case enLOAD_ING:
		return NULL;

	case enLOAD_OVER:
		return m_ppaIBitmap[nFrame];

	default:
		break;
	}	
	return NULL;
}
//------------------------------------------------------------------------
// 得到所有的数据
BOOL CAni::GetData(BYTE* pBuffer, DWORD dwBufferSize)
{
	if (dwBufferSize < m_AniInfoHdr.total_size || !m_pFile)
		return FALSE;

	DWORD dwDataOffset = sizeof(stPackFileHeader) + m_AniInfoHdr.struct_size + 
		sizeof(stFrameIndex)*m_AniInfoHdr.total_frame;

	g_AniFileLock.Lock();
	m_pFile->Seek(dwDataOffset, SEEK_SET);
	if (0 == m_pFile->Read(pBuffer, m_AniInfoHdr.total_size))
	{
		PrintEventLn("CAni::LoadRes: read file [%s] failed!", m_pFile->GetFilePath());
		g_AniFileLock.Unlock();
		return FALSE;
	}
	g_AniFileLock.Unlock();

	SAFE_RELEASE(m_pFile);
	return TRUE;
}
//------------------------------------------------------------------------
int CAni::GetWidth(int nFrame)
{
	if (nFrame<0 || nFrame>=m_AniInfoHdr.total_frame)
		return 0;
	return m_paFrameIndex[nFrame].width;
}
//------------------------------------------------------------------------
int CAni::GetHeight(int nFrame)
{
	if (nFrame<0 || nFrame>=m_AniInfoHdr.total_frame)
		return 0;
	return m_paFrameIndex[nFrame].height;
}
//------------------------------------------------------------------------
void CAni::GetOffset(int nFrame, int& nOffsetX,int& nOffsetY)
{
	if (nFrame<0 || nFrame>=m_AniInfoHdr.total_frame)
		return;
	nOffsetX = m_paFrameIndex[nFrame].clip_offx;
	nOffsetY = m_paFrameIndex[nFrame].clip_offy;
}
//------------------------------------------------------------------------
void CAni::GetLowOffset(int nFrame, int& nOffsetX,int& nOffsetY)
{
	if (nFrame < 0)
		return;
	int nFF = nFrame % (int)m_AniInfoHdr.total_frame;

	nOffsetX = m_paFrameIndex[nFF].clip_offx - m_ptLowOffset.x;
	nOffsetY = m_paFrameIndex[nFF].clip_offy - m_ptLowOffset.y;
}
//------------------------------------------------------------------------