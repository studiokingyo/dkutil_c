
/*!
@file dkcSHA384.c
@brief SHA 384
@author d金魚
*/
#define DKUTIL_C_SHA384_C
#include "dkcSHA384.h"
#include "dkcStdio.h"



DKC_SHA384 *WINAPI dkcAllocSHA384(){
	DKC_SHA384 *p = dkcAllocate(sizeof(DKC_SHA384));
	if(NULL==p){
		return NULL;
	}
	dkcSHA384Init(p);
	return p;
}

void WINAPI dkcSHA384Init(DKC_SHA384 *p){

	p->m_qwH[0] = 0xcbbb9d5dc1059ed8;
  p->m_qwH[1] = 0x629a292a367cd507;
  p->m_qwH[2] = 0x9159015a3070dd17;
  p->m_qwH[3] = 0x152fecd8f70e5939;
  p->m_qwH[4] = 0x67332667ffc00b31;
  p->m_qwH[5] = 0x8eb44a8768581511;
  p->m_qwH[6] = 0xdb0c2e0d64f98fa7;
  p->m_qwH[7] = 0x47b5481dbefa4fa4;
	p->m_qwLNumBits = 0;
	p->m_qwHNumBits = 0;
	p->m_nNumChr = 0;

	p->mFinalized = FALSE;
}

void WINAPI dkcSHA384Load(DKC_SHA384 *p,const BYTE *pBuffer,DWORD dwSize){
	dkcSHA512Load(p,pBuffer,dwSize);
}

void WINAPI dkcSHA384Final(DKC_SHA384 *p){
	dkcSHA512Final(p);
	/*BYTE cZero = 0x00;
	BYTE cOne  = 0x80;
	DWORD dwHNumBits;
	DWORD dwLNumBits;
	if(p->mFinalized){
		return ;
	}

	dwHNumBits = ReverseEndian(p->m_dwHNumBits);
	dwLNumBits = ReverseEndian(p->m_dwLNumBits);

	dkcSHA384Load(p,&cOne, 1);
	while(p->m_nNumChr != SHA_BUFFER_SIZE - 8) dkcSHA384Load(p,&cZero, 1);

	dkcSHA384Load(p,(BYTE *)&dwHNumBits, 4);
	dkcSHA384Load(p,(BYTE *)&dwLNumBits, 4);
	
	//ファイナル処理した。
	p->mFinalized = TRUE;
	*/

}

int WINAPI dkcSHA384DigestStr(DKC_SHA384 *p,char *buff,size_t size){

	//char s[SHA_HASH * 8 + 1];
	char s[SHA384_STR_BUFFER_SIZE];
	//BYTE *tb;
	int i;
	//s[SHA_HASH * 8]='\0';
	s[SHA384_STR_BUFFER_SIZE]='\0';

	if(SHA384_STR_BUFFER_SIZE > size){
		return edk_BufferOverFlow;
	}
	if(FALSE==p->mFinalized){
		//まだFinalやってないっつーの
		return edk_LogicError;
	}

	/*for( i = 0; i < SHA384_HASH; i++){
		sprintf(s + i * 12,     "%06x", (DWORD)(p->m_qwH[i] >> 32));
		sprintf(s + i * 12 + 6, "%06x", (DWORD)(p->m_qwH[i] & 0x00000000ffffffff));
	}*/
	//tb = (BYTE *)p->m_qwH;
	for( i=0;i<SHA384_BIN_BUFFER_SIZE;i++){
		//sprintf(&s[SHA384_HASH * i],"%0x",p->m_qwH[i]);

		//sprintf(s + i * 12,     "%0x", (DWORD)(p->m_qwH[i] >> 32));
		//sprintf(s + i * 12 + 6, "%0x", (DWORD)(p->m_qwH[i] & 0x00000000ffffffff));
		sprintf(s + i * 2,     "%02x",  (BYTE)(p->m_qwH[i >> 3] >> (8 * (~i & 7))));
		//sprintf(s + i * 16 + 8, "%08x", (DWORD)(p->m_qwH[i] & 0x00000000ffffffff));
	}

	//std::string strDigest = s;
	//return strDigest;
	return dkc_strcpy(buff,size,s,strlen(s));
}


int WINAPI dkcSHA384FinalDigestStr(DKC_SHA384 *p,char *buff,size_t size){
	dkcSHA384Final(p);
	return dkcSHA384DigestStr(p,buff,size);
}

int WINAPI dkcSHA384Digest(DKC_SHA384 *p,BYTE *buff,size_t size){
	ULONG i = 0;
	//DWORD *dwp = (DWORD *)buff;
	//char s[SHA384_STR_BUFFER_SIZE];
	//s[SHA384_STR_BUFFER_SIZE]='\0';

	if(SHA384_BIN_BUFFER_SIZE > size){
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
	for(i = 0; i < SHA384_BIN_BUFFER_SIZE; ++i){
		//buff[i] = (BYTE)(p->m_dwH[i >> 2] >> (8 * (~i & 3)));
		buff[i] = (BYTE)(p->m_qwH[i >> 3] >> (8 * (~i & 7)));
	}
	return edk_SUCCEEDED;
	//ナにバカやってんだ私。
	//return dkc_memcpy(buff,size,(const void *)p->m_dwH,sizeof(p->m_dwH));
}


int WINAPI dkcSHA384FinalDigest(DKC_SHA384 *p,BYTE *buff,size_t size){
	dkcSHA384Final(p);
	return dkcSHA384Digest(p,buff,size);
}

int WINAPI dkcFreeSHA384(DKC_SHA384 **p){
	if(NULL==p){
		return edk_FAILED;
	}
	return dkcFree((void **)p);
}