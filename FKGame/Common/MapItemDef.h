/**
*	created:		2012-6-30   5:33
*	filename: 		MapItemDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
enum enMapItemClassID
{
	miUnknow						= 0,	// 未知对象

	// 静态对象(房子、树木、小山、血瓶、钻石等)
	miStaticObj_NoOcc_NoTrans		= 5,	// 静态对象(无占位信息、不需半透)
	miStaticObj_NoOcc_Trans			= 1,	// 静态对象(无占位信息、需要半透)
	miStaticObj_SingleOcc_NoTrans	= 7,	// 静态对象(单占位信息、不需半透)
	miStaticObj_SingleOcc_Trans		= 8,	// 静态对象(单占位信息、需要半透)
	miStaticObj_MultiOcc_NoTrans	= 9,	// 静态对象(多占位信息、不需半透)
	miStaticObj_MultiOcc_Trans		= 2,	// 静态对象(多占位信息、需要半透)

	// 纯动画对象(火焰、硝烟、鸡、牛、鸟等)
	miAniObj_NoOcc_RGB				= 3,	// 动画对象(无占位信息、不带alpha)
	miAniObj_NoOcc_RGBA				= 13,	// 动画对象(无占位信息、带有alpha)
	miAniObj_SingleOcc_RGB			= 14,	// 动画对象(单占位信息、不带alpha)
	miAniObj_SingleOcc_RGBA			= 15,	// 动画对象(单占位信息、带有alpha)
	miAniObj_MultiOcc_RGB			= 4,	// 动画对象(多占位信息、不带alpha)
	miAniObj_MultiOcc_RGBA			= 16,	// 动画对象(多占位信息、带有alpha)

	miUserDefine					= 6,	// 用户自定义对象，如任务标志等，
	miMagicObj						= 10,	// 魔法对象
	miNpcObj						= 11,	// NPC和怪物
	miActorObj						= 12,	// 角色对象
	miStaticObj_Picture				= 17,	// 主要用于帮助图标建立缓存
	miStaticObj_Article				= 19,	// 地上物品，缓冲中传入逻辑物品的图包
	miTrapObj						= 23,	// 陷阱对象
	miSpecificBlock					= 27,	// 上层特殊块
	miSpecificBlockDown				= 29,	// 下层特殊块
	miAniObj_Transprot				= 32,	// 传送门
	miStaticObj_Booth				= 33,	// 货摊

	miMaxClassID					= 36	// 当前可识别的最大类型数
};
//------------------------------------------------------------------------