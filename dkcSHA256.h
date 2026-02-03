/*!
@file dkcSHA256.h
@brief SHA 256 
@author original : General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.
        convert C by  d金魚
*/
#ifndef DKUTIL_C_SHA256_H
#define DKUTIL_C_SHA256_H

#include "dkcOSIndependent.h"

#define SHA256_HASH   8
#define SHA256_BLOCK 16
#define SHA256_WORK  64


///SHA256の生ハッシュ値(binary)に必要なバッファサイズ
#define SHA256_BIN_BUFFER_SIZE (SHA256_HASH * 4)
///SHA256のハッシュ値文字列に必要なバッファサイズ
#define SHA256_STR_BUFFER_SIZE (SHA256_HASH * 8 + 1)

typedef struct dkc_SHA256{
	DWORD m_dwH[SHA256_HASH];
	DWORD m_dwLNumBits;
	DWORD m_dwHNumBits;
	DWORD m_aBlock[SHA256_BLOCK];
	int   m_nNumChr;
	BYTE mFinalized;
}DKC_SHA256;
/*
DKC_EXTERN DKC_SHA256 *WINAPI dkcAllocSHA256();

DKC_EXTERN void WINAPI dkcSHA256Init(DKC_SHA256 *);

DKC_EXTERN void WINAPI dkcSHA256Load(DKC_SHA256 *p,const BYTE *pBuffer,DWORD dwSize);

DKC_EXTERN void WINAPI dkcSHA256Final(DKC_SHA256 *p);

DKC_EXTERN int WINAPI dkcSHA256Digest(DKC_SHA256 *p,char *buff,size_t size);

DKC_EXTERN int WINAPI dkcFreeSHA256(DKC_SHA256 **);
*/



/*!
@return DKC_SHA256構造体への確保したメモリ領域
@note
すでにdkcSHA256Init()は呼ばれています。
*/
DKC_EXTERN DKC_SHA256 *WINAPI dkcAllocSHA256();
///@return dkcAllocSHA256()から取得した領域を初期化する。
DKC_EXTERN void WINAPI dkcSHA256Init(DKC_SHA256 *);
/*!
@param p[in][out] dkcAllocSHA256()で取得したポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
*/
DKC_EXTERN void WINAPI dkcSHA256Load(DKC_SHA256 *p,const BYTE *pBuffer,DWORD dwSize);


/*!
@param p[in][out] dkcAllocSHA256()で取得したポインタ
@note
dkcSHA256FinalDigest()を使用する事を奨励します。
*/
DKC_EXTERN void WINAPI dkcSHA256Final(DKC_SHA256 *p);
/*!
@param p[in][out] dkcAllocSHA256()で取得したポインタ
@param buff[out] 書き込むバッファへのポインタ
@param size[in] buffのサイズ
@return 成功したらedk_SUCCEEDEDが返る
*/
///文字列版
DKC_EXTERN int WINAPI dkcSHA256DigestStr(DKC_SHA256 *p,char *buff,size_t size);

///バイナリ版 @see dkcSHA256DigestStr
DKC_EXTERN int WINAPI dkcSHA256Digest(DKC_SHA256 *p,BYTE *buff,size_t size);

/*!
@see 引数、戻り値についてはdkcSHA256DigestStr()と同じです。 
@note
dkcSHA256Final()とdkcSHA256DigestStr()を使用するより、この関数の使用を奨励します。
*/
DKC_EXTERN int WINAPI dkcSHA256FinalDigestStr(DKC_SHA256 *p,char *buff,size_t size);

///バイナリ版 @see dkcSHA256DigestStr();
DKC_EXTERN int WINAPI dkcSHA256FinalDigest(DKC_SHA256 *p,BYTE *buff,size_t size);

/*!
@param p[in][out] dkcAllocSHA256()で取得したポインタへのポインタ
@return 上手く開放できたらedk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkcFreeSHA256(DKC_SHA256 **p);



#endif //end of include once