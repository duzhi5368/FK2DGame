/**
*	created:		2012-6-22   0:05
*	filename: 		Performance
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "IPerformanceCheck.h"
#include "../DebugHelper/DebugHelper.h"
#include <hash_map>
using stdext::hash_map;
#define _ROOT_ "_ROOT_"
//------------------------------------------------------------------------
class CProfileNode : public IProfileNode
{
public:
	virtual IProfileNode *		GetParent(void);
	virtual IProfileNode *		GetSibling(void);
	virtual IProfileNode *		GetChild(void);
	virtual IProfileNode *		GetSubNode(const char * name);

	virtual void				Reset(void);
	virtual void				Call(void);
	virtual bool				Return(void);

	virtual const char *		GetName(void);
	virtual int					GetTotalCalls(void);
	virtual float				GetTotalTime(void);

	virtual float				GetAverageTime(void);
	virtual float				GetMaxTime(void);
	virtual float				GetMinTime(void);
public:
	CProfileNode(const char * name, CProfileNode * parent);
	~CProfileNode(void);
protected:
	char						Name[256];
	int							TotalCalls;
	float						TotalTime;
	float						MaxTime,MinTime;
	__int64						StartTime;
	int							RecursionCounter;
	CProfileNode *				Parent;
	CProfileNode *				Child;
	CProfileNode *				Sibling;
};
//------------------------------------------------------------------------
class CProfileManager
{
public:
	static	void				Start_Profile(const char * name);
	static	void				Stop_Profile(void);
	static	void				Reset(void);
	static	void				Increment_Frame_Counter(void);
	static	int					Get_Frame_Count_Since_Reset(void);
	static	float				Get_Time_Since_Reset(void);
	static IProfileNode*		GetRoot(void);
	void						Output(void);
	void						OutputTree(IProfileNode * pNode);
	void						GetDesc(char szMsg[256], IProfileNode * pNode);

	CProfileManager(void);
private:
	static	CProfileNode		Root;
	static	IProfileNode *		CurrentNode;
	static	int					FrameCounter;
	static	__int64				ResetTime;

	CDebug						m_Debuger;
};
//------------------------------------------------------------------------