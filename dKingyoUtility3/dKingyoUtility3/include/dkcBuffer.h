/*!
@file dkcBuffer.h
@author d金魚
@since 2004/3/xx
@brief 超簡易バッファールーチン
@note
基本的に、プログラム側で操作するタイプのバッファー。
エラーを起こさないように注意しよう。
*/

#ifndef DKUTIL_C_BUFFER_H
#define DKUTIL_C_BUFFER_H

#include "dkcOSIndependent.h"
#include "dkcStdio.h"

/*!
バッファー構造体
*/
typedef struct dkc_Buffer{
	///バッファへのポインタ
	BYTE *mBuff;
	///バッファのサイズ
	size_t mSize;
}DKC_BUFFER;




/*!
@param data[in] バッファへコピーしたいデータへのポインタ
@param size[in] バッファのサイズ（dataのサイズ）
*/
///バッファ領域を得る。
DKC_EXTERN DKC_BUFFER* WINAPI dkcAllocBuffer(const void *data,size_t size);
/*!
DKC_BUFFERをデリート
@note
必ず使用したあとはこれを呼んでください。
*/
///dkcAllocBuffer()で確保したリスト領域と内部バッファを削除。dkcAllocBufferと対。
DKC_EXTERN int WINAPI dkcFreeBuffer(DKC_BUFFER **ptr);


/*!
バッファの先頭にdataをぶち込む
*/
DKC_EXTERN int WINAPI dkcBufferSet(DKC_BUFFER *ptr,const void *data,size_t size);
///バッファの先頭からoffset分の所にdataをぶち込む
DKC_EXTERN int WINAPI dkcBufferSetOffset(DKC_BUFFER *ptr,
																				 const void *data,size_t size,size_t offset);


/*!
バッファの先頭からsize分データをいただく
*/
DKC_EXTERN int WINAPI dkcBufferGet(DKC_BUFFER *ptr,void *data,size_t size);

DKC_EXTERN int WINAPI dkcBufferGetOffset(DKC_BUFFER *ptr,
																				 void *data,size_t size,size_t offset);
/*!
バッファのサイズを変える。
*/
DKC_EXTERN int WINAPI dkcBufferResize(DKC_BUFFER *ptr,size_t size);
///C++で言うcopy constructor
DKC_EXTERN DKC_BUFFER* WINAPI dkcAllocBufferCopy(const DKC_BUFFER *);

///バッファのサイズ
DKC_EXTERN size_t WINAPI dkcBufferSize(DKC_BUFFER *p);

DKC_EXTERN BYTE *WINAPI dkcBufferPointer(DKC_BUFFER *p);



#if !defined( DKUTIL_C_BUFFER_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcBuffer.c"
#endif


#endif //end of include once