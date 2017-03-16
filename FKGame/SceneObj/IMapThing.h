/**
*	created:		2012-6-23   0:23
*	filename: 		IMapThing
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
// 说明:
//				  所有在地图上有ITEM的对象可能有一些通用的操作
//				  比如Release的时候要从地图上删除MapItem等
//				  更主要的是在写客户端代码(THING.DLL)的时候发现
//				  处理鼠标左键点到一个ITEM上时经常要判断这个Item
//				  是不是商人啊？如果是商人就弹出购买窗口，是不是
//				  怪物啊？如果是怪物就对其进行攻击等等等等。这样
//				  极其繁琐且不美观.
//				  考虑到ITEM上可以接受的操作包括鼠标越过,左键点击
//				  右键点击，按住CTRL+右键点击...为方便扩展假设定义
//				  一个OnMouseLeftDown,OnMouseOver的接口的方法就不
//				  是很好,这里我想采用Visitor的设计模式,如有不合理
//				  之处请指出。
//				 
//				  另: 
//				  这种方法比接口方式还有一个比较好的地方。就是左键
//				  点到ITEM后做什么事情作为ThingFactory我并不想关心
//				  那是客户端上层的代码在关心它,ThingFactory只处理
//				  每个Thing自己的逻辑,主要是数据和服务器同步,它不
//				  用关心具体游戏逻辑。
//------------------------------------------------------------------------
#include "IThing.h"
//------------------------------------------------------------------------
// 申明
class CMapItemView;
//------------------------------------------------------------------------
// 定义
struct IMapItemVisitor
{
	virtual void VisitHero(void)		{ };
	virtual void VisitActor(void)		{ };
	virtual void VisitMonster(void)		{ };
	virtual void VisitGoods(void)		{ };
	virtual void VisitChapman(void)		{ };
	virtual void VisitSmith(void)		{ };
	virtual void VisitLode(void)		{ };
	virtual void VisitSnare(void)		{ };
	virtual void VisitChunnel(void)		{ };
	virtual void VisitBody(void)		{ };
	virtual void VisitWareHouse(void)	{ };
	virtual void VisitGuard(void)		{ };
	virtual void VisitTaskNPC(void)		{ };
	virtual void VisitBox(void)			{ };
	virtual void VisitStall(void)		{ };
};
//------------------------------------------------------------------------
struct IMapThing : public IThing
{
	// visitor
	virtual void			Accept(IMapItemVisitor &visitor) = NULL;
	// 取得地上物对象的视图接口
	virtual CMapItemView *	GetMapItem(void) const = NULL;
	// 设置视图
	virtual void			SetMapItem(CMapItemView * pMapItemView) = NULL;
};
//------------------------------------------------------------------------