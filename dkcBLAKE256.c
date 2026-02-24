
/*!
@file dkcBLAKE256.c
@brief BLAKE-256 / BLAKE-224 hash
@note BLAKE hash function (SHA-3 finalist) - 32-bit variant, 14 rounds
*/
#define DKUTIL_C_BLAKE256_C

#include "dkcBLAKE256.h"
#include "dkcStdio.h"
#include <stdio.h>

/* BLAKE-256 IV (same as SHA-256 IV) */
static const uint32 blake256_iv[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

/* BLAKE-224 IV (same as SHA-224 IV) */
static const uint32 blake224_iv[8] = {
	0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
	0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};

/* Pi constants (32-bit, from fractional part of pi) */
static const uint32 blake256_cst[16] = {
	0x243F6A88, 0x85A308D3, 0x13198A2E, 0x03707344,
	0xA4093822, 0x299F31D0, 0x082EFA98, 0xEC4E6C89,
	0x452821E6, 0x38D01377, 0xBE5466CF, 0x34E90C6C,
	0xC0AC29B7, 0xC97C50DD, 0x3F84D5B5, 0xB5470917
};

/* Sigma permutation (shared across BLAKE family) */
static const unsigned char blake_sigma[10][16] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15},
	{14,10, 4, 8, 9,15,13, 6, 1,12, 0, 2,11, 7, 5, 3},
	{11, 8,12, 0, 5, 2,15,13,10,14, 3, 6, 7, 1, 9, 4},
	{ 7, 9, 3, 1,13,12,11,14, 2, 6, 5,10, 4, 0,15, 8},
	{ 9, 0, 5, 7, 2, 4,10,15,14, 1,11,12, 6, 8, 3,13},
	{ 2,12, 6,10, 0,11, 8, 3, 4,13, 7, 5,15,14, 1, 9},
	{12, 5, 1,15,14,13, 4,10, 0, 7, 6, 3, 9, 2, 8,11},
	{13,11, 7,14,12, 1, 3, 9, 5, 0,15, 4, 8, 6, 2,10},
	{ 6,15,14, 9,11, 3, 0, 8,12, 2,13, 7, 1, 4,10, 5},
	{10, 2, 8, 4, 7, 6, 1, 5,15,11, 9,14, 3,12,13, 0}
};

/* ==============================
 * Internal helpers
 * ============================== */

static DKC_INLINE uint32 blake256_rot(uint32 x, int n){
	return (x >> n) | (x << (32 - n));
}

static DKC_INLINE uint32 blake256_load32(const BYTE *p){
	/* big-endian load */
	return ((uint32)p[0] << 24) | ((uint32)p[1] << 16) |
	       ((uint32)p[2] << 8)  | ((uint32)p[3]);
}

static DKC_INLINE void blake256_store32(BYTE *p, uint32 v){
	p[0] = (BYTE)(v >> 24);
	p[1] = (BYTE)(v >> 16);
	p[2] = (BYTE)(v >> 8);
	p[3] = (BYTE)(v);
}

#define BLAKE256_G(a,b,c,d,i) \
	do { \
		v[a] += v[b] + (m[blake_sigma[r][2*i]] ^ blake256_cst[blake_sigma[r][2*i+1]]); \
		v[d] = blake256_rot(v[d] ^ v[a], 16); \
		v[c] += v[d]; \
		v[b] = blake256_rot(v[b] ^ v[c], 12); \
		v[a] += v[b] + (m[blake_sigma[r][2*i+1]] ^ blake256_cst[blake_sigma[r][2*i]]); \
		v[d] = blake256_rot(v[d] ^ v[a], 8); \
		v[c] += v[d]; \
		v[b] = blake256_rot(v[b] ^ v[c], 7); \
	} while(0)

static void blake256_compress(DKC_BLAKE256 *p, const BYTE *block){
	uint32 v[16];
	uint32 m[16];
	int i;
	int r;

	/* Load message words (big-endian) */
	for(i = 0; i < 16; i++){
		m[i] = blake256_load32(block + i * 4);
	}

	/* Init working vector */
	for(i = 0; i < 8; i++) v[i] = p->h[i];
	v[ 8] = p->s[0] ^ blake256_cst[0];
	v[ 9] = p->s[1] ^ blake256_cst[1];
	v[10] = p->s[2] ^ blake256_cst[2];
	v[11] = p->s[3] ^ blake256_cst[3];
	v[12] = blake256_cst[4];
	v[13] = blake256_cst[5];
	v[14] = blake256_cst[6];
	v[15] = blake256_cst[7];

	if(!p->nullT){
		v[12] ^= p->t[0];
		v[13] ^= p->t[0];
		v[14] ^= p->t[1];
		v[15] ^= p->t[1];
	}

	/* 14 rounds with sigma index modulo 10 */
	for(i = 0; i < 14; i++){
		r = i % 10;
		/* Column step */
		BLAKE256_G(0,4, 8,12, 0);
		BLAKE256_G(1,5, 9,13, 1);
		BLAKE256_G(2,6,10,14, 2);
		BLAKE256_G(3,7,11,15, 3);
		/* Diagonal step */
		BLAKE256_G(0,5,10,15, 4);
		BLAKE256_G(1,6,11,12, 5);
		BLAKE256_G(2,7, 8,13, 6);
		BLAKE256_G(3,4, 9,14, 7);
	}

	/* Finalize */
	for(i = 0; i < 8; i++){
		p->h[i] ^= p->s[i & 3] ^ v[i] ^ v[i + 8];
	}
}

#undef BLAKE256_G

/* ==============================
 * BLAKE-256 API
 * ============================== */

static void blake256_init_with_iv(DKC_BLAKE256 *p, const uint32 *iv){
	int i;
	for(i = 0; i < 8; i++) p->h[i] = iv[i];
	p->s[0] = p->s[1] = p->s[2] = p->s[3] = 0;
	p->t[0] = p->t[1] = 0;
	p->bufLen = 0;
	p->nullT = 0;
	p->mFinalized = FALSE;
}

DKC_BLAKE256 * WINAPI dkcAllocBLAKE256(){
	DKC_BLAKE256 *p = (DKC_BLAKE256 *)dkcAllocate(sizeof(DKC_BLAKE256));
	if(NULL == p) return NULL;
	dkcBLAKE256Init(p);
	return p;
}

void WINAPI dkcBLAKE256Init(DKC_BLAKE256 *p){
	blake256_init_with_iv(p, blake256_iv);
}

void WINAPI dkcBLAKE256Load(DKC_BLAKE256 *p, const BYTE *pBuffer, DWORD dwSize){
	int left, fill;
	if(p->mFinalized) return;

	while(dwSize > 0){
		left = 64 - p->bufLen;
		fill = ((int)dwSize < left) ? (int)dwSize : left;
		memcpy(p->buf + p->bufLen, pBuffer, fill);
		p->bufLen += fill;
		pBuffer += fill;
		dwSize -= fill;

		if(p->bufLen == 64){
			p->t[0] += 512;
			if(p->t[0] < 512) p->t[1]++;
			blake256_compress(p, p->buf);
			p->bufLen = 0;
		}
	}
}

void WINAPI dkcBLAKE256Final(DKC_BLAKE256 *p){
	BYTE msgLen[8];
	uint32 lo, hi;

	if(p->mFinalized) return;

	/* Total message bit length (excluding padding) */
	lo = p->t[0] + ((uint32)p->bufLen << 3);
	hi = p->t[1];
	if(lo < ((uint32)p->bufLen << 3)) hi++;

	blake256_store32(msgLen, hi);
	blake256_store32(msgLen + 4, lo);

	if(p->bufLen == 55){
		/* Exactly one pad byte fits before length */
		p->t[0] = lo;
		p->t[1] = hi;
		p->buf[55] = 0x81;
		memcpy(p->buf + 56, msgLen, 8);
		blake256_compress(p, p->buf);
	} else if(p->bufLen < 55){
		/* Padding fits in one block */
		if(p->bufLen == 0){
			p->nullT = 1;
		} else {
			p->t[0] = lo;
			p->t[1] = hi;
		}
		p->buf[p->bufLen] = 0x80;
		memset(p->buf + p->bufLen + 1, 0, 54 - p->bufLen);
		p->buf[55] = 0x01;
		memcpy(p->buf + 56, msgLen, 8);
		blake256_compress(p, p->buf);
	} else {
		/* Need two blocks */
		p->t[0] = lo;
		p->t[1] = hi;
		p->buf[p->bufLen] = 0x80;
		memset(p->buf + p->bufLen + 1, 0, 63 - p->bufLen);
		blake256_compress(p, p->buf);
		/* Second block with nullT (counter = 0) */
		p->nullT = 1;
		p->t[0] = 0;
		p->t[1] = 0;
		memset(p->buf, 0, 55);
		p->buf[55] = 0x01;
		memcpy(p->buf + 56, msgLen, 8);
		blake256_compress(p, p->buf);
	}

	p->mFinalized = TRUE;
}

int WINAPI dkcBLAKE256DigestStr(DKC_BLAKE256 *p, char *buff, size_t size){
	char s[BLAKE256_STR_BUFFER_SIZE];
	BYTE bin[BLAKE256_BIN_BUFFER_SIZE];
	int i;
	if(BLAKE256_STR_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake256_store32(bin + i * 4, p->h[i]);
	for(i = 0; i < BLAKE256_BIN_BUFFER_SIZE; i++) sprintf(s + i * 2, "%02x", bin[i]);
	s[BLAKE256_BIN_BUFFER_SIZE * 2] = '\0';
	return dkc_strcpy(buff, size, s, BLAKE256_BIN_BUFFER_SIZE * 2);
}

int WINAPI dkcBLAKE256Digest(DKC_BLAKE256 *p, BYTE *buff, size_t size){
	int i;
	if(BLAKE256_BIN_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake256_store32(buff + i * 4, p->h[i]);
	return edk_SUCCEEDED;
}

int WINAPI dkcBLAKE256FinalDigestStr(DKC_BLAKE256 *p, char *buff, size_t size){
	dkcBLAKE256Final(p);
	return dkcBLAKE256DigestStr(p, buff, size);
}

int WINAPI dkcBLAKE256FinalDigest(DKC_BLAKE256 *p, BYTE *buff, size_t size){
	dkcBLAKE256Final(p);
	return dkcBLAKE256Digest(p, buff, size);
}

int WINAPI dkcFreeBLAKE256(DKC_BLAKE256 **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}

/* ==============================
 * BLAKE-224 API (wrapper over BLAKE-256 core with different IV)
 * ============================== */

DKC_BLAKE224 * WINAPI dkcAllocBLAKE224(){
	DKC_BLAKE224 *p = (DKC_BLAKE224 *)dkcAllocate(sizeof(DKC_BLAKE224));
	if(NULL == p) return NULL;
	dkcBLAKE224Init(p);
	return p;
}

void WINAPI dkcBLAKE224Init(DKC_BLAKE224 *p){
	blake256_init_with_iv(p, blake224_iv);
}

void WINAPI dkcBLAKE224Load(DKC_BLAKE224 *p, const BYTE *pBuffer, DWORD dwSize){
	dkcBLAKE256Load(p, pBuffer, dwSize);
}

void WINAPI dkcBLAKE224Final(DKC_BLAKE224 *p){
	dkcBLAKE256Final(p);
}

int WINAPI dkcBLAKE224DigestStr(DKC_BLAKE224 *p, char *buff, size_t size){
	char s[BLAKE224_STR_BUFFER_SIZE];
	BYTE bin[BLAKE256_BIN_BUFFER_SIZE];
	int i;
	if(BLAKE224_STR_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake256_store32(bin + i * 4, p->h[i]);
	for(i = 0; i < BLAKE224_BIN_BUFFER_SIZE; i++) sprintf(s + i * 2, "%02x", bin[i]);
	s[BLAKE224_BIN_BUFFER_SIZE * 2] = '\0';
	return dkc_strcpy(buff, size, s, BLAKE224_BIN_BUFFER_SIZE * 2);
}

int WINAPI dkcBLAKE224Digest(DKC_BLAKE224 *p, BYTE *buff, size_t size){
	int i;
	BYTE bin[BLAKE256_BIN_BUFFER_SIZE];
	if(BLAKE224_BIN_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake256_store32(bin + i * 4, p->h[i]);
	memcpy(buff, bin, BLAKE224_BIN_BUFFER_SIZE);
	return edk_SUCCEEDED;
}

int WINAPI dkcBLAKE224FinalDigestStr(DKC_BLAKE224 *p, char *buff, size_t size){
	dkcBLAKE224Final(p);
	return dkcBLAKE224DigestStr(p, buff, size);
}

int WINAPI dkcBLAKE224FinalDigest(DKC_BLAKE224 *p, BYTE *buff, size_t size){
	dkcBLAKE224Final(p);
	return dkcBLAKE224Digest(p, buff, size);
}

int WINAPI dkcFreeBLAKE224(DKC_BLAKE224 **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}
