/*!
@file dkcThreadLock.h
@author d金魚
@since 2004/3/22
@note
Critical SectionのWrapper
いずれmulti platformにしたい
@brief スレッドのロック（CriticalSection）
*/

#ifndef DKUTIL_C_THREAD_LOCK_H
#define DKUTIL_C_THREAD_LOCK_H

#include "dkcOSIndependent.h"

#ifdef WIN32

typedef struct dkc_ThreadLock{
	CRITICAL_SECTION m_csCriticalSection;
	///	LockしているThreadId(0:非Lock)
	DWORD				m_dwLockedThread ;
	///	Lockされている回数
	int					mLockCount ;
}DKC_THREAD_LOCK;

#endif

DKC_EXTERN DKC_THREAD_LOCK * WINAPI dkcAllocThreadLock();

DKC_EXTERN int WINAPI dkcFreeThreadLock(DKC_THREAD_LOCK **);

DKC_EXTERN void WINAPI dkcThreadLock_Lock(DKC_THREAD_LOCK *);


DKC_EXTERN void WINAPI dkcThreadLock_Unlock(DKC_THREAD_LOCK *);


//DKC_EXTERN BOOL WINAPI dkcThreadLockIsInited(DKC_THREAD_LOCK *);

DKC_EXTERN BOOL WINAPI dkcThreadLockIsLockedByThisThread(DKC_THREAD_LOCK *);

/*
DKC_EXTERN void WINAPI dkcThreadLockInit();

DKC_EXTERN void WINAPI dkcThreadLock_Lock();

DKC_EXTERN void WINAPI dkcThreadLock_Unlock();

DKC_EXTERN void WINAPI dkcThreadLockEnd();

DKC_EXTERN BOOL WINAPI dkcThreadLockIsInited();

DKC_EXTERN BOOL WINAPI dkcThreadLockIsLockedByThisThread();
*/
#if !defined( DKUTIL_C_THREAD_LOCK_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcThreadLock.c"
#endif

#endif //end of include once