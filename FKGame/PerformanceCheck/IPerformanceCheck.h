/**
*	created:		2012-6-22   0:03
*	filename: 		IPerformanceCheck
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
struct IProfileNode
{
	virtual IProfileNode *		GetParent(void)	= NULL;
	virtual IProfileNode *		GetSibling(void) = NULL;
	virtual IProfileNode *		GetChild(void) = NULL;
	virtual IProfileNode *		GetSubNode(const char * name) = NULL;

	virtual void				Reset(void)	= NULL;
	virtual void				Call(void) = NULL;
	virtual bool				Return(void) = NULL;

	virtual const char *		GetName(void) = NULL;
	virtual int					GetTotalCalls(void)	= NULL;
	virtual float				GetTotalTime(void)	= NULL;

	virtual float				GetAverageTime(void) = NULL;
	virtual float				GetMaxTime(void) = NULL;
	virtual float				GetMinTime(void) = NULL;
};
//------------------------------------------------------------------------