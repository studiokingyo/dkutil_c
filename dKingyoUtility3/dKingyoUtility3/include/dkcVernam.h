/*!
@file dkcVernam.h
@brief 単純な Varnam 暗号
@author d金魚
*/
#ifndef DKUTIL_C_VERNAM_H
#define DKUTIL_C_VERNAM_H


#include "dkcOSIndependent.h"



//**********************************************************

///VERNAM暗号用ランダムコールバック
typedef int (WINAPIV *DKC_VERNAM_F_TYPE)(ULONG *,ULONG);

/*!
@param buffer[in][out] 暗号化するバッファへのポインタ（暗号化後のデータが格納される。
@param size[in] bufferのサイズ
@param key[in] キーバッファへのポインタ
@param keysize[in] キーバッファのサイズ
@param seed[in] 種値
@param ff[in] DKC_VERNAM_F_TYPE型の関数へのポインタ
@note
あらかじめ用意していた乱数列（key)から1バイト単位、ff関数の乱数値に従ってランダムに選んで暗号化します。
*/
DKC_EXTERN void WINAPI dkcVernamEncrypt(BYTE *buffer,size_t size,const BYTE *key,size_t keysize,ULONG seed,DKC_VERNAM_F_TYPE ff);
/*!
@param buffer[in][out] 復号化するバッファへのポインタ（複合化後のデータが格納される。
@param size[in] bufferのサイズ
@param key[in] キーバッファへのポインタ
@param keysize[in] キーバッファのサイズ
@param seed[in] 種値
@param ff[in] DKC_VERNAM_F_TYPE型の関数へのポインタ
*/
#define dkcVernamDecrypt(buffer,size,key,keysize,seed,ff) \
	dkcVernamEncrypt(buffer,size,key,keysize,seed,ff)

//DKC_EXTERN void WINAPI dkcVernamEncrypt2(BYTE *buffer,size_t size,const BYTE *key,size_t keysize,const char *operation);

/*!
@param buffer[in][out] 復号化するバッファへのポインタ（複合化後のデータが格納される。
@param size[in] bufferのサイズ
@param seed[in] 種値
@param ff[in] DKC_VERNAM_F_TYPE型の関数へのポインタ
@note
キーに直接乱数値を使います。（正統派バーナム暗号？
*/
DKC_EXTERN void WINAPI dkcEasyVernamEncrypt(BYTE *buffer,size_t size,ULONG seed,DKC_VERNAM_F_TYPE ff);
///@see dkcEasyVernamEncrypt
#define dkcEasyVernamDecrypt(buffer,size,seed,ff) \
	dkcEasyVernamEncrypt(buffer,size,seed,ff)


/*!
@param src[in] 暗号化したい1バイト
@param x[in] キー
@return 暗号化された？1バイト

@section 使い方
BYTE *a="abcde";
BYTE *key="hogee";
int i;
//暗号化
for(i=0;a[i] != '\0';i++){
	a[i] = dkcEncryptByte(a[i],key[i]);
}
//複合化
for(i=0;a[i] != '\0';i++){
	a[i] = dkcDecryptByte(a[i],key[i]);
}
*/
#if !defined(  DKUTIL_C_VERNAM_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcVernam.c"
#endif

#endif //end of include once