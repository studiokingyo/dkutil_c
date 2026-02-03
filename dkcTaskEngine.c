
/*!
@file dkcTaskEngine.c
@brief		いわいるタスク処理
@author		d金魚
@since		2004/08/22
*/
#define DKUTIL_C_TASK_ENGINE_C
#include "dkcTaskEngine.h"

DKC_TASK_ENGINE *dkcAllocTaskEngine(size_t taskworksize){
	BYTE *temp = NULL;

	DKC_TASK_ENGINE *p = NULL;
	DKC_TASK *tp = NULL;

	if(0==taskworksize){
		return NULL;
	}


	
	p = dkcAllocate(sizeof(DKC_TASK_ENGINE));
	if(NULL==p){
		return NULL;
	}

	temp = dkcAllocate(taskworksize * dkcdTASK_NUM);
	if(NULL==temp){
		goto Error;
	}
	tp = dkcAllocate(sizeof(DKC_TASK) * dkcdTASK_NUM);
	if(NULL==tp){
		goto Error;
	}



	dkcTaskEngineInit(p,&tp,&temp);

	return p;


Error:
	//dkcFree((void **)&tp);
	dkcFree((void **)&temp);
	dkcFree((void **)&p);

	return NULL;
}

int dkcFreeTaskEngine(DKC_TASK_ENGINE **pp){
	dkcFree((void **) (*pp)->mTaskPool );//&[0]
	dkcFree((void **) (*pp)->mTaskWorkPool );//&[0]
	return dkcFree((void **) pp );
}

static void Return(DKC_TASK_ENGINE *a,DKC_TASK *b)
{
	return;
}

void dkcTaskEngineInit(DKC_TASK_ENGINE *p,DKC_TASK *TaskPool[dkcdTASK_NUM],BYTE *TaskWorkPool[dkcdTASK_NUM]){
	int i;
	size_t taskworksize = p->mTaskWorkSize;
	p->mNowPriority = 0;
	
	for(i=0;i<dkcdTASK_NUM;i++){
		p->mTaskWorkPool[i] = TaskWorkPool[i * taskworksize];
		
		TaskPool[i]->proc = Return;
		TaskPool[i]->work = p->mTaskWorkPool[i];

		p->mTaskPool[i] = (BYTE *)TaskPool[i];
	}
}

///タスクを設定します。使用済みのプライオリティーの場合等で設定できなかった場合はedk_FAILEDを返します。
int dkcTaskEngineSet(DKC_TASK_ENGINE *p,TASK_COUNT_T prio,DKC_TASK_PROC proc){
	DKC_TASK_PROC sentinel = Return;
	DKC_TASK *pt = (DKC_TASK *)p->mTaskPool[prio];

	if(pt->proc != sentinel)
	{	//使用している。
		return edk_FAILED;
	}

	//使用していないなら登録
	pt->proc = proc;

	return edk_SUCCEEDED;
}

///タスクを削除します。
int dkcTaskEngineKill(DKC_TASK_ENGINE *p,TASK_COUNT_T prio)
{
	DKC_TASK *pt = (DKC_TASK *)p->mTaskPool[prio];

	pt->proc  = Return;
	return edk_SUCCEEDED;
}

///タスクエンジンに登録されているタスクをイテレーションします。
int dkcTaskEngineIteration(DKC_TASK_ENGINE *p){
	unsigned int i;
	//一時変数
	DKC_TASK_PROC proc;
	DKC_TASK *task_p;

	for(i=0;i<dkcdTASK_NUM;i++){
		task_p = (DKC_TASK *)(p->mTaskPool[i]);
		proc = task_p->proc;
		p->mNowPriority = (TASK_COUNT_T)i;
		
		//呼び出す。
		proc(p,task_p);

	}
	return edk_SUCCEEDED;

}


///prioのタスクは空いているかどうか　TRUEなら空いている
BOOL dkcTaskEngineEmpty(DKC_TASK_ENGINE *p,TASK_COUNT_T prio){
	DKC_TASK_PROC tp = Return;
	DKC_TASK *task_p = (DKC_TASK *)p->mTaskPool[prio];

	return (task_p->proc==tp);
}





