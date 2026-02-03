/*!
@file dkcCRC.h
@author d金魚
@since 2004/08/24
@brief CRCルーチン boost::crcのC言語移植版
@bug まだバグチェックしていない。まだ、ある。
@note

Licence : http://www.boost.org/LICENSE_1_0.txt

サポートしていそうなCRC
- CRC32 left right

- CRC-CCITT left right (ITU-T V.41  ... X^16 + X^12 + X^5 + 1 ) (HDLC ??)
- XMODEM left right
- CRC ANSI16 left right

- CRC12 x12+x11+x3+x2+x1+１
- CRC7 

boost::crcのC言語版を目指していると考えていただくと分かりやすいかと･･･。
それにしてもboost::crcの実装はかなりE感じ。
ULONGとしている所がE感じのテクだと思った。
（MinGW等ではtemplate<class T,class C=std::vector<T> >みたいな事が出来ない。)


<h3>参考資料</h3>
- http://www.kmonos.net/alang/boost/classes/crc.html
- 


*/

#ifndef DKUTIL_C_CRC_H
#define DKUTIL_C_CRC_H

#include "dkcOSIndependent.h"




typedef struct dkc_CRC{
	//ロード関数へのポインタ
	//void (*mLoader)(struct dkc_CRC *,const BYTE *,size_t);
	///テーブルへのポインタ
	void *mTable;
	///状態遷移変数
	unsigned long mR;
	///生成済みCRC保存変数
	unsigned long mResult;
	///生成するCRCのビット数
	size_t mBits;
	///生成多項式
	ULONG mTruncPoly;
  ///初期値
	ULONG mInitRem;
	///最後にXORする値
	ULONG mFinalXor;
  
	dkctBOOL mReflectIn;

	dkctBOOL mReflectRem;
}DKC_CRC;

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC( size_t Bits, ULONG TruncPoly,
           ULONG InitRem, ULONG FinalXor,
           dkctBOOL ReflectIn, dkctBOOL ReflectRem);

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_32left();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_32right();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_ansi16left();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_ansi16right();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_ccitt_left();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_ccitt_right();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_xmodem();

DKC_EXTERN int WINAPI dkcFreeCRC(DKC_CRC **pp);

/*!
@param p[in][out] 初期化したいCRC構造体へのポインタ
@param buffer[in] 指定したいバッファ先 ULONGかUSHORT256個の配列である事
@param Bits[in] 何ビットのCRC値を生成するか。普通は16とか32が一般的
@param TruncPoly[in] 生成多項式
@param InitRem[in] 初期値
@param FinalXor[in] CRC計算の最後にXORする値
@param ReflectIn[in] 反対にするかどうか。
@param ReflectRem[in] 反対にするかどうか･･･。(スマン、ヨクワカラン）boost::crcを移植しただけだし･･･。
*/
DKC_EXTERN int WINAPI dkcCRCInit(DKC_CRC *p,void *buffer,
					size_t Bits, ULONG TruncPoly,
          ULONG InitRem, ULONG FinalXor,
          dkctBOOL ReflectIn, dkctBOOL ReflectRem );

/*!
@param Bits[in] 指定するCRC値(dkcCRCInit()のBitsと同じ値にする)
@return 0だとエラー それ以外は 使用するであろうテーブルのbyte単位のサイズを返す。
*/

///dkcCRCInit()のbufferに渡すバッファサイズを求める。
DKC_EXTERN size_t WINAPI dkcCRCGetUseTableSize(size_t Bits);

/*!
@param p[in][out] DKC_CRC構造体へのポインタ
@param pBuffer[in] 読み取るバッファへのポインタ
@param dwSize[in] バッファにアクセスしてOKなサイズ
*/
DKC_EXTERN void WINAPI dkcCRCLoad(DKC_CRC *p,const BYTE *pBuffer,size_t size);

DKC_EXTERN void WINAPI dkcCRCLoadBlock(DKC_CRC *p,const void *Begin,const void *End);


/*!
@param p[in][out] dkcAllocSHA1()で取得したポインタ
@note

*/
DKC_EXTERN ULONG WINAPI dkcCRCFinal(DKC_CRC *p);

DKC_EXTERN ULONG WINAPI dkcCRCResult(const DKC_CRC *p);
#if !defined(   DKUTIL_C_CRC_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcCRC.c"
#endif


#endif //end of include once