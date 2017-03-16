/**
*	created:		2012-6-28   9:18
*	filename: 		TitleController
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKGame/Event/IEvent.h"
#include "../../FKGame/Controller/IController.h"
//------------------------------------------------------------------------
typedef __int64 UID;
//------------------------------------------------------------------------
class CTitleController:public IEventActionListener,
	public IDataReceiver
{
public:
	CTitleController();
	virtual ~CTitleController();
public:
	virtual void OnAction(IEvent *pEvent);

	virtual BOOL OnReceived(const char* pData, int nLen);
	// 获取称号
	void GetTitle();

	// 使用称号
	void UserTitle(long lTitleId);

	// 不使用称号
	void UnUseTitle(long lTitleId);

	// 删除称号
	void DeleteTitle(long lTitleId);

	// 获取其它人的称号
	void GetOtherTitle(UID uid);

	// 获取称号返回
	void GetTitleResponse(LPCSTR pBuf,int nLen);

	// 使用称号返回
	void UseTitleResponse(LPCSTR pBuf,int nLen);

	// 不使用称号
	void UnUseTitleResponse(LPCSTR pBuf,int nLen);

	//删除称号响应
	void DeleteTitleResponse(LPCSTR pBuf,int nLen);

	// 获取到某个玩家的称号
	void GetOtherTitleResponse(LPCSTR pBuf,int nLen);

	// 状态改变通知
	void TitleStatusChange(LPCSTR pBuf,int nLen);

	// 增加一个称号
	void AddTitleResponse(LPCSTR pBuf,int nLen);

};
//------------------------------------------------------------------------