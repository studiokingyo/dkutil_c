/*!
@file dkcSHA384.h
@brief SHA 384
@author d金魚
*/
#ifndef DKUTIL_C_SHA384_H
#define DKUTIL_C_SHA384_H

#include "dkcOSIndependent.h"
#include "dkcSHA512.h"

#define SHA384_HASH  SHA512_HASH
#define SHA384_BLOCK SHA512_BLOCK
#define SHA384_WORK  SHA512_WORK




///SHA1の生ハッシュ値(binary)に必要なバッファサイズ
#define SHA384_BIN_BUFFER_SIZE (SHA384_HASH * 6)//48
///SHA1のハッシュ値文字列に必要なバッファサイズ
#define SHA384_STR_BUFFER_SIZE (SHA384_HASH * 12 + 1)

//構造が同じ･･･。
typedef DKC_SHA512 DKC_SHA384;


/*
typedef struct dkc_SHA384{
	QWORD m_dwH[SHA384_HASH];
	QWORD m_dwLNumBits;
	QWORD m_dwHNumBits;
	QWORD m_aBlock[SHA384_BLOCK];
	int   m_nNumChr;
	BYTE mFinalized;
}DKC_SHA384;
*/

/*!
@return DKC_SHA384構造体への確保したメモリ領域
@note
すでにdkcSHA384Init()は呼ばれています。
*/
DKC_EXTERN DKC_SHA384 *WINAPI dkcAllocSHA384();
///@return dkcAllocSHA384()から取得した領域を初期化する。
DKC_EXTERN void WINAPI dkcSHA384Init(DKC_SHA384 *);
/*!
@param p[in][out] dkcAllocSHA384()で取得したポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
*/
DKC_EXTERN void WINAPI dkcSHA384Load(DKC_SHA384 *p,const BYTE *pBuffer,DWORD dwSize);


/*!
@param p[in][out] dkcAllocSHA384()で取得したポインタ
@note
dkcSHA384FinalDigest()を使用する事を奨励します。
*/
DKC_EXTERN void WINAPI dkcSHA384Final(DKC_SHA384 *p);
/*!
@param p[in][out] dkcAllocSHA384()で取得したポインタ
@param buff[out] 書き込むバッファへのポインタ
@param size[in] buffのサイズ
@return 成功したらedk_SUCCEEDEDが返る
*/
DKC_EXTERN int WINAPI dkcSHA384DigestStr(DKC_SHA384 *p,char *buff,size_t size);
///binaryハッシュ値 版 @see dkcSHA384DigestStr()
DKC_EXTERN int WINAPI dkcSHA384Digest(DKC_SHA384 *p,BYTE *buff,size_t size);

/*!
@see 引数、戻り値についてはdkcSHA384Digest()と同じです。 
@note
dkcSHA384Final()とdkcSHA384Digest()を使用するより、この関数の使用を奨励します。
*/
DKC_EXTERN int WINAPI dkcSHA384FinalDigestStr(DKC_SHA384 *p,char *buff,size_t size);

///binaryハッシュ値 版 @see dkcSHA384FinalDigestStr()
DKC_EXTERN int WINAPI dkcSHA384FinalDigest(DKC_SHA384 *p,BYTE *buff,size_t size);

/*!
@param p[in][out] dkcAllocSHA384()で取得したポインタへのポインタ
@return 上手く開放できたらedk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkcFreeSHA384(DKC_SHA384 **p);




#endif //end of include once