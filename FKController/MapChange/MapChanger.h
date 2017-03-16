/**
*	created:		2012-6-24   20:55
*	filename: 		MapChanger
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		负责跨场景服切换地图
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../FKControllerInclude.h"
#include "../../FKGame/Controller/IController.h"
#include "../../FKGame/Network/IClientSocket.h"
#include "../../FKGame/GameGlobal/IClientGlobal.h"
#include "../../FKGame/Network/IAppNetworkLink.h"
//------------------------------------------------------------------------
class CMapChanger : public IDataReceiver , public ITimerSink
{
private:
	struct SServerKey
	{
		char szIP[128];
		WORD wPort;				//服务器开放的端口
	};		 
private:
	void BoardCast_Client_Play(void);   //进入游戏时,调事件服发消息 ,让其它模块处理
	void __DisConnectNetwork(void);
public:
	CMapChanger();
	virtual ~CMapChanger();

	BOOL OnReceived(const char* pData, int nLen);	

	virtual void	OnTimer(DWORD dwEventID); 	
};
//------------------------------------------------------------------------
extern CMapChanger  g_MapChanger;
//------------------------------------------------------------------------
