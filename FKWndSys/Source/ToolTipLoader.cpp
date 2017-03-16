/**
*	created:		2012-6-23   21:55
*	filename: 		ToolTipLoader
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
CToolTipLoader CToolTipLoader::__instance;
//------------------------------------------------------------------------
void CToolTipLoader::AddToolTip(int nID, LPCSTR szStr)
{
	m_hmapToolTip[nID] = szStr;
}
//------------------------------------------------------------------------
LPCSTR CToolTipLoader::GetToolTip(int nID)
{	
	return m_hmapToolTip[nID].c_str();
}
//------------------------------------------------------------------------
CToolTipLoader & CToolTipLoader::GetInstance(void)
{
	return __instance;
}
//------------------------------------------------------------------------