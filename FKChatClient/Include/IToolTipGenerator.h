/**
*	created:		2012-6-28   8:54
*	filename: 		IToolTipGenerator
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		对需要生成TOOLTIP的物件提供一个统一的接口
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
#include <string>
//------------------------------------------------------------------------
class IToolTipGenerator  
{
public:
	//为物品的TOOLTIP添加一个项
	virtual void AddItem(int nItemID) = 0;
	
	//取TOOLTIP描述
	virtual LPCSTR GetToolTip(void) = 0;
	
	//清除TOOLTIP
	virtual void Clear(void) = 0;
};
//------------------------------------------------------------------------
class CToolTipGenerator : public IToolTipGenerator
{
protected:
	std::string strToolTip;

public:	
	//一般后面的类一般重载此方法就可以了
	virtual void AddItem(int nItemID) {};
	
	//取TOOLTIP描述
	virtual LPCSTR GetToolTip(void) 
	{
		return strToolTip.c_str();
	};
	
	//清除TOOLTIP
	virtual void Clear(void) 
	{
		strToolTip.clear();
	}	
};
//------------------------------------------------------------------------