/*!
@file dkcSHA3.h
@brief SHA3-224/256/384/512 (FIPS 202)
@note Uses Keccak sponge construction
*/
#ifndef DKUTIL_C_SHA3_H
#define DKUTIL_C_SHA3_H

#include "dkcOSIndependent.h"
#include "dkcKeccak.h"

/* SHA3 uses Keccak directly */
typedef DKC_KECCAK DKC_SHA3_224;
typedef DKC_KECCAK DKC_SHA3_256;
typedef DKC_KECCAK DKC_SHA3_384;
typedef DKC_KECCAK DKC_SHA3_512;

/* Buffer size constants */
#define SHA3_224_BIN_BUFFER_SIZE 28
#define SHA3_224_STR_BUFFER_SIZE 57   /* 28*2+1 */
#define SHA3_256_BIN_BUFFER_SIZE 32
#define SHA3_256_STR_BUFFER_SIZE 65   /* 32*2+1 */
#define SHA3_384_BIN_BUFFER_SIZE 48
#define SHA3_384_STR_BUFFER_SIZE 97   /* 48*2+1 */
#define SHA3_512_BIN_BUFFER_SIZE 64
#define SHA3_512_STR_BUFFER_SIZE 129  /* 64*2+1 */

/* ==============================
 * SHA3-224
 * ============================== */
DKC_EXTERN DKC_SHA3_224 * WINAPI dkcAllocSHA3_224();
DKC_EXTERN void WINAPI dkcSHA3_224Init(DKC_SHA3_224 *p);
DKC_EXTERN void WINAPI dkcSHA3_224Load(DKC_SHA3_224 *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcSHA3_224Final(DKC_SHA3_224 *p);
DKC_EXTERN int  WINAPI dkcSHA3_224DigestStr(DKC_SHA3_224 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_224Digest(DKC_SHA3_224 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_224FinalDigestStr(DKC_SHA3_224 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_224FinalDigest(DKC_SHA3_224 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeSHA3_224(DKC_SHA3_224 **p);

/* ==============================
 * SHA3-256
 * ============================== */
DKC_EXTERN DKC_SHA3_256 * WINAPI dkcAllocSHA3_256();
DKC_EXTERN void WINAPI dkcSHA3_256Init(DKC_SHA3_256 *p);
DKC_EXTERN void WINAPI dkcSHA3_256Load(DKC_SHA3_256 *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcSHA3_256Final(DKC_SHA3_256 *p);
DKC_EXTERN int  WINAPI dkcSHA3_256DigestStr(DKC_SHA3_256 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_256Digest(DKC_SHA3_256 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_256FinalDigestStr(DKC_SHA3_256 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_256FinalDigest(DKC_SHA3_256 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeSHA3_256(DKC_SHA3_256 **p);

/* ==============================
 * SHA3-384
 * ============================== */
DKC_EXTERN DKC_SHA3_384 * WINAPI dkcAllocSHA3_384();
DKC_EXTERN void WINAPI dkcSHA3_384Init(DKC_SHA3_384 *p);
DKC_EXTERN void WINAPI dkcSHA3_384Load(DKC_SHA3_384 *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcSHA3_384Final(DKC_SHA3_384 *p);
DKC_EXTERN int  WINAPI dkcSHA3_384DigestStr(DKC_SHA3_384 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_384Digest(DKC_SHA3_384 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_384FinalDigestStr(DKC_SHA3_384 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_384FinalDigest(DKC_SHA3_384 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeSHA3_384(DKC_SHA3_384 **p);

/* ==============================
 * SHA3-512
 * ============================== */
DKC_EXTERN DKC_SHA3_512 * WINAPI dkcAllocSHA3_512();
DKC_EXTERN void WINAPI dkcSHA3_512Init(DKC_SHA3_512 *p);
DKC_EXTERN void WINAPI dkcSHA3_512Load(DKC_SHA3_512 *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcSHA3_512Final(DKC_SHA3_512 *p);
DKC_EXTERN int  WINAPI dkcSHA3_512DigestStr(DKC_SHA3_512 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_512Digest(DKC_SHA3_512 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_512FinalDigestStr(DKC_SHA3_512 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcSHA3_512FinalDigest(DKC_SHA3_512 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeSHA3_512(DKC_SHA3_512 **p);

#endif /*end of include once*/
