
/*!
@file dkcUniqueID.c
@author d金魚
@
*/

#include "dkcUniqueID.h"
#include "dkcStdio.h"



//シングルリスト領域を得る。
DKC_UNIQUE_ID * WINAPI dkcAllocUniqueID(void){
	DKC_UNIQUE_ID *p;
	p = dkcAllocate(sizeof(DKC_UNIQUE_ID));
	if(NULL==p) return NULL;

	p->mStream = dkcAllocMemoryStream(sizeof(int) * 100);
	if(NULL==p->mStream) goto Error;

	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreeUniqueID(DKC_UNIQUE_ID **ptr){
	if(NULL==ptr || *ptr==NULL || NULL==(*ptr)->mStream)
		return edk_ArgumentException;

	dkcFreeMemoryStream(&((*ptr)->mStream));
	return dkcFree((void **)ptr);
}
