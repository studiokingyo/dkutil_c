/**
@author d金魚
@file define.h
@brief C言語の定義(独自拡張アリ･･･)
@since 2005/01/17
@note
<h2>stable releaseされていない機能を使うDLLに関するコンパイルオプションdefine</h2>
USE_DKC_DLL　DKUTIL_CDLL_EXPORTS : DLL modeでdkutil_cをコンパイルす時に必要。
USE_DKC_DLL : DLL modeでリンクするときに必要。

*/


#ifndef DKUTIL_CSTD_DEFINE_H
#define DKUTIL_CSTD_DEFINE_H

#include "auto_config.h"

//**********************************************************
//ここらへんって32ビット環境依存だから＾＾；；；；；注意してね＾＾；；；
//**********************************************************

//定数を定義する
///ULONGLONGの変数のマックス値(最大値)
#define DKINGYO_ULONGLONG_MAX 18446744073709551615 
///LONGLONGの変数のマックス値(最大値)
#define DKINGYO_LONGLONG_MAX 9223372036854775807 
///LONGLONGの変数のミン値(最小値)
#define DKINGYO_LONGLONG_MIN -9223372036854775808 

#ifndef dkcdUINT8_MAX
#	define dkcdUINT8_MAX 255
#endif
#ifndef dkcdUINT16_MAX
#	define dkcdUINT16_MAX 65535
#endif
#ifndef dkcdUINT32_MAX 
#	define dkcdUINT32_MAX 4294967295
#endif
#ifndef dkcdUINT64_MAX 
#	define dkcdUINT64_MAX DKINGYO_ULONGLONG_MAX
#endif

/*

(0xffffffff) unsigned long の最大値 
INT_MAX 2147483647 (signed)  int の最大値 
INT_MIN -2147483647-1 (signed)  int の最小値 
LONG_MAX 2147483647 (signed)  long の最大値 
LONG_MIN -2147483647-1 (signed)  long の最小値 
*/

//**********************************************************
//修飾子の定義
//**********************************************************

#ifdef __cplusplus
#	define DKUTIL_EXTERN extern "C"
#else
///extern...
#	define DKUTIL_EXTERN extern
#endif


#ifndef DKC_INLINE
#define DKC_HAVE_INLINE 1
//inline
/*
#	if _MSC_VER >= 1100 && defined(_inline)
#		define DKC_INLINE _inline
#	elif defined(__inline)
#		define DKC_INLINE __inline
#	elif defined(inline)
#		define DKC_INLINE inline
*/
#	if _MSC_VER >= 1100
#		define DKC_INLINE __inline
#	elif USEINLINE
#		define DKC_INLINE __inline__
#	else
#		define DKC_INLINE
#		undef DKC_HAVE_INLINE
#		define DKC_HAVE_INLINE 0
#	endif

//forceinline
//#	if defined(__forceinline)
#	ifdef FORCEINLINE
#		define DKC_FORCE_INLINE FORCEINLINE
#	elif _MSC_VER >= 1100
#		define DKC_FORCE_INLINE __forceinline
#	else
#		define DKC_FORCE_INLINE DKC_INLINE
#	endif//end of FORCEINLINE
#endif//end of DKC_INLINE

/*
#ifdef _USRDLL
#	define USE_DKC_DLL
#endif
*/

#ifdef WIN32
#	define DKC_EXPORT __declspec(dllexport)
#	define DKC_IMPORT __declspec(dllimport)
#	ifdef USE_DKC_DLL
#		if defined(DKCDLL_EXPORTS) || defined(DKUTIL_CDLL_EXPORTS)
#			define DKC_EXTERN DKUTIL_EXTERN DKC_EXPORT
#		else
#			define DKC_EXTERN DKUTIL_EXTERN DKC_INPORT
#		endif
#	else
#		ifdef _MSC_VER
#			define DKC_EXTERN DKUTIL_EXTERN DKC_INLINE
#		else
#			define DKC_EXTERN DKUTIL_EXTERN
#		endif
#	endif
#else
#	define DKC_EXPORT
#	define DKC_IMPORT
#	define DKC_EXTERN DKUTIL_EXTERN
#endif

#ifdef _DEBUG
#	ifndef DEBUG
#		define DEBUG
#	endif
#endif
#ifdef _NDEBUG
#	ifndef NDEBUG
#		define NDEBUG
#	endif
#endif

//#error "I don't know multithread define..."
#if defined(_MT) || defined(__MT__)//MultiThread
#	define dkcdMultiThread
#endif


#include "macro.h"




enum edkOS{//OSのバージョンを取得するための列挙
	enuWindows3_1 = 1,
	enuWindowsNT3_51,
	enuWindowsNT4_0,
	enuWindows95,
	enuWindows98,
	enuWindows2000,
	enuWindowsMe,
	enuWindowsXP,
	///WindowsXP??のサーバー
	enuWindows_NET,

};


enum edkOSExtension{
	///Windows95 OS R2
	edkOSE_OSR2 = 1,
	///Windows98 SE
	edkOSE_edkSE,
	///WindowsXP HomeEdition
	edkOSE_HomeEditon,
	///WindowsXP Professional Edition または、NT系のProfessional
	edkOSE_Professional,
	///Windows .NETのDataCenter Server
	edkOSE_DataCenter_Server,
	///NT系のAdvanced　鯖
	edkOSE_Advanced_Server,
	edkOSE_Enterprise_Server,
	edkOSE_Web_Server,
	///NT系の普通のサーバー
	edkOSE_Server,

};

///変数のタイプのイナム軍
enum edkVariableType{
	///わからない。
	edkVariableUnknown = 0,//これを削除しないこと。この０のままにしておくこと。
	///int
	edkInt,
	///UINT
	edkUInt,
	///LONGLONG
	edkLongLong,
	///ULONGLONG
	edkULongLong,
	///double
	edkDouble,
	///std::string
	edkString,
};



///文字列の長さの識別子。
enum{
	///INT以内の数
	enuSTRLENGTH_INT_DU = 1,//ここは絶対に１ですからね！！いじっちゃダメ！
	///UINT以内の数
	enuSTRLENGTH_UINT_DU,
	///LONGLONG(__int64)以内の数
	enuSTRLENGTH_LONGLONG_DU,
	///ULONGLONG( unsigned __int64)以内の数
	enuSTRLENGTH_ULONGLONG_DU,
	///だヴる(floatなんていらねぇ（爆))以内の数値？
	enuSTRLENGTH_DOUBLE,
};

///まだ、変わる可能性があるので、あまり使わないで下さい＾＾；
enum edkFileSignature{
	edkfALL = 0,
	//テキスト系
	edkfTXT,
	edkfHTML,
	edkfXML,
	//ムービー系
	edkfAVI,
	edkfMPEG,
	edkfMOV,
	//音系
	edkfWAV,
	edkfMIDI,
	edkfMP3,
	edkfOGG,
	//画像系
	edkfPNG,
	edkfBMP,
	edkfTIFF,
	edkfJPEG,
	edkfGIF,
	//圧縮系
	edkfZIP,
	edkfLZH,
	edkfGZ,
	edkfGCA,
	edkfDGC,
	edkfRAR,
	//最後の印。
	edkfSENTINEL,
};

/**
数列処理のデフォルトID
@attention
下にIDを付け足していく事
*/
#if 0
//old
enum edk_ProcessSignatures{
	edk_NONE_SIGNATURE = 0,

//圧縮系
	edk_LZSS_SIGNATURE = 10,
	edk_ZLIB_SIGNATURE,
	edk_RLE_PACKBITS_SIGNATURE,
	edk_BLOCKSORT_SIGNATURE,

//暗号化系
	edk_BLOWFISH_SIGNATURE = 100,
	edk_ARCFOUR_SIGNATURE,
//その他
	//200
	//300
	//edk_ARRAY_PROCESS_ID_END = USHRT_MAX * 2,
};
#else
//new
enum edk_ProcessSignatures{
	edk_NONE_SIGNATURE = 0,

//圧縮系
	edk_LZSS_SIGNATURE = 0x100,
	edk_ZLIB_SIGNATURE = 0x200,
	edk_RLE_PACKBITS_SIGNATURE = 0x400,
	edk_BLOCKSORT_SIGNATURE = 0x800,
	edk_LZW_SIGNATURE = 0x1000,

//暗号化系
	edk_BLOWFISH_SIGNATURE =	0x10000,
	edk_ARCFOUR_SIGNATURE =		0x20000,
	edk_HC256_SIGNATURE =			0x40000,
	edk_SNOW20_SIGNATURE =		0x80000,
	edk_SNOW2_SIGNATURE = edk_SNOW20_SIGNATURE,
	edk_VERNAM_SIGNATURE =		0x100000,
	edk_RIJNDAEL_SIGNATURE =	0x200000,

//その他
	//200
	//300
	//edk_ARRAY_PROCESS_ID_END = USHRT_MAX * 2,
};

#endif

/*
enum edk_{
	///何もしない
	edkc_Plane = 0,
//圧縮形
	edkc_LZW = 0x1,
	edkc_LZSS =0x2,
	edkc_RLE	=0x4,
	edkc_BLOCKSORTED_RLE = 0x8
//暗号化系
	edkc_RIJNDAEL = 0x10000,
	edkc_Arcfour = 0x20000,
	edkc_Blowfish = 0x40000,
	edkc_HC256 = 0x80000,
	edkc_SNOW20 = 0x100000,
	edkc_VERNAM = 0x200000,
};
*/

/**
dkcSJISFileSystem.hのファイル操作に関する enum
*/
enum edk_SJISFileSystem{
	edkcFileRewrite = 1,
};

/**
セキュアハッシュ系のアルゴリズム列挙定義
アルゴリズムのunsigned int型の選択引数に使う
@see http://d.hatena.ne.jp/studiokingyo/00010023

*/
enum edk_SecureHash{

	edkcSH_MD2 = 0,
	edkcSH_MD4,
	edkcSH_MD5,
	
	edkcSH_SHA0 = 16,
	edkcSH_SHA1 ,
	edkcSH_SHA160 = edkcSH_SHA1,
	edkcSH_SHA256,
	edkcSH_SHA384,
	edkcSH_SHA512,

	edkcSH_RIPEMD128 = 32,
	edkcSH_RIPEMD160,
	edkcSH_RIPEMD256,
	edkcSH_RIPEMD320,

};

/*!
正式版で対応するであろうエラーID類(しかし、対応は未定･･･
@note
edkはEnum of D Kingyoの略 (爆)
*/
enum edkResult{
	///ともかく、エラーらしい。
	edk_FAILED = -1,
	///見事　成功！！
	edk_SUCCEEDED = 0,
	///見つからない。
	edk_Not_Found,
	///ファイルが見つからない。
	edk_FileNotFound,
	///選択されなかった。
	edk_Not_Selected,
	///引数に適さない値が入っていた。(引数エラー
	edk_ArgumentException,
	///Parsingできなかった。
	edk_ParsingError,

	/*!
	内部的にエラーが起こった。<br>
	(まだ、その関数を使う準備が出来ていないとか,
	起こりえないエラーが起こったとか,
	呼び出してはいけないのに呼び出したとか。
	*/
	edk_LogicError,
	///バッファをはみ出してしまうので処理を完了できなかった。
	edk_BufferOverFlow,
	///出力バッファーが足りなくなった。
	edk_OutputBufferWasLost,
	///入力バッファーが足りなくなった。
	edk_InputBufferWasLost,
	//ファイルのシグネチャが合わない
	//edk_FileSignatureException,
	///シグネチャが合わない
	edk_SignatureException,
	///エラーではないが、満足な処理はされなかった。意図とした結果は得られないかもしれない。
	edk_Not_Satisfactory,
	///この関数の処理は定義されていない
	edk_Not_Defined_Function,
	///メモリが足らなかった
	edk_OutOfMemory,
	///この関数で処理するのはもう終わり
	edk_EndProcess,
	///別の結果だった。
	edk_LogicException,
	///処理する価値が無い
	edk_NoValueToProcess,
	//ファイルの内容自体を変化させられた。
	edk_FileCheated_Change,
	///ファイルの内容に問題は無いが、冗長なデータが引っ付いている。
	edk_FileCheated_Addition,
	///変数のオーバーフロー
	edk_VariableOverFlow,
	///変数のアンダーフロー
	edk_VariableUnderFlow,
	///enum edkResult の最後の値
	edk_ResultDefinitionEnd = 32767,
	/**
	enum edkResultの戻り値の仕様と一緒にユーザーが独自定義する戻り値を使うときに以下のように使用する
	@code
	enum edkResultEx{
		edk_HackedValue = edk_UserDefinitionBegin,
		edk_UnhackedValue,
		edk_Kracked_Network,
		edk_Unkracked_Network,
	};
	@endcode
	*/
	edk_UserDefinitionBegin,//
};

#define DKUTIL_SUCCEEDED(s) (s==edk_SUCCEEDED)
#define DKUTIL_FAILED(s) (s)//(s != edk_SUCCEEDED)
#define DKUTIL_FAILED_BOOL(s) (s != edk_SUCCEEDED)
#define DKUTIL_SUCCEEDED_BOOL(s) (s==edk_SUCCEEDED)

#define dkcmFAILED_CHECK_RETURN(t,a) \
	t = a;\
	if(DKUTIL_FAILED(t)){ return t;}

#include "os.h"

#endif //end of include once
