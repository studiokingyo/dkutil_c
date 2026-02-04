
/*!
@file dkcHash.c
@brief lots of Hash Algorithm

*/

#include "dkcHash.h"
#include "dkcStdio.h"


void WINAPI dkcSHO_MD2Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcMD2Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcMD2Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcMD2Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcMD2DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcMD2Digest;

	p->digest_string_size = MD2_STR_BUFFER_SIZE;
	p->digest_binary_size = MD2_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocMD2();


}

void WINAPI dkcSHO_MD4Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcMD4Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcMD4Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcMD4Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcMD4DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcMD4Digest;

	p->digest_string_size = MD4_STR_BUFFER_SIZE;
	p->digest_binary_size = MD4_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocMD4();


}

void WINAPI dkcSHO_MD5Init(DKC_SECURE_HASH_OBJECT *p){


	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcMD5Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcMD5Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcMD5Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcMD5DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcMD5Digest;

	p->digest_string_size = MD5_STR_BUFFER_SIZE;
	p->digest_binary_size = MD5_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocMD5();
}


void WINAPI dkcSHO_SHA1Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA1Digest;

	p->digest_string_size = SHA1_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA1_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA1();
}


void WINAPI dkcSHO_SHA256Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA256Digest;

	p->digest_string_size = SHA256_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA256_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA256();
}


void WINAPI dkcSHO_SHA384Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA384Digest;

	p->digest_string_size = SHA384_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA384_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA384();
}


void WINAPI dkcSHO_SHA3_224Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_224Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_224Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_224Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_224DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_224Digest;

	p->digest_string_size = SHA3_224_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA3_224_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA3_224();
}

void WINAPI dkcSHO_SHA3_256Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_256Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_256Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_256Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_256DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_256Digest;

	p->digest_string_size = SHA3_256_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA3_256_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA3_256();
}

void WINAPI dkcSHO_SHA3_384Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_384Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_384Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_384Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_384DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_384Digest;

	p->digest_string_size = SHA3_384_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA3_384_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA3_384();
}

void WINAPI dkcSHO_SHA3_512Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_512Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_512Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_512Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_512DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA3_512Digest;

	p->digest_string_size = SHA3_512_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA3_512_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA3_512();
}

void WINAPI dkcSHO_BLAKE224Init(DKC_SECURE_HASH_OBJECT *p){
	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE224Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE224Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE224Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE224DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE224Digest;
	p->digest_string_size = BLAKE224_STR_BUFFER_SIZE;
	p->digest_binary_size = BLAKE224_BIN_BUFFER_SIZE;
	p->mObj = dkcAllocBLAKE224();
}

void WINAPI dkcSHO_BLAKE256Init(DKC_SECURE_HASH_OBJECT *p){
	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE256Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE256Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE256Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE256DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE256Digest;
	p->digest_string_size = BLAKE256_STR_BUFFER_SIZE;
	p->digest_binary_size = BLAKE256_BIN_BUFFER_SIZE;
	p->mObj = dkcAllocBLAKE256();
}

void WINAPI dkcSHO_BLAKE384Init(DKC_SECURE_HASH_OBJECT *p){
	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE384Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE384Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE384Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE384DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE384Digest;
	p->digest_string_size = BLAKE384_STR_BUFFER_SIZE;
	p->digest_binary_size = BLAKE384_BIN_BUFFER_SIZE;
	p->mObj = dkcAllocBLAKE384();
}

void WINAPI dkcSHO_BLAKE512Init(DKC_SECURE_HASH_OBJECT *p){
	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE512Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE512Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE512Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE512DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE512Digest;
	p->digest_string_size = BLAKE512_STR_BUFFER_SIZE;
	p->digest_binary_size = BLAKE512_BIN_BUFFER_SIZE;
	p->mObj = dkcAllocBLAKE512();
}

void WINAPI dkcSHO_BLAKE2sInit(DKC_SECURE_HASH_OBJECT *p){
	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2sInit;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2sLoad;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2sFinal;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2sDigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2sDigest;
	p->digest_string_size = BLAKE2S_STR_BUFFER_SIZE;
	p->digest_binary_size = BLAKE2S_BIN_BUFFER_SIZE;
	p->mObj = dkcAllocBLAKE2s();
}

void WINAPI dkcSHO_BLAKE2bInit(DKC_SECURE_HASH_OBJECT *p){
	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2bInit;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2bLoad;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2bFinal;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2bDigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE2bDigest;
	p->digest_string_size = BLAKE2B_STR_BUFFER_SIZE;
	p->digest_binary_size = BLAKE2B_BIN_BUFFER_SIZE;
	p->mObj = dkcAllocBLAKE2b();
}

void WINAPI dkcSHO_BLAKE3Init(DKC_SECURE_HASH_OBJECT *p){
	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE3Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE3Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE3Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE3DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcBLAKE3Digest;
	p->digest_string_size = BLAKE3_STR_BUFFER_SIZE;
	p->digest_binary_size = BLAKE3_BIN_BUFFER_SIZE;
	p->mObj = dkcAllocBLAKE3();
}

///����ɂ��HMAC�̐��������シ��B
void WINAPI dkcSHO_SHA512Init(DKC_SECURE_HASH_OBJECT *p){

	p->Init = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512Init;
	p->Load = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512Load;
	p->Final = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512Final;
	p->DigestStr = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512DigestStr;
	p->Digest = (DKC_WINAPI_PROC_F_TYPE)dkcSHA512Digest;

	p->digest_string_size = SHA512_STR_BUFFER_SIZE;
	p->digest_binary_size = SHA512_BIN_BUFFER_SIZE;

	p->mObj = dkcAllocSHA512();

}
static DKC_INLINE void check_init(){
	dkcmFORCE_NOT_ASSERT(sizeof(void *) != sizeof(DKC_SHA1 *));
}

#define check_() dkcmNOT_ASSERT(sizeof(void *) != sizeof(DKC_SHA1 *));

typedef void (WINAPI *DKC_SHO_HASH_INIT_F_TYPE)(DKC_SECURE_HASH_OBJECT *);


DKC_SECURE_HASH_OBJECT *WINAPI dkcAllocSHO(UINT option)
{
	DKC_SECURE_HASH_OBJECT *p;
	DKC_SHO_HASH_INIT_F_TYPE init_func;

	check_init();
	p = dkcAllocate(sizeof(DKC_SECURE_HASH_OBJECT));
	if(NULL==p){
		return NULL;
	}

	{
		switch(option)
		{
		case edkcSH_MD2:
			init_func = dkcSHO_MD2Init;
			break;
		case edkcSH_MD4:
			init_func = dkcSHO_MD4Init;
			break;
		case edkcSH_MD5:
			init_func = dkcSHO_MD5Init;
			break;
		case edkcSH_SHA1:
			init_func = dkcSHO_SHA1Init;
			break;
		case edkcSH_SHA256:
			init_func = dkcSHO_SHA256Init;
			break;
		case edkcSH_SHA384:
			init_func = dkcSHO_SHA384Init;
			break;
		case edkcSH_SHA512:
			init_func = dkcSHO_SHA512Init;
			break;
		case edkcSH_SHA3_224:
			init_func = dkcSHO_SHA3_224Init;
			break;
		case edkcSH_SHA3_256:
			init_func = dkcSHO_SHA3_256Init;
			break;
		case edkcSH_SHA3_384:
			init_func = dkcSHO_SHA3_384Init;
			break;
		case edkcSH_SHA3_512:
			init_func = dkcSHO_SHA3_512Init;
			break;
		case edkcSH_BLAKE224:
			init_func = dkcSHO_BLAKE224Init;
			break;
		case edkcSH_BLAKE256:
			init_func = dkcSHO_BLAKE256Init;
			break;
		case edkcSH_BLAKE384:
			init_func = dkcSHO_BLAKE384Init;
			break;
		case edkcSH_BLAKE512:
			init_func = dkcSHO_BLAKE512Init;
			break;
		case edkcSH_BLAKE2s:
			init_func = dkcSHO_BLAKE2sInit;
			break;
		case edkcSH_BLAKE2b:
			init_func = dkcSHO_BLAKE2bInit;
			break;
		case edkcSH_BLAKE3:
			init_func = dkcSHO_BLAKE3Init;
			break;
		default:
			goto Error;
		}
	}

	//�e�n�b�V���ɉ�����������
	init_func(p);
	if(NULL==p->mObj){
		goto Error;
	}
	p->mOption = option;
	//dkcAllocate()��memset(0)
	//p->mInited = 0;


	return p;
Error:
	dkcFreeSHO(&p);
	return NULL;
}

int WINAPI dkcFreeSHO(DKC_SECURE_HASH_OBJECT **pp)
{
	DKC_SECURE_HASH_OBJECT *p = *pp;
	int option;
	void **obj;

	if(NULL==pp || NULL==p){
		return edk_FAILED;
	}
	option = p->mOption;
	obj = &(p->mObj);
	
	switch(option)
	{
	case edkcSH_MD2:
		dkcFreeMD2((DKC_MD2 **)obj);
		break;
	case edkcSH_MD4:
		dkcFreeMD4((DKC_MD4 **)obj);
		break;
	case edkcSH_MD5:
		dkcFreeMD5((DKC_MD5 **)obj);
		break;
	case edkcSH_SHA1:
		dkcFreeSHA1((DKC_SHA1 **)obj);
		break;
	case edkcSH_SHA256:
		dkcFreeSHA256((DKC_SHA256 **)obj);
		break;
	case edkcSH_SHA384:
		dkcFreeSHA384((DKC_SHA384 **)obj);
		break;
	case edkcSH_SHA512:
		dkcFreeSHA512((DKC_SHA512 **)obj);
		break;
	case edkcSH_SHA3_224:
		dkcFreeSHA3_224((DKC_SHA3_224 **)obj);
		break;
	case edkcSH_SHA3_256:
		dkcFreeSHA3_256((DKC_SHA3_256 **)obj);
		break;
	case edkcSH_SHA3_384:
		dkcFreeSHA3_384((DKC_SHA3_384 **)obj);
		break;
	case edkcSH_SHA3_512:
		dkcFreeSHA3_512((DKC_SHA3_512 **)obj);
		break;
	case edkcSH_BLAKE224:
		dkcFreeBLAKE224((DKC_BLAKE224 **)obj);
		break;
	case edkcSH_BLAKE256:
		dkcFreeBLAKE256((DKC_BLAKE256 **)obj);
		break;
	case edkcSH_BLAKE384:
		dkcFreeBLAKE384((DKC_BLAKE384 **)obj);
		break;
	case edkcSH_BLAKE512:
		dkcFreeBLAKE512((DKC_BLAKE512 **)obj);
		break;
	case edkcSH_BLAKE2s:
		dkcFreeBLAKE2s((DKC_BLAKE2S **)obj);
		break;
	case edkcSH_BLAKE2b:
		dkcFreeBLAKE2b((DKC_BLAKE2B **)obj);
		break;
	case edkcSH_BLAKE3:
		dkcFreeBLAKE3((DKC_BLAKE3 **)obj);
		break;
	//default:

		
	}//eos
	
	return dkcFree(pp);
}

void WINAPI dkcSHOInit(DKC_SECURE_HASH_OBJECT *p)
{
	DKC_SHO_INIT_F_TYPE init = (DKC_SHO_INIT_F_TYPE)p->Init;
	init(p->mObj);

	p->mInited = TRUE;
}

void WINAPI dkcSHOLoad(DKC_SECURE_HASH_OBJECT *p,const BYTE *pBuffer,DWORD dwSize)
{
	DKC_SHO_LOAD_F_TYPE load = (DKC_SHO_LOAD_F_TYPE)p->Load;
	check_();
	if(FALSE ==	p->mInited){
		dkcSHOInit(p);
	}
	load(p->mObj,pBuffer,dwSize);
}



void WINAPI dkcSHOFinal(DKC_SECURE_HASH_OBJECT *p){
	DKC_SHO_FINAL_F_TYPE final = (DKC_SHO_FINAL_F_TYPE)p->Final;
	check_();
	final(p->mObj);
}

int WINAPI dkcSHODigestStr(DKC_SECURE_HASH_OBJECT *p,char *buff,size_t size)
{
	DKC_SHO_DIGESTSTR_F_TYPE digeststr = (DKC_SHO_DIGESTSTR_F_TYPE)p->DigestStr;
	check_();
	return digeststr(p->mObj,buff,size);
}


int WINAPI dkcSHODigest(DKC_SECURE_HASH_OBJECT *p,BYTE *buff,size_t size)
{
	DKC_SHO_DIGEST_F_TYPE digest = (DKC_SHO_DIGEST_F_TYPE)p->Digest;
	check_();
	return digest(p->mObj,buff,size);
}




static DKC_INLINE int shc(
	UINT hash_option,void *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size,dkctBOOL isBinary
	)
{
	int r = edk_FAILED;
	DKC_SECURE_HASH_OBJECT *p = dkcAllocSHO(hash_option);
	if(NULL==p){
		goto Error;
	}

	dkcSHOLoad(p,data,data_size);

	dkcSHOFinal(p);

	if(isBinary){
		r = dkcSHODigest(p,digest_dest,destsize);
	}else{//false
		r = dkcSHODigestStr(p,digest_dest,destsize);
	}
	if(DKUTIL_FAILED(r)){
		goto Error;
	}
	r = edk_SUCCEEDED;

Error:
	dkcFreeSHO(&p);
	return r;
}

int WINAPI dkcSecureHashCalculateBinaryDigest(
	UINT hash_option,BYTE *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size
	){

	return shc(hash_option,digest_dest,destsize,data,data_size,TRUE);
}

///������̏o�͂����� @see ���̑��������� dkcSecureHashCalculateBinaryDigest()
int WINAPI dkcSecureHashCalculateStringDigest(
	UINT hash_option,char *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size
){

	return shc(hash_option,digest_dest,destsize,data,data_size,FALSE);
}

//**********************************************************


uint8 dkcHash8(uint8 *pd,size_t cycle){
	uint32 v = 0;
	size_t i;
	for(i=0;i<cycle;i++){
		v ^= (pd[i] << 3) + v;
	}
	return (uint8)((v % dkcdUNSIGNED_8BIT_PRIME_NUM) & 0xFF);
}

uint16 dkcHash16(uint8 *pd,size_t cycle){
	uint32 v = 0;
	size_t i;
	for(i=0;i<cycle;i++){
		v = (pd[i] << 3) + v;
	}
	return (uint16)((v % dkcdUNSIGNED_16BIT_PRIME_NUM) & 0xFFFF);
}

uint32 dkcHash24(uint8 *pd,size_t cycle){
	uint32 v = 0;
	size_t i;
	for(i=0;i<cycle;i++){
		v ^= (pd[i] << 3) + v;
	}
	return (v % dkcdUNSIGNED_16BIT_PRIME_NUM) & 0xFFFFFF;
}