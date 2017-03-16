/**
*	created:		2012-6-28   8:03
*	filename: 		GoodsToolTipBuilder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		这个类用来产生物品的ToolTip
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IGoods.h"
#include "IToolTipGenerator.h"
#include "GoodsToolTipCommon.h"
//------------------------------------------------------------------------
class CGoodsToolTipBuilder : public CGoodsToolTipCommon  
{
public:
	enum
	{
		enSoldPrice,      //售价
		enName,           //名称
		enDescription,        //描述
		enNeedLevel,          //使用等级
		enGoodsType,          //物品类型 (矿石 , 草药)
		enRank,               //物品档次
		enBlank,              //空行
		enBind,           //绑定
		enUnknown,            //未知属性
	};

private:
	
	void AddSoldPrice(void);    
	void AddName(void);
	void AddDescription(void);      
	void AddNeedLevel(void);    
	void AddGoodsType(void);
	void AddRank(void);
	void AddBlank(void);
	

public:
	CGoodsToolTipBuilder();
	virtual ~CGoodsToolTipBuilder();

	//用一个新对象重置TOOLTIP生成器
	BOOL SetObject(DWORD dwGoodsID, UID uid);
	//用一个新对象重置TOOLTIP生成器
	BOOL SetObject(DWORD dwGoodsID, IGoods *pGoods);	
	//为物品的TOOLTIP添加一个项
	void AddItem(int nItemID);
};
//------------------------------------------------------------------------