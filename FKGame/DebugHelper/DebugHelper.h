/**
*	created:		2012-6-22   0:10
*	filename: 		DebugHelper
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <stdio.h>
#include <fstream>
//------------------------------------------------------------------------
class CDebug
{
private:		
	fstream		m_filestream;
	BOOL		m_bWriteFlag;

public:
	CDebug(void);
	~CDebug(void);	
	void	Init(char * filename);
	void	WriteDebug(char *string, ...);
	void	WriteLine(char *string, ...);	
	void	Write(char * string);
};
//------------------------------------------------------------------------