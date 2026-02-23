
/*!
@file dkcThread.c
@author d金魚
@since 2005/11/16
*/

#define DKUTIL_C_THREAD_LOCK_C

#include "dkcThread.h"
#include "dkcStdio.h"

#ifdef _MT


DKC_INLINE DKC_THREAD *WINAPI dkcAllocThread()
{
	return dkcAllocate(sizeof(DKC_THREAD));

}

DKC_INLINE int WINAPI dkcFreeThread(DKC_THREAD **pp){
	return dkcFree((void **)pp);
}

DKC_INLINE int WINAPI dkcGetCurrentThread(DKC_THREAD *out)
{
#ifdef WIN32
	out->handle = GetCurrentThread();
	out->id = GetCurrentThreadId();
#else
	out->handle = pthread_self();
#endif
	return edk_SUCCEEDED;

}

/* ========================================
 * Thread Create / Join / Detach
 * ======================================== */

DKC_EXTERN int WINAPI dkcThreadCreate(DKC_THREAD **out, DKC_THREAD_FUNC func, void *data)
{
	DKC_THREAD *p;
	if(NULL == out || NULL == func){
		return edk_ArgumentException;
	}
	p = dkcAllocThread();
	if(NULL == p){
		return edk_FAILED;
	}
#ifdef WIN32
	{
		unsigned int tid;
		p->handle = (HANDLE)_beginthreadex(
			NULL,   /* security */
			0,      /* stack_size */
			func,
			data,
			0,      /* initflag: run immediately */
			&tid
		);
		if(p->handle == 0){
			dkcFreeThread(&p);
			return edk_FAILED;
		}
		p->id = (DWORD)tid;
	}
#else
	{
		int rc = pthread_create(&(p->handle), NULL, func, data);
		if(rc != 0){
			dkcFreeThread(&p);
			return edk_FAILED;
		}
	}
#endif
	*out = p;
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcThreadJoin(DKC_THREAD *p)
{
	if(NULL == p){
		return edk_ArgumentException;
	}
#ifdef WIN32
	if(WaitForSingleObject(p->handle, INFINITE) == WAIT_FAILED){
		return edk_FAILED;
	}
	CloseHandle(p->handle);
	p->handle = NULL;
#else
	{
		int rc = pthread_join(p->handle, NULL);
		if(rc != 0){
			return edk_FAILED;
		}
	}
#endif
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcThreadDetach(DKC_THREAD *p)
{
	if(NULL == p){
		return edk_ArgumentException;
	}
#ifdef WIN32
	if(!CloseHandle(p->handle)){
		return edk_FAILED;
	}
	p->handle = NULL;
#else
	{
		int rc = pthread_detach(p->handle);
		if(rc != 0){
			return edk_FAILED;
		}
	}
#endif
	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcThreadSleep(DWORD milliseconds)
{
#ifdef WIN32
	Sleep(milliseconds);
#else
	usleep((unsigned long)milliseconds * 1000);
#endif
}

DKC_EXTERN void WINAPI dkcThreadYield(void)
{
#ifdef WIN32
	Sleep(0);
#else
	sched_yield();
#endif
}

/* ========================================
 * Thread Priority (existing)
 * ======================================== */

DKC_EXTERN int WINAPI dkcGetThreadPriority(DKC_THREAD *p,int *priority)
{
#ifdef WIN32
	DWORD d = GetThreadPriority(p->handle);
	switch(d){
	case THREAD_PRIORITY_LOWEST:
		*priority = edkcThreadPriorityLowest;
		break;
	case THREAD_PRIORITY_BELOW_NORMAL:
		*priority = edkcThreadPriorityBelowNormal;
		break;
	case THREAD_PRIORITY_NORMAL:
		*priority = edkcThreadPriorityNormal;
		break;
	case THREAD_PRIORITY_ABOVE_NORMAL:
		*priority = edkcThreadPriorityAboveNormal;
		break;
	case THREAD_PRIORITY_HIGHEST:
		*priority = edkcThreadPriorityHighest;
		break;
	case THREAD_PRIORITY_IDLE:
		*priority = edkcThreadPriorityIdle;
		break;
	case THREAD_PRIORITY_TIME_CRITICAL:
		*priority = edkcThreadPriorityTimeCritical;
		break;
	default:
		return edk_FAILED;
	}
#else
	{
		int policy;
		struct sched_param param;
		int rc = pthread_getschedparam(p->handle, &policy, &param);
		if(rc != 0){
			return edk_FAILED;
		}
		*priority = edkcThreadPriorityNormal;
	}
#endif
	return edk_SUCCEEDED;
}
DKC_EXTERN int WINAPI dkcSetThreadPriority(DKC_THREAD *p,int priority)
{
#ifdef WIN32
	int tp;
	switch(priority){
	case edkcThreadPriorityLowest:
		tp = THREAD_PRIORITY_LOWEST;
		break;
	case edkcThreadPriorityBelowNormal:
		tp = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case edkcThreadPriorityNormal:
		tp = THREAD_PRIORITY_NORMAL;
		break;
	case edkcThreadPriorityAboveNormal:
		tp = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case edkcThreadPriorityHighest:
		tp = THREAD_PRIORITY_HIGHEST;
		break;
	case edkcThreadPriorityIdle:
		tp = THREAD_PRIORITY_IDLE;
		break;
	case edkcThreadPriorityTimeCritical:
		tp = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	default:
		return edk_FAILED;
	}
	if(FALSE==SetThreadPriority(p->handle,tp)){
		return edk_FAILED;
	}
#else
	{
		int policy;
		struct sched_param param;
		int rc = pthread_getschedparam(p->handle, &policy, &param);
		if(rc != 0){
			return edk_FAILED;
		}
		(void)priority;
		rc = pthread_setschedparam(p->handle, policy, &param);
		if(rc != 0){
			return edk_FAILED;
		}
	}
#endif
	return edk_SUCCEEDED;
}

DKC_INLINE int WINAPI dkcGetCurrentThreadPriority(int *priority)
{
	DKC_THREAD t;
	int r;
	r = dkcGetCurrentThread(&t);
	if(DKUTIL_FAILED(r)){return r;}
	r = dkcGetThreadPriority(&t,priority);
	return r;

}

DKC_INLINE int WINAPI dkcSetCurrentThreadPriority(int priority)
{
	DKC_THREAD t;
	int r;
	r = dkcGetCurrentThread(&t);
	if(DKUTIL_FAILED(r)){return r;}
	r = dkcSetThreadPriority(&t,priority);
	return r;

}

int WINAPI dkcGetProcessPriority(DKC_THREAD *p,int *priority)
{
#ifdef WIN32

	DWORD d = GetPriorityClass(p->handle);
	switch(d){
	case HIGH_PRIORITY_CLASS:
		*priority = edkcProcessPriorityHigh;
		break;
	case IDLE_PRIORITY_CLASS:
		*priority = edkcProcessPriorityIdle;
		break;
	case NORMAL_PRIORITY_CLASS:
		*priority = edkcProcessPriorityNormal;
		break;
	case REALTIME_PRIORITY_CLASS:
		*priority = edkcProcessPriorityRealTime;
		break;
	default:
		return edk_FAILED;
	}

#else
	(void)p;
	*priority = edkcProcessPriorityNormal;
#endif
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSetProcessPriority(DKC_THREAD *p,int priority){
#ifdef WIN32
	DWORD pc;
	switch(priority){
	case edkcProcessPriorityHigh:
		pc = HIGH_PRIORITY_CLASS;
		break;
	case edkcProcessPriorityIdle:
		pc =IDLE_PRIORITY_CLASS;
		break;
	case edkcProcessPriorityNormal:
		pc =NORMAL_PRIORITY_CLASS;
		break;
	case edkcProcessPriorityRealTime:
		pc =REALTIME_PRIORITY_CLASS;
		break;
	default:
		return edk_FAILED;
	}
	if(FALSE==SetPriorityClass(p->handle,pc)){
		return edk_FAILED;
	}
#else
	(void)p;
	(void)priority;
#endif
	return edk_SUCCEEDED;


}

int WINAPI dkcGetCurrentProcessPriority(int *priority){
	DKC_THREAD t;
	int r;
	r = dkcGetCurrentThread(&t);
	if(DKUTIL_FAILED(r)){return r;}
	r = dkcGetProcessPriority(&t,priority);
	return r;
}
int WINAPI dkcSetCurrentProcessPriority(int priority){
	DKC_THREAD t;
	int r;
	r = dkcGetCurrentThread(&t);
	if(DKUTIL_FAILED(r)){return r;}
	r = dkcSetProcessPriority(&t,priority);
	return r;
}

#endif /* _MT */
