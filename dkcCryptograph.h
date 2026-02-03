/*!
@file dkcCryptograph.h
@brief �Í����n���ׂ�include
@note
INCLUDE CRYPTOGRAPH ALGORITHM ALL
@since 2004/07/11
*/
#ifndef DKUTIL_C_CRYPTOGRAPH_H
#define DKUTIL_C_CRYPTOGRAPH_H

#include "dkcRijndael.h"
#include "dkcBlowfish.h"
#include "dkcVernam.h"

#include "dkcArcfour.h"
#include "dkcCamellia.h"
#include "dkcHC256.h"
#include "dkcSNOW20.h"

#include "dkcHMAC.h"



//**********************************************************
//helper

///1�o�C�g���Í�������B@memo ������Encrypt Decrypt���ɕς��Ȃ�
DKC_EXTERN BYTE WINAPI dkcEncryptByte(BYTE src,BYTE x);

///dkcEncryptByte()�ňÍ������ꂽ�f�[�^�𕡍�������B
///@see dkcEncryptByte()
DKC_EXTERN BYTE WINAPI dkcDecryptByte(BYTE src,BYTE x);

DKC_EXTERN int WINAPI dkcSNOW2GetInitializeVector(const void *key,size_t keysize,
																									 	uint32 *iv3,uint32 *iv2,uint32 *iv1,uint32 *iv0);

DKC_EXTERN DKC_SNOW2 *WINAPI dkcAllocSNOW2NoLimitKeyLength(const void *key,size_t keysize);

DKC_EXTERN DKC_HC256 *WINAPI dkcAllocHC256NoLimitKeyLength(const void *key,size_t keysize);

///@note ��قǂ̖�肪������������͕ς��Ȃ����Ƃɂ���B
DKC_EXTERN int WINAPI dkcCalculateInitializeVector(void *dest,size_t destsize_and_generate_iv_size,const void *key,size_t keysize);

//**********************************************************



#if !defined( DKUTIL_C_CRYPTOGRAPH_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcCryptograph.c"
#endif

#endif //end of include once