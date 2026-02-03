
/*!
@file dkcQueue.c
@author d金魚
@since 2004/3/xx
*/
#define DKUTIL_C_QUEUE_C
#include "dkcQueue.h"
#include "dkcStdio.h"


DKC_QUEUE* WINAPI dkcAllocQueue(size_t numof__,size_t offsetof__){
	DKC_QUEUE *p;
	size_t size = (numof__) * offsetof__;
	if(0==size) return NULL;
	p = (DKC_QUEUE *)dkcAllocate(sizeof(DKC_QUEUE));
	if(NULL==p) return NULL;
	p->mBuffer = (BYTE *)dkcAllocate(size);
	
	if(NULL==p->mBuffer) goto Error;

	p->mExit = 0;
	p->mEntrance = 0;
	p->mSize = size;
	p->mOffsetOf = offsetof__;
	p->mCounter = 0;

	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}
int WINAPI dkcFreeQueue(DKC_QUEUE **ptr){
	if(NULL==ptr || *ptr==NULL || NULL==(*ptr)->mBuffer)
		return edk_ArgumentException;

	dkcFree((void **)&((*ptr)->mBuffer));
	return dkcFree((void **)ptr);
}


int WINAPI dkcQueuePush(DKC_QUEUE *ptr,const void *data){
	BYTE *tp;
	dkcmNOT_ASSERT(NULL==ptr);

	tp = ptr->mBuffer;
	if(/*ptr->mEntrance <= ptr->mExit &&*/ 
		ptr->mCounter >= ptr->mSize / ptr->mOffsetOf)
	{//MAX...
		return edk_FAILED;
	}
	else if(ptr->mEntrance >= ptr->mSize)
	{
		ptr->mEntrance = 0;
	}
	/*if(ptr->mSize < ptr->mOffsetOf + ptr->mEntrance)
	{//入り口が満杯	
		if(ptr->mExit + ptr->mOffsetOf >= ptr->mSize)
		{	//出口も入り口と一緒･･･。
			//と言う事は消せるね。
			dkcQueueClear(ptr);
		}
		else if(ptr->mExit > ptr->mOffsetOf)
		{//前まで出口だった使用していない領域が使えるかも？
			if(ptr->mEntrance < ptr->mExit)
			{//出口の方がオフセットが上だった
				//つまり、すでに**----*** こんな感じになっている。
				ptr->mEntrance += ptr->mOffsetOf;
			}else{//下だった
				//こんな感じ：-----**
				ptr->mEntrance = 0;
			}
		}else
		{//ダメだ完全満席
			return edk_FAILED;
		}
	}else if(ptr->mEntrance < ptr->mExit && 
		ptr->mEntrance+ ptr->mOffsetOf > ptr->mExit)
	{
		return edk_FAILED;
	}*/
	
	memcpy(&tp[ptr->mEntrance],data,ptr->mOffsetOf);
	
	ptr->mEntrance += ptr->mOffsetOf;
	ptr->mCounter++;

	return edk_SUCCEEDED;
}


int WINAPI dkcQueueDynamicPush(DKC_QUEUE *ptr,const void *data)
{
	int result;
	void *NewPtr;
	size_t want_size;

	result = dkcQueuePush(ptr,data);

	if(DKUTIL_SUCCEEDED(result)) return result;

	//メモリ再確保
	{
		want_size = dkcReallocateSizeFunction(ptr->mSize,ptr->mOffsetOf);
		if(DKUTIL_FAILED(
			dkcReallocate(&NewPtr,want_size,(void **)&ptr->mBuffer)
			)){
			return edk_FAILED;
		}
		ptr->mBuffer =(BYTE *)NewPtr;
		ptr->mSize = want_size;
	}
	//もう一回行って見る。
	return dkcQueuePush(ptr,data);

}
void WINAPI dkcQueuePop(DKC_QUEUE *ptr)
{
	dkcmNOT_ASSERT(NULL==ptr);
	ptr->mExit += ptr->mOffsetOf;
	if(ptr->mCounter <= 0)
	{//もうありませんよ＾＾；
		return;
	}
	ptr->mCounter--;

	if(ptr->mExit >= ptr->mSize)
	{//もう一杯イッパイです。
		ptr->mExit = 0;
	}
	return;
}
int WINAPI dkcQueueTop(DKC_QUEUE *ptr,void *get_data){
	BYTE *tp;
	
	dkcmNOT_ASSERT(NULL==ptr || NULL==get_data);
	
	//データが無い。
	//if(ptr->mEntrance < ptr->mExit + ptr->mOffsetOf){
	if(0==ptr->mCounter){
		return edk_FAILED;
	}
	tp = ptr->mBuffer;

	memcpy(get_data,&tp[ptr->mExit],ptr->mOffsetOf);

	return edk_SUCCEEDED;


}

void WINAPI dkcQueueClear(DKC_QUEUE *ptr){
	if(NULL==ptr) return;

	ptr->mExit = 0;//出口を戻す。
	ptr->mEntrance = 0;//入り口も戻す。
	ptr->mCounter = 0;//カウンタも戻す。
}

size_t WINAPI dkcQueueSize(DKC_QUEUE *ptr){
	return ptr->mCounter;
}

BOOL WINAPI dkcQueueIsEmpty(DKC_QUEUE *ptr){
	return (dkcQueueSize(ptr)==0);
}
/*
int WINAPI dkcQueueSerialize(const DKC_QUEUE *ptr,DKC_SERIALIZE *se)
{
	int id = edkcSerializeIDQueue;
	dkcmNOT_ASSERT(NULL==ptr);
	dkcSerializeWrite(se,&id,sizeof(id));
	dkcSerializeWrite(se,ptr,sizeof(DKC_QUEUE));
	return dkcSerializeWrite(se,ptr->mBuffer,ptr->mSize);
}


DKC_QUEUE* WINAPI dkcAllocQueueDeserialize(DKC_DESERIALIZE *se){
	DKC_QUEUE *p;
	DKC_QUEUE t;
	size_t read;
	int id;
	
	dkcDeserializeRead(se,&id,sizeof(id),&read);
	if(id !=  edkcSerializeIDQueue)
	{
		return NULL;
	}
	dkcDeserializeRead(se,&t,sizeof(t),&read);
	
	p = dkcAllocQueue(t.mSize,t.mOffsetOf);
	if(NULL==p) return NULL;

	p->mEntrance = t.mEntrance;
	p->mExit = t.mExit;
	dkcDeserializeRead(se,p->mBuffer,p->mSize,&read);
	
	dkcmNOT_ASSERT(read != p->mSize);
	
	return p;
}*/