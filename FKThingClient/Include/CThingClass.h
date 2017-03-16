/**
*	created:		2012-7-2   23:36
*	filename: 		CThingClass
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		因为THING类型存在树形结构，如果简单地用一个DWORD表示，
					在用的时间就不方便了，比如说要判定一个对像是否为物品
					要依次判断是否为装备，药品，还是材料，比较麻烦。现在对
					像化，就好用多了
					
					其内也是用一个DWORD，前8位表示THINGCLASS类型, 后24位是各种标志，诸如：是不是物品，是不是生物
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/SceneObj/IThingClass.h"
//------------------------------------------------------------------------
class CThingClass : public IThingClass
{
public:
	// 返回类型值
	virtual DWORD	GetClassVal(void);

	// 返回诸如THINGCLASS_ACTOR, THINGCLASS_MONSTER这种
	virtual DWORD	GetThingClass(void);

	// 返回是否为生物
	virtual BOOL	IsCreature(void);	

	// 返回是否为装备
	virtual BOOL	IsEquipment(void);

	// 返回是否为药品
	virtual BOOL	IsMedicament(void);

	// 返回是否为凭证
	virtual BOOL	IsWarrant(void);

	// 返回是否为元宝
	virtual BOOL	IsValuables(void);

	// 设置物品类型
	void			SetThingClass(DWORD dwThingClass);

	// 设置为生物
	void			SetCreature(void);

	// 设置为装备
	void			SetEquipment(void);

	// 设置为药品
	void			SetMedicament(void);

	// 设置为凭证
	void			SetWarrant(void);

	// 设置为元宝
	void			SetValuables(void);

	CThingClass(void);

	~CThingClass(void);
private:
	DWORD			m_dwClassVal;
};
//------------------------------------------------------------------------