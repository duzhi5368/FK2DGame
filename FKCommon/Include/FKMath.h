/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKMath
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
#include "FKCommonPre.h"
//-------------------------------------------------------------------
class CDegree;
class CRadian;
class CVector2f;
class CVector3f;
class CVector4f;
class CRay;
class CSphere;
class CMatrix4f;
class CPlane;
class CAxisAlignedBox;
//-------------------------------------------------------------------
namespace FKCommon
{
	/** 常用的数学方法库
	*/
	class FK_DLL CMath
	{
	public:
		/**
		* Comment: 	度数类型枚举
		*/
		enum ENUM_AngleStyle
		{
			EAS_Degree,			// 角度
			EAS_Radian,			// 弧度
		};
	protected:
		static ENUM_AngleStyle	ms_eAngleStyle;		// 度数类型float 
		static float*			ms_pSinTable;		// Sin表
		static float*			ms_pTanTable;		// Tan表
		static int				ms_nTableSize;		// 三角表大小
		static float			ms_fTableFactor;	// 三角表参数
	public:
		static const float POSTIVE_INFINITY;		// 正无穷
		static const float NEGATIVE_INFINITY;		// 负无穷
		static const float DEGREE_2_RADIAN_PARAM;	// 角度转弧度的参数
		static const float RADIAN_2_DEGREE_PARAM;	// 弧度转角度的参数
	public:
		static float SinTable( float p_fValue );
		static float TanTable( float p_fValue );
	public:
		/**
		* Comment: 	取正
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline int GetAbs( int p_nValue );
		static __forceinline float GetAbs( float p_fValue );
		static __forceinline CDegree GetAbs( const CDegree& p_Deg );
		static __forceinline CRadian GetRad( const CRadian& p_Rad );
		/**
		* Comment: 	获取大于或等于参照值的整数
		* @param:	p_fValue 参数
		* @return:
		* @remarks: 例如 GetCeil( 12.64f ) 得到返回值是 13 或 13.00f（根据函数返回值而不同）
		* @note:
		*/
		static __forceinline int GetIntCeil( float p_fValue );
		static __forceinline float GetCeil( float p_fValue );
		/**
		* Comment: 	获取小于或等于参照值的整数
		* @param:	p_fValue 参数
		* @return:
		* @remarks: 例如 GetFloor( 12.64f ) 得到的返回值是 12 或 12.00f（根据函数返回值而不同）
		* @note:
		*/
		static __forceinline int GetIntFloor( float p_fValue );
		static __forceinline float GetFloor( float p_fValue );
		/**
		* Comment: 	判断一个整数的符号
		* @param:
		* @return:	若大于0则返回 1， 若小于0则返回 -1， 若等于0则返回0
		* @remarks:
		* @note:
		*/
		static __forceinline int GetSign( int p_nValue );
		static __forceinline float GetSign( float p_fValue );
		static __forceinline CRadian GetSign( const CRadian& p_Rad );
		static __forceinline CDegree GetSign( const CDegree& p_Deg );
		/**
		* Comment: 	求某值的反余弦值
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline CRadian GetArcCos( float p_fValue );
		/**
		* Comment: 	求某值的反正弦值
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline CRadian GetArcSin( float p_fValue );
		/**
		* Comment: 	求某值的反正切值
		* @param:
		* @return:	返回值在 -pi/2 到 pi/2 之间。
		* @remarks:
		* @note:
		*/
		static __forceinline CRadian GetArcTan( float p_fValue );
		/**
		* Comment: 	求给定X,Y坐标的反正切值
		* @param:	p_fY 表示点的Y坐标值，p_fX 表示点的X坐标
		* @return:	返回值为正，则表示从X轴逆时针旋转的角度。
					返回值为负，则表示从X轴顺时针旋转的角度。
					返回值取值范围在 -pi 到 pi 之间（不包括-pi）
		* @remarks:
		* @note:
		*/
		static __forceinline CRadian GetArcTan2( float p_fY, float p_fX );
		/**
		* Comment: 	Cos函数
		* @param:	p_Rad 弧度值
		* @param:	p_bUseTable 是否查表
		* @return:
		* @remarks:	若进行查表，则可以更快的得到Cos值，但是将丢失一定的精度。
		* @note:
		*/
		static __forceinline float GetCos( const CRadian& p_Rad, bool p_bUseTable = false );
		/**
		* Comment: 	Cos函数
		* @param:	p_fValue 弧度值
		* @param:	p_bUseTable 是否查表
		* @return:
		* @remarks:	若进行查表，则可以更快的得到Cos值，但是将丢失一定的精度。
		* @note:
		*/
		static __forceinline float GetCos( float p_fValue, bool p_bUseTable = false );
		/**
		* Comment: 	Sin函数
		* @param:	p_Rad 弧度值
		* @param:	p_bUseTable 是否查表
		* @return:
		* @remarks:	若进行查表，则可以更快的得到Cos值，但是将丢失一定的精度。
		* @note:
		*/
		static __forceinline float GetSin( const CRadian& p_Rad, bool p_bUseTable = false );
		/**
		* Comment: 	Sin函数
		* @param:	p_fValue 弧度值
		* @param:	p_bUseTable 是否查表
		* @return:
		* @remarks:	若进行查表，则可以更快的得到Cos值，但是将丢失一定的精度。
		* @note:
		*/
		static __forceinline float GetSin( float p_fValue, bool p_bUseTable = false );
		/**
		* Comment:	Tan函数
		* @param:	p_Rad 弧度值
		* @param:	p_bUseTable 是否查表
		* @return:
		* @remarks:	若进行查表，则可以更快的得到Cos值，但是将丢失一定的精度。
		* @note:
		*/
		static __forceinline float GetTan( const CRadian& p_Rad, bool p_bUseTable = false );
		/**
		* Comment:	Tan函数
		* @param:	p_fValue 弧度值
		* @param:	p_bUseTable 是否查表
		* @return:
		* @remarks:	若进行查表，则可以更快的得到Cos值，但是将丢失一定的精度。
		* @note:
		*/
		static __forceinline float GetTan( float p_fValue, bool p_bUseTable = false );
		/**
		* Comment: 	求 e 的 x 次方
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float GetExp( float p_fValue );
		/**
		* Comment: 	求x的对数
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float GetLog( float p_fValue );
		/**
		* Comment: 	求 x 的 y 次幂
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float GetPow( float p_fBase, float p_fExponent );
		/**
		* Comment: 	求平方
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float GetSqr( float p_fValue );
		/**
		* Comment: 	求平方根
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float GetSqrt( float p_fValue );
		static __forceinline CRadian GetSqrt( const CRadian& p_Rad );
		static __forceinline CDegree GetSqrt( const CDegree& p_Deg );
		/**
		* Comment: 	求平方根的倒数
		* @param:
		* @return:
		* @remarks: 即 1 / GetSqrt( p_fValue ) ，对向量标准化很重要
		* @note:
		*/
		static __forceinline float InvSqrt( float p_fValue );
		/**
		* Comment: 	获得【0，1】内随机数
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float UnitRandom();
		/**
		* Comment: 	获得【p_fLow, p_fHigh】内随机数
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float RangeRandom( float p_fLow, float p_fHeight );
		/**
		* Comment: 	获得【-1，1】内随机数
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float SymmetricRandom();
		/**
		* Comment: 	将角度转换为弧度
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float Degree2Radian( float p_fDegValue );
		/**
		* Comment: 	将弧度转为角度
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static __forceinline float Radian2Degree( float p_fRadValue );
		/**
		* Comment: 	转换Math度数为弧度
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static float Angle2Radian( float p_fAngle );
		/**
		* Comment: 	转换弧度为Math度数
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static float Radian2Angle( float p_Rad );
		/**
		* Comment: 	转换Math度数为角度
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static float Angle2Degree( float p_fAngle );
		/**
		* Comment: 	转换角度为Math度数
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static float Degree2Angle( float p_Deg );
		/**
		* Comment: 	设置Math度数类型
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static void SetAngleStyle( ENUM_AngleStyle p_eStyle );
		/**
		* Comment: 	获取Math度数类型
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static ENUM_AngleStyle GetAngleStyle();
	public:
		/**
		* Comment: 	判断一个点是否在平面的三角形内
		* @param:	p_VecPoint 需要被检查的点
		* @param:	p_VecA，p_VecB，p_VecC 是平面上的三角形三个顶点
		* @return:	若在三角形内，则返回true，若在三角形外，则返回false
		* @remarks:
		* @note:
		*/
		static bool IsPointInTri2D( const CVector2f& p_VecPoint,
			const CVector2f& p_VecA, const CVector2f& p_VecB, const CVector2f& p_VecC );
		/**
		* Comment: 	判断一个点是否在3维空间三角形内
		* @param:	p_VecPoint 需要被检查的点
		* @param:	p_VecA，p_VecB，p_VecC 是空间上的三角形三个顶点
		* @param:	p_VecNormal 空间上三角形的发现
		* @return:	若在三角形内，则返回true，若在三角形外，则返回false
		* @remarks:
		* @note:
		*/
		static bool IsPointInTri3D( const CVector3f& p_VecPoint, const CVector3f& p_VecA,
			const CVector3f& p_VecB, const CVector3f& p_vecC, const CVector3f& p_VecNormal );
	public:
		/**
		* Comment: 	进行射线和平面交点判断
		* @param:	
		* @return:	返回是否相交，同时返回射线和平面的距离。若不相交，则返回 std::pair< false, 0 >
		* @remarks:
		* @note:
		*/
		static std::pair< bool, float > Intersect( const CRay& p_Ray, const CPlane& p_Pla );
		/**
		* Comment: 	判断一个射线和球体的交点
		* @param:	p_bDiscardInside 若射线起始点在球内，是否放弃处理
		* @return:	返回是否相交，同时返回射线和球体的距离,若不相交，则返回 std::pair< false, 0 >
		* @remarks:
		* @note:
		*/
		static std::pair< bool, float > Intersect( const CRay& p_Ray, const CSphere& p_Sphere, bool p_bDiscardInside = true );
		/**
		* Comment: 	判断一个射线是否和一个矩形立体盒有交点
		* @param:
		* @return:	返回是否相交，同时返回射线和矩形立体盒的距离.若不相交，则返回 std::pair< false, 0 >
		* @remarks:
		* @note:
		*/
		static std::pair< bool, float > Intersect( const CRay& p_Ray, const CAxisAlignedBox& p_Box );
		/**
		* Comment: 	判断一个射线是否和一个矩形立体盒有交点，同时返回两个相交点与射线起点的距离。
		* @param:	p_fNearDis 离射线起点较近的交点 到 射线起点 之间的距离。
		* @param:	p_fFarDis 离射线起点较远的交点 到 射线起点 之间的距离。
		* @return:	若相交，则返回true。同时可以保证 p_fFarDis >= p_fNearDis >= 0
		* @remarks:
		* @note:	若射线与矩形立体盒没有交点，则返回false。同时，p_fNearDis和p_fFarDis 的值都不改变。
		*/
		static bool Intersect( const CRay& p_Ray, const CAxisAlignedBox& p_Box, OUT float* p_fNearDis, OUT float* p_fFarDis );
		/**
		* Comment: 	判断一个射线和一个3D空间中三角形的关系
		* @param:	p_VecNormal 三角形的法线
		* @param:	p_bDisposalPositiveSide 是否处理与三角面的正面交点
		* @param:	p_bDisposalNegativeSide 是否处理与三角面的反面交点
		* @return:	若射线与三角面相交，则返回 std::pair< true, 射线起点与交点之间的距离 >
		* @remarks:
		* @note:	若射线与三角面没有交点，则返回std::pair< false, 0 >
		*/
		static std::pair< bool, float > Intersect( const CRay& p_Ray, const CVector3f& p_VecA, const CVector3f& p_VecB,
			const CVector3f& p_VecC, const CVector3f& p_VecNormal, bool p_bDisposalPositiveSide = true, bool p_bDisposalNegativeSide = true );
		/**
		* Comment: 	判断一个射线和一个3D空间中三角形的关系
		* @param:	p_bDisposalPositiveSide 是否处理与三角面的正面交点
		* @param:	p_bDisposalNegativeSide 是否处理与三角面的反面交点
		* @return:	若射线与三角面相交，则返回 std::pair< true, 射线起点与交点之间的距离 >
		* @remarks:
		* @note:	若射线与三角面没有交点，则返回std::pair< false, 0 >，三角面的法线无需外界给出，函数内部进行计算获得。
		*/
		static std::pair< bool, float > Intersect( const CRay& p_Ray, const CVector3f& p_VecA, const CVector3f& p_VecB,
			const CVector3f& p_VecC, bool p_bDisposalPositiveSide = true, bool p_bDisposalNegativeSide = true );
		/**
		* Comment: 	检查一个球体是否和一个矩形立体盒相交
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static bool Intersect( const CSphere& p_Sph, const CAxisAlignedBox& p_Box );
		/**
		* Comment: 	判断一个平面是否和一个矩形立体盒相交
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static bool Intersect( const CPlane& p_Pla, const CAxisAlignedBox& p_Box );
		/**
		* Comment: 	判断一个球体是否和一个平面相交
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static bool Intersect( const CSphere& p_Sph, const CPlane& p_Pla );
		/**
		* Comment: 	判断一个射线和一组Plane之间的相交关系
		* @param:	p_listPlane 一组组成凸多面体的面
		* @param:	p_bIsNormalOutside 这组面的法线是否朝向凸多面体外。
		* @return:
		* @remarks:
		* @note:
		*/
		static std::pair< bool, float > Intersect( const CRay& p_Ray, 
			const std::vector<CPlane>& p_listPlane, bool p_bIsNormalOutside = true );
		static std::pair< bool, float > Intersect( const CRay& p_Ray, 
			const std::list<CPlane>& p_listPlane, bool p_bIsNormalOutside = true );
		/**
		* Comment: 	判断两个float是否相等
		* @param:	p_fTolerance 容差阀值
		* @return:
		* @remarks:
		* @note:
		*/
		static bool IsEqual( float p_fValueA, float p_fValueB, float p_fTolerance = std::numeric_limits<float>::epsilon() );
		/**
		* Comment: 	根据一个三角形三顶点和纹理坐标计算出其切空间
		* @param:	
		* @return:
		* @remarks:	
		* @note:	对于3D空间内一个顶点来说，其所在切空间三个坐标轴分别为该点 法线N，切线T，副法线B。
					切空间则是 M = 【 T， B, N 】，这个切空间对于 BumpMap 灯光转换的意义很大。
		*/
		static CVector3f CalculateTangentSpaceVector( const CVector3f& p_VecPos1, const CVector3f& p_VecPos2,
			const CVector3f& p_VecPos3, float p_fU1, float p_fV1, float p_fU2, float p_fV2, float p_fU3, float p_fV3 );
		/**
		* Comment:  为一个面创建一个反射矩阵
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static CMatrix4f BuildReflectionMatrix( const CPlane& p_Pla );
		/**
		* Comment: 	计算三点所在面的法线（不包括W信息）
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static CVector3f CalculateBasicFaceNormal( const CVector3f& p_Vec1, const CVector3f& p_Vec2, const CVector3f& p_Vec3 );
		/**
		* Comment: 	计算三点所在面的法线（包括了W信息）
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static CVector4f CalculateFaceNormal( const CVector3f& p_Vec1, const CVector3f& p_Vec2, const CVector3f& p_Vec3 );
		/**
		* Comment: 	计算三点所在面的法线（不包括W信息）但不进行格式化
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static CVector3f CalculateBasicFaceNormalWithoutNormalize( const CVector3f& p_Vec1, const CVector3f& p_Vec2, const CVector3f& p_Vec3 );
		/**
		* Comment: 	计算三点所在面的法线（包括了W信息）但不进行格式化
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static CVector4f CalculateFaceNormalWithoutNormalize( const CVector3f& p_Vec1, const CVector3f& p_Vec2, const CVector3f& p_Vec3 );
	public:
		/**
		* Comment: 	将一个值根据一个偏移值和比例参数进行高斯处理后得到值
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		static float GetGaussianDistribution( float p_fValue, float p_fOffset = 0.0f, float p_fScale = 1.0f );
	protected:
		/**
		* Comment: 	构造生成三角标
		* @param:
		* @return:
		* @remarks:
		* @note:
		*/
		void		_BuildTrigTables();
	private:
		float ASM_ARCCOS( float r );
		float ASM_ARCSIN( float r );
		float ASM_ARCTAN( float r );
		float ASM_SIN( float r );
		float ASM_COS( float r );
		float ASM_TAN( float r );
		float ASM_SQRT( float r );			// 求根
		float ASM_RSQRT( float r );			// 返回 1/a, 其中a为r的根。
		float CARMAK_INV_SQRT( float r );	// 卡马克求根倒数。返回1/a, 其中a为r的根
		float ASM_LN( float r );			// 返回 log2(r) / log2(e)
	public:
		/**
		* Comment: 	构造函数
		* @param:	p_unTableSize 三角表的大小
		* @return:
		* @remarks: 三角表是一个用于实现 Sin, Cos, Tan 求值的一个快速查找表
		* @note:
		*/
		CMath( unsigned int p_unTableSize = 4096 );
		~CMath();
		//-------------------------------------------------------------------
		#include "FKMath.inl"
		//-------------------------------------------------------------------
	};
}

