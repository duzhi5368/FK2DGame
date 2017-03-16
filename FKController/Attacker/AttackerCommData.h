/**
*	created:		2012-6-24   22:29
*	filename: 		AttackerCommData
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		游戏主线程与攻击线程通信的结构
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
class AttackerCommData  
{
protected:
	CRITICAL_SECTION	m_section;
	BOOL				m_bThreadWorking;   //标识是否有线程在工作
	BOOL				m_bStopQuest;       //是否有停止请求
public:
	AttackerCommData();
	virtual ~AttackerCommData();
	AttackerCommData(const AttackerCommData& other);
public:
	BOOL IsThreadWorking(void);
	void SetThreadWorking(BOOL bFlag);
	BOOL HasStopQuest(void);
	void SetStopQuest(BOOL bFlag);
};
//------------------------------------------------------------------------