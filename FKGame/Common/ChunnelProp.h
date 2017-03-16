/**
*	created:		2012-7-2   22:00
*	filename: 		ChunnelProp
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		地图通道属性定义
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#pragma pack(1)
//------------------------------------------------------------------------
enum 
{
	CHUNNEL_TRANS_MODE_POINTING = 1,				// 1  = 单向传送
	CHUNNEL_TRANS_MODE_RAND,						// 2  = 随机传送
	CHUNNEL_TRANS_MODE_LIST,						// 3  = 列表传送
	CHUNNEL_TRANS_MODE_ECTYPE,						// 4  = 副本传送
};
//------------------------------------------------------------------------
#define CHUNNEL_TARGET_MAXCOUNT						10
//------------------------------------------------------------------------
// 传送目标点信息
struct SCHUNNEL_TARGET
{
	DWORD			dwTragetMapID;					// 目标地图ID
	POINTS			ptTragetPos;					// 目标位置
	DWORD			dwRadius;						// 随机半径
};
//------------------------------------------------------------------------
// 属性列表
struct SCHUNNEL_PROPERTY_LIST
{
	char			szName[MAX_NAME_LEN];			// 1  = 名字
	POINTS			ptLoc;							// 2  = 位置 
	DWORD			dwResID;						// 3  = 资源ID	
	DWORD			dwTransMode;					// 4  = 传送模式（1：单向传送，2 ：随机传送，3：列表传送）
	DWORD			dwTargetCount;					// 5  = 目标点个数

	SCHUNNEL_TARGET TargetInfor[CHUNNEL_TARGET_MAXCOUNT];
};
//------------------------------------------------------------------------
#pragma pack()
//------------------------------------------------------------------------