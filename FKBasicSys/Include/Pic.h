/**
*	created:		2012-7-2   1:45
*	filename: 		Pic
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "Ani.h"
//------------------------------------------------------------------------
class CPic : public IPoolNodeObject // 36 bytes
{
public:
	IBitmap*			m_pBitmap;
	IFileObject*		m_pFile;					// 文件对象
	BYTE*				m_pPal;
	RLEHEADER			m_rlehdr;
	DWORD				m_dwSize;
	
	CPic(): m_pBitmap(NULL), m_pFile(NULL),m_pPal(NULL),m_dwSize(0){}
	virtual ~CPic();
	IBitmap* GetBitmap();
	virtual DWORD GetSize(){return m_dwSize;}
	virtual void Release(){delete this;}

	BOOL Open(IFileSystem* pFileSystem, LPCSTR szName, DWORD dwResID);
	void Draw(ICanvas* pCanvas, int x, int y, int nFlag, BYTE* pPal=NULL);
	int GetWidth() {return m_rlehdr.dwWidth;}
	int GetHeight() {return m_rlehdr.dwHeight;}

	// 调色板相关
	BYTE* GetPal()
	{
		return m_pPal;
	}
	int GetUseColor() {return m_rlehdr.nUseColor;}
};
//------------------------------------------------------------------------