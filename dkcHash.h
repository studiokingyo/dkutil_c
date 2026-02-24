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


///DKC_SHO�̖��O�͔p�~����\���A��
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
	///hash function block size (used by HMAC for pad width)
	size_t block_size;
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
@param hash_option[in] edk_SecureHash�̂ǂꂩ
@param digest_dest[out] �_�C�W�F�X�g�̏o�̓o�b�t�@
@param dest_size[in] digest_dest�ɓn�����o�b�t�@�̃T�C�Y
@param data[in] �n�b�V�����v�Z�������f�[�^
@param data_size[in] data�̃T�C�Y
*/
///�o�C�i���ł̃n�b�V�����o�͂�����
DKC_EXTERN int WINAPI dkcSecureHashCalculateBinaryDigest(
	UINT hash_option,BYTE *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size
);

///������̏o�͂����� @see ���̑��������� dkcSecureHashCalculateBinaryDigest()
DKC_EXTERN int WINAPI dkcSecureHashCalculateStringDigest(
	UINT hash_option,char *digest_dest,size_t destsize,
	const BYTE *data,size_t data_size
);																							 


DKC_EXTERN uint32 dkcHash24(uint8 *,size_t);
DKC_EXTERN uint16 dkcHash16(uint8 *,size_t);
DKC_EXTERN uint8 dkcHash8(uint8 *,size_t);

#endif //end of include once