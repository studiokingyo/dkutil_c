/*!
@file dkcCamellia.h
@brief 政府が推奨している国産暗号Camellia
*/
#ifndef  DKUTIL_C_CAMELLIA_H
#define  DKUTIL_C_CAMELLIA_H

#include "dkutil_c/dkcOSIndependent.h"


///arcfour algprithm state (RC4互換のアルゴリズム
typedef struct dkc_Camellia{
	unsigned char mi;
	unsigned char mj;
	unsigned char msbox[256];
}DKC_CAMELLIA;

#if 0
/*!
@param key[in] keyへのポインタ
@param keylen[in] keyのサイズ
@return DKC_CAMELLIA_STATE 構造体へのポインタ
@note
使い終わったらdkcFreeCamelliaを使いましょう。
*/
DKC_EXTERN DKC_CAMELLIA_STATE * WINAPI dkcAllocCamellia(const unsigned char *key,size_t keylen);

/*!
@param dkcAllocCamelliaで確保した領域へのポインタへのポインタ
@return edk_SUCCEEDEDで成功
*/
DKC_EXTERN int WINAPI dkcFreeCamellia(DKC_CAMELLIA_STATE **p);

/*!
@param p[in][out] dkcAllocCamellia()で確保した領域へのポインタ
@return XORすべき値
*/
DKC_EXTERN unsigned char WINAPI dkcCamelliaByte(DKC_CAMELLIA_STATE *p);
/*!
@param p[in] dkcAllocCamellia()で確保した領域へのポインタ
@param dest[in] 出力バッファへのポインタ
@param destsize[in] destのサイズ
@param src[in] 入力バッファへのポインタ
@param srcsize[in] srcのサイズ
@return edk_SUCCEEDED で成功
*/
DKC_EXTERN int WINAPI dkcCamelliaEncrypt(DKC_CAMELLIA_STATE *p,
	unsigned char *dest,size_t destsize,
	const unsigned char *src,size_t srcsize);

DKC_EXTERN void WINAPI dkcCamelliaEncryptNoDest(DKC_CAMELLIA_STATE *p,
	unsigned char *dest_and_src,size_t dest_and_srcsize);	

#define dkcCamelliaDecrypt(p,dest,destsize,src,srcsize) \
	dkcCamelliaEncrypt(p,dest,destsize,src,srcsize)

#define dkcCamelliaDecryptNoDest(p,dest_and_src,dest_and_srcsize) \
	dkcCamelliaEncryptNoDest(p,dest_and_src,dest_and_srcsize)

#endif


#if !defined( DKUTIL_C_CAMELLIA_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcCamellia.c"
#endif

#endif //end of include once