/********************************************************************
*
*	本文件是FKMMORPG的一部分。本文件版权归属FreeKnightDuzhi(王宏张)所有。
*	E-mail: duzhi5368@163.com
*	QQ: 	281862942
* 	
*	-----------------------------------------------------------------
*
*	文件名：	FKInsideMacro
*	作者：		FreeKnightDuzhi[ 王宏张 ]
*	文件说明：	内部宏定义
*
*	修改人:		
*	修改内容:
*********************************************************************/

#pragma once

//-------------------------------------------------------------------
#define USE_FK_MUTILCHAR					// 使用单字节字符集
#define USE_FK_MUTILTHREAD					// 使用多线程模式
//-------------------------------------------------------------------
#ifdef FKCOMMON_EXPORTS
	#define FK_DLL __declspec( dllexport )
#else
	#define FK_DLL __declspec( dllimport )
#endif
//-------------------------------------------------------------------
#if defined( _DEBUG ) || defined ( DEBUG )
	#define FK_DEBUG_MODE	1
#else
	#define FK_DEBUG_MODE	0
#endif
//-------------------------------------------------------------------
#ifdef USE_FK_MUTILCHAR
	#define FKCHAR		char
	#define FKSTRING	std::string
#else
	#define FKCHAR		wchar_t
	#define FKSTRING	std::wstring
#endif
//-------------------------------------------------------------------
#ifndef OUT
#define OUT
#endif
//-------------------------------------------------------------------
#define FK_AUTO_MUTEX_NAME	FKAutoMutex
//-------------------------------------------------------------------
#ifdef USE_FK_MUTILTHREAD
	#define FK_AUTO_MUTEX						mutable CFKRecursiveMutex FK_AUTO_MUTEX_NAME;
	#define FK_LOCK_AUTO_MUTEX					CFKRecursiveMutex::CFKScopedLock FKAutoMutexLock( FK_AUTO_MUTEX_NAME );
	#define FK_MUTEX(mutexName)					mutable CFKRecursiveMutex mutexName;
	#define FK_LOCK_MUTEX(mutexName)			CFKRecursiveMutex::CFKScopedLock FKNamedMutexLock( mutexName );	
	#define FK_AUTO_SHARED_MUTEX				mutable CFKRecursiveMutex *FK_AUTO_MUTEX_NAME;
	#define FK_LOCK_AUTO_SHARED_MUTEX			assert(FK_AUTO_MUTEX_NAME); CFKRecursiveMutex::CFKScopedLock FKAutoMutexLock(*FK_AUTO_MUTEX_NAME);
	#define FK_NEW_AUTO_SHARED_MUTEX			assert(!FK_AUTO_MUTEX_NAME); FK_AUTO_MUTEX_NAME = new CFKRecursiveMutex();
	#define FK_DELETE_AUTO_SHARED_MUTEX			assert(FK_AUTO_MUTEX_NAME); delete FK_AUTO_MUTEX_NAME;
	#define FK_COPY_AUTO_SHARED_MUTEX(other)	assert(!FK_AUTO_MUTEX_NAME); FK_AUTO_MUTEX_NAME = other;
	#define FK_SET_AUTO_SHARED_MUTEX_NULL		FK_AUTO_MUTEX_NAME = 0;
	#define FK_MUTEX_CONDITIONAL(mutex)			if( mutex )
#else
	#define FK_AUTO_MUTEX
	#define FK_LOCK_AUTO_MUTEX
	#define FK_MUTEX(mutexName)
	#define FK_LOCK_MUTEX(mutexName)
	#define FK_AUTO_SHARED_MUTEX
	#define FK_LOCK_AUTO_SHARED_MUTEX
	#define FK_NEW_AUTO_SHARED_MUTEX
	#define FK_DELETE_AUTO_SHARED_MUTEX
	#define FK_COPY_AUTO_SHARED_MUTEX(other)
	#define FK_SET_AUTO_SHARED_MUTEX_NULL
	#define FK_MUTEX_CONDITIONAL(mutex)
#endif
//-------------------------------------------------------------------
#define	PI					3.1415926535897932384626433832795028841971693993751f	// PI
#define	HALFPI				1.57079632679489661923f									// 0.5 * PI
#define	TWOPI				6.28318530717958647692f									// 2.0 * PI
#define	INVPI				0.31830988618379067154f									// 1.0 / PI
//-------------------------------------------------------------------