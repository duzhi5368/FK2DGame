/**
*	created:		2012-6-24   20:23
*	filename: 		DllVersion
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		获取当前文件版本信息
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../FKControllerInclude.h"
//------------------------------------------------------------------------
#pragma comment(lib, "version.lib")
//------------------------------------------------------------------------
class CDllVersion  
{
private:
	BOOL m_bIsValid;	
	int  m_Ver[4];

public:
	CDllVersion();
	virtual ~CDllVersion();

	void Init(void);
	BOOL isValid(void);
	BOOL isVersionPass(const char *pData, int nLen);
};
//------------------------------------------------------------------------
extern CDllVersion g_DllVersion;
//------------------------------------------------------------------------
