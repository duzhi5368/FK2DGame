/**
*	created:		2012-6-24   2:57
*	filename: 		TimeAxis
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/TimeAxis.h"
#include "../Include/TimeAxisAux.h"
//------------------------------------------------------------------------
#ifndef TRACE
#define TRACE	OutputDebugString	
#endif
//------------------------------------------------------------------------
DWORD g_dwTimeLine;
//------------------------------------------------------------------------
CTimeAxis::CTimeAxis()//:	m_TimerPosTable(65536,std::tr1::hash<TIMERKEY>(),std::equal_to<TIMERKEY>())
{
	//if(sizeof(SHORT_TIMER_ITERATOR)>sizeof(LONG_TIMER_ITERATOR))
	//{
	//	((CTimeAxis *)(NULL))->m_nCurShortTimer=0;
	//}

	m_bInitTimerList=FALSE;
	m_dwLastCheckTime=0;

	InitTimerList();
}
//------------------------------------------------------------------------
CTimeAxis::~CTimeAxis()
{
}
//------------------------------------------------------------------------
DWORD CTimeAxis::GetTimeTick()
{
	return  GET_TICK_COUNT;
}
//------------------------------------------------------------------------
BOOL CTimeAxis::_SetTimer(DWORD dwEventID, ITimerSink * pTimerSink, DWORD dwElapse)
{
	TIMERDATA TimerData;
	TimerData.dwElapseTime=dwElapse;
	TimerData.dwLastTickCount=GET_TICK_COUNT;
	TimerData.TimerKey.dwEventID=dwEventID;
	TimerData.TimerKey.pTimerSink=pTimerSink;

	//检查是否已经有该定时器的存在
	//HASHTABLE_ITERATOR hie;
	//hie=m_TimerPosTable.find(TimerData.TimerKey);
	//if(hie._M_cur)
	//{
	//	return FALSE;
	//}
	//DWORD dwNextTickCount=TimerData.dwLastTickCount+TimerData.dwElapseTime;
	//if(dwElapse/TIMER_GIID>=TIMER_LONG_ELAPSE)		//属于长时间间隔的TIMER
	//{
	//	g_dwTimeLine=GET_TICK_COUNT;
	//	pair<LONG_TIMER_ITERATOR,bool> pairLong=m_LongElapseTimerList.insert(TimerData);
	//	if(!pairLong.second)
	//	{
	//		TRACE("Error at CTimeAxis::SetTimer,m_LongElapseTimerList.insert return false");
	//		return FALSE;
	//	}
	//	TIMERPOS TimerPos;
	//	TimerPos.bIsShort=FALSE;
	//	*((LONG_TIMER_ITERATOR *)TimerPos.PosBuf)=pairLong.first;
	//	TimerPos.TimerKey=TimerData.TimerKey;
	//	m_TimerPosTable.insert_unique(TimerPos);
	//}
	//else
	//{
	//	TIMERPOS TimerPos;
	//	int nNextTimer=(m_nCurShortTimer+dwElapse/TIMER_GIID)%TIMER_LONG_ELAPSE;
	//	if((GET_TICK_COUNT+dwElapse)-m_dwShortArrayLimit[nNextTimer]>TIMER_GIID)
	//	{
	//		int nShortIndex=(nNextTimer+1)%TIMER_LONG_ELAPSE;
	//		*((SHORT_TIMER_ITERATOR *)TimerPos.PosBuf)=m_ShortElapseTimerArray[nShortIndex].insert(m_ShortElapseTimerArray[nShortIndex].end(),TimerData);
	//		m_dwShortTimerCount[nShortIndex]++;
	//		TimerPos.dwShortTimerIndex=nShortIndex;
	//	}
	//	else
	//	{
	//		*((SHORT_TIMER_ITERATOR *)TimerPos.PosBuf)=m_ShortElapseTimerArray[nNextTimer].insert(m_ShortElapseTimerArray[nNextTimer].end(),TimerData);
	//		m_dwShortTimerCount[nNextTimer]++;
	//		TimerPos.dwShortTimerIndex=nNextTimer;
	//	}
	//	TimerPos.bIsShort=TRUE;
	//	TimerPos.TimerKey=TimerData.TimerKey;
	//	m_TimerPosTable.insert_unique(TimerPos);
	//}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CTimeAxis::SetTimer(DWORD dwEventID, ITimerSink * pTimerSink, DWORD dwElapse, LPCSTR szTimerDesc)
{
#ifndef _DEBUG
	return _SetTimer(dwEventID, pTimerSink, dwElapse);
#else
	TIMERDATA TimerData;
	TimerData.dwElapseTime=dwElapse;
	TimerData.dwLastTickCount=GET_TICK_COUNT;
	TimerData.TimerKey.dwEventID=dwEventID;
	TimerData.TimerKey.pTimerSink=pTimerSink;

	if(szTimerDesc != NULL)
	{
		memcpy(TimerData.TimerKey.szTimerDesc, szTimerDesc, TIMER_DESC_LEN);
	}

	TimerData.TimerKey.szTimerDesc[TIMER_DESC_LEN-1]=0;

	//检查是否已经有该定时器的存在
	//HASHTABLE_ITERATOR hie;
	//hie=m_TimerPosTable.find(TimerData.TimerKey);
	//if(hie._M_cur)
	//{
	//	return FALSE;
	//}
	//DWORD dwNextTickCount=TimerData.dwLastTickCount+TimerData.dwElapseTime;
	//if(dwElapse/TIMER_GIID>=TIMER_LONG_ELAPSE)		//属于长时间间隔的TIMER
	//{
	//	g_dwTimeLine=GET_TICK_COUNT;
	//	pair<LONG_TIMER_ITERATOR,bool> pairLong=m_LongElapseTimerList.insert(TimerData);
	//	if(!pairLong.second)
	//	{
	//		TRACE("Error at CTimeAxis::SetTimer,m_LongElapseTimerList.insert return false");
	//		return FALSE;
	//	}
	//	TIMERPOS TimerPos;
	//	TimerPos.bIsShort=FALSE;
	//	*((LONG_TIMER_ITERATOR *)TimerPos.PosBuf)=pairLong.first;
	//	TimerPos.TimerKey=TimerData.TimerKey;
	//	m_TimerPosTable.insert_unique(TimerPos);
	//}
	//else
	//{
	//	TIMERPOS TimerPos;
	//	int nNextTimer=(m_nCurShortTimer+dwElapse/TIMER_GIID)%TIMER_LONG_ELAPSE;
	//	if((GET_TICK_COUNT+dwElapse)-m_dwShortArrayLimit[nNextTimer]>TIMER_GIID)
	//	{
	//		int nShortIndex=(nNextTimer+1)%TIMER_LONG_ELAPSE;
	//		*((SHORT_TIMER_ITERATOR *)TimerPos.PosBuf)=m_ShortElapseTimerArray[nShortIndex].insert(m_ShortElapseTimerArray[nShortIndex].end(),TimerData);
	//		m_dwShortTimerCount[nShortIndex]++;
	//		TimerPos.dwShortTimerIndex=nShortIndex;
	//	}
	//	else
	//	{
	//		*((SHORT_TIMER_ITERATOR *)TimerPos.PosBuf)=m_ShortElapseTimerArray[nNextTimer].insert(m_ShortElapseTimerArray[nNextTimer].end(),TimerData);
	//		m_dwShortTimerCount[nNextTimer]++;
	//		TimerPos.dwShortTimerIndex=nNextTimer;
	//	}
	//	TimerPos.bIsShort=TRUE;
	//	TimerPos.TimerKey=TimerData.TimerKey;
	//	m_TimerPosTable.insert_unique(TimerPos);
	//}
	return TRUE;

#endif
}
//------------------------------------------------------------------------
void CTimeAxis::KillTimer(DWORD dwEventID,ITimerSink *pTimerSink)
{
	TIMERKEY TimerKey;
	TimerKey.dwEventID=dwEventID;
	TimerKey.pTimerSink=pTimerSink;

	
	//HASHTABLE_ITERATOR hie;
	//hie=m_TimerPosTable.find(TimerKey);
	//if(!hie._M_cur)
	//{
	//	TRACE("Kill Timer can't find The Timer\r\n");
	//	return;
	//}
	//TIMERPOS TimerPos=*hie;
	//if(TimerPos.bIsShort)
	//{
	//	(*(*((SHORT_TIMER_ITERATOR *)TimerPos.PosBuf))).TimerKey.pTimerSink=NULL;
	//	m_dwShortTimerVaildCount[TimerPos.dwShortTimerIndex]--;

	//}
	//else
	//{
	//	m_LongElapseTimerList.erase(*(LONG_TIMER_ITERATOR *)TimerPos.PosBuf);
	//}
	//m_TimerPosTable.erase(TimerKey);
}
//------------------------------------------------------------------------
void CTimeAxis::InitTimerList()
{
	m_nCurShortTimer=0;
	DWORD dwTickCount=GET_TICK_COUNT;
	m_dwLastCheckTime = dwTickCount;
	for(int i=0;i<TIMER_LONG_ELAPSE;i++)
	{
		m_dwShortArrayLimit[i]=dwTickCount;
		dwTickCount+=TIMER_GIID;
	}
	memset(m_dwShortTimerCount,0,sizeof(m_dwShortTimerCount));
	memset(m_dwShortTimerCount,0,sizeof(m_dwShortTimerCount));
}
//------------------------------------------------------------------------
void CTimeAxis::OnCheck()
{
	DWORD dwTickCount=GET_TICK_COUNT;
	while( dwTickCount - m_dwLastCheckTime > TIMER_UNIT )
	{
		m_dwLastCheckTime += TIMER_UNIT;
		OnCheckAux( m_dwLastCheckTime );
	}

	m_dwLastCheckTime = dwTickCount;
	OnCheckAux( m_dwLastCheckTime );
}
//------------------------------------------------------------------------
void CTimeAxis::OnCheckAux( DWORD dwTickCount )
{
	TIMERDATA TimerData;

	DWORD dwTickCountShort = GetTickCount();
	//检查短时间器队列
	int nCount;
	SHORT_TIMER_ITERATOR iteShort;
	nCount=m_dwShortTimerCount[m_nCurShortTimer];
	iteShort=m_ShortElapseTimerArray[m_nCurShortTimer].begin();
	int nEmptyCount = 0;
	for(int i=0;i<nCount;i++)
	{
		TimerData=*(iteShort);
		//如果该时间对象已经被删除，不管，直接到下一个
		if(!TimerData.TimerKey.pTimerSink)
		{
			++iteShort;
			++nEmptyCount;
			continue;
		}
		//计算时间已经超时，触发
		if(TimerData.IsOverTime(dwTickCount))
		{
			SHORT_TIMER_ITERATOR iteTemp;
			TIMERPOS TimerPos;
			iteTemp=iteShort;
			++iteTemp;
			m_ShortElapseTimerArray[m_nCurShortTimer].erase(iteShort);
			m_dwShortTimerCount[m_nCurShortTimer]--;
			m_dwShortTimerVaildCount[m_nCurShortTimer]--;
			iteShort=iteTemp;
			TimerData.dwLastTickCount=dwTickCount;
			int nNextTimer;
			nNextTimer=(m_nCurShortTimer+TimerData.dwElapseTime/TIMER_GIID)%TIMER_LONG_ELAPSE;
			if((dwTickCount+TimerData.dwElapseTime)-m_dwShortArrayLimit[nNextTimer]>TIMER_GIID)
			{
				int nShortIndex=(nNextTimer+1)%TIMER_LONG_ELAPSE;
				*((SHORT_TIMER_ITERATOR *)TimerPos.PosBuf)=m_ShortElapseTimerArray[nShortIndex].insert(m_ShortElapseTimerArray[nShortIndex].end(),TimerData);
				m_dwShortTimerCount[nShortIndex]++;
				m_dwShortTimerVaildCount[nShortIndex]++;
				TimerPos.dwShortTimerIndex=nShortIndex;
			}
			else
			{
				*((SHORT_TIMER_ITERATOR *)TimerPos.PosBuf)=m_ShortElapseTimerArray[nNextTimer].insert(m_ShortElapseTimerArray[nNextTimer].end(),TimerData);
				m_dwShortTimerCount[nNextTimer]++;
				m_dwShortTimerVaildCount[nNextTimer]++;
				TimerPos.dwShortTimerIndex=nNextTimer;
			}
			TimerPos.bIsShort=TRUE;
			TimerPos.TimerKey=TimerData.TimerKey;
			//HASHTABLE_ITERATOR hie;
			//hie=m_TimerPosTable.find(TimerData.TimerKey);
			//if(hie._M_cur)
			//{
			//	*(hie)=TimerPos;
			//}
			//else
			//{
			//	TRACE("warning at OnCheck,can't find Timepos");
			//	m_TimerPosTable.insert_unique(TimerPos);
			//}
			TimerData.TimerKey.pTimerSink->OnTimer(TimerData.TimerKey.dwEventID);
		}
		else
		{
			++iteShort;
		}
	}
	//检查该时间队列是否已经有了被删除的废弃对象
	if(nEmptyCount/*m_dwShortTimerCount[m_nCurShortTimer]!=m_dwShortTimerVaildCount[m_nCurShortTimer]*/)
	{
		nCount=m_dwShortTimerCount[m_nCurShortTimer];
		iteShort=m_ShortElapseTimerArray[m_nCurShortTimer].begin();
		for(int i=0;i<nCount;i++)
		{
			TimerData=*(iteShort);
			if(TimerData.TimerKey.pTimerSink==NULL)
			{
				SHORT_TIMER_ITERATOR iteTemp;
				iteTemp=iteShort;
				++iteTemp;
				m_ShortElapseTimerArray[m_nCurShortTimer].erase(iteShort);
				m_dwShortTimerCount[m_nCurShortTimer]--;
				if(--nEmptyCount <= 0 )
					break;
				iteShort=iteTemp;
			}
			else
			{
				++iteShort;
			}
		}
	}
	m_dwShortTimerVaildCount[m_nCurShortTimer] = m_dwShortTimerCount[m_nCurShortTimer];

	// 递进到下一个时间间隔
	if(dwTickCount-m_dwShortArrayLimit[m_nCurShortTimer]>TIMER_GIID)
	{
		int nNextTimer=(m_nCurShortTimer+1)%TIMER_LONG_ELAPSE;
		nCount=m_dwShortTimerCount[m_nCurShortTimer];
		iteShort=m_ShortElapseTimerArray[m_nCurShortTimer].begin();
		for(int i=0;i<nCount;i++)
		{
			TIMERPOS TimerPos;
			TimerPos.bIsShort=TRUE;
			TimerPos.dwShortTimerIndex=nNextTimer;
			TimerData=*(iteShort);
			TimerPos.TimerKey=TimerData.TimerKey;
			SHORT_TIMER_ITERATOR iteTemp;
			iteTemp=iteShort;
			++iteTemp;
			m_ShortElapseTimerArray[m_nCurShortTimer].erase(iteShort);
			iteShort=iteTemp;
			m_dwShortTimerCount[m_nCurShortTimer]--;
			m_dwShortTimerVaildCount[m_nCurShortTimer]--;
			*((SHORT_TIMER_ITERATOR *)TimerPos.PosBuf)=m_ShortElapseTimerArray[nNextTimer].insert(m_ShortElapseTimerArray[nNextTimer].end(),TimerData);
			m_dwShortTimerCount[nNextTimer]++;
			m_dwShortTimerVaildCount[nNextTimer]++;
			//HASHTABLE_ITERATOR hie;
			//hie=m_TimerPosTable.find(TimerData.TimerKey);
			//if(hie._M_cur)
			//{
			//	*(hie)=TimerPos;
			//}
			//else
			//{
			//	m_TimerPosTable.insert_unique(TimerPos);
			//}
		}
		m_dwShortArrayLimit[m_nCurShortTimer]+=TIMER_LONG_ELAPSE*TIMER_GIID;
		m_nCurShortTimer=nNextTimer;
	}

	//检查长时间器队列
	//for(;!m_LongElapseTimerList.empty();)
	//{
	//	LONG_TIMER_ITERATOR iteLong=m_LongElapseTimerList.begin();
	//	TimerData=*(iteLong);

	//	if(TimerData.IsOverTime(dwTickCount))
	//	{
	//		m_LongElapseTimerList.erase(iteLong);
	//		TimerData.dwLastTickCount=dwTickCount;
	//		pair<LONG_TIMER_ITERATOR,bool> pairLong=m_LongElapseTimerList.insert(TimerData);
	//		if(!pairLong.second)
	//		{
	//			TRACE("Error at CTimeAxis::OnCheck,m_LongElapseTimerList.insert return false");
	//			break;
	//		}
	//		TIMERPOS TimerPos;
	//		TimerPos.bIsShort=FALSE;
	//		*((LONG_TIMER_ITERATOR *)TimerPos.PosBuf)=pairLong.first;
	//		TimerPos.TimerKey=TimerData.TimerKey;
	//		HASHTABLE_ITERATOR hie;
	//		hie=m_TimerPosTable.find(TimerData.TimerKey);
	//		if(hie._M_cur)
	//		{
	//			*(hie)=TimerPos;
	//		}
	//		else
	//		{
	//			m_TimerPosTable.insert_unique(TimerPos);
	//		}

	//		TimerData.TimerKey.pTimerSink->OnTimer(TimerData.TimerKey.dwEventID);
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
}
//------------------------------------------------------------------------
void CTimeAxis::Release()
{
	delete this;
}
//------------------------------------------------------------------------