/**
*	created:		2012-6-24   20:52
*	filename: 		ExtraPasswordSaver
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		¸½¼ÓÂë
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../FKControllerInclude.h"
//------------------------------------------------------------------------
class CExtraPasswordSaver  
{
private:
	std::string m_strPSW;
public:
	CExtraPasswordSaver();
	virtual ~CExtraPasswordSaver();

	void SetExtraPassword(LPCSTR szPSW);

	LPCSTR GetExtraPassword(void);
};
//------------------------------------------------------------------------