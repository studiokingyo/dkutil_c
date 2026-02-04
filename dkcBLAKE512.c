
/*!
@file dkcBLAKE512.c
@brief BLAKE-512 / BLAKE-384 hash
@note BLAKE hash function (SHA-3 finalist) - 64-bit variant, 16 rounds
*/
#define DKUTIL_C_BLAKE512_C

#include "dkcBLAKE512.h"
#include "dkcStdio.h"
#include <stdio.h>

/* BLAKE-512 IV (same as SHA-512 IV) */
static const QWORD blake512_iv[8] = {
	0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
	0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
	0x510e527fade682d1, 0x9b05688c2b3e6c1f,
	0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};

/* BLAKE-384 IV (same as SHA-384 IV) */
static const QWORD blake384_iv[8] = {
	0xcbbb9d5dc1059ed8, 0x629a292a367cd507,
	0x9159015a3070dd17, 0x152fecd8f70e5939,
	0x67332667ffc00b31, 0x8eb44a8768581511,
	0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4};

/* Pi constants (64-bit, from fractional part of pi) */
static const QWORD blake512_cst[16] = {
	0x243F6A8885A308D3, 0x13198A2E03707344,
	0xA4093822299F31D0, 0x082EFA98EC4E6C89,
	0x452821E638D01377, 0xBE5466CF34E90C6C,
	0xC0AC29B7C97C50DD, 0x3F84D5B5B5470917,
	0x9216D5D98979FB1B, 0xD1310BA698DFB5AC,
	0x2FFD72DBD01ADFB7, 0xB8E1AFED6A267E96,
	0xBA7C9045F12C7F99, 0x24A19947B3916CF7,
	0x0801F2E2858EFC16, 0x636920D871574E69};

/* Sigma permutation */
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
	{10, 2, 8, 4, 7, 6, 1, 5,15,11, 9,14, 3,13,12, 0}
};

/* ==============================
 * Internal helpers
 * ============================== */

static DKC_INLINE QWORD blake512_rot(QWORD x, int n){
	return (x >> n) | (x << (64 - n));
}

static DKC_INLINE QWORD blake512_load64(const BYTE *p){
	return ((QWORD)p[0] << 56) | ((QWORD)p[1] << 48) |
	       ((QWORD)p[2] << 40) | ((QWORD)p[3] << 32) |
	       ((QWORD)p[4] << 24) | ((QWORD)p[5] << 16) |
	       ((QWORD)p[6] << 8)  | ((QWORD)p[7]);
}

static DKC_INLINE void blake512_store64(BYTE *p, QWORD v){
	p[0] = (BYTE)(v >> 56);
	p[1] = (BYTE)(v >> 48);
	p[2] = (BYTE)(v >> 40);
	p[3] = (BYTE)(v >> 32);
	p[4] = (BYTE)(v >> 24);
	p[5] = (BYTE)(v >> 16);
	p[6] = (BYTE)(v >> 8);
	p[7] = (BYTE)(v);
}

#define BLAKE512_G(a,b,c,d,i) \
	do { \
		v[a] += v[b] + (m[blake_sigma[r][2*i]] ^ blake512_cst[blake_sigma[r][2*i+1]]); \
		v[d] = blake512_rot(v[d] ^ v[a], 32); \
		v[c] += v[d]; \
		v[b] = blake512_rot(v[b] ^ v[c], 25); \
		v[a] += v[b] + (m[blake_sigma[r][2*i+1]] ^ blake512_cst[blake_sigma[r][2*i]]); \
		v[d] = blake512_rot(v[d] ^ v[a], 16); \
		v[c] += v[d]; \
		v[b] = blake512_rot(v[b] ^ v[c], 11); \
	} while(0)

static void blake512_compress(DKC_BLAKE512 *p, const BYTE *block){
	QWORD v[16];
	QWORD m[16];
	int i;
	int r;

	/* Load message words (big-endian) */
	for(i = 0; i < 16; i++){
		m[i] = blake512_load64(block + i * 8);
	}

	/* Init working vector */
	for(i = 0; i < 8; i++) v[i] = p->h[i];
	v[ 8] = p->s[0] ^ blake512_cst[0];
	v[ 9] = p->s[1] ^ blake512_cst[1];
	v[10] = p->s[2] ^ blake512_cst[2];
	v[11] = p->s[3] ^ blake512_cst[3];
	v[12] = blake512_cst[4];
	v[13] = blake512_cst[5];
	v[14] = blake512_cst[6];
	v[15] = blake512_cst[7];

	if(!p->nullT){
		v[12] ^= p->t[0];
		v[13] ^= p->t[0];
		v[14] ^= p->t[1];
		v[15] ^= p->t[1];
	}

	/* 16 rounds with sigma index modulo 10 */
	for(i = 0; i < 16; i++){
		r = i % 10;
		/* Column step */
		BLAKE512_G(0,4, 8,12, 0);
		BLAKE512_G(1,5, 9,13, 1);
		BLAKE512_G(2,6,10,14, 2);
		BLAKE512_G(3,7,11,15, 3);
		/* Diagonal step */
		BLAKE512_G(0,5,10,15, 4);
		BLAKE512_G(1,6,11,12, 5);
		BLAKE512_G(2,7, 8,13, 6);
		BLAKE512_G(3,4, 9,14, 7);
	}

	/* Finalize */
	for(i = 0; i < 8; i++){
		p->h[i] ^= p->s[i & 3] ^ v[i] ^ v[i + 8];
	}
}

#undef BLAKE512_G

/* ==============================
 * BLAKE-512 API
 * ============================== */

static void blake512_init_with_iv(DKC_BLAKE512 *p, const QWORD *iv){
	int i;
	for(i = 0; i < 8; i++) p->h[i] = iv[i];
	p->s[0] = p->s[1] = p->s[2] = p->s[3] = 0;
	p->t[0] = p->t[1] = 0;
	p->bufLen = 0;
	p->nullT = 0;
	p->mFinalized = FALSE;
}

DKC_BLAKE512 * WINAPI dkcAllocBLAKE512(){
	DKC_BLAKE512 *p = (DKC_BLAKE512 *)dkcAllocate(sizeof(DKC_BLAKE512));
	if(NULL == p) return NULL;
	dkcBLAKE512Init(p);
	return p;
}

void WINAPI dkcBLAKE512Init(DKC_BLAKE512 *p){
	blake512_init_with_iv(p, blake512_iv);
}

void WINAPI dkcBLAKE512Load(DKC_BLAKE512 *p, const BYTE *pBuffer, DWORD dwSize){
	int left, fill;
	if(p->mFinalized) return;

	while(dwSize > 0){
		left = 128 - p->bufLen;
		fill = ((int)dwSize < left) ? (int)dwSize : left;
		memcpy(p->buf + p->bufLen, pBuffer, fill);
		p->bufLen += fill;
		pBuffer += fill;
		dwSize -= fill;

		if(p->bufLen == 128){
			p->t[0] += 1024;
			if(p->t[0] < 1024) p->t[1]++;
			blake512_compress(p, p->buf);
			p->bufLen = 0;
		}
	}
}

void WINAPI dkcBLAKE512Final(DKC_BLAKE512 *p){
	BYTE msgLen[16];
	QWORD lo, hi;

	if(p->mFinalized) return;

	/* Total message bit length (excluding padding) */
	lo = p->t[0] + ((QWORD)p->bufLen << 3);
	hi = p->t[1];
	if(lo < ((QWORD)p->bufLen << 3)) hi++;

	blake512_store64(msgLen, hi);
	blake512_store64(msgLen + 8, lo);

	if(p->bufLen == 111){
		p->t[0] = lo;
		p->t[1] = hi;
		p->buf[111] = 0x81;
		memcpy(p->buf + 112, msgLen, 16);
		blake512_compress(p, p->buf);
	} else if(p->bufLen < 111){
		if(p->bufLen == 0){
			p->nullT = 1;
		} else {
			p->t[0] = lo;
			p->t[1] = hi;
		}
		p->buf[p->bufLen] = 0x80;
		memset(p->buf + p->bufLen + 1, 0, 110 - p->bufLen);
		p->buf[111] = 0x01;
		memcpy(p->buf + 112, msgLen, 16);
		blake512_compress(p, p->buf);
	} else {
		p->t[0] = lo;
		p->t[1] = hi;
		p->buf[p->bufLen] = 0x80;
		memset(p->buf + p->bufLen + 1, 0, 127 - p->bufLen);
		blake512_compress(p, p->buf);
		/* Second block with nullT */
		p->nullT = 1;
		p->t[0] = 0;
		p->t[1] = 0;
		memset(p->buf, 0, 111);
		p->buf[111] = 0x01;
		memcpy(p->buf + 112, msgLen, 16);
		blake512_compress(p, p->buf);
	}

	p->mFinalized = TRUE;
}

int WINAPI dkcBLAKE512DigestStr(DKC_BLAKE512 *p, char *buff, size_t size){
	char s[BLAKE512_STR_BUFFER_SIZE];
	BYTE bin[BLAKE512_BIN_BUFFER_SIZE];
	int i;
	if(BLAKE512_STR_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake512_store64(bin + i * 8, p->h[i]);
	for(i = 0; i < BLAKE512_BIN_BUFFER_SIZE; i++) sprintf(s + i * 2, "%02x", bin[i]);
	s[BLAKE512_BIN_BUFFER_SIZE * 2] = '\0';
	return dkc_strcpy(buff, size, s, BLAKE512_BIN_BUFFER_SIZE * 2);
}

int WINAPI dkcBLAKE512Digest(DKC_BLAKE512 *p, BYTE *buff, size_t size){
	int i;
	if(BLAKE512_BIN_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake512_store64(buff + i * 8, p->h[i]);
	return edk_SUCCEEDED;
}

int WINAPI dkcBLAKE512FinalDigestStr(DKC_BLAKE512 *p, char *buff, size_t size){
	dkcBLAKE512Final(p);
	return dkcBLAKE512DigestStr(p, buff, size);
}

int WINAPI dkcBLAKE512FinalDigest(DKC_BLAKE512 *p, BYTE *buff, size_t size){
	dkcBLAKE512Final(p);
	return dkcBLAKE512Digest(p, buff, size);
}

int WINAPI dkcFreeBLAKE512(DKC_BLAKE512 **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}

/* ==============================
 * BLAKE-384 API (wrapper over BLAKE-512 core with different IV)
 * ============================== */

DKC_BLAKE384 * WINAPI dkcAllocBLAKE384(){
	DKC_BLAKE384 *p = (DKC_BLAKE384 *)dkcAllocate(sizeof(DKC_BLAKE384));
	if(NULL == p) return NULL;
	dkcBLAKE384Init(p);
	return p;
}

void WINAPI dkcBLAKE384Init(DKC_BLAKE384 *p){
	blake512_init_with_iv(p, blake384_iv);
}

void WINAPI dkcBLAKE384Load(DKC_BLAKE384 *p, const BYTE *pBuffer, DWORD dwSize){
	dkcBLAKE512Load(p, pBuffer, dwSize);
}

void WINAPI dkcBLAKE384Final(DKC_BLAKE384 *p){
	dkcBLAKE512Final(p);
}

int WINAPI dkcBLAKE384DigestStr(DKC_BLAKE384 *p, char *buff, size_t size){
	char s[BLAKE384_STR_BUFFER_SIZE];
	BYTE bin[BLAKE512_BIN_BUFFER_SIZE];
	int i;
	if(BLAKE384_STR_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake512_store64(bin + i * 8, p->h[i]);
	for(i = 0; i < BLAKE384_BIN_BUFFER_SIZE; i++) sprintf(s + i * 2, "%02x", bin[i]);
	s[BLAKE384_BIN_BUFFER_SIZE * 2] = '\0';
	return dkc_strcpy(buff, size, s, BLAKE384_BIN_BUFFER_SIZE * 2);
}

int WINAPI dkcBLAKE384Digest(DKC_BLAKE384 *p, BYTE *buff, size_t size){
	int i;
	BYTE bin[BLAKE512_BIN_BUFFER_SIZE];
	if(BLAKE384_BIN_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < 8; i++) blake512_store64(bin + i * 8, p->h[i]);
	memcpy(buff, bin, BLAKE384_BIN_BUFFER_SIZE);
	return edk_SUCCEEDED;
}

int WINAPI dkcBLAKE384FinalDigestStr(DKC_BLAKE384 *p, char *buff, size_t size){
	dkcBLAKE384Final(p);
	return dkcBLAKE384DigestStr(p, buff, size);
}

int WINAPI dkcBLAKE384FinalDigest(DKC_BLAKE384 *p, BYTE *buff, size_t size){
	dkcBLAKE384Final(p);
	return dkcBLAKE384Digest(p, buff, size);
}

int WINAPI dkcFreeBLAKE384(DKC_BLAKE384 **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}
