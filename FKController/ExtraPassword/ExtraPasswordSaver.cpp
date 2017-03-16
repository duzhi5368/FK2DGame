/**
*	created:		2012-6-24   20:53
*	filename: 		ExtraPasswordSaver
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "ExtraPasswordSaver.h"
//------------------------------------------------------------------------
CExtraPasswordSaver::CExtraPasswordSaver()
{

}
//------------------------------------------------------------------------
CExtraPasswordSaver::~CExtraPasswordSaver()
{

}
//------------------------------------------------------------------------
void CExtraPasswordSaver::SetExtraPassword(LPCSTR szPSW)
{
	m_strPSW = szPSW;
}
//------------------------------------------------------------------------
LPCSTR CExtraPasswordSaver::GetExtraPassword(void)
{
	return m_strPSW.c_str();
}
//------------------------------------------------------------------------