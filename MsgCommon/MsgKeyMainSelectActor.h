/**
*	created:		2012-7-3   1:02
*	filename: 		MsgKeyMainSelectActor
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		主消息码CMD_ROOT_SELECTACTOR定义, 此段消息码的根消息码是CMD_ROOT_SELECTACTOR
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 选择人物
#define ROOTSELECTACTOR_SC_MAIN_SELECT			1
#define ROOTSELECTACTOR_CS_MAIN_SELECT			1

// 将调到运行态
#define ROOTSELECTACTOR_SC_MAIN_TURNRUN			2
#define ROOTSELECTACTOR_CS_MAIN_TURNRUN			2

// 创建角色
#define ROOTSELECTACTOR_SC_MAIN_CREATEACTOR		3
#define ROOTSELECTACTOR_CS_MAIN_CREATEACTOR		3

// 删除角色
#define ROOTSELECTACTOR_SC_MAIN_DELETEACTOR		4
#define ROOTSELECTACTOR_CS_MAIN_DELETEACTOR		4

// 发送登录随机数
#define ROOTSELECTACTOR_SC_MAIN_ENTERCODE		5
#define ROOTSELECTACTOR_CS_MAIN_ENTERCODE		5

// 发送MAC
#define ROOTSELECTACTOR_SC_MAIN_MAC				6
#define ROOTSELECTACTOR_CS_MAIN_MAC				6

// QQ用户选择人物
#define ROOTSELECTACTOR_SC_MAIN_SELECT_QQ		7
#define ROOTSELECTACTOR_CS_MAIN_SELECT_QQ		7
//------------------------------------------------------------------------