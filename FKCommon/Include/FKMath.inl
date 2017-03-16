///********************************************************************
//*
//*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
//*	E-mail: duzhi5368@163.com
//*	QQ: 	281862942
//* 	
//*	-----------------------------------------------------------------
//*
//*	文件名：	FKMath
//*	作者：		FreeKnightDuzhi[ 王宏张 ]
//*	文件说明：	
//*
//*	修改人:		
//*	修改内容:
//*********************************************************************/
//
//#pragma once
//
////-------------------------------------------------------------------
//__forceinline int GetAbs( int p_nValue )
//{
//
//}
////-------------------------------------------------------------------
//__forceinline float GetAbs( float p_fValue )
//{
//
//}
////-------------------------------------------------------------------
//__forceinline CDegree GetAbs( const CDegree& p_Deg );
//__forceinline CRadian GetRad( const CRadian& p_Rad );
//__forceinline int GetIntCeil( float p_fValue );
//__forceinline float GetCeil( float p_fValue );
//__forceinline int GetIntFloor( float p_fValue );
//__forceinline float GetFloor( float p_fValue )
//{
//
//}
////-------------------------------------------------------------------
//__forceinline int CMath::GetSign( int p_nValue )
//{
//	return ( p_nValue > 0 ? 1 : ( p_nValue < 0 ? -1 : 0 ) );
//}
////-------------------------------------------------------------------
//__forceinline float CMath::GetSign( float p_fValue )
//{
//	if( p_fValue > 0.0f )
//	{
//		return 1.0f;
//	}
//	if( p_fValue < 0.0f )
//	{
//		return -1.0f;
//	}
//	return 0.0f;
//}
////-------------------------------------------------------------------
//__forceinline CRadian GetSign( const CRadian& p_Rad );
//__forceinline CDegree GetSign( const CDegree& p_Deg )
//{
//
//}
////-------------------------------------------------------------------
//__forceinline CRadian CMath::GetArcCos( float p_fValue )
//{
//	if( -1.0f < p_fValue )
//	{
//		if( p_fValue < 1.0f )
//		{
//			return CRadian( ASM_ARCCOS( p_fValue ) );
//		}
//		else
//		{
//			return CRadian( 0.0f );
//		}
//	}
//	else
//	{
//		return CRadian( PI );
//	}
//}
////-------------------------------------------------------------------
//__forceinline CRadian CMath::GetArcSin( float p_fValue )
//{
//	if( -1.0f < p_fValue )
//	{
//		if( p_fValue < 1.0f )
//		{
//			return CRadian( ASM_ARCSIN( p_fValue ) );
//		}
//		else
//		{
//			return CRadian( HALFPI );
//		}
//	}
//	else
//	{
//		return CRadian( -HALFPI );
//	}
//}
////-------------------------------------------------------------------
//__forceinline CRadian GetArcTan( float p_fValue );
//__forceinline CRadian GetArcTan2( float p_fY, float p_fX );
//__forceinline float GetCos( const CRadian& p_Rad, bool p_bUseTable = false );
//__forceinline float GetCos( float p_fValue, bool p_bUseTable = false );
//__forceinline float GetSin( const CRadian& p_Rad, bool p_bUseTable = false );
//__forceinline float GetSin( float p_fValue, bool p_bUseTable = false );
//__forceinline float GetTan( const CRadian& p_Rad, bool p_bUseTable = false );
//__forceinline float GetTan( float p_fValue, bool p_bUseTable = false );
//__forceinline float GetExp( float p_fValue );
//__forceinline float GetLog( float p_fValue );
//__forceinline float GetPow( float p_fBase, float p_fExponent );
//__forceinline float GetSqr( float p_fValue );
//__forceinline float GetSqrt( float p_fValue );
//__forceinline CRadian GetSqrt( const CRadian& p_Rad );
//__forceinline CDegree GetSqrt( const CDegree& p_Deg );
////-------------------------------------------------------------------
//__forceinline float CMath::InvSqrt( float p_fValue )
//{
//	return float( ASM_RSQRT( p_fValue ) );
//}
////-------------------------------------------------------------------
//__forceinline float CMath::UnitRandom()
//{
//	return ( float( rand() ) / float( RAND_MAX ));
//}
////-------------------------------------------------------------------
//__forceinline float CMath::RangeRandom( float p_fLow, float p_fHeight )
//{
//	return ( p_fHeight - p_fLow ) * UnitRandom() + p_fLow;
//}
////-------------------------------------------------------------------
//__forceinline float CMath::SymmetricRandom()
//{
//	return 2.0f * UnitRandom() - 1.0f;
//}
////-------------------------------------------------------------------
//__forceinline float Degree2Radian( float p_fDegValue );
//__forceinline float Radian2Degree( float p_fRadValue );