/**
*	created:		2012-6-29   12:54
*	filename: 		IContainerClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../Common/Macro.h"
//------------------------------------------------------------------------
/////////////////////////////通知窗口的消息定义///////////////////////////////////
//通知窗口更新
// wParam - int* 需要更新的位置列表
// lParam - int 需要更新的个数
//#define CONTAINER_WND_MSG_UPDATE		0	

/////////////////////////////事件服务器的消息///////////////////////////////
//------------------------------------------------------------------------
#define CONTAINER_UPDATE				1	
//------------------------------------------------------------------------
struct SSingleInfo 
{
	int nLocation;	//位置
	UID uidOldGoods;	//旧的物品UID
	UID uidNewGoods;	//新的物品UID
	SSingleInfo()
	{
		nLocation = -1;
		uidOldGoods = INVALID_UID;
		uidNewGoods = INVALID_UID;
	}
};
//------------------------------------------------------------------------
//更新的响应现场
struct SContainerUpdateResponseContext
{
	SContainerUpdateResponseContext()
	{
		dwContainerID = 0;
		dwUpdateNum = 0;		
	}
	DWORD dwContainerID;
	DWORD dwUpdateNum;	
	//包含dwUpdateNum个SSingleInfo
};
//------------------------------------------------------------------------
struct IContainer;
//------------------------------------------------------------------------
struct IContainerGoods
{
	virtual void Release() = NULL;
	virtual UID GetUID() = NULL;
	virtual DWORD GetNumber() = NULL;
	// 取得物品ID
	virtual DWORD GetGoodsID() = NULL;	
	// 取得物品名字
	virtual LPCSTR GetName() = NULL;	
	// 取得物品的数字型属性
	virtual int	GetPropNum(DWORD dwPropID) = NULL;	
	// 取得物品的字符型属性
	virtual LPCSTR GetPropStr(DWORD dwPropID) = NULL;
	// 判断是否被锁定
	virtual BOOL IsLock() = NULL;
	// 设置锁定状态
	virtual void SetLock(BOOL bLock) = NULL;
	// 获取所在容器指针
	virtual IContainer* GetContainer() = NULL;
	// 设置所在容器指针
	virtual void SetContainer(IContainer* pContainer) = NULL;
	// 获取在容器中的位置
	virtual int GetLocation() = NULL;
	// 设置在容器中的位置
	virtual void SetLocation(int nLocation) = NULL;
	// 获取物品类型
	virtual DWORD GetType() = NULL;
};
//------------------------------------------------------------------------
struct IContainer 
{
	// 绑定输出消息窗口ID
	virtual void BindWnd(DWORD dwWndID) = NULL;
	// 取消绑定
	virtual void UnBindWnd(DWORD dwWndID) = NULL;

	virtual void Release() = NULL;

	// 获取容器总大小
	virtual int GetSize() = NULL;
	// 根据uid获取容器物品
	virtual IContainerGoods* GetGoods(UID uidGoods) = NULL;
	// 根据位置获取物品
	virtual IContainerGoods* GetGoods(int nLocation) = NULL;
	
	// 容器是否允许添加物品
	virtual BOOL CanAdd() = NULL;
	// 判断目标位置是否允许添加指定物品
	virtual BOOL CanAdd(IContainerGoods* pGoods, int nTargetLocation) = NULL;
	// 容器是否允许移出物品
	virtual BOOL CanRemove() = NULL;
	// 判断源位置是否允许移出该物品
	virtual BOOL CanRemove(IContainerGoods* pGoods, int nSrcLocation) = NULL;
	// 提交到服务端,请求添加物品,nTargetLocation = -1代表自动寻找空位
	virtual void AddGoods(IContainerGoods* pGoods, int nTargetLocation, int nNumber) = NULL;	
	// 提交到服务器， 将源容器的物品都添加到本容器
	virtual void AddAllGoodsFromContainer(IContainer *pSrcContainer) = NULL;
	
	//提交到服务端,请求销毁物品
	virtual void DestroyGoods(IContainerGoods *pGoods) = NULL;
	// 获取ID
	virtual DWORD GetID() = NULL;
	// 获取存在物品个数
	virtual DWORD GetGoodsNumber() = NULL;

	// 更新容器物品
	virtual void OnUpdate(LPCSTR buf, int nLen) = NULL;
	// 获取容器类型
	virtual DWORD GetType() = NULL;
	// 打开
	virtual void Open() = NULL;
	// 关闭，不会释放
	virtual void Close() = NULL;
	// 使用指定位置的物品
	virtual BOOL UseGoods(int nLocation) = NULL;
};
//------------------------------------------------------------------------
//容器创建现场
struct SContainerCreateContext
{
	DWORD dwID;
	int	nMaxSize;
	BOOL bCanAdd;
	BOOL bCanRemove;
	DWORD dwType;
	SContainerCreateContext()
	{
		dwID = 0;
		nMaxSize = 0;
		bCanAdd = TRUE;
		bCanRemove = TRUE;
		dwType = 0;
	}
};
//------------------------------------------------------------------------
//容器物品创建现场
struct SGoodsCreateContext
{
	DWORD dwType;		//物品类型
	UID uidGoods;		//物品UID
	BOOL bLock;			//是否锁定
	DWORD dwNumber;		//物品数目
	SGoodsCreateContext()
	{
		dwType = 0;
		uidGoods = INVALID_UID;
		bLock = FALSE;
		dwNumber = 0;
	}
};
//------------------------------------------------------------------------
struct IContainerMgr
{
	virtual void Release() = NULL;
	// 根据现场创建容器
	virtual IContainer* CreateContainer(LPCSTR buf, int nLen) = NULL;
	// 根据容器ID获取容器指针
	virtual IContainer* GetContainer(DWORD dwContainerID) = NULL;
	// 释放容器对象ID
	virtual void ReleaseContainerID(DWORD dwContainerID) = NULL;
	// 根据现场创建容器物品
	virtual IContainerGoods* CreateGoods(LPCSTR buf, int nLen) = NULL;
};
//------------------------------------------------------------------------
struct IClientGlobal;
//------------------------------------------------------------------------
class CContainerMgrHelper
{
private:
	typedef BOOL (*ProcCreateContainerMgr)(IContainerMgr **ppContainerMgr,
		IClientGlobal *pClientGlobal);	
public:
	CContainerMgrHelper()
	{
		m_hDll = NULL;
		m_pContainerMgr = NULL;
		m_szErrMsg[0] = 0;
	}
	~CContainerMgrHelper()
	{		
		Close();
		if (m_hDll != NULL)
		{
			::FreeLibrary(m_hDll);
			m_hDll=NULL;
		}
	}	
	// 返回值：BOOL - 创建成功则返回TRUE,否则返回FALSE
	BOOL Create(IClientGlobal *pClientGlobal)//创建接口对象
	{	
		Close(); 
		try
		{
			if (m_hDll == NULL)
			{
#ifdef _DEBUG
				//第一次调用时未加载dll,下面进行加载dll
				m_hDll = ::LoadLibrary("FKContainerClient_D.dll");
				if (m_hDll == NULL)
					throw "无法加载 FKContainerClient_D.dll";
#else
				//第一次调用时未加载dll,下面进行加载dll
				m_hDll = ::LoadLibrary("FKContainerClient.dll");
				if (m_hDll == NULL)
					throw "无法加载 FKContainerClient.dll";
#endif
			}
			
			//下面获得dll的输出函数，即接口对象的创建函数
			ProcCreateContainerMgr proc = NULL;
			proc=(ProcCreateContainerMgr)::GetProcAddress(m_hDll,
				"CreateContainerMgr");
			if (proc == NULL)
				throw "Can't GetProcAddress('CreateContainerMgr')";
			if (!proc(&m_pContainerMgr, pClientGlobal))
				throw "CreateContainerMgr() error!";
			
			return TRUE;//接口对象创建成功
		}
		catch (LPCSTR szMsg) 
		{
			lstrcpyn(m_szErrMsg, szMsg, sizeof(m_szErrMsg));
			return FALSE;
		}
		catch(...)
		{
			lstrcpyn(m_szErrMsg, "Unknown Error!", sizeof(m_szErrMsg));
			return FALSE;
		}
	}
	void Close()
	{
		if (m_pContainerMgr != NULL)
		{
			m_pContainerMgr->Release();
			m_pContainerMgr = NULL;
		}		
	}
	//重载->,将helper对象的引用直接转为接口的引用
	IContainerMgr* operator ->()
	{
		return m_pContainerMgr;
	}
	IContainerMgr* GetContainerMgr()
	{
		return m_pContainerMgr;
	}
	//判断接口指针是否有效
	BOOL IsValid()
	{
		return (m_pContainerMgr != NULL);
	}
	char* GetErrMsg()
	{
		return m_szErrMsg;
	}	
private:
	IContainerMgr *m_pContainerMgr;						//接口对象
	HINSTANCE m_hDll;									//动态链接库加载与否标志
	char m_szErrMsg[128];								//错误信息
};
//------------------------------------------------------------------------