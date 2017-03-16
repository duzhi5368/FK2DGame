/**
*	created:		2012-6-24   20:47
*	filename: 		MACSender
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../FKControllerInclude.h"
//------------------------------------------------------------------------
namespace NETTOOLS
{
	BOOL GetMACAddress(char * MACAddress);
}
//------------------------------------------------------------------------
class CMACSender  
{
protected:
	int m_nMacPosition;   //服务器发下来, 要求存放MAC的位置
	char m_Mac[512];
public:
	CMACSender();
	virtual ~CMACSender();
public:
	void SetMacPosition(DWORD dwPos);
	int GetMacPosition(void);
	//将MAC发送到服务器
	LPCSTR GetMacPackage(void);
};
//------------------------------------------------------------------------