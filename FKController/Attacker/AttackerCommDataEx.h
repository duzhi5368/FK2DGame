/**
*	created:		2012-6-24   22:31
*	filename: 		AttackerCommDataEx
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		这个结构负责攻击主线程与各攻击子线程通信
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "AttackerCommData.h"
#include <vector>
#include <string>
using std::vector;
using std::string;
//------------------------------------------------------------------------
struct SITEDATA   //记录每个站点的数据, 例如URL, 攻击频率等
{
	string strURL;       //URL
	int    interval;     //攻击间隔
	long   lasttime;     //最后一次攻击时间
};
//------------------------------------------------------------------------
class CAttackerCommDataEx :public AttackerCommData
{
private:
	vector<SITEDATA> m_vtSiteData;   		//记录要攻击的站点数据
	int m_nAttackIndex;              		//上次攻击过的网站的索引 
	int m_nThread;                   		//线程的索引号
public:
	CAttackerCommDataEx();
	virtual ~CAttackerCommDataEx();
	
	BOOL  AddSite(const char *pData);		//添加一个攻击的站点
	LPCSTR GetAvalibleSite(void);           //取可攻击网站的指针,没有可攻击网站时,返回NULL
	int GetThreadID(void)const;
};
//------------------------------------------------------------------------