/*!
@file dkcArcfour.h
@brief RC4互換アルゴリズム Arcfour
*/
#ifndef  DKUTIL_C_ARCFOUR_H
#define  DKUTIL_C_ARCFOUR_H
#include "dkcOSIndependent.h"


//**********************************************************
//RC4互換アルゴリズム Arcfour
//参考：
//http://www21.ocn.ne.jp/~k-west/SSLandTLS/draft-kaukonen-cipher-arcfour-03-Ja.txt
//**********************************************************

///arcfour algprithm state (RC4互換のアルゴリズム
typedef struct dkc_Arcfour_State{
	unsigned char mi;
	unsigned char mj;
	unsigned char msbox[256];
}DKC_ARCFOUR_STATE;

/*!
@param key[in] keyへのポインタ
@param keylen[in] keyのサイズ
@return DKC_ARCFOUR_STATE 構造体へのポインタ
@note
使い終わったらdkcFreeArcfourを使いましょう。
*/
DKC_EXTERN DKC_ARCFOUR_STATE * WINAPI dkcAllocArcfour(const unsigned char *key,size_t keylen);

/*!
@param dkcAllocArcfourで確保した領域へのポインタへのポインタ
@return edk_SUCCEEDEDで成功
*/
DKC_EXTERN int WINAPI dkcFreeArcfour(DKC_ARCFOUR_STATE **p);

/*!
@param p[in][out] dkcAllocArcfour()で確保した領域へのポインタ
@return XORすべき値
*/
DKC_EXTERN unsigned char WINAPI dkcArcfourProcess(DKC_ARCFOUR_STATE *p);
#define dkcArcfourByte(s) dkcArcfourProcess(s)
/*!
@param p[in] dkcAllocArcfour()で確保した領域へのポインタ
@param dest[in] 出力バッファへのポインタ
@param destsize[in] destのサイズ
@param src[in] 入力バッファへのポインタ
@param srcsize[in] srcのサイズ
@return edk_SUCCEEDED で成功
*/
DKC_EXTERN int WINAPI dkcArcfourEncrypt(DKC_ARCFOUR_STATE *p,
	unsigned char *dest,size_t destsize,
	const unsigned char *src,size_t srcsize);

DKC_EXTERN void WINAPI dkcArcfourEncryptNoDest(DKC_ARCFOUR_STATE *p,
	unsigned char *dest_and_src,size_t dest_and_srcsize);	

#define dkcArcfourDecrypt(p,dest,destsize,src,srcsize) \
	dkcArcfourEncrypt(p,dest,destsize,src,srcsize)

#define dkcArcfourDecryptNoDest(p,dest_and_src,dest_and_srcsize) \
	dkcArcfourEncryptNoDest(p,dest_and_src,dest_and_srcsize)


///Arcfourを2バイトに拡張したもの 暗号強度、安全性等は不明　使用は控えた方が良いかと･･･
typedef struct dkc_Arcfour2byte_State{
	unsigned short mi;
	unsigned short mj;
	unsigned short msbox[USHRT_MAX];
}DKC_ARCFOUR2BYTE_STATE;


DKC_EXTERN DKC_ARCFOUR2BYTE_STATE * WINAPI dkcAllocArcfour2Byte(const unsigned char *key,size_t keylen);


DKC_EXTERN int WINAPI dkcFreeArcfour2Byte(DKC_ARCFOUR2BYTE_STATE **p);


DKC_EXTERN unsigned short WINAPI dkcArcfour2ByteProcess(DKC_ARCFOUR2BYTE_STATE *p);

/**

*/
DKC_EXTERN int WINAPI dkcArcfour2ByteEncrypt(DKC_ARCFOUR2BYTE_STATE *p,
	unsigned char *dest,size_t destsize,
	const unsigned char *src,size_t srcsize);


DKC_EXTERN int WINAPI dkcArcfour2ByteEncryptNoDest(DKC_ARCFOUR2BYTE_STATE *p,
	unsigned char *dest_and_src,size_t dest_and_srcsize);	

#define dkcArcfour2ByteDecrypt(p,dest,destsize,src,srcsize) \
	dkcArcfour2ByteEncrypt(p,dest,destsize,src,srcsize)

#define dkcArcfour2ByteDecryptNoDest(p,dest_and_src,dest_and_srcsize) \
	dkcArcfour2ByteEncryptNoDest(p,dest_and_src,dest_and_srcsize)



#if !defined( DKUTIL_C_ARCFOUR_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcArcfour.c"
#endif

#endif //end of include once