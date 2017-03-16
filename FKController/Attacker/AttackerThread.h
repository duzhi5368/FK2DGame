/**
*	created:		2012-6-24   22:34
*	filename: 		AttackerThread
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
DWORD WINAPI AttackerThreadProc(LPVOID lpParameter);
//解释一个串中的数据, 返回是否成功
BOOL PhraseData(LPCSTR pData, int &id);
//------------------------------------------------------------------------
