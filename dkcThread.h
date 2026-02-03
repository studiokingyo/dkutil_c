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

typedef struct dkc_Thread{
	HANDLE handle;
	DWORD id;
}DKC_THREAD;

enum edkcThreadPriority{
	edkcThreadPriorityLowest = 0,
	edkcThreadPriorityBelowNormal ,
	edkcThreadPriorityNormal ,
	edkcThreadPriorityAboveNormal ,
	edkcThreadPriorityHighest ,
	edkcThreadPriorityIdle,
	edkcThreadPriorityTimeCritical,
};

enum edkcProcessPriority{
	edkcProcessPriorityHigh = 0,
	edkcProcessPriorityIdle , 
	edkcProcessPriorityNormal ,
	edkcProcessPriorityRealTime  ,
};
#else

#endif

DKC_EXTERN DKC_THREAD *WINAPI dkcAllocThread();

DKC_EXTERN int WINAPI dkcFreeThread(DKC_THREAD **p);

DKC_EXTERN int WINAPI dkcGetCurrentThread(DKC_THREAD *out);

DKC_EXTERN int WINAPI dkcGetThreadPriority(DKC_THREAD *p,int *priority);
DKC_EXTERN int WINAPI dkcSetThreadPriority(DKC_THREAD *p,int priority);

DKC_EXTERN int WINAPI dkcGetCurrentThreadPriority(int *priority);
DKC_EXTERN int WINAPI dkcSetCurrentThreadPriority(int priority);



DKC_EXTERN int WINAPI dkcGetProcessPriority(DKC_THREAD *p,int *priority);
DKC_EXTERN int WINAPI dkcSetProcessPriority(DKC_THREAD *p,int priority);

DKC_EXTERN int WINAPI dkcGetCurrentProcessPriority(int *priority);
DKC_EXTERN int WINAPI dkcSetCurrentProcessPriority(int priority);

#endif //end of include once