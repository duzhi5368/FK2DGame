/**
*	created:		2012-6-28   9:35
*	filename: 		TitleMgr
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/TitleMgr.h"
//------------------------------------------------------------------------
CTitleMgr::CTitleMgr()
{

}
//------------------------------------------------------------------------
CTitleMgr::~CTitleMgr()
{

}
//------------------------------------------------------------------------
BOOL CTitleMgr::LoadScript(IResScript *pResScript)
{
	IResObject* pRootObj = pResScript->GetResObjectRoot();
	if (pRootObj == NULL)
	{
		return FALSE;
	}	
	// 先获取3个脚本函数名字

	int nCount = pRootObj->GetSubObjectCount();
	for (int i = 0; i < nCount; ++i)
	{
		IResObject* pTitleRes = pRootObj->GetSubObjectByIndex(i);
		if( pTitleRes!= NULL)
		{
			STitle *pTitle = new STitle;
			pTitle->lTitleId = atoi(pTitleRes->GetPropertyValue(3));  //  称号ID
			pTitle->strName = pTitleRes->GetPropertyValue(4);         //  称号名称
			pTitle->sCanUseTimes = atoi(pTitleRes->GetPropertyValue(5));
			pTitle->lCanUseTime = atoi(pTitleRes->GetPropertyValue(6));
			pTitle->strTitleName = pTitleRes->GetPropertyValue(7);
			pTitle->bLogoutOff =  atoi(pTitleRes->GetPropertyValue(8));
			pTitle->bCanOff = atoi(pTitleRes->GetPropertyValue(9));
			pTitle->bCanDel	= atoi(pTitleRes->GetPropertyValue(10));	
			pTitle->lTimeDel = atoi(pTitleRes->GetPropertyValue(11));
			pTitle->strCondition = pTitleRes->GetPropertyValue(12);
			pTitle->strMethod = pTitleRes->GetPropertyValue(13);
			pTitle->strDes = pTitleRes->GetPropertyValue(14);
			pTitle->strExDes = pTitleRes->GetPropertyValue(15);
			m_TitleMap[pTitle->lTitleId] =  pTitle;	
		}
	}
	return TRUE;	
}
//------------------------------------------------------------------------
STitle* CTitleMgr::GetTitle(long lTitleId)
{
	Title_Map_Itr itr = m_TitleMap.find(lTitleId);
	if( itr!= m_TitleMap.end() )
	{
		return (*itr).second;
	}
	return NULL;
}
//------------------------------------------------------------------------
void CTitleMgr::AddActorTitle(SActorTitle2Ex *pActorTitle)
{
	if( pActorTitle == NULL)
	{
		return;
	}
	// 填充扩充结构
	pActorTitle->dwData = (DWORD)GetTitle(pActorTitle->title.lTitleId);
	m_ActorTitleMap.push_back(pActorTitle);
}
//------------------------------------------------------------------------
SActorTitle2Ex* CTitleMgr::GetActorTitle(long lTitleId)
{
	Actor_Title_Map_Itr itr = m_ActorTitleMap.begin();
	for(;itr!=m_ActorTitleMap.end();itr++)
	{
		SActorTitle2Ex *pActorTitle = (*itr);
		if( pActorTitle && pActorTitle->title.lTitleId == lTitleId)
		{
			return pActorTitle;
		}
	}
	return NULL;	
}
//------------------------------------------------------------------------
void CTitleMgr::DeleteActorTitle(long lTitleId)
{
	Actor_Title_Map_Itr itr = m_ActorTitleMap.begin();
	for(;itr!=m_ActorTitleMap.end();itr++)
	{
		SActorTitle2Ex *pActorTitle = (*itr);
		if( pActorTitle && pActorTitle->title.lTitleId == lTitleId)
		{
			delete pActorTitle;
			m_ActorTitleMap.erase(itr);
			break;
		}
	}
}
//------------------------------------------------------------------------
Actor_Title_Map& CTitleMgr::GetActorTitleMap()
{
	return m_ActorTitleMap;
}
//------------------------------------------------------------------------
void CTitleMgr::IntialActorTitleMap()
{

	Actor_Title_Map_Itr itr = m_ActorTitleMap.begin();
	for(;itr!=m_ActorTitleMap.end();itr++)
	{
		SActorTitle2Ex *pActorTitle = (*itr);
		if( pActorTitle )
		{
			delete pActorTitle;
		}
	}
	m_ActorTitleMap.clear();
}
//------------------------------------------------------------------------
void CTitleMgr::FiltTitleName(string& str)
{
	char *pSource = (char *)str.c_str();
	int i = 0;
	string temp;
	while( *pSource!='\0' )
	{
		if( *pSource == '#' )
		{
			if( isdigit(*(pSource+1)) && isdigit(*(pSource+2)) )
			{

				if( *(pSource+3) == '#')
				{
					pSource+=4;
					continue;
				}

			}

		}
		if( *pSource  == ',')
		{
			break;
		}
		temp += (*pSource);
		pSource++;
	}
	str = temp;
}
//------------------------------------------------------------------------