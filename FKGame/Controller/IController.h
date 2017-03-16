/**
*	created:		2012-6-21   23:24
*	filename: 		IController
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
struct IClientGlobal;
//------------------------------------------------------------------------
struct IDataReceiver
{
	virtual BOOL OnReceived(const char* pData, int nLen) = 0;	
};
//------------------------------------------------------------------------
// 用户输入事件管理
struct IController
{
	virtual ~IController(){}

	// 如果已经处理此鼠标消息应返回0
	// 参见WM_LBUTTONUP
	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam){return -1;}

	// 参见WM_LBUTTONDOWN
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam){return -1;}

	// 如果已经处理此鼠标消息应返回0
	// 参见WM_LBUTTONDOWN
	virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam){return -1;}

	// 如果已经处理此鼠标消息应返回0
	// 参见WM_RBUTTONUP
	virtual LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam){return -1;}

	// 如果已经处理此鼠标消息应返回0
	// 参见WM_MOUSEMOVE
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam){return -1;}

	// 如果已经处理此键盘消息应返回0
	// 参见WM_KEYDOWN
	virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam){return -1;}

	// 如果已经处理此键盘消息应返回0
	// 参见WM_KEYDOWN
	virtual LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam){return -1;}
	// 通用命令处理，一般用于处理来自于菜单、dialog等用户输入事件
	// 如果已经处理此消息应返回0
	enum
	{
		enPlayerLogin = 2528	/*用户登陆*/,
		enRecordName  = 2529	/*记录用户名*/,
	};
	virtual LRESULT OnCommand(DWORD dwCmd,DWORD dwParam1,DWORD dwParam2){return - 1;}

	// 如果用户输入了退出键，返回负数，系统将退出
	enum 
	{
		ieEscape = -2			/*用户按了escape键，退出*/,
		ieOK = 1				/*==TRUE，表示无事件发生*/,
		ieUpdate = 2			/*表示地图需立即更新，如居中时主角移动了位置 */
	};
	virtual int Loop(){return 1;}

	virtual void Release()
	{
		delete this;
	}

	/*更改用户控制方式  
		mode 的值为 NULL 时表示禁止用户输入事件	  
		创建失败返回False  , 并将控制模式设为NULL
	*/		
	virtual BOOL changeControlMode(int mode){return TRUE;};

	//需要从网络获取数据的模块 , 通过此函数注册后,才可收到消息 
	virtual BOOL AddReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode) {return TRUE;};
	//移除模块接收者
	virtual BOOL RemoveReceiver(IDataReceiver *pReceiver, int MainMsgCode, int SubMsgCode) {return TRUE;};

	//添加
	virtual int  PushBufferToDataCenterQueue(int nQueueID, LPCSTR pBuffer, int nLen) {return -1;};
	virtual BOOL PopBufferFromDataCenterQueue(int nQueueID) {return NULL;};
	virtual BOOL DeleteDataFromDataCenterQueue(int nIndex) {return TRUE;};
	virtual BOOL ClearDataFromDataCenterQueue(int nQueueID) {return TRUE;};

	// 处理接收数据加解锁 bLock: TRUE->加锁 FALSE->解锁
	virtual void LockReceiver(BOOL bLock) {};
	// 设置是否使用线程处理数据
	virtual void SetThreadRun(BOOL bRun) {};
	// 是否挂机
	virtual BOOL IsAutoOperate(void) {return FALSE;}; 
};
//------------------------------------------------------------------------
// 游戏输入消息控制器辅助类
class CControllerHelper
{
public:
	typedef BOOL (*ProcCreateController)(IClientGlobal *pGlobal, IController** ppController);

	CControllerHelper()
	{
		m_pInput = NULL;
		m_hdll = NULL;
	}
	~CControllerHelper()
	{
		Close();
	}
	BOOL Create(IClientGlobal *pGlobal)
	{
		try
		{
			LoadDLL();
			
			ProcCreateController proc;
			proc = (ProcCreateController)GetProcAddress(m_hdll, "dllCreateController");
			
			if (NULL == proc)
			{
				throw "Can not GetProAddress('dllCreateController')";
			}

			if (FALSE == proc(pGlobal, &m_pInput))
			{
				throw "dllCreateController() catch error";
			}

			return m_pInput != NULL;
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
			return FALSE;
		}
		catch(...)
		{
			OutputDebugString("dllCreateController() catch error\r\n");
			return FALSE;
		}
	}

	void Close()
	{
		if(m_pInput)
		{
			m_pInput->Release();
			m_pInput = NULL;
		}

		if(m_hdll)
		{
			FreeLibrary(m_hdll);
			m_hdll = NULL;
		}
	}
	
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		if(m_pInput == NULL)
			return -1;
		return m_pInput->OnLButtonUp(wParam, lParam);
	}
	
	LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam)
	{
		if(m_pInput == NULL)
			return -1;
		return m_pInput->OnRButtonUp(wParam, lParam);
	}

	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam)
	{
		if(m_pInput == NULL)
			return -1;
		return m_pInput->OnLButtonDown(wParam, lParam);
	}

	// 如果已经处理此鼠标消息应返回0
	// 参见WM_LBUTTONDOWN
	LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam)
	{
		if(m_pInput == NULL)
			return -1;
		return m_pInput->OnRButtonDown(wParam, lParam);
	}

	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam)
	{
		if(m_pInput == NULL)
			return -1;
		return m_pInput->OnMouseMove(wParam, lParam);
	}

	// 如果已经处理此键盘消息应返回0
	// 参见WM_KEYDOWN
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		if(m_pInput == NULL)
			return -1;
		return m_pInput->OnKeyDown(wParam, lParam);
	}

	// 如果已经处理此键盘消息应返回0
	// 参见WM_KEYDOWN
	LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam)
	{
		if(m_pInput == NULL)
			return -1;
		return m_pInput->OnKeyUp(wParam, lParam);
	}

	LRESULT OnCommand(DWORD dwCmd,DWORD dwParam1,DWORD dwParam2)
	{
		if(m_pInput == NULL)
			return -1;
		return m_pInput->OnCommand(dwCmd, dwParam1, dwParam2);
	}

	int Loop()
	{
		if(m_pInput == NULL)
			return 1;
		return m_pInput->Loop();
	}

	IController* operator ->()
	{
		return m_pInput;
	}

	void LoadDLL()
	{
		if(m_hdll == NULL)
		{
#ifdef _DEBUG
			m_hdll = LoadLibrary("FKController_D.dll");
			if(m_hdll == NULL)
				throw "无法加载 FKController_D.dll";
#else
			m_hdll = LoadLibrary("FKController.dll");
			if(m_hdll == NULL)
				throw "无法加载 FKController.dll";
#endif
		}
	}

public:
	IController	*m_pInput;
	HINSTANCE	m_hdll;
};
//------------------------------------------------------------------------