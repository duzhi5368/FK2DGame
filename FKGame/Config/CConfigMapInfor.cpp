/**
*	created:		2012-6-30   23:37
*	filename: 		CConfigMapInfor
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		地图信息配置
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "CConfigMapInfor.h"
#include "../GameGlobal/ClientGlobal.h"
//------------------------------------------------------------------------
BOOL CConfigMapInfor::Create(void)
{	
	IScriptUpdateManager * pScriptUpdateManager = g_ClientGlobal.m_pScriptUpdateManager;
	if(pScriptUpdateManager == NULL)
	{
		return FALSE;
	}
	
	if(!pScriptUpdateManager->LoadScript(MAPINFOR_CONFIG_SCRIPTNAME, static_cast<IScriptUpdateSink *>(this)))
	{
		TraceLn("加载%s脚本失败！", MAPINFOR_CONFIG_SCRIPTNAME);		
		return FALSE;
	}
	
	return TRUE;	
}
//------------------------------------------------------------------------
// 取得所有地图的信息, IN : pInfor 可容纳的MAP_INFOR个数, OUT : 当前有多少张地图
BOOL CConfigMapInfor::GetMapInfor(MAP_INFOR * pInfor, DWORD &dwCount)
{
	if(pInfor == NULL || dwCount <= 0)
	{
		return FALSE;
	}

	int nMaxCount = dwCount;
	dwCount = 0;
	TVECTOR_MAPINFOR::iterator it = m_vectorMapInfor.begin();
	for( ; it != m_vectorMapInfor.end(); ++it)
	{
		pInfor[dwCount++] = (*it);
		if(dwCount >= nMaxCount)
		{
			return FALSE;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
// 取得某一张地图的信息
MAP_INFOR *	CConfigMapInfor::GetMapInfor(DWORD dwMapID)
{
	TVECTOR_MAPINFOR::iterator it = m_vectorMapInfor.begin();
	for( ; it != m_vectorMapInfor.end(); ++it)
	{
		if((*it).nmapid == dwMapID)
		{
			return (&(*it));
		}
	}

	return NULL;
}
//------------------------------------------------------------------------
// 取得当前地图数量
DWORD CConfigMapInfor::GetMapCount(void)
{
	return m_vectorMapInfor.size();
}
//------------------------------------------------------------------------
// 脚本载入成功时通知,注意it有可能为空
BOOL CConfigMapInfor::OnScriptLoad(IScriptIterator * it, IResScript * pResScript, LPCSTR szFile)
{
	if(pResScript == NULL)
	{
		return FALSE;
	}

	IResObject * pRootObject = pResScript->GetResObject(1);
	if(pRootObject == NULL)
	{	
		return FALSE;
	}
	
	int nCount = pRootObject->GetSubObjectCount();	
	for(int i = 0; i < nCount; i++)
	{
		IResObject * pObject = pRootObject->GetSubObjectByIndex(i);
		if(pObject == NULL)
		{
			continue;
		}

		MAP_INFOR mapinfor;
		ZeroMemory(&mapinfor, sizeof(MAP_INFOR));
		pObject->GetPropertyLong("applyserver", mapinfor.napplyserver, 1);
		pObject->GetPropertyLong("open", mapinfor.nopen, 1);
		pObject->GetPropertyLong("mapid", mapinfor.nmapid, INVALID_MAPID);
		pObject->GetPropertyLong("usedynamic", mapinfor.lusedynamic, 1);
		pObject->GetPropertyLong("minimaprate", mapinfor.lminimaprate, 20);

		lstrcpyn(mapinfor.szmapname, pObject->GetPropertyString("mapname"), sizeof(mapinfor.szmapname));
		lstrcpyn(mapinfor.szmapfile, pObject->GetPropertyString("mapfile"), sizeof(mapinfor.szmapfile));
		lstrcpyn(mapinfor.szmaifile, pObject->GetPropertyString("maifile"), sizeof(mapinfor.szmaifile));
		lstrcpyn(mapinfor.szminimapfile, pObject->GetPropertyString("minimapfile"), sizeof(mapinfor.szminimapfile));
		lstrcpyn(mapinfor.szworldmapfile, pObject->GetPropertyString("worldmapfile"), sizeof(mapinfor.szworldmapfile));

		pObject->GetArrayLong("staticrelivepoint", mapinfor.lstaticrelivepoint, 3);
		pObject->GetArrayLong("dynamicrelivepoint", mapinfor.ldynamicrelivepoint, 3);
		pObject->GetArrayLong("redstaticrelivepoint", mapinfor.lredstaticrelivepoint, 3);
		pObject->GetArrayLong("reddynamicrelivepoint", mapinfor.lreddynamicrelivepoint, 3);
		pObject->GetArrayLong("landfallpoint", mapinfor.llandfallpoint, 2);

		lstrcpyn(mapinfor.szmonstercreaterfile, pObject->GetPropertyString("monstercreater"), sizeof(mapinfor.szmonstercreaterfile));
		lstrcpyn(mapinfor.szchunnelcreaterfile, pObject->GetPropertyString("chunnelcreater"), sizeof(mapinfor.szchunnelcreaterfile));
		lstrcpyn(mapinfor.szboxcreaterfile,		pObject->GetPropertyString("boxcreater"),	  sizeof(mapinfor.szboxcreaterfile));

		pObject->GetPropertyLong("backgroundmusic", mapinfor.lbackgroundmusic, 0);

		// 地图大小
		long lMapSize[2];
		pObject->GetArrayLong("mapsizetile", lMapSize, 2);
		mapinfor.lmapsizetileX = lMapSize[0];
		mapinfor.lmapsizetileY = lMapSize[1];

		m_vectorMapInfor.push_back(mapinfor);
	}	
	
	return TRUE;
}
//------------------------------------------------------------------------
// 脚本动态更新时通知,注意it有可能为空
BOOL CConfigMapInfor::OnScriptUpdate(IScriptIterator * it, IResScript * pResScript, LPCSTR szFile)
{
	return FALSE;	
}
//------------------------------------------------------------------------