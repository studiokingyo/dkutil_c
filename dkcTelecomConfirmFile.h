/*!
@file dkcTelecomConfirmFile.h
@brief 通信のときに使うファイルフォーマット
@since 2005/11/17
@note
version 01
 header
 block
*/
#ifndef  DKUTIL_C_TELECOM_CONFIRM_FILE_H
#define  DKUTIL_C_TELECOM_CONFIRM_FILE_H

#include "dkcHash.h"
#include "dkcLZW.h"
#include "dkcStream.h"

#if 0


#define dkcdTELECOM_CONFIRM_FILE_MAX_LENGTH 0x7FFFFFFF

/**
@note
1秒で64KB稼げるとしてSHA512の計算を30秒毎にするとすると1966080byteがよろしいかな？
*/
#define dkcdTELECOM_CONFIRM_FILE_DEFAULT_PAUSE_SIZE 1966080

typedef struct dkc_TelecomConfirmFileHeader01{
	uint32 version;
	///ヘッダ自体のシグネチャ
	uint8 header_sig[SHA512_BIN_BUFFER_SIZE];
	///シグネチャ 圧縮、暗号化前のプレーンな状態のもの）
	uint8 origin_sig[SHA512_BIN_BUFFER_SIZE];
	///このヘッダの後のサイズ
	uint64 length;
	///blockの数
	uint32 block_num;
	///lzwの引数
	uint32 lzw_output_block_size;
}DKC_TELECOM_CONFIRM_FILE_HEADER01;

typedef struct dkc_TelecomConfirmFileBlock01{
	DKC_LZW_HEADER lzw_header;
	///LZWで圧縮済みのデータのMD5値 MD5なのはこちらの方が処理が速いと思われたため。
	uint8 compress_md5[MD5_BIN_BUFFER_SIZE];
	///LZWで解凍されたデータのMD5値
	uint8 origin_md5[MD5_BIN_BUFFER_SIZE];
}DKC_TELECOM_CONFIRM_FILE_BLOCK01;

#define dkcdTELECOM_CONFIRM_FILE_BEGIN_BUFFER_SIZE (1024 * 64)

///
typedef struct dkc_TelecomConfirmFile{
	//setting
	uint32 version;

	//save
	uint32 block_num;
	uint32 lzw_output_block_size;
	//state
	DKC_LZW *mLZW;
	DKC_MD5 *mMD5;
	DKC_SHA512 *mSHA512;

	///OutputBlockする時に使うLZWのdest用
	DKC_BUFFER *mBuff;


}DKC_TELECOM_CONFIRM_FILE;


enum edkcTelecomConfirmFile{
	///all sha512 / block md5 lzw
	edkcTelecomConfirmFileVersion01 = 1,
	edkcTelecomConfirmFileVersionNewest =  edkcTelecomConfirmFileVersion01,
};
#define dkcmTelecomConfirmFileVersionError(a) (a < edkcTelecomConfirmFileVersion01 && edkcTelecomConfirmFileVersionNewest < a)

/**
@param version[in] version値を入れる。enum edkcTelecomConfirmFileのVersionがついている値のどれかを入れる。なるべく新しい値を入れよう。
*/
DKC_EXTERN DKC_TELECOM_CONFIRM_FILE* WINAPI dkcAllocTelecomConfirmFileGenerate(uint32 version,size_t lzw_output_block_size);

DKC_EXTERN DKC_TELECOM_CONFIRM_FILE *WINAPI dkcAllocTelecomConfirmFileExtract(DKC_STREAM *input);

DKC_EXTERN int WINAPI dkcFreeTelecomConfirmFile(DKC_TELECOM_CONFIRM_FILE **);

DKC_EXTERN int WINAPI dkcTelecomConfirmFileInit(DKC_TELECOM_CONFIRM_FILE *p);


///@warning outputするstreamはdkcStreamSeek(output,0,edkcStreamSeekSet);としておくこと
DKC_EXTERN int WINAPI dkcTelecomConfirmFileGenerateBegin(DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *output);

DKC_EXTERN int WINAPI dkcTelecomConfirmFileGenerateOutputBlock(DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *output,const uint8 *buff,size_t size);

DKC_EXTERN int WINAPI dkcTelecomConfirmFileGenerateFinal(DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *output);

/*
DKC_EXTERN int WINAPI dkcTelecomConfirmFileGetHeader(DKC_STREAM *input,DKC_TELECOM_CONFIRM_FILE_HEADER01 *header);

DKC_EXTERN int WINAPI dkcTelecomConfirmFileGetBlock(DKC_STREAM *input,DKC_TELECOM_CONFIRM_FILE_BLOCK01 *block);
*/

DKC_EXTERN int WINAPI dkcTelecomConfirmFileExtractOutputBlock(
	DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *input,const DKC_TELECOM_CONFIRM_FILE_BLOCK *block,
	uint8 *buff,size_t buffsize
);


DKC_EXTERN int WINAPI dkcTelecomConfirmFileEncode(const char *dest_filename,const char *src_filename);


DKC_EXTERN int WINAPI dkcTelecomConfirmFileDecode(const char *dest_filename,const char *src_filename);

																				 
DKC_EXTERN size_t WINAPI dkcTelecomConfirmFileGetDecodeSize(const char *filename);
																				 
/*
DKC_EXTERN int WINAPI dkcTelecomConfirmFileEncode(uint8 *dest,size_t destsize,const uint8 *src,size_t srcsize);

DKC_EXTERN int WINAPI dkcTelecomConfirmFileDecode(uint8 *dest,size_t destsize,const uint8 *src,size_t srcsize);
*/


#if !defined( DKUTIL_C_TELECOM_CONFIRM_FILE_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcTelecomConfirmFile.c"
#endif

#endif //eo0

#endif //end of include once