/*!
@file dkcSort.h
@author d金魚
@since 2004/04/28
@note
sort系のものたくさん。
@brief sort utility... qsort() like
*/

#ifndef DKUTIL_C_SORT_H
#define DKUTIL_C_SORT_H

#include "dkcOSIndependent.h"
#include "dkcBlockSort.h"

typedef DKC_COMPARE_TYPE DKC_SORT_COMPARE_TYPE;
//typedef int (WINAPIV *DKC_SORT_COMPARE_TYPE)(const void *elem1, const void *elem2);

/*!
いわいるシェルソートをする。
*/
DKC_EXTERN void WINAPI dkcShellSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
いわいるコムソートをする。
*/
DKC_EXTERN void WINAPI dkcCombSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
いわゆるバブルソートをする。
*/
DKC_EXTERN void WINAPI dkcBubbleSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
いわいるバイトニックソートをする。
@todo 実装できないかもしれない。
*/
DKC_EXTERN void WINAPI dkcBitonicSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
いわいるクイックソートをする。(qsortを使いましょう＾＾；
@todo qsortのラッパーに終わるかもしれない。
*/
DKC_EXTERN void WINAPI dkcQuickSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);
/*!
いわいるマルチパーテーションソート（多重分割ソート）
@author http://www.tokuyama.ac.jp/home/~kawamura/
@todo 実装できないかもしれない。
*/
DKC_EXTERN void WINAPI dkcMultiPartitionSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);




/*!
Distribution count sort
分布数えソート。大量の記憶領域を使うがO(n)で極速！
しかし、仮想記憶上ではほぼ意味無しか！？
@note
- dkcDistCountSortShort()やdkcDistCountSortChar()は遅いと思います。
- Min_ と Max_ の差が大きいほど　メモリを使います。 sizeof(type) * (abs(Max_) + abs(Min_) + 1 )バイトほど使います。詳しくはソースを見てください。

<s>０～USHRT_MAXまでの値しか使えません。ヽ(`Д´)ﾉｳﾜｧﾝ</s>
@return edk_SUCCEEDEDで成功
*/
//DKC_EXTERN void WINAPI dkcDistCountSort(USHORT *input_,USHORT *output_,size_t size);

DKC_EXTERN int WINAPI dkcDistCountSortInt(size_t num, const int *src, int *dest,int Min_,int Max_);
///@see dkcDistCountSortInt()
DKC_EXTERN int WINAPI dkcDistCountSortShort(size_t num, const short *src, short *dest,short Min_,short Max_);
///@see dkcDistCountSortInt()
DKC_EXTERN int WINAPI dkcDistCountSortChar(size_t num, const char *src, char *dest,char Min_,char Max_);

//DKC_EXTERN void WINAPI dkcDistCountSort

#if !defined(  DKUTIL_C_SORT_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcSort.c"
#endif

#endif //end of include once