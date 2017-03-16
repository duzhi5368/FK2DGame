/**
*	created:		2012-7-1   1:53
*	filename: 		JpegLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		开一个线程实现后台解码jpg
*/
//------------------------------------------------------------------------
#include "../../FKCanvas/IFKCanvas.h"
#include "../Include/JpegLoader.h"
#pragma comment(lib,"../../Depend/ijl/ijl15.lib")
//------------------------------------------------------------------------
#define MAX_JPG_FILE_LEN	(500 * 1024) // 最大的jpg文件不能超过400K
//------------------------------------------------------------------------
//Color modes
#define C16BIT  16
#define C24BIT  24
#define C32BIT  32
//------------------------------------------------------------------------
//16Bit Color Formats
#define PF_555  1
#define PF_565  2
//------------------------------------------------------------------------
//16Bit Color conversion macros
#define RGB555(r, g, b) ((r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10))
#define RGB565(r, g, b) ((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11))
//------------------------------------------------------------------------
#define JPG_FILE_SIZE	300000//291304//118391 //30905	// 分配读取jpg文件的缓存
//------------------------------------------------------------------------
BOOL CJpegLoader::Create(const char* szFileDir,ITrace *pTrace)
{
	Close();
#ifdef USE_PACKAGE_FILESYSTEM
	if (szFileDir && szFileDir[0] != 0)
	{
		char buf[1024];
		int nLen = lstrlen(szFileDir);
		lstrcpyn(buf, szFileDir, 1024);
		if (buf[nLen-1] == '\\' || buf[nLen-1] == '/')
			buf[nLen-1] = '\0';
		
		WIN32_FIND_DATA wfd;
#ifdef _DEBUG
		if (INVALID_HANDLE_VALUE != ::FindFirstFile(buf, &wfd)) // 存在
		{
			if (m_fsHelper.Create(&m_pfsJpgFile, buf, FSS_DIRECTORY, NULL))
			{
				//TraceLn("CreateFileSystem: %s", buf);
			}
		}
		else
#endif
		{
			char buf1[MAX_PATH];
			wsprintf(buf1, "%s.fsp", buf);
			if (INVALID_HANDLE_VALUE != ::FindFirstFile(buf1, &wfd)) // 存在
			{
				m_fsHelper.Create(&m_pfsJpgFile, buf1, FSS_PACKAGE, NULL);
			}
			else
			{
				
			}
		}
	}
	else
	{
		m_fsHelper.Create(&m_pfsJpgFile, "", FSS_DIRECTORY, NULL);
	}
#else
	if(szFileDir != NULL)
		strcpy(m_szJpgFileDir,szFileDir);
	else
		strcpy(m_szJpgFileDir,".\\");
#endif

	m_pJpgFileBuf = new char[MAX_JPG_FILE_LEN];
	if(m_pJpgFileBuf == NULL)
		return FALSE;
	m_pBuffer24 = NULL;
	m_nLastBufferSize = 0;

	if(!m_Thread.Create(pTrace))
	{
		return FALSE;
	}
	DWORD dwEventID;
	if(!m_Thread.m_pThreadRoute->AddEvent(static_cast<IEventSink*>(this),dwEventID))
		return FALSE;
	m_hEventWakeDecode = m_Thread.m_pThreadRoute->GetEventHandle(dwEventID);
	if(!m_Thread.m_pThreadRoute->Start())
	{
		Close();
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 切记，这是在线程中被调用的，嘿嘿
// 功能：解码一个jpg
//........返回出错信息的结点？
void CJpegLoader::OnEvent(DWORD dwEventID)
{
	m_JpgListLock.Lock();
	int nCount = m_JpgList.GetCount();
	if(nCount == 0)
	{
		m_JpgListLock.Unlock();
		return;
	}
	CJpgNode node = m_JpgList.RemoveHead();
	m_JpgListLock.Unlock();

	IBitmap* pBmp = NULL;
	if(node.pData == NULL)
		node.pData = LoadFile(node);
	if(node.pData)
		pBmp = DecodeJpg(node.pData,node.size,node.pBmp);

	m_BmpListLock.Lock();
	m_BmpList.AddTail(CJpgNode(node.dwID,node.nXY,0,NULL,pBmp));
	m_BmpListLock.Unlock();

	// 因不是大量的数据，所以可以间断地解码，跟通讯不同，所以此处不需要优化成只有一个事件时才处理的方式
	SetEvent(m_hEventWakeDecode);
}
//------------------------------------------------------------------------
// 切记，这是在线程中被调用的，嘿嘿
DWORD CJpegLoader::OnTimer(DWORD dwTimerID,DWORD dwParam)
{
	//TraceLn(">>CJpegLoader::OnTimer() : %d",GetTickCount());
	//m_Thread->SetTimer(static_cast<ITimerSink*>(this),1234,1000);
	return 1000;
}
//------------------------------------------------------------------------
void CJpegLoader::Close()
{
	m_Thread.Close();
	if(m_pJpgFileBuf)
	{
		delete[] m_pJpgFileBuf;
		m_pJpgFileBuf = NULL;
	}
	if(m_pBuffer24)
	{
		delete m_pBuffer24;
		m_pBuffer24 = NULL;
	}
	m_nLastBufferSize = 0;
}
//------------------------------------------------------------------------
IBitmap* CJpegLoader::LoadJpgNoUseThread(DWORD dwID)
{
	CJpgNode node;
	node.dwID = dwID;
	IBitmap* pBmp = NULL;
	node.pData = LoadFile(node);
	if (node.pData)
		pBmp = DecodeJpg(node.pData, node.size, pBmp);

	return pBmp;
}
//------------------------------------------------------------------------
BOOL CJpegLoader::AddJpg(DWORD dwID,int nXY,int size,void *pJpgData,IBitmap* pBmp)
{
	m_JpgListLock.Lock();
	m_JpgList.AddTail(CJpgNode(dwID,nXY,size,pJpgData,pBmp));
	SetEvent(m_hEventWakeDecode);	// OnEvent()里会自动触发下一次
	m_JpgListLock.Unlock();
	return TRUE;
}
//------------------------------------------------------------------------
IBitmap* CJpegLoader::DecodeJpgEx(void *pData,int size,IBitmap* pBmp)
{
	if (pBmp == NULL)
		if (!m_CanvasHelper.CreateIBitmap(&pBmp, NULL))
			return NULL;
	struct  MyData{
		int flag;
		int  size;
		void* pData;
	} mydata;
	mydata.flag = 1;
	mydata.size = size;
	mydata.pData = pData;
	if (!pBmp->Load(&mydata))
		return NULL;
	return pBmp;
}
//------------------------------------------------------------------------
// pData : jpeg raw data
// size  : jpeg raw data size
IBitmap* CJpegLoader::DecodeJpg(void *pData,int size,IBitmap* pBmp)
{
	//TRACELN((char*)pData);
	IJLERR jerr;
	//Create the JPEG object
	JPEG_CORE_PROPERTIES jcprops;
	//Initialize the JPEG object
	jerr = ijlInit(&jcprops);
	if (jerr != IJL_OK)
	{
	  //TraceLn("Error : ijlInit problem\n");
	  return NULL;
	}

	//Set the IJL data source as the resource buffer
	jcprops.JPGFile      = NULL;
	jcprops.JPGBytes     = (BYTE*)pData;
	jcprops.JPGSizeBytes = size;

	  //Read JPEG parameters from the buffer
	jerr = ijlRead(&jcprops, IJL_JBUFF_READPARAMS);
	//Make sure Parameter read was successful
	if (jerr != IJL_OK)
	{
	  //TRACE("Error reading JPEG parameters\n");
	  return NULL;
	}

	//Prepare a 24Bit buffer to receive image data
	//Determine the required size
	long szbuff24 = (jcprops.JPGWidth * C24BIT + 7) / 8 * jcprops.JPGHeight;
	if(szbuff24 > m_nLastBufferSize)
	{
		if(m_pBuffer24)
			delete m_pBuffer24;
		//Resize the buffer
		m_pBuffer24 = new BYTE [szbuff24 + 3*1024];	//......可能跟pitch值有关，现加上附加值
		if (m_pBuffer24 == NULL)
		{
			//TRACE("Memory Allocation Error");
			return NULL;
		}
		m_nLastBufferSize = szbuff24;
	}
	//Set up the DIB specification for the JPEG decoder
	jcprops.DIBWidth    = jcprops.JPGWidth;
	jcprops.DIBHeight   = jcprops.JPGHeight; //Implies a bottom-up DIB.
	jcprops.DIBChannels = 3;
	jcprops.DIBColor    = IJL_BGR;
	jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES(jcprops.JPGWidth,3);
	jcprops.DIBBytes    = reinterpret_cast<BYTE*>(m_pBuffer24);

	//Set the JPG color space ... this will always be somewhat of an
	//educated guess at best because JPEG is "color blind" (i.e.,
	//nothing in the bit stream tells you what color space the data was
	//encoded from. However, in this example we assume that we are
	//reading JFIF files which means that 3 channel images are in the
	//YCbCr color space and 1 channel images are in the Y color space.
	switch(jcprops.JPGChannels)
	{
	  case 1: jcprops.JPGColor = IJL_G;
		 break;
  
	  case 3: jcprops.JPGColor = IJL_YCBCR;
		 break;
  
	  default:
		 //This catches everything else, but no color twist will be
		 //performed by the IJL.
		 jcprops.DIBColor = (IJL_COLOR)IJL_OTHER;
		 jcprops.JPGColor = (IJL_COLOR)IJL_OTHER;
		 break;
	}
	//Make sure the read was successful
	jerr = ijlRead(&jcprops, IJL_JBUFF_READWHOLEIMAGE);
	if (jerr != IJL_OK)
	{
	  //TRACE("Error reading JPEG image\n");
	  return NULL;
	}
	/*DWORD dwLast = GetTickCount();
	for(int i = 0; i < 20; i++)
		jerr = ijlRead(&jcprops, IJL_JBUFF_READWHOLEIMAGE);
	char msg[1024];
	//wsprintf(msg,"LoadJPEG() : time = %d ms/%d\n",GetTickCount() - dwLast,i);
	wsprintf(msg,"LoadJPEG() : speed = %d\n",(GetTickCount() - dwLast)/i);
	TRACE(msg);
	MessageBox(NULL,msg,"ok",MB_OK);*/

	if(pBmp != NULL && (pBmp->GetWidth() != jcprops.JPGWidth || pBmp->GetHeight() != jcprops.JPGHeight))
	{
		//TRACELN("Error : CJpegLoader::DecodeJpg() : IBitmap width or height mismatch,recreate a IBitmap");
		pBmp->Release();
		pBmp = NULL;
	}
	if(pBmp == NULL)
	{
		if(!m_CanvasHelper.CreateIBitmap(&pBmp,NULL))
			return NULL;
		if(!pBmp->Create(jcprops.JPGWidth,jcprops.JPGHeight))
		{
			pBmp->Release();
			return NULL;
		}
	}
	struct WBmpStruct
	{
		int kx;				//开始坐标
		int ky;				
		int w;					//宽度
		int h;					//高度
		int pitch;				//行宽字节
		void* ptr;				// 第一扫描行指针
		DWORD userdata;			//用户数据
		BOOL bChild;			//是否为子位图
	};

	WORD* buffer16 = (WORD*)((WBmpStruct*)pBmp->GetBmpStruct())->ptr;

	/*int w = jcprops.JPGWidth;
	int h = jcprops.JPGHeight;
	int i = 0,j=0;
	for(int y = 0; y < w; y))
	{
		for(int x = 0; x < w; x++)
		{
			buffer16[j++] = RGB565(m_pBuffer24[i], m_pBuffer24[i + 1],m_pBuffer24[i + 2]);
			nOff+ = 3;
		}
		j = (j + 7) / 8 * 8;
	}*/
	int j = 0;
	for (int i = 0; i < szbuff24; i += 3)   
		buffer16[j++] = RGB565(m_pBuffer24[i], m_pBuffer24[i + 1],m_pBuffer24[i + 2]);
	return pBmp;
}
//------------------------------------------------------------------------
// 主程序一有空就让解码线程解码
void CJpegLoader::OnThink()
{
	m_Thread.m_pThreadRoute->ResumeThread();
}
//------------------------------------------------------------------------
// 暂停解码
void CJpegLoader::StopThink()
{
	m_Thread.m_pThreadRoute->SuspendThread();
}
//------------------------------------------------------------------------
// 读取一个解码完毕的IBitmap
BOOL CJpegLoader::GetBmp(int& nXY,IBitmap **ppBmp)
{
	if(ppBmp == NULL)
		return FALSE;
	m_BmpListLock.Lock();
	if(m_BmpList.IsEmpty())
	{
		m_BmpListLock.Unlock();
		return FALSE;
	}
	CJpgNode node = m_BmpList.RemoveHead();
 	nXY = node.nXY;
	*ppBmp = (IBitmap*)node.pBmp;
	m_BmpListLock.Unlock();
	return TRUE;
}
//------------------------------------------------------------------------
// 如果旧的请求已经过时，则删除旧的请求啦
void CJpegLoader::RemoveOldJpg(DWORD dwID,IBitmap** ppBmp)
{
	m_JpgListLock.Lock();
	POSITION pos = m_JpgList.GetHeadPosition();
	while(pos)
	{
		POSITION posLast = pos;
		CJpgNode& node = m_JpgList.GetNext(pos);
		if(node.dwID == dwID)
		{
			//TRACELN("=>>>CJpegLoader::RemoveOldJpg() : remove %d",dwID);
			*ppBmp = node.pBmp;
			m_JpgList.RemoveAt(posLast);
			break;
		}
	}
	m_JpgListLock.Unlock();
}
//------------------------------------------------------------------------
//..... 直接读取文件并解码，将来改为从地图文件库中读取数据文件
//..... 注：此处文件读取未加锁，如果多线程序读取就要考虑同步问题
char* CJpegLoader::LoadFile(CJpgNode& node)
{
#ifdef USE_PACKAGE_FILESYSTEM
	char szFile[MAX_PATH];
	wsprintf(szFile,"%04d.jpg",node.dwID);
	node.size = JPG_FILE_SIZE;
	
	static BOOL bLoaded = FALSE;
	//if(!bLoaded)
	{
		IFileObject *file;
		if (!m_pfsJpgFile) return NULL;
		if ((file = m_pfsJpgFile->GetFile(szFile)) == NULL)
		{
			return NULL;
		}
		if (!file->Open())
		{
			return NULL;
		}
		node.size = file->GetLength();
		if (node.size > MAX_JPG_FILE_LEN)
		{
			file->Release();
			return NULL;
		}
		if (file->Read(m_pJpgFileBuf,node.size) != (UINT)node.size)
		{
			file->Release();
			return NULL;
		}
		file->Release();
		bLoaded = TRUE;
	}
#else
	char szFile[MAX_PATH];
	//wsprintf(szFile,"%s/%d.jpg",m_szJpgFileDir,node.dwID);
	wsprintf(szFile,"%s/%04d.jpg",m_szJpgFileDir,node.dwID);
	node.size = JPG_FILE_SIZE;
	
	static BOOL bLoaded = FALSE;
	//if(!bLoaded)
	{
		// 只load第一张
		//wsprintf(szFile,"%s/测试地表03_02.jpg",m_szJpgFileDir);

		FILE *file;
		if((file = fopen(szFile,"rb")) == NULL)
		{
			TRACELN("Error : CJpegLoader::LoadFile() : can not open file : %s",szFile);
			return NULL;
		}
		fseek(file,0,SEEK_END);
		node.size = ftell(file);
		if(node.size > MAX_JPG_FILE_LEN)
		{
			fclose(file);
			TRACELN("Error : CJpegLoader::LoadFile() : file = %s size = %d, %d max",szFile,node.size,MAX_JPG_FILE_LEN);
			return NULL;
		}
		fseek(file,0,SEEK_SET);
		fread(m_pJpgFileBuf,node.size,1,file);
		fclose(file);
		bLoaded = TRUE;
	}
#endif
	return m_pJpgFileBuf;
}
//------------------------------------------------------------------------