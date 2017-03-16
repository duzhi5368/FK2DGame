/**
*	created:		2012-6-28   9:10
*	filename: 		SysMsgReceiver
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		系统信息接收器
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "SysMessageDef.h"
#include "../../FKGame/Event/IEvent.h"
#include "../../FKResManager/IFKResManager.h"
#include "../IReminder.h"
//------------------------------------------------------------------------
// 系统信息接收器
class CSysMsgReceiver  : public IEventActionListener
{
public:
	CSysMsgReceiver(void);
	virtual ~CSysMsgReceiver(void);

public:
	void SetPromptOn(void);
	void Reset(void);
	BOOL Init(IResObject * pResObject);
	void ShowMessage(int nType, const char * pszText);
	void Clear(int nType);
	virtual void OnAction(IEvent * pEvent);
	virtual void OnSysMsgDraw(ICanvas * pCanvas);
    
private:
	void OnFloatMessage(const char * pszText);
	void OnAboveMessage(const char * pszText);	
	void OnMiddleMessage(const char * pszText);
	void OnUnderMessage(const char * pszText);
	RECT GetRect(IResObject * pResObject);

private:
	IReminder * m_pFloatReminder;	
	IReminder * m_pAboveReminder;
	IReminder * m_pMiddleReminder;
	IReminder * m_pUnderReminder;
	BOOL m_bPromptOn;
};
//------------------------------------------------------------------------