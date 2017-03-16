/**
*	created:		2012-6-25   23:58
*	filename: 		IClientWorld
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 申明
struct IThingTable;
//------------------------------------------------------------------------
// 定义
struct IClientWorld
{
	// 关闭
	virtual void			Close(void) = NULL;
	// 消息
	virtual void			OnMessage(DWORD dwKeyMain, LPCSTR buf, int nLen) = NULL;
	// 取得逻辑对象管理器
	virtual IThingTable *	GetThingTable(void) = NULL;
	// 取得当前静态地图ID
	virtual int				GetCurStaticMapID(void) = NULL;
};
//------------------------------------------------------------------------