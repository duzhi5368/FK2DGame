/**
*	created:		2012-6-28   7:43
*	filename: 		EquipToolTipBuilder
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		这个类用来产生装备的TOOLTIP
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IToolTipGenerator.h"
#include "../../FKGame/Common/IConfigDataCenter.h"
#include "../../FKGame/SceneObj/IEquipment.h"
//------------------------------------------------------------------------
class CEquipToolTipBuilder : public CToolTipGenerator  
{
public:
	enum
	{
		enSoldPrice,      		//售价
		enName,           		//名称
		enBind,           		//绑定
		enUnify,          		//唯一
		enCanSold,        		//能否出售
		enAttackSpeed,    		//攻击速度
		enPhysicalAttack, 		//物理攻击
		enMagicalAttack,        //魔法防御
		enPhysicalDefence,		//物理防御
		enMagicalDefence,       //魔法防御  
		enWeight,		     	//重量
		enDur,               	//耐久
		enNeedLevel,         	//需要等级
		enNeedTrible,        	//种族
		enExtraAttribute,    	//额外属性
		enInlay,             	//镶嵌
		enVocation,          	//需要职业
		enRemain,            	//剩余次数
		enDescription,       	//描述
		enEquipPosition,     	//装备位置
		enRank,
		enSlotNum,           	//插孔数
		enBlank,             	//空行
		enRepairPrice,       	//修理价格
		enSuitEffectDesc,    	//套装效果相关
		enUnknown,           	//未知属性
	};

private:
	EQUIPMENT_BASEPROPERTY m_GoodsProp;
	IEquipment *m_pEquipment;
	
	//每个小函数会向TOOLTIP中添加自己的内容
	void AddSoldPrice(void);      			//售价
	void AddName(void);           			//名称
	void AddBind(void);           			//绑定
	void AddUnify(void);          			//唯一
	void AddCanSold(void);        			//能否出售
	void AddAttackSpeed(void);    			//攻击速度
	void AddPhysicalAttack(void); 			//物理攻击
	void AddMagicalAttack(void);        	//魔法防御
	void AddPhysicalDefence(void);			//物理防御
	void AddMagicalDefence(void);       	//魔法防御  
	void AddWeight(void);		     		//重量
	void AddDur(void);               		//耐久
	void AddNeedLevel(void);         		//需要等级
	void AddNeedTrible(void);				//种族
	void AddExtraAttribute(void);    		//额外属性
	void AddInlay(void);             		//镶嵌
	void AddVocation(void);          		//需要职业
	void AddRemain(void);            		//剩余次数
	void AddDescription(void);       		//描述
	void AddEquipPosition(void);       		//装备位置
	void AddRank(void);                		//档次
	void AddSlotNum(void);             		//接口数
	void AddBlank(void);               		//空行
	void AddRepairPrice(void);         		//修复价格
	void AddSuitEffect(void);				//套装效果

	typedef void (CEquipToolTipBuilder::*ITEMFUNCTION)(void);
	ITEMFUNCTION functions[enUnknown];

	int __GetEffectCount(void);
public:

	CEquipToolTipBuilder();
	virtual ~CEquipToolTipBuilder();

	//用一个新对象重置TOOLTIP生成器
	BOOL SetObject(DWORD dwGoodsID, UID uid);
    //用一个新对象重置TOOLTIP生成器
	BOOL SetObject(DWORD dwGoodsID, IGoods *pGoods);
	//为物品的TOOLTIP添加一个项
	void AddItem(int nItemID);	
};
//------------------------------------------------------------------------