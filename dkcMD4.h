/*!
@file dkcMD4.h
@brief MD4 Algorithm
@author wrapped by d金魚
@note
MD4はすでに終わった？アルゴリズムなので、SHA1をご利用ください。
また、2005年度現在で安全性を考慮するならばSHA256以上にすると良いと思われます。
*/



#ifndef DKUTIL_C_MD4_H
#define DKUTIL_C_MD4_H

#include "md_misc.h"
#include "dkcOSIndependent.h"


///MD4バイナリのシグネチャ保存に必要な領域
#define MD4_BIN_BUFFER_SIZE 16
///MD4文字列のシグネチャ保存に必要な領域
#define MD4_STR_BUFFER_SIZE 33

/*
typedef struct dkc_MD4{
	uint32 count[2];
	uint32 abcd[4];
	union{
		uint64 a64[8];
		uint32 a32[16];
		uint16 a16[32];
		uint8  a8[64];
	}u_store;
	uint8 flags;
}DKC_MD4;
*/



/*!
@return DKC_MD4構造体への確保したメモリ領域
@note
すでにdkcMD4Init()は呼ばれています。
*/
DKC_EXTERN DKC_MD4 *WINAPI dkcAllocMD4();

///@return dkcAllocMD4()から取得した領域を初期化する。
DKC_EXTERN void WINAPI dkcMD4Init(DKC_MD4 *);

DKC_INLINE void dkcMD4InitEx(DKC_MD4 *p,uint8 flags)
{
	dkcMD4Init(p);
	p->flags = flags;
}

/*!
@param p[in][out] dkcAllocMD4()で取得したポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
@return edk_SUCCEEDEDで成功
@note
ライブラリの都合により、dkcMD4Load()はINT_MAXより大きいバッファを扱えないため
*/
DKC_EXTERN void WINAPI dkcMD4Load(DKC_MD4 *p,const BYTE *pBuffer,DWORD dwSize);


/*!
@param p[in][out] dkcAllocMD4()で取得したポインタ
@note
dkcMD4FinalDigest()を使用する事を奨励します。
*/
DKC_EXTERN void WINAPI dkcMD4Final(DKC_MD4 *p);
/*!
@param p[in][out] dkcAllocMD4()で取得したポインタ
@param buff[out] 書き込むバッファへのポインタ
@param size[in] buffのサイズ
@return 成功したらedk_SUCCEEDEDが返る
*/
DKC_EXTERN int WINAPI dkcMD4DigestStr(DKC_MD4 *p,char *buff,size_t size);
///@see dkcMD4DigestStr()
DKC_EXTERN int WINAPI dkcMD4Digest(DKC_MD4 *p,BYTE *buff,size_t size);

/*!
@see 引数、戻り値についてはdkcMD4Digest()と同じです。 
@note
dkcMD4Final()とdkcMD4Digest()を使用するより、この関数の使用を奨励します。
*/
DKC_EXTERN int WINAPI dkcMD4FinalDigestStr(DKC_MD4 *p,char *buff,size_t size);
///@see dkcMD4FinalDigestStr()
DKC_EXTERN int WINAPI dkcMD4FinalDigest(DKC_MD4 *p,BYTE *buff,size_t size);

/*!
@param p[in][out] dkcAllocMD4()で取得したポインタへのポインタ
@return 上手く開放できたらedk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkcFreeMD4(DKC_MD4 **p);



#endif //end of include once