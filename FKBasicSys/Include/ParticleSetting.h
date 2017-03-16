/**
*	created:		2012-7-2   1:07
*	filename: 		ParticleSetting
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <map>
using std::map;
//------------------------------------------------------------------------
struct IResScript;
class CParticle;
//------------------------------------------------------------------------
class CParticleSetting  
{
public:
	typedef std::map<long,CParticle*> ParticleSet;
	typedef ParticleSet::iterator iterator;
public:
	CParticleSetting();
	virtual ~CParticleSetting();

	BOOL AddNewParticle(LPCTSTR szName);
	BOOL Initialize(IResScript * pResScript);
	BOOL SaveSetting(IResScript * pResScript);

	static CParticleSetting * GetInstance();

	CParticle * GetParticle(long id);

private:
	void clear();

private:
	ParticleSet m_particle;
	int m_nID;
};
//------------------------------------------------------------------------