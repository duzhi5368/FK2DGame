/**
*	created:		2012-6-18   20:23
*	filename: 		Encrypt
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
class CEncrypt  
{
public:
	CEncrypt();
	virtual ~CEncrypt();

	BOOL Encrypt(BYTE* pBuffer, int nSize, BYTE* pKey);
	BOOL Decrypt(BYTE* pBuffer, int nSize, BYTE* pKey);
	BOOL Encrypt(BYTE* pInput, BYTE* pOutput, int nSize, BYTE* pKey);
	BOOL Decrypt(BYTE* pInput, BYTE* pOutput, int nSize, BYTE* pKey);
private:
	BOOL MakeSimpleMap(BYTE* buf, int nLen);

};
//------------------------------------------------------------------------