/**
*	created:		2012-7-1   6:11
*	filename: 		MapDllApp
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/MapDllApp.h"
#include "../Include/MapView.h"
//------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
//------------------------------------------------------------------------
extern "C" __declspec(dllexport) 
IMapView* dllCreateNewMapView(DWORD dwVer,
							  int nMapWidth, int nMapHeight,
							  LPRECT lprcViewport, LPRECT lprcMiniMapViewport,
							  LPCSTR szGroundJpgFileDir, IFileObject* pGridIdxArrayFile, ITrace* pTrace)
{
	return CreateNewMapView(dwVer, nMapWidth, nMapHeight,
		lprcViewport, lprcMiniMapViewport,
		szGroundJpgFileDir, pGridIdxArrayFile, pTrace);
}

//------------------------------------------------------------------------
extern "C" __declspec(dllexport) 
IMapView* dllLoadMapView(DWORD dwVer, IFileObject* pMapFile, IFileObject* pGridIdxArrayFile, IFileObject* pTinyMapFile,
						 LPCSTR szGroundJpgFileDir, LPCSTR szMapName, LPRECT lprcViewport, 
						 LPRECT lprcMiniMapViewport, LPCSTR szMiniMapJpgFileDir, BOOL bUseDynamicMap, int nMiniMapRate, 
						 IItemClassFactory *pItemCF, ITrace *pTrace)
{
	return LoadMapView(dwVer, pMapFile, pGridIdxArrayFile, pTinyMapFile,
		szGroundJpgFileDir, szMapName, lprcViewport, 
		lprcMiniMapViewport, szMiniMapJpgFileDir, bUseDynamicMap, nMiniMapRate, 
		pItemCF, pTrace);
}
//------------------------------------------------------------------------