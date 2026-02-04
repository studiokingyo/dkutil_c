/*!
@file dkcBLAKE512.h
@brief BLAKE-512 / BLAKE-384 hash
@note BLAKE hash function (SHA-3 finalist) - 64-bit variant
*/
#ifndef DKUTIL_C_BLAKE512_H
#define DKUTIL_C_BLAKE512_H

#include "dkcOSIndependent.h"

#define BLAKE512_BIN_BUFFER_SIZE 64
#define BLAKE512_STR_BUFFER_SIZE 129  /* 64*2+1 */
#define BLAKE384_BIN_BUFFER_SIZE 48
#define BLAKE384_STR_BUFFER_SIZE 97   /* 48*2+1 */

typedef struct dkc_BLAKE512{
	QWORD  h[8];
	QWORD  s[4];
	QWORD  t[2];        /* counter (bits) */
	BYTE   buf[128];
	int    bufLen;
	BYTE   nullT;
	BYTE   mFinalized;
}DKC_BLAKE512;

typedef DKC_BLAKE512 DKC_BLAKE384;

/* ==============================
 * BLAKE-512
 * ============================== */
DKC_EXTERN DKC_BLAKE512 * WINAPI dkcAllocBLAKE512();
DKC_EXTERN void WINAPI dkcBLAKE512Init(DKC_BLAKE512 *p);
DKC_EXTERN void WINAPI dkcBLAKE512Load(DKC_BLAKE512 *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcBLAKE512Final(DKC_BLAKE512 *p);
DKC_EXTERN int  WINAPI dkcBLAKE512DigestStr(DKC_BLAKE512 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE512Digest(DKC_BLAKE512 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE512FinalDigestStr(DKC_BLAKE512 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE512FinalDigest(DKC_BLAKE512 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeBLAKE512(DKC_BLAKE512 **p);

/* ==============================
 * BLAKE-384
 * ============================== */
DKC_EXTERN DKC_BLAKE384 * WINAPI dkcAllocBLAKE384();
DKC_EXTERN void WINAPI dkcBLAKE384Init(DKC_BLAKE384 *p);
DKC_EXTERN void WINAPI dkcBLAKE384Load(DKC_BLAKE384 *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcBLAKE384Final(DKC_BLAKE384 *p);
DKC_EXTERN int  WINAPI dkcBLAKE384DigestStr(DKC_BLAKE384 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE384Digest(DKC_BLAKE384 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE384FinalDigestStr(DKC_BLAKE384 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE384FinalDigest(DKC_BLAKE384 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeBLAKE384(DKC_BLAKE384 **p);

#endif /*end of include once*/
