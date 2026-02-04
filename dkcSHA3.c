
/*!
@file dkcSHA3.c
@brief SHA3-224/256/384/512 (FIPS 202)
@note Wrapper over Keccak sponge construction
*/
#define DKUTIL_C_SHA3_C

#include "dkcSHA3.h"
#include "dkcStdio.h"
#include <stdio.h>

/*
 * SHA3 parameters (FIPS 202):
 *   SHA3-224: rate=144, capacity=56, output=28, suffix=0x06
 *   SHA3-256: rate=136, capacity=64, output=32, suffix=0x06
 *   SHA3-384: rate=104, capacity=96, output=48, suffix=0x06
 *   SHA3-512: rate= 72, capacity=128, output=64, suffix=0x06
 */

/* ==============================
 * Internal helpers
 * ============================== */

static int sha3_digest_str(DKC_KECCAK *p, char *buff, size_t size,
                           size_t bin_size, size_t str_size)
{
	char s[SHA3_512_STR_BUFFER_SIZE]; /* largest possible */
	BYTE bin[SHA3_512_BIN_BUFFER_SIZE];
	size_t i;

	if(str_size > size){
		return edk_BufferOverFlow;
	}
	if(FALSE == p->finalized){
		return edk_LogicError;
	}

	/* Read output bytes from state (little-endian) */
	for(i = 0; i < bin_size; i++){
		bin[i] = (BYTE)(p->state[i / 8] >> (8 * (i % 8)));
	}
	for(i = 0; i < bin_size; i++){
		sprintf(s + i * 2, "%02x", bin[i]);
	}
	s[bin_size * 2] = '\0';

	return dkc_strcpy(buff, size, s, bin_size * 2);
}

static int sha3_digest_bin(DKC_KECCAK *p, BYTE *buff, size_t size,
                           size_t bin_size)
{
	size_t i;

	if(bin_size > size){
		return edk_BufferOverFlow;
	}
	if(FALSE == p->finalized){
		return edk_LogicError;
	}

	/* Read output bytes from state (little-endian) */
	for(i = 0; i < bin_size; i++){
		buff[i] = (BYTE)(p->state[i / 8] >> (8 * (i % 8)));
	}
	return edk_SUCCEEDED;
}

/* ==============================
 * SHA3-224
 * ============================== */

DKC_SHA3_224 * WINAPI dkcAllocSHA3_224(){
	DKC_SHA3_224 *p = (DKC_SHA3_224 *)dkcAllocate(sizeof(DKC_SHA3_224));
	if(NULL == p) return NULL;
	dkcSHA3_224Init(p);
	return p;
}

void WINAPI dkcSHA3_224Init(DKC_SHA3_224 *p){
	dkcKeccakInit(p, 144, 56, 0x06);
}

void WINAPI dkcSHA3_224Load(DKC_SHA3_224 *p, const BYTE *pBuffer, DWORD dwSize){
	dkcKeccakAbsorb(p, pBuffer, (size_t)dwSize);
}

void WINAPI dkcSHA3_224Final(DKC_SHA3_224 *p){
	if(p->finalized) return;
	dkcKeccakSqueeze(p, NULL, 0);
}

int WINAPI dkcSHA3_224DigestStr(DKC_SHA3_224 *p, char *buff, size_t size){
	return sha3_digest_str(p, buff, size, SHA3_224_BIN_BUFFER_SIZE, SHA3_224_STR_BUFFER_SIZE);
}

int WINAPI dkcSHA3_224Digest(DKC_SHA3_224 *p, BYTE *buff, size_t size){
	return sha3_digest_bin(p, buff, size, SHA3_224_BIN_BUFFER_SIZE);
}

int WINAPI dkcSHA3_224FinalDigestStr(DKC_SHA3_224 *p, char *buff, size_t size){
	dkcSHA3_224Final(p);
	return dkcSHA3_224DigestStr(p, buff, size);
}

int WINAPI dkcSHA3_224FinalDigest(DKC_SHA3_224 *p, BYTE *buff, size_t size){
	dkcSHA3_224Final(p);
	return dkcSHA3_224Digest(p, buff, size);
}

int WINAPI dkcFreeSHA3_224(DKC_SHA3_224 **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}

/* ==============================
 * SHA3-256
 * ============================== */

DKC_SHA3_256 * WINAPI dkcAllocSHA3_256(){
	DKC_SHA3_256 *p = (DKC_SHA3_256 *)dkcAllocate(sizeof(DKC_SHA3_256));
	if(NULL == p) return NULL;
	dkcSHA3_256Init(p);
	return p;
}

void WINAPI dkcSHA3_256Init(DKC_SHA3_256 *p){
	dkcKeccakInit(p, 136, 64, 0x06);
}

void WINAPI dkcSHA3_256Load(DKC_SHA3_256 *p, const BYTE *pBuffer, DWORD dwSize){
	dkcKeccakAbsorb(p, pBuffer, (size_t)dwSize);
}

void WINAPI dkcSHA3_256Final(DKC_SHA3_256 *p){
	if(p->finalized) return;
	dkcKeccakSqueeze(p, NULL, 0);
}

int WINAPI dkcSHA3_256DigestStr(DKC_SHA3_256 *p, char *buff, size_t size){
	return sha3_digest_str(p, buff, size, SHA3_256_BIN_BUFFER_SIZE, SHA3_256_STR_BUFFER_SIZE);
}

int WINAPI dkcSHA3_256Digest(DKC_SHA3_256 *p, BYTE *buff, size_t size){
	return sha3_digest_bin(p, buff, size, SHA3_256_BIN_BUFFER_SIZE);
}

int WINAPI dkcSHA3_256FinalDigestStr(DKC_SHA3_256 *p, char *buff, size_t size){
	dkcSHA3_256Final(p);
	return dkcSHA3_256DigestStr(p, buff, size);
}

int WINAPI dkcSHA3_256FinalDigest(DKC_SHA3_256 *p, BYTE *buff, size_t size){
	dkcSHA3_256Final(p);
	return dkcSHA3_256Digest(p, buff, size);
}

int WINAPI dkcFreeSHA3_256(DKC_SHA3_256 **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}

/* ==============================
 * SHA3-384
 * ============================== */

DKC_SHA3_384 * WINAPI dkcAllocSHA3_384(){
	DKC_SHA3_384 *p = (DKC_SHA3_384 *)dkcAllocate(sizeof(DKC_SHA3_384));
	if(NULL == p) return NULL;
	dkcSHA3_384Init(p);
	return p;
}

void WINAPI dkcSHA3_384Init(DKC_SHA3_384 *p){
	dkcKeccakInit(p, 104, 96, 0x06);
}

void WINAPI dkcSHA3_384Load(DKC_SHA3_384 *p, const BYTE *pBuffer, DWORD dwSize){
	dkcKeccakAbsorb(p, pBuffer, (size_t)dwSize);
}

void WINAPI dkcSHA3_384Final(DKC_SHA3_384 *p){
	if(p->finalized) return;
	dkcKeccakSqueeze(p, NULL, 0);
}

int WINAPI dkcSHA3_384DigestStr(DKC_SHA3_384 *p, char *buff, size_t size){
	return sha3_digest_str(p, buff, size, SHA3_384_BIN_BUFFER_SIZE, SHA3_384_STR_BUFFER_SIZE);
}

int WINAPI dkcSHA3_384Digest(DKC_SHA3_384 *p, BYTE *buff, size_t size){
	return sha3_digest_bin(p, buff, size, SHA3_384_BIN_BUFFER_SIZE);
}

int WINAPI dkcSHA3_384FinalDigestStr(DKC_SHA3_384 *p, char *buff, size_t size){
	dkcSHA3_384Final(p);
	return dkcSHA3_384DigestStr(p, buff, size);
}

int WINAPI dkcSHA3_384FinalDigest(DKC_SHA3_384 *p, BYTE *buff, size_t size){
	dkcSHA3_384Final(p);
	return dkcSHA3_384Digest(p, buff, size);
}

int WINAPI dkcFreeSHA3_384(DKC_SHA3_384 **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}

/* ==============================
 * SHA3-512
 * ============================== */

DKC_SHA3_512 * WINAPI dkcAllocSHA3_512(){
	DKC_SHA3_512 *p = (DKC_SHA3_512 *)dkcAllocate(sizeof(DKC_SHA3_512));
	if(NULL == p) return NULL;
	dkcSHA3_512Init(p);
	return p;
}

void WINAPI dkcSHA3_512Init(DKC_SHA3_512 *p){
	dkcKeccakInit(p, 72, 128, 0x06);
}

void WINAPI dkcSHA3_512Load(DKC_SHA3_512 *p, const BYTE *pBuffer, DWORD dwSize){
	dkcKeccakAbsorb(p, pBuffer, (size_t)dwSize);
}

void WINAPI dkcSHA3_512Final(DKC_SHA3_512 *p){
	if(p->finalized) return;
	dkcKeccakSqueeze(p, NULL, 0);
}

int WINAPI dkcSHA3_512DigestStr(DKC_SHA3_512 *p, char *buff, size_t size){
	return sha3_digest_str(p, buff, size, SHA3_512_BIN_BUFFER_SIZE, SHA3_512_STR_BUFFER_SIZE);
}

int WINAPI dkcSHA3_512Digest(DKC_SHA3_512 *p, BYTE *buff, size_t size){
	return sha3_digest_bin(p, buff, size, SHA3_512_BIN_BUFFER_SIZE);
}

int WINAPI dkcSHA3_512FinalDigestStr(DKC_SHA3_512 *p, char *buff, size_t size){
	dkcSHA3_512Final(p);
	return dkcSHA3_512DigestStr(p, buff, size);
}

int WINAPI dkcSHA3_512FinalDigest(DKC_SHA3_512 *p, BYTE *buff, size_t size){
	dkcSHA3_512Final(p);
	return dkcSHA3_512Digest(p, buff, size);
}

int WINAPI dkcFreeSHA3_512(DKC_SHA3_512 **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}
