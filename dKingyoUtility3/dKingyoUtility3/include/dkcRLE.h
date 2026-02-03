/**
@file dkcRLE.h
@brief Run Length Encode 圧縮アルゴリズム
@author d金魚
@note

dkcRLEのRLEの仕様

デコード

DKC_RLE_HEADERを元に解析
<ul>
<li>圧縮の場合
連続数 (1byte) 0,1はありえない
データ (1byte) 0 - 255まで
<li>非圧縮の場合
非圧縮識別値 BYTE		(1byte) dkcdRLE_NOCOMPRESS_IDとする
データの長さ BYTE (1byte) 2Nまで
残りはデータ ( 2N byte ... )
</ul>

ヘッダに記述されていた終端記号(dkcdRLE_END_IDで終了。
ヘッダに記述されていたサイズ通りでない場合はエラー(各関数を参照)を返して終了

DKC_RLE_PACKBITS_HEADERを元に解析
圧縮の場合
unsignedな場合0x80でマスクとって TRUEだったらn- 0x80 のRun長が圧縮されている
非圧縮の場合
unsignedな場合0x80でマスクとって FALSEだったらnがデータである。
*/
#ifndef DKUTIL_C_RLE_H
#define DKUTIL_C_RLE_H


#include "dkcOSIndependent.h"
#include "dkcMemoryStream.h"

#define dkcdRLE_EOF_ID 0x01

#define dkcdRLE_NOCOMPRESS_ID 0x00
///RLEのシグネチャ（変更しても大丈夫だよ～（多分))
#define dkcdRLE_SIGNATURE 'RLE\0'

typedef struct dkc_RLE
{
	///テンポラリバッファ
	BYTE mTemp[256];
	///有効フラグ
	//unsigned char mValidFlag[256];
}DKC_RLE;



typedef struct dkc_RLE_Header{
	///シグネチャ
	unsigned long mSignature;
	///元のサイズ
	USHORT mOriginSize;
	///圧縮したサイズ
	USHORT mCompressedSize;

	///終端 識別値
	BYTE mEOF;

	///非圧縮 識別値
	BYTE mABS;
}DKC_RLE_HEADER;

typedef struct dkc_RLE_Comp{
	BYTE length;
	BYTE data;
}DKC_RLE_COMP;

typedef struct dkc_RLE_NoComp{
	BYTE sig;
	USHORT length;
}DKC_RLE_NOCOMP;


DKC_EXTERN DKC_RLE *WINAPI dkcAllocRLE();

DKC_EXTERN int WINAPI dkcFreeRLE(DKC_RLE **);




/*!
@param ptr[in][out] dkcAllocRLE()で取得したポインタ
@param ph[out] DKC_RLE_HEADERへのポインタ
@param dest[out] デコードされたデータの書き出しバッファへのポインタ
@param dsize[in] destのサイズ (srcの2倍用意してください。)
@param src[in] バッファへのポインタ
@param ssize[in] srcのサイズ
@param sig[in] dkcRLEEncode()の時、指定したシグネチャID
@param CloseProcessSize[in] 処理を打ち切りにするサイズ 
@param aEOF_ID[in] RLEのEOFを示す値　dkcdRLE_EOF_IDを指定する事を奨励する
@param aABS_ID[in] RLEの非圧縮を示す値 dkcdRLE_NOCOMPRESS_IDを指定する事を奨励する
@return
戻り値はedk_SUCCEEDEDで成功。
どちらの条件でも処理を打ち切りにするサイズを超えたらedk_NoValueToProcessが返る。
それ以外はその戻り値によって各自判断。
@note
DKC_RLE_HEADERのメンバのmOriginSizeよりdsizeが小さいとこの関数は失敗します。
チョットしたネタ：
dkcRLEEncodeって命名なんか変じゃない？ Run Length Encode Encode ... ?? まぁ、仕様って事で･･･
*/
DKC_EXTERN int WINAPI dkcRLEEncode(DKC_RLE *ptr,DKC_MEMORYSTREAM *pms,DKC_RLE_HEADER *ph,
																	 BYTE *dest,size_t dsize,const BYTE *src,USHORT ssize,
	size_t CloseProcessSize,ULONG sig,BYTE aEOF_ID,BYTE aABS_ID);
/*!
@param ptr[in][out] dkcAllocRLE()で取得したポインタ
@param pms[in][out] dkcAllocMemoryStream()で取得したMemory Stream Objectへのポインタ
@param ph[out] DKC_RLE_HEADERへのポインタ
@param src[in] バッファへのポインタ
@param ssize[in] srcのサイズ
@param sig[in] RLEだと証明するシグネチャ
@return edk_Resultのどれかが返る。圧縮する価値の無い場合はedk_NoValueToProcessが返る。
*/
DKC_EXTERN int WINAPI dkcRLEDecode(DKC_RLE *p,DKC_MEMORYSTREAM *pms,
												const DKC_RLE_HEADER *ph,const BYTE *src,USHORT ssize,
												ULONG sig);

typedef struct dkc_RLEPackBits_Header{
	///元のサイズ
	size_t mOriginSize;
	///圧縮したサイズ
	size_t mCompressedSize;
	///何個続いたらRLEするか
	int mCount;
}DKC_RLE_PACKBITS_HEADER;


DKC_EXTERN int WINAPI dkcRLEPackBitsEncode(DKC_RLE_PACKBITS_HEADER *p,
																					 BYTE *dest,size_t dsize,
																						const BYTE *src,size_t ssize,BYTE count);

DKC_EXTERN int WINAPI dkcRLEPackBitsDecode(DKC_RLE_PACKBITS_HEADER *p,
																					 BYTE *dest,size_t dsize,
																const BYTE *src,size_t ssize);

#if !defined(  DKUTIL_C_RLE_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcRLE.c"
#endif

#endif //end of include once