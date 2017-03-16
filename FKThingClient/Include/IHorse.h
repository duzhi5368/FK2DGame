/**
*	created:		2012-7-2   22:05
*	filename: 		IHorse
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		马，非物品，非thing，只是一个数据集合
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
struct IHorse
{
	// 设置马匹ID
	virtual void		SetID(int nHorseID) = NULL;

	// 取得马匹ID
	virtual int			GetID(void) = NULL;
};
//------------------------------------------------------------------------