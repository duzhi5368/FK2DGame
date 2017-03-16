/**
*	created:		2012-6-24   1:42
*	filename: 		Performance
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		性能评测
*/
//------------------------------------------------------------------------
#include "PreInclude.h"
#include "Performance.h"
//------------------------------------------------------------------------
inline void Profile_Get_Ticks(__int64 * ticks)
{
	LARGE_INTEGER *p = (LARGE_INTEGER *)ticks;
	QueryPerformanceCounter(p);
}
//------------------------------------------------------------------------
inline float Profile_Get_Tick_Rate(void)
{
	static float _CPUFrequency = -1.0f;
	
	if (_CPUFrequency == -1.0f) {
		__int64 curr_rate = 0;
		::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
		_CPUFrequency = (float)curr_rate;
	} 
	
	return _CPUFrequency;
}
//------------------------------------------------------------------------
CProfileNode::CProfileNode(const char * name, CProfileNode * parent)
{
	TotalCalls = 0;
	TotalTime = 0;
	MaxTime = 0;
	MinTime = (float)_I64_MAX;
	StartTime = 0;
	RecursionCounter = 0;
	Parent = parent;
	Child = NULL;
	Sibling = NULL;

	strcpy(this->Name, name);
	Reset();
}
//------------------------------------------------------------------------
CProfileNode::~CProfileNode(void)
{
	delete Child;
	delete Sibling;
}
//------------------------------------------------------------------------
IProfileNode * CProfileNode::GetParent(void)
{
	return Parent; 
}
//------------------------------------------------------------------------
IProfileNode * CProfileNode::GetSibling(void)
{
	return Sibling; 
}
//------------------------------------------------------------------------
IProfileNode * CProfileNode::GetChild(void)
{
	return Child; 
}
//------------------------------------------------------------------------
const char * CProfileNode::GetName(void)
{
	return Name; 
}
//------------------------------------------------------------------------
int	CProfileNode::GetTotalCalls(void)
{ 
	return TotalCalls; 
}
//------------------------------------------------------------------------
float CProfileNode::GetTotalTime(void)
{
	return TotalTime;
}
//------------------------------------------------------------------------
float CProfileNode::GetAverageTime(void)
{
	if(TotalCalls)return TotalTime / TotalCalls;return 0;
}
//------------------------------------------------------------------------
float CProfileNode::GetMaxTime(void)
{
	return MaxTime;
}
//------------------------------------------------------------------------
float CProfileNode::GetMinTime(void)
{
	return MinTime;
}
//------------------------------------------------------------------------
IProfileNode * CProfileNode::GetSubNode(const char * name)
{
	CProfileNode * child = Child;
	while(child) 
	{
		if(strcmp(child->Name, name) == 0) 
		{
			return child;
		}
		
		child = child->Sibling;
	}
	CProfileNode * node = new CProfileNode(name, this );
	CProfileNode *pTemp = Child;
	CProfileNode *pPre = 0;
	while(pTemp)
	{
		pPre = pTemp;
		pTemp = (CProfileNode*)pTemp->GetSibling();
	}
	if(pPre)
	{
		pPre->Sibling = node;
	}
	else
	{
		Child = node;
	}
	return node;
}
//------------------------------------------------------------------------
void CProfileNode::Reset(void)
{
	TotalCalls = 0;
	TotalTime = 0.0f;
	MinTime = (float)_I64_MAX;
	MaxTime = 0;

	if(Child)
	{
		Child->Reset();
	}
	
	if (Sibling)
	{
		Sibling->Reset();
	}
}
//------------------------------------------------------------------------
void CProfileNode::Call(void)
{
	TotalCalls++;

	if(RecursionCounter++ == 0)
	{
		Profile_Get_Ticks(&StartTime);
	}
}
//------------------------------------------------------------------------
bool CProfileNode::Return(void)
{
	if( --RecursionCounter == 0 && TotalCalls != 0)
	{ 
		__int64 time;
		Profile_Get_Ticks(&time);
		time -= StartTime;

		float fTime = (float)time / Profile_Get_Tick_Rate(); 
		TotalTime += fTime;

		if(fTime > MaxTime)MaxTime = fTime;
		if(fTime < MinTime)
		{
			MinTime = fTime;
		}
	}

	return (RecursionCounter == 0);
}
//------------------------------------------------------------------------
CProfileNode	CProfileManager::Root(_ROOT_, NULL);
IProfileNode *	CProfileManager::CurrentNode = &CProfileManager::Root;
int				CProfileManager::FrameCounter = 0;
__int64			CProfileManager::ResetTime = 0;
//------------------------------------------------------------------------
CProfileManager::CProfileManager(void)
{
	m_Debuger.Init("log\\Profile.txt");
}
//------------------------------------------------------------------------
void CProfileManager::Start_Profile(const char * name)
{
	return;
	if(strcmp(name,CurrentNode->GetName())) 
	{
		CurrentNode = CurrentNode->GetSubNode(name);
	} 
	
	CurrentNode->Call();
}
//------------------------------------------------------------------------
void CProfileManager::Stop_Profile(void)
{
	return;
	if(CurrentNode->Return())
	{
		CurrentNode = CurrentNode->GetParent();
	}
}
//------------------------------------------------------------------------
void CProfileManager::Reset(void)
{ 
	Root.Reset(); 
	FrameCounter = 0;
	Profile_Get_Ticks(&ResetTime);
}
//------------------------------------------------------------------------
void CProfileManager::Increment_Frame_Counter(void)
{
	FrameCounter++;
}
//------------------------------------------------------------------------
float CProfileManager::Get_Time_Since_Reset(void)
{
	__int64 time;
	Profile_Get_Ticks(&time);
	time -= ResetTime;
	return (float)time / Profile_Get_Tick_Rate();
}
//------------------------------------------------------------------------
int	CProfileManager::Get_Frame_Count_Since_Reset(void)
{
	return FrameCounter; 
}
//------------------------------------------------------------------------
IProfileNode * CProfileManager::GetRoot(void)
{
	return &Root;
}
//------------------------------------------------------------------------
void CProfileManager::Output(void)
{
	IProfileNode *pNode = GetRoot();
	if(pNode)
	{
		OutputTree(pNode);
	}
}
//------------------------------------------------------------------------
void CProfileManager::OutputTree(IProfileNode * pNode)
{
	if(pNode == NULL)
	{
		return;
	}

	char szMsg[256];
	GetDesc(szMsg, pNode);

	char szItem[512];
	char * pName = (char*)pNode->GetName();
	if(pName == NULL)
	{
		return;
	}

	int iSize = strlen(pName);
	strcpy(szItem, pNode->GetName());
	for(int k = 0; k < 32 - iSize; k++)
	{
		strcat(szItem, " ");
	}
	strcat(szItem, szMsg);
	m_Debuger.WriteLine(szItem);

	IProfileNode * pChild = pNode->GetChild();
	if(pChild != NULL)
	{
		while(pChild)
		{
			OutputTree(pChild);
			pChild = pChild->GetSibling();
		}
	}
}
//------------------------------------------------------------------------
void CProfileManager::GetDesc(char szMsg[256], IProfileNode * pNode)
{
	if(stricmp(pNode->GetName(), _ROOT_) == 0)
	{
		sprintf(szMsg,"总时间 = %f秒", CProfileManager::Get_Time_Since_Reset());
	}
	else
	{
		sprintf(szMsg,"调用次数 = %8d 总时间 = %6.4f秒 平均 = %6.4f秒 最小 = %6.4f秒 最大 = %6.4f秒", pNode->GetTotalCalls(), pNode->GetTotalTime(), pNode->GetAverageTime(), pNode->GetMinTime(), pNode->GetMaxTime());
	}
}
//------------------------------------------------------------------------