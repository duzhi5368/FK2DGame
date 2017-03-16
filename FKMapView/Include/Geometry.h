/**
*	created:		2012-7-1   2:18
*	filename: 		Geometry
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <Windows.h>
//------------------------------------------------------------------------
// 线段型扩散基类
class CLineDiffuse
{
public:
	CLineDiffuse()
	{
		m_nArrayWidth = 0;
		m_nArrayHeight = 0;
	}
	BOOL Create(int nWidth,int nHeight)
	{
		m_nArrayWidth = nWidth;
		m_nArrayHeight = nHeight;
		return TRUE;
	}
	void Line(int x1, int y1, int x2, int y2);

	
	enum {retFalse = 0,retTrue = 1,retStop = 2};
	/*
	 *	返回retFalse表示出错退出，返回retTrue表现成功，返回retStop表示成功但结束查找
	 */
	virtual int OnFoundTile(int x,int y) = NULL;

private:
	int m_nArrayWidth;
	int m_nArrayHeight;
};
//------------------------------------------------------------------------
// 取得一个tile区域内的所有块
// 因为屏幕显示的区域对应于一个菱形，所以需要按行来enum出每个点
// 这里的rcTileArea定义成{tx,ty,col,row},(tx,ty)表示左上角tile坐标，(col,row)分别表示屏幕横向和纵向的格子数
class CEnumTileArea
{
public:
	CEnumTileArea()
	{
		m_nArrayWidth = 0;
	}
	BOOL Create(int nWidth)
	{
		m_nArrayWidth = nWidth;
		return TRUE;
	}
	BOOL IsValidCo(int x,int y)
	{
		return x >= 0 && x < m_nArrayWidth && x >= 0 && x < m_nArrayWidth;
	}
	void EnumTileArea(const RECT& rcTileArea);
	enum {retFalse = 0,retTrue = 1,retStop = 2};
	/*
	 *	返回retFalse表示出错退出，返回retTrue表现成功，返回retStop表示成功但结束查找
	 */
	virtual int OnFoundTile(int x,int y) = NULL;

private:
	int m_nArrayWidth;
};
//------------------------------------------------------------------------
/* 类CDiffuseBase
说明		：
				1、这是一个基类，要用来派生圆形扩散和椭圆形扩散
				2、这个基类定义了简单的从数组中获取每一圈点的算法
				3、请不要直接使用这个类
*/
class CDiffuseBase
{
	class VEC_POINT ;
	public:
	// 初始化函数，获得整个地图的尺寸,注意：地图坐标为0 based,例如X坐标最大为nWidth-1
	void Create(int nWidth,int nHeight) ;
	
	CDiffuseBase() ;
	virtual ~CDiffuseBase() ;

protected:

	virtual BOOL IsNeedCheckEdge() = NULL ;						// 主要判断是否需要判断边界
	virtual BOOL IsDiffuseComplete( int nCircleIndex ) = NULL ; // 判断是否可以扩散结束了，圆的话就看是否到规定的半径了

	// 提供给子类作出响应的虚函数
	enum{ retFalse = 0 , retTrue = 1 , retStop = 2 , retBlock = 3 , retContinue = 4 } ;
	virtual void OnBeginNewCircle() = NULL;						// 通知子类开始新的一圈
	virtual int OnFoundTile( int x , int y ) = NULL;			// 通知子类，找到一个新的TILE

protected:
	BOOL BeginDiffuse( int nX , int nY ) ;						// 开始扩散参数为中心点坐标
	
	// 从数组读出一个点(x,y)后，用这个函数处理（包括遮挡、间接调用OnFoundtile(...)）
	int TryFindOneTile( int x , int y ,
						int nCurPos , int nCurCirclePointNum ,
						int nCircleIndex ,
						BOOL bIsNeedCheckEdge ,
						int nHeadInArr ,
						BOOL bIsAntiClocekWise) ;

	// 初始化标志矩阵m_arrBlockedMark
	void ResetCurMark() ;
	void BlockBetween2Lines(  POINT *arrStartLine , 
								int nStartNumOfLine ,
							  POINT *arrEndLine , 
								int nEndNumOfLine ,
							 int nMinCircleIndex   ,
							BOOL		bIsSmallAngle ,
							BOOL bIsDrawCircleBlocked) ;
	void GetLine( int nX , int nY , POINT *arrLine , int &nNumOfLine  ) ;
protected:
	int m_nWidth;	//整个地图的宽
	int m_nHeight;
	int m_nOX ;		// 扩散原点的X坐标，注意，是在整个大地图坐标中	
	int m_nOY ;

	BOOL IsBlocked( int x , int y ) ; // 这个点是否被挡住了？注意：是矩阵中的坐标

private:
	// 把一个点后面的所有点设为“被遮挡”，这个算法与大数组的格式密切相关
	void MarkAllBlockTile(	int iPosInCircle		,	// 这个点在这一圈中的位置 
							int iNumOfCirclePoint	,	// 这一圈有多少个点
							int iCircleIndex		,	// 这是第几圈？	void ResetCurMark() ;
							int nHeadInArr			  ,// 这一圈第一个元素在数组中的位置
									BOOL	bIsSmallAngle ,
									BOOL bIsDrawCircleBlocked) ;	

	inline void InsertPoint( int nX , int nY , POINT *arrPoint , int &nNumOfLine ) ;
	BOOL IsInLine( POINT *arrLine , int nNumOfLine , int nX , int nY ) ;
	static int m_arrBlockedMark[64][64]; // 如果某个元素值等于m_nCurMark，就表示这个位置被档住了，将不被显示了
	static int m_nCurMark ; // 当前使用的标志掩码
};
//------------------------------------------------------------------------
/*
CCircleDisffuse类

功能	：		该类将64X64范围内从中心点为圆心到最外围每一个圆的坐标
				存放在一个常量数组中，遍历这个数组以获得每一圈上的点的坐标。

使用方法：
				1、派生出自己的自类，并覆盖虚函数OnBeginNewCircl和OnFoundTile
				2、使用子类对象的Create设定整个地图的范围（可以多次设定）
				3、使用BeginDiffuse开始扩散，没扩散到新的一圈会调用子类的OnBeginNewCircle函数
				每发现新的一点（或TILE）会调用子类的OnFoundTile函数

示例代码：

  		CMyEllipsDiffuse myEllips() ; // CMyEllipsDiffuse派生自CCircleDiffuse
		myEllips.Create( 9999 , 9999 , 35 , 17 ,33) ; // 地图尺寸为9999X9999(注意：该操作意味着：在CMyEllipsDiffuse构造函数中调用了Create和BeginCircleDiffuse函数
*/
class CCircleDiffuse : public CDiffuseBase
{
public:
	// 开始圆形扩散
	BOOL BeginCircleDiffuse(	int nX , int nY ,	// 参数nX,nY表示地图中用于扩散的原点坐标（整个地图的大小已经由Create函数传入）
								int nRadius		) ; // 该圆扩散的最大半径

	CCircleDiffuse(	int nWidth	, int nHeight	) ;
	virtual ~CCircleDiffuse() ;

private:
	virtual BOOL IsNeedCheckEdge() ; // 覆盖基类虚函数
	virtual BOOL IsDiffuseComplete( int nCircleIndex ) ;

private:
	int m_nMaxRadius ; // 当前半径
};
//------------------------------------------------------------------------
/*
	类CFanShapedDiffuse
	扇形扩散，用法与上面的CCircleDiffuse类似
*/
class CFanShapedDiffuse : public CDiffuseBase
{
public:
	// 开始扇形扩散，nStarDegree和nEndDegree取值为0~360
	// 开始角度按逆时针旋转到结束角度为该扇形的区域
	BOOL BeginFanShapedDiffuse(	int nX			, int nY		,
								double dStartDegree , double dEndDegree ,
								int nMaxRadius ) ; 
	
	CFanShapedDiffuse(	int nWidth	, int nHeight );
	virtual ~CFanShapedDiffuse(){} ;

protected:
	virtual BOOL IsNeedCheckEdge() ; // 覆盖基类虚函数
	virtual BOOL IsDiffuseComplete( int nCircleIndex ) ;

private:
	void GetEdgePointFromAngle( double dAngle , int &nX , int &nY ) ;
protected:
	int m_nMaxRadius ;
};
//------------------------------------------------------------------------