/**
*	created:		2012-7-2   21:55
*	filename: 		UserSetting
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		读写用户配置文件
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
struct IUserSetting
{
public:
	// 读取设定
	virtual BOOL  ReadSetting(void) = NULL;
	// 保存设定
	virtual BOOL  SaveSetting(void) = NULL;
	
	//读取技能快捷键
	virtual BOOL  ReadSkillShortcut(void) = NULL;
	//存取技能快捷键
	virtual BOOL  SaveSkillShortcut(void) = NULL;

public:
	/********************运行环境相关****************************************/
	BOOL	m_bFullScreen;				//是否全屏显示
	BOOL	m_bOpenTrace;				//是否打开Trace功能
	BOOL	m_bOpenSound;				//是否打开音效开关
	BOOL	m_bOpenMusic;				//是否打开音乐开关
	long	m_lSoundValue;				//游戏音效大小
	long	m_lMusicValue;				//背景音乐大小

	/********************网络环境相关****************************************/
	char    m_szServerIP[64];			//游戏服务器地址
	long	m_lPort;					//游戏端口
	char    m_szUserName[32];			//默认用户名
	long    m_dwRecordUserName;			//是否纪录用户名

	/********************游戏设定相关****************************************/
	long	m_dwRightSkill;				//当前使用的右手技能
	long    m_dwLeftSkill;				//当前使用的左手技能
	long    m_dwShortcut[16];			//快捷技能
	long    m_dwShortcutGoods[8];		//快捷物品栏
	long	m_lShowNameFlag;			//人物名字显示开关
	BOOL    m_bAcceptGoods;             //是否接收物品
	BOOL    m_bAcceptTrade;             //是否交易

	BOOL    m_bSpeed;					//是否开启游戏速度优化模式
};
//------------------------------------------------------------------------
#define  SHOW_NAME_MASK_SELF		0x00000001	// 显示自己的名字
#define  SHOW_NAME_MASK_ELSE		0x00000010	// 显示其他人的名字
#define  SHOW_NAME_MASK_LEAGUE		0x00000100	// 显示帮派名字
#define  SHOW_NAME_MASK_MONSTER		0x00001000	// 显示怪物名字
//------------------------------------------------------------------------