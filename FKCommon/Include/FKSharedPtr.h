/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKSharedPtr
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
namespace FKCommon
{
	/** 引用计数共享指针。
	* @remarks	这是一个标准的引用计数指针，也有些地方称之为SmartPtr，
	*			意思是一样的，它会在没有对象调用时自己释放。
	*			FK引擎里不会过多的使用它，因为显式delete指针更加高效。
	*			但是个别情况下你的确无法知道一个对象被多少人使用时，
	*			可以考虑使用它。
	* @note		它是多线程安全的。
	*/
	template< class T >
	class TSharedPtr
	{
	public:
		FK_AUTO_SHARED_MUTEX
	public:
		/**
		* Comment: 	构造函数
		* @param:
		* @return:
		* @remarks:	没有进行初始化创建工作
		* @note:
		*/

		TSharedPtr()
			: m_pUseCount( 0 )
			, m_pDataPtr( NULL )
		{
			FK_SET_AUTO_SHARED_MUTEX_NULL
		}
		explicit TSharedPtr( T* p_pPtr )
			: m_pDataPtr( p_pPtr )
			, m_pUseCount( new unsigned int(1) )
		{
			FK_SET_AUTO_SHARED_MUTEX_NULL
			FK_NEW_AUTO_SHARED_MUTEX
		}
		TSharedPtr( const TSharedPtr& p_Other )
			: m_pDataPtr( 0 )
			, m_pUseCount( 0 )
		{
			FK_SET_AUTO_SHARED_MUTEX_NULL
			FK_MUTEX_CONDITIONAL( p_Other.FK_AUTO_MUTEX_NAME )
			{
				FK_LOCK_MUTEX( *p_Other.FK_AUTO_MUTEX_NAME )
				FK_COPY_AUTO_SHARED_MUTEX( p_Other.FK_AUTO_MUTEX_NAME )
				m_pDataPtr = p_Other.m_pDataPtr;
				m_pUseCount = p_Other.m_pUseCount;
				if( m_pUseCount != NULL )
				{
					++(*m_pUseCount);
				}
			}
		}
		TSharedPtr& operator= ( const TSharedPtr& p_Other )
		{
			if( m_pDataPtr == p_Other.m_pDataPtr )
			{
				return *this;
			}
			Release();
			FK_MUTEX_CONDITIONAL( p_Other.FK_AUTO_MUTEX_NAME )
			{
				FK_LOCK_MUTEX( *p_Other.FK_AUTO_MUTEX_NAME )
					FK_COPY_AUTO_SHARED_MUTEX( p_Other.FK_AUTO_MUTEX_NAME )
					m_pDataPtr = p_Other.m_pDataPtr;
				m_pUseCount = p_Other.m_pUseCount;
				if( m_pUseCount != NULL )
				{
					++(*m_pUseCount);
				}
			}
			return *this;
		}
		virtual ~TSharedPtr()
		{
			Release();
		}
	public:
		__forceinline T& operator*() const
		{
			assert( m_pDataPtr );
			return *m_pDataPtr;
		}
		__forceinline T* operator->() const
		{
			assert( m_pDataPtr );
			return m_pDataPtr;
		}
		__forceinline T* Get() const
		{
			return m_pDataPtr;
		}
		__forceinline bool IsUnique() const
		{
			FK_LOCK_AUTO_SHARED_MUTEX
			assert( m_pUseCount != NULL );
			return ( *m_pUseCount == 1 );
		}
		__forceinline unsigned int GetUseCount() const
		{
			FK_LOCK_AUTO_SHARED_MUTEX
			assert( m_pUseCount != NULL );
			return *m_pUseCount;
		}
		__forceinline unsigned int GetUseCountPtr() const
		{
			return m_pUseCount;
		}
		__forceinline T* GetPtr() const
		{
			return m_pDataPtr;
		}
		__forceinline bool IsNull() const
		{
			return ( m_pDataPtr == NULL );
		}
		__forceinline void Clear()
		{
			if( m_pDataPtr != NULL )
			{
				Release();
				m_pDataPtr = NULL;
				m_pUseCount = NULL;
			}
		}
	public:
		/**
		* Comment: 	绑定数据指针到引用计数共享指针内
		* @param:
		* @return:
		* @remarks:	假设该引用计数共享指针未被初始化
		* @note:
		*/
		void Bind( T* p_pDataPtr )
		{
			assert( m_pDataPtr == NULL );
			assert( m_pUseCount == NULL );
			FK_NEW_AUTO_SHARED_MUTEX
			FK_LOCK_AUTO_SHARED_MUTEX
			m_pUseCount = new unsigned int(1);
			m_pDataPtr = p_pDataPtr;
		}
	protected:
		inline void Release()
		{
			bool bDestroyThis = false;

			FK_MUTEX_CONDITIONAL( FK_AUTO_MUTEX_NAME )
			{
				FK_LOCK_AUTO_SHARED_MUTEX
				if( NULL != m_pUseCount )
				{
					if( (--(*m_pUseCount)) == 0 )
					{
						bDestroyThis = true;
					}
				}
			}
			if( bDestroyThis )
			{
				Destroy();
			}
			FK_SET_AUTO_SHARED_MUTEX_NULL
		}

		virtual void Destroy()
		{
			// 如果你当在这里，这说明你在退出程序之前有指针忘记释放
			// 请在退出前使用Clear()函数。
			delete m_pDataPtr;
			delete m_pUseCount;
			FK_DELETE_AUTO_SHARED_MUTEX
		}
	protected:
		///	真正的数据指针
		T*				m_pDataPtr;
		/// 引用计数
		unsigned int*	m_pUseCount;
	};

	template< class T, class U >
	inline bool operator==( TSharedPtr< T > const& p_A, TSharedPtr< U > const& p_B )
	{
		return (p_A.Get() == p_B.Get());
	}
	template< class T, class U >
	inline bool operator!=( TSharedPtr< T > const& p_A, TSharedPtr< U > const& p_B )
	{
		return (p_A.Get() != p_B.Get());
	}
}