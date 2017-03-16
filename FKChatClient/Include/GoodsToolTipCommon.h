/**
*	created:		2012-6-28   8:06
*	filename: 		GoodsToolTipCommon
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		物品分为 物品, 配方, 与炼化瓶3类, 这个类用来实现一些公共处理
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IToolTipGenerator.h"
#include "../../FKGame/SceneObj/IGoods.h"
#include "../../FKGame/Common/IConfigDataCenter.h"
//------------------------------------------------------------------------
class CGoodsToolTipCommon : public CToolTipGenerator  
{
protected:
	IGoods*				m_pGoods;
	GOODS_BASEPROPERTY	m_GoodsProp;
protected:
	void AddBind(void);
public:
	CGoodsToolTipCommon();
	virtual ~CGoodsToolTipCommon();

	virtual BOOL SetObject(DWORD dwGoodsID, UID uid);
    virtual BOOL SetObject(DWORD dwGoodsID, IGoods *pGoods);
};
//------------------------------------------------------------------------