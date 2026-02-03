/*!
@file dkcMD2.h
@brief MD2 Algorithm
@author wrapped by d金魚
@note
MD2はすでに終わった？アルゴリズムなので、SHA1をご利用ください。
また、安全性を考慮するならばSHA256以上にすると良いと思われます。
*/



#ifndef DKUTIL_C_MD2_H
#define DKUTIL_C_MD2_H

#include "md_misc.h"
#include "dkcOSIndependent.h"


///MD2バイナリのシグネチャ保存に必要な領域
#define MD2_BIN_BUFFER_SIZE 16
///MD2文字列のシグネチャ保存に必要な領域
#define MD2_STR_BUFFER_SIZE 33


typedef struct dkc_MD2{
  	
	uint8 C[16];
	uint8 X[48];
	//int count;
	unsigned int count;
	uint8 buf[16];
	uint8 flags;
}DKC_MD2;


/*!
@return DKC_MD2構造体への確保したメモリ領域
@note
すでにdkcMD2Init()は呼ばれています。
*/
DKC_EXTERN DKC_MD2 *WINAPI dkcAllocMD2();

///@return dkcAllocMD2()から取得した領域を初期化する。
DKC_EXTERN void WINAPI dkcMD2Init(DKC_MD2 *);

DKC_INLINE void dkcMD2InitEx(DKC_MD2 *p,uint8 flags)
{
	dkcMD2Init(p);
	p->flags = flags;
}

/*!
@param p[in][out] dkcAllocMD2()で取得したポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
@return edk_SUCCEEDEDで成功
@note
ライブラリの都合により、dkcMD2Load()はINT_MAXより大きいバッファを扱えないため
*/
DKC_EXTERN void WINAPI dkcMD2Load(DKC_MD2 *p,const BYTE *pBuffer,DWORD dwSize);


/*!
@param p[in][out] dkcAllocMD2()で取得したポインタ
@note
dkcMD2FinalDigest()を使用する事を奨励します。
*/
DKC_EXTERN void WINAPI dkcMD2Final(DKC_MD2 *p);
/*!
@param p[in][out] dkcAllocMD2()で取得したポインタ
@param buff[out] 書き込むバッファへのポインタ
@param size[in] buffのサイズ
@return 成功したらedk_SUCCEEDEDが返る
*/
DKC_EXTERN int WINAPI dkcMD2DigestStr(DKC_MD2 *p,char *buff,size_t size);
///@see dkcMD2DigestStr()
DKC_EXTERN int WINAPI dkcMD2Digest(DKC_MD2 *p,BYTE *buff,size_t size);

/*!
@see 引数、戻り値についてはdkcMD2Digest()と同じです。 
@note
dkcMD2Final()とdkcMD2Digest()を使用するより、この関数の使用を奨励します。
*/
DKC_EXTERN int WINAPI dkcMD2FinalDigestStr(DKC_MD2 *p,char *buff,size_t size);
///@see dkcMD2FinalDigestStr()
DKC_EXTERN int WINAPI dkcMD2FinalDigest(DKC_MD2 *p,BYTE *buff,size_t size);

/*!
@param p[in][out] dkcAllocMD2()で取得したポインタへのポインタ
@return 上手く開放できたらedk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkcFreeMD2(DKC_MD2 **p);



#endif //end of include once