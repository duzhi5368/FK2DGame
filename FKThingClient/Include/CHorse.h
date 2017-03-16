/**
*	created:		2012-7-3   23:17
*	filename: 		CHorse
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		马，非物品，非thing，只是一个数据集合
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IHorse.h"
//------------------------------------------------------------------------
class CHorse : public IHorse
{
public:
	// 设置马匹ID
	virtual void		SetID(int nHorseID);
	// 取得马匹ID
	virtual int			GetID(void);

	CHorse(void);
	virtual ~CHorse(void);	
private:
	int					m_nID;
};
//------------------------------------------------------------------------