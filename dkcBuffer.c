
/*!
@file dkcBuffer.c
@author d金魚
@since 2004/3/xx
*/
#define DKUTIL_C_BUFFER_C
#include "dkcBuffer.h"



///バッファ領域を得る。
DKC_BUFFER* WINAPI dkcAllocBuffer(const void *data,size_t size){
	DKC_BUFFER *p;
	
	if(0==size) return NULL;

	p = (DKC_BUFFER *)dkcAllocate(sizeof(DKC_BUFFER));
	if(NULL==p) return NULL;
	

	p->mBuff = (BYTE *)dkcAllocate(size);
	if(NULL==p->mBuff) goto Error;
	
	p->mSize = size;

	if(data){
		if(DKUTIL_FAILED(dkcBufferSet(p,data,size))){
			goto Error;
		}
	}else{
		
	}
	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreeBuffer(DKC_BUFFER **ptr){
	if(NULL==ptr || NULL==*ptr) return edk_ArgumentException;
	dkcFree((void **)&(*ptr)->mBuff);
	dkcFree((void **)ptr);
	return edk_SUCCEEDED;
}



int WINAPI dkcBufferSet(DKC_BUFFER *ptr,const void *data,size_t size)
{
	return dkcBufferSet_INL(ptr,data,size);
}


int WINAPI dkcBufferSetOffset(DKC_BUFFER *ptr,
															const void *data,size_t size,size_t offset){
	return dkcBufferSetOffset_INL(ptr,data,size,offset);
}




int WINAPI dkcBufferGet(DKC_BUFFER *ptr,void *data,size_t size){
	return dkcBufferGet_INL(ptr,data,size);
}

int WINAPI dkcBufferGetOffset(DKC_BUFFER *ptr,
															void *data,size_t size,size_t offset){
	return dkcBufferGetOffset_INL(ptr,data,size,offset);
}

int WINAPI dkcBufferResize(DKC_BUFFER *ptr,size_t size)
{
	return dkcBufferResize_INL(ptr,size);
}



DKC_BUFFER* WINAPI dkcAllocBufferCopy(const DKC_BUFFER *ptr){
	return dkcAllocBufferCopy_INL(ptr);
}

size_t WINAPI dkcBufferSize(DKC_BUFFER *p){
	return dkcBufferSize_INL(p);
}
BYTE *WINAPI dkcBufferPointer(DKC_BUFFER *p){
	return dkcBufferPointer_INL(p);
}