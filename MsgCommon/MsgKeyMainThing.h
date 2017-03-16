/**
*	created:		2012-6-30   1:53
*	filename: 		MsgKeyMainThing
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 发布私有thing
#define ROOTTHING_SC_MAIN_PRIVATECONTEXT		1
#define ROOTTHING_CS_MAIN_PRIVATECONTEXT		1

// 发布公有thing
#define ROOTTHING_SC_MAIN_PUBLICCONTEXT			2
#define ROOTTHING_CS_MAIN_PUBLICCONTEXT			2

// 状态变化
#define ROOTTHING_SC_MAIN_CONTEXTCHANGE			3
#define ROOTTHING_CS_MAIN_CONTEXTCHANGE			3

// 切换地图
#define ROOTTHING_SC_MAIN_CHANGEMAP				4
#define ROOTTHING_CS_MAIN_CHANGEMAP				4

// 逻辑对像行为事件，如移动、攻击、伤害、动作、消失等
#define ROOTTHING_SC_MAIN_ACTION				5
#define ROOTTHING_CS_MAIN_ACTION				5

// 摆摊专用主消息码
#define ROOTTHING_SC_MAIN_STORE					6
#define ROOTTHING_CS_MAIN_STORE					6

// 通用物品栏专用
#define ROOTTHING_SC_MAIN_CONTAINER				7
#define ROOTTHING_CS_MAIN_CONTAINER				7

// 结婚模块消息码专用
#define ROOTTHING_SC_MAIN_MARRY					8
#define ROOTTHING_CS_MAIN_MARRY					8

// 交互系统消息码专用
#define ROOTTHING_SC_MAIN_INTERACT				9
#define ROOTTHING_CS_MAIN_INTERACT				9

// 好友系统消息码专用
#define ROOTTHING_SC_MAIN_FRIEND				10
#define ROOTTHING_CS_MAIN_FRIEND				10

// 计费系统消息码专用
#define ROOTTHING_SC_MAIN_COUNTFEE				11
#define ROOTTHING_CS_MAIN_COUNTFEE				11

// 游戏世界的消息码专用
#define ROOTTHING_SC_MAIN_GAMEWORLD				12
#define ROOTTHING_CS_MAIN_GAMEWORLD				12

// 最大主消息码
#define MAX_ROOTTHING_MAIN_KEY					24

//------------------------------------------------------------------------