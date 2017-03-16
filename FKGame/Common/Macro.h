/**
*	created:		2012-6-20   23:55
*	filename: 		Macro
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
#define FK_TEST
//------------------------------------------------------------------------
#ifdef FK_TEST
#define _USE_BINSCRIPT
// #define _USE_FSP
#endif
//------------------------------------------------------------------------
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) if(p) { (p)->Release(); (p) = NULL; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if(p) { delete (p); (p) = NULL; }
#endif
//------------------------------------------------------------------------
#ifndef ASSERT
#define ASSERT(x)	{if(!(x))__asm int 3}
#endif
//------------------------------------------------------------------------
#define _D3D_ON_
//------------------------------------------------------------------------
// 窗口类名
#define WIN_CLASS_NAME	"FreeKnightGameClass"
// 窗口名
#define WIN_TITLE_NAME	"遗失之岛 Ver1.0"
// 窗口宽度
#define GAME_WINDOW_WIDTH	1024
// 窗口高度
#define GAME_WINDOW_HEIGHT	768
#define	VIEW_WIDTH			1024
#define	VIEW_HEIGHT			768
//------------------------------------------------------------------------
#ifndef PrintEventLn
	#ifdef _DEBUG
	#define PrintEventLn g_DefaultTrace.ThreadSafeDebugPrintLn
	#else
	#define PrintEventLn g_DefaultTrace.ThreadSafeDebugPrintLn
	#endif
#endif
#ifdef TraceLn
#undef TraceLn
#endif
#define TraceLn PrintEventLn
#define SimpleTraceLn g_DefaultTrace.DebugPrintLnWithTime
//------------------------------------------------------------------------
#define XCRASHREPORT_MINI_DUMP_FILE		"Report.dmp"
#define XCRASHREPORT_ERROR_LOG_FILE		"Errorlog.txt"
#define XCRASHREPORT_TRACE_FILE			"Trace.txt"
//------------------------------------------------------------------------
#ifdef _DEBUG
	#define XCRASHREPORT_CRASH_REPORT_APP	"ReportD.dll"
	#else
	#define XCRASHREPORT_CRASH_REPORT_APP	"Report.dll"
#endif
//------------------------------------------------------------------------
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif
//------------------------------------------------------------------------
#ifndef IN
	#define IN
#endif
#ifndef OUT
	#define OUT
#endif
//------------------------------------------------------------------------
#define MAX(i, j) ((i < j) ? j : i)
#define MIN(i, j) (( i < j) ? i : j)
//------------------------------------------------------------------------
#define STARTPROFILE( CHECKPOINT )	CProfileManager::Start_Profile(CHECKPOINT);{
#define STOPPROFILE					}CProfileManager::Stop_Profile();
//------------------------------------------------------------------------
#define INVALID_SOCKET_ID			(-1)
#define MAX_SOCKET_DATA_LEN			(16 * 1024)						// 用户数据最大尺寸，不能再大了，呵呵
#define MAX_SOCKET_RAW_DATA_LEN		(MAX_SOCKET_DATA_LEN + 256)		// 底层通讯包最大长度，多余部分用于命令包头等
#define MAX_SOCKET_BUF_LEN			MAX_SOCKET_RAW_DATA_LEN	
#define MAX_CLIENT_SOCKET			63								// IThreadRoute最多可以同时管理63个通讯连接事件，服务器端程序可能还需要其它事件，所以同时支持的连接会更少
#define MAX_NAME_STRING_LEN			32								// 用户名和密码、代理服务器名需用到这个宏
//------------------------------------------------------------------------
// UID定义
typedef  __int64 UID;
struct _UID
{
	DWORD	dwRotatoryID;
	WORD	wTypeID;
	WORD	wServerID;
};
//------------------------------------------------------------------------
// 无效UID
#define	INVALID_UID					((UID)0xFFFFFFFFFFFFFFFF)
// 无效的地图ID
#define INVALID_MAPID				0
// UID解析
#define GETUIDROTATORYID(uid)		(((_UID&)uid).dwRotatoryID)
#define GETUIDTYPE(uid)				(((_UID&)uid).wTypeID)
#define GETUIDSERVERID(uid)			(((_UID&)uid).wServerID)
// 最长逻辑对象名字长度
#define MAX_NAME_LEN				32
// 最长登陆密码长度
#define MAX_LOGINPASSWORD_LEN		16
// 无效的逻辑对像属性ID，包括所有逻辑对像
#define INVALID_PROPID				-1
// 最大路径长度
#define MAX_PATH_LEN				256
// 广播范围的宽度
#define BROADCAST_AREA_WIDTH		20
// 广播范围的高度
#define BROADCAST_AREA_HEIGHT		20
// 一个屏幕的TILE数
#define SCREEN_TILE					16
// 容许与NPC对话的相距TILE数
#define MEETNPC_SPACE_TILE			8
// 移动的速度等级
#define MAX_MOVESPEED_LEVEL			9
// 普通攻击的速弃等级
#define MAX_ATTACKSPEED_LEVEL		9
//------------------------------------------------------------------------
#define	 MOVE_DELTA				40
//------------------------------------------------------------------------
#define	 TICK_BOTTOM			16	// 最高刷新率
#define  TICK_INACTIVE			100 // 丢失焦点后的刷新率
#define  TICK_TOP				30	// 最低刷新率
#define  TICK_WAIT_LOGIC		49
#define  TICK_TIMES				16
//------------------------------------------------------------------------
#define USER_NAME_MAX_LEN		32
#define PASSWORD_MAX_LEN		32
//------------------------------------------------------------------------
#define LOG_PATH				"%s\\Log"
#define TRACE_LOG_FILE			"Log\\trace.txt"
#define GAME_CONFIG_FILE		"Data\\Config\\FKGameConfig.ini"
#define DEBUG_RES_LIST_FILE		"Data\\scp\\Window.scp"
#define RELEASE_RES_LIST_FILE	"Data\\scp\\Window.res"
#define DEBUG_FILESYSTEM_DIR	"Data\\pic"
#define RELEASE_FILESYSTEM_FILE "Data\\pic\\pic.fsp"
#define	OCCUPANT_BITMAP_FILE	"%s\\Data\\Res\\flags.bmp"
#define CLIENT_VERSION_FILE		"ClientVersion.FKP"
#define EVENT_RES_FILE			"%s\\Data\\Scp\\Event.scp"
#define MAP_RES_DIR				"%s/Data/map"
//------------------------------------------------------------------------