/**
*	created:		2012-6-25   0:13
*	filename: 		Func
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/Func.h"
//------------------------------------------------------------------------
char *MyAllocMemory(int nLen)
{ // 优化内存分配
	if(nLen <= 0)
		return NULL;
	nLen = (nLen / 64 + 1) * 64;
	return new char[nLen];
}
//------------------------------------------------------------------------