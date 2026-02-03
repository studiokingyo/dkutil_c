/*!
@file dkcCamellia.h
@brief Camellia block cipher algorithm (128/192/256-bit key, 128-bit block)
@note
Camellia is a symmetric key block cipher developed jointly by
Mitsubishi Electric and NTT of Japan.
RFC 3713, ISO/IEC 18033-3
*/
#ifndef  DKUTIL_C_CAMELLIA_H
#define  DKUTIL_C_CAMELLIA_H

#include "dkcOSIndependent.h"

///Camellia block size in bytes (128 bits)
#define dkcd_CAMELLIA_BLOCK_SIZE 16

///Camellia max expanded key size in bytes (for 192/256-bit key: 34 subkeys * 8 bytes)
#define dkcd_CAMELLIA_MAX_EKEY_SIZE 272

///Camellia cipher state
typedef struct dkc_Camellia{
	int keyBits;
	unsigned char ekey[dkcd_CAMELLIA_MAX_EKEY_SIZE];
}DKC_CAMELLIA;

/*!
@param key[in] key pointer
@param keylen[in] key size in bytes (16, 24, or 32)
@return DKC_CAMELLIA pointer (NULL on failure)
@note
Call dkcFreeCamellia() to release.
*/
DKC_EXTERN DKC_CAMELLIA * WINAPI dkcAllocCamellia(const unsigned char *key,size_t keylen);

/*!
@param p[in] pointer to pointer allocated by dkcAllocCamellia
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcFreeCamellia(DKC_CAMELLIA **p);

/*!
@param p[in] DKC_CAMELLIA pointer allocated by dkcAllocCamellia()
@param dest[out] output buffer pointer
@param destsize[in] dest buffer size in bytes
@param src[in] input buffer pointer
@param srcsize[in] src size in bytes (must be multiple of 16)
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcCamelliaEncrypt(DKC_CAMELLIA *p,
	unsigned char *dest,size_t destsize,
	const unsigned char *src,size_t srcsize);

/*!
@param p[in] DKC_CAMELLIA pointer allocated by dkcAllocCamellia()
@param dest[out] output buffer pointer
@param destsize[in] dest buffer size in bytes
@param src[in] input buffer pointer
@param srcsize[in] src size in bytes (must be multiple of 16)
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcCamelliaDecrypt(DKC_CAMELLIA *p,
	unsigned char *dest,size_t destsize,
	const unsigned char *src,size_t srcsize);

/*!
@param p[in] DKC_CAMELLIA pointer allocated by dkcAllocCamellia()
@param dest_and_src[in][out] in-place buffer pointer
@param dest_and_srcsize[in] buffer size in bytes (must be multiple of 16)
*/
DKC_EXTERN void WINAPI dkcCamelliaEncryptNoDest(DKC_CAMELLIA *p,
	unsigned char *dest_and_src,size_t dest_and_srcsize);

/*!
@param p[in] DKC_CAMELLIA pointer allocated by dkcAllocCamellia()
@param dest_and_src[in][out] in-place buffer pointer
@param dest_and_srcsize[in] buffer size in bytes (must be multiple of 16)
*/
DKC_EXTERN void WINAPI dkcCamelliaDecryptNoDest(DKC_CAMELLIA *p,
	unsigned char *dest_and_src,size_t dest_and_srcsize);


#if !defined( DKUTIL_C_CAMELLIA_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcCamellia.c"
#endif

#endif //end of include once
