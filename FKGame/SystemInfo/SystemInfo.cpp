/**
*	created:		2012-6-20   23:12
*	filename: 		SystemInfo
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "SystemInfo.h"
//------------------------------------------------------------------------
siString siOsNames[] = 
{
	"Windows NT 4",
	"Windows 2000",
	"Windows XP",
	"Windows .NET",
	"Windows 95",
	"Windows 98",
	"Windows ME",
	"Linux",
	"FreeBSD",
	"Unknown"
};
//------------------------------------------------------------------------
static char errorMsg[255] = "No errors";
//------------------------------------------------------------------------
const char* siGetLastError(void)
{
	return errorMsg;
}
//------------------------------------------------------------------------
void _siSetLastError(const char* errorMessage)
{
	strcpy(errorMsg, errorMessage);
	return;
}
//------------------------------------------------------------------------