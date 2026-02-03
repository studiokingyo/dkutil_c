
/*!
@file dkcSHA256.c
@brief SHA 256 
@author original : General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.
        reconstruct : d金魚
*/
#define DKUTIL_C_SHA256_C
#include "dkcSHA256.h"
#include "dkcStdio.h"


#	define  SHA256_BUFFER_SIZE (SHA256_BLOCK * 4)


const static DWORD c_dwInitH[SHA256_HASH] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19, 
};

const static DWORD c_dwK[SHA256_WORK] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2, 
};


static DKC_INLINE DWORD ReverseEndian(DWORD x) {
	return dkcReverseEndian32(x);
	//return (x << 24) | ((x & 0x0000ff00) << 8) | ((x & 0x00ff0000) >> 8) | (x >> 24);
}
static DKC_INLINE DWORD Rotate(DWORD x, DWORD n) {return (x >> n) | (x << (32 - n));}
static DKC_INLINE DWORD Ch(DWORD x, DWORD y, DWORD z) {return (x & (y ^ z)) ^ z;}
static DKC_INLINE DWORD Maj(DWORD x, DWORD y, DWORD z) {return (x & (y | z)) | (y & z);}
static DKC_INLINE DWORD S0(DWORD x) {return Rotate(x,  2) ^ Rotate(x, 13) ^ Rotate(x, 22);}
static DKC_INLINE DWORD S1(DWORD x) {return Rotate(x,  6) ^ Rotate(x, 11) ^ Rotate(x, 25);}
static DKC_INLINE DWORD s0(DWORD x) {return Rotate(x,  7) ^ Rotate(x, 18) ^ (x >> 3);}
static DKC_INLINE DWORD s1(DWORD x) {return Rotate(x, 17) ^ Rotate(x, 19) ^ (x >> 10);}


static void Generate(DKC_SHA256 *p){
	int i;
	DWORD W[SHA256_WORK];
	DWORD Hash[SHA256_WORK + SHA256_HASH];
	DWORD *pHash;
	DWORD dwT1, dwT2;


	for(i = 0; i < SHA256_BLOCK; i++) W[i] = ReverseEndian(p->m_aBlock[i]);
	for(i = SHA256_BLOCK; i < SHA256_WORK; i++) W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16];

	for(i = 0; i < SHA256_HASH; i++) Hash[SHA256_WORK + i] = p->m_dwH[i];
	pHash = &Hash[SHA256_WORK];
	
	for(i = 0; i < SHA256_WORK; i++){
		pHash--;
		dwT1 = pHash[8] + S1(pHash[5]) + Ch(pHash[5], pHash[6], pHash[7]) + c_dwK[i] + W[i];
		dwT2 = S0(pHash[1]) + Maj(pHash[1], pHash[2], pHash[3]);
		pHash[0] = dwT1 + dwT2;
		pHash[4] += dwT1;
	}
	for(i = 0; i < SHA256_HASH; i++) p->m_dwH[i] += pHash[i];
}

DKC_SHA256 *WINAPI dkcAllocSHA256(){
	DKC_SHA256 *p = dkcAllocate(sizeof(DKC_SHA256));
	if(NULL==p){
		return NULL;
	}
	dkcSHA256Init(p);
	return p;
}

void WINAPI dkcSHA256Init(DKC_SHA256 *p){
	int i;
	for(i = 0; i < SHA256_HASH; i++){
		p->m_dwH[i] = c_dwInitH[i];
	}
	p->m_dwLNumBits = 0;
	p->m_dwHNumBits = 0;
	p->m_nNumChr = 0;
	p->mFinalized = FALSE;
}

void WINAPI dkcSHA256Load(DKC_SHA256 *p,const BYTE *pBuffer,DWORD dwSize){
	DWORD dwReadSize;
	DWORD dwLNumBits;
	BYTE *pBlock;

	if(dwSize == 0) return;
	if(p->mFinalized){
		return;
	}
	dwLNumBits = (p->m_dwLNumBits + (dwSize << 3));
	if(dwLNumBits < p->m_dwLNumBits) p->m_dwHNumBits++;
	p->m_dwHNumBits += dwSize >> 29;
	p->m_dwLNumBits = dwLNumBits;

	pBlock = (BYTE *)p->m_aBlock;

	while(dwSize){
		//dwReadSize = (dwSize < SHA256_BUFFER_SIZE - p->m_nNumChr) ?
		dwReadSize = (dwSize < SHA256_BUFFER_SIZE - (DWORD)p->m_nNumChr) ?
			dwSize :
			(SHA256_BUFFER_SIZE - p->m_nNumChr);
		
		memcpy(pBlock + p->m_nNumChr, pBuffer, dwReadSize);
		
		p->m_nNumChr += dwReadSize;
		pBuffer += dwReadSize;
		dwSize -= dwReadSize;
		
		if(p->m_nNumChr == SHA256_BUFFER_SIZE){
			Generate(p);
			p->m_nNumChr = 0;
		}
	}
}

void WINAPI dkcSHA256Final(DKC_SHA256 *p){

	BYTE cZero = 0x00;
	BYTE cOne  = 0x80;
	DWORD dwHNumBits;
	DWORD dwLNumBits;

	if(p->mFinalized){
		return;
	}

	dwHNumBits = ReverseEndian(p->m_dwHNumBits);
	dwLNumBits = ReverseEndian(p->m_dwLNumBits);

	dkcSHA256Load(p,&cOne, 1);
	while(p->m_nNumChr != SHA256_BUFFER_SIZE - 8) dkcSHA256Load(p,&cZero, 1);

	dkcSHA256Load(p,(BYTE *)&dwHNumBits, 4);
	dkcSHA256Load(p,(BYTE *)&dwLNumBits, 4);

	//ファイナル処理した。
	p->mFinalized = TRUE;
}

int WINAPI dkcSHA256DigestStr(DKC_SHA256 *p,char *buff,size_t size){
	//char s[SHA256_HASH * 8 + 1];
	char s[SHA256_STR_BUFFER_SIZE];
	int i;

	s[SHA256_HASH * 8]='\0';
	if(SHA256_STR_BUFFER_SIZE > size){
		return edk_BufferOverFlow;
	}
	if(FALSE==p->mFinalized){
		//まだFinalやってないっつーの
		return edk_LogicError;
	}

	for(i = 0; i < SHA256_HASH; i++){
		sprintf(s + i * 8, "%08x", p->m_dwH[i]);
	}
	//std::string strDigest = s;
	//return strDigest;
	return dkc_strcpy(buff,size,s,strlen(s));
}
int WINAPI dkcSHA256FinalDigestStr(DKC_SHA256 *p,char *buff,size_t size){
	dkcSHA256Final(p);
	return dkcSHA256DigestStr(p,buff,size);
}



int WINAPI dkcSHA256Digest(DKC_SHA256 *p,BYTE *buff,size_t size){
	size_t i;
	if(SHA256_BIN_BUFFER_SIZE > size){
		return edk_BufferOverFlow;
	}
	if(FALSE==p->mFinalized){
		//まだFinalやってないっつーの
		return edk_LogicError;
	}
	for(i = 0; i < SHA256_BIN_BUFFER_SIZE; ++i){
		buff[i] = (BYTE)(p->m_dwH[i >> 2] >> (8 * (~i & 3)));
	}
	return edk_SUCCEEDED;
	//return dkc_memcpy(buff,size,(const void *)p->m_dwH,sizeof(p->m_dwH));
}

int WINAPI dkcSHA256FinalDigest(DKC_SHA256 *p,BYTE *buff,size_t size){
	dkcSHA256Final(p);
	return dkcSHA256Digest(p,buff,size);
}


int WINAPI dkcFreeSHA256(DKC_SHA256 **p){
	if(NULL==p){
		return edk_FAILED;
	}
	return dkcFree((void **)p);
}