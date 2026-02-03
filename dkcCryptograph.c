
/*!
@file dkcCryptograph.c
@brief Cryptograph algorithm
@since 2004/07/11

*/
#define DKUTIL_C_CRYPTOGRAPH_C
#include "dkcCryptograph.h"






BYTE WINAPI dkcEncryptByte(BYTE a,BYTE key){
	BYTE x = 0,y = 0;

	x |= (BYTE)((a & 0x0f) << 4);
	x |= (BYTE)((a & 0xf0) >> 4);

	y |= (BYTE)((x & 0x33) << 2);
	y |= (BYTE)((x & 0xCC) >> 2);

	y ^= key;//0xaa;
	
	return y;
}

BYTE WINAPI dkcDecryptByte(BYTE a,BYTE key){
	BYTE x = 0,y = 0;
	a ^= key;//0xaa;

	x |= (BYTE)((a & 0x33) << 2);
	x |= (BYTE)((a & 0xCC) >> 2);

	y |= (BYTE)((x & 0x0f) << 4);
	y |= (BYTE)((x & 0xf0) >> 4);

	return y;
}

DKC_INLINE int WINAPI dkcSNOW2GetInitializeVector(const void *key,size_t keysize,
																									 	uint32 *iv3,uint32 *iv2,uint32 *iv1,uint32 *iv0)

{	int r ;//= edk_FAILED;
	uint8 key160[SHA160_BIN_BUFFER_SIZE];
	r = dkcSecureHashCalculateBinaryDigest(edkcSH_SHA160,key160,sizeof(key160),key,keysize);
	if(DKUTIL_FAILED(r)) return r;
	//todo endian check
	/**iv0 = *((uint32 *)key160);
	*iv1 = *((uint32 *)&(key160[4]));
	*iv2 = *((uint32 *)&(key160[8]));
	*iv3 = *((uint32 *)&(key160[12]));
	*/
	memcpy(iv0,key160,sizeof(iv0));
	memcpy(iv1,&(key160[4]),sizeof(iv1));
	memcpy(iv2,&(key160[8]),sizeof(iv2));
	memcpy(iv3,&(key160[12]),sizeof(iv3));

	if(dkcIsBigEndian()){
		*iv0 = dkcReverseEndian32(*iv0);
		*iv1 = dkcReverseEndian32(*iv1);
		*iv2 = dkcReverseEndian32(*iv2);
		*iv3 = dkcReverseEndian32(*iv3);
	}//else if(is
	return r;
}

DKC_INLINE DKC_SNOW2 *WINAPI dkcAllocSNOW2NoLimitKeyLength(const void *key,size_t keysize)
{
	int r ;//= edk_FAILED;

	uint8 key256[SHA256_BIN_BUFFER_SIZE];
	
	uint32 iv0=0,iv1=0,iv2=0,iv3=0;

	r = dkcSNOW2GetInitializeVector(key,keysize,&iv3,&iv2,&iv1,&iv0);
	if(DKUTIL_FAILED(r)) return NULL;
	r = dkcSecureHashCalculateBinaryDigest(edkcSH_SHA256,key256,sizeof(key256),key,keysize);
	if(DKUTIL_FAILED(r)) return NULL;


	return dkcAllocSNOW2(key256,sizeof(key256),iv3,iv2,iv1,iv0);


}


DKC_INLINE DKC_HC256 *WINAPI dkcAllocHC256NoLimitKeyLength(const void *key,size_t keysize)
{
	int r ;//= edk_FAILED;

	uint8 key256[SHA256_BIN_BUFFER_SIZE];
	uint8 iv[dkcdHC256_IV_SIZE];
	r = dkcSecureHashCalculateBinaryDigest(edkcSH_SHA256,key256,sizeof(key256),key,keysize);
	if(DKUTIL_FAILED(r)) return NULL;
	r = dkcCalculateInitializeVector(iv,sizeof(iv),key,keysize);
	if(DKUTIL_FAILED(r)) return NULL;
	return dkcAllocHC256(key256,sizeof(key256),(uint32 *)iv,sizeof(iv));
}

DKC_INLINE int WINAPI dkcCalculateInitializeVector(
	void *dest,size_t destsize_and_generate_iv_size,const void *key,size_t keysize)
{
	uint8 *a = dest;
	int r;
	size_t count,rest,i,j;
	DKC_SHA512 *p;
	uint8 key512[SHA512_BIN_BUFFER_SIZE];
	r = dkcSecureHashCalculateBinaryDigest(
		edkcSH_SHA512,key512,sizeof(key512),key,keysize);
	if(DKUTIL_FAILED(r)) return edk_FAILED;
	
	p = dkcAllocSHA512();
	if(NULL==p) return edk_OutOfMemory;
	
	//初期ペンティアムは勘弁TT
	count = destsize_and_generate_iv_size / SHA512_BIN_BUFFER_SIZE;
	rest = destsize_and_generate_iv_size % SHA512_BIN_BUFFER_SIZE;
	for(i=0;i<count;i++){
		dkcSHA512Init(p);
		dkcSHA512Load(p,key512,sizeof(key512));
		dkcSHA512Load(p,key,keysize);
		dkcSHA512Load(p,key512,sizeof(key512));
		dkcSHA512FinalDigest(p,key512,sizeof(key512));
		for(j=0;j<SHA512_BIN_BUFFER_SIZE;j++){
			a[i] = key512[i];
		}
		a+=SHA512_BIN_BUFFER_SIZE;
	}
	if(rest){
		dkcSHA512Init(p);
		dkcSHA512Load(p,key512,sizeof(key512));
		dkcSHA512Load(p,key,keysize);
		dkcSHA512Load(p,key512,sizeof(key512));
		dkcSHA512FinalDigest(p,key512,sizeof(key512));
		for(j=0;j<rest;j++){
			a[i] = key512[i];
		}
	}
	dkcFreeSHA512(&p);
	return edk_SUCCEEDED;
}

//**********************************************************
