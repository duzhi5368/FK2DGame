/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKRadian
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
#include "FKDegree.h"
//-------------------------------------------------------------------
namespace FKCommon
{
	//-------------------------------------------------------------------
	CRadian::CRadian( const CDegree& p_Degree )
	{
		m_fRad = p_Degree.GetRadian();
	}
	//-------------------------------------------------------------------
	CRadian&	CRadian::operator = ( const CDegree& p_Degree )
	{
		m_fRad = p_Degree.GetRadian();
		return *this;
	}
	//-------------------------------------------------------------------
	CRadian		CRadian::operator + ( const CDegree& p_Degree ) const
	{
		return CRadian( m_fRad + p_Degree.GetRadian() );
	}
	//-------------------------------------------------------------------
	CRadian&	CRadian::operator -= ( const CDegree& p_Degree )
	{
		m_fRad -= p_Degree.GetRadian();
		return *this;
	}
	//-------------------------------------------------------------------
	CRadian&	CRadian::operator += ( const CDegree& p_Degree )
	{
		m_fRad += p_Degree.GetRadian();
		return *this;
	}
	//-------------------------------------------------------------------
	CRadian		CRadian::operator - ( const CDegree& p_Degree ) const
	{
		return CRadian( m_fRad - p_Degree.GetRadian() );
	}
	//-------------------------------------------------------------------
}
