/*!
@file dkcLZSS.h
@since 2004/04/20
@author Original:Lee Noriyuki / Reconstruction:d金魚
@brief LZSS圧縮

*/
#ifndef DKUTIL_C_LZSS_H
#define DKUTIL_C_LZSS_H


#include "dkcOSIndependent.h"

///< 環状バッファの大きさ
#define LZSS_RING_LENGTH		4096
///< 環状バッファのマスク
#define LZSS_RING_MASK			(LZSS_RING_LENGTH-1)
///< 最長一致長
#define LZSS_LONGEST_MATCH		16
///LZSSのシグネチャ（変更しても大丈夫だよ～（多分))
#define dkcdLZSS_SIGNATURE edk_LZSS_SIGNATURE
//#define dkcdLZSS_SIGNATURE 'LZSS'

typedef struct dkc_LZSS
{
	///< 圧縮解凍用ワークデータポインタ
	long Dad[LZSS_RING_LENGTH+1];								
	///< 圧縮解凍用ワークデータポインタ
	long LSon[LZSS_RING_LENGTH+1];								
	///< 圧縮解凍用ワークデータポインタ
	long RSon[LZSS_RING_LENGTH+257];								

	///< 最長一致位置
	long MatchPos;												
	///< 最長一致長
	long MatchLen;													
	///< テキストデータポインタ
	unsigned char Text[LZSS_RING_LENGTH + LZSS_LONGEST_MATCH - 1];
}DKC_LZSS;

typedef struct dkc_LZSS_Header{
	unsigned long mSignature;
	///元のサイズ
	size_t mOriginSize;
	///圧縮したサイズ
	size_t mCompressedSize;

}DKC_LZSS_HEADER;


DKC_EXTERN DKC_LZSS *WINAPI dkcAllocLZSS();

DKC_EXTERN int WINAPI dkcFreeLZSS(DKC_LZSS **);
/*!
@param ptr[in][out] dkcAllocLZSS()で取得したポインタ
@param ph[out] DKC_LZSS_HEADERへのポインタ
@param dest[out] デコードされたデータの書き出しバッファへのポインタ
@param dsize[in] destのサイズ
@param src[in] バッファへのポインタ
@param ssize[in] srcのサイズ
@param sig[in] dkcLZSSEncode()の時、指定したシグネチャID
@return edk_Resultのどれかが返る。
@note
DKC_LZSS_HEADERのメンバのmOriginSizeよりdsizeが小さいとこの関数は失敗します。
*/
DKC_EXTERN int WINAPI dkcLZSSDecode(DKC_LZSS *ptr,DKC_LZSS_HEADER *ph,
	BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,ULONG sig);
/*!
@param ptr[in][out] dkcAllocLZSS()で取得したポインタ
@param ph[out] DKC_LZSS_HEADERへのポインタ
@param dest[out] LZSSで圧縮されたデータの書き出しバッファへのポインタ
@param dsize[in] destのサイズ
@param src[in] バッファへのポインタ
@param ssize[in] srcのサイズ
@param CloseProcessSize[in] 処理を打ち切りにするサイズ dsizeより小さくするのがセオリー
@param sig[in] LZSSだと証明するシグネチャ
@return edk_Resultのどれかが返る。圧縮する価値の無い場合はedk_NoValueToProcessが返る。
*/
DKC_EXTERN int WINAPI dkcLZSSEncode(DKC_LZSS *ptr,DKC_LZSS_HEADER *ph,
	BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,
	size_t CloseProcessSize,ULONG sig);

//headerがLZSSのシグネチャかどうか確認する @return TRUEでLZSSデータだと思う。
//廃止：シグネチャは自由に設定できるように！！
//DKC_EXTERN BOOL WINAPI dkcLZSSIsLZSS(DKC_LZSS_HEADER *p);

#if !defined(  DKUTIL_C_LZSS_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcLZSS.c"
#endif

#endif