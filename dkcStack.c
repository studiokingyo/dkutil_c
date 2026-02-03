
/*!
@file dkcStack.c
@author d金魚
@since 2004/3/xx
*/
#define DKUTIL_C_STACK_C
#include "dkcStack.h"
#include "dkcStdio.h"


DKC_STACK * WINAPI dkcAllocStack(size_t numof__,size_t offsetof__){
	DKC_STACK *p;
	size_t size = numof__ * offsetof__;
	if(0==size) return NULL;
	p = (DKC_STACK *)dkcAllocate(sizeof(DKC_STACK));
	if(NULL==p) return NULL;
	p->mBuffer = (BYTE *)dkcAllocate(size);
	
	if(NULL==p->mBuffer) goto Error;

	p->mCount = 0;
	p->mSize = size;
	p->mOffsetOf = offsetof__;

	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreeStack(DKC_STACK **ptr){
	if(NULL==ptr || *ptr==NULL || NULL==(*ptr)->mBuffer)
		return edk_ArgumentException;

	dkcFree((void **)&((*ptr)->mBuffer));
	return dkcFree((void **)ptr);
}

void WINAPI dkcStackPop(DKC_STACK *ptr){
	dkcmNOT_ASSERT(NULL==ptr);
	
	//もう無い判定
	if(ptr->mCount <= 0){
		return ;
	}

	ptr->mCount --;

	return;
}

int WINAPI dkcStackTop(DKC_STACK *ptr,void *get_data)
{

	BYTE *tp;
	size_t point;
	dkcmNOT_ASSERT(NULL==ptr);
	
	point = ptr->mOffsetOf * (ptr->mCount-1);//mCountの-1の所のアドレスに存在するデータだから･･･。

	//もう無い判定
	if(ptr->mCount <= 0){
		return edk_FAILED;
	}


	tp = ptr->mBuffer;
	
	memcpy(get_data,&tp[point],ptr->mOffsetOf);

	//ptr->mCount --;

	return edk_SUCCEEDED;

}

int WINAPI dkcStackPush(DKC_STACK *ptr,const void *data)
{
	BYTE *tp;
	size_t point;
	dkcmNOT_ASSERT(NULL==ptr);

	point = ptr->mCount * ptr->mOffsetOf;
	//もう限界判定
	if(ptr->mSize <= point ){
		return edk_FAILED;
	}
	tp = ptr->mBuffer;

	memcpy(&tp[point],data,ptr->mOffsetOf);

	ptr->mCount ++;

	return edk_SUCCEEDED;

}

int WINAPI dkcStackDynamicPush(DKC_STACK *ptr,const void *data)
{
	BYTE *tp;
	size_t point;

	void *NewPtr;
	size_t want_size;
	dkcmNOT_ASSERT(NULL==ptr);

	point = ptr->mCount * ptr->mOffsetOf;

	//もう限界判定
	if(ptr->mSize <= point ){
		//メモリ再確保
		want_size = dkcReallocateSizeFunction(ptr->mSize,ptr->mOffsetOf);
		if(DKUTIL_FAILED(
			dkcReallocate(&NewPtr,want_size,(void **)&ptr->mBuffer)
			)){
			return edk_FAILED;
		}
		ptr->mBuffer = (BYTE *)NewPtr;
		ptr->mSize = want_size;

	}
	tp = ptr->mBuffer;

	memcpy(&tp[point],data,ptr->mOffsetOf);

	ptr->mCount ++;

	return edk_SUCCEEDED;


}
void WINAPI dkcStackClear(DKC_STACK *ptr){
	//memset(ptr->mBuffer,0,ptr->mSize);
	ptr->mCount = 0;
}

size_t WINAPI dkcStackSize(DKC_STACK *ptr){
	return ptr->mCount;
}

BOOL WINAPI dkcStackIsEmpty(DKC_STACK *ptr){
	return (dkcStackSize(ptr)==0);
}

DKC_INLINE size_t WINAPI dkcStackCapacity(DKC_STACK *ptr){
	return (ptr->mSize / ptr->mOffsetOf);
}

DKC_INLINE size_t WINAPI dkcStackRestSize(DKC_STACK *ptr){
	return dkcStackCapacity(ptr) - dkcStackSize(ptr);
}
/*
int WINAPI dkcStackSerialize(const DKC_STACK *ptr,DKC_SERIALIZE *se)
{
	int id = edkcSerializeIDStack;
	dkcmNOT_ASSERT(NULL==ptr);
	dkcSerializeWrite(se,&id,sizeof(id));
	dkcSerializeWrite(se,ptr,sizeof(DKC_STACK));
	return dkcSerializeWrite(se,ptr->mBuffer,ptr->mSize);
}

DKC_STACK* WINAPI dkcAllocStackDeserialize(DKC_DESERIALIZE *se)
{
	DKC_STACK *p;
	DKC_STACK t;
	size_t read;
	int id;
	
	dkcDeserializeRead(se,&id,sizeof(id),&read);
	if(id !=  edkcSerializeIDStack)
	{
		return NULL;
	}
	dkcDeserializeRead(se,&t,sizeof(t),&read);
	
	p = dkcAllocStack(t.mSize,t.mOffsetOf);
	if(NULL==p) return NULL;


	dkcDeserializeRead(se,p->mBuffer,p->mSize,&read);
	p->mCount = t.mCount;
	p->mOffsetOf = t.mOffsetOf;


	dkcmNOT_ASSERT(read != p->mSize);
	
	return p;
}
*/
DKC_INLINE void *WINAPI dkcStackPointer(DKC_STACK *ptr){
	return ptr->mBuffer;
}


/*
DKC_STACK * WINAPI dkcAllocStack(size_t numof__,size_t offsetof__){
	DKC_STACK *p;
	size_t size = numof__ * offsetof__;
	if(0==size) return NULL;
	p = dkcAllocate(sizeof(DKC_STACK));
	if(NULL==p) return NULL;
	p->mBuffer = dkcAllocate(size);
	
	if(NULL==p->mBuffer) goto Error;

	p->mCount = 0;
	p->mSize = size;
	p->mOffsetOf = offsetof__;

	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreeStack(DKC_STACK **ptr){
	if(NULL==ptr || *ptr==NULL || NULL==(*ptr)->mBuffer)
		return edk_ArgumentException;

	dkcFree((void **)&((*ptr)->mBuffer));
	return dkcFree((void **)ptr);
}

int WINAPI dkcPopStack(DKC_STACK *ptr,void *get_data){
	BYTE *tp;
	dkcmNOT_ASSERT(NULL==ptr);
	
	//もう無い判定
	if(ptr->mOffsetOf > ptr->mCount){
		return edk_FAILED;
		}

	tp = ptr->mBuffer;
	
	memcpy(get_data,&tp[ptr->mCount - ptr->mOffsetOf],ptr->mOffsetOf);

	ptr->mCount -= ptr->mOffsetOf;

	return edk_SUCCEEDED;
}

int WINAPI dkcPushStack(DKC_STACK *ptr,const void *data)
{
	BYTE *tp;
	dkcmNOT_ASSERT(NULL==ptr);
	//もう限界判定
	if(ptr->mSize < ptr->mCount + ptr->mOffsetOf){
		return edk_FAILED;
	}
	tp = ptr->mBuffer;

	memcpy(&tp[ptr->mCount],data,ptr->mOffsetOf);

	ptr->mCount += ptr->mOffsetOf;

	return edk_SUCCEEDED;

}

void WINAPI dkcStackClear(DKC_STACK *ptr){
	//memset(ptr->mBuffer,0,ptr->mSize);
	ptr->mCount = 0;
}
*/