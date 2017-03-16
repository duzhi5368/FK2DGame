/**
*	created:		2012-6-20   19:58
*	filename: 		DirtyList
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
#define MAX_DIRTY_SIZE 200
#define SFREE -1
#define SEND  -2
//------------------------------------------------------------------------
class CDirtyList  
{
private:
	RECT m_rcList[MAX_DIRTY_SIZE];
	int  m_nPosList[MAX_DIRTY_SIZE];
	int* m_pPrev;
	int m_nCurp, m_nLastp;
	int m_nCount;
	RECT m_rcBorder;
	BOOL m_bWhole;
public:
	CDirtyList();
	virtual ~CDirtyList();
public:
	void	_ClearDirty();
	void	ClearDirty();													//清除整个List
	int		GetDirtyCount(){return m_nCount;}								//得到脏矩形的个数
	void	_AddDirtyRect(int left, int top, int right, int bottom);
	void    AddDirtyRectDirect(int left, int top, int right, int bottom);	//增加脏矩形
	void	AddDirtyRect(RECT& rc);
	void	AddDirtyList(CDirtyList* pList);								//合并二个DirtyList
	RECT*	SeekFirstDirty();												//枚举脏矩形
	RECT*	SeekNextDirty();
	void	PackDirty();													//合并脏矩形
	void	PackDirty(int x, int y, int w, int h);
	void	RemoveDirty(RECT& rc);											//移去脏矩形
	void	SetBorder(RECT& rc){m_rcBorder = rc;}							//设置整个屏幕的矩形
	BOOL	IsWholeDirty(){return m_bWhole;}								//是否全脏
	BOOL	IsDirty(RECT& rc);												//检查些矩形是否为脏
	void	Scroll(int x, int y);
	void	AddWholeScreen();
};
//------------------------------------------------------------------------