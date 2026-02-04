/*!
@file dkcDoubleList.c
@brief 双方向連結リスト実装
*/
#define DKUTIL_C_DOUBLE_LIST_C
#include "dkcDoubleList.h"
#include "dkcStdio.h"

/* ノード初期化 (内部関数) */
static int WINAPI dkcNewDoubleList(DKC_DOUBLELIST *ptr,const void *data,size_t size)
{
	void *p = NULL;
	if(NULL==ptr) return edk_ArgumentException;
	if(ptr->mBuff) return edk_ArgumentException;

	p = dkcAllocate(size);
	if(NULL==p) return edk_OutOfMemory;

	if(NULL != data && 0 != size){
		if(DKUTIL_FAILED(dkc_memcpy(p,size,data,size))){
			dkcFree(&p);
			return edk_FAILED;
		}
	}

	ptr->mBuff = (BYTE *)p;
	ptr->mSize = size;
	ptr->mNext = NULL;
	ptr->mPrev = NULL;

	return edk_SUCCEEDED;
}

/* 全ノード削除 (内部関数) */
static int WINAPI dkcDeleteDoubleList(DKC_DOUBLELIST *ptr)
{
	DKC_DOUBLELIST *p = NULL;
	if(NULL==ptr) return edk_ArgumentException;
	if(NULL==ptr->mBuff) return edk_ArgumentException;

	for(p = ptr;;){
		p = dkcDoubleListErase(p);
		if(p==NULL) break;
	}
	return edk_SUCCEEDED;
}

DKC_DOUBLELIST * WINAPI dkcAllocDoubleList(const void *data,size_t size)
{
	DKC_DOUBLELIST *p = NULL;
	p = (DKC_DOUBLELIST *)dkcAllocate(sizeof(DKC_DOUBLELIST));
	if(NULL==p) return NULL;
	DKUTIL_MEMZERO(p,sizeof(DKC_DOUBLELIST));
	if(DKUTIL_FAILED(dkcNewDoubleList(p,data,size))){
		dkcFree((void **)&p);
		return NULL;
	}
	return p;
}

int WINAPI dkcFreeDoubleList(DKC_DOUBLELIST **ptr)
{
	if(NULL==ptr) return edk_FAILED;
	if(NULL==*ptr) return edk_FAILED;
	if(DKUTIL_FAILED(dkcDeleteDoubleList(*ptr))){
		return edk_FAILED;
	}
	return edk_SUCCEEDED;
}

int WINAPI dkcDoubleListInsertAfter(DKC_DOUBLELIST *ptr,const void *data,size_t size)
{
	DKC_DOUBLELIST *p;
	DKC_DOUBLELIST *old_next;
	if(NULL==ptr) return edk_ArgumentException;

	p = dkcAllocDoubleList(data,size);
	if(NULL==p) return edk_FAILED;

	old_next = ptr->mNext;
	ptr->mNext = p;
	p->mPrev = ptr;
	p->mNext = old_next;
	if(old_next){
		old_next->mPrev = p;
	}

	return edk_SUCCEEDED;
}

int WINAPI dkcDoubleListInsertBefore(DKC_DOUBLELIST *ptr,const void *data,size_t size)
{
	DKC_DOUBLELIST *p;
	DKC_DOUBLELIST *old_prev;
	if(NULL==ptr) return edk_ArgumentException;

	p = dkcAllocDoubleList(data,size);
	if(NULL==p) return edk_FAILED;

	old_prev = ptr->mPrev;
	ptr->mPrev = p;
	p->mNext = ptr;
	p->mPrev = old_prev;
	if(old_prev){
		old_prev->mNext = p;
	}

	return edk_SUCCEEDED;
}

DKC_DOUBLELIST * WINAPI dkcDoubleListErase(DKC_DOUBLELIST *ptr)
{
	DKC_DOUBLELIST *next_node = NULL;
	if(NULL==ptr) return NULL;

	next_node = ptr->mNext;
	if(ptr->mPrev){
		ptr->mPrev->mNext = ptr->mNext;
	}
	if(ptr->mNext){
		ptr->mNext->mPrev = ptr->mPrev;
	}

	dkcFree((void **)&ptr->mBuff);
	dkcFree((void **)&ptr);
	return next_node;
}

size_t WINAPI dkcDoubleListSize(DKC_DOUBLELIST *top)
{
	DKC_DOUBLELIST *it = top;
	size_t count = 0;
	for(;it != NULL;){
		count++;
		it = it->mNext;
	}
	return count;
}

int WINAPI dkcDoubleListGetBuffer(DKC_DOUBLELIST *ptr,void *data,size_t size)
{
	if(NULL==ptr || NULL==data || 0==size){
		return edk_FAILED;
	}
	return dkc_memcpy(data,size,ptr->mBuff,ptr->mSize);
}

int WINAPI dkcDoubleListSetBuffer(DKC_DOUBLELIST *ptr,const void *data,size_t size)
{
	void *NewPtr;

	if(NULL==ptr || NULL==data || 0==size){
		return edk_FAILED;
	}

	if(ptr->mSize < size){
		if(DKUTIL_FAILED(dkcReallocate(&NewPtr,size,(void **)&(ptr->mBuff)))){
			return edk_FAILED;
		}
		ptr->mBuff = (BYTE *)NewPtr;
		ptr->mSize = size;
	}

	if(DKUTIL_FAILED(dkc_memcpy(ptr->mBuff,ptr->mSize,data,size))){
		return edk_FAILED;
	}

	return edk_SUCCEEDED;
}

DKC_BUFFER* WINAPI dkcAllocDoubleListBuffer(const DKC_DOUBLELIST *ptr)
{
	if(NULL==ptr) return NULL;
	return dkcAllocBuffer(ptr->mBuff,ptr->mSize);
}

/* ========================================
 * Object level implementation
 * ======================================== */

static DKC_DOUBLELIST *dkcDLOBeginLogic(struct dkc_DoubleListObject *p){
	return p->mBegin;
}
static DKC_DOUBLELIST *dkcDLOTailLogic(struct dkc_DoubleListObject *p){
	return p->mTail;
}
static BOOL dkcDLOEndLogic(const DKC_DOUBLELIST *p){
	return (NULL==p);
}
static DKC_DOUBLELIST *dkcDLONextLogic(const DKC_DOUBLELIST *p){
	return p->mNext;
}
static DKC_DOUBLELIST *dkcDLOPrevLogic(const DKC_DOUBLELIST *p){
	return p->mPrev;
}

static BOOL dkcDLOPushBackLogic(struct dkc_DoubleListObject *p,const void *data,size_t size)
{
	BOOL result;
	result = DKUTIL_SUCCEEDED_BOOL(dkcDoubleListInsertAfter(p->mTail,data,size));
	if(TRUE==result){
		p->mTail = p->mTail->mNext;
		p->mCount++;
	}
	return result;
}

static BOOL dkcDLOPushFrontLogic(struct dkc_DoubleListObject *p,const void *data,size_t size)
{
	BOOL result;
	result = DKUTIL_SUCCEEDED_BOOL(dkcDoubleListInsertBefore(p->mBegin,data,size));
	if(TRUE==result){
		p->mBegin = p->mBegin->mPrev;
		p->mCount++;
	}
	return result;
}

static void dkcDLOEraseLogic(struct dkc_DoubleListObject *p,DKC_DOUBLELIST *target)
{
	DKC_DOUBLELIST *next_node;
	if(p->mTail == p->mBegin){
		return;
	}
	if(p->mTail == target){
		p->mTail = target->mPrev;
	}
	if(p->mBegin == target){
		p->mBegin = target->mNext;
	}
	next_node = dkcDoubleListErase(target);
	(void)next_node;
	p->mCount--;
}

static BOOL dkcDLOEmptyLogic(struct dkc_DoubleListObject *p){
	return (p->mBegin == p->mTail);
}

static size_t dkcDLOSizeLogic(struct dkc_DoubleListObject *p){
	return p->mCount;
}

DKC_DOUBLELIST_OBJECT * WINAPI dkcAllocDoubleListObject(const void *data,size_t size)
{
	DKC_DOUBLELIST_OBJECT *p = NULL;

	p = (DKC_DOUBLELIST_OBJECT *)dkcAllocate(sizeof(DKC_DOUBLELIST_OBJECT));
	if(NULL==p) return NULL;

	p->mBegin = dkcAllocDoubleList(data,size);
	if(NULL==p->mBegin){
		dkcFree((void **)&p);
		return NULL;
	}
	p->mTail = p->mBegin;
	p->mCount = 1;

	p->begin = dkcDLOBeginLogic;
	p->tail = dkcDLOTailLogic;
	p->end = dkcDLOEndLogic;
	p->next = dkcDLONextLogic;
	p->prev = dkcDLOPrevLogic;

	p->setbuffer = dkcDoubleListSetBuffer;
	p->getbuffer = dkcDoubleListGetBuffer;
	p->push_back = dkcDLOPushBackLogic;
	p->push_front = dkcDLOPushFrontLogic;
	p->erase = dkcDLOEraseLogic;

	p->empty = dkcDLOEmptyLogic;
	p->size = dkcDLOSizeLogic;

	return p;
}

int WINAPI dkcFreeDoubleListObject(DKC_DOUBLELIST_OBJECT **p)
{
	if(NULL==p || NULL==*p) return edk_ArgumentException;
	dkcFreeDoubleList(&(*p)->mBegin);
	return dkcFree((void **)p);
}
