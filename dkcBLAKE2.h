/*!
@file dkcBLAKE2.h
@brief BLAKE2b / BLAKE2s hash (RFC 7693)
@note BLAKE2b: 64-bit, 12 rounds; BLAKE2s: 32-bit, 10 rounds
*/
#ifndef DKUTIL_C_BLAKE2_H
#define DKUTIL_C_BLAKE2_H

#include "dkcOSIndependent.h"

#define BLAKE2B_BIN_BUFFER_SIZE 64
#define BLAKE2B_STR_BUFFER_SIZE 129  /* 64*2+1 */
#define BLAKE2S_BIN_BUFFER_SIZE 32
#define BLAKE2S_STR_BUFFER_SIZE 65   /* 32*2+1 */

typedef struct dkc_BLAKE2b{
	QWORD  h[8];
	QWORD  t[2];        /* counter (bytes) */
	QWORD  f[2];        /* finalization flags */
	BYTE   buf[128];
	size_t bufLen;
	size_t outLen;       /* desired output length (1-64) */
	BYTE   mFinalized;
}DKC_BLAKE2B;

typedef struct dkc_BLAKE2s{
	uint32 h[8];
	uint32 t[2];
	uint32 f[2];
	BYTE   buf[64];
	size_t bufLen;
	size_t outLen;       /* desired output length (1-32) */
	BYTE   mFinalized;
}DKC_BLAKE2S;

/* ==============================
 * BLAKE2b (default 64-byte output)
 * ============================== */
DKC_EXTERN DKC_BLAKE2B * WINAPI dkcAllocBLAKE2b();
DKC_EXTERN void WINAPI dkcBLAKE2bInit(DKC_BLAKE2B *p);
DKC_EXTERN void WINAPI dkcBLAKE2bLoad(DKC_BLAKE2B *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcBLAKE2bFinal(DKC_BLAKE2B *p);
DKC_EXTERN int  WINAPI dkcBLAKE2bDigestStr(DKC_BLAKE2B *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE2bDigest(DKC_BLAKE2B *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE2bFinalDigestStr(DKC_BLAKE2B *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE2bFinalDigest(DKC_BLAKE2B *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeBLAKE2b(DKC_BLAKE2B **p);

/* ==============================
 * BLAKE2s (default 32-byte output)
 * ============================== */
DKC_EXTERN DKC_BLAKE2S * WINAPI dkcAllocBLAKE2s();
DKC_EXTERN void WINAPI dkcBLAKE2sInit(DKC_BLAKE2S *p);
DKC_EXTERN void WINAPI dkcBLAKE2sLoad(DKC_BLAKE2S *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcBLAKE2sFinal(DKC_BLAKE2S *p);
DKC_EXTERN int  WINAPI dkcBLAKE2sDigestStr(DKC_BLAKE2S *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE2sDigest(DKC_BLAKE2S *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE2sFinalDigestStr(DKC_BLAKE2S *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE2sFinalDigest(DKC_BLAKE2S *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeBLAKE2s(DKC_BLAKE2S **p);

#endif /*end of include once*/
