/*!
@file dkcDeque.h
@brief std::deque見たいな物を目指そうとしたが、すでに作る気梨･･･。どちら様かソースを寄付していただけないでしょうか？
*/
#ifndef DKUTIL_C_DEQUE_H
#define DKUTIL_C_DEQUE_H

#include "dkcOSIndependent.h"


/*!
double ended queue構造体
*/
typedef struct dkc_Deque{
	///デックバッファ
	BYTE *mBuffer;
	///デックサイズ
	size_t mSize;
	///データのオフセット
	size_t mOffsetOf;
	///デックの先端と終端。
	size_t mStart,mEnd;
	///内部に入っている数。
	size_t mCount;
}DKC_DEQUE;


/*!
@param numof__[in] double ended queueに使う配列の要素の数
@param offsetof__[in] double ended queueにぶち込むデータのサイズ
@return double ended queueハンドルへのポインタ。
*/
///キュー領域を得る。
DKC_EXTERN DKC_DEQUE* WINAPI dkcAllocDeque(size_t numof__,size_t offsetof__);
/*!
DKC_DEQUEをデリート
*/
DKC_EXTERN int WINAPI dkcFreeDeque(DKC_DEQUE **ptr);


/*!
@note

*/
DKC_EXTERN int WINAPI dkcDequePushBack(DKC_DEQUE *ptr,const void *);
/*!
@param id[in] 同じidを二回解放するとバグリます。
*/
DKC_EXTERN int WINAPI dkcDequePopBack(DKC_DEQUE *ptr,void *);

DKC_EXTERN int WINAPI dkcDequePushFront(DKC_DEQUE *ptr,const void *);
/*!

*/
DKC_EXTERN int WINAPI dkcDequePopFront(DKC_DEQUE *ptr,void *);
/*!
キューをクリアする。
*/
DKC_EXTERN void WINAPI dkcDequeClear(DKC_DEQUE *ptr);


/*!
@param ptr[in] queue
@param point_of__[in] ポイント
@param buffer[in] バッファへのポインタ
@param buffsize[in] バッファのサイズ
*/
DKC_EXTERN int WINAPI dkcDequeGetPoint(const DKC_DEQUE *ptr,size_t point_of__,void *buffer,size_t buffsize);




#endif //end of include once