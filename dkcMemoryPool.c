#include "dkcMemoryPool.h"




///DKC_SAME_OBJECT_POOL�̖{�̂��m�ۂ��܂��B
static DKC_INLINE DKC_SAME_OBJECT_POOL *alloc_sameobjectpool(){
	return dkcAllocate(sizeof(DKC_SAME_OBJECT_POOL));

}

///DKC_SAME_OBJECT_POOL�{�̂��J�����܂��B
static DKC_INLINE int free_sameobjectpool(DKC_SAME_OBJECT_POOL *p){
	return dkcFree(&p);
}

static DKC_INLINE DKC_SAME_OBJECT_POOL *alloc_sameobjectpool_outer(DKC_ALLOC_FUNC_TYPE func){
	/*if(NULL==func){
		return alloc_sameobjectpool();
	}*/
	return func(sizeof(DKC_SAME_OBJECT_POOL));
}
static DKC_INLINE int free_sameobjectpool_outer(void *p,DKC_FREE_FUNC_TYPE func){
	if(!p){
		return edk_FAILED;
	}
	func(p);
	return edk_SUCCEEDED;
}



DKC_INLINE int WINAPI dkcSameObjectPoolInit(
	DKC_SAME_OBJECT_POOL *p,size_t object_size,size_t max_num,
	DKC_ALLOC_FUNC_TYPE alloc_f,DKC_FREE_FUNC_TYPE free_f)
{
	BOOL af,ff;
	if(NULL==p){
		return edk_FAILED;
	}
	af = (NULL==alloc_f);
	ff = (NULL==free_f);
	if(af && ff){//��Ƃ�NULL��������łӂ���ƃA���P�[�^������B
		alloc_f = dkcMallocAdapter;
		free_f = dkcFreeAdapter;
		goto PROC;
	}
	if(NULL==alloc_f || NULL==free_f){
		return edk_ArgumentException;
	}
PROC:

	p->alloc_f = alloc_f;
	p->free_f = free_f;

	p->max_num = max_num;
	p->now_num = 0;
	p->obj_size = object_size;
	return edk_SUCCEEDED;
}																			

///dkcFreeSameObjectPool()���ŌĂ΂�Ă��� DKC_SAME_OBJECT_POOL �\���̂̎g�p��̌�n���֐�
DKC_INLINE void WINAPI dkcSameObjectPoolUninit(DKC_SAME_OBJECT_POOL *p)
{
	DKC_SAME_OBJECT_POOL_NODE *t;		
	for(;p->root != NULL;){
		t = p->root->next;
		p->free_f(p->root);
		p->root = t;
	}
}

DKC_SAME_OBJECT_POOL *WINAPI 
	dkcAllocSameObjectPool(size_t object_size,size_t max_num,
												DKC_ALLOC_FUNC_TYPE alloc_f,DKC_FREE_FUNC_TYPE free_f)
{
	DKC_SAME_OBJECT_POOL *p = alloc_sameobjectpool();
	if(NULL==p) return NULL;

	if(DKUTIL_FAILED(dkcSameObjectPoolInit(p,object_size,max_num,alloc_f,free_f)))
	{
		goto Error;
	}
	if(FALSE==dkcSameObjectPoolReserve(p)){
		goto Error;
	}
	return p;
Error:
	free_sameobjectpool(p);
	return NULL;
}


DKC_INLINE DKC_SAME_OBJECT_POOL *WINAPI dkcAllocSameObjectPoolDynamic(size_t object_size){
	return dkcAllocSameObjectPool(object_size,256,NULL,NULL);
}

DKC_SAME_OBJECT_POOL *WINAPI dkcAllocSameObjectPoolAuto(size_t object_size){
	return dkcAllocSameObjectPool(object_size,256,NULL,NULL);
}




int WINAPI dkcFreeSameObjectPool(DKC_SAME_OBJECT_POOL **pp){
	DKC_SAME_OBJECT_POOL *p = (*pp);
	if(NULL==pp || NULL==p){
		return edk_FAILED;
	}
	//�m�[�h���J��
	dkcSameObjectPoolUninit(p);


	//�ŏI�̈���J��
	return free_sameobjectpool(p);
}


DKC_INLINE BOOL WINAPI dkcSameObjectPoolReserveFast(DKC_SAME_OBJECT_POOL *p)
{
	size_t max = p->max_num,now = p->now_num ;
	size_t size = (p->obj_size > sizeof(DKC_SAME_OBJECT_POOL_NODE *))
		? p->obj_size  : sizeof(DKC_SAME_OBJECT_POOL_NODE *);
	DKC_SAME_OBJECT_POOL_NODE* runner;
	size_t i;

	if(max <= now){
		return FALSE;
	}

	if(!p->root){
		runner = dkcAllocateFast(size);
		//runner = p->alloc_f(size);
		runner->next = NULL;
		p->root = runner;
		dkcmNOT_ASSERT(0!=now);
		now++;
		for (i = now;i<max ; i++) {
			runner->next = dkcAllocateFast(size);
			//runner->next = p->alloc_f(size);
			runner = runner->next;
			now++;
		}

	}else{

		dkcmNOT_ASSERT(max < now);
		runner = p->tail;
		max -= now;
		for(i = 0;i<max;i++){
			runner->next = dkcAllocateFast(size);
			//runner->next = p->alloc_f(size);
			runner = runner->next;
			now++;
		}
		
	}
	runner->next = NULL;
	p->tail = runner;
	p->now_num = now;
	return TRUE;
}


///@return dkcAllocSameObjectPool()��object_size�Ŏw�肵���T�C�Y�̃������̈�
DKC_INLINE void *dkcSameObjectPoolAlloc(DKC_SAME_OBJECT_POOL *p){
	DKC_SAME_OBJECT_POOL_NODE *n = p->root;
	int tr;
	
	if(0==p->now_num){
		dkcmNOT_ASSERT(p->root != NULL);
		if(p->alloc_f == dkcMallocAdapter){
			tr = dkcSameObjectPoolReserveFast(p);
		}else{
			tr = dkcSameObjectPoolReserve(p);
		}
		if(FALSE==tr){
			return NULL;
		}
		return dkcSameObjectPoolAlloc(p);
	}


	p->root = n->next;

	//�}�C�i�X
	p->now_num--;

	return n;
}

/*
void free(void * const chunk)
{      
	nextof(chunk) = first;

  first = chunk;
}
// for the sake of code readability :)
static void * & nextof(void * const ptr)
{ 
	return *(static_cast<void **>(ptr)); 
}
*/
// for the sake of code readability :)

DKC_INLINE void WINAPI dkcSameObjectPoolRecycle(DKC_SAME_OBJECT_POOL *p,void *pv){
	/*
	DKC_SAME_OBJECT_POOL_NODE* head = pv;
	head->next = p->root;
	p->root = head;
	p->now_num ++;
	*/
	*((DKC_SAME_OBJECT_POOL_NODE **)pv) = (DKC_SAME_OBJECT_POOL_NODE *)p->root;
	p->root = (DKC_SAME_OBJECT_POOL_NODE *)pv;
	p->now_num++;

}


DKC_INLINE BOOL WINAPI dkcSameObjectPoolReserve(DKC_SAME_OBJECT_POOL *p){
	size_t max = p->max_num,now = p->now_num ;
	size_t size = (p->obj_size > sizeof(DKC_SAME_OBJECT_POOL_NODE *))
		? p->obj_size  : sizeof(DKC_SAME_OBJECT_POOL_NODE *);
	DKC_SAME_OBJECT_POOL_NODE* runner;
	size_t i;

	if(max <= now){
		return FALSE;
	}

	if(!p->root){
		//runner = dkcAllocateFast(size);
		runner = p->alloc_f(size);
		runner->next = NULL;
		p->root = runner;
		dkcmNOT_ASSERT(0!=now);
		now++;
		for (i = now;i<max ; i++) {
			//runner->next = dkcAllocateFast(size);
			runner->next = p->alloc_f(size);
			runner = runner->next;
			now++;
		}

	}else{

		dkcmNOT_ASSERT(max < now);
		runner = p->tail;
		max -= now;
		for(i = 0;i<max;i++){
			//runner->next = dkcAllocateFast(size);
			runner->next = p->alloc_f(size);
			runner = runner->next;
			now++;
		}
		
	}
	runner->next = NULL;
	p->tail = runner;
	p->now_num = now;
	return TRUE;
}


#if 0

#endif

#if 0
DKC_MEMORY_POOL *WINAPI dkcAllocMemoryPool(size_t poolsize,UINT flag){
	DKC_MEMORY_POOL *p = dkcAllocate(sizeof(DKC_MEMORY_POOL));
	void *a = NULL;

	if(NULL==p){
		return NULL;
	}
	a = dkcAllocate(size);
	if(NULL==a){
		goto Error;
	}
	
	p->mpool = a;



	p->mAobo = dkcAllocArrayOneByOneDynamic(sizeof(void *),256);
	if(NULL==p->mAobo){
		goto Error;
	}
	return p;

Error:
	dkcFree(&a);
	dkcFree(&p);

	return NULL;

}

int WINAPI dkcFreeMemoryPool(DKC_MEMORY_POOL **p){
	if(NULL==p)
		return edk_FAILED;

	dkcFreeArrayOneByOne(&((*p)->mAobo));
	dkcFree(&((*p)->mpool));
	return dkcFree(p);
}


void *WINAPI dkcMemoryPoolAlloc(size_t size){
	


}

int WINAPI dkcMemoryPoolFree(void *p){

}

int WINAPI dkcMemoryPoolSafeFree(void **pp){

}
#endif
