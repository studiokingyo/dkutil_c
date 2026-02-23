/*!
@file dkcThreadLock.h
@author d金魚
@since 2004/3/22
@note
Critical SectionのWrapper
2005/12/31にPOSIX対応、pthreadを使えるようにしたがコンパイルできるかどうかは不明

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
	dkctThreadID	mLockedThread ;
	int					mLockCount ;
}DKC_THREAD_LOCK;

typedef struct dkc_ThreadRWLock{
	CRITICAL_SECTION cs;
	int reader_count;
	HANDLE writer_event;
}DKC_THREAD_RWLOCK;

typedef struct dkc_ThreadCond{
	HANDLE event;
	CRITICAL_SECTION waiter_cs;
	int waiter_count;
}DKC_THREAD_COND;

#else

typedef void *dkctThreadID;
#define dkcdINVALID_THREAD_ID ((dkctThreadID)(NULL))

typedef struct dkc_ThreadLock{
	pthread_mutex_t mMutex;
	dkctThreadID				mLockedThread ;
	int					mLockCount ;
}DKC_THREAD_LOCK;

typedef struct dkc_ThreadRWLock{
	pthread_rwlock_t rwlock;
}DKC_THREAD_RWLOCK;

typedef struct dkc_ThreadCond{
	pthread_cond_t cond;
}DKC_THREAD_COND;

#endif

#ifdef _MT

DKC_INLINE dkctThreadID dkcGetThreadID(){
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

/* Mutex */
DKC_EXTERN DKC_THREAD_LOCK * WINAPI dkcAllocThreadLock();

DKC_EXTERN int WINAPI dkcFreeThreadLock(DKC_THREAD_LOCK **);

DKC_EXTERN void WINAPI dkcThreadLock_Lock(DKC_THREAD_LOCK *);

DKC_EXTERN void WINAPI dkcThreadLock_Unlock(DKC_THREAD_LOCK *);

DKC_EXTERN BOOL WINAPI dkcThreadLock_TryLock(DKC_THREAD_LOCK *);

DKC_EXTERN BOOL WINAPI dkcThreadLockIsLockedByThisThread(DKC_THREAD_LOCK *);
#define dkcThreadLockIsLockedByCurrentThread(p) dkcThreadLockIsLockedByThisThread(p)

/* RWLock */
DKC_EXTERN DKC_THREAD_RWLOCK * WINAPI dkcAllocThreadRWLock(void);
DKC_EXTERN int WINAPI dkcFreeThreadRWLock(DKC_THREAD_RWLOCK **pp);
DKC_EXTERN void WINAPI dkcThreadRWLock_ReaderLock(DKC_THREAD_RWLOCK *p);
DKC_EXTERN void WINAPI dkcThreadRWLock_ReaderUnlock(DKC_THREAD_RWLOCK *p);
DKC_EXTERN void WINAPI dkcThreadRWLock_WriterLock(DKC_THREAD_RWLOCK *p);
DKC_EXTERN void WINAPI dkcThreadRWLock_WriterUnlock(DKC_THREAD_RWLOCK *p);

/* Condition Variable */
DKC_EXTERN DKC_THREAD_COND * WINAPI dkcAllocThreadCond(void);
DKC_EXTERN int WINAPI dkcFreeThreadCond(DKC_THREAD_COND **pp);
DKC_EXTERN void WINAPI dkcThreadCond_Wait(DKC_THREAD_COND *cond, DKC_THREAD_LOCK *mutex);
DKC_EXTERN void WINAPI dkcThreadCond_Signal(DKC_THREAD_COND *cond);
DKC_EXTERN void WINAPI dkcThreadCond_Broadcast(DKC_THREAD_COND *cond);

#if !defined( DKUTIL_C_THREAD_LOCK_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcThreadLock.c"
#endif

#endif /* _MT */

#endif
