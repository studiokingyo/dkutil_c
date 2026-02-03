/**
@file dkc_misc.h
@brief dKingyo Utility C OSIndependent misc
@sicne 2006/01/10
@note
OS Independent
*/
#ifndef DKUTIL_C_DKC_MISC_H
#define DKUTIL_C_DKC_MISC_H



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
//typedef void *(*DKC_REALLOC_F_TYPE)(void *,size_t);

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
	typedef void (WINAPIV *DKC_FREE_F_TYPE)(void *);
	typedef void *(WINAPIV *DKC_MALLOC_F_TYPE)(size_t);
	///reallocの関数の型
	typedef void *(WINAPIV *DKC_REALLOC_F_TYPE)(void *,size_t);
	///関数のポインタ型
	//typedef int (WINAPI *PROC)();
	typedef PROC DKC_WINAPI_PROC_F_TYPE ;
#else		
	typedef void (*DKC_FREE_F_TYPE)(void *);
	typedef void *(*DKC_MALLOC_F_TYPE)(size_t);
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


//**********************************************************
//インライン関数

DKC_INLINE void *dkcAllocateFast_INL(size_t size){
	return malloc(size);
}
DKC_INLINE void *dkcAllocateFill_INL(size_t size,uint8 fill){
	void *p = dkcAllocateFast_INL(size);
	if(NULL==p) return NULL;
	memset(p,fill,size);
	return p;
}

DKC_INLINE void* dkcAllocate_INL(size_t size){
	return dkcAllocateFill_INL(size,0);	
/*void *ptr;
	ptr = dkcAllocateFast(size);
	if(NULL==ptr) return NULL;
	memset(ptr,0,size);
	return ptr;
	*/
}

DKC_INLINE int	dkcFree_INL(void **p){
	if(!*p) return edk_FAILED;
	free(*p);
	*p = NULL;
	return edk_SUCCEEDED;
}

#endif //end of include once