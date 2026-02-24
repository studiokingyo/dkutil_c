
/*!
@file dkcBLAKE2.c
@brief BLAKE2b / BLAKE2s hash (RFC 7693)
@note BLAKE2b: 64-bit, 12 rounds; BLAKE2s: 32-bit, 10 rounds
      No pi constant XOR (simplified G function vs original BLAKE)
*/
#define DKUTIL_C_BLAKE2_C

#include "dkcBLAKE2.h"
#include "dkcStdio.h"
#include <stdio.h>

/* BLAKE2b IV (same as SHA-512 IV) */
static const QWORD blake2b_iv[8] = {
	0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
	0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
	0x510e527fade682d1, 0x9b05688c2b3e6c1f,
	0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
};

/* BLAKE2s IV (same as SHA-256 IV) */
static const uint32 blake2s_iv[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

/* Sigma permutation (same as BLAKE) */
static const unsigned char blake2_sigma[10][16] = {
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
 * BLAKE2b internal
 * ============================== */

static DKC_INLINE QWORD blake2b_rot(QWORD x, int n){
	return (x >> n) | (x << (64 - n));
}

static DKC_INLINE QWORD blake2b_load64le(const BYTE *p){
	return ((QWORD)p[0])       | ((QWORD)p[1] << 8)  |
	       ((QWORD)p[2] << 16) | ((QWORD)p[3] << 24) |
	       ((QWORD)p[4] << 32) | ((QWORD)p[5] << 40) |
	       ((QWORD)p[6] << 48) | ((QWORD)p[7] << 56);
}

static DKC_INLINE void blake2b_store64le(BYTE *p, QWORD v){
	p[0] = (BYTE)(v);
	p[1] = (BYTE)(v >> 8);
	p[2] = (BYTE)(v >> 16);
	p[3] = (BYTE)(v >> 24);
	p[4] = (BYTE)(v >> 32);
	p[5] = (BYTE)(v >> 40);
	p[6] = (BYTE)(v >> 48);
	p[7] = (BYTE)(v >> 56);
}

#define BLAKE2B_G(a,b,c,d,x,y) \
	do { \
		v[a] += v[b] + x; \
		v[d] = blake2b_rot(v[d] ^ v[a], 32); \
		v[c] += v[d]; \
		v[b] = blake2b_rot(v[b] ^ v[c], 24); \
		v[a] += v[b] + y; \
		v[d] = blake2b_rot(v[d] ^ v[a], 16); \
		v[c] += v[d]; \
		v[b] = blake2b_rot(v[b] ^ v[c], 63); \
	} while(0)

static void blake2b_compress(DKC_BLAKE2B *ctx, const BYTE *block){
	QWORD v[16];
	QWORD m[16];
	int i;
	int r;
	const unsigned char *s;

	for(i = 0; i < 16; i++){
		m[i] = blake2b_load64le(block + i * 8);
	}

	for(i = 0; i < 8; i++) v[i] = ctx->h[i];
	v[ 8] = blake2b_iv[0];
	v[ 9] = blake2b_iv[1];
	v[10] = blake2b_iv[2];
	v[11] = blake2b_iv[3];
	v[12] = blake2b_iv[4] ^ ctx->t[0];
	v[13] = blake2b_iv[5] ^ ctx->t[1];
	v[14] = blake2b_iv[6] ^ ctx->f[0];
	v[15] = blake2b_iv[7] ^ ctx->f[1];

	/* 12 rounds */
	for(i = 0; i < 12; i++){
		r = i % 10;
		s = blake2_sigma[r];
		BLAKE2B_G(0,4, 8,12, m[s[ 0]], m[s[ 1]]);
		BLAKE2B_G(1,5, 9,13, m[s[ 2]], m[s[ 3]]);
		BLAKE2B_G(2,6,10,14, m[s[ 4]], m[s[ 5]]);
		BLAKE2B_G(3,7,11,15, m[s[ 6]], m[s[ 7]]);
		BLAKE2B_G(0,5,10,15, m[s[ 8]], m[s[ 9]]);
		BLAKE2B_G(1,6,11,12, m[s[10]], m[s[11]]);
		BLAKE2B_G(2,7, 8,13, m[s[12]], m[s[13]]);
		BLAKE2B_G(3,4, 9,14, m[s[14]], m[s[15]]);
	}

	for(i = 0; i < 8; i++){
		ctx->h[i] ^= v[i] ^ v[i + 8];
	}
}

#undef BLAKE2B_G

/* ==============================
 * BLAKE2b API
 * ============================== */

DKC_BLAKE2B * WINAPI dkcAllocBLAKE2b(){
	DKC_BLAKE2B *p = (DKC_BLAKE2B *)dkcAllocate(sizeof(DKC_BLAKE2B));
	if(NULL == p) return NULL;
	dkcBLAKE2bInit(p);
	return p;
}

void WINAPI dkcBLAKE2bInit(DKC_BLAKE2B *p){
	int i;
	p->outLen = 64; /* default output length */
	for(i = 0; i < 8; i++) p->h[i] = blake2b_iv[i];
	/* XOR parameter block into h[0]: fan-out=1, depth=1, digest_length=64 */
	p->h[0] ^= 0x01010040; /* 0x01010000 | 0 | 64 */
	p->t[0] = p->t[1] = 0;
	p->f[0] = p->f[1] = 0;
	p->bufLen = 0;
	p->mFinalized = FALSE;
}

void WINAPI dkcBLAKE2bLoad(DKC_BLAKE2B *p, const BYTE *pBuffer, DWORD dwSize){
	size_t left, fill;
	if(p->mFinalized) return;

	while(dwSize > 0){
		/* If buffer is full, compress it first */
		if(p->bufLen == 128){
			p->t[0] += 128;
			if(p->t[0] < 128) p->t[1]++;
			blake2b_compress(p, p->buf);
			p->bufLen = 0;
		}
		left = 128 - p->bufLen;
		fill = ((size_t)dwSize < left) ? (size_t)dwSize : left;
		memcpy(p->buf + p->bufLen, pBuffer, fill);
		p->bufLen += fill;
		pBuffer += fill;
		dwSize -= (DWORD)fill;
	}
}

void WINAPI dkcBLAKE2bFinal(DKC_BLAKE2B *p){
	if(p->mFinalized) return;

	p->t[0] += (QWORD)p->bufLen;
	if(p->t[0] < (QWORD)p->bufLen) p->t[1]++;
	p->f[0] = ~((QWORD)0); /* set finalization flag */

	/* Pad remaining buffer with zeros */
	if(p->bufLen < 128){
		memset(p->buf + p->bufLen, 0, 128 - p->bufLen);
	}
	blake2b_compress(p, p->buf);
	p->mFinalized = TRUE;
}

int WINAPI dkcBLAKE2bDigestStr(DKC_BLAKE2B *p, char *buff, size_t size){
	char s[BLAKE2B_STR_BUFFER_SIZE];
	BYTE bin[BLAKE2B_BIN_BUFFER_SIZE];
	size_t i;
	size_t str_size;
	str_size = p->outLen * 2 + 1;
	if(str_size > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake2b_store64le(bin + i * 8, p->h[i]);
	for(i = 0; i < p->outLen; i++) sprintf(s + i * 2, "%02x", bin[i]);
	s[p->outLen * 2] = '\0';
	return dkc_strcpy(buff, size, s, p->outLen * 2);
}

int WINAPI dkcBLAKE2bDigest(DKC_BLAKE2B *p, BYTE *buff, size_t size){
	BYTE bin[BLAKE2B_BIN_BUFFER_SIZE];
	size_t i;
	if(p->outLen > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake2b_store64le(bin + i * 8, p->h[i]);
	memcpy(buff, bin, p->outLen);
	return edk_SUCCEEDED;
}

int WINAPI dkcBLAKE2bFinalDigestStr(DKC_BLAKE2B *p, char *buff, size_t size){
	dkcBLAKE2bFinal(p);
	return dkcBLAKE2bDigestStr(p, buff, size);
}

int WINAPI dkcBLAKE2bFinalDigest(DKC_BLAKE2B *p, BYTE *buff, size_t size){
	dkcBLAKE2bFinal(p);
	return dkcBLAKE2bDigest(p, buff, size);
}

int WINAPI dkcFreeBLAKE2b(DKC_BLAKE2B **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}

/* ==============================
 * BLAKE2s internal
 * ============================== */

static DKC_INLINE uint32 blake2s_rot(uint32 x, int n){
	return (x >> n) | (x << (32 - n));
}

static DKC_INLINE uint32 blake2s_load32le(const BYTE *p){
	return ((uint32)p[0])       | ((uint32)p[1] << 8) |
	       ((uint32)p[2] << 16) | ((uint32)p[3] << 24);
}

static DKC_INLINE void blake2s_store32le(BYTE *p, uint32 v){
	p[0] = (BYTE)(v);
	p[1] = (BYTE)(v >> 8);
	p[2] = (BYTE)(v >> 16);
	p[3] = (BYTE)(v >> 24);
}

#define BLAKE2S_G(a,b,c,d,x,y) \
	do { \
		v[a] += v[b] + x; \
		v[d] = blake2s_rot(v[d] ^ v[a], 16); \
		v[c] += v[d]; \
		v[b] = blake2s_rot(v[b] ^ v[c], 12); \
		v[a] += v[b] + y; \
		v[d] = blake2s_rot(v[d] ^ v[a], 8); \
		v[c] += v[d]; \
		v[b] = blake2s_rot(v[b] ^ v[c], 7); \
	} while(0)

static void blake2s_compress(DKC_BLAKE2S *ctx, const BYTE *block){
	uint32 v[16];
	uint32 m[16];
	int i;
	int r;
	const unsigned char *s;

	for(i = 0; i < 16; i++){
		m[i] = blake2s_load32le(block + i * 4);
	}

	for(i = 0; i < 8; i++) v[i] = ctx->h[i];
	v[ 8] = blake2s_iv[0];
	v[ 9] = blake2s_iv[1];
	v[10] = blake2s_iv[2];
	v[11] = blake2s_iv[3];
	v[12] = blake2s_iv[4] ^ ctx->t[0];
	v[13] = blake2s_iv[5] ^ ctx->t[1];
	v[14] = blake2s_iv[6] ^ ctx->f[0];
	v[15] = blake2s_iv[7] ^ ctx->f[1];

	/* 10 rounds */
	for(i = 0; i < 10; i++){
		r = i % 10;
		s = blake2_sigma[r];
		BLAKE2S_G(0,4, 8,12, m[s[ 0]], m[s[ 1]]);
		BLAKE2S_G(1,5, 9,13, m[s[ 2]], m[s[ 3]]);
		BLAKE2S_G(2,6,10,14, m[s[ 4]], m[s[ 5]]);
		BLAKE2S_G(3,7,11,15, m[s[ 6]], m[s[ 7]]);
		BLAKE2S_G(0,5,10,15, m[s[ 8]], m[s[ 9]]);
		BLAKE2S_G(1,6,11,12, m[s[10]], m[s[11]]);
		BLAKE2S_G(2,7, 8,13, m[s[12]], m[s[13]]);
		BLAKE2S_G(3,4, 9,14, m[s[14]], m[s[15]]);
	}

	for(i = 0; i < 8; i++){
		ctx->h[i] ^= v[i] ^ v[i + 8];
	}
}

#undef BLAKE2S_G

/* ==============================
 * BLAKE2s API
 * ============================== */

DKC_BLAKE2S * WINAPI dkcAllocBLAKE2s(){
	DKC_BLAKE2S *p = (DKC_BLAKE2S *)dkcAllocate(sizeof(DKC_BLAKE2S));
	if(NULL == p) return NULL;
	dkcBLAKE2sInit(p);
	return p;
}

void WINAPI dkcBLAKE2sInit(DKC_BLAKE2S *p){
	int i;
	p->outLen = 32; /* default output length */
	for(i = 0; i < 8; i++) p->h[i] = blake2s_iv[i];
	/* XOR parameter block into h[0]: fan-out=1, depth=1, digest_length=32 */
	p->h[0] ^= 0x01010020; /* 0x01010000 | 0 | 32 */
	p->t[0] = p->t[1] = 0;
	p->f[0] = p->f[1] = 0;
	p->bufLen = 0;
	p->mFinalized = FALSE;
}

void WINAPI dkcBLAKE2sLoad(DKC_BLAKE2S *p, const BYTE *pBuffer, DWORD dwSize){
	size_t left, fill;
	if(p->mFinalized) return;

	while(dwSize > 0){
		if(p->bufLen == 64){
			p->t[0] += 64;
			if(p->t[0] < 64) p->t[1]++;
			blake2s_compress(p, p->buf);
			p->bufLen = 0;
		}
		left = 64 - p->bufLen;
		fill = ((size_t)dwSize < left) ? (size_t)dwSize : left;
		memcpy(p->buf + p->bufLen, pBuffer, fill);
		p->bufLen += fill;
		pBuffer += fill;
		dwSize -= (DWORD)fill;
	}
}

void WINAPI dkcBLAKE2sFinal(DKC_BLAKE2S *p){
	if(p->mFinalized) return;

	p->t[0] += (uint32)p->bufLen;
	if(p->t[0] < (uint32)p->bufLen) p->t[1]++;
	p->f[0] = ~((uint32)0);

	if(p->bufLen < 64){
		memset(p->buf + p->bufLen, 0, 64 - p->bufLen);
	}
	blake2s_compress(p, p->buf);
	p->mFinalized = TRUE;
}

int WINAPI dkcBLAKE2sDigestStr(DKC_BLAKE2S *p, char *buff, size_t size){
	char s[BLAKE2S_STR_BUFFER_SIZE];
	BYTE bin[BLAKE2S_BIN_BUFFER_SIZE];
	size_t i;
	size_t str_size;
	str_size = p->outLen * 2 + 1;
	if(str_size > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake2s_store32le(bin + i * 4, p->h[i]);
	for(i = 0; i < p->outLen; i++) sprintf(s + i * 2, "%02x", bin[i]);
	s[p->outLen * 2] = '\0';
	return dkc_strcpy(buff, size, s, p->outLen * 2);
}

int WINAPI dkcBLAKE2sDigest(DKC_BLAKE2S *p, BYTE *buff, size_t size){
	BYTE bin[BLAKE2S_BIN_BUFFER_SIZE];
	size_t i;
	if(p->outLen > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake2s_store32le(bin + i * 4, p->h[i]);
	memcpy(buff, bin, p->outLen);
	return edk_SUCCEEDED;
}

int WINAPI dkcBLAKE2sFinalDigestStr(DKC_BLAKE2S *p, char *buff, size_t size){
	dkcBLAKE2sFinal(p);
	return dkcBLAKE2sDigestStr(p, buff, size);
}

int WINAPI dkcBLAKE2sFinalDigest(DKC_BLAKE2S *p, BYTE *buff, size_t size){
	dkcBLAKE2sFinal(p);
	return dkcBLAKE2sDigest(p, buff, size);
}

int WINAPI dkcFreeBLAKE2s(DKC_BLAKE2S **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}
