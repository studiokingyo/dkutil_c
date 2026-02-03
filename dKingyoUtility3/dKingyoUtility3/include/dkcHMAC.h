/*!
@file dkcHMAC.h
@brief HMAC: Keyed-Hashing for Message Authentication
@note
<a href="http://www.weidai.com/scan-mirror/mac.html#HMAC">HMAC</a>
 HMAC(K, text) = H(K XOR opad, H(K XOR ipad, text)) 
http://d.hatena.ne.jp/studiokingyo/20050106

疑問：HMACに使うMD値は文字列でいいんかいナ？
*/
#ifndef DKUTIL_C_HMAC_H
#define DKUTIL_C_HMAC_H

#include "dkcOSIndependent.h"
#include "dkcHash.h"

/*
typedef int (WINAPI *DKC_HMAC_INIT_F_TYPE)(void *);
typedef void (WINAPI *DKC_HMAC_LOAD_F_TYPE)(void *,const BYTE *pBuffer,DWORD dwSize);
typedef void (WINAPI *DKC_HMAC_FINAL_F_TYPE)(void *p);
typedef int (WINAPI *DKC_HMAC_DIGESTSTR_F_TYPE)(void *p,char *buff,size_t size);
typedef int (WINAPI *DKC_HMAC_DIGEST_F_TYPE)(void *p,BYTE *buff,size_t size);

#define dkcdPAD_BLOCK_SIZE 65
*/
typedef struct dkc_HMAC{
	/*
	DKC_WINAPI_PROC_F_TYPE Init;
	DKC_WINAPI_PROC_F_TYPE Load;
	DKC_WINAPI_PROC_F_TYPE Final;
	DKC_WINAPI_PROC_F_TYPE DigestStr;
	DKC_WINAPI_PROC_F_TYPE Digest;
	///string hash value size
	size_t digest_string_size;
	///binary hash value size
	size_t digest_binary_size;
	*/
	DKC_SECURE_HASH_OBJECT *sho;
	////often 0x36
	BYTE ipad_init;
	//BYTE ipad[dkcdPAD_BLOCK_SIZE];
	BYTE *ipad;
	///often 0x5c
	BYTE opad_init;
	///
	//BYTE opad[dkcdPAD_BLOCK_SIZE];
	BYTE *opad;
	///ipad opadのサイズ
	size_t pad_size;


	BYTE mInnerHashKeyed;
	BYTE mInited;

}DKC_HMAC;


///@note MD5は既にクラック？されたので使用の奨励はしない。SHA512の使用を奨励する。
DKC_EXTERN void WINAPI dkcHMAC_MD5Init(DKC_HMAC *);
DKC_EXTERN void WINAPI dkcHMAC_SHA1Init(DKC_HMAC *);
DKC_EXTERN void WINAPI dkcHMAC_SHA256Init(DKC_HMAC *);
DKC_EXTERN void WINAPI dkcHMAC_SHA384Init(DKC_HMAC *);
///これによるHMACの生成を奨励する。
DKC_EXTERN void WINAPI dkcHMAC_SHA512Init(DKC_HMAC *);

typedef void (WINAPI *DKC_HMAC_HASH_INIT_F_TYPE)(DKC_HMAC *);


///@param option[in] edk_SecureHashを使用
DKC_EXTERN DKC_HMAC *WINAPI 	dkcAllocHMAC(UINT option);

/*
DKC_EXTERN DKC_HMAC *WINAPI 
	dkcAllocHMAC(
		DKC_WINAPI_PROC_F_TYPE Init,
		DKC_WINAPI_PROC_F_TYPE Load,
		DKC_WINAPI_PROC_F_TYPE Final,
		DKC_WINAPI_PROC_F_TYPE DigestStr,
		DKC_WINAPI_PROC_F_TYPE Digest,
		BYTE ipad,
		BYTE opad,
		size_t digest_size,
		size_t block_size,
		DKC_HMAC_INIT_F_TYPE init_func
);
*/
/*!
@param p[in][out] dkcAllocHMAC()で取得したポインタへのポインタ
@return 上手く開放できたらedk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkcFreeHMAC(DKC_HMAC **p);


				 
/**
@param p[in][out] dkcAllocHMAC()で取得したポインタ
*/
///DKC_HMACを初期化する。<s>別にdkcAllocHMAC()した後 即dkcHMACLoad()を呼んでも問題無い。</s>
DKC_EXTERN void WINAPI dkcHMACInit(DKC_HMAC *p,const BYTE *key,size_t key_length);

/**
@param p[in][out] dkcAllocHMAC()で取得したポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
*/
DKC_EXTERN void WINAPI dkcHMACLoad(DKC_HMAC *p,const BYTE *pBuffer,DWORD dwSize);


/*!
@param p[in][out] dkcAllocHMAC()で取得したポインタ
@note
dkcHMACFinalDigest()を使用する事を奨励します。
*/
DKC_EXTERN int WINAPI dkcHMACFinal(DKC_HMAC *p);
/*!
@param p[in][out] dkcAllocHMAC()で取得したポインタ
@param buff[out] 書き込むバッファへのポインタ
@param size[in] buffのサイズ
@return 成功したらedk_SUCCEEDEDが返る
*/
///文字列のMessage Digestを取得する。
DKC_EXTERN int WINAPI dkcHMACDigestStr(DKC_HMAC *p,char *buff,size_t size);

///バイナリのMessage Digestを取得する。
DKC_EXTERN int WINAPI dkcHMACDigest(DKC_HMAC *p,BYTE *buff,size_t size);

DKC_EXTERN int WINAPI dkcHMACFinalDigestStr(DKC_HMAC *p,char *buff,size_t size);

DKC_EXTERN int WINAPI dkcHMACFinalDigest(DKC_HMAC *p,BYTE *buff,size_t size);




/**
@param hash_option[in] edk_SecureHashのどれか
@param digest_dest[out] ダイジェストの出力バッファ
@param dest_size[in] digest_destに渡したバッファのサイズ
@param data[in] ハッシュを計算したいデータ
@param data_size[in] dataのサイズ
@param key[in] キーのサイズ
@param keysize[in] keyのbyte単位のサイズ
*/
///バイナリでのHMAC値を出力をする
DKC_EXTERN int WINAPI dkcHMACCalculateBinaryDigest(
	UINT hash_option,BYTE *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size,const BYTE *key,size_t keysize
);

///文字列のHMAC値を出力をする @see その他引数等は dkcHMACCalculateBinaryDigest()
DKC_EXTERN int WINAPI dkcHMACHashCalculateStringDigest(
	UINT hash_option,char *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size,const BYTE *key,size_t keysize
);

DKC_EXTERN int WINAPI dkcHMAC(DKC_HMAC *,const BYTE *data,size_t data_size,const BYTE *key,size_t key_size,
						 BYTE *bin_result_buff,size_t buffsize);

DKC_EXTERN int WINAPI dkcHMACStr(DKC_HMAC *p,const BYTE *data,size_t data_size,const BYTE *key,size_t key_size,
						 char *str_result_buff,size_t buffsize);

#endif //end of include once