/**
*	created:		2012-6-30   1:51
*	filename: 		MsgKeySystem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		系统事件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 客户端进入游戏
#define MSG_SYSTEM_CLIENT_PLAY						1

// 客户端小退
#define	MSG_SYSTEM_CLIENT_BACKSELEHERO				2

// 点了信息中心
#define MSG_SYSTEM_CLIENT_MSGCENTER_POP				3

// 场景服登录中心服
#define MSG_SYSTEM_ZONESERVER_LOGIN_CENTER			4

// 场景服与中心服断开
#define MSG_SYSTEM_ZONESERVER_LOGOUT_CENTER			5

// 客户端小退完成
#define	MSG_SYSTEM_CLIENT_BACKSELEHERO_COMPLETE		6
//------------------------------------------------------------------------
struct SYSTEM_CLIENT_MSGCENTER_CONTEXT
{
	int  nQueueID;
	int	 nIndex;	
	char Buffer[512];
};
//------------------------------------------------------------------------
/////////////////////////////////////////////////////////
//// ROOTTHING_SC_MAIN_GAMEWORLD 子消息
/////////////////////////////////////////////////////////
/*///////////////////////////////////////////////////////
消  息    码：GAMEWORLD_MSGID_LOOK_ACTOR_EQUIP
描        述：变更客户端天气
///////////////////////////////////////////////////////*/
#define MAINACTION_MSGID_CHANGEWEATHER				1
//------------------------------------------------------------------------