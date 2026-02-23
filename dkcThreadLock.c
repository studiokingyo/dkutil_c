
/*!
@file dkcThreadLock.c
@author d金魚
@since 2004/3/xx
*/

#define DKUTIL_C_THREAD_LOCK_C

#include "dkcThreadLock.h"
#include "dkcStdio.h"

#ifdef _MT


/* ========================================
 * Mutex
 * ======================================== */

DKC_THREAD_LOCK * WINAPI dkcAllocThreadLock(){
	DKC_THREAD_LOCK *p = (DKC_THREAD_LOCK *)dkcAllocate(sizeof(DKC_THREAD_LOCK));
	if(NULL==p){
		return NULL;
	}
#ifdef WIN32
	InitializeCriticalSection(&(p->m_csCriticalSection));

#else
	pthread_mutex_init(&(p->mMutex), NULL);

#endif
	p->mLockedThread = dkcdINVALID_THREAD_ID;
	p->mLockCount	= 0;

	return p;
}

int WINAPI dkcFreeThreadLock(DKC_THREAD_LOCK **pp){
	const char *asstr = "dkcThreadLockのLock回数分だけUnlockしないまま終了している";
	if(NULL==pp || NULL==*pp){
		return edk_ArgumentException;
	}
	if ((*pp)->mLockCount!=0) {
		int i;
		dkcmNOT_ASSERT(asstr);
		for(i=0;i<(*pp)->mLockCount;i++){
			dkcThreadLock_Unlock((*pp));
		}
	}
#ifdef WIN32
	DeleteCriticalSection(&((*pp)->m_csCriticalSection));
#else
	pthread_mutex_destroy(&((*pp)->mMutex));
#endif
	if ((*pp)->mLockCount!=0) {
		dkcmFORCE_NOT_ASSERT(asstr);
	}
	return dkcFree((void **)pp);
}

void WINAPI dkcThreadLock_Lock(DKC_THREAD_LOCK *p){
	dkcmFORCE_NOT_ASSERT(NULL==p);
#ifdef WIN32
	EnterCriticalSection(&(p->m_csCriticalSection));
#else
	pthread_mutex_lock(&(p->mMutex));

#endif
	p->mLockedThread = dkcGetThreadID();

	p->mLockCount++;
}


void WINAPI dkcThreadLock_Unlock(DKC_THREAD_LOCK *p){
	dkcmFORCE_NOT_ASSERT(NULL==p);

	if (p->mLockCount==0){
		dkcmNOT_ASSERT("CriticalSectionManagerにEnterしていないのにLeaveしている");
	}

	if ((--(p->mLockCount))==0) {
		p->mLockedThread = dkcdINVALID_THREAD_ID;
	}
#ifdef WIN32
	LeaveCriticalSection(&(p->m_csCriticalSection));
#else
	pthread_mutex_unlock(&(p->mMutex));
#endif
}

BOOL WINAPI dkcThreadLock_TryLock(DKC_THREAD_LOCK *p)
{
	dkcmFORCE_NOT_ASSERT(NULL==p);
#ifdef WIN32
	if(TryEnterCriticalSection(&(p->m_csCriticalSection))){
		p->mLockedThread = dkcGetThreadID();
		p->mLockCount++;
		return TRUE;
	}
	return FALSE;
#else
	if(pthread_mutex_trylock(&(p->mMutex)) == 0){
		p->mLockedThread = dkcGetThreadID();
		p->mLockCount++;
		return TRUE;
	}
	return FALSE;
#endif
}

BOOL WINAPI dkcThreadLockIsLockedByThisThread(DKC_THREAD_LOCK *p)
{
	dkctThreadID dw;

	dkcmFORCE_NOT_ASSERT(NULL==p);
	dw = dkcGetThreadID();

	return (p->mLockedThread == dw);
}


/* ========================================
 * RWLock
 * ======================================== */

DKC_THREAD_RWLOCK * WINAPI dkcAllocThreadRWLock(void)
{
	DKC_THREAD_RWLOCK *p = (DKC_THREAD_RWLOCK *)dkcAllocate(sizeof(DKC_THREAD_RWLOCK));
	if(NULL == p){
		return NULL;
	}
#ifdef WIN32
	InitializeCriticalSection(&(p->cs));
	p->reader_count = 0;
	p->writer_event = CreateEvent(NULL, TRUE, TRUE, NULL); /* manual reset, initially signaled */
	if(p->writer_event == NULL){
		DeleteCriticalSection(&(p->cs));
		dkcFree((void **)&p);
		return NULL;
	}
#else
	pthread_rwlock_init(&(p->rwlock), NULL);
#endif
	return p;
}

int WINAPI dkcFreeThreadRWLock(DKC_THREAD_RWLOCK **pp)
{
	if(NULL == pp || NULL == *pp){
		return edk_ArgumentException;
	}
#ifdef WIN32
	CloseHandle((*pp)->writer_event);
	DeleteCriticalSection(&((*pp)->cs));
#else
	pthread_rwlock_destroy(&((*pp)->rwlock));
#endif
	return dkcFree((void **)pp);
}

void WINAPI dkcThreadRWLock_ReaderLock(DKC_THREAD_RWLOCK *p)
{
#ifdef WIN32
	EnterCriticalSection(&(p->cs));
	p->reader_count++;
	if(p->reader_count == 1){
		/* first reader: block writers */
		ResetEvent(p->writer_event);
	}
	LeaveCriticalSection(&(p->cs));
#else
	pthread_rwlock_rdlock(&(p->rwlock));
#endif
}

void WINAPI dkcThreadRWLock_ReaderUnlock(DKC_THREAD_RWLOCK *p)
{
#ifdef WIN32
	EnterCriticalSection(&(p->cs));
	p->reader_count--;
	if(p->reader_count == 0){
		/* last reader: allow writers */
		SetEvent(p->writer_event);
	}
	LeaveCriticalSection(&(p->cs));
#else
	pthread_rwlock_unlock(&(p->rwlock));
#endif
}

void WINAPI dkcThreadRWLock_WriterLock(DKC_THREAD_RWLOCK *p)
{
#ifdef WIN32
	/* wait until no readers */
	WaitForSingleObject(p->writer_event, INFINITE);
	EnterCriticalSection(&(p->cs));
	/* prevent new readers while we hold CS */
	ResetEvent(p->writer_event);
#else
	pthread_rwlock_wrlock(&(p->rwlock));
#endif
}

void WINAPI dkcThreadRWLock_WriterUnlock(DKC_THREAD_RWLOCK *p)
{
#ifdef WIN32
	SetEvent(p->writer_event);
	LeaveCriticalSection(&(p->cs));
#else
	pthread_rwlock_unlock(&(p->rwlock));
#endif
}

/* ========================================
 * Condition Variable
 * ======================================== */

DKC_THREAD_COND * WINAPI dkcAllocThreadCond(void)
{
	DKC_THREAD_COND *p = (DKC_THREAD_COND *)dkcAllocate(sizeof(DKC_THREAD_COND));
	if(NULL == p){
		return NULL;
	}
#ifdef WIN32
	p->event = CreateEvent(NULL, FALSE, FALSE, NULL); /* auto reset */
	if(p->event == NULL){
		dkcFree((void **)&p);
		return NULL;
	}
	InitializeCriticalSection(&(p->waiter_cs));
	p->waiter_count = 0;
#else
	pthread_cond_init(&(p->cond), NULL);
#endif
	return p;
}

int WINAPI dkcFreeThreadCond(DKC_THREAD_COND **pp)
{
	if(NULL == pp || NULL == *pp){
		return edk_ArgumentException;
	}
#ifdef WIN32
	CloseHandle((*pp)->event);
	DeleteCriticalSection(&((*pp)->waiter_cs));
#else
	pthread_cond_destroy(&((*pp)->cond));
#endif
	return dkcFree((void **)pp);
}

void WINAPI dkcThreadCond_Wait(DKC_THREAD_COND *cond, DKC_THREAD_LOCK *mutex)
{
#ifdef WIN32
	EnterCriticalSection(&(cond->waiter_cs));
	cond->waiter_count++;
	LeaveCriticalSection(&(cond->waiter_cs));

	/* release the mutex, wait for signal, re-acquire mutex */
	dkcThreadLock_Unlock(mutex);
	WaitForSingleObject(cond->event, INFINITE);
	dkcThreadLock_Lock(mutex);

	EnterCriticalSection(&(cond->waiter_cs));
	cond->waiter_count--;
	LeaveCriticalSection(&(cond->waiter_cs));
#else
	/* must adjust lock tracking before pthread_cond_wait releases mutex */
	mutex->mLockCount--;
	if(mutex->mLockCount == 0){
		mutex->mLockedThread = dkcdINVALID_THREAD_ID;
	}
	pthread_cond_wait(&(cond->cond), &(mutex->mMutex));
	mutex->mLockedThread = dkcGetThreadID();
	mutex->mLockCount++;
#endif
}

void WINAPI dkcThreadCond_Signal(DKC_THREAD_COND *cond)
{
#ifdef WIN32
	{
		int have_waiters;
		EnterCriticalSection(&(cond->waiter_cs));
		have_waiters = (cond->waiter_count > 0);
		LeaveCriticalSection(&(cond->waiter_cs));
		if(have_waiters){
			SetEvent(cond->event);
		}
	}
#else
	pthread_cond_signal(&(cond->cond));
#endif
}

void WINAPI dkcThreadCond_Broadcast(DKC_THREAD_COND *cond)
{
#ifdef WIN32
	{
		int have_waiters;
		int i;
		EnterCriticalSection(&(cond->waiter_cs));
		have_waiters = cond->waiter_count;
		LeaveCriticalSection(&(cond->waiter_cs));
		for(i = 0; i < have_waiters; i++){
			SetEvent(cond->event);
		}
	}
#else
	pthread_cond_broadcast(&(cond->cond));
#endif
}

#endif /* _MT */
