
/*!
@file dkcArcfour.c
@brief RC4互換アルゴリズム Arcfour stream cipher algorithm
@author d金魚
*/
#define  DKUTIL_C_ARCFOUR_C
#include "dkcArcfour.h"
#include "dkcStdio.h"



DKC_ARCFOUR_STATE *WINAPI dkcAllocArcfour(
	const unsigned char *key,size_t keylen)
{

	int i,j;
	//unsigned char temp;
	unsigned char *sb1,sb2[256];
	DKC_ARCFOUR_STATE *p;
	if(NULL==key || 0==keylen){
		return NULL;
	}

	memset(sb2,0,sizeof(sb2));
	//allocate state struct
	p = (DKC_ARCFOUR_STATE *)dkcAllocate(sizeof(DKC_ARCFOUR_STATE));
	if(NULL==p){
		return NULL;
	}
	sb1 = p->msbox;
	
	//initialize
	for(i=0;i<256;i++){
		sb1[i] = (unsigned char)i;
		//嗅ぎ回られないように(なんか、意味あるのか？ないナァ？
		//p->msbox2[i] = key [i % keylen];
	}
	j=i=0;
	
	for(;j<256;j++){
		sb2[j] = key [j % keylen];
	}


	for(i=0;i<256;i++){
		//j = (j + sb1 [i] + sb2 [i]) % 256;
		j = (j + sb1[i] + sb2[i]) & 0xff;
		
		
		//swap
		SWAP_TEMP(register uint8, sb1 [i],sb1 [j]);
		/*temp = sb1 [i];
		sb1 [i] = sb1 [j];
		sb1 [j] = temp;*/
		/*sb1[i] = sb1[j] - sb1[i] ;
    sb1[j] -= sb1[i] ;
    sb1[i] += sb1[j] ;*/
	//SWAP_NUM(sb1[i],sb1[j]);
	}
	return p;
}

DKC_INLINE unsigned char WINAPI dkcArcfourProcess(DKC_ARCFOUR_STATE *p){
  //unsigned char i,j,temp;
	unsigned char i,j;
  unsigned char *sb1 = p->msbox;
  
  //calc
  i = (unsigned char )(p->mi+1);// % 256;
  j = (unsigned char )(p->mj + sb1[i]);// % 256;
  
	 //swap
	SWAP_FAST8( sb1 [i],sb1 [j]);
  /*temp = sb1 [i];
  sb1 [i] = sb1 [j];
  sb1 [j] = temp;
	*/
  //SWAP_NUM(sb1[i],sb1[j]);
  //refresh
  p->mi = i;
  p->mj = j;
  //calc
  i = (unsigned char )(sb1 [i] + sb1 [j]);// % 256;
  j = (unsigned char )sb1[i];
  return j;
}

static DKC_INLINE void dkcArcfourEncrypt_Base(DKC_ARCFOUR_STATE *p,
	unsigned char *dest,unsigned const char *src,size_t srcsize)
{

	size_t cc;
  unsigned char i,j;
  unsigned char *sb1 = p->msbox;
	//unsigned char temp;
  
 	 //calc
	i = (unsigned char )(p->mi);
	j = (unsigned char )(p->mj);

	for(cc = 0;cc < srcsize;cc++){
		 	 //calc
		i = (unsigned char )(i+1);
		j = (unsigned char )(j + sb1[i]);
  
		 //swap
		
		SWAP_FAST8(sb1 [i],sb1 [j]);
		/*temp = sb1 [i];
		sb1 [i] = sb1 [j];
		sb1 [j] = temp;
		*/

		//SWAP_NUM_EX(sb1[i],sb1[j],unsigned char);

		//calc
		dest[cc] = (unsigned char )
			(src[cc] ^ (unsigned char )sb1[
				(unsigned char )(sb1 [i] + sb1 [j]) 
			] 
		);
	}
	//refresh
	p->mi = i;
	p->mj = j;

}

int WINAPI dkcArcfourEncrypt(DKC_ARCFOUR_STATE *p,
	unsigned char *dest,size_t destsize,
	const unsigned char *src,size_t srcsize)
{
	//size_t i=0;
	if(destsize < srcsize){
		return edk_BufferOverFlow;
	}
	dkcArcfourEncrypt_Base(p,dest,src,srcsize);

	/*for(i = 0;i < srcsize;i++){
		dest[i] = (unsigned char )(src[i] ^ dkcArcfourByte(p) );
	}*/

	return edk_SUCCEEDED;
}



void WINAPI dkcArcfourEncryptNoDest(DKC_ARCFOUR_STATE *p,
	unsigned char *dest_and_src,size_t dest_and_srcsize)
{
	dkcArcfourEncrypt_Base(p,dest_and_src,dest_and_src,dest_and_srcsize);
}

int WINAPI dkcFreeArcfour(DKC_ARCFOUR_STATE **p){
	if(NULL==p){
		return edk_FAILED;
	}
	return dkcFree((void **)p);
}



//**********************************************************
//2byteバージョン

DKC_ARCFOUR2BYTE_STATE *WINAPI dkcAllocArcfour2Byte(
	const unsigned char *key,size_t keylen)
{

	int i,j;
	//unsigned short temp;
	unsigned short *sb1,sb2[USHRT_MAX];
	DKC_ARCFOUR2BYTE_STATE *p;
	if(NULL==key || 0==keylen){
		return NULL;
	}
	
	memset(sb2,0,sizeof(sb2));
	//allocate state struct
	p = (DKC_ARCFOUR2BYTE_STATE *)dkcAllocate(sizeof(DKC_ARCFOUR2BYTE_STATE));
	if(NULL==p){
		return NULL;
	}
	sb1 = p->msbox;
	
	//initialize
	for(i=0;i<USHRT_MAX;i++){
		sb1[i] = (unsigned short)i;
		//嗅ぎ回られないように(なんか、意味あるのか？ないナァ？
		//p->msbox2[i] = key [i % keylen];
	}
	j=i=0;
	
	for(;j<USHRT_MAX;j++){
		sb2[j] = key [j % keylen];
	}


	for(i=0;i<USHRT_MAX;i++){
		//j = (j + sb1 [i] + sb2 [i]) % 256;
		j = (j + sb1[i] + sb2[i]) & 0xff;
		
		SWAP_TEMP(register uint16,sb1 [i],sb1 [j]);
		/*temp = sb1 [i];
		sb1 [i] = sb1 [j];
		sb1 [j] = temp;*/
		/*sb1[i] = sb1[j] - sb1[i] ;
    sb1[j] -= sb1[i] ;
    sb1[i] += sb1[j] ;*/
	//SWAP_NUM(sb1[i],sb1[j]);
	}
	return p;
}

DKC_INLINE unsigned short WINAPI dkcArcfour2ByteProcess(DKC_ARCFOUR2BYTE_STATE *p){
  //unsigned short i,j,temp;
	unsigned short i,j;
  unsigned short *sb1 = p->msbox;
  
  //calc
  i = (unsigned short )(p->mi+1);// % 256;
  j = (unsigned short )(p->mj + sb1[i]);// % 256;
  
	 //swap
  SWAP_FAST16(sb1[i],sb1[j]);
	/*temp = sb1 [i];
  sb1 [i] = sb1 [j];
  sb1 [j] = temp;
	*/
  //SWAP_NUM(sb1[i],sb1[j]);
  
	//refresh
  p->mi = i;
  p->mj = j;
  //calc
  i = (unsigned short )(sb1 [i] + sb1 [j]);// % 256;
  j = (unsigned short )sb1[i];
  return j;
}

/**
@param srcsize[in] バイト単位のサイズではなく、unsigned short単位の数
*/
static DKC_INLINE void dkcArcfour2ByteEncrypt_Base(DKC_ARCFOUR2BYTE_STATE *p,
	unsigned short *dest,unsigned const short *src,size_t srcsize)
{

	size_t cc;
  unsigned short i,j;
  unsigned short *sb1 = p->msbox;
	//unsigned short temp;
  
 	 //calc
	i = (unsigned short )(p->mi);
	j = (unsigned short )(p->mj);

	for(cc = 0;cc < srcsize;cc++){
		 	 //calc
		i = (unsigned short )(i+1);
		j = (unsigned short )(j + sb1[i]);
  
		 //swap
		SWAP_FAST16(sb1[i],sb1[j]);
		/*temp = sb1 [i];
		sb1 [i] = sb1 [j];
		sb1 [j] = temp;
		*/

		//SWAP_NUM_EX(sb1[i],sb1[j],unsigned char);

		//calc
		dest[cc] = (unsigned short )
			(src[cc] ^ (unsigned short )sb1[
				(unsigned short )(sb1 [i] + sb1 [j]) 
			] 
		);
	}
	//refresh
	p->mi = i;
	p->mj = j;

}

int WINAPI dkcArcfour2ByteEncrypt(DKC_ARCFOUR2BYTE_STATE *p,
	unsigned char *dest,size_t destsize,
	const unsigned char *src,size_t srcsize)
{
	//size_t i=0;
	if(destsize < srcsize){
		return edk_BufferOverFlow;
	}
	if(srcsize % 2 != 0){//2の倍数じゃないとねぇ
		return edk_ArgumentException;
	}

	dkcArcfour2ByteEncrypt_Base(p,
		(unsigned short *)dest,(unsigned short *)src,
		srcsize / 2
	);

	/*for(i = 0;i < srcsize;i++){
		dest[i] = (unsigned char )(src[i] ^ dkcArcfourByte(p) );
	}*/

	return edk_SUCCEEDED;
}



int WINAPI dkcArcfour2ByteEncryptNoDest(DKC_ARCFOUR2BYTE_STATE *p,
	unsigned char *dest_and_src,size_t dest_and_srcsize)
{
	if(dest_and_srcsize % 2 != 0){//2の倍数じゃないとねぇ
		return edk_ArgumentException;
	}
	dkcArcfour2ByteEncrypt_Base(p,
		(unsigned short *)dest_and_src,(unsigned short *)dest_and_src,
		dest_and_srcsize / 2
	);
	return edk_SUCCEEDED;
}

int WINAPI dkcFreeArcfour2Byte(DKC_ARCFOUR2BYTE_STATE **p){
	if(NULL==p){
		return edk_FAILED;
	}
	return dkcFree((void **)p);
}




