/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKRecursiveMutex
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
#include <windows.h>
#include "FKNoCopyable.h"
//-------------------------------------------------------------------

namespace FKCommon
{
	class FK_DLL CFKRecursiveMutex : public IFKNoCopyable
	{
		//-------------------------------------------------------------------
		/**	一个域守卫锁。可以防止忘记释放锁。
		*/
		class CFKScopedLock : public IFKNoCopyable
		{
			friend class CFKRecursiveMutex;
		public:
			CFKScopedLock()
				: m_pMutex( NULL )
			{

			}
			CFKScopedLock( CFKRecursiveMutex& p_Mutex )
			{
				Acquire( p_Mutex );
			}
			~CFKScopedLock()
			{
				if( NULL != m_pMutex )
				{
					Release();
				}
			}
			void Acquire( CFKRecursiveMutex& p_Mutex )
			{
				m_pMutex = &p_Mutex;
				p_Mutex.Lock();
			}
			bool TryAcquire( CFKRecursiveMutex& p_Mutex )
			{
				bool bResult = p_Mutex.TryLock();
				if( bResult )
				{
					m_pMutex = &p_Mutex;
				}
				return bResult;
			}
			void Release()
			{
				m_pMutex->Unlock();
				m_pMutex = NULL;
			}
		private:
			CFKRecursiveMutex*	m_pMutex;
		};
		//-------------------------------------------------------------------
		friend class CFKScopedLock;
	public:
		CFKRecursiveMutex()
		{
			InitializeCriticalSection( &m_Impl );
		}
		~CFKRecursiveMutex()
		{
			DeleteCriticalSection( &m_Impl );
		}
		void Lock()
		{
			EnterCriticalSection( &m_Impl );
		}
		bool TryLock()
		{
			return ( TryEnterCriticalSection( &m_Impl ) != 0 );
		}
		void Unlock()
		{
			LeaveCriticalSection( &m_Impl );
		}
	private:
		CRITICAL_SECTION		m_Impl;
	};
}