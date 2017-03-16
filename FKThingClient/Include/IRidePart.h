/**
*	created:		2012-7-2   22:06
*	filename: 		IRidePart
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		骑乘部件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 申明
struct IThingPart;
struct IHorse;
//------------------------------------------------------------------------
// 定义
struct IRidePart : public IThingPart
{
	// 取得马匹
	virtual IHorse *		GetHorse(int nIndex) = NULL;

	// 是否正在骑马
	virtual BOOL			IsRiding(void) = NULL;
};
//------------------------------------------------------------------------