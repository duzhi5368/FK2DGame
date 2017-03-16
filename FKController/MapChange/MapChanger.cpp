/**
*	created:		2012-6-24   20:56
*	filename: 		MapChanger
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "MapChanger.h"
//------------------------------------------------------------------------
CMapChanger  g_MapChanger;
//------------------------------------------------------------------------
CMapChanger::CMapChanger()
{

}
//------------------------------------------------------------------------
CMapChanger::~CMapChanger()
{

}
//------------------------------------------------------------------------
void CMapChanger::OnTimer(DWORD dwEventID)
{
	g_ClientGlobal->m_TimerAxisHelper.m_pTimeAxis->KillTimer(0, this);	
}
//------------------------------------------------------------------------
BOOL CMapChanger::OnReceived(const char* pData, int nLen)
{	
	TraceLn("收到了切换地图消息 , CMapChanger::OnReceived");
/*
	struct __SC_THING_CHANAGEMAP_HEAD
	{//数据传进来时已经去掉两个消息头了, 原SC_THING_CHANAGEMAP_HEAD 不合适,这里自己写一个
		DWORD	m_dwChangeMode;		// 切换模式.1 : 本进程切换, 2 ：跨进程切换
		int		m_nMapID;			// 地图ID
		POINT	m_ptWorld;			// 世界坐标
	};

	//停掉音乐
	g_pGlobal->GetSoundEngine()->ClearAllSound();
	g_pGlobal->GetSoundEngine()->ResetCurrentEnvironmentSoundConfig();

	//关掉输入框
	g_pGlobal->m_pWndMgr->OnEvent(WND_INPUTWND, WE_CW_INPUT_CLEAR, 0, 0);


	//准备换地图了,通知各窗口取消注册快捷键, 以免玩家在换图时通过快捷键调出窗口
	g_pGlobal->m_pWndMgr->OnEvent(WND_MAIN, WE_AW_ALLWND_UNREGACCELE, 0, 0);
	g_pGlobal->m_pWndMgr->OnEvent(WND_CHAT, WE_AW_ALLWND_UNREGACCELE, 0, 0);
	g_pGlobal->m_pWndMgr->OnEvent(WND_TARGETRIM, WE_AW_ALLWND_UNREGACCELE, 0, 0);
	g_pGlobal->m_pWndMgr->OnEvent(WND_MINIMAP, WE_AW_ALLWND_UNREGACCELE, 0, 0);

	//收到换地图消息后隐藏一些窗口, 这些窗口通常与人物的某些事件绑定了,隐藏时会消除绑定
	g_pGlobal->m_pWndMgr->ShowWnd(WND_MAIN, SW_HIDE);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_PLAYERINFO, SW_HIDE);

	//技能窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_PRACTICESKILL);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_PRACTICESKILL, SW_HIDE);						
	g_pGlobal->m_pWndMgr->CreateWnd(WND_VOCATIONSKILL);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_VOCATIONSKILL, SW_HIDE);									
	g_pGlobal->m_pWndMgr->CreateWnd(WND_WORKSKILL);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_WORKSKILL, SW_HIDE);

	//工作技能窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_REFINE);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_REFINE, SW_HIDE);						
	g_pGlobal->m_pWndMgr->CreateWnd(WND_COMPOSE);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_COMPOSE, SW_HIDE);		
	g_pGlobal->m_pWndMgr->CreateWnd(WND_INLAY);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_INLAY, SW_HIDE);	
	g_pGlobal->m_pWndMgr->CreateWnd(WND_EPURATE);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_EPURATE, SW_HIDE);	
	g_pGlobal->m_pWndMgr->CreateWnd(WND_PHARMACY);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_PHARMACY, SW_HIDE);	

	g_pGlobal->m_pWndMgr->CreateWnd(WND_REQUEST_WORKSKILL);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_REQUEST_WORKSKILL, SW_HIDE);						
	g_pGlobal->m_pWndMgr->CreateWnd(WND_REFINE_EX);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_REFINE_EX, SW_HIDE);						
	g_pGlobal->m_pWndMgr->CreateWnd(WND_COMPOSE_EX);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_COMPOSE_EX, SW_HIDE);		
	g_pGlobal->m_pWndMgr->CreateWnd(WND_INLAY_EX);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_INLAY_EX, SW_HIDE);	
	g_pGlobal->m_pWndMgr->CreateWnd(WND_EPURATE_EX);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_EPURATE_EX, SW_HIDE);	



	//关闭登录系列窗口, 打开的聊天窗口等
	g_pGlobal->m_pWndMgr->ShowWnd(WND_ACTORSELECT, SW_HIDE);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_ACTORSELECT_BAK, SW_HIDE);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_LOGIN_BAK, SW_HIDE);

	g_pGlobal->m_pWndMgr->ShowWnd(WND_LOGIN, SW_HIDE);
	g_pGlobal->m_pWndMgr->CreateWnd(WND_MINIMAP);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_MINIMAP, SW_HIDE);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_MINIMAP, SW_SHOW);	
	g_pGlobal->m_pWndMgr->CreateWnd(WND_CHAT);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_CHAT, SW_SHOW);	

	g_pGlobal->m_pWndMgr->CreateWnd(WND_TARGETRIM);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_TARGETRIM, SW_HIDE);	
	g_pGlobal->m_pWndMgr->CreateWnd(WND_EQUIPMENT);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_EQUIPMENT, SW_HIDE);		
	g_pGlobal->m_pWndMgr->CreateWnd(WND_PACKAGE);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_PACKAGE, SW_HIDE);

	//财宝窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_TREASUREBOX);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_TREASUREBOX, SW_HIDE);	

	//通知人物属性窗口, 开始监听人物创建的消息
	g_pGlobal->m_pWndMgr->CreateWnd(WND_PLAYERINFO);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_PLAYERINFO, SW_HIDE);	
	g_pGlobal->m_pWndMgr->OnEvent(WND_PLAYERINFO, WE_CW_PLAYERINFO_STARTLISTENBORN, 0, 0);	


	//系统菜单窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_SYSTEMMENU);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_SYSTEMMENU, SW_HIDE);	

	//世界地图窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_WORLDMAP);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_WORLDMAP, SW_HIDE);

	//别人的装备窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_EQUIPMENT_OTHER);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_EQUIPMENT_OTHER, SW_HIDE);

	//图型设置窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_GRAPHICSSETTING);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_GRAPHICSSETTING, SW_HIDE);

	//自动回复窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_AUTOREVIVE);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_AUTOREVIVE, SW_HIDE);

	//第二快捷栏
	g_pGlobal->m_pWndMgr->CreateWnd(WND_EXTRASHORTCUT);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_EXTRASHORTCUT, SW_HIDE);

	//宠物命令
	g_pGlobal->m_pWndMgr->CreateWnd(WND_PETCOMMAND);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_PETCOMMAND, SW_HIDE);

	//下落卷轴窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_DROPSCROLL);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_DROPSCROLL, SW_SHOW);

	//校验码窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_VALIDATIONCODE);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_VALIDATIONCODE, SW_HIDE);

	//进度条窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_PROCESSBAR);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_PROCESSBAR, SW_HIDE);

	//LUA进度条窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_LUAPROCESSBAR);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_LUAPROCESSBAR, SW_HIDE);

	//创建通用密码窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_GENERALPASSWORD);	
	g_pGlobal->m_pWndMgr->ShowWnd(WND_GENERALPASSWORD, SW_HIDE);

	//Loading窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_LOADING);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_LOADING, SW_HIDE);

	//主窗口要在技能窗口创建后才能创建,因为它要向技能窗口取技能图片
	g_pGlobal->m_pWndMgr->CreateWnd(WND_MAIN);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_MAIN, SW_SHOW);	

	//氏族家园窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_CLAN_HOMESTEAD);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_CLAN_HOMESTEAD, SW_HIDE);	

	//氏族技能窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_CLAN_SKILL);
	g_pGlobal->m_pWndMgr->ShowWnd(WND_CLAN_SKILL, SW_HIDE);	

	//新合成窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_COMPOSEV2);		
	g_pGlobal->m_pWndMgr->ShowWnd(WND_COMPOSEV2, SW_HIDE);	

	//新提炼窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_FUSIONV2);		
	g_pGlobal->m_pWndMgr->ShowWnd(WND_FUSIONV2, SW_HIDE);	

	//新炼丹窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_PHARMACYV2);		
	g_pGlobal->m_pWndMgr->ShowWnd(WND_PHARMACYV2, SW_HIDE);

	//新镶嵌窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_INLAYV2);		
	g_pGlobal->m_pWndMgr->ShowWnd(WND_INLAYV2, SW_HIDE);

	//新精炼窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_REFINEV2);		
	g_pGlobal->m_pWndMgr->ShowWnd(WND_REFINEV2, SW_HIDE);

	//新提炼窗口
	g_pGlobal->m_pWndMgr->CreateWnd(WND_EPURATEV2);		
	g_pGlobal->m_pWndMgr->ShowWnd(WND_EPURATEV2, SW_HIDE);

	//将技能数据有效位调为无效, 否则会弹出"未学会形态转换, 不能显示第二快捷栏"
	g_pGlobal->m_pWndMgr->OnEvent(WND_GRAPHICSSETTING, WE_GRAPHICSSETTING_SETSKILLDATAFLAG, 0, 0);

	__SC_THING_CHANAGEMAP_HEAD *pHead = (__SC_THING_CHANAGEMAP_HEAD *)pData;

	if (pHead->m_dwChangeMode == 2)
	{		
		g_MapChangingLinkSink.SetServer(pData + sizeof(__SC_THING_CHANAGEMAP_HEAD), sizeof(SServerKey));

		//切换到地图转换态去		
		g_MapChangingLinkSink.RegisterLogonMsg();  //注册,开始等待人物断线消息

		g_pGlobal->m_pWndMgr->CreateWnd(WND_LOADING);
		g_pGlobal->m_pWndMgr->ShowWnd(WND_LOADING, SW_SHOW);

		//将操作模式设为默认那种, 免得玩家在选择画面还继续发命令控制人物
		g_pGlobal->GetController()->changeControlMode(0);

		g_pGlobal->m_pWndMgr->ShowWnd(WND_MAIN, SW_HIDE);		
	}	

	//发出登录消息 ,通知其它模块, 玩家开始进入游戏
	BoardCast_Client_Play();

	//停掉所有音效, 再复位环境音效, 否则(相同类型地图传送时,听不到音乐(因冻结时间)
	g_pGlobal->GetSoundEngine()->ClearAllSound();
	g_pGlobal->GetSoundEngine()->ResetCurrentEnvironmentSoundConfig();
*/
	return TRUE;	
}
//------------------------------------------------------------------------
void CMapChanger::BoardCast_Client_Play(void)
{
	//IEventServer *pEventServer = g_pGlobal->m_pEventServer;
	//if (pEventServer)
	//{
	//	pEventServer->FireAction(NULL, 0, MSG_SYSTEM_CLIENT_PLAY, SRC_TYPE_ID_SYSTEM, (DWORD)this);
	//}
}
//------------------------------------------------------------------------
void CMapChanger::__DisConnectNetwork(void)
{
	if (g_ClientGlobal->m_pAppNetworkLink == NULL)
	{
		return;
	}
	IClientLink *pLink = g_ClientGlobal->m_pAppNetworkLink->GetClientLink();

	if (pLink && pLink->IsConnected())
	{
		pLink->CloseConnect();
	}		
}
//------------------------------------------------------------------------