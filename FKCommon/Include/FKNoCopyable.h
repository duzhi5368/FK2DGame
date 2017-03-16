/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKNoCopyable
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------

namespace FKCommon
{
	/** 私有拷贝构造和私有重载操作符类，保证继承于该类的子类不可能被拷贝。
	*/
	class FK_DLL IFKNoCopyable
	{
	protected:
		IFKNoCopyable(){}
		~IFKNoCopyable(){}
	private:
		IFKNoCopyable( const IFKNoCopyable& );
		const IFKNoCopyable& operator= ( const IFKNoCopyable& );
	};
}