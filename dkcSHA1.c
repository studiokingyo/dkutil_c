
/*!
@file dkcSHA1.c
@brief SHA 1
@author original : General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.
        reconstruct : d金魚
*/

#define DKUTIL_C_SHA1_C
#include "dkcSHA1.h"
#include "dkcStdio.h"


#define SHA_BUFFER_SIZE (SHA_BLOCK * 4)


const static DWORD c_dwInitH0 = 0x67452301;
const static DWORD c_dwInitH1 = 0xefcdab89;
const static DWORD c_dwInitH2 = 0x98badcfe;
const static DWORD c_dwInitH3 = 0x10325476;
const static DWORD c_dwInitH4 = 0xc3d2e1f0;

const static DWORD c_dwK_00_19 = 0x5a827999;
const static DWORD c_dwK_20_39 = 0x6ed9eba1;
const static DWORD c_dwK_40_59 = 0x8f1bbcdc;
const static DWORD c_dwK_60_79 = 0xca62c1d6;

static DKC_INLINE DWORD ReverseEndian(DWORD dwValue)
{
	//return (dwValue << 24) | ((dwValue & 0x0000ff00) << 8) | ((dwValue & 0x00ff0000) >> 8) | (dwValue >> 24);
	return dkcReverseEndian32(dwValue);
}

static DKC_INLINE DWORD Rotate(DWORD dwValue, DWORD dwNum)
{
	return (dwValue << dwNum) | (dwValue >> (32 - dwNum));
}


static DKC_INLINE void Generate(DKC_SHA1 *p)
{
	int i;
	DWORD Work[SHA_WORK];
	DWORD Hash[SHA_WORK + SHA_HASH];
	DWORD *pHash;

	for(i = 0; i < SHA_BLOCK; i++) Work[i] = ReverseEndian(p->m_aBlock[i]);
	for(i = SHA_BLOCK; i < SHA_WORK; i++){
		Work[i] = Rotate(Work[i - 3] ^ Work[i - 8] ^ Work[i - 14] ^ Work[i - 16], 1);
	}

	for(i = 0; i < 5; i++) Hash[SHA_WORK + i] = p->m_dwH[i];
	pHash = &Hash[SHA_WORK];

	for(i = 0; i < 20; i++){
		pHash--;
		pHash[0] = (pHash[2] & (pHash[3] ^ pHash[4])) ^ pHash[4];
		pHash[0] += Rotate(pHash[1], 5) + pHash[5] + Work[i] + c_dwK_00_19;
		pHash[2] = Rotate(pHash[2], 30);
	}
	for(i = 20; i < 40; i++){
		pHash--;
		pHash[0] = pHash[2] ^ pHash[3] ^ pHash[4];
		pHash[0] += Rotate(pHash[1], 5) + pHash[5] + Work[i] + c_dwK_20_39;
		pHash[2] = Rotate(pHash[2], 30);
	}
	for(i = 40; i < 60; i++){
		pHash--;
		pHash[0] = (pHash[2] & (pHash[3] | pHash[4])) | (pHash[3] & pHash[4]);
		pHash[0] += Rotate(pHash[1], 5) + pHash[5] + Work[i] + c_dwK_40_59;
		pHash[2] = Rotate(pHash[2], 30);
	}
	for(i = 60; i < 80; i++){
		pHash--;
		pHash[0] = pHash[2] ^ pHash[3] ^ pHash[4];
		pHash[0] += Rotate(pHash[1], 5) + pHash[5] + Work[i] + c_dwK_60_79;
		pHash[2] = Rotate(pHash[2], 30);
	}

	for(i = 0; i < 5; i++) p->m_dwH[i] += pHash[i];
}

DKC_SHA1 *WINAPI dkcAllocSHA1(){
	DKC_SHA1 *p = (DKC_SHA1 *)dkcAllocate(sizeof(DKC_SHA1));
	if(NULL==p){
		return NULL;
	}
	dkcSHA1Init(p);
	return p;
}

void WINAPI dkcSHA1Init(DKC_SHA1 *p){
	p->m_dwH[0] = c_dwInitH0;
	p->m_dwH[1] = c_dwInitH1;
	p->m_dwH[2] = c_dwInitH2;
	p->m_dwH[3] = c_dwInitH3;
	p->m_dwH[4] = c_dwInitH4;
	p->m_dwLNumBits = 0;
	p->m_dwHNumBits = 0;
	p->m_nNumChr = 0;

	p->mFinalized = FALSE;
}

void WINAPI dkcSHA1Load(DKC_SHA1 *p,const BYTE *pBuffer,DWORD dwSize){
	DWORD dwLNumBits;
	BYTE *pBlock;
	DWORD dwReadSize;
	
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
		//dwReadSize = (dwSize < SHA_BUFFER_SIZE - p->m_nNumChr) ?
		dwReadSize = (dwSize < SHA_BUFFER_SIZE - (DWORD)p->m_nNumChr) ?
			dwSize :
			(SHA_BUFFER_SIZE - p->m_nNumChr);

		memcpy(pBlock + p->m_nNumChr, pBuffer, dwReadSize);

		p->m_nNumChr += dwReadSize;
		pBuffer += dwReadSize;
		dwSize -= dwReadSize;

		if(p->m_nNumChr == SHA_BUFFER_SIZE){
			Generate(p);
			p->m_nNumChr = 0;
		}
	}

}

void WINAPI dkcSHA1Final(DKC_SHA1 *p){
	BYTE cZero = 0x00;
	BYTE cOne  = 0x80;
	DWORD dwHNumBits;
	DWORD dwLNumBits;
	if(p->mFinalized){
		return ;
	}

	dwHNumBits = ReverseEndian(p->m_dwHNumBits);
	dwLNumBits = ReverseEndian(p->m_dwLNumBits);

	dkcSHA1Load(p,&cOne, 1);
	while(p->m_nNumChr != SHA_BUFFER_SIZE - 8) dkcSHA1Load(p,&cZero, 1);

	dkcSHA1Load(p,(BYTE *)&dwHNumBits, 4);
	dkcSHA1Load(p,(BYTE *)&dwLNumBits, 4);
	
	//ファイナル処理した。
	p->mFinalized = TRUE;

}

int WINAPI dkcSHA1DigestStr(DKC_SHA1 *p,char *buff,size_t size){

	//char s[SHA_HASH * 8 + 1];
	char s[SHA1_STR_BUFFER_SIZE];
	int i;
	//s[SHA_HASH * 8]='\0';
	s[SHA1_STR_BUFFER_SIZE]='\0';

	if(SHA1_STR_BUFFER_SIZE > size){
		return edk_BufferOverFlow;
	}
	if(FALSE==p->mFinalized){
		//まだFinalやってないっつーの
		return edk_LogicError;
	}

	for( i = 0; i < SHA_HASH; i++){
		sprintf(s + i * 8, "%08x", p->m_dwH[i]);
	}
	//std::string strDigest = s;
	//return strDigest;
	return dkc_strcpy(buff,size,s,strlen(s));
}


int WINAPI dkcSHA1FinalDigestStr(DKC_SHA1 *p,char *buff,size_t size){
	dkcSHA1Final(p);
	return dkcSHA1DigestStr(p,buff,size);
}

int WINAPI dkcSHA1Digest(DKC_SHA1 *p,BYTE *buff,size_t size){
	size_t i = 0;
	//DWORD *dwp = (DWORD *)buff;
	//char s[SHA1_STR_BUFFER_SIZE];
	//s[SHA1_STR_BUFFER_SIZE]='\0';

	if(SHA1_BIN_BUFFER_SIZE > size){
		return edk_BufferOverFlow;
	}
	if(FALSE==p->mFinalized){
		//まだFinalやってないっつーの
		return edk_LogicError;
	}
	/*
	for( i = 0; i < SHA_HASH; i++){
		sprintf(s + i * 8, "%08x", p->m_dwH[i]);
		dwp[i] = atoi(s + i * 8);
	}*/
	for(i = 0; i < SHA1_BIN_BUFFER_SIZE; ++i){
		buff[i] = (BYTE)(p->m_dwH[i >> 2] >> (8 * (~i & 3)));
	}
	return edk_SUCCEEDED;
	//ナにバカやってんだ私。
	//return dkc_memcpy(buff,size,(const void *)p->m_dwH,sizeof(p->m_dwH));
}


int WINAPI dkcSHA1FinalDigest(DKC_SHA1 *p,BYTE *buff,size_t size){
	dkcSHA1Final(p);
	return dkcSHA1Digest(p,buff,size);
}

int WINAPI dkcFreeSHA1(DKC_SHA1 **p){
	if(NULL==p){
		return edk_FAILED;
	}
	return dkcFree((void **)p);
}