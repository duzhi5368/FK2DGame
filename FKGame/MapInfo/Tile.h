/**
*	created:		2012-6-21   23:01
*	filename: 		Tile
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "MapItem.h"
//------------------------------------------------------------------------
struct IArchive;
typedef CMultLenArray<DWORD,CMapItem*>		CLayerList;
//------------------------------------------------------------------------
/* 管理地表上的一块CTile

   说明：
	1、一个tile上管理一个CLayerList或一个CMapItem
	2、CLayerList数组 : 为解决循环访问数组时的删除问题，删除对象时仅将指针设置为空
*/ 
//------------------------------------------------------------------------
class CTile
{
public:
	CTile()
	{
		m_dwFlag = fInitData;		// 默认为空的无效块
		m_pLayerList = NULL;
	}
	~CTile()
	{
		Close();
	}
/*
                  0000 0000 0000 0000 0000 0000 0000 0000
              32 _|||| |||| |||| |||| |||| |||| |||| ||||_  1. 块是否有效
              31 __||| |||| |||| |||| |||| |||| |||| |||__  2. m_pMapItem指向唯一的对象，否则表示指针为m_pLayerList
              30 ___|| |||| |||| |||| |||| |||| |||| ||___  3.
              29 ____| |||| |||| |||| |||| |||| |||| |____  4.
              28 ______|||| |||| |||| |||| |||| ||||______  5. 地表是否格挡
              27 _______||| |||| |||| |||| |||| |||_______  6. 飞行在地表中是否格挡
              26 ________|| |||| |||| |||| |||| ||________  7. 地表的漫延格挡
              25 _________| |||| |||| |||| |||| |_________  8.
              24 ___________|||| |||| |||| |||1_____________9. 
									  |||| ||2______________10.
									  |||| |4_______________11.
									  |||| 8________________12.
									  |||1__________________13.
									  ||2___________________14.物体导致的行走格挡
									  |4____________________15.物体导致的漫延格挡
									  8___________________  16.物体导致飞行格挡性






*/
	enum 
	{
		maskValid = 0x1,														/*块是否有效*/

		/////////////////////// 行走格挡值 ////////////////////////
		// 0表示无格挡，值越大，格挡率越高
		maskBlock_Surface = 0x0010,												/*地表格挡值，地表的固定属性*/
		maskBlock_Item =    0x2000,												/*放置了地上物或特殊事件导致地表的格挡值(如某块地域被施法)*/
		maskBlock = (maskBlock_Surface | maskBlock_Item),						/*地表和物体共同决定此块的格挡值*/

		//////////////////////// 飞行格挡值 ///////////////////////
		// 0表示无格挡，1表示有格挡
		maskFlyBlock_Surface = 0x0020,											/*地表导致的飞行格挡值*/
		maskFlyBlock_Item =    0x8000,											/*物体导致的飞行格挡值,0表示无格挡，1表示有格挡*/
		maskFlyBlock = (maskFlyBlock_Surface | maskFlyBlock_Item),				/*查寻此前块飞行是否可通过，由地表和物体共同决定的*/

		//////////////////////// 漫延格挡值 ///////////////////////
		// 0表示无格挡，1表示有格挡，一般指地表或对象对气体、火焰的格挡
		maskDiffuseBlock_Surface = 0x0040,										/*地表导致的漫延格挡值*/
		maskDiffuseBlock_Item =    0x4000,										/*物体导致的漫延格挡值,0表示无格挡，1表示有格挡*/
		maskDiffuseBlock = (maskDiffuseBlock_Surface | maskDiffuseBlock_Item),	/*查寻此前块漫延是否可通过，由地表和物体共同决定的*/

		///////////////// 地上物的行走格挡、飞行格和漫延格挡挡统一处理////////
		// 地表的行走格挡、飞行格挡、漫延格挡
		maskOccupant_Surface = (maskBlock_Surface | maskFlyBlock_Surface | maskDiffuseBlock_Surface),
		// 地上物的行走格挡和飞行格挡
		maskOccupant_Item = (maskBlock_Item | maskFlyBlock_Item | maskDiffuseBlock_Item),
		// 地上物和地表的魔法格挡和行走格挡
		maskOccupant = maskOccupant_Surface | maskOccupant_Item,

		/////////////////////// 运行时态标志位 /////////////////////
		// maskTileType = 0x1FF0000, /*标识块类型，如草地、沙地等*/
		maskIsMapItem = 0x2,													/*m_pMapItem指向唯一的对象，否则表示指针为m_pLayerList*/
		maskIsLayerList =  0xFFFFFFFD,											/*m_pLayerList*/
		fInitData = 0,															/*初始值：无效块、可通过*/

		fMostTileValue = maskValid,												/*大部分块的值跟这个相同，因此存盘时，只存一个字节:0x04*/
		fMostTileValueFlag = 0x04,												// 这个字节表示最常见的地表：可通过有效块

	};
	//------------------------------------------------------------------------
	//定义策划写脚本的数值，把各种格档值与1、2、3、4、……对应
	enum		
	{
		masktype_block = 0x1,					// 行走阻挡
		masktype_fly,							// 飞行阻挡
		masktype_flood,							// 蔓延阻挡
		masktype_block_and_fly,					// 行走飞行阻挡
		masktype_block_and_flood,				// 行走和蔓延阻挡
		masktype_fly_and_flood,					// 飞行蔓延阻挡
		masktype_block_fly_flood,				// 行走飞行蔓延阻挡
	};
	//------------------------------------------------------------------------
	// 返回是否此块可通过
	// 块的通过性由地表和块上所有对象共同决定的
	DWORD IsBlock()const
	{
		return m_dwFlag & maskBlock;
	}
	DWORD IsNotBlock()const
	{
		return !(m_dwFlag & maskBlock);
	}
	// 返回此块是否有飞行格挡
	// 块的飞行格挡性由地表和块上所有对象共同决定
	DWORD IsFlyBlock()const
	{
		return m_dwFlag & maskFlyBlock;
	}
	// 返回此块是否有漫延格挡
	// 块的漫延格挡性由地表和块上所有对象共同决定
	DWORD IsDiffuseBlock()const
	{
		return m_dwFlag & maskDiffuseBlock;
	}
	// 地表是否有格挡
	DWORD IsBlock_Surface()const
	{
		return m_dwFlag & maskBlock_Surface;
	}
	// 设置地表有格挡
	void Set_Block_Surface(DWORD dwFlag = maskBlock_Surface)
	{
		m_dwFlag |= dwFlag & maskBlock_Surface;
	}
	// 清除地表格挡
	void Clear_Block_Surface()
	{
		m_dwFlag &= ~maskBlock_Surface;
	}
	// 是否为地上物导致格挡
	DWORD IsBlock_Item()const
	{
		return m_dwFlag & maskBlock_Item;
	}
	// 设置地上物导致格挡
	void Set_Block_Item(DWORD dwFlag = maskBlock_Item)
	{
		m_dwFlag |= dwFlag & maskOccupant_Item;
	}
	// 清除地上物格挡
	void Clear_Block_Item()
	{
		m_dwFlag &= ~maskBlock_Item;
	}
	// 是否为地上物导致飞行格挡
	DWORD IsFlyBlock_Surface()const
	{
		return m_dwFlag & maskFlyBlock_Surface;
	}
	// 设置地上物导致飞行格挡
	void Set_FlyBlock_Surface(DWORD dwFlag = maskFlyBlock_Surface)
	{
		m_dwFlag |= dwFlag & maskFlyBlock_Surface;
	}
	// 清除地上物飞行格挡
	void Clear_FlyBlock_Surface()
	{
		m_dwFlag &= ~maskFlyBlock_Surface;
	}
	// 是否为地上物导致飞行格挡
	DWORD IsFlyBlock_Item()const
	{
		return m_dwFlag & maskFlyBlock_Item;
	}
	// 设置地上物导致飞行格挡
	void Set_FlyBlock_Item(DWORD dwFlag = maskFlyBlock_Item)
	{
		m_dwFlag |= dwFlag & maskFlyBlock_Item;
	}
	// 清除地上物飞行格挡
	void Clear_FlyBlock_Item()
	{
		m_dwFlag &= ~maskFlyBlock_Item;
	}
	// 是否为地上物导致漫延格挡
	DWORD IsDiffuseBlock_Surface()const
	{
		return m_dwFlag & maskDiffuseBlock_Surface;
	}
	// 设置地上物导致漫延格挡
	void Set_DiffuseBlock_Surface(DWORD dwFlag = maskDiffuseBlock_Surface)
	{
		m_dwFlag |= dwFlag & maskDiffuseBlock_Surface;
	}
	// 清除地上物漫延格挡
	void Clear_DiffuseBlock_Surface()
	{
		m_dwFlag &= ~maskDiffuseBlock_Surface;
	}
	// 是否为地上物导致漫延格挡
	DWORD IsDiffuseBlock_Item()const
	{
		return m_dwFlag & maskDiffuseBlock_Item;
	}
	// 设置地上物导致漫延格挡
	void Set_DiffuseBlock_Item(DWORD dwFlag = maskDiffuseBlock_Item)
	{
		m_dwFlag |= dwFlag & maskDiffuseBlock_Item;
	}
	// 清除地上物漫延格挡
	void Clear_DiffuseBlock_Item()
	{
		m_dwFlag &= ~maskDiffuseBlock_Item;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	///////////////// occupant表示行走格挡和飞行格挡统一处理 ////////////////////////////

	// 地表的行走和飞行格挡值统一处理
	DWORD IsOccupant_Surface()const
	{
		return m_dwFlag & maskOccupant_Surface;
	}
	void Set_Occupant_Surface(DWORD dwFlag)
	{
		m_dwFlag |= dwFlag & maskOccupant_Surface;
	}
	void Clear_Occupant_Surface()
	{
		m_dwFlag &= ~maskOccupant_Surface;
	}
	DWORD IsOccupant_Item()const
	{
		return m_dwFlag & maskOccupant_Item;
	}
	void Set_Occupant_Item(DWORD dwFlag = maskOccupant_Item)
	{
		m_dwFlag |= dwFlag & maskOccupant_Item;
	}
	void Clear_Occupant_Item()
	{
		m_dwFlag &= ~maskOccupant_Item;
	}
	// 同上，占物块列表中保存的是一个BYTE数组，一个BYTE对应
	void Set_Occupant_Item_Byte(BYTE cbFlag)
	{
		m_dwFlag |= (((DWORD)cbFlag) << 8) & maskOccupant_Item;
	}
	void Clear_Occupant_Item_Byte(BYTE cbFlag = (BYTE)(maskOccupant >> 8))
	{
		m_dwFlag &= ~(((DWORD)cbFlag) << 8);
	}
	
	// 根据当前块上所有对象的情况更新格挡情况
	// 对块的对象增删以后调用此操作
	// 说明：只能根据此tile上的单块进行处理，无法将覆盖此块的多占位块占位信息更新到此块
	void OnUpdateItems()
	{
		//Clear_Occupant_Item();
		int nCount = GetLayerCount();
		for(int i = 0; i < nCount; i++)
			Set_Occupant_Item(GetLayer(i)->GetOccupantValue());
	}
	// 删除对象时，先清除此对象的占位信息，然后将此列上其它对象的占位信息更新到tile上
	void RemoveItemOccupant(DWORD dwFlag)
	{
		m_dwFlag &= ~dwFlag;
		int nCount = GetLayerCount();
		for(int i = 0; i < nCount; i++)
			Set_Occupant_Item(GetLayer(i)->GetOccupantValue());
	}
	int GetLayerCount() const
	{
		if(m_pMapItem == NULL)
			return 0;
		else if(m_dwFlag & maskIsMapItem)
			return 1;
		else 
			return m_pLayerList->GetCount();
	}
	// 当前只有一个结点且m_pMapItem指向一个CMapItem
	BOOL IsMapItemValid()
	{
		return m_dwFlag & maskIsMapItem;
	}
	CMapItem* GetLayer(int nIndex)
	{
		if(m_dwFlag & maskIsMapItem) // 此时只有一个结点，不理nIndex值
			return m_pMapItem;
		else
		{
			if (nIndex < m_pLayerList->GetCount())
				return (*m_pLayerList)[nIndex];
		}
		return NULL;
	}
	void SetOneLayer(CMapItem* pItem)
	{
		m_pMapItem = pItem;
		m_dwFlag |= maskIsMapItem;
	}
	void SetLayerList(CLayerList* pLL)
	{
		m_pLayerList = pLL;
		m_dwFlag &= maskIsLayerList;
	}
	void SetNoLayer()
	{
		m_pMapItem = NULL;
		m_dwFlag &= maskIsLayerList;
	}

	CMapItem* GetMapItem()
	{
		return m_pMapItem;
	}

	CLayerList* GetLayerList()const
	{
		return m_pLayerList;
	}

	// 直接获取指针地址，以便当数组变长时改变指针的值
	CLayerList** GetLayerListPointer()
	{
		return &m_pLayerList;
	}

	// 设置此块为不在有效区域内
	void SetInValid()
	{
		Close();
	}
	// 是否为有地图范围之内的有效块，如果m_dwFlag为0，表示此块为填充地图的无效块，用于使地图能用二维数据表示
	BOOL IsValid()const
	{
		return (BOOL) (m_dwFlag & maskValid);
	}

	// 如果是一个最常见的地表类型，即无地上物、可通过地表，则压缩存储
	BOOL IsMostTileValue() const
	{
		return m_dwFlag == fMostTileValue && GetLayerCount() == 0;
	}
	void SetMostValue()
	{
		m_dwFlag = fMostTileValue;
	}

	DWORD GetFlag() const
	{
		return m_dwFlag;
	}
	void SetFlag(DWORD dwFlag)
	{
		m_dwFlag = dwFlag;
	}

	// 检查pItem是否在此tile上
	BOOL FindItem(CMapItem* pItem)
	{
		int nCount = GetLayerCount();
		for(int i = 0; i < nCount; i++)
		{
			if(GetLayer(i) == pItem)
				return TRUE;
		}
		return FALSE;
	}

	void Create()
	{
		m_dwFlag = maskValid | fInitData;	// 有效块、可通过、无地上物、不可通过
		m_pLayerList = NULL;
	}
	void Close()
	{
		m_dwFlag = fInitData;
	}
	
	// bClearOccupantWhenRead 为TRUE时表于服务器端读地表信息时不能预先清除地表上的maskOccupant_Item
	BOOL OnSerialize(BOOL bWrite,IArchive* pArchive,BOOL bClearOccupantWhenRead)
	{
		/* 
			格式：
				wLayerNum + Layer0 + Layer1 + .......
		*/
		//enum{tileBlockFlag/*一个tile的起始标志*/ = 0x613};
		struct MAP_ARCHIVE_TILE_HEADER
		{
			//DWORD dwTileBlockFlag;
			DWORD dwFlag;
		};
		MAP_ARCHIVE_TILE_HEADER mt;

		if(bWrite)
		{ // 保存块对象CTile，以及块对象上的所有CMapItem
			MAP_ARCHIVE_TILE_HEADER mt;
			mt.dwFlag = m_dwFlag;
			// 写CTile配置
			pArchive->Write((char*)&mt,sizeof(mt));
		}
		else
		{ // 读取块对象CTile，以及块对象上的所有CMapItem
			pArchive->Read((char*)&mt,sizeof(mt));
			m_dwFlag = mt.dwFlag;
			// 用maskOccupant_Item清除地上物导致的占位，好处是：地上物的占位信息在上次保存地图以后可能已经发生改变
			// 加载地图时添加地上物对象会导致占位信息得到更新
			if(bClearOccupantWhenRead)
				m_dwFlag |= mt.dwFlag & (~CTile::maskOccupant_Item);
			
		}
		return TRUE;
	}

protected:
	DWORD		m_dwFlag;
	union
	{
		CLayerList*	m_pLayerList;
		CMapItem*	m_pMapItem;			// 只有一个对象时，由m_pMapItem管理，当有二层以上对象时，由m_pLayerList管理
										// 因为大部分块上只有一个对象
	};
};
//------------------------------------------------------------------------