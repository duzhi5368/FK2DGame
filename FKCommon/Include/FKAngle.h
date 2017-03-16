/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKAngle
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
#include "FKAngle.h"
#include "FKDegree.h"
//-------------------------------------------------------------------
namespace FKCommon
{
	class CAngel
	{
	private:
		float			m_fAngle;
	public:
		explicit CAngel( float p_fAngle )
			: m_fAngle( p_fAngle )
		{

		}
		inline operator CRadian() const
		{
			return CRadian( CMath::Angle2Radian( m_fAngle ) );
		}
		inline operator CDegree() const
		{
			return CDegree( CMath::Angle2Degree( m_fAngle ) );
		}
	};
}