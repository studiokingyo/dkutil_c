/*!
@file dkcTwofish.h
@brief Twofish block cipher algorithm (128/192/256-bit key, 128-bit block)
@note
Twofish is a symmetric key block cipher designed by Bruce Schneier,
John Kelsey, Doug Whiting, David Wagner, Chris Hall, and Niels Ferguson.
AES finalist, 128-bit block size, 16 Feistel rounds.
*/
#ifndef  DKUTIL_C_TWOFISH_H
#define  DKUTIL_C_TWOFISH_H

#include "dkcOSIndependent.h"

/*Twofish block size in bytes (128 bits)*/
#define dkcd_TWOFISH_BLOCK_SIZE 16

/*Twofish number of rounds*/
#define dkcd_TWOFISH_ROUNDS 16

/*Twofish cipher state*/
typedef struct dkc_Twofish{
	uint32 S[4][256];    /* key-dependent S-boxes (4 x 256) */
	uint32 K[40];        /* 40 subkeys (whitening 8 + round 32) */
	int keyLen;          /* key length in bytes (16/24/32) */
}DKC_TWOFISH;

/*!
@param key[in] key pointer
@param keylen[in] key size in bytes (16, 24, or 32)
@return DKC_TWOFISH pointer (NULL on failure)
@note
Call dkcFreeTwofish() to release.
*/
DKC_EXTERN DKC_TWOFISH * WINAPI dkcAllocTwofish(const unsigned char *key,size_t keylen);

/*!
@param p[in] pointer to pointer allocated by dkcAllocTwofish
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcFreeTwofish(DKC_TWOFISH **p);

/*!
@param p[in] DKC_TWOFISH pointer allocated by dkcAllocTwofish()
@param dest[out] output buffer pointer
@param destsize[in] dest buffer size in bytes
@param src[in] input buffer pointer
@param srcsize[in] src size in bytes (must be multiple of 16)
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcTwofishEncrypt(DKC_TWOFISH *p,
	unsigned char *dest,size_t destsize,
	const unsigned char *src,size_t srcsize);

/*!
@param p[in] DKC_TWOFISH pointer allocated by dkcAllocTwofish()
@param dest[out] output buffer pointer
@param destsize[in] dest buffer size in bytes
@param src[in] input buffer pointer
@param srcsize[in] src size in bytes (must be multiple of 16)
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcTwofishDecrypt(DKC_TWOFISH *p,
	unsigned char *dest,size_t destsize,
	const unsigned char *src,size_t srcsize);

/*!
@param p[in] DKC_TWOFISH pointer allocated by dkcAllocTwofish()
@param dest_and_src[in][out] in-place buffer pointer
@param dest_and_srcsize[in] buffer size in bytes (must be multiple of 16)
*/
DKC_EXTERN void WINAPI dkcTwofishEncryptNoDest(DKC_TWOFISH *p,
	unsigned char *dest_and_src,size_t dest_and_srcsize);

/*!
@param p[in] DKC_TWOFISH pointer allocated by dkcAllocTwofish()
@param dest_and_src[in][out] in-place buffer pointer
@param dest_and_srcsize[in] buffer size in bytes (must be multiple of 16)
*/
DKC_EXTERN void WINAPI dkcTwofishDecryptNoDest(DKC_TWOFISH *p,
	unsigned char *dest_and_src,size_t dest_and_srcsize);


#if !defined( DKUTIL_C_TWOFISH_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcTwofish.c"
#endif

#endif /*end of include once*/
