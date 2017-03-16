/**
*	created:		2012-6-28   9:32
*	filename: 		TitleDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <string>
#include <Windows.h>
using std::string;
//------------------------------------------------------------------------
// 无限次使用
#define INFINITY_TIME_USE  -1000
// 无效称号ID
#define INVALID_TITLE_ID (-1)
//------------------------------------------------------------------------
#pragma pack(push,1)
//------------------------------------------------------------------------
// 称号状态
enum TITLE_STATUS
{
	 UNKNOW_TITLE,     //  不了解
     UNDERSTAND_TITLE, //  了解
     HOLD_TITLE,       //  拥有
	 USE_TITLE         //  使用称号
};
//------------------------------------------------------------------------
struct STitle
{
   	string  strUseFun;          //  使用称号的回调函数
	string  strUnUseFun;        //  不使用称号的回调函数
	string  strDeleteFun;     //  删除称号的回调函数
    long    lTitleId;           //  称号ID
	string  strName;            //  称号名称
	short   sCanUseTimes;       //  可使用的次数
	long    lCanUseTime;        //  可是用的时间
	string  strTitleName;       //  实际名字
	BOOL    bLogoutOff;         //  下线是否取消
	BOOL    bCanOff;            //  能否取消
    BOOL    bCanDel;            //  能否删除
	long    lTimeDel;           //  超时时间,超过了这个时间就会自动删除
    string  strCondition;       //  获取条件说明
	string  strMethod;          //  获取方法说明
	string  strDes;             //  描述
	string  strExDes;           //  其他描述信息
};
//------------------------------------------------------------------------
// 玩家身上的称号这个结构主要用在服务器端
struct SActorTitle
{
    long   lTitleId;      // 称号ID
	string strName;       // 称号名字
	long   lGetTime;      // 获取这个称号的时间
	short  sLeftUsedTimes; // 剩余使用次数
    long   lCurLeftTime;  // 正在使用的称号还剩下多少时间
	BYTE   byStatus;      // 当前称号的状态
};
//------------------------------------------------------------------------
// 下面2个结构主要用客户端
struct SActorTitle2
{
   long  lTitleId;     // 称号ID 
   char  szName[32];   // 名字
   long  lGetTime;     // 获得时间
   long  lStatus;      // 称号状态
};
//------------------------------------------------------------------------
struct SActorTitle2Ex
{
    SActorTitle2 title; // 称号
	DWORD dwData;       // 扩展数据
};
//------------------------------------------------------------------------
enum Title_CMD
{
   TITLE_CMD_GET,          // 获取自己的所有称号
   TITLE_CMD_USE,          // 使用称号
   TITLE_CMD_UNUSE,        // 不使用称号
   TITLE_CMD_DELETE,       // 删除称号
   TITLE_CMD_GETOTHER,     // 获取某个玩家的称号
   TITLE_CMD_STATUSCHANGE, // 状态改变通知
   TITLE_CMD_ADD,          // 增加一个称号
   TITLE_CMD_MAX

};
//------------------------------------------------------------------------
#pragma pack(pop)
//------------------------------------------------------------------------