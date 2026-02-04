/*!
@file dkcBLAKE256.h
@brief BLAKE-256 / BLAKE-224 hash
@note BLAKE hash function (SHA-3 finalist) - 32-bit variant
*/
#ifndef DKUTIL_C_BLAKE256_H
#define DKUTIL_C_BLAKE256_H

#include "dkcOSIndependent.h"

#define BLAKE256_BIN_BUFFER_SIZE 32
#define BLAKE256_STR_BUFFER_SIZE 65   /* 32*2+1 */
#define BLAKE224_BIN_BUFFER_SIZE 28
#define BLAKE224_STR_BUFFER_SIZE 57   /* 28*2+1 */

typedef struct dkc_BLAKE256{
	uint32 h[8];        /* chaining value */
	uint32 s[4];        /* salt (zero default) */
	uint32 t[2];        /* counter low, high (bits) */
	BYTE   buf[64];     /* partial block buffer */
	int    bufLen;
	BYTE   nullT;       /* null counter flag for finalization */
	BYTE   mFinalized;
}DKC_BLAKE256;

typedef DKC_BLAKE256 DKC_BLAKE224;

/* ==============================
 * BLAKE-256
 * ============================== */
DKC_EXTERN DKC_BLAKE256 * WINAPI dkcAllocBLAKE256();
DKC_EXTERN void WINAPI dkcBLAKE256Init(DKC_BLAKE256 *p);
DKC_EXTERN void WINAPI dkcBLAKE256Load(DKC_BLAKE256 *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcBLAKE256Final(DKC_BLAKE256 *p);
DKC_EXTERN int  WINAPI dkcBLAKE256DigestStr(DKC_BLAKE256 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE256Digest(DKC_BLAKE256 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE256FinalDigestStr(DKC_BLAKE256 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE256FinalDigest(DKC_BLAKE256 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeBLAKE256(DKC_BLAKE256 **p);

/* ==============================
 * BLAKE-224
 * ============================== */
DKC_EXTERN DKC_BLAKE224 * WINAPI dkcAllocBLAKE224();
DKC_EXTERN void WINAPI dkcBLAKE224Init(DKC_BLAKE224 *p);
DKC_EXTERN void WINAPI dkcBLAKE224Load(DKC_BLAKE224 *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcBLAKE224Final(DKC_BLAKE224 *p);
DKC_EXTERN int  WINAPI dkcBLAKE224DigestStr(DKC_BLAKE224 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE224Digest(DKC_BLAKE224 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE224FinalDigestStr(DKC_BLAKE224 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE224FinalDigest(DKC_BLAKE224 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeBLAKE224(DKC_BLAKE224 **p);

#endif /*end of include once*/
