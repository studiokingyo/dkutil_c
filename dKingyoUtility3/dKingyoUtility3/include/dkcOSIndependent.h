/*!
@file dkcOSIndependent.h
@brief 独立系
@note
詳しい説明はdkc.hに書いています。

*/
#ifndef DKUTIL_C_OS_INDEPENDENT_H
#define DKUTIL_C_OS_INDEPENDENT_H





#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#	pragma once
#else


#endif

#ifdef __cplusplus //C++のみ

#	include <limits>


#endif


#include "dkcDefined.h"
#include <limits.h>
#include <float.h>
#include <assert.h>

/*
#define DKUTIL_C_USE_DKUTIL_CSTD
#ifdef DKUTIL_C_USE_DKUTIL_CSTD
#define DKUTIL_CSTD_NOT_DEFINE_STDLIB_MACRO
#	include "dkutil_cstd/stdlib.h"
#else
#	include <stdlib.h>
#endif
*/
#define DKUTIL_CSTD_NOT_DEFINE_STDLIB_MACRO
#	include "dkutil_cstd/stdlib.h"
#	include <stdlib.h>

#ifdef __BORLANDC__
#	include <dir.h>
#else
#	include <direct.h>
#endif

#if defined(_WIN32) || defined(__WIN32__)
#	ifndef WIN32
#		define WIN32
#	endif
#endif

//**********************************************************
//型定義

#if defined(_MSC_VER) || defined(WIN32) || defined(__BCPLUSPLUS__)
	typedef unsigned __int64 QWORD;

#else
	typedef unsigned long long QWORD;

#endif




///boolean型
typedef unsigned char dkctBOOL;
/*
#ifndef uint32
typedef unsigned long uint32;
#endif

#ifndef uint16
typedef unsigned short uint16;
#endif

#ifndef uint8
typedef unsigned char uint8;
#endif
*/


#ifndef WIN32

#ifndef DWORD
#	define DWORD  		uint32
#endif
#ifndef WORD
#	define WORD  		uint16
#endif
#ifndef BYTE
#	define BYTE  		uint8
#endif





#ifndef ULONGLONG
#	define ULONGLONG unsigned long long int
#endif

#ifndef LONGLONG
#	define LONGLONG long long int
#endif


#ifndef TRUE
#	define TRUE 1
#endif
#ifndef FALSE
#	define FALSE 0
#endif

#ifndef NULL
#	define NULL 0
#endif

#	define dkcdPATH_SEP '/'
#	define dkcdPATH_SEP_STR "/"
#	define dkcdPATH_LIST_SEP ':'
#	define dkcdPATH_LIST_SEP_STR ":"
#	define dkcmIS_PATH_SEP(c) (dkcdPATH_SEP==(c))
/*.,:;*/
#	define dkcmIS_INVALID_FILENAME_CHAR(c) ( ((c) =='\\') || ((c)=='/') || ((c)=='*') || ((c)=='?') \
		|| ((c)== '"') || ((c) == '<') || ((c) == '>') || ((c) == '|') )

#	define dkcmIS_INVALID_FOLDERNAME_CHAR(c) \
	dkcmIS_INVALID_FILENAME_CHAR(c)

#define dkcmIS_INVALID_PATH_CHAR(c) \
	( ( (c)=='\\') || ((c)=='*') || ((c)=='?') || ((c)== '"') || \
	((c) == '<') || ((c) == '>') || ((c) == '|') )

//reallocの関数の型
typedef void *(*DKC_REALLOC_F_TYPE)(void *,size_t);

typedef int BOOL;
//typedef unsigned char BYTE;




#ifndef CALLBACK
#	define CALLBACK
#endif
#ifndef WINAPI
#	define WINAPI
#endif

#else //else of WIN32

#ifdef _MSC_VER
#	ifdef _DEBUG
#		ifndef DEBUG
#			define DEBUG
#		endif
#	else
#		ifndef NDEBUG
#			define NDEBUG
#		endif
#	endif

#	pragma warning(disable:4115)//union{ } struct{ }
#	pragma warning(disable:4100)//引数が関数の本体で 1 度も参照されません。
#	pragma warning(disable:4206)//プリプロセス後のファイルが空です。 #if 0 #endif
#	pragma warning(disable:4710)//場合によってはｲﾝﾗｲﾝ処理を行いません。
#	pragma warning(disable:4505)//参照されていないﾛｰｶﾙ関数は削除されました。
#endif

# include <windows.h>




#	ifndef CALLBACK
#		define CALLBACK __stdcall
#	endif
#	ifndef WINAPI
#		define WINAPI __stdcall
#	endif



#	define dkcdPATH_SEP '\\'
#	define dkcdPATH_SEP_STR "\\"
#	define dkcdPATH_LIST_SEP ';'
#	define dkcdPATH_LIST_SEP_STR ";"
#	define dkcmIS_PATH_SEP(c) (dkcdPATH_SEP==(c) || '/'==(c))

/*!.,;*/
#	define dkcmIS_INVALID_FILENAME_CHAR(c) ( ((c) =='\\') || ((c)=='/') || ((c)=='*') || ((c)=='?') \
		|| ((c)== '"') || ((c) == '<') || ((c) == '>') || ((c) == '|') || (':'== (c)) )

#	define dkcmIS_INVALID_FOLDERNAME_CHAR(c) \
	dkcmIS_INVALID_FILENAME_CHAR(c)

///ファイルパスに使ってはいけない文字列だったらTRUE
#	define dkcmIS_INVALID_PATH_CHAR(c) \
	((c)=='*') || ((c)=='?') || ((c)== '"') || \
	((c) == '<') || ((c) == '>') || ((c) == '|')

#ifdef _MT
#	define dkcdMultiThread 
#endif

#ifdef _MSC_VER
#define CRTDBG_MAP_ALLOC
#	include <crtdbg.h>
#	ifdef DEBUG
#		pragma comment(linker,"/NODEFAULTLIB:LIBC")
#		pragma comment(linker,"/NODEFAULTLIB:LIBCMT")
#	ifdef dkcdMultiThread 
#		pragma comment(linker,"/NODEFAULTLIB:LIBCD")
#	else
#		pragma comment(linker,"/NODEFAULTLIB:LIBCMTD")
#	endif
#else
#		pragma comment(linker,"/NODEFAULTLIB:LIBCD")
#		pragma comment(linker,"/NODEFAULTLIB:LIBCMTD")
#	ifdef dkcdMultiThread 
#		pragma comment(linker,"/NODEFAULTLIB:LIBC")
#	else
#		pragma comment(linker,"/NODEFAULTLIB:LIBCMT")
#	endif
#endif
#endif //end of _MSC_VER

#endif //end of  #ifndef WIN32



#ifdef WIN32//Windowsならば
/// choose a byte order for your hardware
#define ORDER_DCBA	// chosing Intel in this case
#define dkcdLITTLE_ENDIAN 1
#define dkcdBIG_ENDIAN 0
#else //macとかならば
#define ORDER_ABCD
#define dkcdLITTLE_ENDIAN 0
#define dkcdBIG_ENDIAN 1
//#define ORDER_ABCD //motorola (Macとか)
#endif

#ifdef ORDER_DCBA  	// DCBA - little endian - intel

	union aword {
	  DWORD dword;
	  BYTE byte [4];
	  struct {
	    unsigned int byte3:8;
	    unsigned int byte2:8;
	    unsigned int byte1:8;
	    unsigned int byte0:8;
	  } w;
	};

union dkc4ByteUnionLittleEndianBase{
	DWORD dword;
	BYTE byte [4];
	struct {
	  unsigned int byte0:8;
	  unsigned int byte1:8;
	  unsigned int byte2:8;
	  unsigned int byte3:8;
	} w;
};
#endif

#ifdef ORDER_ABCD  	// ABCD - big endian - motorola
	union aword {
	  DWORD dword;
	  BYTE byte [4];
	  struct {
	    unsigned int byte0:8;
	    unsigned int byte1:8;
	    unsigned int byte2:8;
	    unsigned int byte3:8;
	  } w;
	};
	union dkc4ByteUnionLittleEndianBase{
		DWORD dword;
		BYTE byte [4];
		struct {
			unsigned int byte3:8;
	    unsigned int byte2:8;
	    unsigned int byte1:8;
	    unsigned int byte0:8;
		} w;
};
#endif

#ifdef ORDER_BADC  	// BADC - vax
	union aword {
	  DWORD dword;
	  BYTE byte [4];
	  struct {
	    unsigned int byte1:8;
	    unsigned int byte0:8;
	    unsigned int byte3:8;
	    unsigned int byte2:8;
	  } w;
};
#endif

typedef union aword DKC_4BYTE_UNION_BIG_ENDIAN_BASE;

typedef union dkc4ByteUnionLittleEndianBase DKC_4BYTE_UNION_LITTLE_ENDIAN_BASE;

typedef union dkc_2int32_to_int64{
	struct{
		LONG LowPart;
		LONG HighPart;
	}u;
	LONGLONG QuadPart;
}DKC_TWOINT32_TO_INT64;
/*
#ifdef WIN32
	typedef DKC_4BYTE_UNION_LITTLE_ENDIAN_BASE DKC_4BYTE_UNION;
*/



#ifdef WIN32
	///reallocの関数の型
	typedef void *(WINAPIV *DKC_REALLOC_F_TYPE)(void *,size_t);
	///関数のポインタ型
	//typedef int (WINAPI *PROC)();
	typedef PROC DKC_WINAPI_PROC_F_TYPE ;
#else
	
	typedef void *(*DKC_REALLOC_F_TYPE)(void *,size_t);
	typedef void *DKC_WINAPI_PROC_F_TYPE;
#endif

///いろんな場面で使われるコールバック関数
typedef BOOL (*DKC_STD_CALLBACK)(void *peculiar,void *user);


//**********************************************************
//limits

#ifndef ULONGLONG_MAX
#	define ULONGLONG_MAX DKINGYO_ULONGLONG_MAX
#endif
#ifndef LONGLONG_MAX
#	define LONGLONG_MAX DKINGYO_LONGLONG_MAX
#endif
#ifndef LONGLONG_MIN
#	define LONGLONG_MIN DKINGYO_LONGLONG_MIN
#endif



#ifndef dkcdMAXPATH 
#	if defined(PATH_MAX)
#		define dkcdMAXPATH   PATH_MAX

#	elif defined(MAX_PATH)
#		define dkcdMAXPATH   MAX_PATH 

#	elif  defined(_MAX_PATH)
#		define dkcdMAXPATH   _MAX_PATH

#	else
#		define dkcdMAXPATH   1024

#	endif
#endif

#ifndef dkcdMAXPATH_LEN
#	define dkcdMAXPATH_LEN dkcdMAXPATH
#endif

/* 何事も例外を考えるのが大切だ。って事でbufferは普通の２倍取得する･･･。（ｵｲ 
	追記：
	見事に予想は的中してバッファサイズは約２倍無いと危ない条件がある事が分かった。
	*/


#ifndef dkcdMAXPATH_BUFFER 
/**
　なんかWindows NT系だとこういう場合もあるらしいから
詳しくは下記のURLの資料を参照されたし
http://beefway.hp.infoseek.co.jp/prog/filename.html
*/
///安全なMAX_PATHのバッファのサイズ(バイト単位）
#	define dkcdMAXPATH_BUFFER (dkcdMAXPATH * 2 + 2)
#endif


#ifndef dkcdMAXPATH_FIXED_SIZE
///dkcdMAXPATH_BUFFERのサイズは各OSによってサイズが違うのでOSや環境によって左右されない固定サイズを定義しておくのが良いと思った
#	define dkcdMAXPATH_FIXED_SIZE 2050

#endif

#ifndef MAYBE
#	define MAYBE 2
#endif




typedef struct dkc_4char{
	signed char mode1,mode2,mode3,mode4;
}DKC_4CHAR;




///@return FALSEならNativeなファイルパスではない。
DKC_EXTERN BOOL WINAPI dkcIsNativePathString(const char *s,size_t size);



/*
@param OldSize[in] 元のメモリ領域のサイズ
@param ExpandSize[in] 足りないメモリ領域のサイズ
@return 多分、理想系のサイズ？カナ？
@note reallocする時の最適なメモリサイズを計算する。
(内部実装はちょくちょく変わります。
*/

DKC_EXTERN  size_t dkcReallocateSizeFunction(size_t OldSize,size_t ExpandSize);

///malloc()系 関数の型
typedef void *( WINAPIV *DKC_ALLOC_FUNC_TYPE)(size_t);
///free()系 関数の型
typedef void (WINAPIV *DKC_FREE_FUNC_TYPE)(void *);

DKC_EXTERN void *WINAPIV dkcMallocAdapter(size_t);

DKC_EXTERN void WINAPIV dkcFreeAdapter(void *);

DKC_EXTERN  void* WINAPIV dkcAllocate(size_t size);

DKC_EXTERN  int	WINAPIV dkcFree(void **p);

DKC_EXTERN void *WINAPIV dkcAllocateFast(size_t size);

DKC_EXTERN void *WINAPIV dkcAllocateFill(size_t size,BYTE fill);

DKC_EXTERN  int WINAPI dkcReallocate(
	void **NewPtr,size_t NewSize,void **OldPtr
);

/*!
@param your_realloc[in] reallocする関数
@param NewPtr[out] 新しいメモリ空間アドレス
@param NewSize[in] 新しいメモリ空間アドレスのサイズ。
@param OldPtr[in][out] 古いメモリ空間アドレス。関数が成功すると、OldPtrは無効になる
@param OldSize[in] OldPtrのサイズ
*/
///@return edk_SUCCEEDEDだったら成功
DKC_EXTERN int WINAPI dkcReallocateEx(
	DKC_REALLOC_F_TYPE your_realloc,void **NewPtr,size_t NewSize,
	void **OldPtr
);

/*!
@param your_realloc[in] reallocする関数
@param NewPtr[out] 新しいメモリ空間アドレス
@param OldPtr[in][out] 古いメモリ空間アドレス。関数が成功すると、OldPtrは無効になる
@param OldSize[in] OldPtrのサイズ
@param reallocated_size[out] 確保したサイズ
@note
メモリサイズ拡張専用
*/
DKC_EXTERN int WINAPI dkcReallocateAutoExpand(
	DKC_REALLOC_F_TYPE your_realloc,void **NewPtr,
	void **OldPtr,size_t OldSize,size_t *reallocated_size
);

DKC_EXTERN int WINAPI dkcGetMemorySize(uint64 *pTotalMemory,uint64 *pFreeMemory);

/*!
演算でSwapさせる。SWAP_NUM()マクロを参照。
*/
DKC_EXTERN void WINAPI 
	dkcSwap(void *p1,void *p2,size_t size);
/*!
どんな方法でもいいからともかく速くSwapさせる。
@note
size分のメモリを動的確保してスワップします。
*/

DKC_EXTERN BOOL WINAPI dkcSwapFast(void *p1,void *p2,size_t size);

/*!
@param p1[in] ULONGLONG *のバッファへのポインタ
@param p2[in] ULONGLONG *のバッファへのポインタ
@param size[in] p1,p2バッファのサイズ
@note
p1,p2共にULONGLONG *に強制型キャストしてください。
*/
DKC_EXTERN void WINAPI dkcSwap64(ULONGLONG *p1,ULONGLONG *p2,size_t size);


//#define dkcSwap(p1_,p2_,size) dkcSwap64(p1_,p2_,size)
/*
#define dkcSwapFast(p1,p2,size) dkcSwap64(p1,p2,size)
*/


///fopenの時の第二引数が有効な値かどうか調べる。
DKC_EXTERN BOOL dkcIs_foepn_mode(const char *s);
///fopenのラッパー
DKC_EXTERN FILE * WINAPI dkcFOpen(const char *filename,const char *mode);
///fcloseのラッパー
DKC_EXTERN int WINAPI dkcFClose(FILE **ptr);

///freadの全部責任持って読み込みますバージョン。(読み込めない場合はEOFかerror)
DKC_EXTERN size_t WINAPI dkcFReadAll(void *,size_t size,FILE *fp);


///@see dkcFReadAll() dkcFReadAllのfwrite版
DKC_EXTERN size_t WINAPI dkcFWriteAll(const void *,size_t size,FILE *fp);

///dkcFileSize()を普段はお使いください。エラー時の時は動作は未定義です。
DKC_EXTERN size_t WINAPI dkcFSize(FILE *fp);


///fopenのファイル関数を使ってバイナリセーブを行う。@note 類似: dkcMemoryToFile()
DKC_EXTERN int WINAPI dkcSaveBinary(const void *data,size_t size,const char *fname);
///バイナリデータをメモリに読み込む。dkcSaveBinary()と対 @note 類似: dkcFileToMemory()
DKC_EXTERN int WINAPI dkcLoadBinary(void *data,size_t size,const char *fname,size_t *readsize);
///fopenでテキストセーブを行う
DKC_EXTERN int WINAPI dkcSaveText(const char *text,size_t length,const char *fname);
///テキストデータをメモリに読み込む。dkcSaveText()と対
DKC_EXTERN int WINAPI dkcLoadText(char *text,size_t length,const char *fname,size_t *readsize);
///空ファイルを作る。
DKC_EXTERN BOOL WINAPI dkcCreateEmptyFile(const char *filename);
///エラーログ(ファイル)を高速に初期化する、@note 完全に抹消するわけではない @param filename[in] ファイルの名前
DKC_EXTERN int WINAPI dkcInitFileFast( const char *filename );
///ログを吐く @param filename[in] ファイルネーム @param str[in] 吐く文字列 @note 出力速度が遅いので多用は奨励しない
DKC_EXTERN  int WINAPI dkcAddLogFile( const char *filename,const char *str , ... );

DKC_EXTERN void WINAPI dkcTwoDWORDToULONGLONG(ULONGLONG *dest,DWORD high,DWORD low);

DKC_EXTERN void WINAPI dkcULONGLONGToTwoDWORD(DWORD *dhigh,DWORD *dlow,ULONGLONG src);

DKC_EXTERN void WINAPI dkcLONGLONGToTwoLONG(LONG *high,LONG *low,LONGLONG src);

DKC_EXTERN void WINAPI dkcTwoLONGToLONGLONG(LONGLONG *dest,LONG high,LONG low);


///@note エンディアン環境によって上手く出来るか分からない
DKC_INLINE void dkcTwoLONGToULONGLONG(ULONGLONG *dest,LONG high,LONG low){
	ULONG *p = (ULONG *)&low,*p2;
	*dest = *p;
	p = (ULONG *)(dest + 1);
	p2 = (ULONG *)&high;
	*p = *p2;
}

DKC_EXTERN int WINAPI dkcOutputDebugString(const char *str,...);
/*!
@return almost TRUE(1)
*/
DKC_EXTERN int WINAPI dkcErrorMessage(const char *expression,
										 const char *filename,size_t line,const char *message,...);



#if defined(DEBUG) || defined(DKUTIL_DEBUG)
#	define dkcmNOT_ASSERT(ex) ( (ex) && dkcErrorMessage(#ex,__FILE__,__LINE__,NULL) )
#	define dkcmASSERT(ex) ( (ex) || dkcErrorMessage(#ex,__FILE__,__LINE__,NULL) )
#	define dkcmNOT_ASSERT_MESSAGE(ex,mes) ( (ex) && dkcErrorMessage(#ex,__FILE__,__LINE__,mes) )
#	define dkcmASSERT_MESSAGE(ex,mes) ( (ex) || dkcErrorMessage(#ex,__FILE__,__LINE__,mes) )
#else
#	define dkcmNOT_ASSERT_MESSAGE(ex,mes) ((void)0)
#	define dkcmASSERT_MESSAGE(ex,mes) ((void)0)
#	define dkcmNOT_ASSERT(ex) ((void)0)
#	define dkcmASSERT(ex) ((void)0)
#endif

#	define dkcmFORCE_NOT_ASSERT(ex) ( (ex) && dkcErrorMessage(#ex,__FILE__,__LINE__,NULL) )
#	define dkcmFORCE_ASSERT(ex) ( (ex) || dkcErrorMessage(#ex,__FILE__,__LINE__,NULL) )
#	define dkcmFORCE_NOT_ASSERT_MESSAGE(ex,mes) ( (ex) && dkcErrorMessage(#ex,__FILE__,__LINE__,mes) )
#	define dkcmFORCE_ASSERT_MESSAGE(ex,mes) ( (ex) || dkcErrorMessage(#ex,__FILE__,__LINE__,mes) )

///@return path separatorの入った文字列(1byte毎にpath separatorの文字定数が入っている)をゲット										 
DKC_EXTERN const char *WINAPI dkcGetPathSep();

/**
@param isForceExit[in] TRUEを指定するとdkutil_cの奨励環境と違った場合、強制終了する。
@return edk_Result
*/
DKC_EXTERN int dkcDynamicCheckEnvironment(BOOL isForceExit);
///@param flag[in] TRUEでチェックを有効
DKC_EXTERN void WINAPI dkcCheckMemoryLeak(BOOL flag);
///@return MMXが使えるならTRUE
DKC_EXTERN BOOL WINAPI dkcIsMMX();



#define dkcmREVERSE_ENDIAN16(x) ( (USHORT)( (x >> 8) | (x << 8) ) )

#define dkcmREVERSE_ENDIAN32(x) \
	( (x << 24) | ( (x & 0x0000ff00) << 8) | ( (x & 0x00ff0000) >> 8 ) | (x >> 24) )

#ifdef WIN32
#	define dkcmREVERSE_ENDIAN64(x) (\
		(ULONGLONG)dkcReverseEndian32( (DWORD) (x & 0x00000000ffffffff) ) << 32 )\
		| \
		dkcReverseEndian32((DWORD)(x >> 32)\
	)
#else
#	define dkcmREVERSE_ENDIAN64(x) dkcReverseEndian64(x)

#endif
///@note  General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.より
DKC_EXTERN ULONG dkcReverseEndian32(ULONG x);
///@note  General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.より
DKC_EXTERN ULONGLONG dkcReverseEndian64(ULONGLONG x);
///endian change 16 bit version
DKC_EXTERN USHORT dkcReverseEndian16(USHORT x);

///@return little endian ならTRUE
DKC_EXTERN BOOL dkcIsLittleEndian();
///@return big endianならTRUE
#define dkcIsBigEndian() (!dkcIsLittleEndian())

enum edkcByteOrder{
	///big-endian motorola
	edkcByteOrder_ABCD = 0,
	///little-endian intel
	edkcByteOrder_DCBA,
	///BADC - vax i386?
	edkcByteOrder_BADC,
	edkcByteOrder_Unknown = 255,
};
typedef union dkc_byte_order_check_union{
	uint32 x;
	uint8 abcd[4];
}DKC_BYTE_ORDER_CHECK_UNION;
///@return enum edkcByteOrder
DKC_EXTERN int WINAPI dkcGetByteOrder();

///unsigned long型の二つの整数をプラスしたらオーバーフローするかどうかをチェックする。
DKC_EXTERN BOOL dkcCheckOverflowULONG(ULONG a1,ULONG a2);
DKC_EXTERN BOOL dkcCheckOverflowULONGLONG(ULONGLONG a1,ULONGLONG a2);

#define dkcCheckOverflow32(a,b) dkcCheckOverflowULONG(a,b)
#define dkcCheckOverflow64(a,b) dkcCheckOverflowULONGLONG(a,b)


typedef int (WINAPIV *DKC_COMPARE_TYPE)(const void *,const void *);

/*!
@param dest[out] ｎ番目のデータを入れるためのバッファ widthバイトのバッファが必要
@param a_src[in] メモリへのポインタ
@param n[in] 何個の要素があるか
@param k[in] 何個目の要素のオフセットを知りたいか
@param width[in] オフセット
@param less[in] LESSのコンペア関数へのポインタ
@return edk_SUCCEEDEDで成功
オフセット値は (BYTE *)a_src + width * offset である。
@note
ちなみにa_srcに渡したポインタは内部でメモリ確保され、コピーされます。
@code
int greater_comp(const void *v1,const void *v2){
	return *( ( int * ) v2 ) - *( ( int * ) v1 );
}
void test()
{
	int arr[]={0,1,2,3};
	int dest;
	dkcSelect((void *)&dest,(const void *)arr,4,3,sizeof(int),greater_comp);
}
@endcode
*/
///いわいるセレクト(選択)アルゴリズム
DKC_EXTERN int WINAPI dkcSelect(void *dest,const void *a_src,size_t n,int k,size_t width ,DKC_COMPARE_TYPE comp);

/*!
最大値を選択する
@see dkcSelect() の 引数kが無いだけ
*/
DKC_EXTERN int WINAPI dkcSelectMax(void *dest,const void *a, size_t n,size_t width ,DKC_COMPARE_TYPE comp);
/*!
最小値を選択する 
@see dkcSelect() の 引数kが無いだけ
*/
DKC_EXTERN int WINAPI dkcSelectMin(void *dest,const void *a, size_t n,size_t width ,DKC_COMPARE_TYPE comp);


DKC_INLINE int dkcRotateShiftRightMemoryLogic(void *dest,size_t size,size_t n,void *workbuff,size_t worksize);


DKC_EXTERN int WINAPI dkcRotateShiftRightMemory(void *dest,size_t size,size_t n);

/*
DKC_EXTERN void dkcSetAllocateMode(int flag);

DKC_EXTERN void *dkcDebugAlloc
*/



#if DKC_HAVE_INLINE==1

DKC_FORCE_INLINE void *WINAPI dkcSecureFillMemory (void * Destination,size_t Length,uint8 Fill){
	volatile uint8 *vptr = (volatile uint8 *)Destination;
	while(Length){
		*vptr = Fill;
		vptr++;
		Length--;
	}
	return Destination;
}

DKC_FORCE_INLINE void *dkcSecureZeroMemory(void  *ptr, size_t cnt)
{
	/*volatile char *vptr = (volatile char *)ptr;
	while (cnt) {
		*vptr = 0;
		vptr++;
		cnt--;
	}
	return ptr;*/
	return dkcSecureFillMemory(ptr,cnt,0);
}


///読み込めない時（FALSE時）はfeofやferror()を使ってください。
DKC_INLINE BOOL dkcFReadAllCheck(void *buff,size_t size,FILE *fp)
{
	if(size != dkcFReadAll(buff,size,fp))
		return FALSE;
	return TRUE;
}
DKC_INLINE BOOL dkcFWriteAllCheck(const void *buff,size_t size,FILE *fp)
{
	if(size != dkcFWriteAll(buff,size,fp))
		return FALSE;
	return TRUE;
}



///妥当なテンポラリバッファのサイズを返す
DKC_INLINE size_t dkcGetProperTemporaryBufferSize(){
	uint64 total_size,free_size;
	if(DKUTIL_FAILED(dkcGetMemorySize(&total_size,&free_size)))
	{
		return 1024 * 64;
	}

	/*if(free_size < 1024 * 1024)
	{
		return 1024;
	}
	//freespaceが半分よりあったら
	if(free_size > total_size / 2)
	{
		return total_size / 0x4000;//32768
	}*/
	total_size = free_size / 0x1000;//4096
	if(total_size > 1024 * 1024){
		total_size = 1024 * 1024;
	}
	else if(0==total_size) 
	{
		total_size = free_size % 0x1000;
		if(0==total_size){
			return 1;
		}
	}
	return (size_t)total_size;
}
#else

// WINAPI ...

#endif





#ifdef __cplusplus

namespace dkutil{


/*!

@param data[in] ファイルのデータへのポインタ(void *)にキャストしてぶち込んでください。
@param size[in] ファイルのデータのサイズ　(書き込むサイズ)
@param fname[in] ファイルの名前
@param mode[in] デフォルトで="wb" オープンモード 詳しくはMSDNのfopen関数を見よ！
@return true=成功 false=失敗

*/
///fopenのファイル関数を使ってバイナリセーブを行う。
inline bool SaveBinary(const void *data,size_t size,const char *fname,const char *mode="wb"){//="wb"
	FILE *fp;
	fp = fopen( fname , mode ) ;//wb
	if(fp==NULL) return false;
	//fwrite( data , size  , 1 , fp ) ;
	dkcFWriteAll(data,size,fp);
	//fwrite( data , sizeof(size)  , 1 , fp ) ;
	fclose( fp ) ;
	return true;
	//return dkcSaveBinary(data,size,fname);
}

/*!
@param data[out] ファイルのデータのバッファ (void *)にキャストしてぶち込んでください。
@param size[in] ファイルのデータのバッファサイズ　(読み込むサイズ)
@param fname[in] ファイルの名前
@param mode[in] デフォルトで="rb" オープンモード 詳しくはMSDNのfopen関数を見よ！
@return true=成功 false=失敗
*/
///バイナリデータをメモリに読み込む。SaveBinary()と対
inline bool LoadBinary(void *data,size_t size,const char *fname,const char *mode="rb" ){//="rb"
	FILE *fp ;
	fp = fopen( fname , mode ) ;//rb
	if(fp==NULL)return false;
	dkcFReadAll(data,size,fp);
	//fread( data ,  size , 1 , fp ) ;
	//fread( data , sizeof(size)  , 1 , fp ) ;
	fclose( fp ) ;
	return true;
}

inline bool SaveText(const char *text,size_t length,const char *fname,const char *mode="wt"){
	return SaveBinary(text,length,fname,mode);
}
inline bool LoadText(char *text,size_t length,const char *fname,const char *mode="rt"){
	return LoadBinary(text,length,fname,mode);
}



}//end of dkutil namespace


#else //for c lang




#endif //end of __cplusplus

#endif //end of include once