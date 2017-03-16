/**
*	created:		2012-7-2   3:57
*	filename: 		ActorPreView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		角色预显示类
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "AniGroup.h"
#include "Resloader.h"
#include "../../FKGame/Timer/ITimerAxis.h"
#include "../../FKGame/Common/CreatureViewDef.h"
#include "Pal.h"
//------------------------------------------------------------------------
class CActPreView : public ITimerSink
{
public:
	CActPreView();
	virtual ~CActPreView();
	
	//更换图包
	BOOL ChangeModule (int nIndex, DWORD dwResID, BYTE nPal, BYTE nAlpha);
	CAniGroup* RequestRes(int nModuleType);
	void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag);
	virtual void OnTimer(DWORD dwEventID);
	void CreatePreView (int nType)
	{
		m_AniContext.nType = nType;
		m_nDelay = g_ResLoader.GetFrameDelay(nType, m_AniContext.nState);
		if (m_nDelay > 0)
		{
			g_pTimeAxis->SetTimer(0,
				static_cast<ITimerSink*>(this),
				m_nDelay,
				"BasicSys!!CActPreView::CreatePreView");
		}
	}
	void SetState (int nState);

	//生物本身相关函数，包括状态、动画、帧处理等
	int  GetCurAni(int nFind);			//返回当前是第几个动画，参数：第几个部件

	void ReleasePreView();
public:
	//图包相关
	CAniGroup*			m_pModule[CrtPartMax];				//组件图包
	DWORD				m_dwModuleID[CrtPartMax];			//组件图包ID
	BYTE				m_bytePal[CrtPartMax];				//组件的调色板索引号
	BYTE				m_byteAlpha[CrtPartMax];			//组件的透明度索引号
	CPal				m_Pal[CrtPartMax];					//组件的调色板
	DWORD				m_dwDrawFlag;
	SAniContext			m_AniContext;						// 动画上下文
	BYTE*				m_PartStateFlag[CrtPartMax];		//保存各个部件的状态标志
	int					m_nDelay;							//动画延时
};
//------------------------------------------------------------------------