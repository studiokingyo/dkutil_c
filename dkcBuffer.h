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

/**
dkcAllocBuffer()を使わないで初期化。
@warning よってこの関数の使用にはメモリの状態遷移をしっかり頭に入れておく事が重要
@note
なお、InitおよびUninitはdkcRedBlackTree.hと密接にかかわっている。
*/
DKC_INLINE void dkcBufferInit(DKC_BUFFER *p,size_t size){
	p->mBuff=(BYTE *)dkcAllocate(size);
	p->mSize = size;
}
///dkcBufferInit()で初期化したものの終了処理
DKC_INLINE void dkcBufferUninit(DKC_BUFFER *p){
	if(p->mBuff){
		dkcFree((void **)&p->mBuff);
	}
	p->mSize = 0;
}
DKC_INLINE int dkcBufferCopy(DKC_BUFFER *p,const DKC_BUFFER *src){
	void *tp = p->mBuff;
	if(tp){//メモリが残っていたら削除するなり。
		dkcFree(&tp);
	}
	p->mBuff = (BYTE *)dkcAllocate(src->mSize);
	if(NULL==p->mBuff){
		return edk_OutOfMemory;
	}
	p->mSize = src->mSize;
	return edk_SUCCEEDED;
}
DKC_INLINE void dkcBufferCopyShared(DKC_BUFFER *p,DKC_BUFFER *src){
	p->mBuff = src->mBuff;
	p->mSize = src->mSize;
}
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


//**********************************************************
//inline 
DKC_INLINE int WINAPI dkcBufferSet_INL(DKC_BUFFER *ptr,const void *data,size_t size)
{
	//if(NULL==ptr) return edk_ArgumentException;//

	return dkc_memcpy(ptr->mBuff,ptr->mSize,data,size);
}


DKC_INLINE int WINAPI dkcBufferSetOffset_INL(DKC_BUFFER *ptr,
															const void *data,size_t size,size_t offset){
	if(ptr->mSize < offset + size){
		return edk_BufferOverFlow;
	}
	return dkc_memcpy(
		ptr->mBuff + offset,
		ptr->mSize - offset
		,data,size);
}




DKC_INLINE int WINAPI dkcBufferGet_INL(DKC_BUFFER *ptr,void *data,size_t size){
	//if(NULL==ptr) return edk_ArgumentException;//fast
	return dkc_memcpy(data,size,ptr->mBuff,ptr->mSize);
}

DKC_INLINE int WINAPI dkcBufferGetOffset_INL(DKC_BUFFER *ptr,
															void *data,size_t size,size_t offset){
	if(ptr->mSize < offset + size){
		return edk_BufferOverFlow;
	}
	return dkc_memcpy(
		data,size,
		ptr->mBuff + offset,
		size
	);

}

DKC_INLINE int WINAPI dkcBufferResize_INL(DKC_BUFFER *ptr,size_t size)
{
	void *NewPtr;
	if(NULL==ptr || 0==size || NULL==ptr->mBuff) return edk_ArgumentException;
	if(DKUTIL_FAILED(dkcReallocate(&NewPtr,size,(void **)&(ptr->mBuff)))){
		return edk_FAILED;
	}

	ptr->mBuff = (BYTE *)NewPtr;
	ptr->mSize = size;

	return edk_SUCCEEDED;
}



DKC_INLINE DKC_BUFFER* WINAPI dkcAllocBufferCopy_INL(const DKC_BUFFER *ptr){
	dkcmNOT_ASSERT(NULL==ptr || NULL==ptr->mBuff);

	return dkcAllocBuffer(ptr->mBuff,ptr->mSize);
}

DKC_INLINE size_t WINAPI dkcBufferSize_INL(DKC_BUFFER *p){
	return p->mSize;
}
DKC_INLINE BYTE *WINAPI dkcBufferPointer_INL(DKC_BUFFER *p){
	return p->mBuff;
}

#if !defined( DKUTIL_C_BUFFER_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcBuffer.c"
#endif


#endif //end of include once