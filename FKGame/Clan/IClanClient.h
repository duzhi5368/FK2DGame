/**
*	created:		2012-6-29   14:07
*	filename: 		IClanClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
/*
 * 
 * 客户端氏族系统接口文件,包括两接口
 * 1.氏族头顶部件:
 *     挂在玩家身上,保存玩家的称号,所属氏族和氏族Icon信息
 * 2.客户端氏族系统接口
 *     用于获取氏族系统的相关操作
 *
 * 请将每次的修改记录说明于下:
 */
//------------------------------------------------------------------------
#include <Windows.h>
#include "../../FKGame/SceneObj/IThingPart.h"
//------------------------------------------------------------------------
#define CLAN_CLIENT_VERSION  1  
//判断是否是联盟ID
#define IS_UNIONID(_x)				(((unsigned long)_x) >= 0x100000)
//------------------------------------------------------------------------
struct IActor;
// 好友模块的助手类
struct IClientGlobal;
//------------------------------------------------------------------------
//氏族头顶部件,
struct IClanClientPate : public IThingPart
{
	//描  述：获得氏族的名称，如果没有氏族返回 ""
	virtual const char *GetClanName() = 0;

	//描  述：是否是氏族长
	virtual bool IsClanShaikh() = 0;

	//描  述：是有氏族关系
	virtual bool IsHaveClan() = 0;

	//氏族战中判断是否是敌对的氏族
	virtual bool IsMyOpposedClan(long lClanID) = 0;

	//更新成战中的敌对氏族
	virtual bool UpdateMyOpposedClan(const BYTE* pData,int nLen) = 0;

	//是否能攻击
	virtual bool IsAttackCamp(UID uid) = 0;

	//取出玩家身上的城战联盟ID,如果返回0表示没有联盟ID
	virtual int  GetSiegeCmapId() = 0;

	//判断是否是城主联盟
	virtual bool IsCastellanCamp() = 0;

	//是否是城主氏族
	virtual bool IsCastellanClan(long lCastleId) = 0;
};
//------------------------------------------------------------------------
//客户端氏族系统
struct IClanClient
{
	virtual void Release(void)    = 0;

	//描 述: 打开氏族系统面板
	virtual void OpenClanSysPanel(void) = 0;

	//描 述: 获取氏族头顶部件 
	virtual IClanClientPate* GetClanPatePart(void) = 0;

	//用于释放氏族的窗口
	virtual void ReleaseClanWnd(void) = 0;
};
//------------------------------------------------------------------------
class ClanClientHelper
{
	typedef BOOL (*getClanClientInstance)( DWORD dwVersion
										  ,IClientGlobal * pGlobal
										  ,IClanClient ** ppInst);

	HINSTANCE     m_hDll;
	IClanClient * m_pInstance;
public:
	ClanClientHelper()
		:m_hDll(0),m_pInstance(0)
	{}
	~ClanClientHelper()
	{
		Close();
	}

	void Close()
	{
		if(m_pInstance)
		{
			m_pInstance->Release();
			m_pInstance = 0;
		}
		if(m_hDll)
		{
			FreeLibrary(m_hDll);
			m_hDll = 0;
		}
			
	}

	IClanClient *GetModule(void) const
	{
		return m_pInstance;
	}

	IClanClient *operator ->(void) const
	{
		return m_pInstance;
	}

	BOOL Create(IClientGlobal *pGlobal)
	{
		if(m_pInstance)
			return TRUE;

		try
		{
#ifdef _DEBUG
			m_hDll = LoadLibrary("FKClanClient_D.dll");
			if(!m_hDll)
			{
				throw "无法加载 FKClanClient_D.dll!";
			}
#else
			m_hDll = LoadLibrary("FKClanClient.dll");
			if(!m_hDll)
			{
				throw "无法加载 FKClanClient.dll!";
			}
#endif
			
			getClanClientInstance pFun = (getClanClientInstance) GetProcAddress(m_hDll,"getClanClientInstance");
			if(!pFun)
			{
				throw "Not Find getClanClientInstance !";
			}
			
			if(!pFun(CLAN_CLIENT_VERSION,pGlobal,&m_pInstance))
			{
				throw "getClanClientInstance() Failed!";
			}
			return (m_pInstance != NULL);
			
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
		}
		catch(...)
		{
			OutputDebugString("getClanClientInstance() catch error\r\n");
		}
		return FALSE;
	}
	
};
//------------------------------------------------------------------------