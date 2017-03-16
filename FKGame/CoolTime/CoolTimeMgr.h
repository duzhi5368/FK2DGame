/**
*	created:		2012-6-29   15:41
*	filename: 		CoolTimeMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		冷却时间管理器，外部接口，提供冷却时间的基本操作
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../SceneObj/IThingPart.h"
//------------------------------------------------------------------------
//冷却时间观察者，当监听的操作开始冷却时收到通知，
//客户端可以实现这个接口来完成动画的开始播放。
class IColdTimeObserver
{
public:
	// 描  述：冷却开始
	// 参  数：[输入] long lColdTime - 冷却时间长度(毫秒)
	// 返回值：void
	// 注  意：在本函数中不能取消订阅冷却开始事件
	virtual void OnStart(long lColdTime) = NULL;
};
//------------------------------------------------------------------------
class IColdTimeMgr : public IThingPart
{
public:
	// 描  述：查询是否可以进行某个操作
	// 参  数：[输入] long lType  - 操作类型
	// 参  数：[输入] long lValue - 操作值,操作类型中的唯一ID 
	// 返回值：BOOL - 如果可以操作,即没有在冷却,则返回true,否则返回false
	virtual BOOL CanOperate(long lType, long lValue) = NULL;

	// 描  述：进行某个操作,内部将引起冷却开始
	// 参  数：[输入] long lType  - 操作类型
	// 参  数：[输入] long lValue - 操作值,操作类型中的唯一ID 
	// 返回值：BOOL - 如果成功则返回true,否则返回false
	virtual BOOL Operate(long lType, long lValue) = NULL;

	// 描  述：获取某个操作的冷却时间长度(毫秒)
	// 参  数：[输入] long lType  - 操作类型
	// 参  数：[输入] long lValue - 操作值,操作类型中的唯一ID 
	// 返回值：long - 返回该操作的冷却时间长度(毫秒)
	virtual long GetColdTime(long lType, long lValue) = NULL;

	// 描  述：设置某个操作的冷却时间长度(毫秒)
	// 参  数：[输入] long lType  - 操作类型
	// 参  数：[输入] long lValue - 操作值,操作类型中的唯一ID 
	// 参  数：[输入] long lCodeTime - 冷却事件长度(毫秒)
	// 返回值：void
	virtual void SetColdTime(long lType, long lValue, long lCodeTime) = NULL;

	// 描  述：获取某个操作的开始冷却时间
	// 参  数：[输入] long lType  - 操作类型
	// 参  数：[输入] long lValue - 操作值,操作类型中的唯一ID 
	// 返回值：long - 返回对应操作的开始冷却时间
	virtual long GetStartTime(long lType, long lValue) = NULL;

	// 描  述：订阅冷却开始事件
	// 参  数：[输入] long lType  - 操作类型
	// 参  数：[输入] long lValue - 操作值,操作类型中的唯一ID 
	// 参  数：[输入] IColdTimeObserver *pColdTimeObserver - 冷却开始事件监听者 
	// 返回值：void
	virtual void Subscibe(long lType, long lValue,
		IColdTimeObserver *pColdTimeObserver) = NULL;

	// 描  述：取消订阅冷却开始事件
	// 参  数：[输入] long lType  - 操作类型
	// 参  数：[输入] long lValue - 操作值,操作类型中的唯一ID 
	// 参  数：[输入] IColdTimeObserver *pColdTimeObserver - 冷却开始事件监听者 
	// 返回值：void
	virtual void Unsubscibe(long lType, long lValue,
		IColdTimeObserver *pColdTimeObserver) = NULL;

	virtual void Release() = NULL;
	
	virtual void SetData(LPCSTR buf, int nLen) = NULL;
	virtual BOOL GetData(LPSTR buf, int &nLen) = NULL;
};
//------------------------------------------------------------------------
struct IColdTimeSystem
{
	virtual void Release() = NULL;
	
	// 描述: 创建一个冷却管理器
	virtual IColdTimeMgr* CreateColdTimeMgr() = NULL;
	// 描  述：获取某个操作的冷却时间长度(毫秒)
	// 参  数：[输入] long lType  - 操作类型
	// 参  数：[输入] long lValue - 操作值,操作类型中的唯一ID 
	// 返回值：long - 返回该操作的冷却时间长度(毫秒)
	virtual long GetColdTime(long lType, long lValue) = NULL;
};
//------------------------------------------------------------------------
struct ITrace;
struct IClientGlobal;
struct IScriptUpdateManager;
//------------------------------------------------------------------------
class CColdTimeSystemHelper
{
private:
	typedef BOOL (*ProcCreateColdTimeSysObject)(
		IColdTimeSystem **ppColdTimeSys, 
		ITrace *pTrace, IScriptUpdateManager *pSrciptUpdateManager,
		LPCSTR szFile, IClientGlobal *pClientGlobal);
	
public:
	CColdTimeSystemHelper()
	{
		m_hDll = NULL;
		m_pColdTimeSys = NULL;
		m_szErrMsg[0] = 0;
	}
	~CColdTimeSystemHelper()
	{
		Close();
		if (m_hDll != NULL)//注意，如果是lib方式，由于m_hDll总是NULL,所以下面的::FreeLibary()不会被调用
		{
			::FreeLibrary(m_hDll);
			m_hDll=NULL;
		}
	}	
	//文件名如果为空，则取默认文件名
	BOOL Create(ITrace *pTrace, IScriptUpdateManager *pSrciptUpdateManager,
		LPCSTR szFile = NULL, IClientGlobal *pClientGlobal = NULL)
	{
		Close();
		try
		{
			if (m_hDll == NULL)
			{
				//第一次调用时未加载dll,下面进行加载dll
#ifdef _DEBUG
				m_hDll = ::LoadLibrary("FKColdTime_D.dll");
				if (m_hDll == NULL)
					throw "无法加载 FKColdTime_D.dll";
#else
				m_hDll = ::LoadLibrary("FKColdTime.dll");
				if (m_hDll == NULL)
					throw "无法加载 FKColdTime.dll";
#endif
			}
			//下面获得dll的输出函数，即接口对象的创建函数
			ProcCreateColdTimeSysObject proc = NULL;
			proc=(ProcCreateColdTimeSysObject)::GetProcAddress(m_hDll,
				"CreateColdTimeSysObject");
			if (proc == NULL)
				throw "Can't GetProcAddress('CreateColdTimeSysObject')";
			if (!proc(&m_pColdTimeSys, pTrace, pSrciptUpdateManager, szFile, pClientGlobal))
				throw "CreateColdTimeSysObject() error!";
			
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
	void Close()//销毁接口对象
	{
		if (m_pColdTimeSys != NULL)
		{
			m_pColdTimeSys->Release();
			m_pColdTimeSys = NULL;			
		}
	}
	//重载->,将helper对象的引用直接转为接口的引用
	IColdTimeSystem* operator ->()
	{
		return m_pColdTimeSys;
	}
	//判断接口指针是否有效
	BOOL IsValid()
	{
		return (m_pColdTimeSys != NULL);
	}
	char* GetErrMsg()
	{
		return m_szErrMsg;
	}
	IColdTimeSystem* GetInterface()
	{
		return m_pColdTimeSys;
	}
private:
	IColdTimeSystem *m_pColdTimeSys;					//接口对象
	HINSTANCE m_hDll;									//动态链接库加载与否标志
	char m_szErrMsg[128];								//错误信息
};
//------------------------------------------------------------------------
