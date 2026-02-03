/*!
@file dkcThreadLock.h
@author d金魚
@since 2004/3/22
@note
Critical SectionのWrapper
<s>いずれmulti platformにしたい</s>
2005/12/31にPOSIX？に対応、pthreadを使えるようにしたけどコンパイルできるかどうかは不明

@brief スレッドのロック（CRITICAL_SECTION,pthread_mutex_t）
*/

#ifndef DKUTIL_C_THREAD_LOCK_H
#define DKUTIL_C_THREAD_LOCK_H

#include "dkcOSIndependent.h"

#ifdef WIN32
typedef DWORD dkctThreadID;
#define dkcdINVALID_THREAD_ID ((dkctThreadID)(-1))

typedef struct dkc_ThreadLock{
	CRITICAL_SECTION m_csCriticalSection;
	///	LockしているThreadId(0:非Lock)
	dkctThreadID	mLockedThread ;
	///	Lockされている回数
	int					mLockCount ;
}DKC_THREAD_LOCK;

#else

typedef void *dkctThreadID;
#define dkcdINVALID_THREAD_ID ((dkctThreadID)(NULL))

typedef struct dkc_ThreadLock{
	pthread_mutex_t mMutex;
	///	LockしているThreadId(0:非Lock)
	dkctThreadID				mLockedThread ;
	///	Lockされている回数
	int					mLockCount ;
}DKC_THREAD_LOCK;


#endif

DKC_INLINE dkctThreadID dkcGetThreadID(){
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

DKC_EXTERN DKC_THREAD_LOCK * WINAPI dkcAllocThreadLock();

DKC_EXTERN int WINAPI dkcFreeThreadLock(DKC_THREAD_LOCK **);

DKC_EXTERN void WINAPI dkcThreadLock_Lock(DKC_THREAD_LOCK *);


DKC_EXTERN void WINAPI dkcThreadLock_Unlock(DKC_THREAD_LOCK *);


//DKC_EXTERN BOOL WINAPI dkcThreadLockIsInited(DKC_THREAD_LOCK *);

DKC_EXTERN BOOL WINAPI dkcThreadLockIsLockedByThisThread(DKC_THREAD_LOCK *);
#define dkcThreadLockIsLockedByCurrentThread(p) dkcThreadLockIsLockedByThisThread(p)
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