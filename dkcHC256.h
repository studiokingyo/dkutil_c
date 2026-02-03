/*!
@file dkcHC256.h
@brief HC256 stream cipher Algorithm
@author Hongjun Wu in 2003. convert by d金魚
@since original 2003 / converted in 2004/12/20
@note
参考資料：
http://d.hatena.ne.jp/studiokingyo/20041216#p4
http://www.i2r.a-star.edu.sg/icsd/staff/hongjun/hc/

<h3>略語解説</h3>
<PRE>
DOE : Dependent On Endian / エンディアンに依存する
</PRE>
*/



#ifndef DKUTIL_C_HC256_H
#define DKUTIL_C_HC256_H

#include "dkcOSIndependent.h"

#define dkcdHC256_KEY_SIZE 32
#define dkcdHC256_IV_SIZE 32
typedef struct dkc_HC256{
	uint32 P[1024],Q[1024];
	uint32 X[16],Y[16];
	uint32 counter2048; // counter2048 = i mod 2048;
	union{
		uint64 a64[8];
		uint32 a32[16];
		uint16 a16[32];
		uint8  a8[64];
	}u_store;
	///store
	size_t store_size;
}DKC_HC256;


/**
@see  keyやivは内部でいじり倒されるのでそれが嫌な場合はdkcAllocHC256Const()を使ってください。
*/
///keyのサイズは32byte ivも32byte
DKC_EXTERN DKC_HC256 *WINAPI dkcAllocHC256(BYTE *key,size_t size,uint32 *iv,size_t ivsize);

DKC_EXTERN DKC_HC256 *dkcAllocHC256Const(const BYTE *key,size_t size,const uint32 *iv,size_t ivsize);

	


DKC_EXTERN int WINAPI dkcFreeHC256(DKC_HC256 **);


//DKC_EXTERN uint32 dkcHC256Process32(DKC_HC256 *,uint32 u);
DKC_EXTERN uint8 WINAPI dkcHC256Process(DKC_HC256 *p);
//DKC_EXTERN uint8 WINAPI dkcHC256_Byte(DKC_HC256 *p);
#define dkcHC256_Byte(p) dkcHC256Process(p);
/**
@note

*/

DKC_EXTERN int WINAPI dkcHC256EncryptDOE(DKC_HC256 *p,uint8 *dest,size_t dsize,const uint8 *src,size_t ssize);
#define dkcHC256DecryptDOE(a,b,c,d,e) dkcHC256EncryptDOE(a,b,c,d,e)

DKC_EXTERN int WINAPI dkcHC256EncryptNoDestDOE(DKC_HC256 *p,uint8 *inout,size_t size);
#define dkcHC256DecryptNoDestDOE(a,b,c) dkcHC256EncryptNoDestDOE(a,b,c)


//DKC_EXTERN int WINAPI dkcHC256EncryptBlock512DOE(DKC_HC256 *p,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize);

//#define dkcHC256DecryptBlock512DOE(p,dest,dsize,src,ssize) dkcHC256EncryptBlock512DOE(p,dest,dsize,src,ssize)

//DKC_EXTERN int WINAPI dkcHC256EncryptBlock512NoDestDOE(DKC_HC256 *p,BYTE *dest,size_t size);
//DKC_EXTERN int WINAPI dkcHC256EncryptBlock512NoDestDOE(DKC_HC256 *p,uint32 *block64byte);

//#define dkcHC256DecryptBlock512NoDestDOE(p,dest,size) dkcHC256EncryptBlock512NoDestDOE(p,dest,size)

//DKC_EXTERN int WINAPI dkcHC256EncryptNoDestDOE(DKC_HC256 *p,uint8 *inout,size_t size);


//#define dkcHC256EncryptNoDest(a,b,c) dkcHC256EncryptDOE(a,b,c,b,c)

//#define dkcHC256DecryptNoDest(a,b,c) dkcHC256EncryptDOE(a,b,c,b,c)


//DKC_EXTERN int WINAPI dkcHC256Decrypt(DKC_HC256 *p,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize);
/*
DKC_EXTERN int WINAPI dkcHC256Encrypt(DKC_HC256 *p,BYTE *dest,int dsize,const BYTE *src,int ssize);


DKC_EXTERN int WINAPI dkcHC256Decrypt(DKC_HC256 *p,BYTE *dest,int dsize,const BYTE *src,int ssize);
*/





#endif //end of include once