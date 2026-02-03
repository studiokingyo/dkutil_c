/*!
@file dkcBlockSort.h
@author d金魚
@since 2004/10/17
@brief block sort algorithm
*/

#ifndef DKUTIL_C_BLOCKSORT_H
#define DKUTIL_C_BLOCKSORT_H

#include "dkcOSIndependent.h"

typedef struct dkc_BlockSortInfo{
	///何番目に出力されたか
	size_t mOffset;
	///出力されたデータへのポインタ（dkcBlockSortEncode()に渡したbuffと同じアドレスを指す）
	void *mResultPointer;
}DKC_BLOCKSORT_INFO;
/**
@param buff[in][out] ソートするデータ
@param buffsize[in] buffの有効範囲のバイト単位のサイズ
@param p[out] 処理結果を返す。この関数の戻り値が成功を示さなければ正しい処理結果を返さない。
@return edk_SUCCEEDEDで成功。それ以外は処理を完了されていない。
@note
buffに入れたデータはメチャクチャになっているので
buffにいったんぶち込んだデータをこの関数が失敗した再、再利用してはいけない。

*/
DKC_EXTERN int WINAPI dkcBlockSortEncode(void *buff,size_t buffsize,DKC_BLOCKSORT_INFO *p);

DKC_EXTERN int WINAPI dkcBlockSortDecode(void *buff,size_t buffsize,DKC_BLOCKSORT_INFO *p);

#if !defined(  DKUTIL_C_BLOCKSORT_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcBlockSort.c"
#endif
#endif //end of include once