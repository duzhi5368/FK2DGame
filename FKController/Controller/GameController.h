/**
*	created:		2012-6-25   0:39
*	filename: 		GameController
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		游戏逻辑部分,处理各种键盘鼠标输入,创建游戏界面
*/
//------------------------------------------------------------------------
#include "IControlModel.h"
#include "../../FKGame/Controller/IController.h"
#include "../../FKGame/Network/IClientSocket.h"
#include "../../FKTrace/IFKTrace.h"
#include "../ValidationCode/WndCallback.h"
#include "BaseControlMode.h"
//------------------------------------------------------------------------
//调用createConnection() 时,,指明创建哪一个收发状态
#define SINKSTATE_LOGIN         0
#define SINKSTATE_ACTORSELECT   1
#define SINKSTATE_SCENE         2
#define SINKSTATE_RUNNING       3
#define SINKSTATE_MAPCHINGING   4
//------------------------------------------------------------------------
#define FKTEST_MSG_CENTER_TYPE_MAX 7
#ifndef MSG_CENTER_TYPE_MAX
#define MSG_CENTER_TYPE_MAX FKTEST_MSG_CENTER_TYPE_MAX
#endif
//------------------------------------------------------------------------
// 游戏输入消息控制器
class CGameControl : public IController
{
private:
	struct DataCenterMessage
	{
		int nMsgID;            
		char *pBuffer;
		int nLen;
	};
	std::list<DataCenterMessage> m_lsQueue[MSG_CENTER_TYPE_MAX];
	int m_nMsgID_Count;    //信息ID计数器

public:
	virtual LRESULT     OnRButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT     OnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnKeyUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnCommand(DWORD dwCmd,DWORD dwParam1,DWORD dwParam2);

	// 处理接收数据加解锁 bLock: TRUE->加锁 FALSE->解锁
	virtual void		LockReceiver(BOOL bLock);
	// 设置是否使用线程处理数据
	virtual void		SetThreadRun(BOOL bRun);

	virtual int			Loop();
	// 释放过程
	virtual void		Release(void);
public:
	CGameControl();
	virtual ~CGameControl();

	// 创建过程
	BOOL				Create(void);

	/*创建网络连接  , SinkState 指明创建哪一个槽的连接, 
	   SINKSTATE_LOGIN        -- 登录   
	   SINKSTATE_ACTORSELECT  --人物选择    
	   SINKSTATE_SCENE        --场景服     
	   SINKSTATE_RUNNING      --运行
    */
	BOOL				createConnection(int SinkState);

	//更改控制模式
	virtual BOOL changeControlMode(int mode);

	//需要从网络获取数据的模块 , 通过此函数注册后,才可收到消息 
	virtual BOOL AddReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode) ;
	
	//移除模块接收者
	virtual BOOL RemoveReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode) ;
	
	virtual int  PushBufferToDataCenterQueue(int nQueueID, LPCSTR pBuffer, int nLen) ;
	
	virtual BOOL PopBufferFromDataCenterQueue(int nQueueID);
	
	virtual BOOL DeleteDataFromDataCenterQueue(int nIndex) ;
	
	virtual BOOL ClearDataFromDataCenterQueue(int nQueueID);

	virtual BOOL IsAutoOperate(void);

private:
	CBaseControlMode    m_ControlMode1;
	IControlMode        m_DefaultMode;
 	IControlMode      * m_pControlMode;
	CIWndMgrHelper		m_WndHelper;
	CWndCallback		m_WndCallback;
	CClientLinkHelper	m_SocketHelper;

	CWMConnection<CGameControl> m_MsgCenterPop;    //窗口通知信息中心,弹出消息
	void OnMsgCenterClicked( WPARAM wParam, LPARAM lParam,BOOL &bContinue);		
};
//------------------------------------------------------------------------
extern CGameControl * g_pGameControl;
//------------------------------------------------------------------------