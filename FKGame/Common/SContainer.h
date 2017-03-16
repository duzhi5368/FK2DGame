/**
*	created:		2012-7-4   1:03
*	filename: 		SContainer
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define MAINCONTAINER_CSC_ADD		0		//往容器添加一个物品
#define MAINCONTAINER_CSC_ADDALL	1		//添加一个容器中所有物品到另外一个容器中
#define MAINCONTAINER_CSC_DESTORY	2		//销毁物品
#define MAINCONTAINER_CSC_REMOVE	3		//从容器移出物品
#define MAINCONTAINER_CSC_UPDATE	4		//通知客户端更新

#define MAINCONTAINER_CSC_OPEN		5		//打开
#define MAINCONTAINER_CSC_CLOSE		6		//关闭
#define MAINCONTAINER_CSC_USE		7		//使用
//------------------------------------------------------------------------
///////////////////////////物品类型定义//////////////////////////////////////
#define GOODS_TYPE_REALY			1		//真实物品
#define GOODS_TYPE_REFER			2		//物品引用
#define GOODS_TYPE_SKILL			3		//技能
//------------------------------------------------------------------------
/////////////////////////容器标志类型/////////////////////////////
#define CONTAINER_FLAGTYPE_PACKET		1		//物品栏
#define CONTAINER_FLAGTYPE_EQUIP		2		//装备栏
#define CONTAINER_FLAGTYPE_WAREHOUSE	3		//仓库
#define CONTAINER_FLAGTYPE_TRADE		4		//交易
#define CONTAINER_FLAGTYPE_MONSTER_BOX	5		//怪物掉落的宝箱
#define CONTAINER_FLAGTYPE_RESOURCE_BOX 6		//矿石药草等等
#define CONTAINER_FLAGTYPE_SYSTEM_BOX	7		//系统产生的宝箱
//------------------------------------------------------------------------
/////////////////////////服务器客户端通讯相关现场/////////////////////////////
//打开容器的现场
struct SOpenContainerContext
{
	DWORD dwContainerID;
	SOpenContainerContext()
	{
		memset(this, 0, sizeof(SOpenContainerContext));
	}
};
//------------------------------------------------------------------------
//关闭容器的现场
#define SCloseContainerContext SOpenContainerContext
//------------------------------------------------------------------------
//客户端发上来的物品添加现场
struct SGoodsAddContext
{
	DWORD dwTargetContainerID;	//目标容器id
	int nTargetLocation;		//在目标容器中的位置	
	UID uidGoods;				//物品uid
	int nGoodsNumber;			//物品数目
	DWORD dwSrcContainerID;		//源容器id
	int nSrcLocation;			//在源容器中的位置	
	SGoodsAddContext()
	{
		dwTargetContainerID = 0;
		nTargetLocation = -1;
		uidGoods = 0;
		nGoodsNumber = 0;
		dwSrcContainerID = 0;
		nSrcLocation = -1;
	}
};
//------------------------------------------------------------------------
//客户端发上来的将一个容器中所有物品转移到另一个容器中的现场
struct SContainerToContainer 
{
	DWORD dwTargetContainerID;	//目标容器id
	DWORD dwSrcContainerID;		//源容器id
	SContainerToContainer()
	{
		memset(this, 0 ,sizeof(SContainerToContainer));
	}
};
//------------------------------------------------------------------------
//销毁物品现场
struct SDestoryGoodsContext
{
	UID uidGoods;				//物品uid
	DWORD dwSrcContainerID;		//源容器id
	int nSrcLocation;			//在源容器中的位置
	SDestoryGoodsContext()
	{
		memset(this, 0, sizeof(SDestoryGoodsContext));
	}
};
//------------------------------------------------------------------------
//更新客户端
struct SContainerUpdateHead
{
	SContainerUpdateHead()
	{
		dwContainerID = 0;
	}
	DWORD dwContainerID;		//容器id	
	//带一个或者多个SSingleUpdateInfo
};
//------------------------------------------------------------------------
struct SSingleUpdateInfo
{
	UID uidGoods;				//物品uid
	int nLocation;				//所有在容器中的位置
	BOOL bLock;					//是否锁定
	DWORD dwNumber;				//数目

	SSingleUpdateInfo()
	{
		uidGoods = INVALID_UID;
		nLocation = -1;
		bLock = FALSE;
		dwNumber = 0;
	}
};
//------------------------------------------------------------------------
//使用物品现场
struct SUseGoodsContext
{	
	DWORD dwSrcContainerID;		//源容器id
	int nSrcLocation;			//在源容器中的位置	
	SUseGoodsContext()
	{
		memset(this, 0, sizeof(SUseGoodsContext));
	}
};
//------------------------------------------------------------------------
/////////////////////////事件相关现场/////////////////////////////////////////
//容器相关响应现场
struct SSGoodsResponseContext
{
	DWORD dwOperatorID;		//操作者
	DWORD dwContainerID;	//所在容器ID
	UID uidGoods;			//物品UID
	int nLocation;			//物品所在位置
	DWORD dwSrcContainerID; //物品源容器ID
	SSGoodsResponseContext()
	{
		memset(this, 0, sizeof(SSGoodsResponseContext));
	}
};
//------------------------------------------------------------------------
//物品添加或者删除否决现场,其实与响应现场相同
#define SSGoodsVoteContext	SSGoodsResponseContext
#define SSGoodsUseContext	SSGoodsResponseContext
//------------------------------------------------------------------------

