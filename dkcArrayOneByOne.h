
/*!
@author d金魚
@file dkcArrayOneByOne.h
@brief 配列プールにO(1)でデータを入れて、参照IDを返すコンテナです。
@since 2004/08/22
@note
アルゴリズム概要

- 配列の空き領域への配列の添え字（以下 参照ID)をStackに入れておく。
- 挿入するときはStackから空き領域を得る、そして参照IDを返す。
- 削除するときは参照IDを受け取り、そのIDをStackに返す。
- 初期化フラグによって自動的に内部バッファが動的拡張されたりする。
- バッファをいつでもリサイズする事が出来る。（しかし、内部にデータがある場合はバッファを小さくは出来ない。）

仕様
INT_MAX個の要素までしか挿入できない。
*/
#ifndef DKUTIL_C_ARRAY_ONEBYONE_H
#define DKUTIL_C_ARRAY_ONEBYONE_H

#include "dkcOSIndependent.h"
#include "dkcBuffer.h"
#include "dkcStack.h"

/*!
配列の中から空き領域を得るための機構が使う構造体
*/
typedef struct dkc_ArrayOneByOne{
	///バッファ
	DKC_BUFFER *mBuff;
	///空き領域スタック
	DKC_STACK *mStack;
	///一つの要素のオフセット
	size_t mOffset;
	///要素数
	size_t mNum;
	///拡張サイズ(個数)
	size_t mExtNum;
	///フラグ
	BYTE mFlag;

	
}DKC_ARRAY_ONEBYONE;

enum{
	///バッファの拡張は出来ないタイプ
	edkcArrayOneByOneStatic = 0,
	///バッファの格調が出来るタイプ
	edkcArrayOneByOneDynamic,
};
/*!
@param offset[in] データのオフセット
@param num[in] 初期バッファの数
(確保されるメモリ領域はoffset * num)
@note
内部でバッファを動的拡張しないモードで初期化します。
*/
DKC_EXTERN DKC_ARRAY_ONEBYONE * WINAPI dkcAllocArrayOneByOneStatic(size_t offset,size_t num);
///@see dkcAllocArrayOneByOneStatic() バッファが足りない場合、バッファを動的拡張するモードです。
DKC_EXTERN DKC_ARRAY_ONEBYONE * WINAPI dkcAllocArrayOneByOneDynamic(size_t offset,size_t num,size_t ext_num);

///dkcAllocArrayOneByOne　系で取得したメモリ領域を開放します。
DKC_EXTERN int WINAPI dkcFreeArrayOneByOne(DKC_ARRAY_ONEBYONE **pp);

///@see dkcArrayOneByOnePush()の安全版
DKC_EXTERN int WINAPI dkcArrayOneByOnePushSafe(DKC_ARRAY_ONEBYONE *,const void *data,size_t size);

/*!
@return 参照IDを返す。失敗した場合は -1
*/
///要素を挿入する
DKC_EXTERN int WINAPI dkcArrayOneByOnePush(DKC_ARRAY_ONEBYONE *,const void *data);

/*!
@return enum edk_Resultのどれかを返す。
*/
///要素を削除する
DKC_EXTERN int WINAPI dkcArrayOneByOnePop(DKC_ARRAY_ONEBYONE *,int id);

/*!
@param id[in] Pushで得た参照IDを指定する
@param ref[out] 関数が成功した場合、idへのポインタが返る。 
@return enum edk_Resultのどれかを返す。
*/
DKC_EXTERN int WINAPI dkcArrayOneByOneReference(DKC_ARRAY_ONEBYONE *,int id,void *ref);

///現在の使用している要素数を返す。
DKC_EXTERN size_t WINAPI dkcArrayOneByOneSize(DKC_ARRAY_ONEBYONE *);
///このコンテナの許容量（要素数）を返す。
DKC_EXTERN size_t WINAPI dkcArrayOneByOneCapacity(DKC_ARRAY_ONEBYONE *);
///このコンテナの残り領域（要素数）を返す
DKC_EXTERN size_t WINAPI dkcArrayOneByOneRestSize(DKC_ARRAY_ONEBYONE *p);
///一つの要素のサイズ（オフセット）を返す
DKC_EXTERN size_t WINAPI dkcArrayOneByOneOffset(DKC_ARRAY_ONEBYONE *);

//DKC_EXTERN int WINAPI dkcArrayOneByOneExpansion


/*
#ifndef DKUTIL_C_ARRAY_ONEBYONE_C
#	include "dkcArrayOneByOne.c"
#endif
*/
#endif //end of include once