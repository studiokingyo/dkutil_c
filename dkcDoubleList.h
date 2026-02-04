/*!
@file dkcDoubleList.h
@brief 双方向連結リスト (Doubly Linked List)
@note
dkcSingleList.h の双方向版。
mPrev ポインタにより逆方向走査が O(1) で可能。
InsertBefore による前方挿入もサポート。
*/

#ifndef DKUTIL_C_DOUBLE_LIST_H
#define DKUTIL_C_DOUBLE_LIST_H

#include "dkcOSIndependent.h"
#include "dkcBuffer.h"

/*!
双方向連結リスト構造体
*/
typedef struct dkc_DoubleList{
	BYTE *mBuff;
	size_t mSize;
	struct dkc_DoubleList *mNext;
	struct dkc_DoubleList *mPrev;
}DKC_DOUBLELIST;

typedef struct dkc_DoubleListObject{
	DKC_DOUBLELIST *mBegin;
	DKC_DOUBLELIST *mTail;
	size_t mCount;
	DKC_DOUBLELIST* (*next)(const DKC_DOUBLELIST *);
	DKC_DOUBLELIST* (*prev)(const DKC_DOUBLELIST *);
	DKC_DOUBLELIST* (*begin)(struct dkc_DoubleListObject *);
	DKC_DOUBLELIST* (*tail)(struct dkc_DoubleListObject *);
	BOOL (*end)(const DKC_DOUBLELIST *);
	BOOL (*push_back)(struct dkc_DoubleListObject *,const void *data,size_t size);
	BOOL (*push_front)(struct dkc_DoubleListObject *,const void *data,size_t size);
	void (*erase)(struct dkc_DoubleListObject *p,DKC_DOUBLELIST *target);
	int (WINAPI *setbuffer)(DKC_DOUBLELIST *ptr,const void *data,size_t size);
	int (WINAPI *getbuffer)(DKC_DOUBLELIST *ptr,void *data,size_t size);
	BOOL (*empty)(struct dkc_DoubleListObject *);
	size_t (*size)(struct dkc_DoubleListObject *);
}DKC_DOUBLELIST_OBJECT;

/* node level API */

DKC_EXTERN DKC_DOUBLELIST * WINAPI dkcAllocDoubleList(const void *data,size_t size);

DKC_EXTERN int WINAPI dkcFreeDoubleList(DKC_DOUBLELIST **ptr);

DKC_EXTERN int WINAPI dkcDoubleListInsertAfter(DKC_DOUBLELIST *ptr,const void *data,size_t size);

DKC_EXTERN int WINAPI dkcDoubleListInsertBefore(DKC_DOUBLELIST *ptr,const void *data,size_t size);

DKC_EXTERN DKC_DOUBLELIST * WINAPI dkcDoubleListErase(DKC_DOUBLELIST *ptr);

DKC_EXTERN size_t WINAPI dkcDoubleListSize(DKC_DOUBLELIST *top);

DKC_EXTERN int WINAPI dkcDoubleListGetBuffer(DKC_DOUBLELIST *ptr,void *data,size_t size);

DKC_EXTERN int WINAPI dkcDoubleListSetBuffer(DKC_DOUBLELIST *ptr,const void *data,size_t size);

DKC_EXTERN DKC_BUFFER* WINAPI dkcAllocDoubleListBuffer(const DKC_DOUBLELIST *ptr);

#define dkcFreeDoubleListBuffer(p) dkcFreeBuffer(p)

/* object level API */

DKC_EXTERN DKC_DOUBLELIST_OBJECT * WINAPI dkcAllocDoubleListObject(const void *data,size_t size);

DKC_EXTERN int WINAPI dkcFreeDoubleListObject(DKC_DOUBLELIST_OBJECT **ptr);

#endif /* DKUTIL_C_DOUBLE_LIST_H */
