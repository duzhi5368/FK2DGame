/**
*	created:		2012-7-1   1:45
*	filename: 		JpegLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../../Depend/ijl/ijl.h"
#include "../../FKGame/Thread/IThreadRoute.h"
#include "../../FKFileSystem/Include/FKTempl.h"
#include "../../FKCanvas/IFKCanvas.h"
#include "../../FKFileSystem/IFKFileSystem.h"
//------------------------------------------------------------------------
#ifndef USE_PACKAGE_FILESYSTEM
#define USE_PACKAGE_FILESYSTEM
#endif
//------------------------------------------------------------------------
class CJpegLoader : public IEventSink ,public ITimerEventSink
{
public:
	CJpegLoader::CJpegLoader()
	{
		m_hEventWakeDecode = NULL;
		m_pBuffer24 = NULL;
		m_nLastBufferSize = 0;
		m_pJpgFileBuf = NULL;
#ifdef USE_PACKAGE_FILESYSTEM
		m_pfsJpgFile = NULL;
#else
		m_szJpgFileDir[0] = 0;
#endif
	}

	CJpegLoader::~CJpegLoader()
	{
		Close();
	}

	void RemoveOldJpg(DWORD dwID,IBitmap** ppBmp);
	BOOL Create(const char* szFileDir,ITrace* pTrace);
	void Close();

	BOOL GetBmp(int& nXY,IBitmap **ppBmp);
	void StopThink();
	void OnThink();
	IBitmap* DecodeJpg(void* pData,int size,IBitmap* pBmp);
	IBitmap* DecodeJpgEx(void* pData,int size,IBitmap* pBmp);
	BOOL AddJpg(DWORD dwID,int nXY,int size,void* pJpgData,IBitmap* pBmp);
	IBitmap* LoadJpgNoUseThread(DWORD dwID);

	virtual void OnEvent(DWORD dwEventID);
	virtual DWORD OnTimer(DWORD dwTimerID,DWORD dwParam);

private:
	CThreadRouteHelper		m_Thread;
	HANDLE					m_hEventWakeDecode;
	struct CJpgNode
	{
		CJpgNode()
		{
		}
		CJpgNode(DWORD dwID,int nXY,int size,void* pData,IBitmap* pBmp)
		{
			this->dwID = dwID;
			this->nXY = nXY;
			this->size = size;
			this->pData = pData;
			this->pBmp = pBmp;
		}
		DWORD	dwID;
		int		nXY;	// 地图块坐标
		int		size;
		void*	pData;
		IBitmap* pBmp;
	};
	char* LoadFile(CJpgNode& node);

	CList<CJpgNode,CJpgNode&>		m_JpgList;	// jpg数据缓冲指针列表
	CCriticalSectionLock			m_JpgListLock;

	CList<CJpgNode,CJpgNode&>		m_BmpList;	// jpg解码以后生成bmp放在队列时
	CCriticalSectionLock			m_BmpListLock;

	ICanvasHelper					m_CanvasHelper;

	BYTE* m_pBuffer24;	// 解压bmp所需要缓冲区，
	int	  m_nLastBufferSize;

	char* m_pJpgFileBuf;// 读取jpg文件所要的缓冲，限制最大不能超过200K

#ifdef USE_PACKAGE_FILESYSTEM
	CIFileSystemHelper	m_fsHelper;
	IFileSystem*		m_pfsJpgFile;
#else
	char  m_szJpgFileDir[MAX_PATH];
#endif
};
//------------------------------------------------------------------------