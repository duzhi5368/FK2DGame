/**
*	created:		2012-6-28   9:14
*	filename: 		TitleClient
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		³ÆºÅ¿Í»§¶Ë
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "TitleWnd.h"
#include "TitleMgr.h"
#include "TitleController.h"
#include "../../FKGame/SceneObj/IActor.h"
//------------------------------------------------------------------------
class CTitleClient  
{
protected:
	CTitleClient();
public:
	virtual ~CTitleClient();
public:
    BOOL Create(IClientGlobal *pGlobal,IResScript *pRescript);
    void SetPlayerTitle(UID uid,long lTitleId,LPCSTR lpName = NULL);
	void Speak(LPCSTR lpStr);
	static CTitleClient* Instance();
public:
	IActor *GetActor(UID uid);
    CTitleMgr& GetTitleMgr();
	CTitleController& GetTitleController();
private:
    static CTitleClient* s_pInstance;
private:
    IClientGlobal * m_pGlobal;	
    CTitleMgr  m_TitleMgr;		
	CTitleWnd  *m_pTitleWnd;
	CTitleController m_Controller;
};
//------------------------------------------------------------------------