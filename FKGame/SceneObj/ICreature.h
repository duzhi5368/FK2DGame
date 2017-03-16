/**
*	created:		2012-6-23   0:24
*	filename: 		ICreature
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IMapThing.h"
#include "ICreatureActState.h"
//------------------------------------------------------------------------
// 申明
struct IThingPart;
struct ICreatureCommand;
struct IBitmap;
//------------------------------------------------------------------------
// 定义接口
struct ICreature : public IMapThing
{
	// 取得生物名字
	virtual LPCSTR				GetName(void) = NULL;
	// 设置生物的数字型属性
	virtual int					SetPropNum(DWORD dwPropID, int nValue) = NULL;
	// 取得生物的数字型属性
	virtual int					GetPropNum(DWORD dwPropID) = NULL;
	// 设置生物的字符型属性
	virtual LPCSTR				SetPropStr(DWORD dwPropID, LPCSTR strPropValue) = NULL;
	// 取得生物的字符型属性
	virtual LPCSTR				GetPropStr(DWORD dwPropID) = NULL;
	// 添加生物部件, 如果已存在,则会添加失败，逻辑对像释放时，会主动释放
	virtual BOOL				AddPart(IThingPart * pPart) = NULL;
	// 删除生物部件, 并不释放
	virtual BOOL				RemovePart(DWORD dwPartID) = NULL;
	// 取得生物部件
	virtual IThingPart *		GetPart(DWORD dwPartID) = NULL;
	// 设置当前行动状态
	virtual BOOL				GotoActState(DWORD dwActStateID, LPCSTR pContext = NULL, int nLen = 0) = NULL;
	// 取得当前行动状态
	virtual ICreatureActState * GetCurActState(void) = NULL;
	// 增加一生物命令
	virtual BOOL				AddCreatureCommand(ICreatureCommand * pCreatureCommand) = NULL;
	// 取得生物头像
	virtual IBitmap *			GetHeadBitmap(void) = NULL;
	// 设置处在死亡状态
	virtual void				SetDying(BOOL bDying) = NULL;
	// 是否正处在死亡
	virtual BOOL				IsDying(void) = NULL;
};
//------------------------------------------------------------------------