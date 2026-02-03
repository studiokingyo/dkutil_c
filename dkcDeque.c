
/*!
@file dkcDeque.c
@author d金魚
@
*/
#define DKUTIL_C_DEQUE_C
#include "dkcDeque.h"
#include "dkcStdio.h"



DKC_DEQUE* WINAPI dkcAllocDeque(size_t numof__,size_t offsetof__)
{
	DKC_DEQUE *p;
	size_t size = numof__ * offsetof__;

	p = dkcAllocate(sizeof(DKC_DEQUE));
	if(NULL==p) return NULL;

	p->mBuffer = dkcAllocate(size);
	if(NULL==p->mBuffer) goto Error;

	/* dkcAllocateでmemset(0)なのでヽ(ﾟдﾟ)ノ　ええじゃないか
	p->mCount = 0;
	p->mEnd = 0;
	p->mStart = 0;
	*/
	p->mSize = size;
	p->mOffsetOf = offsetof__;
Error:
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreeDeque(DKC_DEQUE **ptr){
	if(NULL==ptr || NULL == (*ptr)) return edk_ArgumentException;
	dkcFree((void **)&(*ptr)->mBuffer);
	return dkcFree((void **)ptr);
}
#if 0


int WINAPI dkcDequePushBack(DKC_DEQUE *ptr,const void *data)
{
	BYTE *tb;
	dkcmNOT_ASSERT(NULL==ptr);

	if(ptr->mCount >= ptr->mSize / ptr->mOffsetOf){
		return edk_FAILED;
	}

	tb = ptr->mBuffer;

	memcpy(&tb[ptr->mEnd],data,ptr->mOffsetOf);


	ptr->mEnd += ptr->mOffsetOf;#error ここはmEndのlength checkが必要
	ptr->mCount++;


}
#endif
/*!
@param id[in] 同じidを二回解放するとバグリます。
*/
int WINAPI dkcDequePopBack(DKC_DEQUE *ptr,void *);

int WINAPI dkcDequePushFront(DKC_DEQUE *ptr,const void *);
/*!

*/
int WINAPI dkcDequePopFront(DKC_DEQUE *ptr,void *);
/*!
キューをクリアする。
*/
void WINAPI dkcDequeClear(DKC_DEQUE *ptr);

//int WINAPI dkcDequeSerialize(const DKC_DEQUE *ptr,DKC_SERIALIZE *se);

//DKC_DEQUE* WINAPI dkcAllocQueueDeserialize(DKC_DESERIALIZE *se);

/*!
@param ptr[in] queue
@param point_of__[in] ポイント
@param buffer[in] バッファへのポインタ
@param buffsize[in] バッファのサイズ
*/
int WINAPI dkcDequeGetPoint(const DKC_DEQUE *ptr,size_t point_of__,void *buffer,size_t buffsize);



