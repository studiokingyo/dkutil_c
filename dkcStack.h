/*
@file dkcStack.h
@author d金魚
@since 2004/3/xx
@brief スタック構造
*/


#ifndef DKUTIL_C_STACK_H
#define DKUTIL_C_STACK_H



#include "dkcOSIndependent.h"

/*
typedef struct dkc_Stack{
	///バッファ。
	BYTE *mBuffer;
	///バッファのサイズ
	size_t mSize;
	///後尾
	size_t mRear;
	///データのオフセット
	size_t mOffsetOf;
}DKC_STACK;
*/
/*!
スタック構造体
*/
typedef struct dkc_Stack{
	///バッファ。
	BYTE *mBuffer;
	///バッファのサイズ
	size_t mSize;
	///カウント
	size_t mCount;
	///データのオフセット
	size_t mOffsetOf;
}DKC_STACK;




/*!
@param numof__[in] スタックに使う配列の要素の数
@param offsetof__[in] スタックにぶち込むデータのサイズ
@return スタックハンドルへのポインタ。
*/
DKC_EXTERN DKC_STACK * WINAPI dkcAllocStack(size_t numoff__,size_t offsetof__);
/*!
@note
必ず使用したあとはこれを呼んでください。
dkcAllocStackと対。
*/

DKC_EXTERN int WINAPI dkcFreeStack(DKC_STACK **ptr);


/**

*/
///スタックをポップする
DKC_EXTERN void WINAPI dkcStackPop(DKC_STACK *ptr);
/*!*/
DKC_EXTERN int WINAPI dkcStackTop(DKC_STACK *ptr,void *);
///スタックに入っている要素数を返す
DKC_EXTERN size_t WINAPI dkcStackSize(DKC_STACK *ptr);
///@return スタックが空だったらTRUE
DKC_EXTERN BOOL WINAPI dkcStackIsEmpty(DKC_STACK *ptr);
/*!
\
*/
///スタックにデータを詰めこむ
DKC_EXTERN int WINAPI dkcStackPush(DKC_STACK *ptr,const void *);
///スタックにデータを詰めこむ（動的拡張版）
DKC_EXTERN int WINAPI dkcStackDynamicPush(DKC_STACK *ptr,const void *);

///スタックのポインタを得る。
DKC_EXTERN void *WINAPI dkcStackPointer(DKC_STACK *ptr);
/*!
Stack内のカウンタを０に戻して、スタックを事実上クリアする。
*/
DKC_EXTERN void WINAPI dkcStackClear(DKC_STACK *ptr);
/// 許容量(すでに確保された領域の大きさ)
DKC_EXTERN size_t WINAPI dkcStackCapacity(DKC_STACK *ptr);
///残り領域
DKC_EXTERN size_t WINAPI dkcStackRestSize(DKC_STACK *ptr);



#if !defined( DKUTIL_C_STACK_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcStack.c"
#endif

#endif //end of include once