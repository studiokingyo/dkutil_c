
/**
@author d金魚
@file dkcHMAC.c
@brief HMAC: Keyed-Hashing for Message Authentication

*/

#include "dkcHMAC.h"
#include "dkcStdio.h"


#define dkcdHMAC_IMPL_STRING 0

/*
void WINAPI dkcHMAC_MD5Init(DKC_HMAC *p){


	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcMD5Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcMD5Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcMD5Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcMD5DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcMD5Digest;
	p->ipad_init = 0x36;
	p->opad_init = 0x5c;
	p->digest_string_size = MD5_STR_BUFFER_SIZE;
	p->digest_binary_size = MD5_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocMD5();
}


void WINAPI dkcHMAC_SHA1Init(DKC_HMAC *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1Digest;
	p->ipad_init = 0x36;
	p->opad_init = 0x5c;
	p->digest_string_size = SHA1_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA1_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA1();
}


void WINAPI dkcHMAC_SHA256Init(DKC_HMAC *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256Digest;
	p->ipad_init = 0x36;
	p->opad_init = 0x5c;
	p->digest_string_size = SHA256_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA256_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA256();
}


void WINAPI dkcHMAC_SHA384Init(DKC_HMAC *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384Digest;

	p->digest_string_size = SHA384_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA384_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA384();
}


///これによるHMACの生成を奨励する。
void WINAPI dkcHMAC_SHA512Init(DKC_HMAC *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512Digest;
	p->ipad_init = 0x36;
	p->opad_init = 0x5c;
	p->digest_string_size = SHA512_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA512_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA512();

}
*/
DKC_HMAC *WINAPI dkcAllocHMAC(UINT option)
{
	DKC_HMAC *p;

	size_t padsize ;

	//check_init();

	p = dkcAllocate(sizeof(DKC_HMAC));
	if(NULL==p){
		return NULL;
	}

	p->sho = dkcAllocSHO(option);
	if(NULL==p->sho){
		goto Error;
	}



	//初期化した値から必要なパッドを確保
	p->pad_size = p->sho->digest_binary_size * 4;

	padsize = p->pad_size + 1;// + 1は文字列なので･･･

	p->ipad_init = 0x36;
	p->opad_init = 0x5c;
	
	p->ipad = malloc(	padsize );
	if(NULL==p->ipad){
		goto Error;
	}
	p->opad = malloc( padsize );
	if(NULL==p->opad){
		goto Error;
	}

	
	return p;
Error:
	dkcFreeHMAC(&p);
	return NULL;
}

int WINAPI dkcFreeHMAC(DKC_HMAC **pp)
{
	DKC_HMAC *p = *pp;

	if(NULL==pp || NULL==p){
		return edk_FAILED;
	}

	dkcFreeSHO(&(p->sho));
	
	if(p->ipad){
		free(p->ipad);
	}
	if(p->opad){
		free(p->opad);
	}
	return dkcFree(pp);
}

void WINAPI dkcHMACInit(DKC_HMAC *p,const BYTE *key,size_t key_length)
{

	size_t padsize = p->pad_size;

	if (TRUE==p->mInnerHashKeyed)
	{
		//中身のハッシュ値を初期化
		dkcSHOInit(p->sho);
		//成功＾＾ 中身はないよフラグをする。
		p->mInnerHashKeyed = FALSE;
	}

	

	//パッド作り
	if(key_length <= padsize)
	{
		memcpy(p->ipad,key,key_length);
	}
	else
	{

#if dkcdHMAC_IMPL_STRING //文字列版
		dkcSecureHashCalculateStringDigest(
			p->sho->mOption,
			(char *)p->ipad,p->pad_size,
			key,key_length
		);
		//update key_length
		key_length = p->sho->digest_string_size;
		dkcmASSERT(key_length <= p->sho->digest_string_size);

#else //binary版
	dkcSecureHashCalculateBinaryDigest(
			p->sho->mOption,
			p->ipad,p->pad_size,
			key,key_length
		);
		//update key_length
		key_length = p->sho->digest_binary_size;
		dkcmASSERT(key_length <= p->sho->digest_binary_size);
#endif
	}


	dkcmASSERT(key_length <= padsize);
	//バッファの埋め合わせ
	memset(p->ipad + key_length, 0, padsize - key_length);
		
	//pad初期化
	{
		BYTE *ipad,*opad,iv,ov;
		size_t i;

		ipad = p->ipad;
		opad = p->opad;
		iv = p->ipad_init;
		ov = p->opad_init;
		
		for (i=0; i<padsize; i++)
		{

			opad[i] = (BYTE)(ipad[i] ^ ov);
			ipad[i] ^= iv;
		}

	}

	p->mInited = 1;

}

///ipadをハッシュ関数に通過させる処理
void DKC_INLINE WINAPI dkcHMACKeyInner(DKC_HMAC *p)
{
	dkcmASSERT(!p->mInnerHashKeyed);
	dkcSHOLoad(p->sho,p->ipad,p->pad_size);

	p->mInnerHashKeyed = TRUE;
}


void WINAPI dkcHMACLoad(DKC_HMAC *p,const BYTE *pBuffer,DWORD dwSize)
{
	dkcmASSERT(1==p->mInited);

	if(FALSE ==	p->mInnerHashKeyed){
		dkcHMACKeyInner(p);
	}
	dkcSHOLoad(p->sho,pBuffer,dwSize);
}



int WINAPI dkcHMACFinal(DKC_HMAC *p){
	//ThrowIfInvalidTruncatedSize(size);

#if dkcdHMAC_IMPL_STRING

	size_t tempsize = p->pad_size;
	char *temp = malloc(tempsize);

#else
	size_t tempsize = p->pad_size / 4;
	char *temp = malloc(tempsize);

#endif
	DKC_SECURE_HASH_OBJECT *pt = p->sho;
	dkcmNOT_ASSERT(FALSE==p->mInited);

	if(NULL==temp){
		return edk_FAILED;
	}
	if(FALSE ==	p->mInnerHashKeyed){
		dkcHMACKeyInner(p);
	}

	//End of inner process
	dkcSHOFinal(pt);
#if dkcdHMAC_IMPL_STRING
	dkcSHODigestStr(pt,temp,tempsize);
#else
	dkcSHODigest(pt,(BYTE *)temp,tempsize);
#endif

	//begin outer process
	dkcSHOInit(pt);
	dkcSHOLoad(pt,p->opad,p->pad_size);
	dkcSHOLoad(pt,(const BYTE *)temp,tempsize);

	dkcSHOFinal(pt);

	//ココバグっているかも？
	//p->mInnerHashKeyed = FALSE;
	//state を 2にする
	p->mInited = 2;

	free(temp); 
	return edk_SUCCEEDED;
}

int WINAPI dkcHMACDigestStr(DKC_HMAC *p,char *buff,size_t size)
{
	dkcmASSERT(2==p->mInited);
	return dkcSHODigestStr(p->sho,buff,size);
}


int WINAPI dkcHMACDigest(DKC_HMAC *p,BYTE *buff,size_t size)
{
	dkcmASSERT(2==p->mInited);
	return dkcSHODigest(p->sho,buff,size);
}




int WINAPI dkcHMACStr(DKC_HMAC *p,const BYTE *data,size_t data_size,const BYTE *key,size_t key_size,
						 char *str_result_buff,size_t buffsize)
{
	int r;

	dkcHMACInit(p,key,key_size);
	dkcHMACLoad(p,data,data_size);
	r = dkcHMACFinal(p);
	if(DKUTIL_FAILED(r)) return r;
	r = dkcHMACDigestStr(p,str_result_buff,buffsize);
	//if(DKUTIL_FAILED(r)) return r;
	return r;
}
int WINAPI dkcHMAC(DKC_HMAC *p,const BYTE *data,size_t data_size,const BYTE *key,size_t key_size,
						 BYTE *bin_result_buff,size_t buffsize)
{
	int r;

	dkcHMACInit(p,key,key_size);
	dkcHMACLoad(p,data,data_size);
	r = dkcHMACFinal(p);
	if(DKUTIL_FAILED(r)) return r;
	r = dkcHMACDigest(p,bin_result_buff,buffsize);
	//if(DKUTIL_FAILED(r)) return r;
	return r;
}

int WINAPI dkcHMACCalculateBinaryDigest(
	UINT hash_option,BYTE *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size,const BYTE *key,size_t keysize
){
	int r;
	DKC_HMAC *p = dkcAllocHMAC(hash_option);
	if(NULL==p) return edk_FAILED;
	r = dkcHMAC(p,data,data_size,key,keysize,digest_dest,destsize);
	dkcFreeHMAC(&p);
	return r;


}

int WINAPI dkcHMACHashCalculateStringDigest(
	UINT hash_option,char *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size,const BYTE *key,size_t keysize
	){
	int r;
	DKC_HMAC *p = dkcAllocHMAC(hash_option);
	if(NULL==p) return edk_FAILED;
	r = dkcHMACStr(p,data,data_size,key,keysize,digest_dest,destsize);
	dkcFreeHMAC(&p);
	return r;
}

int WINAPI dkcHMACFinalDigestStr(DKC_HMAC *p,char *buff,size_t size){
	dkcHMACFinal(p);
	return dkcHMACDigestStr(p,buff,size);
}

int WINAPI dkcHMACFinalDigest(DKC_HMAC *p,BYTE *buff,size_t size){
	dkcHMACFinal(p);
	return dkcHMACDigest(p,buff,size);

}
