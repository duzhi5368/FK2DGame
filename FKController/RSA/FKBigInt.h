/**
*	created:		2012-2-29   4:41
*	filename: 		FKBigInt
*	author:			FreeKnight
*	Copyright (C): 	TianCong (2012) FreeKnightServerEngine
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define BI_MAXLEN 8  
//------------------------------------------------------------------------
class CBigInt  
{  
public:  
	unsigned m_nLength;  
	unsigned long m_ulValue[BI_MAXLEN];  
	CBigInt();  

	void Mov(unsigned __int64 A);  
	void Mov(CBigInt& A);  
	CBigInt Sub(CBigInt& A);  
	CBigInt Mul(CBigInt& A);  
	CBigInt Mod(CBigInt& A);  
	CBigInt Sub(unsigned long A);  
	CBigInt Mul(unsigned long A);  
	CBigInt Div(unsigned long A);  
	unsigned long Mod(unsigned long A);   
	int Cmp(CBigInt& A);  
	CBigInt ModExp(CBigInt& A, CBigInt& B);  
};  

