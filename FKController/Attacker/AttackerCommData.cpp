/**
*	created:		2012-6-24   22:30
*	filename: 		AttackerCommData
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "AttackerCommData.h"
//------------------------------------------------------------------------
AttackerCommData::AttackerCommData()
{
	InitializeCriticalSection(&m_section);		
	m_bThreadWorking = FALSE;
	m_bStopQuest = FALSE;
}
//------------------------------------------------------------------------
AttackerCommData::~AttackerCommData()
{
	DeleteCriticalSection(&m_section);
}
//------------------------------------------------------------------------
AttackerCommData::AttackerCommData(const AttackerCommData& other)
{
	InitializeCriticalSection(&m_section);		
	m_bThreadWorking = other.m_bThreadWorking;
	m_bStopQuest = other.m_bStopQuest;
}
//------------------------------------------------------------------------
BOOL AttackerCommData::IsThreadWorking(void)
{
	BOOL bFlag;
	EnterCriticalSection(&m_section);
	bFlag = m_bThreadWorking;
	LeaveCriticalSection(&m_section);
	return bFlag;	
}
//------------------------------------------------------------------------
void AttackerCommData::SetThreadWorking(BOOL bFlag)
{
	EnterCriticalSection(&m_section);
	m_bThreadWorking = bFlag;
	LeaveCriticalSection(&m_section);
}
//------------------------------------------------------------------------
BOOL AttackerCommData::HasStopQuest(void)
{
	BOOL bFlag;
	EnterCriticalSection(&m_section);
	bFlag = m_bStopQuest;
	LeaveCriticalSection(&m_section);
	return bFlag;
}
//------------------------------------------------------------------------
void AttackerCommData::SetStopQuest(BOOL bFlag)
{
	EnterCriticalSection(&m_section);
	m_bStopQuest = bFlag;
	LeaveCriticalSection(&m_section);
}
//------------------------------------------------------------------------