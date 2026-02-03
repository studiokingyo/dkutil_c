
/*!
@file dkcThreadLock.c
@author d金魚
@since 2004/3/xx
*/

#define DKUTIL_C_THREAD_LOCK_C

#include "dkcThreadLock.h"
#include "dkcStdio.h"



DKC_THREAD_LOCK * WINAPI dkcAllocThreadLock(){
	DKC_THREAD_LOCK *p = (DKC_THREAD_LOCK *)dkcAllocate(sizeof(DKC_THREAD_LOCK));
	if(NULL==p){
		return NULL;
	}
#ifdef WIN32
	InitializeCriticalSection(&(p->m_csCriticalSection));

#else
	pthread_mutex_init(&(p->mMutex), NULL);  // POSIX

#endif
	p->mLockedThread = dkcdINVALID_THREAD_ID;
	p->mLockCount	= 0;
	
	return p;
}

int WINAPI dkcFreeThreadLock(DKC_THREAD_LOCK **pp){
	const char *asstr = "dkcThreadLockをLockした回数だけUnlockしないまま終了している";
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
	//	↑異なるスレッドからここに入ってくることはできない
	//	(CriticalSectionの定義より)
#else
	pthread_mutex_lock(&(p->mMutex));
	
#endif
	p->mLockedThread = dkcGetThreadID();

	p->mLockCount++;
}


void WINAPI dkcThreadLock_Unlock(DKC_THREAD_LOCK *p){
	dkcmFORCE_NOT_ASSERT(NULL==p);

	if (p->mLockCount==0){
		dkcmNOT_ASSERT("CriticalSectionManagerをEnterしていないのにLeaveしている");
	}

	if ((--(p->mLockCount))==0) {
		p->mLockedThread = dkcdINVALID_THREAD_ID;
	}
#ifdef WIN32	
	//	↑Leaveした直後に他スレッドがEnterする可能性がある
	LeaveCriticalSection(&(p->m_csCriticalSection));
#else
	pthread_mutex_unlock(&(p->mMutex));
#endif
}

/*
BOOL WINAPI dkcThreadLockIsInited(DKC_THREAD_LOCK *){

}
*/
BOOL WINAPI dkcThreadLockIsLockedByThisThread(DKC_THREAD_LOCK *p)
{
	dkctThreadID dw;

	dkcmFORCE_NOT_ASSERT(NULL==p);
	dw = dkcGetThreadID();

	return (p->mLockedThread == dw);
}


/*
static CRITICAL_SECTION g_csCriticalSection;
static DWORD				g_dwLockedThread = 0;	//	LockしているThreadId(0:非Lock)
static int					gLockCount = 0;		//	Lockされている回数
	//	(同一スレッドならば複数回Lockできるので)
//初期化しているかどうかフラグ。
static BOOL gInited = FALSE;



static void Init(){
	{
		if(TRUE==gInited){return;}
	}
	InitializeCriticalSection(&g_csCriticalSection);
	g_dwLockedThread = (DWORD)-1;
	gLockCount	= 0;
#	ifdef DEBUG
	ODS("dkcLockThreadInit\n");
#	endif
	gInited = TRUE;
}

static void End(){
	{
		if(FALSE==gInited){return;}
	}
	DeleteCriticalSection(&g_csCriticalSection);
	gInited = FALSE;
	memset(&g_csCriticalSection,0,sizeof(g_csCriticalSection));
	if (gLockCount!=0) {
		dkcmNOT_ASSERT("dkcThreadLockをLockした回数だけUnlockしないまま終了している");
	}
#	ifdef DEBUG
	ODS("dkcLockThreadEnd\n");
#	endif
}


///	CriticalSectionに入る
static void Enter(){
	if(FALSE==gInited){
		dkcmFORCE_NOT_ASSERT("dkcThreadLockInit()で初期化していない");
		return;
	}
	EnterCriticalSection(&g_csCriticalSection);
	
	g_dwLockedThread = GetCurrentThreadId();
	//	↑異なるスレッドからここに入ってくることはできない
	//	(CriticalSectionの定義より)
	gLockCount++;

}

///CriticalSectionから抜ける
static void Leave(){
	if(FALSE==gInited){
		dkcmFORCE_NOT_ASSERT("dkcThreadLockInit()で初期化していない");
		return;
	}
	if (gLockCount==0){
		dkcmNOT_ASSERT("CriticalSectionManagerをEnterしていないのにLeaveしている");
	}

	if (--gLockCount==0) {
		g_dwLockedThread = (DWORD)-1;
	}
	//	↑Leaveした直後に他スレッドがEnterする可能性がある
	LeaveCriticalSection(&g_csCriticalSection);
}

BOOL WINAPI dkcThreadLockIsLockedByThisThread(){
	DWORD dw = GetCurrentThreadId();
	return (g_dwLockedThread == dw);
}

BOOL WINAPI dkcThreadLockIsInited()
{
	return gInited;
}

void WINAPI dkcThreadLockInit(){
	Init();
}

void WINAPI dkcThreadLock_Lock(){
	Enter();
}

void WINAPI dkcThreadLock_Unlock(){
	Leave();
}

void WINAPI dkcThreadLockEnd(){
	End();
}
*/