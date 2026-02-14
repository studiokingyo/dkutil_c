/*!
@file dkcThread.h
@author d金魚
@since 2005/11/16
@note

@brief Thread関係ユーティリティ
*/

#ifndef DKUTIL_C_THREAD_H
#define DKUTIL_C_THREAD_H

#include "dkcThreadLock.h"

#ifdef WIN32
#include <process.h>

typedef struct dkc_Thread{
	HANDLE handle;
	DWORD id;
}DKC_THREAD;

typedef unsigned int (WINAPI *DKC_THREAD_FUNC)(void *data);

enum edkcThreadPriority{
	edkcThreadPriorityLowest = 0,
	edkcThreadPriorityBelowNormal ,
	edkcThreadPriorityNormal ,
	edkcThreadPriorityAboveNormal ,
	edkcThreadPriorityHighest ,
	edkcThreadPriorityIdle,
	edkcThreadPriorityTimeCritical
};

enum edkcProcessPriority{
	edkcProcessPriorityHigh = 0,
	edkcProcessPriorityIdle ,
	edkcProcessPriorityNormal ,
	edkcProcessPriorityRealTime
};
#else

#include <pthread.h>
#include <sched.h>
#include <unistd.h>

typedef struct dkc_Thread{
	pthread_t handle;
}DKC_THREAD;

typedef void* (*DKC_THREAD_FUNC)(void *data);

enum edkcThreadPriority{
	edkcThreadPriorityLowest = 0,
	edkcThreadPriorityBelowNormal ,
	edkcThreadPriorityNormal ,
	edkcThreadPriorityAboveNormal ,
	edkcThreadPriorityHighest ,
	edkcThreadPriorityIdle,
	edkcThreadPriorityTimeCritical
};

enum edkcProcessPriority{
	edkcProcessPriorityHigh = 0,
	edkcProcessPriorityIdle ,
	edkcProcessPriorityNormal ,
	edkcProcessPriorityRealTime
};

#endif

DKC_EXTERN DKC_THREAD *WINAPI dkcAllocThread();

DKC_EXTERN int WINAPI dkcFreeThread(DKC_THREAD **p);

DKC_EXTERN int WINAPI dkcGetCurrentThread(DKC_THREAD *out);

DKC_EXTERN int WINAPI dkcThreadCreate(DKC_THREAD **out, DKC_THREAD_FUNC func, void *data);

DKC_EXTERN int WINAPI dkcThreadJoin(DKC_THREAD *p);

DKC_EXTERN int WINAPI dkcThreadDetach(DKC_THREAD *p);

DKC_EXTERN void WINAPI dkcThreadSleep(DWORD milliseconds);

DKC_EXTERN void WINAPI dkcThreadYield(void);

DKC_EXTERN int WINAPI dkcGetThreadPriority(DKC_THREAD *p,int *priority);
DKC_EXTERN int WINAPI dkcSetThreadPriority(DKC_THREAD *p,int priority);

DKC_EXTERN int WINAPI dkcGetCurrentThreadPriority(int *priority);
DKC_EXTERN int WINAPI dkcSetCurrentThreadPriority(int priority);



DKC_EXTERN int WINAPI dkcGetProcessPriority(DKC_THREAD *p,int *priority);
DKC_EXTERN int WINAPI dkcSetProcessPriority(DKC_THREAD *p,int priority);

DKC_EXTERN int WINAPI dkcGetCurrentProcessPriority(int *priority);
DKC_EXTERN int WINAPI dkcSetCurrentProcessPriority(int priority);

#endif
