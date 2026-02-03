/*!
@file dkcDCF01.h
@brief dKingyo Cryption File Version01
@note
dKingyo Cryption File Version01形式の実装
version01
データ構造
 HEADER01
 暗号化済みデータ
暗号化にはSNOW2を使用する。
<br>
同じファイルを別々のパスワードで暗号化したりすると
パスワードかロジックを解析される可能性があるかも？
なのでその場合は二重暗号化する等してください。
@since 2005/11/22
*/
#ifndef DKUTIL_C_DCF01_H
#define DKUTIL_C_DCF01_H

#include "dcf_misc.h"

typedef struct dkc_dKingyoCryptionFileHeader01{
	uint8 hash_value[SHA512_BIN_BUFFER_SIZE];
	uint64 data_size;
}DKC_DCF_HEADER01;


/**
@param attach_cp[in] DKC_SNOW2オブジェクトへのポインタ
@param attach_sp[in] DKC_STREAMオブジェクトへのポインタ　あらかじめ書き込みか読み込みか、およびedkcStreamInitFile64で初期化してください。
*/
DKC_EXTERN DKC_DCF *WINAPI dkcAllocDCF_Version01_SNOW2(DKC_SNOW2 *attach_cp,DKC_STREAM *attach_sp);

DKC_EXTERN DKC_DCF *WINAPI dkcAllocDCF01(const char *filename,uint32 stream_flags,const void *key,size_t keysize);

DKC_EXTERN int WINAPI dkcFreeDCF01(DKC_DCF **pp);

/**
内部メッセージダイジェスト関数を初期化する
*/
DKC_EXTERN void WINAPI dkcDCF01Init(DKC_DCF *p);


/**
@return edk_SUCCEEDEDで成功。
失敗したらdkcDCFInit()を呼び出して最初から処理しなおしてください。
*/
DKC_EXTERN int WINAPI dkcDCF01Write(DKC_DCF *p,const void *data,size_t size,size_t *write_size);
/**
@return edk_SUCCEEDEDでハッシュ書き込み成功
失敗したらdkcDCFInit()を呼び出して最初から処理しなおしてください。
*/
DKC_EXTERN int WINAPI dkcDCF01WriteFinal(DKC_DCF *p);
/**
@return edk_EndProcessですべて読み込んだ。edk_SUCCEEDEDで正常処理中。
戻り値が前述以外だったらdkcDCFInit()を呼び出して最初から処理しなおしてください。
*/
DKC_EXTERN int WINAPI dkcDCF01Read(DKC_DCF *p,void *buff,size_t size,size_t *readsize);

/**
@return edk_SUCCEEDEDでハッシュチェック完了。複合化したデータは正常。
*/
DKC_EXTERN int WINAPI dkcDCF01ReadFinal(DKC_DCF *p);

//DKC_EXTERN BOOL WINAPI dkcDCF01PasswordCheck(const char *filename,const void *password,size_t size);

DKC_EXTERN int WINAPI dkcFileToDCF01(const char *save_filename,const char *target_filename,
					 const void *key,size_t keysize);

DKC_EXTERN int WINAPI dkcDCF01ToFile(const char *dest_filename,const char *target_filename,
					 const void *key,size_t keysize);



#endif //end of include once