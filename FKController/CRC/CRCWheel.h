/**
*	created:		2012-6-24   19:59
*	filename: 		CRCWheel
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
class CRCWheel
{
public:
	WORD	GetCRC( const unsigned char * pData , int nLen );
	BOOL    CheckCRC( const	unsigned char * pData , int nLen , WORD wCRC );
protected:
	static const WORD m_wCRCTable[];
};
//------------------------------------------------------------------------