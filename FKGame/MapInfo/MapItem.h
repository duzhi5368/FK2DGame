/**
*	created:		2012-6-21   23:05
*	filename: 		MapItem
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../../FKFileSystem/Include/FKTempl.h"
#include "../../FKGame/DataArchive/IDataArchive.h"
//------------------------------------------------------------------------
// 对象层值，例如尸体层自动放到最底层
// CMapContainer::AddLayerItem()时会根据层值大小决定对象插入到哪个位置
enum 
{
	milvNoSort = 0,/*不需要排序*/
	milvSpecificBlock = 2,
	milvLowest = 10, 
	milvMidle = 100,
	milvHight = 1000,
	milvHighest = 10000
};
//------------------------------------------------------------------------
// 占位矩形的(top,left)要求对象落脚点必须定义在最左角的那个块上
struct COccupantTileListHeader
{
	short nOffAnchorX;	// 基点相对放置点(即锚点)的偏移
	short nOffAnchorY;
	WORD wWidth;		// 占位矩形的宽
	WORD wHeight;		// 占位矩形的高
};
//------------------------------------------------------------------------
//迷你地图上ITEM的PIC ID
enum MINIITEMPICID
{
	PIC_MIN = 0,
	PIC_NPC = 0,
	PIC_ORE,
	PIC_MEDICIAL,
	PIC_PET,
	PIC_DOOR,
	PIC_TEAMMATE,
	PIC_OTHERACTOR,
	PIC_SELF,
	PIC_MONSTER,
	PIC_MAX,	
};
//------------------------------------------------------------------------
// 名称：占位块列表
// 功能：定义大物体占位块，用于大物体排序、大物体导致地表不可通过信息
typedef CMultLenArray<COccupantTileListHeader, char> COccupantTileList;
//------------------------------------------------------------------------
// 定义
class CMapItem
{
public:
	// 通用控制命令，适合于基于消息的管理，因而藕合性更小。
	// 对象在OnCommand()里发生事件时，也以消息的方式通知主程序，类似于SendMessage()
	// 与windows消息机制不同，这里的SendMessage()只是调用主程序的接口函数
	virtual int					OnCommand(DWORD dwCmdID, DWORD dwParam1,DWORD dwParam2)
	{
		return 0; 
	}
	// 获取对象类型。如角色、NPC、物品、武器等，是按位定义的
	virtual DWORD				GetType(void) const 
	{
		return 0; 
	}
	// 获取对象的类ID，主要用于地图存储，也可用于识别GetType()之外的附加对象类，如NPC有很多种
	// 根据ClassID可以将地址强制转成相应的对象类指针
	// 地图时的存储格式一般为 : dwClassID + wLen + data
	virtual	DWORD				GetClassID(void) const
	{
		return 0; 
	}
	// 对象在地图中的显示层，如尸体层比地上物层要低，有层次关系的对象通过CMapContainer::AddLayerItem()加入到地图容器中
	// 服务器实现的对象不需要考虑层值
	virtual DWORD				GetLayerValue(void) 
	{
		return milvMidle; 
	}
	// 有时GetType()只能返回对象的大的类别，特定对象可能有自己特有数据,GetType()其实是GetItemData的特殊式
	// 注：第一个数据总是预定义数据
	virtual DWORD				GetItemData(void) const
	{
		return 0; 
	}
	// 设置对象的私有数据，返回原先的值
	// 用于定义对象的自定义数据，如地图编辑器需要定义额外数据
	virtual void				SetItemData(DWORD dwData)
	{

	}
	//	保存或读取数据，用于从map文件中读取对象数据或将游戏中的对象在地图中的信息保存到map文件中
	//	用这个接口实现对象的保存和创建时的初始化
	//	输入：
	//		bSave为TRUE时，表示请求保存对象数据
	//		bSave为FALSE时,表示读取对象现场，设置对象属性
	//	返回：
	//		返回TRUE:
	//			存档成功
	//		返回FALSE：
	//			操作不成功，或不需要保存此对象
	//
	//	说明：
	//		相关函数还有GetClassID()，地图编辑器保存地图时的存储格式一般为：dwClassID + wLen + data
	// enum{sWrite/*存盘*/ = TRUE,sRead/*读盘*/ = FALSE};
	virtual BOOL				OnSerialize(BOOL bWrite, IArchive* pArchive)
	{
		return FALSE; 
	}
	// 根据数据现场创建对象
	virtual BOOL				OnCreateItem(const char* buf, int nLen)
	{ 
		return FALSE; 
	}
	// 单占位对象的占位值。如果此对象影响地表的可通过性，刚返回非0值
	// 有多占位列表的对象和无多占位列表的对象都应定义这个返回值
	// 有多占位列表的对象返回基块占位值
	// 默认为有飞行、行走格挡和漫延格挡
	virtual DWORD				GetOccupantValue(void) 
	{
		return 0x2000; 
	}
	// 读取占位块列表，默认为无占位位列表
	virtual COccupantTileList * GetOccupantTileList(void) const
	{ 
		return NULL; 
	}
	// 对象释放
	virtual void				Release(void)
	{ 
		delete this; 
	}
	// 更新(如：重载脚本数据等)
	virtual void				OnUpdate(void)
	{ 

	}
	// item在地图上的位置相关
	virtual void				SetTile(POINT &ptTile)
	{
		m_ptTile = ptTile;
	}
	// 取得TILE
	virtual POINT				GetTile(void)
	{
		return m_ptTile;
	}
	// 设置在小地图上的颜色	
	virtual void SetFlagColorOnMinmap(DWORD dwColor)
	{

	};
	// 取得在小地图上的颜色
	virtual DWORD GetFlagColorOnMinmap(void)
	{
		return 0;
	}
	CMapItem(void)
	{
		m_ptTile.x = 0;
		m_ptTile.y = 0;
	}
	virtual ~CMapItem()
	{

	}
	// 设置是否在小地图上显示
	virtual void SetVisibleOnMinmap(BOOL bVisible)
	{
	};
	// 取得是否在小地图上显示
	virtual BOOL GetVisibleOnMinmap(void)
	{
		return FALSE;
	}

protected:
	POINT	m_ptTile;
};
//------------------------------------------------------------------------
// 占位块列表管理类, 继承此类即可支持占位块列表。
class COccupant
{
public:

	// 读取占位块列表
	virtual COccupantTileList * GetOccupantTileList(void) const 
	{
		return m_pOccupantTileList; 
	}
	// 取得占位块信息
	virtual BOOL				OnSerialize(BOOL bWrite,IArchive* pArchive)
	{
		if(bWrite != TRUE)
		{
			if(m_pOccupantTileList)
			{
				return pArchive->Write((char*)m_pOccupantTileList,m_pOccupantTileList->GetMemSize());
			}
			else
			{ // 出错：无多占位对象。可能是因为对象尚未创建完成
				//TRACE("Error : COccupant::OnSerialize() : m_pOccupantTileList == NULL\r\n");
				return FALSE;
			}
		}
		else
		{ // 读盘
			COccupantTileList* pList = (COccupantTileList*)pArchive->GetBuffer();
			if(!SetOccupantTileList(pList))
				return FALSE;
			pArchive->Seek(pList->GetMemSize());
			return TRUE;
		}
	}
	// 设置占位块列表
	// 如果pList为NULL，表示清除当前占位块列表
	virtual BOOL				SetOccupantTileList(const COccupantTileList* pList)
	{
		if(m_pOccupantTileList)
		{
			delete m_pOccupantTileList;
			m_pOccupantTileList = NULL;
		}
		if(pList == NULL)
		{
			return TRUE;
		}

		if(!pList->IsValid())
		{
			//TRACE("Error : CStaticPicWithOccupant::SetOccupantTileList() : pList is invalid\r\n");
			return FALSE;
		}

		if(NULL == (m_pOccupantTileList = COccupantTileList::Create(pList->GetArraySize())))
		{
			return FALSE;
		}

		int nSize = pList->GetMemSize();
		memcpy(m_pOccupantTileList,pList,nSize);

		return TRUE;
	}

	COccupant(void)
	{
		m_pOccupantTileList = NULL;
	}
	virtual ~COccupant(void)
	{
		SetOccupantTileList(NULL);
	}
protected:
	COccupantTileList *			m_pOccupantTileList;
};
//------------------------------------------------------------------------