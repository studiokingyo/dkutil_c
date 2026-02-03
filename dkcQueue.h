/*!
@file dkcQueue.h
@author d金魚
@since 2004/3/xx
@brief キュー Queue
*/
#ifndef DKUTIL_C_QUEUE_H
#define DKUTIL_C_QUEUE_H

#include "dkcOSIndependent.h"


/*!
キュー構造体
*/
typedef struct dkc_Queue{
	///バッファ。
	BYTE *mBuffer;
	///バッファのサイズ
	size_t mSize;
	///入り口と出口のオフセット
	size_t mEntrance,mExit;
	///データのオフセット
	size_t mOffsetOf;
	///カウンタ
	size_t mCounter;
}DKC_QUEUE;



/*!
@param numof__[in] キューに使う配列の要素の数
@param offsetof__[in] キューにぶち込むデータのサイズ
@return キューハンドルへのポインタ。
*/
///キュー領域を得る。
DKC_EXTERN DKC_QUEUE* WINAPI dkcAllocQueue(size_t numof__,size_t offsetof__);
/*!
DKC_QUEUEをデリート
@note
必ず使用したあとはこれを呼んでください。
*/
///dkcAllocNewQueueと対。
DKC_EXTERN int WINAPI dkcFreeQueue(DKC_QUEUE **ptr);


/*!
@note

*/
DKC_EXTERN int WINAPI dkcQueuePush(DKC_QUEUE *ptr,const void *);

DKC_EXTERN int WINAPI dkcQueueDynamicPush(DKC_QUEUE *ptr,const void *);
/*!
@param id[in] 同じidを二回解放するとバグリます。
*/
DKC_EXTERN void WINAPI dkcQueuePop(DKC_QUEUE *ptr);

DKC_EXTERN int WINAPI dkcQueueTop(DKC_QUEUE *ptr,void *);


DKC_EXTERN size_t WINAPI dkcQueueSize(DKC_QUEUE*ptr);

DKC_EXTERN BOOL WINAPI dkcQueueIsEmpty(DKC_QUEUE *ptr);
/*!
キューをクリアする。
*/
DKC_EXTERN void WINAPI dkcQueueClear(DKC_QUEUE *ptr);

//DKC_EXTERN int WINAPI dkcQueueSerialize(const DKC_QUEUE *ptr,DKC_SERIALIZE *se);

//DKC_EXTERN DKC_QUEUE* WINAPI dkcAllocQueueDeserialize(DKC_DESERIALIZE *se);

/*!
@param ptr[in] queue
@param point_of__[in] ポイント
@param buffer[in] バッファへのポインタ
@param buffsize[in] バッファのサイズ
*/
DKC_EXTERN int WINAPI dkcQueueGetPoint(const DKC_QUEUE *ptr,size_t point_of__,void *buffer,size_t buffsize);

/*
#ifndef DKUTIL_C_QUEUE_C
#	include "dkcQueue.c"
#endif
*/
#endif //end of include once