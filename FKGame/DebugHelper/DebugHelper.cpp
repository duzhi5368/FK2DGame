/**
*	created:		2012-6-22   0:11
*	filename: 		DebugHelper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "DebugHelper.h"
//------------------------------------------------------------------------
CDebug::CDebug(void)
{
	m_bWriteFlag = FALSE;
}
//------------------------------------------------------------------------
void CDebug::Init(char * filename)
{
	m_bWriteFlag = TRUE;
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	m_filestream.open(filename, ios::out);	

	WriteLine("--------------------- “≈ ß÷Æµ∫ ---------------------");
	WriteLine("-----------------%4d-%2d-%2d  %02d:%02d:%02d-----------------", 
		SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,
		SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond );	
}
//------------------------------------------------------------------------
CDebug::~CDebug()
{
	WriteLine("\r\n----------------------- Ω· ¯ -----------------------\n");
	m_filestream.close();
}
//------------------------------------------------------------------------
void CDebug::WriteLine(char *string, ...)
{
	if(m_bWriteFlag && m_filestream.is_open())
	{
		char buffer[1024];
		va_list argp;
		va_start(argp,string);
		vsprintf(buffer,string,argp);
		va_end(argp);

		m_filestream<<buffer<<endl;
		m_filestream.flush();
	}
}
//------------------------------------------------------------------------
void CDebug::WriteDebug(char *string, ...)
{	
	if(m_bWriteFlag && m_filestream.is_open())
	{
		char buffer[1024];
		va_list argp;	
		va_start(argp,string);
		vsprintf(buffer,string,argp);
		va_end(argp);

		m_filestream<<buffer;
		m_filestream.flush();
	}	
}
//------------------------------------------------------------------------
void CDebug::Write(char * string)
{
	if(m_bWriteFlag && m_filestream.is_open())
	{
		m_filestream<<string<<endl;
		m_filestream.flush();
	}	
}
//------------------------------------------------------------------------