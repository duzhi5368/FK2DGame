/**
*	created:		2012-6-28   10:26
*	filename: 		ToolTipBuilder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		这个类用来产生ToolTip
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Include/EquipToolTipBuilder.h"
#include "../Include/SkillToolTipBuilder.h"
#include "../Include/GoodsToolTipBuilder.h"
//------------------------------------------------------------------------
class CToolTipBuilder  
{
private:
	//本类的唯一实例
	static CToolTipBuilder *pBuilder;   
private:
	IToolTipGenerator * m_pGenerator;           //当前使用的ToolTip生成器
	CToolTipGenerator m_DefaultGenerator;		//默认的ToolTip生成器
	CEquipToolTipBuilder m_EquipGenerator;		//装备的ToolTip生成器
	CSkillToolTipBuilder m_SkillGenerator;		//技能的ToolTip生成器
	CGoodsToolTipBuilder  m_GoodsGenerator;		//物品的ToolTip生成器
public:
	//为物品的TOOLTIP添加一个项
	void AddItem(int nItemID);
	//用一个新对象重置TOOLTIP生成器
	BOOL SetObject(DWORD dwGoodsID, UID uid);
	//用一个新对象重置TOOLTIP生成器
	BOOL SetObject(DWORD dwGoodsID, IGoods *pGoods);
	//用一个新对象重置TOOLTIP生成器
	BOOL SetObject(int nSkillID);
	//取TOOLTIP描述
	LPCSTR GetToolTip(void);

public:
	CToolTipBuilder();
	virtual ~CToolTipBuilder();
	static BOOL Init(void);
	static void Release(void);
	static CToolTipBuilder * Instance(void);
};
//------------------------------------------------------------------------
