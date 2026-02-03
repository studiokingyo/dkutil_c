/*!
@file dkcSHA512.h
@brief SHA 512
@author original : General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.
        convert C by  d金魚
*/
#ifndef DKUTIL_C_SHA512_H
#define DKUTIL_C_SHA512_H




#include "dkcOSIndependent.h"

#define SHA512_HASH   8
#define SHA512_BLOCK 16
#define SHA512_WORK  80


///SHA512の生ハッシュ値(binary)に必要なバッファサイズ
#define SHA512_BIN_BUFFER_SIZE (SHA512_HASH * 8)
///SHA512のハッシュ文字列に必要なバッファサイズ
#define SHA512_STR_BUFFER_SIZE (SHA512_HASH * 16 + 1)


typedef struct dkc_SHA512{
	BOOL  m_bMMX;
	///多分、これがSHA512のハッシュ値
	QWORD m_qwH[SHA512_HASH];
	QWORD m_qwLNumBits;
	QWORD m_qwHNumBits;
	QWORD m_aBlock[SHA512_BLOCK];
	int   m_nNumChr;
	BYTE mFinalized;
}DKC_SHA512;
/*
DKC_EXTERN DKC_SHA512 *WINAPI dkcAllocSHA512();

DKC_EXTERN void WINAPI dkcSHA512Init(DKC_SHA512 *);

DKC_EXTERN void WINAPI dkcSHA512Load(DKC_SHA512 *p,const BYTE *byte,DWORD size);

DKC_EXTERN void WINAPI dkcSHA512Final(DKC_SHA512 *p);

DKC_EXTERN int WINAPI dkcSHA512Digest(DKC_SHA512 *p,char *buff,size_t size);

DKC_EXTERN int WINAPI dkcFreeSHA512(DKC_SHA512 **);
*/



/*!
@return DKC_SHA512構造体への確保したメモリ領域
@note
すでにdkcSHA512Init()は呼ばれています。
*/
DKC_EXTERN DKC_SHA512 *WINAPI dkcAllocSHA512();
///@return dkcAllocSHA512()から取得した領域を初期化する。
DKC_EXTERN void WINAPI dkcSHA512Init(DKC_SHA512 *);
/*!
@param p[in][out] dkcAllocSHA512()で取得したポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
*/
DKC_EXTERN void WINAPI dkcSHA512Load(DKC_SHA512 *p,const BYTE *pBuffer,DWORD dwSize);


/*!
@param p[in][out] dkcAllocSHA512()で取得したポインタ
@note
dkcSHA512FinalDigest()を使用する事を奨励します。
*/
DKC_EXTERN void WINAPI dkcSHA512Final(DKC_SHA512 *p);
/*!
@param p[in][out] dkcAllocSHA512()で取得したポインタ
@param buff[out] 書き込むバッファへのポインタ
@param size[in] buffのサイズ
@return 成功したらedk_SUCCEEDEDが返る
*/
///文字列のMessage Digestを取得する。
DKC_EXTERN int WINAPI dkcSHA512DigestStr(DKC_SHA512 *p,char *buff,size_t size);

///バイナリのMessage Digestを取得する。
DKC_EXTERN int WINAPI dkcSHA512Digest(DKC_SHA512 *p,BYTE *buff,size_t size);

/*!
@see 引数、戻り値についてはdkcSHA512DigestStr()と同じです。 
@note
dkcSHA512Final()とdkcSHA512DigestStr()を使用するより、この関数の使用を奨励します。
*/
DKC_EXTERN int WINAPI dkcSHA512FinalDigestStr(DKC_SHA512 *p,char *buff,size_t size);
///@see dkcSHA512Digest() dkcSHA512FinalDigestStr()
///dkcSHA512FinalDigestStr()の binaryのハッシュ値 版
DKC_EXTERN int WINAPI dkcSHA512FinalDigest(DKC_SHA512 *p,BYTE *buff,size_t size);

/*!
@param p[in][out] dkcAllocSHA512()で取得したポインタへのポインタ
@return 上手く開放できたらedk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkcFreeSHA512(DKC_SHA512 **p);



#endif //end of include once