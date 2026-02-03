/*!
@file dkcMD5.h
@brief MD5 Algorithm
@author wrapped by d金魚
@note
MD5はすでに終わった？アルゴリズムなので、SHA1をご利用ください。
また、2005年度現在で安全性を考慮するならばSHA256以上にすると良いと思われます。
*/



#ifndef DKUTIL_C_MD5_H
#define DKUTIL_C_MD5_H

#include "md_misc.h"
#include "dkcOSIndependent.h"


///MD5バイナリのシグネチャ保存に必要な領域
#define MD5_BIN_BUFFER_SIZE 16
///MD5文字列のシグネチャ保存に必要な領域
#define MD5_STR_BUFFER_SIZE 33

/*!
@return DKC_MD5構造体への確保したメモリ領域
@note
すでにdkcMD5Init()は呼ばれています。
*/
DKC_EXTERN DKC_MD5 *WINAPI dkcAllocMD5();

///@return dkcAllocMD5()から取得した領域を初期化する。
DKC_EXTERN void WINAPI dkcMD5Init(DKC_MD5 *);

DKC_INLINE void dkcMD5InitEx(DKC_MD5 *p,uint8 flags)
{
	dkcMD5Init(p);
	p->flags = flags;
}

/*!
@param p[in][out] dkcAllocMD5()で取得したポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
@return edk_SUCCEEDEDで成功
@note
ライブラリの都合により、dkcMD5Load()はINT_MAXより大きいバッファを扱えないため
*/
DKC_EXTERN void WINAPI dkcMD5Load(DKC_MD5 *p,const BYTE *pBuffer,DWORD dwSize);


/*!
@param p[in][out] dkcAllocMD5()で取得したポインタ
@note
dkcMD5FinalDigest()を使用する事を奨励します。
*/
DKC_EXTERN void WINAPI dkcMD5Final(DKC_MD5 *p);
/*!
@param p[in][out] dkcAllocMD5()で取得したポインタ
@param buff[out] 書き込むバッファへのポインタ
@param size[in] buffのサイズ
@return 成功したらedk_SUCCEEDEDが返る
*/
DKC_EXTERN int WINAPI dkcMD5DigestStr(DKC_MD5 *p,char *buff,size_t size);
///@see dkcMD5DigestStr()
DKC_EXTERN int WINAPI dkcMD5Digest(DKC_MD5 *p,BYTE *buff,size_t size);

/*!
@see 引数、戻り値についてはdkcMD5Digest()と同じです。 
@note
dkcMD5Final()とdkcMD5Digest()を使用するより、この関数の使用を奨励します。
*/
DKC_EXTERN int WINAPI dkcMD5FinalDigestStr(DKC_MD5 *p,char *buff,size_t size);
///@see dkcMD5FinalDigestStr()
DKC_EXTERN int WINAPI dkcMD5FinalDigest(DKC_MD5 *p,BYTE *buff,size_t size);

/*!
@param p[in][out] dkcAllocMD5()で取得したポインタへのポインタ
@return 上手く開放できたらedk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkcFreeMD5(DKC_MD5 **p);

///len != len2 だと落ちます。 
DKC_EXTERN void WINAPI dkcMD5LoadDouble(
	DKC_MD5 *p,uint8 const *buf, uint32 len,
	DKC_MD5 *p2,uint8 const *buf2, uint32 len2);

//**********************************************************

#ifndef DKINGYOUTILITY3_EXPORTS
typedef struct dkc_MD5_Adapter{
	//md5_state_t d;
	void *mpObj;
	uint32 mObjFlag;
	BYTE mFinalized;
}DKC_MD5_ADAPTER;

enum edkcMD5ObjectFlag
{
	/**
	デフォルトのMD5オブジェクトが生成される
	*/
	edkcMD5_Default = 0,
	///md5.cの実装が使われる
	edkcMD5_Aladdin,
};
/*!
@return DKC_MD5_ADAPTER構造体への確保したメモリ領域
@note
すでにdkcMD5Init()は呼ばれています。
*/
DKC_EXTERN DKC_MD5_ADAPTER *WINAPI dkcAllocMD5Adapter(uint32 md5_objflag);

///@return dkcAllocMD5()から取得した領域を初期化する。
DKC_EXTERN void WINAPI dkcMD5AdapterInit(DKC_MD5_ADAPTER *);
/*!
@param p[in][out] dkcAllocMD5()で取得したポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
@return edk_SUCCEEDEDで成功
@note
ライブラリの都合により、dkcMD5Load()はINT_MAXより大きいバッファを扱えないため
*/
DKC_EXTERN int WINAPI dkcMD5AdapterLoad(DKC_MD5_ADAPTER *p,const BYTE *pBuffer,DWORD dwSize);

///dkcSHA1Load() dkcSHA256Load()等と同じ仕様のLoad
DKC_EXTERN void WINAPI dkcMD5AdapterLoadStandard(DKC_MD5_ADAPTER *p,const BYTE *pBuffer,DWORD dwSize);


/*!
@param p[in][out] dkcAllocMD5()で取得したポインタ
@note
dkcMD5FinalDigest()を使用する事を奨励します。
*/
DKC_EXTERN void WINAPI dkcMD5AdapterFinal(DKC_MD5_ADAPTER *p);
/*!
@param p[in][out] dkcAllocMD5()で取得したポインタ
@param buff[out] 書き込むバッファへのポインタ
@param size[in] buffのサイズ
@return 成功したらedk_SUCCEEDEDが返る
*/
DKC_EXTERN int WINAPI dkcMD5AdapterDigestStr(DKC_MD5_ADAPTER *p,char *buff,size_t size);
///@see dkcMD5DigestStr()
DKC_EXTERN int WINAPI dkcMD5AdapterDigest(DKC_MD5_ADAPTER *p,BYTE *buff,size_t size);

/*!
@see 引数、戻り値についてはdkcMD5Digest()と同じです。 
@note
dkcMD5Final()とdkcMD5Digest()を使用するより、この関数の使用を奨励します。
*/
DKC_EXTERN int WINAPI dkcMD5AdapterFinalDigestStr(DKC_MD5_ADAPTER *p,char *buff,size_t size);
///@see dkcMD5FinalDigestStr()
DKC_EXTERN int WINAPI dkcMD5AdapterFinalDigest(DKC_MD5_ADAPTER *p,BYTE *buff,size_t size);

/*!
@param p[in][out] dkcAllocMD5()で取得したポインタへのポインタ
@return 上手く開放できたらedk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkcFreeMD5Adapter(DKC_MD5_ADAPTER **p);

#endif



#endif //end of include once