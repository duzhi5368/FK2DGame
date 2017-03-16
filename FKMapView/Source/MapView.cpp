/**
*	created:		2012-7-1   5:56
*	filename: 		MapView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
/*
 * 功能		: 负责地图显示，包括视口管理、九宫格管理等
 * 说明		: 可能有多个地图对象存在，如切换场景时一般把大地图保存在内存中不删除
 * 改进		: 显示排序方法改变为：将可视所有对象加入一个队列中，显示之前作一次排序，然后按排序的顺序显示
 */
//------------------------------------------------------------------------
#include "../Include/MapView.h"
#include "../../FKFileSystem/Include/FKTempl.h"
#include "../../FKGame/SceneObj/IItemClassFactory.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "../../FKGame/WndSys/WndSys.h"
#include "../Include/MapViewCommon.h"
#include <time.h>
//------------------------------------------------------------------------
#define _DRAWOCCUPANT
//------------------------------------------------------------------------
#define SCR_W	1024
#define SCR_H	768
//------------------------------------------------------------------------
// 九宫格单元格大小
#define MAP_GRID_WIDTH				400
#define MAP_GRID_HEIGHT				300
//------------------------------------------------------------------------
CMapView::CMapView()
{
	m_dwDrawFlag = 0;
	m_pSaveBmp = NULL;
	m_pSaveCanvas = NULL;
	
	m_pMengBmp = NULL;
	m_pWngBmp = NULL;
	m_pMengBmpBig = NULL;
	m_pWngBmpBig = NULL;

	m_pMiniBmp = NULL;
	m_pMiniCanvas = NULL;
	m_pMiniBmpBig = NULL;
	m_pMiniCanvasBig = NULL;

	m_ptOldSave.x = -2000;
	m_ptOldSave.y = -2000;
	m_ptSave.x = 0;
	m_ptSave.y = 0;
	m_pOccupantBitmap = NULL;
	m_bReSortDrawItemList = FALSE;
	m_pPlayerItem = NULL;
	m_szGrondFileDirName[0] = 0;
	m_szMapName[0] = 0;
	m_bOptimizedScrollScreen = FALSE;
	m_VisableListGround.clear();
	m_VisableListObj.clear();
	m_VisableListMultiOcc.clear();
	m_DirtyRectList.clear();
	m_bMouseInMiniMap = FALSE;
	m_ptMousePos.x = m_ptMousePos.y = 0;
}
//------------------------------------------------------------------------
CMapView::~CMapView()
{
	SAFE_RELEASE(m_pSaveBmp);
	SAFE_RELEASE(m_pSaveCanvas);

	SAFE_RELEASE(m_pMiniBmp);
	SAFE_RELEASE(m_pMiniCanvas);

	SAFE_RELEASE(m_pMiniBmpBig);
	SAFE_RELEASE(m_pMiniCanvasBig);

}
//------------------------------------------------------------------------
void CMapView::Close()
{
	m_TilesView.Close();
	m_GroundView.Close();
	try
	{
	m_Map.Close();
	}
	catch (...)
	{
	}
}
//------------------------------------------------------------------------
BOOL CMapView::ScrollViewport(int dx, int dy)
{
	if (dx == 0 && dy == 0) 
		return FALSE;
	m_GroundView.ScrollViewport(dx, dy);
	const RECT& rc = GetViewportRect();
	m_ptSave.x = (rc.left + rc.right) / 2;
	m_ptSave.y = (rc.top + rc.bottom) / 2;
	return m_TilesView.ScrollToCenter(m_ptSave);
}
//------------------------------------------------------------------------
int CMapView::OnDrawOccupant(const POINT& ptTile, CTile* pTile, void* pParam)
{
	POINT ptCenter;
	m_GroundView.TileToPixel(ptTile,ptCenter);
	if (pTile->IsBlock())
		((ICanvas*)pParam)->Draw(ptCenter.x-32, ptCenter.y-16 ,64, 32, 
			m_pOccupantBitmap, 0, 32*1, BLIT_MASK);
	if (pTile->IsFlyBlock())
		((ICanvas*)pParam)->Draw(ptCenter.x-32, ptCenter.y-16 ,64, 32, 
			m_pOccupantBitmap, 0, 32*2, BLIT_MASK);
	if (pTile->IsDiffuseBlock())
		((ICanvas*)pParam)->Draw(ptCenter.x-32, ptCenter.y-16 ,64, 32, 
			m_pOccupantBitmap, 0, 32*3, BLIT_MASK);
	return -1;	// 返回负数表示继续enum下一块
}
//------------------------------------------------------------------------
void CMapView::OnSizeViewport(int nViewWidth, int nViewHeight)
{
	if (!m_GroundView.SizeViewport(nViewWidth, nViewHeight))
		return;
	const RECT& rc = GetViewportRect();
	POINT ptPlayerPos;
	ptPlayerPos.x = (rc.left + rc.right) >> 1;
	ptPlayerPos.y = (rc.top + rc.bottom) >> 1;
	m_TilesView.SizeViewport(nViewWidth, nViewHeight);
	m_TilesView.ScrollToCenter(ptPlayerPos);
}
//------------------------------------------------------------------------
// 从"E:\game\bin\res\grd\grass"中解出最后的grass
void ExtractDirNameFromPath(LPCSTR szGroundJpgFileDir, LPSTR szDirName)
{
	int len = strlen(szGroundJpgFileDir);
	int nOff = len - 1;
	while (nOff >= 0 && szGroundJpgFileDir[nOff] != '\\' && szGroundJpgFileDir[nOff] != '/')
		nOff --;
	ASSERT(len - nOff - 1 <= 31); // 字符串长度不超过31
	if (nOff > 0)
		strcpy(szDirName, szGroundJpgFileDir + nOff + 1);
	else
		szDirName[0] = 0;
}
//------------------------------------------------------------------------
// 假设CMapContainer当前有效，创建地表视图
BOOL CMapView::CreateGroundView(LPRECT lprcViewport, 
								IItemClassFactory* pItemCF, 
						LPCSTR szGroundJpgFileDir, IFileObject* pGridIdxArrayFile)
{
	if (!m_Map.IsValid())
	{
		TraceLn("Error : CMapView::CreateGroundView() : the map is not Serialized!");
		return FALSE;
	}

	if (!m_GroundView.Create(m_Map.GetMapWidth(), m_Map.GetMapHeight(),
		MAP_GRID_WIDTH, MAP_GRID_HEIGHT,
		lprcViewport,
		szGroundJpgFileDir, pGridIdxArrayFile, NULL))
		return FALSE;

	ExtractDirNameFromPath(szGroundJpgFileDir, m_szGrondFileDirName); // 记录目录名，以便存盘时保存了.map文件中

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapView::CreateNewMap(int nMapWidth,int nMapHeight,
							LPRECT lprcViewport, LPRECT lprcMiniMapViewport,
							LPCSTR szGroundJpgFileDir, IFileObject* pGridIdxArrayFile, ITrace* pTrace)
{
	Close();
	g_DefaultTrace.Create(pTrace);

	// 将地图的宽高进行64(32)对齐处理
	int w = nMapWidth, h = nMapHeight;
	w &= ~63;
	h &= ~31;

	// 创建坐标系统
	m_MapCo.Create(w, h);

	// 创建地图容器
	RECT rcTilesPreRead = {0, 0, w, h};
	if (!m_Map.Create(w, h, rcTilesPreRead, pTrace))
		return FALSE;

	// 创建地表视图
	if (!CreateGroundView(lprcViewport, NULL, szGroundJpgFileDir, pGridIdxArrayFile))
		return FALSE;

	// 创建地图tile信息视图
	RECT rc = {0, 0, w, h};
	POINT ptPlayerPos = {0, 0};
	if (!m_TilesView.Create(w, h, 
		ptPlayerPos, this, FALSE))
		return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapView::LoadMapView(IFileObject* pMapFile, IFileObject* pGridIdxArrayFile, IFileObject* pTinyMapFile,
						   LPCSTR szGroundJpgFileDir, LPCSTR szMapName, LPRECT lprcViewport, 
						   LPRECT lprcMiniMapViewport, // 地图和小地图的视区
						   LPCSTR szMiniMapJpgFileDir, BOOL bUseDynamicMap, int nMiniMapRate, 
						   IItemClassFactory* pItemCF, ITrace* pTrace)
{
	// 加载地图文档
	if (!m_MapArchiveMgr.LoadMap(pMapFile))
	{
		return FALSE;
	}
	
	// 读地图
	m_Map.SetUseDynamicMap(bUseDynamicMap);
	if (!OnSerialize(FALSE, GetMapArchive(), pItemCF, lprcViewport, pTrace))
	{
		return FALSE;
	}

	// 地图大小64(32)对齐
	int w = GetMapWidth();
	int h = GetMapHeight();
	w &= ~63;
	h &= ~31;

	// 创建坐标系统
	m_MapCo.Create(w, h);

	// 创建地表视图
	char szGrdDir[256];
	wsprintf(szGrdDir, "%s\\%s", szGroundJpgFileDir, m_Map.m_szGround);
	if (!CreateGroundView(lprcViewport, pItemCF, szGrdDir, pGridIdxArrayFile))
	{
		return FALSE;
	}

	// 创建地图tile信息视图
	POINT ptPlayerPos;
	ptPlayerPos.x = (lprcViewport->left + lprcViewport->right) / 2;
	ptPlayerPos.y = (lprcViewport->top + lprcViewport->bottom) / 2;
	if (bUseDynamicMap)
	{
		RECT rcPreRead;
		m_Map.ViewRectToPreReadRect(*lprcViewport, rcPreRead, w, h);
		if (!m_TilesView.Create(w, h, 
			ptPlayerPos, this, TRUE))
		{
			return FALSE;
		}
	}
	else
	{
		RECT rc = {0, 0, w, h};
		if (!m_TilesView.Create(w, h, 
			ptPlayerPos, this, FALSE))
		{
			return FALSE;
		}
	}

	// 创建小地图
	if (!m_MiniMap.Create(w, h, lprcMiniMapViewport, szMiniMapJpgFileDir, nMiniMapRate, pTinyMapFile))
	{
		return FALSE;
	}

	// 小地图上绘制的名字
	if (szMapName)
		lstrcpyn(m_szMapName, szMapName, MAX_SHORT_NAME);

	ICanvasHelper CanvasHelper;
	CanvasHelper.CreateIBitmap(&m_pSaveBmp);
	m_pSaveBmp->Create(SAVE_CANVAS_W, SAVE_CANVAS_H);
	CanvasHelper.CreateICanvas(&m_pSaveCanvas);
	m_pSaveCanvas->SelectBitmap(m_pSaveBmp);

	m_pSaveCanvas->AddWholeScreen();


	char szCurPath[MAX_PATH], szTempPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);

	sprintf(szTempPath, "%s\\Data\\Pic\\MinMap\\MiniMapMeng.tga", szCurPath);
	m_pMengBmp = CImageLoader::GetInstance()->LoadImage(szTempPath);
	if(!m_pMengBmp)
		return FALSE;

	sprintf(szTempPath, "%s\\Data\\Pic\\MinMap\\MiniMapWng.tga", szCurPath);
	m_pWngBmp = CImageLoader::GetInstance()->LoadImage(szTempPath);
	if(!m_pWngBmp)
		return FALSE;

	sprintf(szTempPath, "%s\\Data\\Pic\\MinMap\\MiniMapMeng_Big.tga", szCurPath);
	m_pMengBmpBig = CImageLoader::GetInstance()->LoadImage(szTempPath);
	if(!m_pMengBmpBig)
		return FALSE;

	sprintf(szTempPath, "%s\\Data\\Pic\\MinMap\\MiniMapWng_Big.tga", szCurPath);
	m_pWngBmpBig = CImageLoader::GetInstance()->LoadImage(szTempPath);
	if(!m_pWngBmpBig)
		return FALSE;


	CanvasHelper.CreateIBitmap(&m_pMiniBmp);
	m_pMiniBmp->Create(MINIMAP_SMALL_WIDTH, MINIMAP_SMALL_HEIGHT);
	CanvasHelper.CreateICanvas(&m_pMiniCanvas);
	m_pMiniCanvas->SelectBitmap(m_pMiniBmp);

	CanvasHelper.CreateIBitmap(&m_pMiniBmpBig);
	m_pMiniBmpBig->Create(MINIMAP_BIG_WIDTH, MINIMAP_BIG_HEIGHT);
	CanvasHelper.CreateICanvas(&m_pMiniCanvasBig);
	m_pMiniCanvasBig->SelectBitmap(m_pMiniBmpBig);

	m_pMiniCanvas->AddWholeScreen();
	m_pMiniCanvasBig->AddWholeScreen();

	// 初始化随机数发生器
	g_RandGen.Seed((unsigned)time(NULL));

	sprintf(szTempPath, "%s\\Data\\Pic\\weather", szCurPath);
	if(!m_Weather.Create(szTempPath))
		return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------
IArchive* CMapView::GetMapArchive()
{
	return static_cast<IArchive*>(&m_MapArchiveMgr);
}
//------------------------------------------------------------------------
// DWORD dwFlag
/*
 *	enum {点击了对象范围，对象可选中}
 *  enum {0x0001,0x004}
 *
 */
CMapItemView* CMapView::HitTest(const RECT& rcScreen, POINT* pPointFound, DWORD dwHitFlag)
{
	CMapItemViewPtr pItemView;
	POINT ptCenter;
	RECT rcClick;
	DWORD dwClassID;

	DisplayListPtr it;
	DisplayListPtr end = m_DisplayList.end();
	// 宝箱第一
	for (it=--m_DisplayList.end(); it!=end; --it)
	{
		pItemView = *it;

		if (dwHitFlag & IMapView::htCanSel) // 只选择能选择的对象
		{
			if (!(pItemView->GetFlag() & CMapItemView::flagCanbeSelected))
				continue;
		}
		
		if(pItemView->GetClassID() != miStaticObj_Article)
			continue;

		m_GroundView.PixelToViewTop(pItemView->m_ptWold, ptCenter);
		rcClick = rcScreen;
		rcClick.left	-= 5;
		rcClick.top		-= 5;
		rcClick.right	+= 5;
		rcClick.bottom	+= 5;
		OffsetRect(&rcClick, -ptCenter.x, -ptCenter.y);
		BOOL bRet = pItemView->HitTest(rcClick);
		if (bRet)
		{
			if (pPointFound)
				*pPointFound = pItemView->GetTile();
			return pItemView.pItem;
		}		
	}

	// 摊位第二
	for (it=--m_DisplayList.end(); it!=end; --it)
	{
		pItemView = *it;

		if (dwHitFlag & IMapView::htCanSel) // 只选择能选择的对象
		{
			if (!(pItemView->GetFlag() & CMapItemView::flagCanbeSelected))
				continue;
		}
		
		dwClassID = pItemView->GetClassID();
		if(dwClassID != miStaticObj_Booth
			|| dwClassID == miStaticObj_Article)
			continue;
		
		m_GroundView.PixelToViewTop(pItemView->m_ptWold, ptCenter);
		rcClick = rcScreen;
		rcClick.left	-= 5;
		rcClick.top		-= 5;
		rcClick.right	+= 5;
		rcClick.bottom	+= 5;
		OffsetRect(&rcClick, -ptCenter.x, -ptCenter.y);
		BOOL bRet = pItemView->HitTest(rcClick);
		if (bRet)
		{
			if (pPointFound)
				*pPointFound = pItemView->GetTile();
			return pItemView.pItem;
		}		
	}
	
	// 怪物第三
	for (it=--m_DisplayList.end(); it!=end; --it)
	{
		pItemView = *it;
		if (dwHitFlag & IMapView::htCanSel) // 只选择能选择的对象
		{
			if (!(pItemView->GetFlag() & CMapItemView::flagCanbeSelected))
				continue;
		}
		
		dwClassID = pItemView->GetClassID();
		if(dwClassID != miNpcObj
			|| dwClassID == miStaticObj_Booth
			|| dwClassID == miStaticObj_Article)
			continue;

		m_GroundView.PixelToViewTop(pItemView->m_ptWold, ptCenter);
		rcClick = rcScreen;
		rcClick.left	-= 5;
		rcClick.top		-= 5;
		rcClick.right	+= 5;
		rcClick.bottom	+= 5;
		OffsetRect(&rcClick, -ptCenter.x, -ptCenter.y);
		BOOL bRet = pItemView->HitTest(rcClick);
		if (bRet)
		{
			if (pPointFound)
				*pPointFound = pItemView->GetTile();
			return pItemView.pItem;
		}		
	}
	
	// 传送门第四
	for (it=--m_DisplayList.end(); it!=end; --it)
	{
		pItemView = *it;
		if (dwHitFlag & IMapView::htCanSel) // 只选择能选择的对象
		{
			if (!(pItemView->GetFlag() & CMapItemView::flagCanbeSelected))
				continue;
		}
		
		dwClassID = pItemView->GetClassID();
		if(dwClassID != miAniObj_Transprot
			|| dwClassID == miNpcObj
			|| dwClassID == miStaticObj_Booth
			|| dwClassID == miStaticObj_Article)
			continue;

		m_GroundView.PixelToViewTop(pItemView->m_ptWold, ptCenter);
		rcClick = rcScreen;
		rcClick.left	-= 5;
		rcClick.top		-= 5;
		rcClick.right	+= 5;
		rcClick.bottom	+= 5;
		OffsetRect(&rcClick, -ptCenter.x, -ptCenter.y);
		BOOL bRet = pItemView->HitTest(rcClick);
		if (bRet)
		{
			if (pPointFound)
				*pPointFound = pItemView->GetTile();
			return pItemView.pItem;
		}		
	}
	
	// 其他最后
	for (it=--m_DisplayList.end(); it!=end; --it)
	{
		pItemView = *it;
		if(pItemView->GetClassID() == miStaticObj_Article
			|| pItemView->GetClassID() == miStaticObj_Booth)
			continue;

		if (dwHitFlag & IMapView::htCanSel) // 只选择能选择的对象
		{
			if (!(pItemView->GetFlag() & CMapItemView::flagCanbeSelected))
				continue;
		}
		m_GroundView.PixelToViewTop(pItemView->m_ptWold, ptCenter);
		rcClick = rcScreen;
		OffsetRect(&rcClick, -ptCenter.x, -ptCenter.y);
		BOOL bRet = pItemView->HitTest(rcClick);
		if (bRet)
		{
			if (pPointFound)
				*pPointFound = pItemView->GetTile();
			return pItemView.pItem;
		}		
	}
	return NULL;
}
//------------------------------------------------------------------------
// 保存地图图片(或缩略图)
BOOL CMapView::SaveMapBitmapToFile(LPCSTR szFileName, int nStartRow, int nStartCol, int nRows, int nCols, 
									 float fRate, BOOL bDrawGround, BOOL bCombine)
{
	const RECT& rc = m_GroundView.GetViewportRect();
	RECT rcOld;
	CopyRect(&rcOld, &rc);
	
	OnSizeViewport(SCR_W, SCR_H);

	int nMapWidthScr = PixelCood2GridCood(GetMapWidth(), SCR_W);
	int nMapHeightScr = PixelCood2GridCood(GetMapHeight(), SCR_H);
	if (nMapWidthScr < nStartRow + nRows)
		nRows = nMapWidthScr - nStartRow;
	if (nMapHeightScr < nStartCol + nCols)
		nCols = nMapHeightScr - nStartCol;
	if (nRows <= 0 || nCols <= 0)
		return FALSE;

	ICanvasHelper CanvasHelper;
	IBitmap* pBmp,*pCombineBmp,*pMiniBmp;
	if(!CanvasHelper.CreateIBitmap(&pBmp, NULL))
		return FALSE;
	if (bCombine)
	{
		if(!CanvasHelper.CreateIBitmap(&pCombineBmp, NULL))
			return FALSE;
		if(!pCombineBmp->Create(SCR_W*nCols*fRate,SCR_H*nRows*fRate))
		{
			pCombineBmp->Release();
			return FALSE;
		}
		pCombineBmp->Clear(0);
		if(!CanvasHelper.CreateIBitmap(&pMiniBmp, NULL))
			return FALSE;
		if(!pMiniBmp->Create(SCR_W*fRate, SCR_H*fRate))
		{
			pMiniBmp->Release();
			return FALSE;
		}
		pMiniBmp->Clear(0);
	}

	ICanvas* pCanvas;
	if(!CanvasHelper.CreateICanvas(&pCanvas))
	{
		pBmp = NULL;
		return FALSE;
	}
	if(!pBmp->Create(SCR_W, SCR_H))
	{
		pBmp->Release();
		return FALSE;
	}
	pCanvas->SelectBitmap(pBmp);
	pCanvas->AddWholeScreen();

	int nW = SCR_W*fRate;
	int nH = SCR_H*fRate;

	// 处理每一屏
	char buf[256];
	int nDoneCount = 0;
	ScrollViewport(-m_GroundView.GetMapWidth(), -m_GroundView.GetMapHeight()); // 确保滚动到左上角
	ScrollViewport(SCR_W*nStartCol, SCR_H*nStartRow);
	for (int i=nStartRow; i<nStartRow+nRows; i++) // 每一行
	{
		TraceLn("save line = %d", i);
		for (int j=nStartCol; j<nStartCol+nCols; j++) // 每一列
		{
			pBmp->Clear(0);
			if(bDrawGround)
				m_GroundView.OnDraw(pCanvas);
			
			//m_DisplayList.sort(); // 排一下顺序先
			DisplayListPtr end = m_DisplayList.end();
			for(DisplayListPtr it=m_DisplayList.begin(); it!=end; ++it)
			{
				POINT ptCenter;
				m_GroundView.PixelToViewTop((POINT&)(*it).pItem->m_ptWold, ptCenter);
				(*it)->OnDraw(pCanvas,ptCenter,BLIT_MASK);
			}
			if (!bCombine)
			{
				wsprintf(buf, "%s_%06d.bmp", szFileName, i*nCols+j);
				pBmp->Save(buf);
				if (fRate!=1.0f)
					StretchBitmap(buf, nW, nH);
			}
			else
			{
				wsprintf(buf, "%s", szFileName);
				pBmp->Save(szFileName);
				if (fRate!=1.0f)
					StretchBitmap(buf, nW, nH);

				FILE* hFile = fopen(buf, "rb");
				fseek(hFile, 0, SEEK_END);
				int nFileSize = ftell(hFile);
				fseek(hFile, 0, SEEK_SET);
				BYTE* pFileBuf = new BYTE[nFileSize];
				fread(pFileBuf, nFileSize, 1, hFile);
				fclose(hFile);
				pMiniBmp->Clear(0);
				pMiniBmp->Load(pFileBuf);
				delete[] pFileBuf;
				pCombineBmp->Draw((j-nStartCol)*nW, (i-nStartRow)*nH, nW, nH, pMiniBmp, 0, 0);
			}

			if (j < nStartCol+nCols-1)
				ScrollViewport(SCR_W, 0);
		}
		ScrollViewport(-m_GroundView.GetMapWidth(), 0); // 确保滚动到本行的开头
		ScrollViewport(SCR_W*nStartCol, SCR_H); // 下一行的正确位置
	}

	pCanvas->Release();
	ScrollViewport(-m_GroundView.GetMapWidth(), -m_GroundView.GetMapHeight()); // 确保滚动到左上角

	if (bCombine && pCombineBmp)
	{
		pCombineBmp->Save(szFileName);
	}

	OnSizeViewport(rcOld.right - rcOld.left, rcOld.bottom - rcOld.top);
	
	TraceLn("Save map bitmap.......OK");
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapView::StretchBitmap(LPCSTR szFileName, int nNewWidth, int nNewHeight)
{
	// 先以规定大小加载图片到内存
	HBITMAP hBmp = (HBITMAP)::LoadImage(GetModuleHandle(NULL), szFileName, 
		IMAGE_BITMAP, nNewWidth, nNewHeight, LR_CREATEDIBSECTION|LR_LOADFROMFILE);
	if (!hBmp) return FALSE;
	
	BITMAP m_Bm;
	HDC hMemDC = NULL;
	
	::CreateCompatibleDC(hMemDC);
	::SelectObject(hMemDC, hBmp);
	::GetObject(hBmp, sizeof(m_Bm),&m_Bm);
	
	// 构造文件头和信息头
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	
	int headsize = sizeof(bfh) + sizeof(bih);
	int nFileSize = headsize + m_Bm.bmWidthBytes*m_Bm.bmHeight;
	
	bfh.bfType = 'MB';
	bfh.bfSize = nFileSize;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = headsize;
	
	bih.biSize			= sizeof(bih);
	bih.biWidth			= nNewWidth;
	bih.biHeight		= nNewHeight;
	bih.biPlanes		= 1;
	bih.biBitCount		= 24;
	bih.biCompression	= 0;
	bih.biSizeImage		= 0;
	bih.biXPelsPerMeter	= 72;
	bih.biYPelsPerMeter	= 72;
	bih.biClrUsed		= 0;
	bih.biClrImportant	= 0;

	if (nFileSize)
	{
		BYTE* pFileBuf = new BYTE[nFileSize];
		
		memcpy(pFileBuf, &bfh, sizeof(bfh));
		memcpy(pFileBuf + sizeof(bfh), &bih, sizeof(bih));
		memcpy(pFileBuf + headsize, m_Bm.bmBits, nFileSize - headsize);
		
		FILE* hFile = fopen(szFileName, "w+b");
		fwrite(pFileBuf, nFileSize, 1, hFile);
		fclose(hFile);

		delete[] pFileBuf;
		return TRUE;
	}
	
	return FALSE;
}
//------------------------------------------------------------------------
// 以屏幕某点为中心，滚动屏幕
BOOL CMapView::ScrollToCenter(POINT ptScreenCenter)
{
	int dx = ptScreenCenter.x - m_GroundView.GetViewWidth() / 2;
	int dy = ptScreenCenter.y - m_GroundView.GetViewHeight() / 2;
	if (dx == 0 && dy == 0)
		return FALSE;
	if (!m_bOptimizedScrollScreen)
		return ScrollViewport(dx, dy);
#define VERT_CONST	3
	int ddx, ddy;
	ddx = ddy = 0;
	if (dx < 0)
	{
		if (dx < -400)
			ddx = dx;
		else
		{
			ddx = (-dx) >> (VERT_CONST+1);
			ddx = -ddx;
			if (ddx == 0)
				ddx = -1;
		}

	}
	else if (dx > 0)
	{
		if (dx > 400)
			ddx = dx;
		else
		{
			ddx = dx >> (VERT_CONST+1);
			if (ddx == 0)
				ddx = 1;
		}
	}

	if (dy < 0)
	{
		if (dy < -300)
			ddy = dy;
		else
		{
			ddy = (-dy) >> VERT_CONST;
			ddy = -ddy;
			if (ddy == 0)
				ddy = -1;
		}
	}
	else if (dy > 0)
	{
		if (dy > 300)
			ddy = dy;
		else
		{
			ddy = dy >> VERT_CONST;
			if (ddy == 0)
				ddy = 1;
		}
	}
	ScrollViewport(ddx, ddy);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapView::OnSerialize(BOOL bWrite, IArchive* pArchive, IItemClassFactory *pItemCF, LPRECT lprcViewport, ITrace* pTrace)
{
	if(bWrite)
		return m_Map.OnSaveMap(pArchive, m_szGrondFileDirName, pTrace);
	else
	{
		if (lprcViewport)
			return m_Map.OnLoadMap(pArchive, pItemCF, lprcViewport, pTrace);
		else
			return m_Map.OnAddMap112(pArchive, pItemCF, lprcViewport, pTrace);
	}
}
//------------------------------------------------------------------------
// 运行时刻加入地图的一般都是逻辑对象
// 有一个隐患：当把一个对象加入队列两次时，且删除时只删除一个，会导致引用无效对象地址
// 人物立即读取资源，而不是不预读
BOOL CMapView::AddItem(const POINT& ptTile, CMapItemView* pItem, DWORD dwParam)
{
	if (!pItem)
		return FALSE;
	pItem->SetTile((POINT&)ptTile, static_cast<IMapView*>(this));
	
	if(dwParam != 2)
	{
		// 先将逻辑对象添加到list中(pItem->GetOccupantValue()可以知道是否是尸体)
		if (pItem->GetItemData() && dwParam == 0)
		{
			m_TilesView.AddItemToList(pItem, ptTile);
		}
	}

	// 添加到地图容器
	m_Map.AddItem(ptTile, pItem, dwParam);
	
	if(dwParam != 2)
	{
		// 添加到显示列表
		if (IsItemInViewArea(ptTile, pItem))
			OnMapItemEnterViewport(ptTile, pItem, NULL);
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapView::RemoveItem(const POINT& ptTile, CMapItemView* pItem, DWORD dwParam)
{
	if (!pItem)
		return FALSE;
	
	if(dwParam != 2)
	{
		// 先从list中删除逻辑对象
		if (pItem->GetItemData() && dwParam == 0)
		{
			m_TilesView.RemoveItemFromList(pItem, ptTile);
		}

		// 从显示列表中移除
		if (pItem->IsVisable())
		{
			OnMapItemLeaveViewport(ptTile, pItem, NULL);
		}
	}

	// 从地图容器中移除
	if (!m_Map.RemoveItem(ptTile, pItem, dwParam))
	{

	}
	
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapView::MoveItem(const POINT& ptTileFrom, const POINT& ptTileTo, CMapItemView* pItem, DWORD dwParam)
{
	if (!pItem)
		return FALSE;
	
	// 将逻辑对象在list中移动
	if (pItem->GetItemData())
	{
		m_TilesView.MoveItemInList(pItem, ptTileFrom, ptTileTo);
	}

	// 先从地图容器中移除
	m_Map.RemoveItem(ptTileFrom, pItem, dwParam);
	
	// 加入到地图容器中
	m_Map.AddItem(ptTileTo, pItem, dwParam);

	// 更新显示列表
	if (pItem->IsVisable())
	{
		// 可见 -> 不可见
		if (!IsItemInViewArea(ptTileTo, pItem))
		{
			OnMapItemLeaveViewport(ptTileFrom, pItem, NULL);
		}
		else
		{
			// 可见 -> 可见 ：更新位置，等待重新排序
			UpdateItemInDisplayList(ptTileTo, pItem, TRUE);
		}
	}
	else
	{
		// 不可见 -> 可见区域
		if (IsItemInViewArea(ptTileTo, pItem)) 
		{
			OnMapItemEnterViewport(ptTileTo, pItem, NULL);
		}
		// 不可见 -> 不可见
	}
	
	pItem->SetTile((POINT&)ptTileTo, static_cast<IMapView*>(this));
	return TRUE;
}
//------------------------------------------------------------------------
// 加入对象占位信息
BOOL CMapView::AddItemOccupant(const POINT& ptTile, CMapItemView* pItem)
{
	// 添加到地图容器
	return m_Map.AddItemOccupant(ptTile, pItem);
}
//------------------------------------------------------------------------
// 删除对象占位信息
BOOL CMapView::RemoveItemOccupant(const POINT& ptTile, CMapItemView* pItem)
{
	// 从地图容器中移除
	return m_Map.RemoveItemOccupant(ptTile, pItem);
}
//------------------------------------------------------------------------
void CMapView::AddItemToDisplayList(const POINT& ptTile, CMapItemView* pItem)
{
	ASSERT(!pItem->IsVisable());
	pItem->SetTile((POINT&)ptTile, static_cast<IMapView*>(this));
	pItem->AddFlag(CMapItemView::flagVisable);
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;
	m_DisplayList.push_front(ptr);
}
//------------------------------------------------------------------------
void CMapView::UpdateItemInDisplayList(const POINT& ptTile, CMapItemView* pItem, BOOL bVisible)
{
	pItem->SetTile((POINT&)ptTile, static_cast<IMapView*>(this));
	if (bVisible)
		pItem->AddFlag(CMapItemView::flagVisable);
	else
		pItem->RemoveFlag(CMapItemView::flagVisable);
}
//------------------------------------------------------------------------
void CMapView::RemoveItemFromDisplayList(CMapItemView* pItem)
{
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;
	if (pItem->IsVisable())
	{
		m_DisplayList.remove(ptr);
		pItem->RemoveFlag(CMapItemView::flagVisable);
	}
}
//------------------------------------------------------------------------
void CMapView::AddItemToLogicList(CMapItemView* pItem)
{
	m_LogicItemList.push_front(pItem);
}
//------------------------------------------------------------------------
void CMapView::RemoveItemFromLogicList(CMapItemView* pItem)
{
	m_LogicItemList.remove(pItem);
}
//------------------------------------------------------------------------
// item即将进入视口(这个视口一般比实际的视口(800*600)要大)
void CMapView::OnMapItemEnterViewport(const POINT& ptTile, CMapItem* pItem, void* pParam)
{
	if (!pItem)
		return;

	CMapItemView* pItemView = (CMapItemView*)pItem;
	if (!pItemView->OnEnterViewport(TRUE))
	{
		TraceColorLn(MY_TRACE_COLOR, "CMapView::OnMapItemEnterViewport,OnEnterViewport(TRUE) failed!");		
	}

	// 加入显示列表
	if (!pItemView->IsVisable())
		AddItemToDisplayList(ptTile, pItemView);
	else
	{
		// 已经在显示列表中，需要检查位置实际上已经发生了改变
		ASSERT(0); // 应该不会执行到这里(使用随机转轴时会跳到这里)
		UpdateItemInDisplayList(ptTile, pItemView, TRUE);
	}
}
//------------------------------------------------------------------------
// item已经离开视口
void CMapView::OnMapItemLeaveViewport(const POINT& ptTile, CMapItem* pItem, void* pParam)
{
	if (!pItem)
		return;

	CMapItemView* pItemView = (CMapItemView*)pItem;

	if (!pItemView->OnEnterViewport(FALSE))
	{
		TraceColorLn(MY_TRACE_COLOR, "Item 移出视口失败,ID = %d", pItem->GetClassID());
	}

	// 如果在显示列表中则删除
	RemoveItemFromDisplayList(pItemView);
}
//------------------------------------------------------------------------
// 检查对象是否在可显示的范围之内
BOOL CMapView::IsItemVisable(const POINT& ptTile, CMapItemView* pItemView)
{
	POINT ptCenter;
	ptCenter = pItemView->m_ptWold;
	RECT rcItem = pItemView->GetShowRect();
	OffsetRect(&rcItem, ptCenter.x, ptCenter.y);
	return IntersectRect(&rcItem, &rcItem, &m_GroundView.GetViewportRect());
}
//------------------------------------------------------------------------
// 对象在视图预读区域内
BOOL CMapView::IsItemInViewArea(const POINT& ptTile, CMapItemView* pItemView)
{
	POINT ptCenter;
	ptCenter = pItemView->m_ptWold;
	RECT rcPreRead;
	m_TilesView.GetViewCoveredGridRect(rcPreRead);
	return PtInRect(&rcPreRead, ptCenter);
}
//------------------------------------------------------------------------
// 对象在逻辑预读区域内
BOOL CMapView::IsItemInLogicArea(const POINT& ptTile, CMapItemView* pItemView)
{
	POINT ptCenter;
	ptCenter = pItemView->m_ptWold;
	RECT rcPreRead;
	m_TilesView.GetPreReadCoveredGridRect(rcPreRead);
	return PtInRect(&rcPreRead, ptCenter);
}
//------------------------------------------------------------------------
// 绘制地图
void CMapView::OnDraw(ICanvas* pCanvas, const void* pParam)
{
	if (!m_Map.IsValid())
		return;

	BOOL bAfterPlayer = FALSE;
	RECT rcPlayerHitRect;
	POINT ptPlayerCenter;
	DisplayListPtr end, it;
	CMapItemViewPtr pItemViewPtr;

	// 获取主角在屏幕上位置，以便判断其后的大对象是否需要半透明
	if (m_pPlayerItem)
	{
		// 主角大致范围(注：这样的做法有一个问题，当主角变身以后，主角的大小会发生改变，象这样取固定大小就不合适了)
		m_GroundView.PixelToViewTop(m_pPlayerItem->m_ptWold, ptPlayerCenter);
		rcPlayerHitRect.left = ptPlayerCenter.x - 3;
		rcPlayerHitRect.right = ptPlayerCenter.x + 3;
		rcPlayerHitRect.top = ptPlayerCenter.y - 3;
		rcPlayerHitRect.bottom = ptPlayerCenter.y + 3;
	}
	
	// 计算上次画图与这次的偏移
	int nDrawIncx = m_ptOldSave.x - m_ptSave.x;
	int nDrawIncy = m_ptOldSave.y - m_ptSave.y;
	m_ptOldSave = m_ptSave;
	m_pSaveCanvas->ResetDirtyRect();

	// 获取需要显示的东东
	m_VisableListGround.clear();
	m_VisableListObj.clear();
	m_VisableListMultiOcc.clear();
	end = m_DisplayList.end();
	for (it=m_DisplayList.begin(); it!=end; ++it)
	{
		pItemViewPtr = *it;
		POINT ptTile = pItemViewPtr->GetTile();
		if (IsItemVisable(ptTile, pItemViewPtr.pItem))
		{
			if(pItemViewPtr.pItem->GetLayerValue() <= milvSpecificBlock)
			{
				AddSortItem3(pItemViewPtr.pItem, m_VisableListGround);
			}
			else
			{
				AddSortItem0(pItemViewPtr.pItem, m_VisableListMultiOcc);
			}
		}
	}

	if(nDrawIncx != 0 || nDrawIncy != 0)
	{
		if(abs(nDrawIncx) < SCR_W && abs(nDrawIncy) < SCR_H)
		{
			m_pSaveCanvas->Scroll(nDrawIncx,nDrawIncy);

			RECT area1 = {0, 0, SAVE_CANVAS_W, SAVE_CANVAS_H};
			RECT area2 = {0, 0, SAVE_CANVAS_W, SAVE_CANVAS_H};

			if(nDrawIncx > 0)
				area1.right = nDrawIncx;
			else
				area1.left = SAVE_CANVAS_W + nDrawIncx;

			if(nDrawIncy > 0)
				area2.bottom = nDrawIncy;
			else
				area2.top = SAVE_CANVAS_H + nDrawIncy;

			m_pSaveCanvas->AddDirtyRect(area1);
			m_pSaveCanvas->AddDirtyRect(area2);
		}
		else
		{
			m_pSaveCanvas->AddWholeScreen();
		}
		// 绘制地表
		m_GroundView.OnDraw(m_pSaveCanvas);

		// 绘制地表上的特殊块
		end = m_VisableListGround.end();
		for (it = m_VisableListGround.begin(); it!=end; ++it)
		{
			CMapItemViewPtr& pItemViewPtr = *it;
			POINT ptCenter;
			m_GroundView.PixelToViewTop(pItemViewPtr.pItem->m_ptWold, ptCenter);
			pItemViewPtr.pItem->OnDraw(m_pSaveCanvas, ptCenter, BLIT_MASK);
		}
	}

	pCanvas->Draw(0, 0,m_pSaveCanvas);

#ifndef _DRAWOCCUPANT
	if (m_dwDrawFlag & maskDrawGrid)
#else
	if (m_dwDrawFlag & maskDrawOccupant)
#endif
	m_GroundView.DrawTileLine(pCanvas);

	// 绘制地表上的东东
	AddPointToList();
	
	end = m_VisableListObj.end();
	for (it=m_VisableListObj.begin(); it!=end; ++it)
	{
		CMapItemViewPtr& pItemViewPtr = *it;
		POINT ptCenter;
		m_GroundView.PixelToViewTop(pItemViewPtr.pItem->m_ptWold, ptCenter);
		pItemViewPtr.pItem->OnDraw(pCanvas, ptCenter, BLIT_MASK);
	}
	
	// 主角以半透明重绘
	if (m_pPlayerItem)// && bPlayerNeedTrans)
		m_pPlayerItem->OnDraw(pCanvas, ptPlayerCenter, BLIT_MASK | BLIT_TRANS);		
	
	// 绘制顶层对象
	int nSortIndex=0;
	end = m_VisableListObj.end();
	for (it=m_VisableListObj.begin(); it!=end; ++it,nSortIndex++)
	{
		CMapItemViewPtr& pItemViewPtr = *it;
		POINT ptCenter;
		m_GroundView.PixelToViewTop(pItemViewPtr.pItem->m_ptWold, ptCenter);
		pItemViewPtr.pItem->OnDrawTopmostObject(pCanvas, ptCenter, (DWORD)nSortIndex);
	}

	// 绘制占位块标志
#ifndef _DRAWOCCUPANT
	if ((m_dwDrawFlag & maskDrawOccupant) && m_pOccupantBitmap)
#else
	if (m_dwDrawFlag & maskDrawOccupant)
#endif
	{
		#ifdef _DRAWOCCUPANT
		if (m_pOccupantBitmap == NULL)
		{
			HANDLE f1 = CreateFile("Data\\Res\\flags.bmp", GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if (f1 != INVALID_HANDLE_VALUE)
			{
				m_MiniMap.m_CanvasHelper.CreateIBitmap(&m_pOccupantBitmap);
				DWORD realsize, size = GetFileSize(f1, NULL);
				BYTE* pbuf = new BYTE[size];
				ReadFile(f1, pbuf, size, &realsize, NULL);
				m_pOccupantBitmap->Load(pbuf);
				CloseHandle(f1);
				delete[] pbuf;
			}
		}
		#endif
		RECT rcWorld = m_GroundView.GetViewportRect();
		InflateRect(&rcWorld, 64 * 2, 32 * 2);
		IntersectRect(&rcWorld, &rcWorld, &m_MapCo.GetMapRect());
		SNAP_TILE ListBuf[MAX_SNAP_ITEM_COUNT];
		int nListCount = MAX_SNAP_ITEM_COUNT;
 		if (m_Map.SnapshotTileByWorldRect(rcWorld, nListCount, ListBuf))
		{
			for (int i=0; i<nListCount; i++)
			{
				OnDrawOccupant(ListBuf[i].ptTile, ListBuf[i].pTile, pCanvas);
			}
		}
	}
	
	// 绘制天气
	if(m_Weather.NeedDraw())
	{
		const RECT& rcView = m_GroundView.GetViewportRect();
		int nScrCenterX = (rcView.left + rcView.right) / 2;
		int nScrCenterY = (rcView.top + rcView.bottom) / 2;

		m_Weather.OnDraw(pCanvas, nScrCenterX, nScrCenterY);
	}

	// 绘制小地图
	DrawObjectInMiniMap(pCanvas, (ITTFont*)pParam);

#ifdef _DRAWOCCUPANT
	if (m_dwDrawFlag & maskDrawOccupant)
	{
		DrawRectangleGrid(pCanvas, 1024, 512, WPixel(0,255,0), WPixel(0,0,255));
	}
	if (m_pPlayerItem)
	{
		char buf[256];
		POINT ptTile = m_pPlayerItem->GetTile();
		POINT ptGrid, ptTileCenter;
		ptTileCenter = m_pPlayerItem->m_ptWold;
		ptGrid.x = ptTileCenter.x >> SHIFT_WIDTH;
		ptGrid.y = ptTileCenter.y >> SHIFT_HEIGHT;
		wsprintf(buf, "%d,%d [%d,%d]", ptTile.x, ptTile.y, ptGrid.x, ptGrid.y);
		pCanvas->Draw(200, 2, buf, WPixel(255,255,255));
	}
#endif
}
//------------------------------------------------------------------------
// 在屏幕上画矩形网格
void CMapView::DrawRectangleGrid(ICanvas* pCanvas, int nGridWidth, int nGridHeight, WPixel clrLine, WPixel clrText)
{
	ASSERT(nGridWidth > 0 && nGridHeight > 0);
	if (!pCanvas) return;
	const int nMaxLines = 100;

	// 视口大小
	int nViewWidth = m_GroundView.GetViewWidth();
	int nViewHeight = m_GroundView.GetViewHeight();
	
	// 计算视区内有多少条竖线和横线
	int nOffx = m_GroundView.GetViewTopX() % nGridWidth;
	int xnum = PixelCood2GridCood(nOffx + nViewWidth, nGridWidth);
	if (xnum > nMaxLines) xnum = nMaxLines;
	int nOffy = m_GroundView.GetViewTopY() % nGridHeight;
	int ynum = PixelCood2GridCood(nOffy + nViewHeight, nGridHeight);
	if (ynum > nMaxLines) ynum = nMaxLines;
	
	char scroff[128];
	struct stInfo
	{
		int nPosOff;
		int nScrOff;
	};
	
	stInfo xoff[nMaxLines];
	int x = 0;
	while (xnum > 0) // 画竖线
	{
		POINT ptFrom, ptTo;
		ptFrom.x = ptTo.x = nGridWidth - nOffx + (xnum-2)*nGridWidth;
		ptFrom.y = 0, ptTo.y = nViewHeight;
		pCanvas->DrawLine(ptFrom, ptTo, clrLine);
		int xSrcOff = (m_GroundView.GetViewTopX()+ptFrom.x)/nGridWidth;
		xnum --;
		xoff[x].nPosOff = ptFrom.x;
		xoff[x++].nScrOff = xSrcOff;
	}
	
	stInfo yoff[nMaxLines];
	int y = 0;
	while (ynum > 0) // 画横线
	{
		POINT ptFrom, ptTo;
		ptFrom.x = 0, ptTo.x = nViewWidth;
		ptFrom.y = ptTo.y = nGridHeight - nOffy + (ynum-2)*nGridHeight;
		pCanvas->DrawLine(ptFrom, ptTo, clrLine);
		int ySrcOff = (m_GroundView.GetViewTopY()+ptFrom.y)/nGridHeight;
		ynum --;
		yoff[y].nPosOff = ptFrom.y;
		yoff[y++].nScrOff = ySrcOff;
	}
	
	// 绘制每个网格的坐标
	for (int i=0; i<x; i++)
	{
		for (int j=0; j<y; j++)
		{
			wsprintf(scroff, "%d,%d", xoff[i].nScrOff, yoff[j].nScrOff);
			RECT rc = {xoff[i].nPosOff, yoff[j].nPosOff, xoff[i].nPosOff+50, yoff[j].nPosOff+12};
			pCanvas->DrawBox(rc, clrLine, BLIT_TRANS, 150);
			pCanvas->Draw(xoff[i].nPosOff+2, yoff[j].nPosOff+2, scroff, clrText);
		}
	}
}
//------------------------------------------------------------------------
#ifdef _DEBUG
void CMapView::DrawRect(ICanvas *pCanvas, RECT& rc, WPixel clr)
{
	if (!pCanvas) return;
	POINT ptFrom,ptTo;
	ptFrom.x=rc.left, ptFrom.y=rc.top, ptTo.x=rc.right-1, ptTo.y=rc.top;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 上横
	ptFrom.x=rc.right-1, ptFrom.y=rc.top, ptTo.x=rc.right-1, ptTo.y=rc.bottom;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 右竖
	ptFrom.x=rc.right-1, ptFrom.y=rc.bottom-1, ptTo.x=rc.left, ptTo.y=rc.bottom-1;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 下横
	ptFrom.x=rc.left, ptFrom.y=rc.bottom-1, ptTo.x=rc.left, ptTo.y=rc.top;
	pCanvas->DrawLine(ptFrom, ptTo, clr); // 左竖
}
#endif
//------------------------------------------------------------------------
// 绘制小地图中的各种对象
void CMapView::DrawObjectInMiniMap(ICanvas* pCanvas, ITTFont* pFont)
{	
	BOOL bForceTrans = FALSE;
	if (m_MiniMap.IsVisible())
	{
		// 绘制小地图图片
		const RECT& rcView = m_GroundView.GetViewportRect();
		int nScrCenterX = (rcView.left + rcView.right) / 2;
		int nScrCenterY = (rcView.top + rcView.bottom) / 2;

		if(m_pPlayerItem != NULL)
		{
			POINT ptTile = {0,0};
			m_pPlayerItem->OnCommand(CrtCmdGetProperty,CrtPropAbsolutePosition,(DWORD)&ptTile);
			
			int dx = ptTile.x - m_GroundView.GetViewWidth() / 2;
			int dy = ptTile.y - m_GroundView.GetViewHeight() / 2;
			nScrCenterX += dx;
			nScrCenterY += dy;
		}

		if(m_MiniMap.GetMiniMapBigOrSmall())
		{
			m_pMiniCanvasBig->Clear(0);
			m_MiniMap.OnDraw(m_pMiniCanvasBig, nScrCenterX, nScrCenterY, bForceTrans);
		}
		else
		{
			m_pMiniCanvas->Clear(0);
			m_MiniMap.OnDraw(m_pMiniCanvas, nScrCenterX, nScrCenterY, bForceTrans);
		}

		POINT  ptCenter;
		CObjectList* pList = m_TilesView.GetObjectList();
		ASSERT(pList);
		MapItemInfoList* pObjList = pList->GetListPtr();
		MapItemInfoListPtr _end = pObjList->end();
		WPixel wpMinMapFlagColor;
		for (MapItemInfoListPtr it=pObjList->begin(); it!=_end; ++it)
		{
			stMapItemInfo& info = *it;
			if (info.pItem && info.pItem != m_pPlayerItem && info.pItem->GetVisibleOnMinmap())
			{
				TileToWorld(info.ptTile, ptCenter);
				DWORD dwCID = info.pItem->GetClassID();
				wpMinMapFlagColor = WPixel(info.pItem->GetFlagColorOnMinmap());
				if(m_MiniMap.GetMiniMapBigOrSmall())
					m_MiniMap.DrawObjByClassID(m_pMiniCanvasBig, ptCenter.x, ptCenter.y, info.pItem->GetOccupantValue(), dwCID, wpMinMapFlagColor);
				else
					m_MiniMap.DrawObjByClassID(m_pMiniCanvas, ptCenter.x, ptCenter.y, info.pItem->GetOccupantValue(), dwCID, wpMinMapFlagColor);
			}
		}

		// 最后绘制主角
		wpMinMapFlagColor = WPixel(0, 255, 0);
		if(m_pPlayerItem != NULL)
		{
			wpMinMapFlagColor = WPixel(m_pPlayerItem->GetFlagColorOnMinmap());
		}
		if(m_MiniMap.GetMiniMapBigOrSmall())
		{
			m_MiniMap.DrawMainActor(m_pMiniCanvasBig, wpMinMapFlagColor);
			m_pMengBmpBig->Draw(m_pMiniCanvasBig, -5, -23);
			pCanvas->Draw(MINIMAP_BIG_LEFT_IN_SCREEN, MINIMAP_BIG_TOP_IN_SCREEN, m_pMiniCanvasBig, BLIT_MASK);
			m_pWngBmpBig->Draw(pCanvas, 591+204, 0);
		}
		else
		{
			m_MiniMap.DrawMainActor(m_pMiniCanvas, wpMinMapFlagColor);

			m_pMengBmp->Draw(m_pMiniCanvas, -63, -22);
			pCanvas->Draw(MINIMAP_SMALL_LEFT_IN_SCREEN, MINIMAP_SMALL_TOP_IN_SCREEN, m_pMiniCanvas, BLIT_MASK);
			m_pWngBmp->Draw(pCanvas, 613+204, 0);
		}
		
		if (pFont)
		{
			// 绘制地图坐标在左上角
			if (m_bMouseInMiniMap)
			{
				POINT ptMouse;
				m_MiniMap.CalcMousePos(m_ptMousePos, ptMouse);
				int x = ptMouse.x >> 6;
				int y = ptMouse.y >> 5;
				if(x < 0)
					x = 0;
				if(y < 0)
					y = 0;
				char buf[MAX_PATH];
				wsprintf(buf, "X:%3d", x);
				pCanvas->DrawText(pFont, 728+204, 4, buf, WPixel(255,255,148));
				wsprintf(buf, "Y:%3d", y);
				pCanvas->DrawText(pFont, 766+204, 4, buf, WPixel(255,255,148));
			}
			else
			{
				if (m_pPlayerItem)
				{
					POINT ptPlayerPos;
					ptPlayerPos = m_pPlayerItem->m_ptWold;
					int x = ptPlayerPos.x >> 6;
					int y = ptPlayerPos.y >> 5;
					char buf[MAX_PATH];
					wsprintf(buf, "X:%3d", x);
					pCanvas->DrawText(pFont, 728+204, 4, buf, WPixel(255,255,148));
					wsprintf(buf, "Y:%3d", y);
					pCanvas->DrawText(pFont, 766+204, 4, buf, WPixel(255,255,148));
				}
			}

			// 绘制地图名称
			if (m_szMapName[0])
			{
				pCanvas->DrawText(pFont, 665+204, 4, m_szMapName, WPixel(255,255,148));
			}
		}
	}
}
//------------------------------------------------------------------------
// 设置小地图是否可见
void CMapView::SetMiniMapVisible(BOOL bVisible)
{
	m_MiniMap.SetVisible(bVisible);
}
//------------------------------------------------------------------------
// 设置小地图的视图矩形
void CMapView::SetMiniMapViewport(LPRECT lprcViewport)
{
	m_MiniMap.SetViewport(lprcViewport);
}
//------------------------------------------------------------------------
void CMapView::SetMiniMapTransparent(int nAlpha)
{
	m_MiniMap.SetMiniMapTransparent(nAlpha);
}
//------------------------------------------------------------------------
BOOL CMapView::GetTinyMapVisible()
{
	return TRUE;
}
//------------------------------------------------------------------------
void CMapView::SetMiniMapBigOrSmall(BOOL bBig)
{
	m_MiniMap.SetMiniMapBigOrSmall(bBig);
}
//------------------------------------------------------------------------
// 改变地图天气
void CMapView::ChangeWeather(SWeatherInfo info)
{
	m_Weather.ChangeWeather(info);
}
//------------------------------------------------------------------------
void CMapView::SetTinyMapVisible(BOOL bVisible)
{
#ifdef _DRAWOCCUPANT
	if (bVisible)
	{
		if (m_dwDrawFlag & maskDrawOccupant)
			m_dwDrawFlag &= ~maskDrawOccupant;
		else
			m_dwDrawFlag |= maskDrawOccupant;
	}

#endif
}
//------------------------------------------------------------------------
BOOL CMapView::QuickFillOccupant(POINT& ptTileInRect, int nGridWidth, int nGridHeight, int nTileRows, int nTileCols)
{
	POINT ptSrc;
	m_MapCo.TileToPixel(ptTileInRect, ptSrc);
	
	// 校正
	int nGridX = ptSrc.x / nGridWidth;
	int nGridY = ptSrc.y / nGridHeight;
	ptSrc.x = nGridX * nGridWidth;	
	ptSrc.y = nGridY * nGridHeight;
	if (nGridY & 1)	ptSrc.y += 16;
	POINT ptTileLeftTop;
	m_MapCo.PixelToTile(ptSrc, ptTileLeftTop);
	m_MapCo.TileToPixel(ptTileLeftTop, ptSrc);
	int nIndex = m_GroundView.GetIndexByPoint(ptSrc);
	TraceLn("First fill index = %d", nIndex);

	int nTotalGridX = PixelCood2GridCood(GetMapWidth(), nGridWidth);
	int nTotalGridY = PixelCood2GridCood(GetMapHeight(), nGridHeight);

	int nCount = 0;
	POINT pt = {0, 0};
	int nPitchY = 0;
	for (int row=0; row<nTotalGridY; row++,pt.y+=nGridHeight) // each row
	{
		pt.x = 0;
		if (row & 1) nPitchY = 16;
		else nPitchY = 0;
		for (int col=0; col<nTotalGridX; col++,pt.x+=nGridWidth)
		{
			POINT ptTemp = pt;
			ptTemp.y += nPitchY;
			if (m_GroundView.GetIndexByPoint(ptTemp) == nIndex)
			{
				nCount ++;
				TraceLn("Fill grid(%d,%d)", pt.x/nGridWidth, pt.y/nGridHeight);
				m_Map.CopyBlockInfoForSurface(ptSrc, ptTemp, nTileRows, nTileCols*2);
			}
		}
	}
	return nCount;
}
//------------------------------------------------------------------------
// 应用程序在一个循环中发现还剩余有时间时，可以调用这个方法来让地图预加载某些图片资源
void CMapView::OnHandleIdle(DWORD dwParam)
{
}
//------------------------------------------------------------------------
BOOL CMapView::FindPath(POINTS ptFrom, POINTS ptTo, POINTS** ppBuffer, int& nPathLen)
{
	return m_Map.FindPath(ptFrom, ptTo, ppBuffer, nPathLen);
}
//------------------------------------------------------------------------
BOOL CMapView::FindSimplePath(POINTS ptFrom, POINTS ptTo, POINTS** ppBuffer, int& nPathLen)
{
	return m_Map.FindSimplePath(ptFrom, ptTo, ppBuffer, nPathLen);
}
//------------------------------------------------------------------------
BOOL CMapView::IsIntersect(CMapItemView& item1, CMapItemView& item2, RECT& rc)
{
	POINT ptTileWorld;
	ptTileWorld = item1.m_ptWold;
	RECT rc1 = item1.GetShowRect();
	OffsetRect(&rc1, ptTileWorld.x, ptTileWorld.y);
	
	ptTileWorld = item2.m_ptWold;
	RECT rc2 = item2.GetShowRect();
	OffsetRect(&rc2, ptTileWorld.x, ptTileWorld.y);
	return IntersectRect(&rc, &rc1, &rc2);
}
//------------------------------------------------------------------------
int CMapView::CmpPointPoint(CMapItemView& item1, CMapItemView& item2)
{
	RECT rc;
	if (IsIntersect(item1, item2, rc))
	{
		if (item1.m_ptLeft.y == item2.m_ptLeft.y)
			return CMP_EQ;
		return (item1.m_ptLeft.y < item2.m_ptLeft.y) ? CMP_LT : CMP_GT;
	}
	return CMP_EQ;
}
//------------------------------------------------------------------------
int CMapView::CmpPointLine(CMapItemView& item1, CMapItemView& item2)
{
	RECT rc;
	if (IsIntersect(item1, item2, rc))
	{
		if (item2.m_ptLeft.y == item2.m_ptRight.y) // 水平线
			return (item1.m_ptLeft.y < item2.m_ptLeft.y) ? CMP_LT : CMP_GT;
		int Ycross = (item1.m_ptLeft.x - item2.m_ptLeft.x) * (item2.m_ptRight.y - item2.m_ptLeft.y )
					/ (item2.m_ptRight.x - item2.m_ptLeft.x) + item2.m_ptLeft.y;
		if ((Ycross > item1.m_ptLeft.y) || 
			(Ycross == item1.m_ptLeft.y && Ycross <= item2.m_ptLeft.y && Ycross <= item2.m_ptRight.y)) 
			return CMP_LT;
		return CMP_GT;
	}
	return CMP_EQ;
}
//------------------------------------------------------------------------
int CMapView::CmpLineLine(CMapItemView& item1, CMapItemView& item2)
{
	RECT rc;
	if (IsIntersect(item1, item2, rc))
	{
		int Xcross = (rc.left + rc.right ) /2; // 取矩形交集的中心线作为比较的垂直线
		int Ycross1 = (Xcross - item1.m_ptLeft.x) * (item1.m_ptRight.y - item1.m_ptLeft.y ) 
			/ (item1.m_ptRight.x - item1.m_ptLeft.x) + item1.m_ptLeft.y;
		int Ycross2 = (Xcross - item2.m_ptLeft.x) * (item2.m_ptRight.y - item2.m_ptLeft.y ) 
			/ (item2.m_ptRight.x - item2.m_ptLeft.x) + item2.m_ptLeft.y;
		if (Ycross1 == Ycross2) // 如在一条TILE直线上的城墙可满足这个条件
		{
			int dy_center = (item1.m_ptLeft.y + item1.m_ptRight.y) - (item2.m_ptLeft.y + item2.m_ptRight.y);
			return (dy_center < 0) ? CMP_LT : CMP_GT;
		}
		return (Ycross1 < Ycross2) ? CMP_LT : CMP_GT;
	}
	return CMP_EQ;
}
//------------------------------------------------------------------------
BOOL CMapView::Compare(CMapItemView& item1, CMapItemView& item2)
{
	int nL1 = item1.GetLayerValue();
	int nL2 = item2.GetLayerValue();
	if (nL1 != nL2)
		return (nL1 < nL2) ? CMP_LT : CMP_GT;
	
	int nRtn = 0;
	if (item1.m_ptLeft.x == item1.m_ptRight.x && item1.m_ptLeft.y == item1.m_ptRight.y) // this is point
	{
		if (item2.m_ptLeft.x == item2.m_ptRight.x && item2.m_ptLeft.y == item2.m_ptRight.y) // item is point
			nRtn = CmpPointPoint(item1, item2);
		else
			nRtn = CmpPointLine(item1, item2);
	}
	else
	{
		if (item2.m_ptLeft.x == item2.m_ptRight.x && item2.m_ptLeft.y == item2.m_ptRight.y) // item is point
			nRtn = -CmpPointLine(item2, item1);
		else
			nRtn = CmpLineLine(item1, item2);
	}
	
	return nRtn;
}
//------------------------------------------------------------------------
// 先前的排序算法
void CMapView::AddSortItem0(CMapItemView* pItem, DisplayList& _DisplayList)
{
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;
	POINT ptTileCenter;
	ptTileCenter = pItem->m_ptWold;
	
	BOOL bFound = FALSE;
	DisplayListPtr it = _DisplayList.begin();
	DisplayListPtr _end = _DisplayList.end();
	
	POINT ptTempTileCenter;
	while (it != _end)
	{
		ptTempTileCenter = (*it).pItem->m_ptWold;
		if (ptTileCenter.y < ptTempTileCenter.y || 
			(ptTileCenter.y == ptTempTileCenter.y && ptTileCenter.x < ptTempTileCenter.x))
		{
			bFound = TRUE;
			_DisplayList.insert(it, ptr);
			break;
		}
		else ++it;
	}
	if (!bFound)
		_DisplayList.push_back(ptr);
}
//------------------------------------------------------------------------
void CMapView::UpdateSortItem0(CMapItemView* pItem, DisplayList& _DisplayList)
{
}
//------------------------------------------------------------------------
// 先前的排序算法
void CMapView::AddSortItem1(CMapItemView* pItem, DisplayList& _DisplayList)
{
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;

	BOOL bFound = FALSE;
	DisplayListPtr it = _DisplayList.begin();
	DisplayListPtr _end = _DisplayList.end();

	while (it != _end)
	{
		if (Compare(*ptr.pItem, *(*it).pItem) == CMP_LT)
		{
			bFound = TRUE;
			_DisplayList.insert(it, ptr);
			break;
		}
		else ++it;
	}
	if (!bFound)
		_DisplayList.push_back(ptr);
}
//------------------------------------------------------------------------
void CMapView::UpdateSortItem1(CMapItemView* pItem, DisplayList& _DisplayList)
{
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;
	_DisplayList.remove(ptr);
	AddSortItem1(pItem, _DisplayList);
}
//------------------------------------------------------------------------
// 现在的排序算法
void CMapView::AddSortItem2(CMapItemView* pItem, DisplayList& _DisplayList)
{
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;

	BOOL bFound = FALSE;
	DisplayListPtr _begin = _DisplayList.begin();
	DisplayListPtr _end = _DisplayList.end();
	DisplayListPtr it_line1 = _begin;
	DisplayListPtr it_line2 = _begin;
	DisplayListPtr it = _begin;
	
	while (it != _end)
	{
		CMapItemViewPtr& p = (*it);
		if (!p.IsPoint())
		{
			if (Compare(*ptr.pItem, *p.pItem) == CMP_LT)
			{
				it_line2 = it;
				break;
			}
			else
			{
				it_line1 = it_line2 = it;
				++it;
			}
		}
		else ++it;
	}
	
	if (it_line1 == it_line2) // 没有直线或者有直线但是最后一根直线，插入在此直线的后面
		it_line2 = _end;
	it = it_line1;
	while (it != it_line2)
	{
		if (Compare(*ptr.pItem, *(*it).pItem) == CMP_LT)
		{
			bFound = TRUE;
			_DisplayList.insert(it, ptr);
			break;
		}
		else ++it;
	}
	if (!bFound)
		_DisplayList.insert(it_line2, ptr);
}
//------------------------------------------------------------------------
void CMapView::UpdateSortItem2(CMapItemView* pItem, DisplayList& _DisplayList)
{
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;
	
	BOOL bFound = FALSE;
	int	 nDist = 0;
	DisplayListPtr _begin = _DisplayList.begin();
	DisplayListPtr _end = _DisplayList.end();
	DisplayListPtr it = _begin;
	for (; it!=_end; ++it)
	{
		if ((*it).pItem == ptr.pItem)
		{
			nDist = distance(_begin, it);
			_DisplayList.erase(it);
			break;
		}
	}
	
	_begin = _DisplayList.begin();
	_end = _DisplayList.end();
	it = _begin;
	DisplayListPtr it_line1 = _begin;
	DisplayListPtr it_line2 = _begin;
	
	if (nDist > 0)
		advance(it, nDist-1);
	if (it == _end)
		_DisplayList.push_back(ptr);
	else
	{
		int nCmp = Compare(*ptr.pItem, *(*it).pItem);
		if (nCmp == CMP_LT) // 朝前面找
		{
			for (--it; it != _end; --it)
			{
				nCmp = Compare(*ptr.pItem, *(*it).pItem);
				if (nCmp == CMP_LT)
				{
					bFound = TRUE;
					_DisplayList.insert(it, ptr);
					break;
				}
				else if (nCmp == CMP_GT)
				{
					bFound = TRUE;
					_DisplayList.insert(++it, ptr);
					break;
				}
			}
			if (!bFound)
				_DisplayList.push_front(ptr);
		}
		else //if (nCmp == CMP_GT) // 朝后面找
		{
			for (++it; it!=_end; ++it)
			{
				if (Compare(*ptr.pItem, *(*it).pItem) == CMP_LT)
				{
					bFound = TRUE;
					_DisplayList.insert(it, ptr);
					break;
				}
			}
			if (!bFound)
				_DisplayList.push_back(ptr);
		}
	}
}
//------------------------------------------------------------------------
void CMapView::AddSortItemPoint(CMapItemView* pItem, DisplayList& _DisplayList)
{
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;
	
	BOOL bFound = FALSE;
	DisplayListPtr it = _DisplayList.begin();
	DisplayListPtr _end = _DisplayList.end();
	DisplayListPtr it_ptr = it;
	DisplayListPtr it_tmp;
	
	CMapItemView* ppItem = ptr.pItem;
	while (it != _end)
	{
		if(ptr.pItem->m_ptLeft.y < (*it).pItem->m_ptLeft.y
			|| (ptr.pItem->m_ptLeft.y == (*it).pItem->m_ptLeft.y 
				&& ptr.pItem->m_ptLeft.x <= (*it).pItem->m_ptLeft.x) )
		{
			bFound = TRUE;
			if (it == _DisplayList.begin())
			{
				_DisplayList.push_front(ptr);
				it = _DisplayList.begin();
				++it;
			}
			else
			{
				_DisplayList.insert(it, ptr);
			}
			it_ptr = it;
			--it_ptr;
			break;
		}
		else ++it;
	}
	if (!bFound)
		_DisplayList.push_back(ptr);
}
//------------------------------------------------------------------------
void CMapView::AddSortItem3(CMapItemView* pItem, DisplayList& _DisplayList)
{
	DWORD dwStart = GetTickCount();
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;
	
	BOOL bFound = FALSE;
	DisplayListPtr it = _DisplayList.begin();
	DisplayListPtr _end = _DisplayList.end();
	DisplayListPtr it_ptr = it;
	DisplayListPtr it_tmp;
	
	while (it != _end)
	{
		if (Compare(*ptr.pItem, *(*it).pItem) == CMP_LT)
		{
			bFound = TRUE;
			if (it == _DisplayList.begin())
			{
				_DisplayList.push_front(ptr);
				it = _DisplayList.begin();
				++it;
			}
			else
			{
				_DisplayList.insert(it, ptr);
			}
			it_ptr = it;
			--it_ptr;
			break;
		}
		else ++it;
	}
	if (!bFound)
		_DisplayList.push_back(ptr);

	if (bFound) // 校验
	{
		// 如果插入点的后面仍然有对象小于插入的对象，则依次移到插入点的前面
		_end = _DisplayList.end();
		int nErrNum = 0;
		for (++it; it!=_end; ++it)
		{
			if (Compare(*ptr.pItem, *(*it).pItem) == CMP_GT)
			{
				nErrNum++;
				it_tmp = it;
				--it_tmp;
				_DisplayList.insert(it_ptr, *it);
				_DisplayList.erase(it);
				it = it_tmp;
				_end = _DisplayList.end();
			}
		}

		DWORD dw = GetTickCount() - dwStart;
		if (dw > 20)
		{
			TraceLn("排序时间过长 = %d ms, 错误节点个数 = %d 个, 视图绘制对象个数 = %d个", dw, nErrNum, _DisplayList.size());
		}
	}
}
//------------------------------------------------------------------------
void CMapView::AddSortItem31(CMapItemView* pItem, DisplayList& _DisplayList)
{
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;
	
	BOOL bFound = FALSE;
	DisplayListPtr _begin = _DisplayList.begin();
	DisplayListPtr _end = _DisplayList.end();
	DisplayListPtr it_line1 = _begin;
	DisplayListPtr it_line2 = _begin;
	DisplayListPtr it = _begin;
	
	while (it != _end)
	{
		CMapItemViewPtr& p = (*it);
		if (!p.IsPoint()) // line
		{
			if (Compare(*ptr.pItem, *p.pItem) == CMP_LT)
			{
				it_line2 = it;
				break;
			}
			else
			{
				it_line1 = it_line2 = it;
				++it;
			}
		}
		else ++it;
	}
	
	if (it_line1 == it_line2) // 没有直线或者有直线但是最后一根直线，插入在此直线的后面
		it_line2 = _end;
	it = it_line1;
	while (it != it_line2)
	{
		if (Compare(*ptr.pItem, *(*it).pItem) == CMP_LT)
		{
			bFound = TRUE;
			_DisplayList.insert(it, ptr);
			break;
		}
		else ++it;
	}
	if (!bFound)
		_DisplayList.insert(it_line2, ptr);
}
//------------------------------------------------------------------------
void CMapView::UpdateSortItem3(CMapItemView* pItem, DisplayList& _DisplayList)
{
	CMapItemViewPtr ptr;
	ptr.pItem = pItem;
	_DisplayList.remove(ptr);
	AddSortItem3(pItem, _DisplayList);
}
//------------------------------------------------------------------------
void CMapView::AddPointToList()
{
	m_VisableListObj.clear();
	DisplayListPtr it = m_VisableListMultiOcc.begin();
	DisplayListPtr _end = m_VisableListMultiOcc.end();
	DisplayListPtr it_tmp;
	for (; it!=_end; ++it)
	{
		AddSortItem3((*it).pItem, m_VisableListObj);
	}
}
//------------------------------------------------------------------------
int CMapView::OnSomeHandle(DWORD dwParam1, DWORD dwParam2)
{
	if (dwParam1 == 0)
	{
		m_ptMousePos = *(POINT*)dwParam2;
		if(m_ptMousePos.x < 0)
			m_ptMousePos.x = 0;
		if(m_ptMousePos.y < 0)
			m_ptMousePos.y = 0;
	}
	else
		m_bMouseInMiniMap = (BOOL)dwParam2;
	return 0;
}
//------------------------------------------------------------------------
// 根据一个地图文件数据创建地图视图(采用动态地图，适合游戏使用)
IMapView* LoadMapView(DWORD dwVer, 
					  IFileObject* pMapFile, IFileObject* pGridIdxArrayFile, IFileObject* pTinyMapFile,
					  LPCSTR szGroundJpgFileDir, LPCSTR szMapName, LPRECT lprcViewport, 
					  LPRECT lprcMiniMapViewport, // 地图和小地图的视区
					  LPCSTR szMiniMapJpgFileDir, BOOL bUseDynamicMap, int nMiniMapRate, 
					  IItemClassFactory* pItemCF, ITrace* pTrace)
{
	g_DefaultTrace.Create(pTrace);
	if (dwVer > VERSION_MAPVIEW)
	{
		return FALSE;
	}

	CMapView* pMapView = new CMapView();
	if (!pMapView)
		return NULL;

	if (!pMapView->LoadMapView(pMapFile, pGridIdxArrayFile, pTinyMapFile,
		szGroundJpgFileDir, szMapName, lprcViewport, 
		lprcMiniMapViewport, szMiniMapJpgFileDir, bUseDynamicMap, nMiniMapRate, 
		pItemCF, pTrace))
	{
		delete pMapView;
		return NULL;
	}

	return static_cast<IMapView*>(pMapView);
}
//------------------------------------------------------------------------
IMapView* CreateNewMapView(DWORD dwVer, int nMapWidth, int nMapHeight,
				   LPRECT lprcViewport, LPRECT lprcMiniMapViewport,
				   LPCSTR szGroundJpgFileDir, IFileObject* pGridIdxArrayFile, ITrace* pTrace)
{
	g_DefaultTrace.Create(pTrace);
	if (dwVer > VERSION_MAPVIEW)
	{
		TraceLn("Error : CreateNewMapView() : dwVer = %d,%d wanted", dwVer, VERSION_MAPVIEW);
		return FALSE;
	}

	CMapView* pMapView = new CMapView();
	if (!pMapView)
		return NULL;

	if (!pMapView->CreateNewMap(nMapWidth, nMapHeight, 
		lprcViewport, lprcMiniMapViewport, 
		szGroundJpgFileDir, pGridIdxArrayFile, pTrace))
	{
		delete pMapView;
		return NULL;
	}
	
	return static_cast<IMapView*>(pMapView);
}
//------------------------------------------------------------------------