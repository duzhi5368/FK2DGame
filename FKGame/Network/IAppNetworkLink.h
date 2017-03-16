/**
*	created:		2012-6-22   0:25
*	filename: 		IAppNetworkLink
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IClientSocket.h"
#include "../Common/Macro.h"
//------------------------------------------------------------------------
enum 
{
	NETWORKLINKTYPE_LOGIN = 0,
	NETWORKLINKTYPE_MAPCHANGING, 
	NETWORKLINKTYPE_SELACTOR, 
	NETWORKLINKTYPE_SCENE, 	
	NETWORKLINKTYPE_RUNNING, 
	NETWORKLINKTYPE_MAX, 
};
//------------------------------------------------------------------------
struct IAppNetworkLink : public IClientLinkSink
{
public:
	// 发送数据包
	virtual int				Send(LPCSTR buf, int nLen) = NULL;
	// 取得IClientLink指针, 使用方法更丰富
	virtual IClientLink *	GetClientLink(void) = NULL;
	// 返回当前连接到了哪一个状态
	virtual int GetType(void) = NULL;
};
//------------------------------------------------------------------------