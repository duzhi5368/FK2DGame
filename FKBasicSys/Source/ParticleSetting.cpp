/**
*	created:		2012-7-2   1:08
*	filename: 		ParticleSetting
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/BasicCommon.h"
#include "../Include/ParticleSetting.h"
#include "../Include/Particle.h"
#include "../../FKTrace/IFKTrace.h"
#include "../../FKResManager/IFKResManager.h"
//------------------------------------------------------------------------
CParticleSetting::CParticleSetting()
{
	m_nID = 0;
}
//------------------------------------------------------------------------
CParticleSetting::~CParticleSetting()
{
	clear();
}
//------------------------------------------------------------------------
CParticleSetting * CParticleSetting::GetInstance()
{
	static CParticleSetting particleSetting;
	return & particleSetting;
}
//------------------------------------------------------------------------
BOOL CParticleSetting::AddNewParticle(LPCTSTR szName)
{
	CParticle *pParticle = new CParticle();
	if(pParticle == NULL)
		return false;

	lstrcpyn(pParticle->m_szName, szName, sizeof(pParticle->m_szName));

	m_nID++;
	m_particle.insert(std::make_pair<long const,CParticle*>(m_nID,pParticle));

	return true;
}
//------------------------------------------------------------------------
BOOL CParticleSetting::Initialize(IResScript * pResScript)
{
	clear();
	m_nID = -1;
	IResObject * pRoot = pResScript->GetResObjectRoot();
	for(int index =0; index<pRoot->GetSubObjectCount();++index)
	{
		IResObject * pResObject = pRoot->GetSubObjectByIndex(index);
		if(!pResObject)
		{
			return false;
		}

		CParticle *pParticle = new CParticle();
		if(pParticle == NULL)
			return false;

		int nID;
		pResObject->GetPropertyLong("ID", (long &)nID, 0);
		if(nID > m_nID)
			m_nID = nID;

		pResObject->GetPropertyLong("PartAlpha",		(long &)pParticle->m_nPartAlpha,	255);

		pResObject->GetPropertyLong("Life",				(long &)pParticle->m_nLife,			10);
		pResObject->GetPropertyLong("PerCaiyangNum",	(long &)pParticle->m_nPerPtcNum,	5);
		pResObject->GetPropertyLong("ResID",			(long &)pParticle->m_dwResID,		0);

		pResObject->GetPropertyLong("X_Speed_Min",		(long &)pParticle->m_nXSpeedMin,	0);
		pResObject->GetPropertyLong("X_Speed_Max",		(long &)pParticle->m_nXSpeedMax,	0);
		pResObject->GetPropertyLong("Y_Speed_Min",		(long &)pParticle->m_nYSpeedMin,	0);
		pResObject->GetPropertyLong("Y_Speed_Max",		(long &)pParticle->m_nYSpeedMax,	0);

		pResObject->GetPropertyLong("X_Add_Speed",		(long &)pParticle->m_nXAddSpeed,	0);
		pResObject->GetPropertyLong("Y_Add_Speed",		(long &)pParticle->m_nYAddSpeed,	0);

		pResObject->GetPropertyLong("X_DrawOffset",		(long &)pParticle->m_nXDrawOffset,	0);
		pResObject->GetPropertyLong("Y_DrawOffset",		(long &)pParticle->m_nYDrawOffset,	0);

		pResObject->GetPropertyLong("X_CaiYang",		(long &)pParticle->m_nXCaiYang,		5);
		pResObject->GetPropertyLong("Y_CaiYang",		(long &)pParticle->m_nYCaiYang,		5);

		pResObject->GetPropertyLong("Alpha_Begin",		(long &)pParticle->m_nAlphaBegin,	255);
		pResObject->GetPropertyLong("Alpha_End",		(long &)pParticle->m_nAlphaEnd,		255);

		lstrcpyn(pParticle->m_szName, pResObject->GetObjectName(), sizeof(pParticle->m_szName));

		if(m_particle.find(nID)!=m_particle.end())
		{
			return false;
		}

		m_particle.insert(std::make_pair<long const,CParticle*>(nID,pParticle));
	}

	return true;
}
//------------------------------------------------------------------------
BOOL CParticleSetting::SaveSetting(IResScript * pResScript)
{
	IResObject * pRoot = pResScript->GetResObjectRoot();
	while(pRoot->GetSubObjectCount() > 0)
	{
		pRoot->EraseSubObjectByIndex(0);
	}

	for(iterator it = m_particle.begin(); it!=m_particle.end(); ++it)
	{
		CParticle *pParticle = (*it).second;
		if(pParticle == NULL)
			continue;

		IResObject* pResObject = pRoot->CreateSubObject();

		pResObject->SetObjectInfo(pParticle->m_szName);
		pResObject->SetPropertyLong("ID", (long &)(*it).first);

		pResObject->SetPropertyLong("PartAlpha",		(long &)pParticle->m_nPartAlpha);

		pResObject->SetPropertyLong("Life",				(long &)pParticle->m_nLife);
		pResObject->SetPropertyLong("PerCaiyangNum",	(long &)pParticle->m_nPerPtcNum);
		pResObject->SetPropertyLong("ResID",			(long &)pParticle->m_dwResID);

		pResObject->SetPropertyLong("X_Speed_Min",		(long &)pParticle->m_nXSpeedMin);
		pResObject->SetPropertyLong("X_Speed_Max",		(long &)pParticle->m_nXSpeedMax);
		pResObject->SetPropertyLong("Y_Speed_Min",		(long &)pParticle->m_nYSpeedMin);
		pResObject->SetPropertyLong("Y_Speed_Max",		(long &)pParticle->m_nYSpeedMax);

		pResObject->SetPropertyLong("X_Add_Speed",		(long &)pParticle->m_nXAddSpeed);
		pResObject->SetPropertyLong("Y_Add_Speed",		(long &)pParticle->m_nYAddSpeed);

		pResObject->SetPropertyLong("X_DrawOffset",		(long &)pParticle->m_nXDrawOffset);
		pResObject->SetPropertyLong("Y_DrawOffset",		(long &)pParticle->m_nYDrawOffset);

		pResObject->SetPropertyLong("X_CaiYang",		(long &)pParticle->m_nXCaiYang);
		pResObject->SetPropertyLong("Y_CaiYang",		(long &)pParticle->m_nYCaiYang);

		pResObject->SetPropertyLong("Alpha_Begin",		(long &)pParticle->m_nAlphaBegin);
		pResObject->SetPropertyLong("Alpha_End",		(long &)pParticle->m_nAlphaEnd);
	}

	pResScript->SaveScript();

	return true;
}
//------------------------------------------------------------------------
void CParticleSetting::clear()
{
	for(iterator it = m_particle.begin(); it!=m_particle.end(); ++it)
	{
		(*it).second->CloseRes();
		delete (*it).second;
		(*it).second = NULL;
	}
	m_particle.clear();
}
//------------------------------------------------------------------------
CParticle * CParticleSetting::GetParticle(long id)
{
	iterator it = m_particle.find(id);
	if(it != m_particle.end())
		return (*it).second;
	return NULL;
}
//------------------------------------------------------------------------