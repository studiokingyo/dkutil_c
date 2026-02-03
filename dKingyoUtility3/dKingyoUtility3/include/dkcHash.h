/*!
@file dkcHash.h
@brief lots of Hash Algorithm
*/
#ifndef dkutil_c_Hash
#define dkutil_c_Hash

#include "dkcOSIndependent.h"
#include "st.h"
#include "dkcSHA.h"
#include "dkcMessageDigest.h"


typedef int (WINAPI *DKC_SHO_INIT_F_TYPE)(void *);
typedef void (WINAPI *DKC_SHO_LOAD_F_TYPE)(void *,const BYTE *pBuffer,DWORD dwSize);
typedef void (WINAPI *DKC_SHO_FINAL_F_TYPE)(void *p);
typedef int (WINAPI *DKC_SHO_DIGESTSTR_F_TYPE)(void *p,char *buff,size_t size);
typedef int (WINAPI *DKC_SHO_DIGEST_F_TYPE)(void *p,BYTE *buff,size_t size);


///DKC_SHOの名前は廃止する可能性アリ
typedef struct dkc_SecureHashObject{
	DKC_WINAPI_PROC_F_TYPE Init;
	DKC_WINAPI_PROC_F_TYPE Load;
	DKC_WINAPI_PROC_F_TYPE Final;
	DKC_WINAPI_PROC_F_TYPE DigestStr;
	DKC_WINAPI_PROC_F_TYPE Digest;
	///string hash value size
	size_t digest_string_size;
	///binary hash value size
	size_t digest_binary_size;
	UINT mOption;
	void *mObj;
	BYTE mInited;
}DKC_SECURE_HASH_OBJECT,DKC_SHO;


DKC_SECURE_HASH_OBJECT *WINAPI dkcAllocSHO(UINT option);

int WINAPI dkcFreeSHO(DKC_SECURE_HASH_OBJECT **pp);

void WINAPI dkcSHOInit(DKC_SECURE_HASH_OBJECT *p);

void WINAPI dkcSHOLoad(DKC_SECURE_HASH_OBJECT *p,const BYTE *pBuffer,DWORD dwSize);

void WINAPI dkcSHOFinal(DKC_SECURE_HASH_OBJECT *p);

int WINAPI dkcSHODigestStr(DKC_SECURE_HASH_OBJECT *p,char *buff,size_t size);

int WINAPI dkcSHODigest(DKC_SECURE_HASH_OBJECT *p,BYTE *buff,size_t size);

/**
@param hash_option[in] edk_SecureHashのどれか
@param digest_dest[out] ダイジェストの出力バッファ
@param dest_size[in] digest_destに渡したバッファのサイズ
@param data[in] ハッシュを計算したいデータ
@param data_size[in] dataのサイズ
*/
///バイナリでのハッシュを出力をする
DKC_EXTERN int WINAPI dkcSecureHashCalculateBinaryDigest(
	UINT hash_option,BYTE *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size
);

///文字列の出力をする @see その他引数等は dkcSecureHashCalculateBinaryDigest()
DKC_EXTERN int WINAPI dkcSecureHashCalculateStringDigest(
	UINT hash_option,char *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size
);																							 


DKC_EXTERN uint32 dkcHash24(uint8 *,size_t);
DKC_EXTERN uint16 dkcHash16(uint8 *,size_t);
DKC_EXTERN uint8 dkcHash8(uint8 *,size_t);


#if 0//defined(__cplusplus)

namespace dkutil{

///とりあえず、雰囲気だけ･･･
class map_st{
	st_table *mM;
public:
	map_st(){
		mM = st_init_strtable();
	}
	bool reset(){
		clear();
		mM = st_init_strtable();
	}
	bool insert(const char *key,const char *data){
		
	}
	void clear(){
		if(mM){
			st_free_table(mM);
		}
	}
	bool lookup(const char *key,char *buff,size_t size){
		st_lookup(m, key, &buff);
	}


};



}//end of namespace


#endif

#endif //end of include once