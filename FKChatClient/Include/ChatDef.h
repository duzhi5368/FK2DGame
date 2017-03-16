/**
*	created:		2012-6-28   4:41
*	filename: 		ChatDef
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		聊天频道的定义
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
typedef __int64 UID;
//------------------------------------------------------------------------
#define CHAT_MESSAGE_MAX	400			// 消息的最大长度
#define CHAT_NAME_MAX		256			// 玩家名称的最大长度
#define CHAT_LOCAL_MAX		10			// 附近频道的以玩家为中心的范围
//------------------------------------------------------------------------
// 聊天显示窗口的显示方式
enum CHAT_WINDOW
{
	CW_ALTERNATE,	// 交替
	CW_HIDE,		// 隐藏
	CW_SHOW			// 显示
};
//------------------------------------------------------------------------
// 自定义的命令
enum CHAT_CUSTOM_CMD
{
	CHAT_CMD_NONE,
	CHAT_CMD_PING,
	CHAT_CMD_BULLETIN,
	CHAT_CMD_SHUTUP,
	CHAT_CMD_KICK,
	CHAT_CMD_FIND,
	CHAT_CMD_GOTO,
	CHAT_CMD_FAIL,
	CHAT_CMD_FETCH,
	CHAT_CMD_FETCH2,              // 按DBID拉人
	CHAT_CMD_UPDATEFREEWORLDMSG   // 更新世界聊天免费条数
};
//------------------------------------------------------------------------
// 频道定义(最小频道必须为0,以便于操作)
enum CHANNEL
{
	CHANNEL_SYSTEM = 0,		// 系统
	CHANNEL_LOCAL,			// 附近
	CHANNEL_PRIVATE,		// 私聊
	CHANNEL_TEAM,			// 队伍
	CHANNEL_CLAN,			// 氏族
	CHANNEL_WORLD,			// 世界
	CHANNEL_MONSTER        // 怪物频道
};
//------------------------------------------------------------------------
// 世界频道的发言冻结时间设置
#define COLDTIME_CHAT_TYPE				3
#define COLDTIME_CHAT_WORLD_VALUE		1  //世界频道
#define COLDTIME_CHAT_ELSE_VALUE        2  //其他频道


#define CHANNEL_MIN CHANNEL_SYSTEM		// 最小频道
#define CHANNEL_MAX CHANNEL_WORLD		// 最大频道
#define CHANNEL_COUNT (CHANNEL_MAX + 1)	// 频道个数
//------------------------------------------------------------------------
// 检查是不是有效的频道
#define IS_VALID_CHANNEL(c) (CHANNEL_MIN <= (c) && (c) <= CHANNEL_MAX)
//------------------------------------------------------------------------
#pragma pack(push, 1)
//------------------------------------------------------------------------
// 聊天信息头
struct SChatMsgHeader
{
	BYTE nChannel;				// 频道
	UID  uidSender;				// 发送者UID
	char szSender[32];			// 发送者
};
//------------------------------------------------------------------------
// 系统聊天信息
struct SSystemChatMsg
{
	SChatMsgHeader header;
	BYTE nCustomCmd;
	WORD wTextLen;
	char szText[1];
};
//------------------------------------------------------------------------
// 附近聊天信息
struct SLocalChatMsg
{
	SChatMsgHeader header;
	WORD wTextLen;
	char szText[1];
};
//------------------------------------------------------------------------
// 两人对聊信息
struct SPrivateChatMsg
{
	SChatMsgHeader header;
	char szReceiver[32];
	WORD wTextLen;
	char szText[1];
};
//------------------------------------------------------------------------
// 队伍聊天信息
struct STeamChatMsg
{
	SChatMsgHeader header;
	DWORD dwTeamId;
	WORD wTextLen;
	char szText[1];
};
//------------------------------------------------------------------------
// 氏族聊天信息
struct SClanChatMsg
{
	SChatMsgHeader header;
	DWORD dwDBID;           //氏族特殊处理,需要传入DBID
    DWORD dwClanId;
	WORD wTextLen;
	char szText[1];
};
//------------------------------------------------------------------------
// 世界聊天信息
struct SWorldChatMsg
{
	SChatMsgHeader header;
	WORD wTextLen;
	char szText[1];
};
//------------------------------------------------------------------------
// 怪物聊天信息
struct SMonsterChatMsg
{
	SChatMsgHeader header;
	WORD wTextLen;
	char szText[1];
};
//------------------------------------------------------------------------
#pragma pack(pop)
//------------------------------------------------------------------------