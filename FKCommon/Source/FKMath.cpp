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

#include "../Include/FKMath.h"

//-------------------------------------------------------------------
namespace FKCommon
{
	//-------------------------------------------------------------------
	const float CMath::POSTIVE_INFINITY = std::numeric_limits< float >::infinity();
	const float CMath::NEGATIVE_INFINITY = -std::numeric_limits< float >::infinity();
	const float CMath::DEGREE_2_RADIAN_PARAM = PI / float( 180.0f );
	const float CMath::RADIAN_2_DEGREE_PARAM = float( 180.0f ) / PI;
	CMath::ENUM_AngleStyle	CMath::ms_eAngleStyle	= CMath::EAS_Degree;
	float*					CMath::ms_pSinTable		= NULL;
	float*					CMath::ms_pTanTable		= NULL;
	int						CMath::ms_nTableSize	= 0;	
	float					CMath::ms_fTableFactor	= 0.0f;

	//-------------------------------------------------------------------
	float CMath::ASM_ARCCOS( float r )
	{
		float asm_one = 1.f;
		float asm_half_pi = HALFPI;
		__asm 
		{
			fld r // r0 = r
			fld r // r1 = r0, r0 = r
			fmul r // r0 = r0 * r
			fsubr asm_one // r0 = r0 - 1.f
			fsqrt // r0 = sqrtf( r0 )
			fchs // r0 = - r0
			fdiv // r0 = r1 / r0
			fld1 // {{ r0 = atan( r0 )
			fpatan // }}
			fadd asm_half_pi // r0 = r0 + pi / 2
		} // returns r0
	}
	//-------------------------------------------------------------------
	float CMath::ASM_ARCSIN( float r )
	{
		const float asm_one = 1.f;
		__asm 
		{
			fld r			// r0 = r
			fld r			// r1 = r0, r0 = r
			fmul r			// r0 = r0 * r
			fsubr asm_one	// r0 = r0 - 1.f
			fsqrt			// r0 = sqrtf( r0 )
			fdiv			// r0 = r1 / r0
			fld1			// {{ r0 = atan( r0 )
			fpatan			// }}
		}					// returns r0
	}
	//-------------------------------------------------------------------
	float CMath::ASM_ARCTAN( float r )
	{
		__asm 
		{
			fld r		// r0 = r
			fld1		// {{ r0 = atan( r0 )
			fpatan		// }}
		}				// returns r0
	}
	//-------------------------------------------------------------------
	float CMath::ASM_SIN( float r )
	{
		__asm 
		{
			fld r		// r0 = r
			fsin		// r0 = sinf( r0 )
		}				// returns r0
	}
	//-------------------------------------------------------------------
	float CMath::ASM_COS( float r )
	{
		__asm 
		{
			fld r		// r0 = r
			fcos		// r0 = cosf( r0 )
		}				// returns r0
	}
	//-------------------------------------------------------------------
	float CMath::ASM_TAN( float r )
	{
		__asm 
		{
			fld r		// r0 = r
			fsin		// r0 = sinf( r0 )
			fld r		// r1 = r0, r0 = r
			fcos		// r0 = cosf( r0 )
			fdiv		// r0 = r1 / r0
		}				// returns r0
	}
	//-------------------------------------------------------------------
	float CMath::ASM_SQRT( float r )
	{
		__asm 
		{
			fld r		// r0 = r
			fsqrt		// r0 = sqrtf( r0 )
		}				// returns r0
	}
	//-------------------------------------------------------------------
	float CMath::ASM_RSQRT( float r )
	{
		__asm 
		{
			fld1		// r0 = 1.f
			fld r		// r1 = r0, r0 = r
			fsqrt		// r0 = sqrtf( r0 )
			fdiv		// r0 = r1 / r0
		}				// returns r0
	}
	//-------------------------------------------------------------------
	float CMath::CARMAK_INV_SQRT( float r )
	{
		float fHalfR = 0.5f * r;
		int i = *(int*)&r;
		i = 0x5f3759df - ( i >> 1 );
		r = *(float*)&i;
		r = r * ( 1.5f - fHalfR * r * r );
		return r;
	}
	//-------------------------------------------------------------------
	float CMath::ASM_LN( float r )
	{
		const float asm_e = 2.71828182846f;
		const float asm_1_div_log2_e = .693147180559f;
		const float asm_neg1_div_3 = -.33333333333333333333333333333f;
		const float asm_neg2_div_3 = -.66666666666666666666666666667f;
		const float asm_2 = 2.f;

		int log_2 = 0;

		__asm 
		{
			// log_2 = ( ( r >> 0x17 ) & 0xFF ) - 0x80;
			mov eax, r
			sar eax, 0x17
			and eax, 0xFF
			sub eax, 0x80
			mov log_2, eax

			// r = ( r & 0x807fffff ) + 0x3f800000;
			mov ebx, r
			and ebx, 0x807FFFFF
			add ebx, 0x3F800000
			mov r, ebx

			// r = ( asm_neg1_div_3 * r + asm_2 ) * r + asm_neg2_div_3;   // (1)
			fld r
			fmul asm_neg1_div_3
			fadd asm_2
			fmul r
			fadd asm_neg2_div_3
			fild log_2
			fadd
			fmul asm_1_div_log2_e
		}
	}
	//-------------------------------------------------------------------
	CMath::CMath( unsigned int p_unTableSize )
	{
		ms_eAngleStyle	= EAS_Degree;		//	默认是角度模式
		ms_nTableSize	= p_unTableSize;
		ms_fTableFactor = ms_nTableSize / TWOPI;

		ms_pSinTable	= new float[ms_nTableSize];
		ms_pTanTable	= new float[ms_nTableSize];

		_BuildTrigTables();
	}
	//-------------------------------------------------------------------
	CMath::~CMath()
	{
		if( ms_pSinTable != NULL )
		{
			delete [] ms_pSinTable;
			ms_pSinTable = NULL;
		}
		if( ms_pTanTable != NULL )
		{
			delete [] ms_pTanTable;
			ms_pTanTable = NULL;
		}
	}
	//-------------------------------------------------------------------
	void CMath::_BuildTrigTables()
	{
		float fTempAngle = 0.0f;
		for( int i = 0; i < ms_nTableSize; ++i )
		{
			fTempAngle = TWOPI * i / ms_nTableSize;
			ms_pSinTable[i] = ASM_SIN( fTempAngle );
			ms_pTanTable[i] = ASM_TAN( fTempAngle );
		}
	}
	//-------------------------------------------------------------------
	float CMath::SinTable( float p_fValue )
	{
		int nReallyIndex = 0;
		if( p_fValue >= 0 )
		{
			nReallyIndex = static_cast< int >( p_fValue * ms_fTableFactor ) % ms_nTableSize ;
		}
		else
		{
			nReallyIndex = ms_nTableSize - ( static_cast< int >( -p_fValue * ms_fTableFactor  ) % ms_nTableSize ) - 1;
		}

		return ms_pSinTable[nReallyIndex];
	}
	//-------------------------------------------------------------------
	float CMath::TanTable( float p_fValue )
	{
		int nReallyIndex = 0;
		nReallyIndex = static_cast< int >( p_fValue *= ms_fTableFactor ) % ms_nTableSize;
		return ms_pTanTable[nReallyIndex];
	}
	//-------------------------------------------------------------------
	float CMath::Angle2Radian( float p_fAngle )
	{
		if( ms_eAngleStyle == EAS_Degree )
		{
			return p_fAngle * DEGREE_2_RADIAN_PARAM;
		}
		else
		{
			return p_fAngle;
		}
	}
	//-------------------------------------------------------------------
	float CMath::Radian2Angle( float p_Rad )
	{
		if( ms_eAngleStyle == EAS_Degree )
		{
			return p_Rad * RADIAN_2_DEGREE_PARAM;
		}
		else
		{
			return p_Rad;
		}
	}
	//-------------------------------------------------------------------
	float CMath::Angle2Degree( float p_fAngle )
	{
		if( ms_eAngleStyle == EAS_Degree )
		{
			return p_fAngle;
		}
		else
		{
			return p_fAngle * RADIAN_2_DEGREE_PARAM;
		}
	}
	//-------------------------------------------------------------------
	float CMath::Degree2Angle( float p_Deg )
	{
		if( ms_eAngleStyle == EAS_Degree )
		{
			return p_Deg;
		}
		else
		{
			return p_Deg * DEGREE_2_RADIAN_PARAM;
		}
	}
	//-------------------------------------------------------------------
	void CMath::SetAngleStyle( CMath::ENUM_AngleStyle p_eStyle )
	{
		ms_eAngleStyle = p_eStyle;
	}
	//-------------------------------------------------------------------
	CMath::ENUM_AngleStyle CMath::GetAngleStyle()
	{
		return ms_eAngleStyle;
	}
	//-------------------------------------------------------------------
	//bool CMath::IsPointInTri2D( const CVector2f& p_VecPoint,
	//	const CVector2f& p_VecA, const CVector2f& p_VecB, const CVector2f& p_VecC )
	//{
	//	// 等待下次继续
	//	//CVector2f v1, v2;
	//	//float fDot[3];
	//	//bool bZeroDot[3];

	//	//v1 = p_VecB - p_VecA;
	//	//v2 = p_VecPoint - p_VecA;


	//}
	////-------------------------------------------------------------------
	//static bool IsPointInTri3D( const CVector3f& p_VecPoint, const CVector3f& p_VecA,
	//	const CVector3f& p_VecB, const CVector3f& p_vecC, const CVector3f& p_VecNormal );
	//static std::pair< bool, float > Intersect( const CRay& p_Ray, const CPlane& p_Pla );
	//static std::pair< bool, float > Intersect( const CRay& p_Ray, const CSphere& p_Sphere, bool p_bDiscardInside = true );
	//static std::pair< bool, float > Intersect( const CRay& p_Ray, const CAxisAlignedBox& p_Box );
	//static bool Intersect( const CRay& p_Ray, const CAxisAlignedBox& p_Box, OUT float* p_fNearDis, OUT float* p_fFarDis );
	//static std::pair< bool, float > Intersect( const CRay& p_Ray, const CVector3f& p_VecA, const CVector3f& p_VecB,
	//	const CVector3f& p_VecC, const CVector3f& p_VecNormal, bool p_bDisposalPositiveSide = true, bool p_bDisposalNegativeSide = true );
	//static std::pair< bool, float > Intersect( const CRay& p_Ray, const CVector3f& p_VecA, const CVector3f& p_VecB,
	//	const CVector3f& p_VecC, bool p_bDisposalPositiveSide = true, bool p_bDisposalNegativeSide = true );
	//static bool Intersect( const CSphere& p_Sph, const CAxisAlignedBox& p_Box );
	//static bool Intersect( const CPlane& p_Pla, const CAxisAlignedBox& p_Box );
	//static bool Intersect( const CSphere& p_Sph, const CPlane& p_Pla );
	//static std::pair< bool, float > Intersect( const CRay& p_Ray, 
	//	const std::vector<CPlane>& p_listPlane, bool p_bIsNormalOutside = true );
	//static std::pair< bool, float > Intersect( const CRay& p_Ray, 
	//	const std::list<CPlane>& p_listPlane, bool p_bIsNormalOutside = true );
	//static bool IsEqual( float p_fValueA, float p_fValueB, float p_fTolerance = std::numeric_limits<float>::epsilon() );
	//static CVector3f CalculateTangentSpaceVector( const CVector3f& p_VecPos1, const CVector3f& p_VecPos2,
	//	const CVector3f& p_VecPos3, float p_fU1, float p_fV1, float p_fU2, float p_fV2, float p_fU3, float p_fV3 );
	//static CMatrix4f BuildReflectionMatrix( const CPlane& p_Pla );
	//static CVector3f CalculateBasicFaceNormal( const CVector3f& p_Vec1, const CVector3f& p_Vec2, const CVector3f& p_Vec3 );
	//static CVector4f CalculateFaceNormal( const CVector3f& p_Vec1, const CVector3f& p_Vec2, const CVector3f& p_Vec3 );
	//static CVector3f CalculateBasicFaceNormalWithoutNormalize( const CVector3f& p_Vec1, const CVector3f& p_Vec2, const CVector3f& p_Vec3 );
	//static CVector4f CalculateFaceNormalWithoutNormalize( const CVector3f& p_Vec1, const CVector3f& p_Vec2, const CVector3f& p_Vec3 );
	//static float GetGaussianDistribution( float p_fValue, float p_fOffset = 0.0f, float p_fScale = 1.0f );
	//-------------------------------------------------------------------
}