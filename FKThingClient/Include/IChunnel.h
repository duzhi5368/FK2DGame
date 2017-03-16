/**
*	created:		2012-7-2   21:58
*	filename: 		IChunnel
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		地图通道
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IMapThing.h"
#include "../../FKGame/Common/ChunnelProp.h"
//------------------------------------------------------------------------
struct IChunnel : public IMapThing
{
	// 取得名字
	virtual LPCSTR				GetName(void) = NULL;

	// 取得传送方式
	virtual DWORD				GetTransMode(void) = NULL;

	// 取得目标点信息
	virtual SCHUNNEL_TARGET *	GetTargetInfor(DWORD &dwCount) = NULL;
};
//------------------------------------------------------------------------