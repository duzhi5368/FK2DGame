/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKDegree
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
class CRadian;
//-------------------------------------------------------------------
namespace FKCommon
{
	/** 角度
	*/
	class CDegree
	{
	private:
		float				m_fDeg;
	public:
		explicit CDegree( float p_fDeg = 0.0f )
			: m_fDeg( p_fDeg )
		{

		}
		CDegree( const CRadian& p_Red );
		CDegree& operator = ( const float& p_fDeg )
		{
			m_fDeg = p_fDeg;
			return *this;
		}
		CDegree& operator = ( const CDegree& p_Deg )
		{
			m_fDeg = p_Deg.m_fDeg;
			return *this;
		}
		CDegree& operator = ( const CRadian& p_Rad );
		__forceinline float GetDegree() const
		{
			return m_fDeg;
		}
		const CDegree& operator + () const
		{
			return *this;
		}
		CDegree operator + ( const CDegree& p_Deg ) const
		{
			return CDegree( m_fDeg + p_Deg.m_fDeg );
		}
		CDegree operator + ( const CRadian& p_Rad ) const;
		CDegree& operator += ( const CDegree& p_Deg ) 
		{
			m_fDeg += p_Deg.GetDegree();
			return *this;
		}
		CDegree& operator += ( const CRadian& p_Rad ) ;
		CDegree operator - () const 
		{
			return CDegree( -m_fDeg );
		}
		CDegree operator - ( const CDegree& p_Deg ) const
		{
			return CDegree( m_fDeg - p_Deg.GetDegree() );
		}
		CDegree operator - ( const CRadian& p_Rad ) const;
		CDegree& operator -= ( const CDegree& p_Deg )
		{
			m_fDeg -= p_Deg.GetDegree();
			return *this;
		}
		CDegree& operator -= ( const CRadian& p_Rad );
		CDegree operator * ( float p_fValue ) const
		{
			return CDegree( m_fDeg * p_fValue );
		}
		CDegree operator * ( const CDegree& p_Deg ) const
		{
			return CDegree( m_fDeg * p_Deg.GetDegree() );
		}
		CDegree& operator *= ( float p_fValue )
		{
			m_fDeg *= p_fValue;
			return *this;
		}
		CDegree operator / ( float p_fValue ) const
		{
			return CDegree( m_fDeg / p_fValue );
		}
		CDegree& operator /= ( float p_fValue )
		{
			m_fDeg /= p_fValue;
			return *this;
		}
	public:
		float GetRadian() const;
		float GetAngle() const;
	public:
		bool	operator < ( const CDegree& p_Deg ) const	{ return m_fDeg <	p_Deg.m_fDeg; }
		bool	operator <= ( const CDegree& p_Deg ) const	{ return m_fDeg <=	p_Deg.m_fDeg; }
		bool	operator == ( const CDegree& p_Deg ) const	{ return m_fDeg ==	p_Deg.m_fDeg; }
		bool	operator != ( const CDegree& p_Deg ) const	{ return m_fDeg !=	p_Deg.m_fDeg; }
		bool	operator >= ( const CDegree& p_Deg ) const	{ return m_fDeg >=	p_Deg.m_fDeg; }
		bool	operator > ( const CDegree& p_Deg ) const	{ return m_fDeg >	p_Deg.m_fDeg; }
	};

	CDegree operator * ( float p_fA, const CDegree& p_Deg );
	CDegree operator / ( float p_fA, const CDegree& p_Deg );
}

//-------------------------------------------------------------------
#include "FKDegree.inl"
//-------------------------------------------------------------------