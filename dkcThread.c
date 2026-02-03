
/*!
@file dkcThread.c
@author d‹à‹›
@since 2005/11/16
*/

#define DKUTIL_C_THREAD_LOCK_C

#include "dkcThread.h"
#include "dkcStdio.h"


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

#endif
	return edk_SUCCEEDED;

}


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


#endif
	return edk_SUCCEEDED;
}

DKC_INLINE int WINAPI dkcGetCurrentThreadPriority(int *priority)
{
	DKC_THREAD t;
	int r;
#ifdef WIN32
	r = dkcGetCurrentThread(&t);
	if(DKUTIL_FAILED(r)){return r;}
	r = dkcGetThreadPriority(&t,priority);
#else

#endif
	return r;

}

DKC_INLINE int WINAPI dkcSetCurrentThreadPriority(int priority)
{
	DKC_THREAD t;
	int r;
#ifdef WIN32
	r = dkcGetCurrentThread(&t);
	if(DKUTIL_FAILED(r)){return r;}
	r = dkcSetThreadPriority(&t,priority);
#else

#endif
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


#endif
	return edk_SUCCEEDED;


}

int WINAPI dkcGetCurrentProcessPriority(int *priority){
	DKC_THREAD t;
	int r;
#ifdef WIN32
	r = dkcGetCurrentThread(&t);
	if(DKUTIL_FAILED(r)){return r;}
	r = dkcGetProcessPriority(&t,priority);
#else

#endif
	return r;
}
int WINAPI dkcSetCurrentProcessPriority(int priority){
	DKC_THREAD t;
	int r;
#ifdef WIN32
	r = dkcGetCurrentThread(&t);
	if(DKUTIL_FAILED(r)){return r;}
	r = dkcSetProcessPriority(&t,priority);
#else

#endif
	return r;
}
