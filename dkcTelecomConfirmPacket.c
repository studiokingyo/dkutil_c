
/**
@file dkcTelecomConfirmPacket.c
@brief 通信のときに使う独自パケットフォーマット
@author d金魚
@since 2005/09/01
*/
#define  DKUTIL_C_TELECOM_CONFIRM_PACKET_C
#include "dkcTelecomConfirmPacket.h"

#if 0

#include "dkcMath.h"
#include "dkcLZW.h"

static DKC_INLINE ULONG all_add(uint8 *a,size_t size){
	size_t i=0;
	ULONG t=0;
	for(;i<size;i++){
		t += a[i];
	}
	return t;
}


DKC_TELECOM_CONFIRM_PACKET* WINAPI dkcAllocTelecomConfirmPacketAuto(const void *key,size_t keysize)
{
	return dkcAllocTelecomConfirmPacket(dkcdTELECOM_CONFIRM_PACKET_DEFAULT_PAUSE_SIZE,
		key,keysize,
		edk_SNOW2_SIGNATURE,
		FALSE
	);

}


//DKC_TELECOM_CONFIRM_PACKET* WINAPI dkcAllocTelecomConfirmPacket(size_t pause_size,const void *key,size_t keysize,BOOL isNaturalKey)

//DKC_TELECOM_CONFIRM_PACKET* WINAPI dkcAllocTelecomConfirmPacket(size_t pause_size,UINT flag,const void *key,size_t keysize,BOOL isNaturalKey)
DKC_EXTERN DKC_TELECOM_CONFIRM_PACKET* WINAPI dkcAllocTelecomConfirmPacket(size_t pause_size,const void *key,size_t keysize,UINT flag,BOOL isNaturalKey)
{
	DKC_TELECOM_CONFIRM_PACKET* p = dkcAllocate(sizeof(DKC_TELECOM_CONFIRM_PACKET));
	int r;
	if(NULL==p) return NULL;

	p->pause_size = pause_size;
	//p->pkey = NULL;
	//p->keysize = keysize;
	p->flag = flag;
	p->psig = dkcAllocSHA512();
	if(NULL==p->psig) goto Error;


	dkcSHA512Init(p->psig);
	dkcSHA512Load(p->psig,key,keysize);

	r = dkcSHA512FinalDigest(p->psig,p->key512,sizeof(p->key512));
	if(DKUTIL_FAILED(r)) goto Error;

	if(edk_ARCFOUR_SIGNATURE & flag){
		//todo endian check
		if(isNaturalKey){
			p->crypt_obj = dkcAllocArcfour(key,keysize);
		}else{
			p->crypt_obj = dkcAllocArcfour(p->key512,sizeof(p->key512));
		}
	}else if(edk_HC256_SIGNATURE & flag){
		//todo endian check
		if(isNaturalKey){
			p->crypt_obj = dkcAllocHC256NoLimitKeyLength(key,keysize);
		}else{
			p->crypt_obj = dkcAllocHC256NoLimitKeyLength(p->key512,sizeof(p->key512));
		}
	}else if(edk_SNOW20_SIGNATURE & flag){
		if(isNaturalKey){
			p->crypt_obj = dkcAllocSNOW2NoLimitKeyLength(key,keysize);
		}else{
			p->crypt_obj = dkcAllocSNOW2NoLimitKeyLength(p->key512,sizeof(p->key512));
		}
	}else if(edk_VERNAM_SIGNATURE & flag){
		dkcmNOT_ASSERT("NO IMPL");
	}


/*


	r = dkcSecureHashCalculateBinaryDigest(edkcSH_SHA160,p->key160,sizeof(p->key160),key,keysize);
	if(DKUTIL_FAILED(r)) goto Error;
	*/
	

	dkcSrand(&(p->seed),all_add(p->key512,sizeof(p->key512)));
	
	
	p->isNaturalKey = (uint8)(isNaturalKey != 0);
	return p;

Error:
	dkcFreeSHA512(&(p->psig));//NULLでは動かない
	dkcFree(&p);
	return NULL;
}

int WINAPI dkcFreeTelecomConfirmPacket(DKC_TELECOM_CONFIRM_PACKET **p){
	if(edk_ARCFOUR_SIGNATURE & (*p)->flag){
		dkcFreeArcfour((DKC_ARCFOUR_STATE **)&((*p)->crypt_obj));
	}else if(edk_HC256_SIGNATURE & (*p)->flag){
		dkcFreeHC256((DKC_HC256**)&((*p)->crypt_obj));
	}else if(edk_SNOW20_SIGNATURE & (*p)->flag){
		dkcFreeSNOW2((DKC_SNOW2**)&((*p)->crypt_obj));
	}else if(edk_VERNAM_SIGNATURE & (*p)->flag){
		dkcmNOT_ASSERT("NO IMPL");
	}

	dkcFreeSHA512(&((*p)->psig));
	return dkcFree(p);
}


///tcp = TelecomConfirmPacket little endian copy
static void copy_tcp_header(void *dest,DKC_TELECOM_CONFIRM_PACKET_HEADER *a){
	DKC_TELECOM_CONFIRM_PACKET_HEADER *p = dest;
	memcpy(	p->sig,a->sig,sizeof(p->sig));
	if(dkcIsBigEndian()){
		p->length = dkcReverseEndian32(a->length);
	}else{
		p->length = a->length;
	}
}

///little endianでコピーする
static void copy_lzw_header(void *dest,const DKC_LZW_HEADER *a){
	DKC_LZW_HEADER *p = dest;
	
	if(dkcIsBigEndian()){
		p->mSignature = dkcReverseEndian32(a->mSignature);
		p->mOriginSize = dkcReverseEndian32(a->mOriginSize);
		p->mCompressedSize = dkcReverseEndian32(a->mCompressedSize);
		p->option = dkcReverseEndian32(a->option);
	}else{
		memcpy(p,a,sizeof(*p));
	}
}

static DKC_INLINE int tcp_encode_pause(DKC_TELECOM_CONFIRM_PACKET *p,
																				 uint8 *dest,size_t destsize,const uint8 *src,size_t srcsize)
{
	int r;
	DKC_TELECOM_CONFIRM_PACKET_HEADER header;
	DKC_LZW_HEADER hlzw;	//save lzw state

	size_t tt = sizeof(header) + sizeof(hlzw);

	//errorcheck routine
	if(dkcdTELECOM_CONFIRM_PACKET_MAX_LENGTH < srcsize + tt){
		return edk_ArgumentException;
	}
	if(destsize < tt){
		return edk_BufferOverFlow;
	}

	//header routine

	//別に&する必要ないかも＾＾；
	header.length = srcsize & dkcdTELECOM_CONFIRM_PACKET_MAX_LENGTH;
	//もっといい方法があるはずだな。だけどisNaturalKeyは常に1とは限らないので･･･ todo debuggerでチェック
	header.length |= (p->isNaturalKey) ? (1<<31) : 0;


	dkcSHA512Init(p->psig);
	dkcSHA512Load(p->psig,src,srcsize);
	r = dkcSHA512FinalDigest(p->psig,header.sig,sizeof(header.sig));
	if(DKUTIL_FAILED(r)) return r;

	copy_tcp_header(dest,&header);
	//compression process

	
	{
		DKC_LZW *plzw;
		
		uint8 *dest_t;
		size_t dest_size_t;
		
		
		

		dest_t = dest + tt;
		dest_size_t = destsize - tt;
		plzw = dkcAllocLZW(p->pause_size);
		if(NULL==plzw){
			r = edk_FAILED;
			goto Error;
		}

		r = dkcLZWEncode(plzw,&hlzw,dest_t,dest_size_t,src,srcsize,dest_size_t,
			dkcRand(&(p->seed)),//notice
			edkcLZW_Default);

		dkcFreeLZW(&plzw);

		if(DKUTIL_FAILED(r)) goto Error;
		
		//headerを入れる。
		dest_t = dest + sizeof(header);
		copy_lzw_header(dest_t,&hlzw);


		
	}//end of lzw


	//crypt process
	{
		uint8 *dest_t = dest + tt;
		size_t dest_size_t = hlzw.mCompressedSize ;
		if(edk_ARCFOUR_SIGNATURE & p->flag){
			dkcArcfourEncryptNoDest(p->crypt_obj,dest_t,dest_size_t);
		}else if(edk_HC256_SIGNATURE & p->flag){
			dkcHC256EncryptNoDestDOE(p->crypt_obj,dest_t,dest_size_t);
		}else if(edk_SNOW20_SIGNATURE & p->flag){
			dkcSNOW2EncryptNoDestDOE(p->crypt_obj,dest_t,dest_size_t);
		}else if(edk_VERNAM_SIGNATURE & p->flag){
			dkcmNOT_ASSERT("NO IMPL");
		}
				
		
	}//end of crypt process

Error:
		return r;
}

int WINAPI dkcTelecomConfirmPacketEncode(DKC_TELECOM_CONFIRM_PACKET *p,
																				 uint8 *dest,size_t destsize,const uint8 *src,size_t srcsize)
{
	size_t count = srcsize / p->pause_size;
	size_t rest = srcsize % p->pause_size,i;
	int r;size_t offset = 0;
	for(i=0;i<count;i++){
		offset = count * p->pause_size;
		r = tcp_encode_pause(p,dest,destsize,src,srcsize);
		if(DKUTIL_FAILED(r)) return r;
	}
	
}

static DKC_INLINE void get_lzw_header(DKC_LZW_HEADER *p,const uint8 *src){
	DKC_LZW_HEADER *a = (DKC_LZW_HEADER *)((uint8 *)src + sizeof(DKC_LZW_HEADER));
	if(dkcIsBigEndian()){
		p->mSignature = dkcReverseEndian32(a->mSignature);
		p->mOriginSize = dkcReverseEndian32(a->mOriginSize);
		p->mCompressedSize = dkcReverseEndian32(a->mCompressedSize);
		p->option = dkcReverseEndian32(a->option);
	}else{
		memcpy(p,a,sizeof(*p));
	}
}
size_t WINAPI dkcGetOutputSize(const uint8 *src){
	DKC_LZW_HEADER h;
	get_lzw_header(&h,src);
	return h.mOriginSize;
}


int WINAPI dkcTelecomConfirmPacketDecode(DKC_TELECOM_CONFIRM_PACKET *p,uint8 *dest,size_t destsize,const uint8 *src,size_t srcsize)
{
	int r;
	DKC_TELECOM_CONFIRM_PACKET_HEADER header;
	DKC_LZW_HEADER hlzw;	//save lzw state

	size_t tt = sizeof(header) + sizeof(hlzw);

	//errorcheck routine
	if(dkcdTELECOM_CONFIRM_PACKET_MAX_LENGTH < srcsize + tt){
		return edk_ArgumentException;
	}


	//header routine


	copy_tcp_header((void *)&header,(DKC_TELECOM_CONFIRM_PACKET_HEADER *)src);
	get_lzw_header(&hlzw,src);
	if(hlzw.mOriginSize < destsize){
		return edk_BufferOverFlow;
	}
	//compression process

	
	{
		DKC_LZW *plzw;
		
		uint8 *src_t;
		size_t src_size_t;
		
	
		
		

		src_t = (uint8 *)src + tt;
		src_size_t = srcsize - tt;
		plzw = dkcAllocLZW(p->pause_size );
		if(NULL==plzw){
			r = edk_FAILED;
			goto Error;
		}

		r = dkcLZWDecode(plzw,&hlzw,dest,destsize,src_t,src_size_t,
			dkcRand(&(p->seed)));

		dkcFreeLZW(&plzw);

		if(DKUTIL_FAILED(r)) goto Error;


		
	}//end of lzw


	//crypt process
	{
		uint8 *dest_t = dest;
		size_t dest_size_t = hlzw.mOriginSize ;

		if(edk_ARCFOUR_SIGNATURE & p->flag){
			dkcArcfourEncryptNoDest(p->crypt_obj,dest_t,dest_size_t);
		}else if(edk_HC256_SIGNATURE & p->flag){
			dkcHC256EncryptNoDestDOE(p->crypt_obj,dest_t,dest_size_t);
		}else if(edk_SNOW20_SIGNATURE & p->flag){
			dkcSNOW2EncryptNoDestDOE(p->crypt_obj,dest_t,dest_size_t);
		}else if(edk_VERNAM_SIGNATURE & p->flag){
			dkcmNOT_ASSERT("NO IMPL");
		}
				
		
	}//end of crypt process

Error:
		return r;
}

#endif