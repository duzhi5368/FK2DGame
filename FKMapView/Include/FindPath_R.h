/**
*	created:		2012-7-1   2:40
*	filename: 		FindPath_R
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		双轮廓法寻路
*/
//------------------------------------------------------------------------
//	算法描述
//
//	1. 路径搜索时不论是否会碰撞直接向目标点逼进 ( 找出蓝线 )
//  3. 沿蓝线行进，遇到轮廓线沿轮廓线转弯
//	4. 沿轮廓线前进，如果又碰到蓝线则沿蓝线前进
//	5. 最终会到达目标点	, 搜索结束
//
//   功能：用轮廓算法寻找路径
// 
//   约定：TileContainer是包含地图单元格的容器,须实现
//		   IsBlock( POINTS ) 方法
//
//   说明：当前版本是 8 个方向寻路 斜角有东西绊住时不能走
//		   若要定义不同的规则需要重载 FindPath
//
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "MapViewCommon.h"
//------------------------------------------------------------------------
struct   RIVERTAG;
//------------------------------------------------------------------------
// 可以重新定义MAX_SEARCH_AREA以更改成你需要的最大搜索范围
#ifndef  MAX_SEARCH_AREA
#define  MAX_SEARCH_AREA	128
#endif
//------------------------------------------------------------------------
template < class TileContainer , int MaxPathLen = 1024 >
class CPathFinder_R{
private:
	TileContainer	*  m_pTiles;			// 判断某点是否阻挡的函数指针
	RIVERTAG  *  m_pNearest;				// 最接近的点
	int	         m_nWidth;					// 地图宽度
	int		     m_nHeight;					// 地图高度

	POINTS		 m_ptPath[ MaxPathLen ];	// 路径缓冲区
	int			 m_nPathLen;				// 路径长度

	DWORD		 m_cFindID;					//用来标示这是第几次寻路

	// 临时数据
	RIVERTAG     m_TempTags[ MAX_SEARCH_AREA ][ MAX_SEARCH_AREA ];
	POINTS       m_ptStart;					// 本次寻路的起始点
public:
	int          m_nSearchNodes;			// 最近一次寻路搜索的点数
	DWORD        m_dwElapseTime;			// 最近一次寻路消耗的时间

public:
	CPathFinder_R();
	virtual ~CPathFinder_R();

	// 初始化地图数据如果出错程序抛出错误 类型LPCTSTR
	//
	// pMap		:	地图图块指针
	// nWidth   :   地图宽度    
	// nHeight  :   地图高度
	BOOL Create( TileContainer * pContainer , int nWidth , int nHeight  );

	// 寻路 , 如果出错程序抛出错误 类型LPCTSTR
	// ptFrom	:   路径的起始点
	// ptTo		:   路径的目标点
	// ...		
	BOOL FindPath( POINTS ptFrom , POINTS ptTo , POINTS ** ppBuffer , int & nPathLen );

	// 寻找一个简单路径
	// 有时候搜路不需要很精确只需要返回一个模糊路径就可
	BOOL FindSimplePath( POINTS ptFrom , POINTS ptTo , POINTS ** ppBuffer , int & nPathLen );

	// 调用 FindPath 后取回路径
	// pBuffer	:   取返回路径上个点的缓冲区
	// nMaxStep	:	缓冲区的大小 , 防止越界
	// 返回  	:   -1	------------ 参数错误
	//				0 - nMaxStep 路径点的个数
	//				> nMaxStep	 缓冲区太小
	BOOL GetPath( POINTS ** ppBuffer , int & nPathLen );

	// 清除内存, 清除后下次要用需重新调用 InitializeData
	void Close();

private:
	//   判断一些不能走的特例 , 比如斜方向有东西拌住时不能走
	BOOL CheckInValidMoving( POINTS ptFrom , POINTS ptTo );

	//	 判断一个点是否在地图矩阵内
	bool PointInMap( POINTS pos );

	// 根据指针取得它的位置
	POINTS  AddressToPos( RIVERTAG * pAddr );

	BOOL   FindPathAux( POINTS ptFrom , POINTS ptTo );
	BOOL   GetPathAux( POINTS * pBuffer , int & nMaxStep );

	// 从一个指定点开始绕过障碍
	// pos		:  回绕起始点
	// ptTo		:  搜路终点
	// nRunDir	:  起点的运行方向
	// nEchoDir	:  回绕的方向	1	-----------  逆时针
	//							-1	-----------  顺时针
	//							其他值不允许
	bool Echo2( POINTS & pos , POINTS ptTo , int nRunDir , int nEchoDir );
	// 两个方向同时搜索的版本
	bool Echo( POINTS & pos , POINTS ptTo , int nRunDir , int nEchoDir , int nRunDir2 , int nEchoDir2 );

	// 取得临时信息
	RIVERTAG * GetInfo( POINTS pos );

	// 路径优化
	void PathOptimize();

	// 连通两个点
	BOOL CennectPoint( POINTS pos1 , POINTS pos2 );

	// 测试两个点是否能连通
	BOOL TestConnectPoint( POINTS pos1 , POINTS pos2 );

	// 判断一个点是否是阻挡的
	BOOL IsBlock( POINTS pos );

	// 返回一个向量在东南西北八方向上最靠近的一个
	int  GetNearestDir( int nDeltaX , int nDeltaY );
};
//------------------------------------------------------------------------
#define		BLUE_FLAG	  ( 0x01 << 1 )		// 蓝线标志位
#define     VISIT_FLAG	  ( 0x01 << 2 )		// 访问标志位
#define     OPEN_FLAG     ( 0x01 << 3 )     // 已加入OPEN
//------------------------------------------------------------------------
#define     DIR_HIT( d )      ( (char)(0x01 << (4+d)) )
#define     DIR_CW( d )		  ( (char)(0x01 << (d)) )		//	顺时针记号
#define     DIR_CCW( d )	  ( (char)(0x01 << (4+d)) )		//  逆时针记号
//------------------------------------------------------------------------
struct RIVERTAG				// 用来记录轮廓线和其他一些临时信息
{
	RIVERTAG  * pNext;		// 待处理的点链表
	RIVERTAG  * pParent;	// 父亲指针用来返回路径时回朔
	DWORD       cFindID;	// 第几次寻路
	char		cFlag;		// 信息标志字节	高三位表示碰撞时缓存的测试方向和cDir不一样
	char        cDir;		// 经过时的方向
	char        cEchoDir;	// 经过时绕转的方向
	char        cReserse;	// 保留值
};
//------------------------------------------------------------------------
template < class TileContainer , int MaxPathLen>
CPathFinder_R< TileContainer , MaxPathLen >::CPathFinder_R()
{
	m_pTiles	=	NULL;	
	m_pNearest  =   NULL;
	m_nWidth	=	0;	
	m_nHeight	=	0;
	m_cFindID   =   0;
}
//------------------------------------------------------------------------
template < class TileContainer , int MaxPathLen>
CPathFinder_R< TileContainer , MaxPathLen >::~CPathFinder_R()
{
	Close();
}
//------------------------------------------------------------------------
template < class TileContainer , int MaxPathLen>
void CPathFinder_R< TileContainer , MaxPathLen >::Close()
{
	m_pTiles	=	NULL;	
	m_nWidth	=	0;	
	m_nHeight	=	0;	
	m_pNearest  =   NULL;
}
//------------------------------------------------------------------------
template < class TileContainer , int MaxPathLen>
BOOL CPathFinder_R< TileContainer , MaxPathLen >::Create( TileContainer * pContainer , int nWidth , int nHeight  )
{
	if( pContainer == NULL ) {
		throw( "Parameter error! CPathFinder_R< TileContainer , MaxPathLen >::InitializeData" );
		return FALSE;
	}
	if( nWidth <= 0 || nHeight <= 0 )
	{
		throw( "Parameter error! CPathFinder_R< TileContainer , MaxPathLen >::InitializeData\r\n" );
		return FALSE;
	}

	Close();

	m_nWidth	= nWidth;
	m_nHeight	= nHeight;

	m_pTiles   = pContainer;

	return TRUE;		// return OK!
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >	
BOOL CPathFinder_R< TileContainer , MaxPathLen >::FindPathAux( POINTS ptFrom , POINTS ptTo )
{
	if( !PointInMap( ptFrom ) || !PointInMap( ptTo ) )
	{
		return FALSE;
	}

	//清空上一次寻路的变量
	m_pNearest = NULL;
	m_nSearchNodes =0;

	m_ptStart = ptFrom;

	// 同一个点
	if( ptFrom.x == ptTo.x && ptTo.y == ptFrom.y ) return FALSE;

	POINTS       ptRun	   = ptFrom;

	RIVERTAG  * pOldNode   = NULL;
	RIVERTAG  * pNode	   = NULL;

	// 不能到的点就直接走过去，碰到障碍就停
	if( IsBlock( ptTo ) )
	{
		while( 1 )
		{
			m_nSearchNodes ++;

#ifdef	_DEBUG
			ASSERT( PointInMap( ptRun ) );
#endif
			pNode = GetInfo( ptRun );

			if( pNode == NULL ) return FALSE;

			pNode->pParent = pOldNode;

			pOldNode = pNode;


			if( IsBlock( ptRun ) && m_pNearest != NULL ) return TRUE;

			if( ptTo.y == ptRun.y && ptTo.x == ptRun.x ) return TRUE;

			m_pNearest = pNode;

			if( ptTo.y > ptRun.y ) ptRun.y += 1;
			if( ptTo.y < ptRun.y ) ptRun.y -= 1;
			if( ptTo.x > ptRun.x ) ptRun.x += 1;
			if( ptTo.x < ptRun.x ) ptRun.x -= 1;
		}
	}

	//  第一步走出蓝线
	while( 1 )
	{
		m_nSearchNodes ++;

#ifdef	_DEBUG
		ASSERT( PointInMap( ptRun ) );
#endif
		pNode = GetInfo( ptRun );

		if( pNode == NULL ) return FALSE; 

		pNode->cFlag |= BLUE_FLAG;

		pNode->pParent = pOldNode;

		pOldNode = pNode;

		if( ptTo.y == ptRun.y && ptTo.x == ptRun.x )
			break;

		if( ptTo.y > ptRun.y ) ptRun.y += 1;
		if( ptTo.y < ptRun.y ) ptRun.y -= 1;
		if( ptTo.x > ptRun.x ) ptRun.x += 1;
		if( ptTo.x < ptRun.x ) ptRun.x -= 1;
	}

	ptRun	   = ptFrom;
	POINTS oldpos = ptFrom;
	//  第二步沿蓝线走遇到障碍绕过直到回到蓝线为止
	while( 1 )
	{

#ifdef	_DEBUG
		ASSERT( PointInMap( ptRun ) );
#endif

		pNode = GetInfo( ptRun );

		if( pNode == NULL ) return FALSE;

		pNode->cFlag |= VISIT_FLAG;

		//碰到障碍就把它的前一个点加入到待处理队列
		if( IsBlock( ptRun ) )// || CheckInValidMoving( oldpos , ptRun ) )
		{
			if( ptRun.y > oldpos.y && ptRun.x == oldpos.x)
			{
				ptRun = oldpos;
				if( !Echo( ptRun , ptTo , 1 , -1 , 1 , 1 ) ) return TRUE;
			}
			if( ptRun.y < oldpos.y  && ptRun.x == oldpos.x)
			{
				ptRun = oldpos;
				if( !Echo( ptRun , ptTo , 3 , -1 , 3 , 1) ) return TRUE;
			}
			if( ptRun.x > oldpos.x && ptRun.y == oldpos.y)
			{
				ptRun = oldpos;
				if( !Echo( ptRun , ptTo , 0 , -1 , 0 , 1 ) ) return TRUE;
			}
			if( ptRun.x < oldpos.x && ptRun.y == oldpos.y)
			{
				ptRun = oldpos;
				if( !Echo( ptRun , ptTo , 2 , -1 , 2 , 1 ) ) return TRUE;
			}
			if( ptRun.x < oldpos.x && ptRun.y < oldpos.y)
			{
				ptRun = oldpos;
				if( !Echo( ptRun , ptTo , 3 , -1 , 2 , 1 ) ) return TRUE;
			}
			if( ptRun.x > oldpos.x && ptRun.y < oldpos.y)
			{
				ptRun = oldpos;
				if( !Echo( ptRun , ptTo , 0 , -1 , 3 , 1 ) ) return TRUE;
			}
			if( ptRun.x > oldpos.x && ptRun.y > oldpos.y)
			{
				ptRun = oldpos;
				if( !Echo( ptRun , ptTo , 1 , -1 , 0 , 1 ) ) return TRUE;
			}
			if( ptRun.x < oldpos.x && ptRun.y > oldpos.y)
			{
				ptRun = oldpos;
				if( !Echo( ptRun , ptTo , 2 , -1 , 1 , 1 ) ) return TRUE;
			}
		}

		pOldNode = pNode;
		oldpos = ptRun;

		m_pNearest = GetInfo( ptRun );
		if( m_pNearest == NULL ) return FALSE;
		if( ptTo.y == ptRun.y && ptTo.x == ptRun.x ) return TRUE;

		if( ptTo.y > ptRun.y ) ptRun.y += 1;
		if( ptTo.y < ptRun.y ) ptRun.y -= 1;
		if( ptTo.x > ptRun.x ) ptRun.x += 1;
		if( ptTo.x < ptRun.x ) ptRun.x -= 1;
	}

	return FALSE;
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
BOOL  CPathFinder_R< TileContainer , MaxPathLen >::GetPathAux( POINTS * pBuffer , int & nMaxStep )
{
	if( pBuffer == NULL ) return FALSE;

	int nCou = 0;

	RIVERTAG * pNode = m_pNearest;
	while( pNode != NULL )
	{
		nCou ++;
		pNode = pNode->pParent;
		if( nCou > MaxPathLen ) 
		{
			nCou = MaxPathLen - 1;
			TraceLn(" the path is to long ! CPathFinder_R< TileContainer , MaxPathLen >::GetPathAux" );
			nMaxStep = 0;
			return FALSE;
		}
	}

	// OVER STACK
	if( nCou <= 0 || nCou > nMaxStep ) 
	{
		nMaxStep = nCou;
		return FALSE;
	}

	nMaxStep = nCou;

	int retValue = nCou;
	pNode = m_pNearest;
	while( pNode != NULL )
	{
		memcpy( &pBuffer[ --nCou ] , &(AddressToPos( pNode )) , sizeof( POINTS ) );

#ifdef _DEBUG
		ASSERT( nCou >= 0 );
		if( nCou == 0 ) break;
#endif
		pNode = pNode->pParent;
	}

	return TRUE;
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
inline BOOL CPathFinder_R< TileContainer , MaxPathLen >::CheckInValidMoving( POINTS ptFrom , POINTS ptTo )
{

#ifdef _DEBUG
	ASSERT( PointInMap( ptFrom ) );
	ASSERT( PointInMap( ptTo )   );
#endif

	return abs( ptFrom.x - ptTo.x ) == 1 && abs( ptFrom.y - ptTo.y ) == 1 
		&& ( m_pTileContainers[ ptFrom.y*m_nWidth + ptTo.x ].IsBlock()
		|| m_pTileContainers[ ptTo.y*m_nWidth + ptFrom.x ].IsBlock() );
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
inline bool CPathFinder_R< TileContainer , MaxPathLen >::PointInMap( POINTS pos )
{
	return pos.x >= 0 && pos.x < m_nWidth && pos.y >= 0 && pos.y < m_nHeight;
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
BOOL CPathFinder_R< TileContainer , MaxPathLen >::
FindPath( POINTS ptFrom , POINTS ptTo , POINTS ** ppBuffer , int & nPathLen )
{
	if( m_cFindID >= 0xffffffff ) 
	{
		// 清理旧纪录
		ZeroMemory( m_TempTags , sizeof( m_TempTags ) );
		m_cFindID = 0;
	}
	m_cFindID ++;

	if( !FindPathAux( ptFrom , ptTo ) ) return FALSE;

	m_nPathLen = MaxPathLen;
	if( !GetPathAux( m_ptPath , m_nPathLen ) ) return FALSE;
	if( m_nPathLen <= 1 ) return FALSE;

	// 如果搜索到的路径太差就进行优化
	int nLen = abs( ptFrom.x - ptTo.x ) + abs( ptFrom.y - ptTo.y );
	if( m_nPathLen > nLen  &&  m_nPathLen > 10 ) PathOptimize();

	if (m_nPathLen <= 0
		|| m_nPathLen > MaxPathLen
		)
		return FALSE;

	if( !GetPath( ppBuffer , nPathLen ) ) return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
BOOL CPathFinder_R< TileContainer , MaxPathLen >::
FindSimplePath( POINTS ptFrom , POINTS ptTo , POINTS ** ppBuffer , int & nPathLen )
{
	static int s_nDirMove[][2] = { 
		{ 0  , -1 }	,	// 北
		{ 1  , -1 } ,	// 东北
		{ 1	 , 0  } ,	// 东
		{ 1	 , 1  }	,	// 东南
		{ 0	 , 1  }	,	// 南
		{ -1 , 1  }	,	// 西南
		{ -1 , 0  } ,	// 西
		{ -1 , -1 }	,	// 西北
	};	// 八个方向上的移动


	if( m_cFindID >= 0xffffffff ) 
	{
		// 清理旧纪录
		ZeroMemory( m_TempTags , sizeof( m_TempTags ) );
		m_cFindID = 0;
	}
	m_cFindID ++;

	if( !PointInMap( ptFrom ) || !PointInMap( ptTo ) )
	{
		return FALSE;
	}

	//清空上一次寻路的变量
	m_pNearest = NULL;
	m_nSearchNodes =0;

	m_ptStart = ptFrom;

	POINTS      ptRun	   = ptFrom;
	POINTS		oldpos	   = ptFrom;
	RIVERTAG  * pOldNode   = NULL;
	RIVERTAG  * pNode	   = NULL;

	// 同一个点
	if( ptFrom.x == ptTo.x && ptTo.y == ptFrom.y ) return FALSE;

	int nMoveDir = GetNearestDir( ptTo.x - ptFrom.x , ptTo.y - ptFrom.y );;
	while( 1 )
	{

#ifdef	_DEBUG
		ASSERT( PointInMap( ptRun ) );
#endif

		pNode = GetInfo( ptRun );

		if( pNode == NULL ) return FALSE;

		pNode->pParent = pOldNode;

		pOldNode = pNode;
		oldpos = ptRun;

		m_pNearest = GetInfo( ptRun );
		if( m_pNearest == NULL ) return FALSE;
		if( ptTo.y == ptRun.y && ptTo.x == ptRun.x ) break;

		int dx = ptTo.x - ptRun.x;
		int dy = ptTo.y - ptRun.y;

		if( dx == 0 || dy == 0 || abs(dx) == abs(dy) )
		{
			if( nMoveDir != GetNearestDir( dx , dy ) ) break;
		}

		ptRun.y +=	s_nDirMove[ nMoveDir ][ 1 ];
		ptRun.x +=	s_nDirMove[ nMoveDir ][ 0 ];

		if( IsBlock( ptRun ) )
		{
			break;
		}
	}

	m_nPathLen = MaxPathLen;
	if( !GetPathAux( m_ptPath , m_nPathLen ) ) return FALSE;
	if( m_nPathLen <= 1 ) return FALSE;

	if( !GetPath( ppBuffer , nPathLen ) ) return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
BOOL CPathFinder_R< TileContainer , MaxPathLen >::
GetPath( POINTS ** ppBuffer , int & nPathLen )
{
	if( ppBuffer == NULL ) return FALSE;

	* ppBuffer = m_ptPath;
	nPathLen = m_nPathLen;

	return TRUE;
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
bool CPathFinder_R< TileContainer , MaxPathLen >::
Echo2( POINTS & pos , POINTS ptTo , int nRunDir , int nEchoDir )
{
	static const POINTS	ptDir[ 4 ] = { { 1 , 0 } , { 0 , 1 } , { -1 , 0} , {0 , -1} };

	RIVERTAG * pNode;
	POINTS	   ptNode = pos;
	POINTS	   temppos;
	POINTS     ptOldPos = pos;
	int        nTestDir;
	int        nCount = 0;
	bool       bHit = true;

	while( 1 )
	{
		nCount ++;
		m_nSearchNodes ++;

		pNode = GetInfo( ptNode );
		if( pNode == NULL ) return FALSE;

		if( ptNode.x != ptOldPos.x || ptNode.y != ptOldPos.y )
		{
			//找到目标点或者找到蓝点
			if( ( pNode->cFlag & VISIT_FLAG )== 0 )
			{

				if( pNode->pParent == NULL || pNode->cFlag & BLUE_FLAG )
					pNode->pParent = GetInfo( ptOldPos );

				if( ptTo.x == ptNode.x && ptTo.y == ptNode.y || pNode->cFlag & BLUE_FLAG )
				{
					pos = ptNode;
					pNode->cDir  |= DIR_HIT( nRunDir );
					pNode->cFlag |= VISIT_FLAG;
					return true;
				}
			}else{
				//如果这一点已经路过了并且方向也相同就退出
				if( pNode->cDir & DIR_HIT( nRunDir ) ) return false;
			}
			pNode->cDir  |= DIR_HIT( nRunDir );
			pNode->cFlag |= VISIT_FLAG;
		}
		ptOldPos = ptNode;

		//测试垂直方向能否移动
		nTestDir = nRunDir + nEchoDir;
		if( nTestDir < 0 ) nTestDir += 4;
		if( nTestDir >= 4 ) nTestDir -= 4;

		temppos.x = ptNode.x + ptDir[ nTestDir ].x;
		temppos.y = ptNode.y + ptDir[ nTestDir ].y;

		//正在试图沿边界走,退出
		if( !PointInMap( temppos ) ) return false;

		//垂直方向能走
		if( !IsBlock( temppos ) )
		{
			//转向
			nRunDir = nTestDir;
			ptNode = temppos;
			//绕的方向也转向
			if( bHit ) nEchoDir *= -1;
			bHit = false;
			continue;
		}

		bHit = false;
		//第一次不可能转不成功转个向
		if( nCount == 1 ) 
		{
			nRunDir += 2;
			nRunDir %= 4;
			bHit = false;
			nEchoDir *= -1;
			continue;
		}

		//如果不能走就前进一个
		temppos.x = ptNode.x + ptDir[ nRunDir ].x;
		temppos.y = ptNode.y + ptDir[ nRunDir ].y;

		//正在试图走出边界,退出
		if( !PointInMap( temppos ) ) return false;

		//如果被阻挡就转向否则就前进一格
		if( IsBlock( temppos ) )
		{
			nEchoDir *= -1;
			bHit = true;

			// 如果被卡死了退一步再调
			if( GetInfo( temppos ) == NULL ) return FALSE;
			if( GetInfo( temppos )->cFlag & DIR_HIT( nRunDir ) )
			{
				nRunDir += 2;
				nRunDir %= 4;
			}

			GetInfo( temppos )->cFlag |= DIR_HIT( nRunDir );

		}else{
			ptNode = temppos;
		}
	}
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
bool  CPathFinder_R< TileContainer , MaxPathLen >::Echo( POINTS & pos , POINTS ptTo , int nRunDir , int nEchoDir , int nRunDir2 , int nEchoDir2 )
{
	static const POINTS	ptDir[ 4 ] = { { 1 , 0 } , { 0 , 1 } , { -1 , 0} , {0 , -1} };

	RIVERTAG * pNode = NULL;
	POINTS	   ptNode = pos;
	POINTS	   temppos;
	POINTS     ptOldPos = pos;
	int        nTestDir;
	int        nCount = 0;
	bool       bHit = true;

	RIVERTAG * pNode2 = NULL;
	POINTS	   ptNode2 = pos;
	POINTS	   temppos2;
	POINTS     ptOldPos2 = pos;
	int        nTestDir2;
	int        nCount2 = 0;
	bool       bHit2 = true;

	//两边同时行进
	bool   bRunning1 = true;
	bool   bRunning2 = true;

	void * lastpos = NULL;
	int lastcount = 0;
	void * lastpos2 = NULL;
	int lastcount2 = 0;

	while( bRunning1 || bRunning2 )
	{

		if( bRunning1 )
		{
			nCount ++;
			m_nSearchNodes ++;

			pNode = GetInfo( ptNode );
			if( pNode == NULL ) return FALSE;

			// 监测是否找到目标的路
			if( ptNode.x != ptOldPos.x || ptNode.y != ptOldPos.y )
			{
				// 如果这点没有路过
				if( ( pNode->cFlag & VISIT_FLAG ) == 0 )
				{

					if( pNode->pParent == NULL || pNode->cFlag & BLUE_FLAG )
						pNode->pParent = GetInfo( ptOldPos );

					//找到目标点或者找到蓝点	
					if( ptTo.x == ptNode.x && ptTo.y == ptNode.y || pNode->cFlag & BLUE_FLAG )
					{
						pos = ptNode;
						pNode->cDir  |= DIR_HIT( nRunDir );
						pNode->cFlag |= VISIT_FLAG;
						return true;
					}
				}
			}

			// 如果以相同的方式经过同一个点,就退出
			if( ( pNode->cFlag & VISIT_FLAG ) && ( pNode->cDir & DIR_HIT( nRunDir ) ) )
			{
				if( nEchoDir == 1 && ( pNode->cEchoDir & DIR_CW( nRunDir ) )  ) 
				{
					bRunning1 = false;
					continue;
				}
				if( nEchoDir == -1 && ( pNode->cEchoDir & DIR_CCW( nRunDir ) )  ) 
				{
					bRunning1 = false;
					continue;
				}
			}
			pNode->cDir  |= DIR_HIT( nRunDir );
			pNode->cFlag |= VISIT_FLAG;
			if( nEchoDir == 1 ) pNode->cEchoDir |= DIR_CW( nRunDir );
			if( nEchoDir == -1 ) pNode->cEchoDir |= DIR_CCW( nRunDir );

			ptOldPos = ptNode;

			//测试垂直方向能否移动
			nTestDir = nRunDir + nEchoDir;
			if( nTestDir < 0 ) nTestDir += 4;
			if( nTestDir >= 4 ) nTestDir -= 4;

			temppos.x = ptNode.x + ptDir[ nTestDir ].x;
			temppos.y = ptNode.y + ptDir[ nTestDir ].y;

			//正在试图沿边界走,退出
			if( !PointInMap( temppos ) ) 
			{
				bRunning1 = false;
				continue;
			}
			//垂直方向能走
			if( !IsBlock( temppos ) )
			{
				//转向
				nRunDir = nTestDir;
				ptNode = temppos;
				//绕的方向也转向
				if( bHit ) nEchoDir *= -1;
				bHit = false;
				continue;
			}

			bHit = false;
			//第一次不可能转不成功转个向
			if( nCount == 1 ) 
			{
				nRunDir += 2;
				nRunDir %= 4;
				bHit = false;
				nEchoDir *= -1;
				continue;
			}

			//如果不能走就前进一个
			temppos.x = ptNode.x + ptDir[ nRunDir ].x;
			temppos.y = ptNode.y + ptDir[ nRunDir ].y;

			//正在试图走出边界,退出
			if( !PointInMap( temppos ) ) 
			{
				bRunning1 = false;
				continue;
			}

			//如果被阻挡就转向否则就前进一格
			if( IsBlock( temppos ) )
			{
				nEchoDir *= -1;
				bHit = true;

				// 如果被卡死了退一步再调
				if( GetInfo( temppos ) == NULL ) 
				{
					bRunning1 = false;
					continue;
				}
				if( GetInfo( temppos )->cFlag & DIR_HIT( nRunDir ) )
				{
					nRunDir += 2;
					nRunDir %= 4;
				}

				GetInfo( temppos )->cFlag |= DIR_HIT( nRunDir );

			}else{
				ptNode = temppos;
			}
		}

		//反方向
		if( bRunning2 )
		{
			nCount2 ++;
			m_nSearchNodes ++;

			pNode2 = GetInfo( ptNode2 );
			if( pNode2 == NULL ) return FALSE;

			if( ptNode2.x != ptOldPos2.x || ptNode2.y != ptOldPos2.y )
			{
				// 如果这点没有路过
				if( ( pNode2->cFlag & VISIT_FLAG )== 0 )
				{

					if( pNode2->pParent == NULL || pNode2->cFlag & BLUE_FLAG )
						pNode2->pParent = GetInfo( ptOldPos2 );

					//找到目标点或者找到蓝点
					if( ptTo.x == ptNode2.x && ptTo.y == ptNode2.y || pNode2->cFlag & BLUE_FLAG )
					{
						pos = ptNode2;
						pNode2->cDir  |= DIR_HIT( nRunDir2 );
						pNode2->cFlag |= VISIT_FLAG;
						return true;
					}
				}else{
					//如果这一点已经路过了并且方向也相同就退出
					if( pNode2->cDir & DIR_HIT( nRunDir2 ) ) 
					{
						bRunning2 = false;
						continue;
					}

				}
				pNode2->cDir  |= DIR_HIT( nRunDir2 );
				pNode2->cFlag |= VISIT_FLAG;
			}

			// 如果以相同的方式经过同一个点,就退出
			if( ( pNode2->cFlag & VISIT_FLAG ) && ( pNode2->cDir & DIR_HIT( nRunDir2 ) ) )
			{
				if( nEchoDir2 == 1 && ( pNode2->cEchoDir & DIR_CW( nRunDir2 ) )  ) 
				{
					bRunning2 = false;
					continue;
				}
				if( nEchoDir2 == -1 && ( pNode2->cEchoDir & DIR_CCW( nRunDir2 ) )  ) 
				{
					bRunning2 = false;
					continue;
				}
			}
			pNode2->cDir  |= DIR_HIT( nRunDir2 );
			pNode2->cFlag |= VISIT_FLAG;
			if( nEchoDir2 == 1 ) pNode2->cEchoDir |= DIR_CW( nRunDir2 );
			if( nEchoDir2 == -1 ) pNode2->cEchoDir |= DIR_CCW( nRunDir2 );

			ptOldPos2 = ptNode2;

			//测试垂直方向能否移动
			nTestDir2 = nRunDir2 + nEchoDir2;
			if( nTestDir2 < 0 ) nTestDir2 += 4;
			if( nTestDir2 >= 4 ) nTestDir2 -= 4;

			temppos2.x = ptNode2.x + ptDir[ nTestDir2 ].x;
			temppos2.y = ptNode2.y + ptDir[ nTestDir2 ].y;

			//正在试图沿边界走,退出
			if( !PointInMap( temppos2 ) )
			{
				bRunning2 = false;
				continue;
			}

			//垂直方向能走
			if( !IsBlock( temppos2 ) )
			{
				//转向
				nRunDir2 = nTestDir2;
				ptNode2 = temppos2;
				//绕的方向也转向
				if( bHit2 ) nEchoDir2 *= -1;
				bHit2 = false;
				continue;
			}

			bHit2 = false;
			//第一次不可能转不成功转个向
			if( nCount2 == 1 ) 
			{
				nRunDir2 += 2;
				nRunDir2 %= 4;
				bHit2 = false;
				nEchoDir2 *= -1;
				continue;
			}

			//如果不能走就前进一个
			temppos2.x = ptNode2.x + ptDir[ nRunDir2 ].x;
			temppos2.y = ptNode2.y + ptDir[ nRunDir2 ].y;

			//正在试图走出边界,退出
			if( !PointInMap( temppos2 ) )
			{
				bRunning2 = false;
				continue;
			}

			//如果被阻挡就转向否则就前进一格
			if( IsBlock( temppos2 ) )
			{
				nEchoDir2 *= -1;
				bHit2 = true;

				// 如果被卡死了退一步再调
				if( GetInfo( temppos2 ) == NULL ) 
				{
					bRunning2 = false;
					continue;
				}
				if( GetInfo( temppos2 )->cFlag & DIR_HIT( nRunDir2 ) )
				{
					nRunDir2 += 2;
					nRunDir2 %= 4;
				}

				GetInfo( temppos2 )->cFlag |= DIR_HIT( nRunDir2 );

			}else{
				ptNode2 = temppos2;
			}
		}

	}

	return false;
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
inline RIVERTAG * CPathFinder_R< TileContainer , MaxPathLen >::GetInfo( POINTS pos )
{
	// 寻路起始点在64,64的位置
	int x = pos.x - m_ptStart.x + ( MAX_SEARCH_AREA >> 1 );
	int y = pos.y - m_ptStart.y + ( MAX_SEARCH_AREA >> 1 );
	// 出界
	if( x < 0 || x >= 128 || y < 0 || y >= 128 ) 
	{
		return FALSE;
	}

	RIVERTAG * pReturn = &(m_TempTags[y][x]);
	if( pReturn->cFindID != m_cFindID )
	{
		memset( pReturn , 0 , sizeof( RIVERTAG ) );
		pReturn->cFindID = m_cFindID;
	}

	return pReturn;
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
POINTS  CPathFinder_R< TileContainer , MaxPathLen >::AddressToPos( RIVERTAG * pAddr )
{
	int index = pAddr - &(m_TempTags[0][0]);

#ifdef _DEBUG
	ASSERT( index >= 0 );
#endif


	POINTS pos;
	pos.x = m_ptStart.x - ( MAX_SEARCH_AREA >> 1 ) + index % MAX_SEARCH_AREA;
	pos.y = m_ptStart.y - ( MAX_SEARCH_AREA >> 1 ) + index / MAX_SEARCH_AREA;

	return pos;
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
void  CPathFinder_R< TileContainer , MaxPathLen >::PathOptimize()
{
	// 找到所有拐点
	int			 nCornerIndex[ MaxPathLen ];
	int			 nCornerNum;
	nCornerNum = 0;

	RIVERTAG * pInfo = GetInfo( m_ptPath[ 0 ] );
	if( pInfo == NULL ) return;
	char   nLastRunDir = pInfo->cDir;
	nCornerIndex[ nCornerNum ++ ] = 0;
	for( int i = 1 ; i < m_nPathLen ; i ++ )
	{
		RIVERTAG * pInfo = GetInfo( m_ptPath[ i ] );
		if( pInfo == NULL ) continue;
		if( pInfo->cFlag & BLUE_FLAG ) continue;

		char nRunDir = pInfo->cDir;
		if( nRunDir != nLastRunDir ) 
		{
			nCornerIndex[ nCornerNum ++ ] = i;
			nLastRunDir = nRunDir;
		}
	}
	nCornerIndex[ nCornerNum ++ ] = m_nPathLen - 1;

	// 连接找到的拐点
	bool bConnected = false;
	int nNowIndex = -1;
	POINTS ptNow , ptTo;

	while( nNowIndex++ < nCornerNum - 1 )		// 从后往前尝试连接
	{
		if( nCornerIndex[ nNowIndex ] < 0 ) continue;
		ptNow =  m_ptPath[ nCornerIndex[ nNowIndex ] ];

		for( int i = nCornerNum - 1 ; i > nNowIndex; i -- )
		{
			ptTo =  m_ptPath[ nCornerIndex[ i ] ];

			// 如果可以连接就剃除掉中间点
			ASSERT( !(ptNow.x == ptTo.x && ptNow.y == ptTo.y) );

			if( TestConnectPoint( ptNow , ptTo ) )
			{
				if( !CennectPoint( ptNow , ptTo ) )
				{
				}
				else
					bConnected = true;
				// 清除中间点
				for ( int j = nNowIndex+1 ; j < i ; j ++ )
					nCornerIndex[ j ] = -1;

				// 继续下一个点
				break;
			}
		}
	}

	m_nPathLen = MaxPathLen;
	if( bConnected == true ) GetPathAux( m_ptPath , m_nPathLen );
}
//------------------------------------------------------------------------
template< class TileContainer , int MaxPathLen >
BOOL CPathFinder_R< TileContainer , MaxPathLen >::CennectPoint( POINTS pos1 , POINTS pos2 )
{
#ifdef	_DEBUG
	ASSERT( pos1.x != pos2.x || pos1.y != pos2.y );
#endif

	RIVERTAG * pNode = NULL;
	RIVERTAG * pOldNode = NULL;
	RIVERTAG * pEndNode = GetInfo( pos2 );

	if( pEndNode == NULL ) return FALSE;

	while( 1 )
	{
#ifdef	_DEBUG
		ASSERT( PointInMap( pos1 ) );
#endif
		pNode = GetInfo( pos1 );

		if( pNode == NULL ) return FALSE;

		if( pOldNode ) 
		{
			pNode->pParent = pOldNode;
		}

		pOldNode = pNode;

		if( pEndNode == pNode )
			return TRUE;

		if( pos2.y > pos1.y ) pos1.y += 1;
		if( pos2.y < pos1.y ) pos1.y -= 1;
		if( pos2.x > pos1.x ) pos1.x += 1;
		if( pos2.x < pos1.x ) pos1.x -= 1;

		if( IsBlock( pos1 ) ) return FALSE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
// 测试两个点是否能连通
template< class TileContainer , int MaxPathLen >
BOOL CPathFinder_R< TileContainer , MaxPathLen >::TestConnectPoint( POINTS pos1 , POINTS pos2 )
{
#ifdef	_DEBUG
	ASSERT( pos1.x != pos2.x || pos1.y != pos2.y );
#endif

	while( 1 )
	{
#ifdef	_DEBUG
		ASSERT( PointInMap( pos1 ) );
#endif

		if( pos2.y == pos1.y && pos2.x == pos1.x )
			return TRUE;

		if( pos2.y > pos1.y ) pos1.y += 1;
		if( pos2.y < pos1.y ) pos1.y -= 1;
		if( pos2.x > pos1.x ) pos1.x += 1;
		if( pos2.x < pos1.x ) pos1.x -= 1;

		if( IsBlock( pos1 ) ) return FALSE;
	}

	return FALSE;
}
//------------------------------------------------------------------------
// 判断一个点是否是阻挡的
template< class TileContainer , int MaxPathLen >
inline BOOL CPathFinder_R< TileContainer , MaxPathLen >::IsBlock( POINTS pos )
{
	ASSERT( m_pTiles );
	return m_pTiles->IsBlock( pos );
}
//------------------------------------------------------------------------
// 返回一个向量在东南西北八方向上最靠近的一个
template< class TileContainer , int MaxPathLen >
int  CPathFinder_R< TileContainer , MaxPathLen >::GetNearestDir( int nDeltaX , int nDeltaY )
{
	float fSlope; // 斜率
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? 4 : 0;
	else
		fSlope = (float)nDeltaY/nDeltaX;

	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 2.414f)	return  4; // 90	(67.5～90.0) < tan(67.5) = 2.414 >
			if (fSlope >= 0.414f)	return  3; // 45	(22.5～67.5) < tan(22.5) = 0.414 >
			if (fSlope >=   0.0f)	return  2; // 0		(00.0～22.5)
		}
		else // 3
		{
			if (fSlope >= 2.414f)	return 0;
			if (fSlope >= 0.414f)	return 7;
			if (fSlope >=   0.0f)	return 6;
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 2.414f)	return  0;
			if (fSlope >= 0.414f)	return  1;
			if (fSlope >=   0.0f)	return  2;
		}
		else // 2
		{
			if (fSlope >= 2.414f)	return 4;
			if (fSlope >= 0.414f)	return 5;
			if (fSlope >=   0.0f)	return 6;		
		}
	}
	return 0;
}
//------------------------------------------------------------------------