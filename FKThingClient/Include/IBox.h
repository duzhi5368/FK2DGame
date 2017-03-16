/**
*	created:		2012-7-2   21:56
*	filename: 		IBox
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		盒子
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IMapThing.h"
//------------------------------------------------------------------------
struct ICreatureActState;
class CMapItemView;
//------------------------------------------------------------------------
struct IBox : public IMapThing
{
	// 得到名字
	virtual LPCSTR GetName() const = 0;

	// 向服务端请求
	// 打开盒子
	virtual BOOL Open() = 0;

	// 关闭盒子
	virtual void Close() = 0;

	// 服务端的响应
	virtual void Respond(LPCSTR pBuf, int nLen) = 0;
	
	// 得到类型
	virtual DWORD GetType() const = 0;
	
	// 得到id
	virtual DWORD GetID() const = 0;
	
	// 设置权限
	virtual void SetRight(BOOL bRight) = 0;
	
	// 检测盒子能不能打开
	virtual BOOL IsCanOpen(void) = 0;
};
//------------------------------------------------------------------------