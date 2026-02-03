/*!
@file dkcSHA1.h
@brief SHA 1
@author original : General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.
        convert C by  d金魚
*/
#ifndef DKUTIL_C_SHA1_H
#define DKUTIL_C_SHA1_H

#include "dkcOSIndependent.h"

#define SHA_HASH  5
#define SHA_BLOCK 16
#define SHA_WORK  80


///SHA1の生ハッシュ値(binary)に必要なバッファサイズ
#define SHA1_BIN_BUFFER_SIZE (SHA_HASH * 4)
///SHA1のハッシュ値文字列に必要なバッファサイズ
#define SHA1_STR_BUFFER_SIZE (SHA_HASH * 8 + 1)

#define SHA160_BIN_BUFFER_SIZE SHA1_BIN_BUFFER_SIZE

#define SHA160_STR_BUFFER_SIZE SHA1_STR_BUFFER_SIZE


typedef struct dkc_SHA1{
	DWORD m_dwH[SHA_HASH];
	DWORD m_dwLNumBits;
	DWORD m_dwHNumBits;
	DWORD m_aBlock[SHA_BLOCK];
	int   m_nNumChr;
	BYTE mFinalized;
}DKC_SHA1,DKC_SHA160;
/*
DKC_EXTERN DKC_SHA1 *WINAPI dkcAllocSHA1();

DKC_EXTERN void WINAPI dkcSHA1Init(DKC_SHA1 *);

DKC_EXTERN void WINAPI dkcSHA1Load(DKC_SHA1 *p,const BYTE *pBuffer,DWORD dwSize);

DKC_EXTERN void WINAPI dkcSHA1Final(DKC_SHA1 *p);

DKC_EXTERN int WINAPI dkcSHA1Digest(DKC_SHA1 *p,char *buff,size_t size);

DKC_EXTERN int WINAPI dkcFreeSHA1(DKC_SHA1 **);
*/


/*!
@return DKC_SHA1構造体への確保したメモリ領域
@note
すでにdkcSHA1Init()は呼ばれています。
*/
DKC_EXTERN DKC_SHA1 *WINAPI dkcAllocSHA1();
///@return dkcAllocSHA1()から取得した領域を初期化する。
DKC_EXTERN void WINAPI dkcSHA1Init(DKC_SHA1 *);
/*!
@param p[in][out] dkcAllocSHA1()で取得したポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
*/
DKC_EXTERN void WINAPI dkcSHA1Load(DKC_SHA1 *p,const BYTE *pBuffer,DWORD dwSize);


/*!
@param p[in][out] dkcAllocSHA1()で取得したポインタ
@note
dkcSHA1FinalDigest()を使用する事を奨励します。
*/
DKC_EXTERN void WINAPI dkcSHA1Final(DKC_SHA1 *p);
/*!
@param p[in][out] dkcAllocSHA1()で取得したポインタ
@param buff[out] 書き込むバッファへのポインタ
@param size[in] buffのサイズ
@return 成功したらedk_SUCCEEDEDが返る
*/
DKC_EXTERN int WINAPI dkcSHA1DigestStr(DKC_SHA1 *p,char *buff,size_t size);
///binaryハッシュ値 版 @see dkcSHA1DigestStr()
DKC_EXTERN int WINAPI dkcSHA1Digest(DKC_SHA1 *p,BYTE *buff,size_t size);

/*!
@see 引数、戻り値についてはdkcSHA1Digest()と同じです。 
@note
dkcSHA1Final()とdkcSHA1Digest()を使用するより、この関数の使用を奨励します。
*/
DKC_EXTERN int WINAPI dkcSHA1FinalDigestStr(DKC_SHA1 *p,char *buff,size_t size);

///binaryハッシュ値 版 @see dkcSHA1FinalDigestStr()
DKC_EXTERN int WINAPI dkcSHA1FinalDigest(DKC_SHA1 *p,BYTE *buff,size_t size);

/*!
@param p[in][out] dkcAllocSHA1()で取得したポインタへのポインタ
@return 上手く開放できたらedk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkcFreeSHA1(DKC_SHA1 **p);


#endif //end of include once