/*!
@file dkcSNOW20.h
@brief SNOW2.0 cipher Algorithm
@since 2004/12/31
@note
参考資料：
http://www.it.lth.se/cryptology/snow/

@todo endianがビッグでもリトルでも、その他でも出力結果が同じで動くかどうかを確かめること

*/



#ifndef DKUTIL_C_SNOW20_H
#define DKUTIL_C_SNOW20_H

#include "dkcOSIndependent.h"


#define dkcdSNOW2_BOX_SIZE 16


typedef struct dkc_SNOW2{
	
	///エントロピーバッファ
	uint32 sw[dkcdSNOW2_BOX_SIZE];
	///FSM
	uint32 outfrom_fsm,r1,r2;
	///byte単位出力値蓄積バッファ
	union{
		uint64 a64[dkcdSNOW2_BOX_SIZE / 2];
		uint32 a32[dkcdSNOW2_BOX_SIZE];
		uint16 a16[dkcdSNOW2_BOX_SIZE * 2];
		uint8  a8[dkcdSNOW2_BOX_SIZE * 4];
	}u_store;
		
	//DKC_4BYTE_UNION_LITTLE_ENDIAN_BASE store;
	///何byte蓄積されているか
	uint8 store_size;
}DKC_SNOW2;



/**

@param keysize[in] には16か32を入れないとダメ
@note
keyのサイズは128bit (16byte)か256bit (32byte)

キーのエンディアンについては未定義･･･。
*/
DKC_EXTERN DKC_SNOW2 *WINAPI dkcAllocSNOW2(BYTE *key,size_t keysize,
																					 uint32 IV3,uint32 IV2,uint32 IV1,uint32 IV0);

DKC_EXTERN int WINAPI dkcSNOW2Init(DKC_SNOW2 *p,uint8 *key,size_t keysize,uint32 IV3,uint32 IV2,uint32 IV1,uint32 IV0);

DKC_INLINE DKC_SNOW2 *dkcAllocSNOW2Const(const BYTE *key,size_t keysize,uint32 IV3,uint32 IV2,uint32 IV1,uint32 IV0)
{
	uint8 v[32];
	switch(keysize){
	case 16:
		memcpy(v,key,16);
		memset(&v[16],0,16);
		break;
	case 32:
		memcpy(v,key,32);
		break;
	default:
		return NULL;
	}
	return dkcAllocSNOW2(v,keysize,IV3,IV2,IV1,IV0);
}

DKC_EXTERN int WINAPI dkcFreeSNOW2(DKC_SNOW2 **);

///1byteの暗号化用ストリームを生成する
DKC_EXTERN uint8 WINAPI dkcSNOW2Process(DKC_SNOW2 *p);
#define dkcSNOW2_Byte(p) dkcSNOW2Process(p)
/**
@note 
*/
///1byte単位で暗号化でいるSNOW2
DKC_EXTERN int WINAPI dkcSNOW2EncryptNoDestDOE(DKC_SNOW2 *p,uint8 *inout,size_t size);

DKC_EXTERN int WINAPI dkcSNOW2EncryptDOE(DKC_SNOW2 *,uint8 *dest,size_t dsize,const uint8 *src,size_t ssize);
#define dkcSNOW2DecryptDOE(a,b,c,d,e) dkcSNOW2EncryptDOE(a,b,c,d,e)
//#define dkcSNOW2EncryptNoDestODE(a,b,c) dkcSNOW2EncryptODE(a,b,c,b,c)
#define dkcSNOW2DecryptNoDestDOE(a,b,c) dkcSNOW2EncryptNoDestDOE(a,b,c)

//DKC_EXTERN int WINAPI dkcSNOW2EncryptNoDestODE(DKC_SNOW2 *,void *mem,size_t size)

#endif //end of include once