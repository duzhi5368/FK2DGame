/**
*	created:		2012-6-24   1:45
*	filename: 		CommonFunc
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "CommonFunc.h"
//------------------------------------------------------------------------
CCommonFunc::CCommonFunc(void)
{
	m_nMapID = INVALID_MAPID;
}
//------------------------------------------------------------------------
CCommonFunc::~CCommonFunc(void)
{
	Close();
}
//------------------------------------------------------------------------
BOOL CCommonFunc::Create()
{
	return TRUE;
}
//------------------------------------------------------------------------
void CCommonFunc::Close()
{
	CloseGameWorld();

	m_MapViewHelper.Close();
}
//------------------------------------------------------------------------
// 小退,清掉冗余资源
void CCommonFunc::ClearMemoryResource(void)
{
	if(g_ClientGlobal.m_pClientWorld)
	{
		g_ClientGlobal.m_pClientWorld->Close();		
	}

	m_MapViewHelper.Close();
	g_ClientGlobal.m_pMapView = NULL;
}
//------------------------------------------------------------------------
// 读取一个文件，返回一个分配内存，请使用者自行释放
char* CCommonFunc::LoadFile(const char *szFileName)
{
	FILE *file;
	if((file = fopen(szFileName,"rb")) == NULL)
	{
		TraceLn("Error : CGlobal::OnEvent() : can not open file : %s",szFileName);
		return NULL;
	}
	fseek(file,0,SEEK_END);
	int size = ftell(file);
	char* p =new char[size];
	if(p == NULL)
		return NULL;
	fseek(file,SEEK_SET,0);
	fread(p,size,1,file);
	fclose(file);
	return p;
}
//------------------------------------------------------------------------
IBitmap* CCommonFunc::LoadBitmap(const char *szFileName)
{
	IBitmap* pBmp = NULL;
	if(!g_ClientGlobal.m_CanvasHelper.CreateIBitmap(&pBmp))
		return NULL;
	char *pFileBuffer = LoadFile(szFileName);
	if(pFileBuffer == NULL)
		return NULL;
	if(!pBmp->Load(pFileBuffer))
	{
		delete pFileBuffer;
		pBmp->Release();
		return NULL;
	}
	delete pFileBuffer;
	return pBmp;
}
//------------------------------------------------------------------------
void MsgBox(LPCSTR szMsg, LPCSTR szCaption, DWORD uType)
{
	MessageBox(g_ClientGlobal.m_hMainWnd,szMsg,szCaption,uType);
}
//------------------------------------------------------------------------
BOOL GetCursorPointTile(POINT &ptTile)
{
	if(g_ClientGlobal.m_pMapView == NULL)
	{
		return FALSE;
	}

	// 这里假设游戏就在主窗口下运行
	if(::GetActiveWindow() != g_ClientGlobal.m_hMainWnd)
	{
		return FALSE;
	}

	if(g_ClientGlobal.m_pMapView == NULL)
	{
		return FALSE;
	}

	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(g_ClientGlobal.m_hMainWnd, &cursor);
	
	const RECT& rcView = g_ClientGlobal.m_pMapView->GetViewportRect();
	RECT rcScreen = {0, 0, rcView.right - rcView.left, rcView.bottom - rcView.top};
	if(!PtInRect(&rcScreen, cursor))
	{	
		return FALSE;
	}

	g_ClientGlobal.m_pMapView->ScreenToTile(cursor,ptTile);
	if(!g_ClientGlobal.m_pMapView->IsValidTile(ptTile))
	{
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------
CMapItemView* HitTestItem(POINT* pptFound)
{
	POINT ptTile;
	if(!GetCursorPointTile(ptTile))
	{
		return NULL;
	}
	
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(g_ClientGlobal.m_hMainWnd,&cursor);
	
	POINT ptFound;
	RECT rcClick ={cursor.x - 3,cursor.y - 3,cursor.x + 3,cursor.y + 3};
	CMapItemView* pItem = g_ClientGlobal.m_pMapView->HitTest(rcClick,&ptFound);
	if(pptFound)
	{
		*pptFound = ptFound;
	}

	return pItem;
}
//------------------------------------------------------------------------
void CenterWindow(HWND hWndCenter)
{
	RECT rcWin,rcDesktop;
	::GetWindowRect(hWndCenter,&rcWin);
	::GetWindowRect(GetDesktopWindow(),&rcDesktop);
	SetWindowPos(hWndCenter,NULL, (rcDesktop.right - rcDesktop.left) / 2 - (rcWin.right - rcWin.left) / 2, 
		(rcDesktop.bottom - rcDesktop.top) / 2 - (rcWin.bottom - rcWin.top) / 2, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
//------------------------------------------------------------------------
BOOL CCommonFunc::LoadMap(int nMapID, POINT ptWorld)
{
	char szMapDir[MAX_PATH] = { 0 };
	sprintf(szMapDir,    MAP_RES_DIR,			g_ClientGlobal.m_szWorkDir);
	IFileSystem * pFileSystem = NULL;
	g_ClientGlobal.m_FileSystemHelper.Create(&pFileSystem, szMapDir, FSS_DIRECTORY, g_ClientGlobal.m_TraceHelper.m_pTrace);
	if(pFileSystem == NULL)
	{
		return FALSE;
	}

	if( g_ClientGlobal.m_pDataCenter == NULL )
	{
		return FALSE;
	}

	MAP_INFOR * pMapInfor = g_ClientGlobal.m_pDataCenter->GetMapInfor(nMapID);
	if(pMapInfor == NULL)
	{
		return FALSE;
	}

	char szMapFile[MAX_PATH] = { 0 };
	char szMaiFile[MAX_PATH] = { 0 };
	char szMiniMap[MAX_PATH] = { 0 };
	char szGroundDir[MAX_PATH] = { 0 };
	char szWorldMap[MAX_PATH] = { 0 };

	sprintf(szMapFile,   "%s/Data/map/%s",		g_ClientGlobal.m_szWorkDir, pMapInfor->szmapfile);
	sprintf(szMaiFile,   "%s/Data/map/%s",		g_ClientGlobal.m_szWorkDir, pMapInfor->szmaifile);
	sprintf(szMiniMap,   "%s/Data/res/grd/%s",	g_ClientGlobal.m_szWorkDir, pMapInfor->szminimapfile);
	sprintf(szWorldMap,  "%s/Data/res/grd/%s",	g_ClientGlobal.m_szWorkDir, pMapInfor->szworldmapfile);
	sprintf(szGroundDir, "%s/Data/res/grd",		g_ClientGlobal.m_szWorkDir);

	IFileObject * pTinyMapFile		= pFileSystem->GetFile(pMapInfor->szworldmapfile);
	IFileObject * pMapFile			= pFileSystem->GetFile(pMapInfor->szmapfile);
	IFileObject * pGridIdxArrayFile = pFileSystem->GetFile(pMapInfor->szmaifile);
	IFileObject * pMiniMapFile		= pFileSystem->GetFile(pMapInfor->szminimapfile);

	RECT rcViewport, rcMiniMapViewport;
	rcViewport.left		= ptWorld.x - GAME_WINDOW_WIDTH / 2;
	rcViewport.top		= ptWorld.y - GAME_WINDOW_HEIGHT / 2;
	rcViewport.right	= rcViewport.left + GAME_WINDOW_WIDTH;
	rcViewport.bottom	= rcViewport.top + GAME_WINDOW_HEIGHT;
	if(rcViewport.left < 0)
	{
		rcViewport.right -= rcViewport.left;
		rcViewport.left = 0;
	}
	if(rcViewport.top < 0)
	{
		rcViewport.bottom -= rcViewport.top;
		rcViewport.top = 0;
	}
	rcMiniMapViewport.left = GAME_WINDOW_WIDTH - 150;
	rcMiniMapViewport.top = 0;
	rcMiniMapViewport.right = GAME_WINDOW_WIDTH;
	rcMiniMapViewport.bottom  = 150;

	/*
	 *	pMapFile: 地图文件
	 *	pGridIdxArrayFile: 地表索引数组文件
	 *	pTinyMapFile: 地图框架图文件
	 *	szgroundJpgFiledir: 地表jpg文件目录
	 *	szMapName: 地图名称
	 *	lprcViewport: 地图当前的视口(宽=1024,高=768)
	 *	lprcMiniMapViewport: 小地图在大地图中的视口
	 *	szMiniMapJpgFileDir: 小地图jpg文件目录
	 *	bUseDynamicMap: 是否使用动态地图管理(当地图较小时，请不要使用动态地图管理)
	 *	nMiniMapRate: 小地图图片的缩放比例(默认为20)
	 *	pItemCF: item工厂
	 *	pTrace: Trace接口
	 */

	if(!m_MapViewHelper.LoadMap(pMapFile,					//	地图文件
								pGridIdxArrayFile,			//  地表索引数组文件
								pTinyMapFile,				//  地图框架图文件
								szGroundDir,				//  地表jpg文件目录
								pMapInfor->szmapname,		//  地图名称
								&rcViewport,				//  地图当前的视口
								&rcMiniMapViewport,			//  小地图在大地图中的视口
								szMiniMap ,					//  小地图jpg文件目录
								pMapInfor->lusedynamic,		//	是否使用动态地图管理
								pMapInfor->lminimaprate,	//  小地图图片的缩放比例
								g_BasicSysHelper.m_pICF,	//  item工厂
								g_ClientGlobal.m_TraceHelper.m_pTrace))	// Trace接口
	{
		TraceColorLn(MY_TRACE_COLOR , "Load Map (MAP ID = %d[%s])FAILED!", nMapID, pMapInfor->szmapname);
		return FALSE;
	}
	
	g_ClientGlobal.m_pMapView = m_MapViewHelper.m_pMapView;

	SAFE_RELEASE(pTinyMapFile);
	SAFE_RELEASE(pMapFile);
	SAFE_RELEASE(pGridIdxArrayFile);

	pFileSystem->Release();
/*
	// 播放背景音乐
	g_BackgroundMusicManager.Play(nMapID);
*/
	m_nMapID = nMapID;

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CCommonFunc::CreateNewWorld(int nMapID,  POINT ptWorld)
{

	if(g_ClientGlobal.m_pClientWorld)
	{
		g_ClientGlobal.m_pClientWorld->Close();		
	}

	m_MapViewHelper.Close();
	g_ClientGlobal.m_pMapView = NULL;

	if(!g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis)
	{
		g_ClientGlobal.m_TimerAxisHelper.m_pTimeAxis->Release();
		g_ClientGlobal.m_TimerAxisHelper.Create();
	}

	try
	{
		// Loading...
		if(!LoadMap(nMapID, ptWorld))
		{
			throw "LoadMap() failed";	
		}

		if(g_ClientGlobal.m_ClientWorld.Create() == FALSE)
		{
			return FALSE;
		}
		
		return TRUE;
	}
	catch(LPCSTR szMsg)
	{
		MessageBox( NULL ,szMsg,"ERROR!",MB_OK);
		CloseGameWorld();
		return FALSE;
	}
	
	return TRUE;
}
//------------------------------------------------------------------------
void CCommonFunc::CloseGameWorld(void)
{
	if(g_ClientGlobal.m_pClientWorld)
	{
		g_ClientGlobal.m_pClientWorld->Close();
		g_ClientGlobal.m_pClientWorld = NULL;
	}

	m_ControllerHelper.Close();
	m_MapViewHelper.Close();
	g_ClientGlobal.m_pMapView = NULL;
	g_BasicSysHelper.Close();
}
//------------------------------------------------------------------------