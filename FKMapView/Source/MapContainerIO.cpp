/**
*	created:		2012-7-1   6:28
*	filename: 		MapContainerIO
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../Include/MapContainer.h"
#include "../../FKGame/SceneObj/IItemClassFactory.h"
//------------------------------------------------------------------------
// 如果不需要保存地图上对象的占位块信息，可打开下面的宏
//#define _NOT_LOAD_ITEM_OCCUPANT
//#define _NOT_SAVE_ITEM_OCCUPANT
//------------------------------------------------------------------------
void CMapContainer::ViewRectToPreReadRect(RECT& rcView, RECT& rcPreRead, int nMapWidth, int nMapHeight)
{
	rcPreRead.left = (rcView.right + rcView.left)/2 - (TILES_PREREAD_WIDTH)/2;
	rcPreRead.top = (rcView.bottom + rcView.top)/2 - (TILES_PREREAD_HEIGHT)/2;
	if (rcPreRead.left < 0)
		rcPreRead.left = 0;
	if (rcPreRead.top < 0)
		rcPreRead.top = 0;
	if (rcPreRead.left > nMapWidth - TILES_PREREAD_WIDTH)
		rcPreRead.left = nMapWidth - TILES_PREREAD_WIDTH;
	if (rcPreRead.top > nMapHeight - TILES_PREREAD_HEIGHT)
		rcPreRead.top = nMapHeight - TILES_PREREAD_HEIGHT;
	int nGridX = rcPreRead.left >> SHIFT_WIDTH;
	int nGridY = rcPreRead.top >> SHIFT_HEIGHT;
	rcPreRead.left = nGridX << SHIFT_WIDTH;
	rcPreRead.top = nGridY << SHIFT_HEIGHT;
	rcPreRead.right = rcPreRead.left + TILES_PREREAD_WIDTH;
	rcPreRead.bottom = rcPreRead.top + TILES_PREREAD_HEIGHT;
}
//------------------------------------------------------------------------
// 暂只实现了client端的存储，以后考虑服务器端实现
BOOL CMapContainer::OnSaveMap(IArchive* pArchive, LPCSTR szGroundJpgDir, ITrace* pTrace)
{
	g_DefaultTrace.Create(pTrace);	
	return OnSaveMap112(pArchive, szGroundJpgDir, GRID_WIDTH, GRID_HEIGHT, pTrace);

	if(!IsValid())
	{
		TraceLn("Error : CMapContainer::OnSerialize() : not Create() yet");
		return FALSE;
	}
	TraceLn(">>>>>>>>>>>>>>>>>>>> Write map data <<<<<<<<<<<<<<<<");

	MAP_ARCHIVE_HEADER* pMH = (MAP_ARCHIVE_HEADER*)pArchive->GetBuffer();
	// 1、写基本配置
	MAP_ARCHIVE_HEADER mh;
	mh.Init(m_nMapWidth,m_nMapHeight,szGroundJpgDir);
	pArchive->Write((char*)&mh,sizeof(MAP_ARCHIVE_HEADER));
	

	// 2、写各块上的数据
	for(int row = 0; row < m_nMatrixWidth; row++)
	{
		CTile* pTile = &m_pTiles[row * m_nMatrixWidth];
		int col = 0;
		for(;col < m_nMatrixWidth && !pTile->IsValid(); col++,pTile++);	// 排除前面无效结点

		// 保存格式：CTile + wItemCount + CMapItem0 + CMapItem1 + .....
		// CMapItem格式：dwClassID + wObjectLen + ObjectData
		MAP_ARHCIVE_LINE_HEADER* pMALH = (MAP_ARHCIVE_LINE_HEADER*)pArchive->GetBuffer();
		pMALH->nFromX = col;
		
		pArchive->Seek(sizeof(MAP_ARHCIVE_LINE_HEADER));

		int nTileCount = 0;
		for(;col < m_nMatrixWidth && pTile->IsValid();nTileCount++,col++,pTile++)
		{
			// 保存CTile
			if(pTile->IsMostTileValue())	// 如果是一个最常见的地表类型，即无地上物、可通过地表，则压缩存储
			{
				BYTE cbMostData = CTile::fMostTileValueFlag;
				pArchive->Write((char*)&cbMostData,1);
				continue;
			}

			pTile->OnSerialize(TRUE,pArchive,0);

			// 保存wItemCount
			WORD wItemCount = pTile->GetLayerCount();
			pArchive->Write((char*)&wItemCount,sizeof(wItemCount));

			// 保存CTile上的所有CMapItem数据
			for(WORD i = 0; i < wItemCount; i++)
			{
				CMapItem* pItem = pTile->GetLayer(i);

				// 保存dwClassID
				DWORD dwClassID = pItem->GetClassID();
				pArchive->Write((char*)&dwClassID,sizeof(dwClassID));

				// 这个偏移是对象长度dwObjectLen的位置，对象存储结束以后再计算对象长度
				WORD* pObjectLen = (WORD*)pArchive->GetBuffer();
				pArchive->Seek(sizeof(WORD));
				int nBeginOff = pArchive->GetBufferOffset();

				// 保存对象现场
				if(!pItem->OnSerialize(TRUE,pArchive))
					TraceLn("Error : CMapContainer::OnSerialize() : OnSerialize() error,dwClassID = %d,@Tile(%d,%d)",dwClassID,col,row);

				// 保存对象长度dwObjectLen
				*pObjectLen = (WORD)(pArchive->GetBufferOffset() - nBeginOff);
			}
		}
		pMALH->nTileCount = nTileCount;
	}
	pMH->dwSize = (DWORD)((char*)pArchive->GetBuffer() - (char*)pMH);
	TraceLn(">>>>>>>>>>>>>>>>>>>> End map data <<<<<<<<<<<<<<<<");
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::OnLoadMap(IArchive* pArchive,IItemClassFactory* pItemCF, 
							  LPRECT lprcViewport, ITrace* pTrace)
{
	g_DefaultTrace.Create(pTrace);
	
	// 读取地图文件头
	MAP_ARCHIVE_HEADER _mah;
	pArchive->Read((char*)&_mah, sizeof(MAP_ARCHIVE_HEADER));
	if (!_mah.IsValid())
	{
		return FALSE;
	}
	lstrcpy(m_szGround, _mah.szGroundJpgDir);
	pArchive->SeekTo(0);
	if (_mah.dwVersion == 0x00010102)
		return OnLoadMap112(pArchive, pItemCF, lprcViewport, pTrace);
	// 下面读取1.11版的格式
	if (IsUseDynamicMap())
	{
		TraceLn( "地图文件版本太低，不支持动态地图");
		return FALSE;
	}
	TraceLn(">>>>>>>>>>>>>>>>>>>> read map data <<<<<<<<<<<<<<<<");

	MAP_ARCHIVE_HEADER mh;
	pArchive->Read((char*)&mh,sizeof(MAP_ARCHIVE_HEADER));
	if(!mh.IsValid())
	{
		TraceLn("Error : CMapContainer::OnSerialize() map file is not valid,Version = %06X",mh.dwVersion);
		return FALSE;
	}

	// 将地图的宽高进行64(32)对齐处理
	mh.nMapWidth &= ~63;
	mh.nMapHeight &= ~31;

	RECT rcTilesPreRead;
	if (IsUseDynamicMap())
		ViewRectToPreReadRect(*lprcViewport, rcTilesPreRead, mh.nMapWidth, mh.nMapHeight);
	else
		SetRect(&rcTilesPreRead, 0, 0, mh.nMapWidth, mh.nMapHeight);
	if (!Create(mh.nMapWidth, mh.nMapHeight, rcTilesPreRead, pTrace))
		return FALSE;
	for(int row = 0; row < m_nMatrixWidth; row++)
	{
		MAP_ARHCIVE_LINE_HEADER malh;
		if(!pArchive->Read(&malh,sizeof(malh)))
			break;
		if(malh.nFromX < 0 || malh.nFromX >= m_nMatrixWidth || malh.nTileCount < 0 || malh.nTileCount >= m_nMatrixWidth)
			break;
		CTile* pTile = &m_pTiles[row * m_nMatrixWidth + malh.nFromX];
		int col = malh.nFromX;
		for(int t = 0 ;t < malh.nTileCount ; t++,col++,pTile++)
		{
			// 读取CTile
			BYTE cbMostData = 0;
			pArchive->Read((char*)&cbMostData,1);
			if(cbMostData == CTile::fMostTileValueFlag)	// 如果是一个最常见的地表类型，即无地上物、可通过地表，则压缩存储
			{
				pTile->SetMostValue();
				continue;
			}
			pArchive->Seek(-1);

			if(!pTile->OnSerialize(FALSE,pArchive,TRUE))
				break;
			
			// 读取wItemCount
			WORD wItemCount = 0;
			pArchive->Read(&wItemCount,sizeof(wItemCount));
			// 读取CTile上的所有CMapItem数据
			for(WORD wItem = 0; wItem < wItemCount;wItem++)
			{
				// 读取dwClassID
				DWORD dwClassID = 0;
				if(!pArchive->Read((char*)&dwClassID,sizeof(dwClassID)))
					break;
				WORD wObjectLen = 0;
				if(!pArchive->Read((char*)&wObjectLen,sizeof(wObjectLen)))
					break;
				
				int nBeginOff = pArchive->GetBufferOffset();	// 记住读取对象前的偏移
				// 创建对象
				CMapItem* pItem = pItemCF->CreateItem(dwClassID,pArchive);
				POINT pt = {malh.nFromX + t,row};
				if(pItem == NULL)
				{
					TraceLn("Error : CMapContainer::OnSerialize() : dwClassID = %d,@Tile(%d,%d)",dwClassID,pt.x,pt.y);
				}
				else
				{
					if (!AddItem(pt, pItem))
						pItem->Release();
					else
						m_MapItemList.push_back(pItem);
					// END
					if (pArchive->GetBufferOffset() != nBeginOff + wObjectLen)
					{
						TraceLn("Error : CMapContainer::OnSerialize() : current archive offset = %d,but %d + %d wanted,dwClassID = %d,@Tile(%d,%d)",
							pArchive->GetBufferOffset(),nBeginOff,wObjectLen,dwClassID,pt.x,pt.y);
						pArchive->SeekTo(nBeginOff + wObjectLen);	// 强制将指针移动到对象数据长度之后的位置，防止对象多读或少读数据
					}
				}
					
			}
		}
	}
	TraceLn(">>>>>>>>>>>>>>>>>>>> End map data <<<<<<<<<<<<<<<<");
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::OnLoadMap112(IArchive* pArchive,IItemClassFactory* pItemCF,LPRECT lprcViewport, ITrace* pTrace)
{
	m_pArchive = pArchive;
	m_pItemCF = pItemCF;
	TraceLn(">>>>>>>>>>>>>>>>>>>> 开始读取地图数据 <<<<<<<<<<<<<<<<");
#ifdef _DEBUG
	DWORD dwStart = GetTickCount();
#endif
	// 读取地图文件头
	MAP_ARCHIVE_HEADER mh;
	int nMapOffset = pArchive->GetBufferOffset();
	pArchive->Read((char*)&mh, sizeof(MAP_ARCHIVE_HEADER));
	if (!mh.IsValid())
	{
		return FALSE;
	}
	
	// 将地图的宽高进行64(32)对齐处理
	mh.nMapWidth &= ~63;
	mh.nMapHeight &= ~31;

	// 视口校验
	RECT rcRect = {0, 0, mh.nMapWidth, mh.nMapHeight};
	if (!IntersectRect(&rcRect, &rcRect, lprcViewport))
	{
		::MessageBox(NULL, "CMapView::OnLoadMap112(), 传入的视区不在地图范围之内", "ERROR", MB_OK);
		return FALSE;
	}

	// 裁剪处理
	if (lprcViewport->left < 0)
	{
		lprcViewport->left = 0;
		lprcViewport->right = 800;
	}
	if (lprcViewport->right > mh.nMapWidth)
	{
		lprcViewport->right = mh.nMapWidth;
		lprcViewport->left = mh.nMapWidth - 800;
	}
	if (lprcViewport->top < 0)
	{
		lprcViewport->top = 0;
		lprcViewport->bottom = 600;
	}
	if (lprcViewport->bottom > mh.nMapHeight)
	{
		lprcViewport->bottom = mh.nMapHeight;
		lprcViewport->top = mh.nMapHeight - 600;
	}
	
	// 创建地图
	RECT rcTilesPreRead;
	if (IsUseDynamicMap())
		ViewRectToPreReadRect(*lprcViewport, rcTilesPreRead, mh.nMapWidth, mh.nMapHeight);
	else
		SetRect(&rcTilesPreRead, 0, 0, mh.nMapWidth, mh.nMapHeight);
	if (!Create(mh.nMapWidth, mh.nMapHeight, rcTilesPreRead, pTrace))
		return FALSE;
	
	// 网格行列数
	int nGridRow = (m_nMapHeight - 1)/mh.nGridHeight + 1;
	int nGridCol = (m_nMapWidth - 1)/mh.nGridWidth + 1;
	TraceLn("本地图行数 = %d 列数 = %d", nGridRow, nGridCol );
	
	// 2、记录映射表地址,映射表中记录每个网格块相对于文件头的偏移
	m_pMapTable = (DWORD*)pArchive->GetBuffer();
	
	// 3、读取每个网格块的信息
	if (!IsUseDynamicMap())
	{
		int nTileRow = mh.nGridHeight / 32;
		int nTileCol = mh.nGridWidth / (64/2);
		POINT ptTileLeftTop;
		POINT ptLeftTop;
		for (int row=0; row<nGridRow; row++)
		{
			for (int col=0; col<nGridCol; col++)
			{
				// 定位每个网格块的位置
				pArchive->SeekTo(m_pMapTable[row*nGridCol+col] + nMapOffset);
				
				ptLeftTop.x = col*mh.nGridWidth;
				ptLeftTop.y = row*mh.nGridHeight;
				
				m_MapCo.PixelToTile(ptLeftTop, ptTileLeftTop);			
				if (!_LoadBlock(pArchive, ptTileLeftTop, nTileRow, nTileCol, pItemCF))
					return FALSE;
			}
		}	
	}
#ifdef _DEBUG
	TraceLn("本地图加载消耗时间 = %d", GetTickCount()-dwStart);
#endif	
	TraceLn(">>>>>>>>>>>>>>>>>>>> 读取地图数据完成 <<<<<<<<<<<<<<<<");
	return TRUE;
}
//------------------------------------------------------------------------
// 用于在本地图上加入另外一个跟本地图同样大小的地图文件
BOOL CMapContainer::OnAddMap112(IArchive* pArchive, IItemClassFactory* pItemCF, LPRECT lprcViewport, ITrace* pTrace)
{
	TraceLn(">>>>>>>>>>>>>>>>>>>> read map data <<<<<<<<<<<<<<<<");
	// 读取地图文件头
	MAP_ARCHIVE_HEADER mh;
	int nMapOffset = pArchive->GetBufferOffset();
	pArchive->Read((char*)&mh, sizeof(MAP_ARCHIVE_HEADER));
	if (!mh.IsValid())
	{
		TraceLn("Error : CMapContainer::OnSerialize() map file is not valid,Version = %06X",mh.dwVersion);
		return FALSE;
	}
	if (mh.nMapWidth != m_nMapWidth || mh.nMapHeight != m_nMapHeight)
	{
		TraceLn("加入的地图文档与当前的地图文档大小不匹配!");
		return FALSE;
	}
	
	// 网格行列数
	int nGridRow = (m_nMapHeight - 1)/mh.nGridHeight + 1;
	int nGridCol = (m_nMapWidth - 1)/mh.nGridWidth + 1;
	
	// 2、记录映射表地址,映射表中记录每个网格块相对于文件头的偏移
	DWORD* pMapTable = (DWORD*)pArchive->GetBuffer();
	
	// 3、读取每个网格块的信息
	if (!IsUseDynamicMap())
	{
		int nTileRow = mh.nGridHeight / 32;
		int nTileCol = mh.nGridWidth / (64/2);
		POINT ptTileLeftTop;
		POINT ptLeftTop;
		for (int row=0; row<nGridRow; row++)
		{
			for (int col=0; col<nGridCol; col++)
			{
				// 定位每个网格块的位置
				pArchive->SeekTo(pMapTable[row*nGridCol+col] + nMapOffset);
				
				ptLeftTop.x = col*mh.nGridWidth;
				ptLeftTop.y = row*mh.nGridHeight;
				
				m_MapCo.PixelToTile(ptLeftTop, ptTileLeftTop);
				if (!_LoadBlock(pArchive, ptTileLeftTop, nTileRow, nTileCol, pItemCF))
					return FALSE;
			}
		}	
	}
	
	TraceLn(">>>>>>>>>>>>>>>>>>>> End map data <<<<<<<<<<<<<<<<");
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::_LoadBlock(IArchive* pArchive, POINT ptTileLeftTop, 
							   int nTileRow, int nTileCol, IItemClassFactory* pItemCF)
{
	POINT ptCurTile = ptTileLeftTop;
	BOOL bIsEmptyTile = FALSE;
	int nCount = 0;
	CTile* pTile = NULL;

	for (int row=0; row<nTileRow; row++) // 一个“之”字行
	{
		ptCurTile.x = ptTileLeftTop.x + row;
		ptCurTile.y = ptTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++) // 列数
		{
			if (nCount == 0) // 读取数据
			{
				BYTE btyData;
				pArchive->Read(&btyData, sizeof(btyData));
				if (btyData & 0x80) // 连续的非空TILE
				{
					bIsEmptyTile = FALSE;
					nCount = btyData - 128 + 1;
				}
				else // 连续的空TILE
				{
					bIsEmptyTile = TRUE;
					nCount = btyData + 1;
				}
			}

			pTile = GetTilePtr(ptCurTile);
			if (pTile)
			{
				if (!bIsEmptyTile)
					_LoadTileInfo(pArchive, pTile, ptCurTile, pItemCF);
				nCount --;
			}
			
			ptCurTile.x += !(col&1); // odd
			ptCurTile.y += col&1;
		}
	}

	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::_LoadTileInfo(IArchive* pArchive, CTile* pTile, 
								  POINT& ptTile, IItemClassFactory* pItemCF)
{
	// 读取 Flags (1/2 bytes)
#ifdef _NOT_LOAD_ITEM_OCCUPANT
	BYTE nFlags = 0;
#else
	WORD nFlags = 0;
#endif
	pArchive->Read((char*)&nFlags, sizeof(nFlags));
	DWORD dwFlags = (DWORD)nFlags;
	dwFlags |= pTile->GetFlag();
	pTile->SetFlag(dwFlags);
	
	// 读取ItemCount (1 byte)
	BYTE nItemCount = 0;
	pArchive->Read((char*)&nItemCount, sizeof(nItemCount));
	
	// 保存CTile上的所有CMapItem数据
	for (int i=0; i<nItemCount; i++)
	{
		// 读取dwClassID (1 bytes)
		BYTE nClassID = 0;
		if (!pArchive->Read((char*)&nClassID, sizeof(nClassID)))
			break;

		int nBeginOff = pArchive->GetBufferOffset();	// 记住读取对象前的偏移

		if (pItemCF == NULL) // for server
		{
			pArchive->Seek(sizeof(DWORD)); // 只记录了一个ID
		}
		else
		{
			// 创建对象
			CMapItem* pItem = pItemCF->CreateItem((DWORD)nClassID, pArchive);
			if (pItem == NULL)
			{
				if ((DWORD)pArchive->GetBufferOffset() != (nBeginOff + sizeof(DWORD))) // 只记录了一个ID
				{
					pArchive->SeekTo(nBeginOff + sizeof(DWORD));	// 强制将指针移动到对象数据长度之后的位置，防止对象多读或少读数据
				}
				continue;
			}
			else
			{
#ifdef _DEBUG
				if ((DWORD)pArchive->GetBufferOffset() != (nBeginOff + sizeof(DWORD))) // 只记录了一个ID
				{
					TraceLn("Error: CMapContainer::_LoadTileInfo, CreateItem() read data size error, dwClassID=%d,@Tile(%d,%d)",
						nClassID,ptTile.x,ptTile.y);
					pArchive->SeekTo(nBeginOff + sizeof(DWORD));	// 强制将指针移动到对象数据长度之后的位置，防止对象多读或少读数据
				}
#endif
				if (!AddItem(ptTile, pItem))
					pItem->Release();
				else
					m_MapItemList.push_back(pItem);
			}
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::OnSaveMap112(IArchive* pArchive,LPCSTR szGroundJpgDir,
								int nGridWidth, int nGridHeight, ITrace* pTrace)
{
	g_DefaultTrace.Create(pTrace);
	if ((nGridWidth&63) || (nGridHeight&31))
	{
		TraceLn("网格尺寸必须是64,32的整数倍");
		return FALSE;
	}

	if(!IsValid())
	{
		TraceLn("Error : CMapContainer::OnSerialize() : not Create() yet");
		return FALSE;
	}

	// 网格行列数
	int nGridRow = PixelCood2GridCood(m_nMapHeight, nGridHeight);
	int nGridCol = PixelCood2GridCood(m_nMapWidth, nGridWidth);

	
	MAP_ARCHIVE_HEADER* pMH = (MAP_ARCHIVE_HEADER*)pArchive->GetBuffer();
	int nMapOffset = pArchive->GetBufferOffset();
	// 1、保存地图文件头
	MAP_ARCHIVE_HEADER mh;
	mh.Init(m_nMapWidth,m_nMapHeight,szGroundJpgDir);
	mh.nGridWidth = nGridWidth;
	mh.nGridHeight = nGridHeight;
	pArchive->Write((char*)&mh,sizeof(MAP_ARCHIVE_HEADER));

	// 2、预留映射表,映射表中记录每个网格块相对于文件头的偏移
	int nMapTableItemCount = nGridRow*nGridCol;
	DWORD* pMapTablePos = (DWORD*)pArchive->GetBuffer();
	pArchive->Seek(nMapTableItemCount*sizeof(DWORD));

	// 3、保存每个网格块的信息
	int nTileRow = nGridHeight / 32;
	int nTileCol = nGridWidth / (64/2);
	POINT ptTileLeftTop;
	POINT ptLeftTop;
	for (int row=0; row<nGridRow; row++)
	{
		for (int col=0; col<nGridCol; col++)
		{
			// 更新映射表内容
			pMapTablePos[row*nGridCol+col] = pArchive->GetBufferOffset() - nMapOffset;

			ptLeftTop.x = col*nGridWidth;
			ptLeftTop.y = row*nGridHeight;

			m_MapCo.PixelToTile(ptLeftTop, ptTileLeftTop);
			if (!_SaveBlock(pArchive, ptTileLeftTop, nTileRow, nTileCol))
				return FALSE;
		}
	}

	pMH->dwSize = (DWORD)((char*)pArchive->GetBuffer() - (char*)pMH);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::_SaveBlock(IArchive* pArchive, POINT ptTileLeftTop, int nTileRow, int nTileCol)
{
	POINT ptCurTile = ptTileLeftTop;
	BOOL bIsEmptyTile = FALSE;
	int nCount = 0;
	CTile* pTile=NULL;
	POINT ptTileList[130];
	for (int row=0; row<nTileRow; row++) // 一个“之”字行
	{
		ptCurTile.x = ptTileLeftTop.x + row;
		ptCurTile.y = ptTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++) // 列数
		{
			pTile = GetTilePtr(ptCurTile);
			if (pTile)
			{
				if (pTile->IsMostTileValue()
#ifdef _NOT_SAVE_ITEM_OCCUPANT
					|| (pTile->IsOccupant_Item()&&
					!pTile->IsOccupant_Surface()&&
					pTile->GetLayerCount()==0)
#endif
					) // 空TILE
				{
					if (bIsEmptyTile) // 重复
					{
						nCount++;
						if (nCount >= 128)
						{
							_SaveMultiEmptyTileInfo(pArchive, nCount);
							nCount = 0;
						}
					}
					else
					{
						// 记录非空TILE信息
						_SaveMultiTileInfo(pArchive, ptTileList, nCount);
						nCount = 1;
					}
					bIsEmptyTile = TRUE;
				}
				else // 非空TILE
				{
					if (bIsEmptyTile)
					{
						_SaveMultiEmptyTileInfo(pArchive, nCount);
						nCount = 0;
						ptTileList[nCount++] = ptCurTile;
					}
					else
					{
						// Write non-empty tile info
						ptTileList[nCount] = ptCurTile;
						nCount ++;
						if (nCount >= 128)
						{
							_SaveMultiTileInfo(pArchive, ptTileList, nCount);
							nCount = 0;
						}
					}
					bIsEmptyTile = FALSE;
				}
			}
			
			ptCurTile.x += !(col&1); // odd
			ptCurTile.y += col&1;
		}
	}
	
	// 最后一次的数据
	if (bIsEmptyTile)
		_SaveMultiEmptyTileInfo(pArchive, nCount);
	else
		_SaveMultiTileInfo(pArchive, ptTileList, nCount);
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::_SaveMultiEmptyTileInfo(IArchive* pArchive, int nCount)
{
	if (nCount == 0)
		return TRUE;
	if (nCount > 128)
	{
		TraceLn("CMapContainer::_SaveMultiEmptyTileInfo, repeat count more than 128!");
		return FALSE;
	}
	// 先写入连续空TILE的个数(偏移了-1,为了用0~127表达1~128)
	BYTE bytCount = nCount-1;
	pArchive->Write((char*)&bytCount, sizeof(bytCount));
	return TRUE;
}
//------------------------------------------------------------------------
BOOL CMapContainer::_SaveMultiTileInfo(IArchive* pArchive, POINT* ptTileList, int nCount)
{
	if (nCount == 0)
		return TRUE;
	if (nCount > 128)
	{
		TraceLn("CMapContainer::_SaveMultiTileInfo, repeat count more than 128!");
		return FALSE;
	}

	// 先写入连续非空TILE的个数(偏移了127,为了用128~255表达1~128,最高位表明是非空TILE)
	BYTE bytCount = nCount-1 + 128;
	pArchive->Write((char*)&bytCount,sizeof(bytCount));

	for (int i=0; i<nCount; i++)
	{
		CTile* pTile = GetTilePtr(ptTileList[i]);
		if (pTile)
			if (!_SaveTileInfo(pArchive, pTile))
				return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 保存格式：CTile(1/2bytes) + nItemCount(1byte) + CMapItem0(5bytes) + CMapItem1(5bytes) + ...
// CMapItem格式(5bytes)：nClassID(1byte) + ObjectID(4bytes)
BOOL CMapContainer::_SaveTileInfo(IArchive* pArchive, CTile* pTile)
{
	// 保存 Flags (1/2 bytes)
	DWORD dwFlags = pTile->GetFlag();
#ifdef _NOT_SAVE_ITEM_OCCUPANT
	dwFlags &= ~CTile::maskOccupant_Item; // 去掉占位块上item的值
	BYTE nFlags = (BYTE)dwFlags;
#else
	WORD nFlags = (WORD)dwFlags;
#endif
	pArchive->Write((char*)&nFlags, sizeof(nFlags));
	
	// 保存ItemCount (1 byte)
	BYTE nItemCount = pTile->GetLayerCount();
#ifdef _DEBUG
	BYTE* pCount = (BYTE*)pArchive->GetBuffer();
#endif
	pArchive->Write((char*)&nItemCount, sizeof(nItemCount));
	
	// 保存CTile上的所有CMapItem数据
	for (int i=0; i<nItemCount; i++)
	{
		CMapItem* pItem = pTile->GetLayer(i);
		
		// 保存dwClassID (1 bytes)
		BYTE nClassID = (BYTE)pItem->GetClassID();
#ifdef _DEBUG
		switch (nClassID) 
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 7:
		case 8:
		case 9:
		case 13:
		case 14:
		case 15:
		case 16:
		case 27:
		case 29:
			break;
		default: // 其他对象不允许保存到地图上
			{
				(*pCount) --;
				continue;
			}	
			break;
		}
#endif
		pArchive->Write((char*)&nClassID, sizeof(nClassID));
		
		int nBeginOffset = pArchive->GetBufferOffset();

		// 保存对象现场(只记录ID，因为对象的占位块信息可能在外部要修改) (4 bytes)
		if (!pItem->OnSerialize(TRUE, pArchive))
		{
			TraceLn("Error : CMapContainer::_SaveTileInfo, OnSerialize() error,dwClassID = %d",nClassID);
			return FALSE;
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------
// 服务器读取地表信息
BOOL CMapContainer::OnReadServerMap(IArchive *pArchive, ITrace *pTrace)
{
	return OnReadServerMap112(pArchive, pTrace);
	g_DefaultTrace.Create(pTrace);
	TraceLn(">>>>>>>>>>>>>>>>>>>> read map data <<<<<<<<<<<<<<<<");
	
	MAP_ARCHIVE_HEADER mh;
	pArchive->Read((char*)&mh,sizeof(MAP_ARCHIVE_HEADER));
	if(!mh.IsValid())
	{
		TraceLn("Error : CMapContainer::OnReadServerMap() map file is not valid,Version = %06X",mh.dwVersion);
		return FALSE;
	}
	
	RECT rcTilesPreRead = {0, 0, mh.nMapWidth, mh.nMapHeight};
	if (!Create(mh.nMapWidth, mh.nMapHeight, rcTilesPreRead, NULL))
		return FALSE;
	int row = 0;
	for(; row < m_nMatrixWidth; row++)
	{
		MAP_ARHCIVE_LINE_HEADER malh;
		if(!pArchive->Read(&malh,sizeof(malh)))
			break;
		if(malh.nFromX < 0 || malh.nFromX >= m_nMatrixWidth || malh.nTileCount < 0 || malh.nTileCount >= m_nMatrixWidth)
			break;
		CTile* pTile = &m_pTiles[row * m_nMatrixWidth + malh.nFromX];
		int col = malh.nFromX;
		for(int t = 0 ;t < malh.nTileCount ; t++,col++,pTile++)
		{
			// 读取CTile
			BYTE cbMostData = 0;
			pArchive->Read((char*)&cbMostData,1);
			if(cbMostData == CTile::fMostTileValueFlag)	// 如果是一个最常见的地表类型，即无地上物、可通过地表，则压缩存储
			{
				pTile->SetMostValue();
				continue;
			}
			pArchive->Seek(-1);
			
			if(!pTile->OnSerialize(FALSE,pArchive,FALSE))
				break;
			
			// 读取wItemCount
			WORD wItemCount = 0;
			pArchive->Read(&wItemCount,sizeof(wItemCount));
			// 读取CTile上的所有CMapItem数据
			for(WORD wItem = 0; wItem < wItemCount;wItem++)
			{
				// 读取dwClassID
				DWORD dwClassID = 0;
				if(!pArchive->Read((char*)&dwClassID,sizeof(dwClassID)))
					break;
				WORD wObjectLen = 0;
				if(!pArchive->Read((char*)&wObjectLen,sizeof(wObjectLen)))
					break;
				// 服务器不需要创建客户端item，直接跳过，将来可能需要设定一些服务器专用的item
				//........
				pArchive->Seek(wObjectLen);	
			}
		}
	}
	if(row < m_nMatrixWidth)
	{
		TraceLn("CMapContainer::OnReadServerMap(sRead) failed!!");
		return FALSE;
	}
	TraceLn(">>>>>>>>>>>>>>>>>>>> End map data <<<<<<<<<<<<<<<<");
	return TRUE;	
}
//------------------------------------------------------------------------
// 服务器读取地表信息
BOOL CMapContainer::OnReadServerMap112(IArchive *pArchive, ITrace *pTrace)
{
	m_pArchive = pArchive;
	m_pItemCF = NULL;
	TraceLn(">>>>>>>>>>>>>>>>>>>> read map data <<<<<<<<<<<<<<<<");
#ifdef _DEBUG
	DWORD dwStart = GetTickCount();
#endif
	// 读取地图文件头
	MAP_ARCHIVE_HEADER mh;
	int nMapOffset = pArchive->GetBufferOffset();
	pArchive->Read((char*)&mh, sizeof(MAP_ARCHIVE_HEADER));
	if (!mh.IsValid())
	{
		TraceLn("Error : CMapContainer::OnReadServerMap112() map file is not valid,Version = %06X",mh.dwVersion);
		return FALSE;
	}
	
	// 将地图的宽高进行64(32)对齐处理
	mh.nMapWidth &= ~63;
	mh.nMapHeight &= ~31;
	
	// 创建地图
	RECT rcTilesPreRead = {0, 0, mh.nMapWidth, mh.nMapHeight};
	if (!Create(mh.nMapWidth, mh.nMapHeight, rcTilesPreRead, pTrace))
		return FALSE;
	
	// 网格行列数
	int nGridRow = (m_nMapHeight - 1)/mh.nGridHeight + 1;
	int nGridCol = (m_nMapWidth - 1)/mh.nGridWidth + 1;
	
	// 2、记录映射表地址,映射表中记录每个网格块相对于文件头的偏移
	m_pMapTable = (DWORD*)pArchive->GetBuffer();
	
	// 3、读取每个网格块的信息
	int nTileRow = mh.nGridHeight / 32;
	int nTileCol = mh.nGridWidth / (64/2);
	POINT ptTileLeftTop;
	POINT ptLeftTop;
	for (int row=0; row<nGridRow; row++)
	{
		for (int col=0; col<nGridCol; col++)
		{
			// 定位每个网格块的位置
			pArchive->SeekTo(m_pMapTable[row*nGridCol+col] + nMapOffset);
			
			ptLeftTop.x = col*mh.nGridWidth;
			ptLeftTop.y = row*mh.nGridHeight;
			
			m_MapCo.PixelToTile(ptLeftTop, ptTileLeftTop);
			if (!_LoadBlock(pArchive, ptTileLeftTop, nTileRow, nTileCol, NULL))
				return FALSE;
		}
	}	
#ifdef _DEBUG
	TraceLn("time = %d", GetTickCount()-dwStart);
#endif	
	TraceLn(">>>>>>>>>>>>>>>>>>>> End map data <<<<<<<<<<<<<<<<");
	return TRUE;
}
//------------------------------------------------------------------------
/*
 A → B
 dx=0,dy<0
 (0,0)
┌─────────┐
│                B │
│(0,-dy)           │
├─────────┤
│╳╳╳╳╳╳╳╳╳│
├─────────┤(kx,ky)
│                  │
│A                 │
└─────────┘(nCols,nRows)
 kx=nCols;
 ky=nRows+dy;
*/
//------------------------------------------------------------------------
void CMapContainer::_MovePreReadRect_Top(BLOCK_INFO* pBlockInfo, int nRows, int nCols, 
				 int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	// 行[0,-dy,kx,ky)的块信息向后移动(-dy)行
	int kx = nCols;
	int ky = nRows+dy;
	int dy_nCols = dy*nCols;
	int row_nCols = (ky-1)*nCols;
	for (row=ky-1; row>=(-dy); row--,row_nCols-=nCols)
	{
		for (col=0; col<nCols; col++)
		{
			CopyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+col].ptLeftTop, nTileRow, nTileCol);
		}
	}

	// 更新原点
	SetOriginTile(ptNewTileOrigin);

	// 前(-dy)行加载新的信息
	row_nCols = 0;
	for (row=0; row<(-dy); row++,row_nCols+=nCols)
	{
		for (col=0; col<nCols; col++)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
//------------------------------------------------------------------------
/*
 A → B
 dx>0,dy<0
 (0,0)
┌┄┄┄┄┌─────────┐
┆        │                B │
┆        │(dx,-dy)          │
┌────┼────┐        │
│        │╳╳╳╳│        │
│        └────┼────┘
│                  │(kx,ky) ┆
│A                 │        ┆
└─────────┘┄┄┄┄┘(nCols,nRows)
 kx=nCols-dx;
 ky=nRows+dy;
*/
//------------------------------------------------------------------------
void CMapContainer::_MovePreReadRect_RightTop(BLOCK_INFO* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	// 区域[dx,-dy,kx,ky)的块信息向左下移动(-dy)行(dx列)
	int kx = nCols-dx;
	int ky = nRows+dy;
	int dy_nCols = dy*nCols;
	int row_nCols;
	for (col=dx; col<kx; col++)
	{
		row_nCols = -dy_nCols;
		for (row=-dy; row<ky; row++,row_nCols+=nCols)
		{
			CopyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+(col-dx)].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	// 更新原点
	SetOriginTile(ptNewTileOrigin);
	
	// 最后dx列加载新的信息
	for (col=kx; col<nCols; col++)
	{
		row_nCols = 0;
		for (row=0; row<ky; row++,row_nCols+=nCols)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}

	// 前(-dy)行加载新的信息
	row_nCols = 0;
	for (row=0; row<(-dy); row++,row_nCols+=nCols)
	{
		for (col=dx; col<kx; col++)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
//------------------------------------------------------------------------
/*
 A → B
 dx>0,dy=0
 (0,0)   (dx,0)
┌────┬────┬────┐
│        │╳╳╳╳│       B│
│        │╳╳╳╳│        │
│        │╳╳╳╳│        │
│A       │╳╳╳╳│(kx,ky) │
└────┴────┴────┘(nCols,nRows)
 kx=nCols-dx;
 ky=nRows;
*/
//------------------------------------------------------------------------
void CMapContainer::_MovePreReadRect_Right(BLOCK_INFO* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	// 区域[dx,0,kx,ky)的块信息向左移动dx列
	int kx = nCols-dx;
	int ky = nRows;
	int row_nCols = 0;
	for (col=dx; col<kx; col++)
	{
		row_nCols = 0;
		for (row=0; row<ky; row++,row_nCols+=nCols)
		{
			CopyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+(col-dx)].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	// 更新原点
	SetOriginTile(ptNewTileOrigin);
	
	// 右边dx列加载新的信息
	for (col=kx; col<nCols; col++)
	{
		row_nCols = 0;
		for (row=0; row<ky; row++,row_nCols+=nCols)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
//------------------------------------------------------------------------
/*
 A → B
 dx>0,dy>0
 (0,0)
┌─────────┐┄┄┄┄┐
│A                 │        ┆
│      (dx,dy)     │        ┆
│        ┌────┼────┐
│        │╳╳╳╳│        │
└────┼────┘(kx,ky) │
┆        │                  │
┆        │                 B│
└┄┄┄┄┴─────────┘(nCols,nRows)
 kx=nCols-dx;
 ky=nRows-dy;
*/
//------------------------------------------------------------------------
void CMapContainer::_MovePreReadRect_RightDown(BLOCK_INFO* pBlockInfo, int nRows, int nCols,
				int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	// 区域[dx,dy,kx,ky)的块信息向左上移动dx列(dy行)
	int kx = nCols - dx;
	int ky = nRows - dy;
	int dy_nCols = dy*nCols;
	int row_nCols;
	for (col=dx; col<kx; col++)
	{
		row_nCols = dy_nCols;
		for (row=dy; row<ky; row++,row_nCols+=nCols)
		{
			CopyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+(col-dx)].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	// 更新原点
	SetOriginTile(ptNewTileOrigin);
	
	// 最后dx列加载新的信息
	for (col=kx; col<nCols; col++)
	{
		row_nCols = dy_nCols;
		for (row=dy; row<nRows; row++,row_nCols+=nCols)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
	
	// 最后dy行加载新的信息
	row_nCols = ky*nCols;
	for (row=ky; row<nRows; row++,row_nCols+=nCols)
	{
		for (col=dx; col<kx; col++)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
//------------------------------------------------------------------------
/*
 A → B
 dx=0,dy>0
 (0,0)
┌─────────┐
│A                 │
│(0,dy)            │
├─────────┤
│╳╳╳╳╳╳╳╳╳│
├─────────┤(kx,ky)
│                  │
│                 B│
└─────────┘(nCols,nRows)
 kx=nCols;
 ky=nRows-dy;
*/
//------------------------------------------------------------------------
void CMapContainer::_MovePreReadRect_Down(BLOCK_INFO* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	// 区域[0,dy,kx,ky)的块信息向上移动dy行
	int kx = nCols;
	int ky = nRows - dy;
	int dy_nCols = dy*nCols;
	int row_nCols = dy_nCols;
	for (row=dy; row<ky; row++,row_nCols+=nCols)
	{
		for (col=0; col<nCols; col++)
		{
			CopyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+col].ptLeftTop, nTileRow, nTileCol);
		}
	}

	// 更新原点
	SetOriginTile(ptNewTileOrigin);

	// 最后dy行加载新的信息
	row_nCols = ky*nCols;
	for (row=ky; row<nRows; row++,row_nCols+=nCols)
	{
		for (col=0; col<nCols; col++)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
//------------------------------------------------------------------------
/*
 A → B
 dx<0,dy>0
 (0,0)
┌┄┄┄┄┌─────────┐
┆        │A                 │
┆        │(-dx,dy)          │
┌────┼────┐        │
│        │╳╳╳╳│        │
│        └────┼────┘
│                  │(kx,ky) ┆
│                 B│        ┆
└─────────┘┄┄┄┄┘(nCols,nRows)
 kx=nCols+dx;
 ky=nRows-dy;
*/
//------------------------------------------------------------------------
void CMapContainer::_MovePreReadRect_LeftDown(BLOCK_INFO* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	// 区域[-dx,dy,kx,ky)的块信息向右上移动一(-dx)列(dy行)
	int kx = nCols + dx;
	int ky = nRows - dy;
	int dy_nCols = dy*nCols;
	int row_nCols;
	for (col=kx-1; col>=(-dx); col--)
	{
		row_nCols = dy_nCols;
		for (row=dy; row<ky; row++,row_nCols+=nCols)
		{
			CopyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+(col+(-dx))].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	// 更新原点
	SetOriginTile(ptNewTileOrigin);
	
	// 左边(-dx)列加载新的信息
	for (col=0; col<(-dx); col++)
	{
		row_nCols = dy_nCols;
		for (row=dy; row<nRows; row++,row_nCols+=nCols)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
	
	// 最后dy行加载新的信息
	row_nCols = ky*nCols;
	for (row=ky; row<nRows; row++,row_nCols+=nCols)
	{
		for (col=(-dx); col<kx; col++)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
//------------------------------------------------------------------------
/*
 A → B
 dx<0,dy=0
 (0,0)   (-dx,0)
┌────┬────┬────┐
│        │╳╳╳╳│       A│
│        │╳╳╳╳│        │
│        │╳╳╳╳│        │
│B       │╳╳╳╳│(kx,ky) │
└────┴────┴────┘(nCols,nRows)
 kx=nCols+dx;
 ky=nRows;
*/
//------------------------------------------------------------------------
void CMapContainer::_MovePreReadRect_Left(BLOCK_INFO* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	// 区域[-dx,0,kx,ky)的块信息向后移动(-dx)列
	int kx = nCols + dx;
	int ky = nRows;
	int row_nCols = 0;
	for (col=kx-1; col>=(-dx); col--)
	{
		row_nCols = 0;
		for (row=0; row<nRows; row++,row_nCols+=nCols)
		{
			CopyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+(col+(-dx))].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	// 更新原点
	SetOriginTile(ptNewTileOrigin);
	
	// 左边(-dx)列加载新的信息
	for (col=0; col<(-dx); col++)
	{
		row_nCols = 0;
		for (row=0; row<nRows; row++,row_nCols+=nCols)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
//------------------------------------------------------------------------
/*
 A → B
 dx<0,dy<0
 (0,0)
┌─────────┐┄┄┄┄┐
│B                 │        ┆
│      (-dx,-dy)   │        ┆
│        ┌────┼────┐
│        │╳╳╳╳│        │
└────┼────┘(kx,ky) │
┆        │                  │
┆        │                 A│
└┄┄┄┄┴─────────┘(nCols,nRows)
 kx=nCols+dx;
 ky=nRows+dy;
*/
//------------------------------------------------------------------------
void CMapContainer::_MovePreReadRect_LeftTop(BLOCK_INFO* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	// 区域[-dx,-dy,kx,ky)的块信息向右下移动(-dx)列((-dy)行)
	int kx = nCols + dx;
	int ky = nRows + dy;
	int dy_nCols = dy*nCols;
	int row_nCols;
	for (col=kx-1; col>=(-dx); col--)
	{
		row_nCols = -dy_nCols;
		for (row=(-dy); row<ky; row++,row_nCols+=nCols)
		{
			CopyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+(col+(-dx))].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	// 更新原点
	SetOriginTile(ptNewTileOrigin);
	
	// 左边(-dx)列加载新的信息
	for (col=0; col<(-dx); col++)
	{
		row_nCols = 0;
		for (row=0; row<ky; row++,row_nCols+=nCols)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
	
	// 上面(-dy)行加载新的信息
	row_nCols = 0;
	for (row=0; row<(-dy); row++,row_nCols+=nCols)
	{
		for (col=(-dx); col<kx; col++)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
//------------------------------------------------------------------------
/*
 A → B
 dx?,dy?
 (0,0)
┌┄┄┄┄┄┄┄┄┄┄┄┌─────────┐
┆                      │A                 │
┆                      │                  │  
┆                      │                  │
┆                      │                  │
┌─────────┐  └─────────┘
│                  │                      ┆
│                  │                      ┆
│                  │                      ┆
│                 B│                      ┆
└─────────┘┄┄┄┄┄┄┄┄┄┄┄┘(nCols,nRows)
 kx=nCols+dx;
 ky=nRows-dy;
*/
//------------------------------------------------------------------------
void CMapContainer::_MovePreReadRect_All(BLOCK_INFO* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	// 更新原点
	SetOriginTile(ptNewTileOrigin);
	
	// 每行每列加载新的信息
	for (col=0; col<nCols; col++)
	{
		int row_nCols = 0;
		for (row=0; row<nRows; row++,row_nCols+=nCols)
		{
			LoadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
//------------------------------------------------------------------------