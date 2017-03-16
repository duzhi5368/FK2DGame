/**
*	created:		2012-6-30   23:33
*	filename: 		CConfigMapInfor
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		地图信息配置
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Script/IScriptUpdate.h"
#include "../Common/IConfigDataCenter.h"
//------------------------------------------------------------------------
// 地图信息配置脚本名字
#ifdef _DEBUG
#define MAPINFOR_CONFIG_SCRIPTNAME	"Data\\Scp\\mapinfor.scp"
#else
#define MAPINFOR_CONFIG_SCRIPTNAME	"Data\\Scp\\mapinfor.res"
#endif
//------------------------------------------------------------------------
class CConfigMapInfor : public IScriptUpdateSink
{
	typedef vector<MAP_INFOR>	TVECTOR_MAPINFOR;
public:
	// 脚本载入成功时通知,注意it有可能为空
	virtual BOOL	OnScriptLoad(IScriptIterator * it, IResScript * pResScript, LPCSTR szFile);
	
	// 脚本动态更新时通知,注意it有可能为空
	virtual BOOL	OnScriptUpdate(IScriptIterator * it, IResScript * pResScript, LPCSTR szFile);
	
	// 创建
	BOOL			Create(void);

	// 取得所有地图的信息, IN : pInfor 可容纳的MAP_INFOR个数, OUT : 当前有多少张地图
	BOOL			GetMapInfor(MAP_INFOR * pInfor, DWORD &dwCount);
	
	// 取得某一张地图的信息
	MAP_INFOR *		GetMapInfor(DWORD dwMapID);
	
	// 取得当前地图数量
	DWORD			GetMapCount(void);

private:
	TVECTOR_MAPINFOR	m_vectorMapInfor;
};
//------------------------------------------------------------------------