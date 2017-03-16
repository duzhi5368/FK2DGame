/**
*	created:		2012-6-23   21:37
*	filename: 		ImageLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		image图片加载器，负责管理和缓存，可支持bmp和tga,jpg
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
#include "../../../Depend/ijl/ijl.h"
#include "../Include/FKWndSysCommon.h"
#include "../../FKFileSystem/IFKFileSystem.h"
//------------------------------------------------------------------------
#pragma comment (lib, "../../Depend/ijl/ijl15.lib")
//------------------------------------------------------------------------
CImageLoader* CImageLoader::_instance = NULL;
//------------------------------------------------------------------------
CImageLoader::CImageLoader()
{
	m_vImageList.clear();
	m_pFS = NULL;
	CImageLoader::_instance = this;			// 如果直接定义这个类的全局变量，则不需要new出来
}
//------------------------------------------------------------------------
CImageLoader::~CImageLoader()
{
	Clear();
}
//------------------------------------------------------------------------
// 获得图片加载器实例(始终保持单实例)
CImageLoader* CImageLoader::GetInstance()
{
#ifdef _DEBUG
	if (CImageLoader::_instance != NULL)
	{
		static int flag = 0;
		if (flag == 0)
		{
			flag = 1; 
			if (!CImageLoader::_instance->GetFileSystem())
				OutputDebugString("Warning:CImageLoader::m_pFS == NULL!!!\r\n");
		}
	}
#endif
	if (CImageLoader::_instance == NULL)
		CImageLoader::_instance = new CImageLoader;
	return CImageLoader::_instance;
}
//------------------------------------------------------------------------
// 释放图片加载器实例
void CImageLoader::ReleaseInstance()
{
	if (CImageLoader::_instance)
	{
		CImageLoader::_instance->Clear();
		CImageLoader* p = CImageLoader::_instance;
		CImageLoader::_instance = NULL;
		delete p;
	}
}
//------------------------------------------------------------------------
// 清除资源
void CImageLoader::Clear()
{
	CacheImageListPtr _end = m_vImageList.end();
	PrintEventLn("CImageLoader::Clear, %d images is not release", m_vImageList.size());
	for (CacheImageListPtr it=m_vImageList.begin(); it!=_end; ++it)
	{
		(*it).second.pImage->Release();
	}
	m_vImageList.clear();
}
//------------------------------------------------------------------------
// 加载某张图片，如果缓冲中存在此图片，则直接返回缓冲中的图片，同时将该图片引用计数加1
// szFileName 可传入绝对路径，和相对路径(不支持"./"、"../"、".\"、"..\"之类的相对路径，
// 仅仅支持"face.bmp"，"face\face01.bmp"之类的相对路径)
IBitmap* CImageLoader::LoadImage(const char* szFileName)
{
	if (szFileName == NULL || szFileName[0] == '\0')
		return NULL;

	CacheImageListPtr it = m_vImageList.find(szFileName);
	if (it != m_vImageList.end())
	{
		(*it).second.nRefCount ++;
		return (*it).second.pImage;
	}

	// 缓存中没有该图片，加载它
	stCacheImage cacheimage;
	cacheimage.nRefCount = 0;

	if ((cacheimage.pImage = _loadImage(szFileName)) == NULL)
		return NULL;

	cacheimage.nRefCount = 1;
	m_vImageList[szFileName] = cacheimage;
#ifdef _DEBUG
	if (m_vImageList.size() >= 500)
	{
		OutputDebugString("WARNING: Image count >= 500 !!!!!\r\n");
	}
#endif
	return cacheimage.pImage;
}
//------------------------------------------------------------------------
// 复制一张图片，如果缓冲中存在此图片，则直接将该图片引用计数加1
IBitmap* CImageLoader::CopyImage(IBitmap* pImage)
{
	if (pImage == NULL)
		return NULL;
	CacheImageListPtr _end = m_vImageList.end();
	for (CacheImageListPtr it=m_vImageList.begin(); it!=_end; ++it)
	{
		stCacheImage CacheImage = (*it).second;
		if (CacheImage.pImage == pImage)
		{
			(*it).second.nRefCount++;
			return pImage;
		}
	}
	return NULL;
}
//------------------------------------------------------------------------
// 释放某张图片，如果缓冲中存在此图片，则直接将该图片引用计数减1
// 如果计数引用为0，则删除缓冲中的该图片
void CImageLoader::FreeImage(const char* szFileName)
{
	if (szFileName == NULL || szFileName[0] == '\0')
		return;
	
	CacheImageListPtr it = m_vImageList.find(szFileName);
	if (it != m_vImageList.end())
	{
		if (--((*it).second.nRefCount) == 0)
		{
			(*it).second.pImage->Release();
			m_vImageList.erase(it);
#ifdef _DEBUG
			if (m_vImageList.size() <= 0)
			{
				OutputDebugString("WARNING: Image count <= 0 !!!!!\r\n");
			}
#endif
		}
	}
}
//------------------------------------------------------------------------
// 释放某张图片，如果缓冲中存在此图片，则直接将该图片引用计数减1
// 如果计数引用为0，则删除缓冲中的该图片
void CImageLoader::FreeImage(IBitmap* pImage)
{
	if (pImage == NULL)
		return;
	CacheImageListPtr _end = m_vImageList.end();
	for (CacheImageListPtr it=m_vImageList.begin(); it!=_end; ++it)
	{
		stCacheImage CacheImage = (*it).second;
		if (CacheImage.pImage == pImage)
		{
			if (--CacheImage.nRefCount == 0)
			{
				CacheImage.pImage->Release();
				m_vImageList.erase(it);
#ifdef _DEBUG
				if (m_vImageList.size() <= 0)
				{
					OutputDebugString("WARNING: Image count <= 0 !!!!!\r\n");
				}
#endif
			}
			break;
		}
	}
}
//------------------------------------------------------------------------
BOOL CImageLoader::SetFileSystem(IFileSystem* pFS)
{
	if (pFS == NULL)
		return FALSE;
	m_pFS = pFS;
	return TRUE;
}
//------------------------------------------------------------------------
// 加载一张真正的图片
IBitmap* CImageLoader::_loadImage(const char* szFileName)
{
	if (m_pFS == NULL)
		return NULL;
	// 分析图片类型
	int nImageType = -1;
	char szExt[8];
	int nLen = strlen(szFileName);
	strcpy(szExt, &szFileName[nLen-3]);
	if (strcmpi(szExt, "bmp") == 0)
		nImageType = 0;
	else if (strcmpi(szExt, "tga") == 0)
		nImageType = 1;
	else if (strcmpi(szExt, "jpg") == 0)
		nImageType = 2;

	if (nImageType == -1)
		return NULL;

	// 读文件
	IBitmap* pImage = NULL;
	IFileObject* pFileObj;
	if ((pFileObj = m_pFS->GetFile(szFileName)) == NULL)
	{
		PrintEventLn("Error: GetFile() : can not get file[%s]", szFileName);
		return NULL;
	}
	if (!pFileObj->Open())
	{
		PrintEventLn("Error: Open() : can not open file[%s]", szFileName);
		return NULL;
	}
	DWORD dwSize = pFileObj->GetLength();
	BYTE* pbuf = new BYTE[dwSize];
	if (pFileObj->Read(pbuf, dwSize) != (UINT)dwSize)
	{
		pFileObj->Release();
		delete[] pbuf;
		PrintEventLn("Error: Read() : read file error[%s]", szFileName);
		return NULL;
	}
	pFileObj->Release();

	// 构造图片对象
	if (nImageType == 0)
	{
		if (!m_CanvasHelper.CreateIBitmap(&pImage))
		{
			delete[] pbuf;
			return NULL;
		}
	}
	else if (nImageType == 1)
	{
		IAlphaBitmap* pAlphaImage;
		if (!m_CanvasHelper.CreateIAlphaBitmap(&pAlphaImage))
		{
			delete[] pbuf;
			return NULL;
		}
		pImage = static_cast<IBitmap*>(pAlphaImage);
	}
	else if (nImageType == 2)   
	{
		//先建立普通位图
		if (!m_CanvasHelper.CreateIBitmap(&pImage))
		{
			delete[] pbuf;
			return NULL;
		}
		//将JPG数据解码
		BYTE* pTempbuf = __DecodeJpeg(pbuf, dwSize);
		if (pTempbuf == NULL)   //解码失败
		{  
			delete[] pbuf;
			return NULL;
		}
		delete[] pbuf;      //解码成功后,换掉原来的缓冲区
		pbuf = pTempbuf;
	}

	// 加载
	if (!pImage->Load(pbuf))
	{
		pImage->Release();
		delete[] pbuf;
		return NULL;
	}

	delete[] pbuf;
	return pImage;
}
//------------------------------------------------------------------------
BYTE * CImageLoader::__DecodeJpeg(BYTE *pData, int nLen)
{
	if (pData == NULL)
	{
		return NULL;
	}

	BYTE *pDIBData = NULL;
	BOOL bIjlInited = FALSE;

	JPEG_CORE_PROPERTIES jcprop;
	IJLERR jerr;	
	try
	{
		jerr = ijlInit(&jcprop);
		if (jerr != IJL_OK)
		{
			return NULL;
		}
		bIjlInited = TRUE;   //指明已经初始化
				
		jcprop.JPGBytes = pData;
		jcprop.JPGSizeBytes = nLen;
		jerr = ijlRead(&jcprop, IJL_JBUFF_READPARAMS);
		if (jerr != IJL_OK)
		{		
			ijlFree(&jcprop);
			return NULL;
		}

		int nWidth = jcprop.JPGWidth;
		int nHeight = jcprop.JPGHeight;
		int nChannels = 3;  
		int nBytePerLine = nWidth * nChannels;
		int nBytePerLinePad = IJL_DIB_PAD_BYTES(nWidth, nChannels);
		int nDIBImageSize = (nBytePerLine + nBytePerLinePad) * nHeight;
		pDIBData = new BYTE[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nDIBImageSize];
		if (pDIBData == NULL)
		{
			ijlFree(&jcprop);			
			return NULL;
		}

		BITMAPFILEHEADER filehead;
		memset(&filehead, 0, sizeof(BITMAPFILEHEADER));
		filehead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nDIBImageSize;
		filehead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		filehead.bfType = 19778;   //即'BM'
		memcpy(pDIBData, &filehead, sizeof(BITMAPFILEHEADER));

		BITMAPINFOHEADER infohead;
		memset(&infohead, 0, sizeof(BITMAPINFOHEADER));
		infohead.biSize = sizeof(BITMAPINFOHEADER);
		infohead.biWidth = nWidth;
		infohead.biHeight = nHeight;
		infohead.biPlanes = 1;
		infohead.biBitCount = 24;
		infohead.biCompression = BI_RGB;
		memcpy(pDIBData + sizeof(BITMAPFILEHEADER), &infohead, sizeof(BITMAPINFOHEADER));

		jcprop.DIBWidth = nWidth;
		jcprop.DIBHeight = nHeight;
		jcprop.DIBChannels = nChannels;
		jcprop.DIBColor = IJL_BGR;
		jcprop.DIBPadBytes = nBytePerLinePad;
		jcprop.DIBBytes = pDIBData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		switch(jcprop.JPGChannels) {
		case 1:
			jcprop.JPGColor = IJL_G;
			break;
		case 3:
			jcprop.JPGColor = IJL_YCBCR;
			break;
		default:
			jcprop.JPGColor = IJL_OTHER;
			jcprop.DIBColor = IJL_OTHER;
		}

		jerr = ijlRead(&jcprop, IJL_JBUFF_READWHOLEIMAGE);
		if (jerr != IJL_OK)
		{			
			ijlFree(&jcprop);			
			delete[] pDIBData;			
			return NULL;
		}

		ijlFree(&jcprop);	

		int nNumLine = nBytePerLine + nBytePerLinePad;   //每行字节数
		BYTE *pTemp = new BYTE[nNumLine];
		if (pTemp)
		{
			BYTE *pDataStart = pDIBData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			int nHalfHeight = nHeight / 2;
			for (int i = 0; i < nHalfHeight; ++i)
			{
				BYTE *pSrc1 = pDataStart + i * nNumLine;
				BYTE *pSrc2 = pDataStart + (nHeight - 1 - i) * nNumLine;
				memcpy(pTemp, pSrc1, nNumLine);
				memcpy(pSrc1, pSrc2, nNumLine);
				memcpy(pSrc2, pTemp, nNumLine);
			}
			delete[] pTemp;
		}
		
		return pDIBData;
	}
	catch (...) {
		if (pDIBData)
		{
			delete[] pDIBData;
		}
		if (bIjlInited)
		{
			ijlFree(&jcprop);
		}
		return NULL;
	}
	
	return NULL;
}
//------------------------------------------------------------------------
CImageLoaderEx* CImageLoaderEx::_instance = NULL;
//------------------------------------------------------------------------
CImageLoaderEx::CImageLoaderEx()
{
	m_vImageList.clear();
	m_pFS = NULL;
	CImageLoaderEx::_instance = this;			// 如果直接定义这个类的全局变量，则不需要new出来
}
//------------------------------------------------------------------------
CImageLoaderEx::~CImageLoaderEx()
{
	Clear();
}
//------------------------------------------------------------------------
// 获得图片加载器实例(始终保持单实例)
CImageLoaderEx* CImageLoaderEx::GetInstance()
{
#ifdef _DEBUG
	if (CImageLoaderEx::_instance != NULL)
	{
		static int flag = 0;
		if (flag == 0)
		{
			flag = 1; 
			if (!CImageLoaderEx::_instance->GetFileSystem())
				OutputDebugString("Warning:CImageLoaderEx::m_pFS == NULL!!!\r\n");
		}
	}
#endif
	if (CImageLoaderEx::_instance == NULL)
		CImageLoaderEx::_instance = new CImageLoaderEx;
	return CImageLoaderEx::_instance;
}
//------------------------------------------------------------------------
// 释放图片加载器实例
void CImageLoaderEx::ReleaseInstance()
{
	if (CImageLoaderEx::_instance)
	{
		CImageLoaderEx::_instance->Clear();
		CImageLoaderEx* p = CImageLoaderEx::_instance;
		CImageLoaderEx::_instance = NULL;
		delete p;
	}
}
//------------------------------------------------------------------------
// 清除资源
void CImageLoaderEx::Clear()
{
	CacheImageListPtr _end = m_vImageList.end();
	PrintEventLn("CImageLoaderEx::Clear, %d images is not release", m_vImageList.size());
	for (CacheImageListPtr it=m_vImageList.begin(); it!=_end; ++it)
	{
		(*it).second.pImage->Release();
	}
	m_vImageList.clear();
}
//------------------------------------------------------------------------
// 加载某张图片，如果缓冲中存在此图片，则直接返回缓冲中的图片，同时将该图片引用计数加1
// szFileName 可传入绝对路径，和相对路径(不支持"./"、"../"、".\"、"..\"之类的相对路径，
// 仅仅支持"face.bmp"，"face\face01.bmp"之类的相对路径)
IBitmap* CImageLoaderEx::LoadImage(const char* szFileName)
{
	if (szFileName == NULL || szFileName[0] == '\0')
		return NULL;

	CacheImageListPtr it = m_vImageList.find(szFileName);
	if (it != m_vImageList.end())
	{
		(*it).second.nRefCount ++;
		return (*it).second.pImage;
	}

	// 缓存中没有该图片，加载它
	stCacheImage cacheimage;
	cacheimage.nRefCount = 0;

	if ((cacheimage.pImage = _loadImage(szFileName)) == NULL)
		return NULL;

	cacheimage.nRefCount = 1;
	m_vImageList[szFileName] = cacheimage;
#ifdef _DEBUG
	if (m_vImageList.size() >= 500)
	{
		OutputDebugString("WARNING: Image count >= 500 !!!!!\r\n");
	}
#endif
	return cacheimage.pImage;
}
//------------------------------------------------------------------------
// 复制一张图片，如果缓冲中存在此图片，则直接将该图片引用计数加1
IBitmap* CImageLoaderEx::CopyImage(IBitmap* pImage)
{
	if (pImage == NULL)
		return NULL;
	CacheImageListPtr _end = m_vImageList.end();
	for (CacheImageListPtr it=m_vImageList.begin(); it!=_end; ++it)
	{
		stCacheImage CacheImage = (*it).second;
		if (CacheImage.pImage == pImage)
		{
			(*it).second.nRefCount++;
			return pImage;
		}
	}
	return NULL;
}
//------------------------------------------------------------------------
// 释放某张图片，如果缓冲中存在此图片，则直接将该图片引用计数减1
// 如果计数引用为0，则删除缓冲中的该图片
void CImageLoaderEx::FreeImage(const char* szFileName)
{
	if (szFileName == NULL || szFileName[0] == '\0')
		return;
	
	CacheImageListPtr it = m_vImageList.find(szFileName);
	if (it != m_vImageList.end())
	{
		if (--((*it).second.nRefCount) == 0)
		{
			(*it).second.pImage->Release();
			m_vImageList.erase(it);
#ifdef _DEBUG
			if (m_vImageList.size() <= 0)
			{
				OutputDebugString("WARNING: Image count <= 0 !!!!!\r\n");
			}
#endif
		}
	}
}
//------------------------------------------------------------------------
// 释放某张图片，如果缓冲中存在此图片，则直接将该图片引用计数减1
// 如果计数引用为0，则删除缓冲中的该图片
void CImageLoaderEx::FreeImage(IBitmap* pImage)
{
	if (pImage == NULL)
		return;
	CacheImageListPtr _end = m_vImageList.end();
	for (CacheImageListPtr it=m_vImageList.begin(); it!=_end; ++it)
	{
		stCacheImage CacheImage = (*it).second;
		if (CacheImage.pImage == pImage)
		{
			if (--CacheImage.nRefCount == 0)
			{
				CacheImage.pImage->Release();
				m_vImageList.erase(it);
#ifdef _DEBUG
				if (m_vImageList.size() <= 0)
				{
					OutputDebugString("WARNING: Image count <= 0 !!!!!\r\n");
				}
#endif
			}
			break;
		}
	}
}
//------------------------------------------------------------------------
BOOL CImageLoaderEx::SetFileSystem(IFileSystem* pFS)
{
	if (pFS == NULL)
		return FALSE;
	m_pFS = pFS;
	return TRUE;
}
//------------------------------------------------------------------------
// 加载一张真正的图片
IBitmap* CImageLoaderEx::_loadImage(const char* szFileName)
{
	if (m_pFS == NULL)
		return NULL;
	// 分析图片类型
	int nImageType = -1;
	char szExt[8];
	int nLen = strlen(szFileName);
	strcpy(szExt, &szFileName[nLen-3]);
	if (strcmpi(szExt, "bmp") == 0)
		nImageType = 0;
	else if (strcmpi(szExt, "tga") == 0)
		nImageType = 1;
  
	if (nImageType == -1)
		return NULL;

	// 读文件
	IBitmap* pImage = NULL;
	IFileObject* pFileObj;
	if ((pFileObj = m_pFS->GetFile(szFileName)) == NULL)
	{
		PrintEventLn("Error: GetFile() : can not get file[%s]", szFileName);
		return NULL;
	}
	if (!pFileObj->Open())
	{
		PrintEventLn("Error: Open() : can not open file[%s]", szFileName);
		return NULL;
	}
	DWORD dwSize = pFileObj->GetLength();
	BYTE* pbuf = new BYTE[dwSize];
	if (pFileObj->Read(pbuf, dwSize) != (UINT)dwSize)
	{
		pFileObj->Release();
		delete[] pbuf;
		PrintEventLn("Error: Read() : read file error[%s]", szFileName);
		return NULL;
	}
	pFileObj->Release();

	// 构造图片对象
	if (nImageType == 0)
	{
		if (!m_CanvasHelper.CreateIBitmap(&pImage))
		{
			delete[] pbuf;
			return NULL;
		}
	}
	else if (nImageType == 1)
	{
		IAlphaBitmap* pAlphaImage;
		if (!m_CanvasHelper.CreateIAlphaBitmap(&pAlphaImage))
		{
			delete[] pbuf;
			return NULL;
		}
		pImage = static_cast<IBitmap*>(pAlphaImage);
	}

	// 加载
	if (!pImage->Load(pbuf))
	{
		pImage->Release();
		delete[] pbuf;
		return NULL;
	}

	delete[] pbuf;
	return pImage;
}
//------------------------------------------------------------------------