/*!
@file dkcTelecomConfirmPacket.h
@brief 通信のときに使う独自パケットフォーマット
@since 2005/09/01
*/
#ifndef  DKUTIL_C_TELECOM_CONFIRM_PACKET_H
#define  DKUTIL_C_TELECOM_CONFIRM_PACKET_H

#include "dkcHash.h"
#include "dkcCryptograph.h"


//
#if 0

#define dkcdTELECOM_CONFIRM_PACKET_MAX_LENGTH 0x7FFFFFFF

/**
@note
1秒で64KB稼げるとしてSHA512の計算を30秒毎にするとすると1966080byteがよろしいかな？
*/
#define dkcdTELECOM_CONFIRM_PACKET_DEFAULT_PAUSE_SIZE 1966080

typedef struct dkc_TelecomConfirmPacketHeader{
///シグネチャ（length込,圧縮、暗号化前のプレーンな状態のもの）
	uint8 sig[SHA512_BIN_BUFFER_SIZE];
	
	///このヘッダの後のサイズ
	uint32 length;
}DKC_TELECOM_CONFIRM_PACKET_HEADER;

	
/**
仕様：
リトルエンディアン

*/
///データ通信確認機能つきパケット
typedef struct dkc_TelecomConfirmPacket{
	///パケットの区切りサイズ（ヘッダー込）
	size_t pause_size;
	//フラグ 
	//UINT flag;
	///signature generator
	DKC_SHA512 *psig;
	///ランダムシグネチャID用種値
	ULONG seed;
	

	///キーの値

	///SHA512したキー
	uint8 key512[SHA512_BIN_BUFFER_SIZE];
	/*
	///キーのサイズ
	size_t keysize;
	///生のキー
	uint8 *pkey;
	///SHA256したキー
	uint8 key256[SHA256_BIN_BUFFER_SIZE];
	uint8 key160[SHA160_BIN_BUFFER_SIZE];
	*/

	///暗号化のフラグ
	UINT flag;
	///暗号化オブジェクトへのポインタ
	void *crypt_obj;
	uint8 isNaturalKey;
}DKC_TELECOM_CONFIRM_PACKET;

/*
@param pause_size[in] 区切りサイズ
@param flag[in] enum edk_ProcessSignaturesのどれかを入れる
@param key[in] 暗号化キー(圧縮時も必ず入れてください）
@param keysize[in] キーのサイズ

DKC_EXTERN DKC_TELECOM_CONFIRM_PACKET* WINAPI dkcAllocTelecomConfirmPacket(size_t pause_size,UINT flag,const void *key,size_t keysize,BOOL isNaturalKey);
*/

/**
@param pause_size[in] 区切りサイズ
@param key[in] 暗号化キー(圧縮時も必ず入れてください）
@param keysize[in] キーのサイズ
@param flag[in] enum edk_ProcessSignaturesの暗号化系のどれかひとつを入れる
@param isNaturalKey[in] そのままのキーを暗号化キーとしてEncodeDecode時に使うのならばTRUE
*/
DKC_EXTERN DKC_TELECOM_CONFIRM_PACKET* WINAPI dkcAllocTelecomConfirmPacket(size_t pause_size,const void *key,size_t keysize,UINT flag,BOOL isNaturalKey);

DKC_EXTERN DKC_TELECOM_CONFIRM_PACKET* WINAPI dkcAllocTelecomConfirmPacketAuto(const void *key,size_t keysize);

DKC_EXTERN int WINAPI dkcFreeTelecomConfirmPacket(DKC_TELECOM_CONFIRM_PACKET **);



DKC_EXTERN int WINAPI dkcTelecomConfirmPacketEncode(DKC_TELECOM_CONFIRM_PACKET *p,
																				 uint8 *dest,size_t destsize,const uint8 *src,size_t srcsize);


DKC_EXTERN int WINAPI dkcTelecomConfirmPacketDecode(DKC_TELECOM_CONFIRM_PACKET *p,
																				 uint8 *dest,size_t destsize,const uint8 *src,size_t srcsize);

																				 
DKC_EXTERN size_t WINAPI dkcGetOutputSize(const uint8 *src);
																				 
/*
DKC_EXTERN int WINAPI dkcTelecomConfirmPacketEncode(uint8 *dest,size_t destsize,const uint8 *src,size_t srcsize);

DKC_EXTERN int WINAPI dkcTelecomConfirmPacketDecode(uint8 *dest,size_t destsize,const uint8 *src,size_t srcsize);
*/


#if !defined( DKUTIL_C_TELECOM_CONFIRM_PACKET_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcTelecomConfirmPacket.c"
#endif

#endif //end of if0

#endif //end of include once