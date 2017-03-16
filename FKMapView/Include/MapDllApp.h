/**
*	created:		2012-7-1   6:10
*	filename: 		MapDllApp
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
#ifdef FKMAPDLLAPP_EXPORTS
#define MAPDLLAPP_API __declspec(dllexport)
#else
#define MAPDLLAPP_API __declspec(dllimport)
#endif
//------------------------------------------------------------------------
struct ICanvas;
struct IArchive;
struct ITrace;
struct IMapView;
struct IItemClassFactory;
struct IFileObject;
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) 
IMapView* dllCreateNewMapView(DWORD dwVer,
							  int nMapWidth, int nMapHeight,
							  LPRECT lprcViewport, LPRECT lprcMiniMapViewport,
							  LPCSTR szGroundJpgFileDir, IFileObject* pGridIdxArrayFile, ITrace* pTrace);
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) 
IMapView* dllLoadMapView(DWORD dwVer, IFileObject* pMapFile, IFileObject* pGridIdxArrayFile, IFileObject* pTinyMapFile,
						 LPCSTR szGroundJpgFileDir, LPCSTR szMapName, LPRECT lprcViewport, 
						 LPRECT lprcMiniMapViewport, LPCSTR szMiniMapJpgFileDir, BOOL bUseDynamicMap, int nMiniMapRate, 
						 IItemClassFactory *pItemCF, ITrace *pTrace);
//------------------------------------------------------------------------