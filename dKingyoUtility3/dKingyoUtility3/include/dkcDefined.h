
/*!
@author d金魚
@since 2003/9/13
@brief 定義郡
@file dkcDefined.h
@note
C language only

C++にはこんな感じの定義をするものに ios_base ってものが最近ある事を知る。
*/


#ifndef DKUTIL_C_DEFINED_H
#define DKUTIL_C_DEFINED_H


#include <stdio.h>
#include <stddef.h>
#include "dkutil_cstd/define.h"

//**********************************************************
//共通の定義
//**********************************************************

///8bitに収まる素数 (unsigned)
#define dkcdUNSIGNED_8BIT_PRIME_NUM 251 
///8bitに収まる素数 (signed)
#define dkcdSIGNED_8BIT_PRIME_NUM 127

///16bitに収まる素数 (unsigned)
#define dkcdUNSIGNED_16BIT_PRIME_NUM 65521

#define dkcdSIGNED_16BIT_PRIME_NUM 32749

///24bitに収まる素数 (unsigned)
#define dkcdUNSIGNED_24BIT_PRIME_NUM 16777213

///64bitに収まる最大値 (unsigned)
#define dkcd64BIT_MAX DKINGYO_ULONGLONG_MAX
///64bitに収まる最小値 (signed)
#define dkcd64BIT_MIN DKINGYO_LONGLONG_MIN

///メンバ関数が定義されていない時に返す変数
#define DKINGYO_NOT_MOUNTED -100
///DKINGYO_NOT_MOUNTEDと同じ
#define DKINGYO_NO_DEFINED_FUNCTION DKINGYO_NOT_MOUNTED





//**********************************************************
//識別するための定義
//**********************************************************




///画像ファイルを解析する時に使う＾＾
enum{
	enuDKINGYO_BMPFILEHEADER_ADR = 1,
	enuDKINGYO_BMPCOREHEADER_ADR,
	enuDKINGYO_BMPINFOHEADER_ADR,
	enuDKINGYO_BMPPALLETE_ADR,
	enuDKINGYO_BMPGRAPH_ADR,
	///isBmpFile()でのflagにぶち込む識別子 Bitmapファイルかどうか正確なチェックを行う。
	enuDKINGYO_ISBMPFILE_EXACT_CHECK,
	///isBmpFile()でのflagにぶち込む識別子 偽装ビットマップかどうか調べる。
	enuDKINGYO_ISBMPFILE_CAMOUFLAGE_CHECK,
};


//**********************************************************
//デフォルト数の設定用定数の定義
//**********************************************************
enum{
	///拡張型引数のバッファの追加サイズを指定する
	enuExtendVaListLength = 256,
	///コンテナ系のテンプレートクラスでどのくらいのサイズ（または数）を確保するかを設定する
	enuDefaultExpandSize = 100,
};

//**********************************************************
//型定義
//**********************************************************

//共通型
typedef void (*VOIDFUNC)(void); 
typedef void (*VOIDARGFUNC)(void *);
typedef void (*VOIDARGFUNCFLAG)(void *,int);


	
typedef struct dkc_Allocator{
	void (*malloc__)(size_t size);
	void (*free__)(void *);
	void (*realloc__)(void *,size_t);
	void (*zero_memory)(void *,size_t);
}DKC_ALLOCATOR;	
	

//**********************************************************
//dKingyoUtility2と同じ　マクロとか
//**********************************************************


/// MessageBoxを表示
#define MB(str) MessageBox(NULL,str,"Message",MB_OK)//MessageBox
/// MessageBoxを表示（タイトル指定可能バージョン）
#define MB2(s,title) MessageBox(NULL,s,title,MB_OK);
///OutputDebugStringを使用
#define ODS(str) OutputDebugString(str)//OutputDebugString

/**
@see dkcMemoryStreamDump() dkcMemoryStreamWriteToMemory() dkcBitMemoryStreamLoadFromFile()

*/
enum{
	/**
	@warning 注意：データが書き出されていない領域も指定するので注意。
	有効データのみを指定する場合はedkcStreamBufferToNowOffsetを指定すること。
	*/
	///ストリームバッファのすべてを指定
	edkcStreamBufferAll = 0,
	///ストリームバッファの最初だけを指定
	edkcStreamBufferFirst = 1,
	///ストリームバッファの最後だけを指定
	//edkcStreamBufferLast = 2,
	///今指定されているオフセットまでを指定
	edkcStreamBufferToNowOffset = 3,

	edkcSeekCurrent = SEEK_CUR,
	///最後の位置からシーク
	edkcSeekEnd = SEEK_END,
	///最初の位置からシーク
	edkcSeekSet = SEEK_SET,
};


#ifndef int32
#	define int32 int  		
#endif
#ifndef int16
#	define int16 short
#endif
#ifndef int8
#	define int8 char 		
#endif

#ifdef WIN32



#else

#ifndef uint32
#	define uint32 unsigned int 		
#endif
#ifndef uint16 
#	define uint16 unsigned short 
#endif
#ifndef uint8
#	define uint8 unsigned char
#endif


#endif

#endif // end of include once
