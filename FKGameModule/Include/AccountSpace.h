/**
*	created:		2012-6-25   23:50
*	filename: 		AccountSpace
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include <memory.h>
//------------------------------------------------------------------------
#define  DISTANCE_INVALID	0x7FFFFFFF
//------------------------------------------------------------------------
template<int iWidth = 32,int iHigh = 32 ,int iUnitWidth = 1,int iUnitHigh=1>
class CAccountSpace
{
private:
	int* pCache;
	int m_iWidth;
	int m_iHigh;
	int m_iUnitWidth;
	int m_iUnitHigh;
	bool m_bIsdefine;
public:
	CAccountSpace();
	virtual	~CAccountSpace();
	int Distance(int X1,int Y1,int X2,int Y2);
	int Distance(POINT &pt1, POINT &pt2);
	int Distance(POINTS &pt1, POINTS &pt2);
};
//------------------------------------------------------------------------
template<int iWidth,int iHigh ,int iUnitWidth ,int iUnitHigh>
CAccountSpace< iWidth , iHigh  , iUnitWidth , iUnitHigh>::CAccountSpace()
{
	m_iWidth=iWidth;
	m_iHigh=iHigh;
	m_iUnitWidth=iUnitWidth;
	m_iUnitHigh=iUnitHigh;
	pCache=(int*)calloc((iWidth+1)*(iHigh+1),sizeof(int));
	memset(pCache,0,sizeof(pCache));
	for(int vHigh=0;vHigh<=m_iHigh;vHigh++)
	{
		for(int vWidth=0;vWidth<=m_iWidth;vWidth++)
		{
			//计算每一格与原点的距离，并存贮
			pCache[vHigh*(m_iWidth+1)+vWidth] = (int)pow( (double)(pow( (double)(vWidth*m_iUnitWidth),2)+pow( (double)(vHigh*m_iUnitHigh),2)),0.5 );
		}
	}
}
//------------------------------------------------------------------------
template<int iWidth ,int iHigh  ,int iUnitWidth ,int iUnitHigh>
CAccountSpace< iWidth , iHigh  , iUnitWidth , iUnitHigh>::~CAccountSpace()
{
	free(pCache);
}
//------------------------------------------------------------------------
template<int iWidth ,int iHigh  ,int iUnitWidth ,int iUnitHigh>
int CAccountSpace< iWidth , iHigh  , iUnitWidth , iUnitHigh>::Distance(int X1,int Y1,int X2,int Y2)
{ 
	int vHigh=abs(Y1-Y2)/m_iUnitHigh;
	int vWidth=abs(X1-X2)/m_iUnitWidth;
	if((vWidth>m_iHigh)||(vHigh>m_iWidth))
		return DISTANCE_INVALID;
	return pCache[vHigh*(m_iWidth+1)+vWidth];
}
//------------------------------------------------------------------------
template<int iWidth ,int iHigh  ,int iUnitWidth ,int iUnitHigh>
int CAccountSpace< iWidth , iHigh  , iUnitWidth , iUnitHigh>::Distance(POINT &pt1, POINT &pt2)
{
	return Distance(pt1.x, pt1.y, pt2.x, pt2.y);
}
//------------------------------------------------------------------------
template<int iWidth ,int iHigh  ,int iUnitWidth ,int iUnitHigh>
int CAccountSpace< iWidth , iHigh  , iUnitWidth , iUnitHigh>::Distance(POINTS &pt1, POINTS &pt2)
{
	return Distance(pt1.x, pt1.y, pt2.x, pt2.y);
}
//------------------------------------------------------------------------