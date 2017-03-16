/**
*	created:		2012-7-2   1:46
*	filename: 		Pic
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/Pic.h"
#include "../Include/ResLoader.h"
//------------------------------------------------------------------------
CPic::~CPic()
{
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pFile);
	SAFE_DELETE_ARRAY(m_pPal);
}
//------------------------------------------------------------------------
BOOL CPic::Open(IFileSystem* pFileSystem, LPCSTR szName, DWORD dwResID)
{
	ASSERT(pFileSystem && szName && "file system is NULL or file name is NULL");

	// load bitmap
	SAFE_RELEASE(m_pFile);
	m_pFile = pFileSystem->GetFile(szName);
	if (!m_pFile)
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CPic::Open(): get file [%s] failed!", szName);
		_TraceOnce(flag1, false, buf);
		return FALSE;
	}

	if (!m_pFile->Open())
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CPic::Open(): open file [%s] failed!", m_pFile->GetFilePath());
		_TraceOnce(flag2, false, buf);
		return FALSE;
	}

	// read file header
	if (0 == m_pFile->Read(&m_rlehdr, sizeof(RLEHEADER)))
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CPic::Open(): read file [%s] failed!", m_pFile->GetFilePath());
		_TraceOnce(flag3, false, buf);
		SAFE_RELEASE(m_pFile);
		return FALSE;
	}

	// check validity
	if (!m_rlehdr.IsValid())
	{
		char buf[MAX_PATH];
		wsprintf(buf, "ERROR: CPic::Open(): invalid sfp file header!");
		_TraceOnce(flag4, false, buf);
		SAFE_RELEASE(m_pFile);
		return FALSE;
	}

	if (m_rlehdr.nUseColor == 16)
	{
		TraceLn("CPic::Open(): not support 16bit tga");
		SAFE_RELEASE(m_pFile);
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
void CPic::Draw(ICanvas* pCanvas, int x, int y, int nFlag, BYTE* pPal)
{
	ASSERT(pCanvas);

	if (!m_pBitmap)
	{
		if (!GetBitmap())
			return;
	}

	if (nFlag & BLIT_TRANS)
		m_pBitmap->SetUserData(TRANS_VALUE);

	if (!pPal) 
		pPal = m_pPal;
	try{
		pCanvas->DrawRle(x, y, static_cast<IRleBitmap*>(m_pBitmap), pPal, nFlag);
	}catch (...) {
	}
}
//------------------------------------------------------------------------
IBitmap* CPic::GetBitmap()
{
	if (!m_pBitmap && m_pFile)
	{
		m_pFile->SeekToBegin();

		BYTE* pBuf = new BYTE[m_rlehdr.dwTotalSize];
		if (0 == m_pFile->Read(pBuf, m_rlehdr.dwTotalSize))
		{
			TraceLn("CPic::GetBitmap(): read file failed!");
			return NULL;
		}

		// 修改RLEHEADER结构，使Canvas在加载时不处理调色板
		RLEHEADER* pRleHdr = (RLEHEADER*)pBuf;
		pRleHdr->nUseColor = 0;

		// 这里加载调色板
		//m_pPal = (BYTE*) new WPixel[m_rlehdr.nUseColor];

		// 下面3行是对先前打包机留下的隐患做的补救处理
		m_pPal = new BYTE[512];
		WPixel* pPixel = (WPixel*)m_pPal;
		pPixel[255] = WPixel(255,0,255);

		memcpy(m_pPal, pBuf+sizeof(RLEHEADER), m_rlehdr.nUseColor<<1);

		IRleBitmap* pRleBitmap;
		if (!g_CanvasHelper.CreateIRleBitmap(&pRleBitmap, g_DefaultTrace.GetTraceObject()))
			return NULL;

		if (!pRleBitmap->Load(pBuf))
			return NULL;

		g_ResLoader.AddSize(pRleBitmap->GetSize());
		m_dwSize += pRleBitmap->GetSize();

		pRleBitmap->SetPalette(m_pPal, 0);// 第二个参数无意义，所以没填
		SAFE_DELETE_ARRAY(pBuf);

		m_pBitmap = static_cast<IBitmap*>(pRleBitmap); 	
	}

	return m_pBitmap;
}
//------------------------------------------------------------------------