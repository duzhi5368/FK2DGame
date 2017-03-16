/**
*	created:		2012-6-28   4:30
*	filename: 		BulletinWnd
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		系统公告栏
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "BulletinContainer.h"
//------------------------------------------------------------------------
// 系统公告栏
class CBulletinWnd : public XMainWnd
{
public:
	CBulletinWnd(void);
	virtual ~CBulletinWnd(void);
	
public:
	BOOL Create(IResObject * pResObject);
	virtual BOOL IsTransparent(POINT& pt);
	virtual DWORD OnMouseDown(UINT nButton, UINT nShift, POINT pt);
	virtual DWORD OnMouseMove(UINT nButton, UINT nShift, POINT pt);
	void AddBulletin(LPCSTR lpszBulletin);

private:
	CBulletinContainer m_container;
};
//------------------------------------------------------------------------