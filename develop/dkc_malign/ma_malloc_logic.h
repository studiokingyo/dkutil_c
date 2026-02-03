#ifndef DKC_MALIGN_MALLOC_LOGIC_H
#define DKC_MALIGN_MALLOC_LOGIC_H


#include <dkutil_c/dkc_misc.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdarg.h>

typedef void *
typedef struct{
	DKC_MALLOC_F_TYPE mMalloc;
	DKC_FREE_F_TYPE mFree;
	size_t mAlign;

	//for red black tree
	void *mTreeRoot;
	void *mTreeSentinel;

}DKC_MA_MALLOC_STATE;

DKC_EXTERN DKC_MA_MALLOC_STATE *
	dkcAllocMallocLogic(size_t align_v);

DKC_EXTERN int dkcFreeMallocLogic(DKC_MA_MALLOC_STATE **pp);


DKC_INLINE void *dkcMallocLogicAllocate_INL(DKC_MA_MALLOC_STATE *p,size_t size)
{
	return p->mMalloc(size);
}

DKC_INLINE void dkcMallocLogicFree_INL(DKC_MA_MALLOC_STATE *p,void *ptr){
	p->mFree(ptr);
}

DKC_INLINE void dkcMallocLogicSetAlign(DKC_MA_MALLOC_STATE *p,size_t align_v)
{
	p->mAlign = align_v;
}

///Žg—p‚ð§—ã‚µ‚È‚¢B
DKC_INLINE void dkcMallocLogicSetFunction(DKC_MA_MALLOC_STATE *p,DKC_MALLOC_F_TYPE alloc_v,DKC_FREE_F_TYPE free_v)
{

	p->mMalloc = alloc_v;
	p->mFree = free_v;

}

DKC_EXTERN void *dkcMallocLogicAllocate(DKC_MA_MALLOC_STATE *p,size_t size);

DKC_EXTERN void dkcMallocLogicFree(DKC_MA_MALLOC_STATE *p,void *);


DKC_EXTERN int dkcMallocLogicAllFree(DKC_MA_MALLOC_STATE *p);



#endif
