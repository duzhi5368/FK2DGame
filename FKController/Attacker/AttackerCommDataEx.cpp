/**
*	created:		2012-6-24   22:33
*	filename: 		AttackerCommDataEx
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "AttackerCommDataEx.h"
//------------------------------------------------------------------------
CAttackerCommDataEx::CAttackerCommDataEx()
{
	m_nThread = 0;
	m_nAttackIndex = 0;
}
//------------------------------------------------------------------------
CAttackerCommDataEx::~CAttackerCommDataEx()
{

}
//------------------------------------------------------------------------
BOOL CAttackerCommDataEx::AddSite(const char *pData)
{
	SITEDATA sitedata;
	char temp[512];	
	sscanf(pData, "[%d,%d]%s", &m_nThread, &(sitedata.interval), temp); 
	sitedata.strURL = temp;
	sitedata.lasttime = 0;
	if (sitedata.interval < 1000)
	{
		sitedata.interval = 1000;
	}

	m_vtSiteData.push_back(sitedata);
	return TRUE;
}
//------------------------------------------------------------------------
LPCSTR CAttackerCommDataEx::GetAvalibleSite(void)
{
	LPCSTR szResult = NULL;
	if (m_vtSiteData.empty())
	{
		return szResult; 
	}

	long lCurrentTime = GetTickCount();

	int nVTSize = m_vtSiteData.size();
	for (int i = 0; i < nVTSize; ++i)
	{
		m_nAttackIndex = (m_nAttackIndex + 1) % nVTSize;
		if (lCurrentTime - m_vtSiteData[m_nAttackIndex].lasttime > m_vtSiteData[m_nAttackIndex].interval)
		{
			m_vtSiteData[m_nAttackIndex].lasttime = lCurrentTime;
			szResult = m_vtSiteData[m_nAttackIndex].strURL.c_str();
			break;
		}	
	}
	return szResult;
}
//------------------------------------------------------------------------
int CAttackerCommDataEx::GetThreadID(void)const
{
	return m_nThread;
}
//------------------------------------------------------------------------