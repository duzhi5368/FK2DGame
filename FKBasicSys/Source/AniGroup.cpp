/**
*	created:		2012-7-2   1:15
*	filename: 		AniGroup
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/Particle.h"
#include "../Include/AniGroup.h"
//------------------------------------------------------------------------
extern CAniGroupLoader* g_pAniGroupLoader;
//------------------------------------------------------------------------
CAniGroup::CAniGroup()
{
	m_paAniIndex		= NULL;
	m_paFrameIndex		= NULL;
	m_ppaIBitmap		= NULL;
	m_ppaState			= NULL;
	m_pFile				= NULL;
	m_pPal				= NULL;
	m_nDataOffset		= 0;
	m_nMaxUserPalCount	= 0;
	m_dwResID			= 0;
	memset(m_pUserPal, 0, sizeof(m_pUserPal));
	m_dwSize			= 0;
}
//------------------------------------------------------------------------
CAniGroup::~CAniGroup()
{
	for(int i = 0; i < CrtStateMax; i++)
	{
		if(m_LoadState[i] == enLOAD_ING)
		{
			g_pAniGroupLoader->RemoveNode(m_dwResID);
			break;
		}
	}

	SAFE_RELEASE(m_pFile);
	SAFE_DELETE_ARRAY(m_paFrameIndex);
	SAFE_DELETE_ARRAY(m_paAniIndex);
	SAFE_DELETE_ARRAY(m_ppaState);
	SAFE_DELETE_ARRAY(m_pPal);
	memset(m_pUserPal, 0, sizeof(m_pUserPal));
	if (!m_ppaIBitmap) return;

	for(int j = 0; j < m_GroupInfoHdr.total_frame; j++)
	{
		SAFE_RELEASE(m_ppaIBitmap[j]);
	}
	SAFE_DELETE_ARRAY(m_ppaIBitmap);

	for(mask_iterator it = m_setMask.begin(); it!=m_setMask.end(); ++it)
	{
		delete (*it).second;
		(*it).second = NULL;
	}
	m_setMask.clear();
}
//------------------------------------------------------------------------
// 打开文件
bool CAniGroup::Open(IFileSystem* pFileSystem, LPCSTR szName, DWORD dwResID)
{
	ASSERT(pFileSystem && szName && "file system is NULL or file name is NULL");

	m_dwResID = dwResID;
	// load ani
	SAFE_RELEASE(m_pFile);
	g_AniGroupFileLock.Lock();
	m_pFile = pFileSystem->GetFile(szName);
	if (!m_pFile)
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAniGroup::Open(): get file [%s] failed!", szName);
		_TraceOnce(flag1, false, buf);
		g_AniGroupFileLock.Unlock();
		return FALSE;
	}

	if (!m_pFile->Open())
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAniGroup::Open(): open file [%s] failed!", m_pFile->GetFilePath());
		_TraceOnce(flag2, false, buf);
		g_AniGroupFileLock.Unlock();
		return FALSE;
	}

	// read file header
	stPackFileHeader pfh;
	if (0 == m_pFile->Read(&pfh, sizeof(stPackFileHeader)))
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAniGroup::Open(%d): read file [%s] failed!", __LINE__, m_pFile->GetFilePath());
		_TraceOnce(flag3, false, buf);
		SAFE_RELEASE(m_pFile);
		g_AniGroupFileLock.Unlock();
		return FALSE;
	}

	// check validity
	if (!pfh.IsValid())
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAniGroup::Open(): invalid pack file header!");
		_TraceOnce(flag4, false, buf);
		SAFE_RELEASE(m_pFile);
		g_AniGroupFileLock.Unlock();
		return FALSE;
	}
	m_nDataOffset = pfh.data_offset;

	// read ani group info
	long lIndexOffset = 0;
	DWORD dwIndexSize = pfh.data_offset - sizeof(stPackFileHeader);
	BYTE* pIndex = new BYTE[dwIndexSize];
	if (0 == m_pFile->Read(pIndex, dwIndexSize))
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CAniGroup::Open(%d): read file [%s] failed!", __LINE__, m_pFile->GetFilePath());
		_TraceOnce(flag5, false, buf);
		SAFE_DELETE_ARRAY(pIndex);
		SAFE_RELEASE(m_pFile);
		g_AniGroupFileLock.Unlock();
		return FALSE;
	}
	g_AniGroupFileLock.Unlock();

	memcpy(&m_GroupInfoHdr, pIndex + lIndexOffset, sizeof(stGroupInfoHeader));	
	lIndexOffset += sizeof(stGroupInfoHeader);

	// read palette
	if (pfh.pack_type == ptMultiStatePublicPalette)
	{
		int nPalSize = m_GroupInfoHdr.use_color<<1;

		// 下面3行是对先前打包机留下的隐患做的补救处理
		m_pPal = new BYTE[512];
		WPixel* pPixel = (WPixel*)m_pPal;
		pPixel[255] = WPixel(255,0,255);

		memcpy(m_pPal, pIndex + lIndexOffset, nPalSize);
		lIndexOffset += nPalSize;
	}

	for(mask_iterator it = m_setMask.begin(); it!=m_setMask.end(); ++it)
	{
		delete (*it).second;
		(*it).second = NULL;
	}
	m_setMask.clear();

	m_ppaIBitmap = new IBitmap*[m_GroupInfoHdr.total_frame];
	memset(m_ppaIBitmap, 0, m_GroupInfoHdr.total_frame*sizeof(m_ppaIBitmap));
	m_ppaState = new IBitmap*[m_GroupInfoHdr.total_ani>>3];
	memset(m_ppaState, 0, (m_GroupInfoHdr.total_ani>>3)*sizeof(m_ppaState));

	// copy ani group index info
	m_paAniIndex = new stAniIndex[m_GroupInfoHdr.total_ani];
	memcpy(m_paAniIndex, pIndex + lIndexOffset, 
		sizeof(stAniIndex)*m_GroupInfoHdr.total_ani);
	lIndexOffset += sizeof(stAniIndex)*m_GroupInfoHdr.total_ani;

	// copy frame index info
	m_paFrameIndex = new stFrameIndex[m_GroupInfoHdr.total_frame];
	memcpy(m_paFrameIndex, pIndex + lIndexOffset,
		sizeof(stFrameIndex)*m_GroupInfoHdr.total_frame);
	SAFE_DELETE_ARRAY(pIndex);

	memset(m_LoadState, 0, sizeof(m_LoadState));
	return TRUE;
}
//------------------------------------------------------------------------
// 显示第几个动画的第几帧
void CAniGroup::Draw(ICanvas *pCanvas, int nX, int nY, int nAni, int nFrame, int nFlag, BYTE nAlpha, BYTE* pPal, CParticle * pParticle)
{
	if ( nAni<0 || nAni>=m_GroupInfoHdr.total_ani || 
		nFrame <0 || nFrame >=m_paAniIndex[nAni].ani_frames)
		return;

	// 如果未在内存中，载入
	if (!GetStateBitmap(nAni>>3))
		return;

	long lIndex = m_paAniIndex[nAni].key_frame + nFrame;
	IRleBitmap* pIRleBitmap = (IRleBitmap*)m_ppaIBitmap[lIndex];
	if (pIRleBitmap)
	{
		if(pParticle)
		{
			nAlpha = pParticle->m_nPartAlpha;
			if(nAlpha <= 0)
				nAlpha = 1;
			else if(nAlpha > 255)
				nAlpha = 255;
		}

		int nPtcAlpha = 255;
		if (nAlpha == 0)
		{
			if (nFlag & BLIT_TRANS)
			{
				pIRleBitmap->SetUserData(TRANS_VALUE);
				nPtcAlpha = TRANS_VALUE;
			}
		}
		else
		{
			pIRleBitmap->SetUserData(nAlpha);
			nFlag = BLIT_MASK | BLIT_TRANS;
		}

		if (!pPal)
			pPal = m_pPal;
		try
		{
			if(pParticle)
			{
				CMultiParticleMask * pMask = GetParticleMask(pParticle,lIndex,pPal);
				if(pMask)
				{
					CParticleMask * pParticleMask = pMask->GetMask(lIndex);
					if(pParticleMask)
					{
						POINT ptOffset = {nX,nY};
						pParticle->GenNext();
						pParticle->AddNewParticle(pParticleMask,ptOffset);
						pParticle->Draw(pCanvas,nX,nY,nPtcAlpha);
					}
				}
			}

			pCanvas->DrawRle(nX, nY, pIRleBitmap, pPal, nFlag);
		}
		catch (...)
		{
		}
	}
}
//------------------------------------------------------------------------
// 得到IBitmap
IBitmap* CAniGroup::GetBitmap(int nAni, int nFrame)
{
	if (nAni<0 || nAni>=m_GroupInfoHdr.total_ani ||
		nFrame<0 || nFrame>=m_paAniIndex[nAni].ani_frames) 
		return NULL;

	// 如果未在内存中，载入
	if (!GetStateBitmap(nAni>>3))
		return NULL;

	long lIndex = m_paAniIndex[nAni].key_frame + nFrame;
	return m_ppaIBitmap[lIndex];
}
//------------------------------------------------------------------------
IBitmap* CAniGroup::GetStateBitmap(int nState)
{
	switch(m_LoadState[nState])
	{
	case enLOAD_NONE:
		m_LoadState[nState] = enLOAD_ING;
		stAniGroupNode node;
		node.dwResID = m_dwResID;
		node.pAniGroup = this;
		node.nState = nState;
		g_pAniGroupLoader->AddNodeLoad(node);
		return NULL;

	case enLOAD_ING:
		return NULL;

	case enLOAD_OVER:
		return m_ppaState[nState];

	default:
		break;
	}

	return NULL;
}
//------------------------------------------------------------------------
BOOL CAniGroup::GetStateData(int nState, BYTE* pBuffer, DWORD dwBufferSize)
{
	if (!m_pFile) 
		return FALSE;
	DWORD dwDataOffset = m_nDataOffset + m_paAniIndex[nState<<3].ani_offset;

	g_AniGroupFileLock.Lock();
	m_pFile->Seek(dwDataOffset, SEEK_SET);
	if (0 == m_pFile->Read(pBuffer, dwBufferSize))
	{
		PrintEventLn("CAniGroup::LoadRes: read ani group file [%s] failed!  size:%d  bufe:%d", m_pFile->GetFilePath(), dwBufferSize, pBuffer != NULL);

		g_AniGroupFileLock.Unlock();
		return FALSE;
	}
	g_AniGroupFileLock.Unlock();

	return TRUE;
}
//------------------------------------------------------------------------
DWORD CAniGroup::GetStateSize(int nState)
{
	DWORD dwSize = 0;
	int nAni = nState << 3;
	for (int j=0; j<8; j++ ) // 8个ani
	{
		long nIdx = m_paAniIndex[nAni+j].key_frame + m_paAniIndex[nAni+j].ani_frames;
		dwSize += m_paFrameIndex[nIdx-1].data_offset + m_paFrameIndex[nIdx-1].data_size;
	}

	return dwSize;
}
//------------------------------------------------------------------------
int	CAniGroup::GetTotalFrames(int nAni)
{
	if (nAni<0 || nAni>=m_GroupInfoHdr.total_ani) 
		return 0;
	return m_paAniIndex[nAni].ani_frames;
}
//------------------------------------------------------------------------
void CAniGroup::GetOffset(int nAni, int nFrame, int& nOffsetX, int& nOffsetY)
{
	if (nAni<0 || nAni>=m_GroupInfoHdr.total_ani)
	{
		return;
	}
	if (nFrame<0 || nFrame>=m_paAniIndex[nAni].ani_frames)
	{
		return;
	}
	nOffsetX = m_paFrameIndex[m_paAniIndex[nAni].key_frame+nFrame].clip_offx;
	nOffsetY = m_paFrameIndex[m_paAniIndex[nAni].key_frame+nFrame].clip_offy;
}
//------------------------------------------------------------------------
int CAniGroup::GetWidth(int nAni, int nFrame)
{
	if (nAni<0 || nAni>=m_GroupInfoHdr.total_ani ||
		nFrame<0 || nFrame>=m_paAniIndex[nAni].ani_frames) 
		return 0;
	return m_paFrameIndex[m_paAniIndex[nAni].key_frame+nFrame].width;
}
//------------------------------------------------------------------------
int CAniGroup::GetHeight(int nAni, int nFrame)
{
	if (nAni<0 || nAni>=m_GroupInfoHdr.total_ani ||
		nFrame<0 || nFrame>=m_paAniIndex[nAni].ani_frames) 
		return 0;
	return m_paFrameIndex[m_paAniIndex[nAni].key_frame+nFrame].height;
}
//------------------------------------------------------------------------
CMultiParticleMask * CAniGroup::GetParticleMask(CParticle * pParticle,int cFrame,BYTE * pPal)
{
	if(!pParticle)
	{
		return NULL;
	}
	POINTS ptSimpling;
	pParticle->GetSimpling(ptSimpling);
	int nIndex = ((ptSimpling.x<<16)+ptSimpling.y);
	mask_iterator it = m_setMask.find(nIndex);
	if(it != m_setMask.end())
	{
		if(!(*it).second->GetMask(cFrame)->Size())
		{
			//该帧还没有初始化
			if(!(*it).second->GetMask(cFrame)->Create(ptSimpling.x,ptSimpling.y,
				(IRleBitmap*)(m_ppaIBitmap[cFrame]),pPal))
				return NULL;
		}
		return (*it).second;
	}

	std::auto_ptr<CMultiParticleMask> pMask(new CMultiParticleMask());
	if(!pMask->Create(pParticle,(IRleBitmap**)m_ppaIBitmap,m_GroupInfoHdr.total_frame,pPal))
	{
		return NULL;
	}

	m_setMask.insert(std::make_pair<const int,CMultiParticleMask*>(nIndex,pMask.get()));

	return pMask.release();
}
//------------------------------------------------------------------------