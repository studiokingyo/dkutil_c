/*!
@file dkcString.h
@author d金魚
@since 2004/2/28
@note
SYN氏のHPより。http://www.emit.jp/　SHIFT-JISの文字列の検索のコードを頂きました。
この場をお借りしてお礼申し上げます。m(_ _)m<br>
<br>
SHIFT-JIS(以下 SJIS)
@brief 文字列処理
*/
#ifndef DKUTIL_C_STRING_H
#define DKUTIL_C_STRING_H


#include "dkcOSIndependent.h"
#include "dkcMemoryStream.h"




#define UCHAR_MAX_STR "255"
///unsigned char のMAX時の文字列の長さ（NULL文字無し) 
#define UCHAR_MAX_STR_LEN 3


#define USHRT_MAX_STR "65535"

#define USHRT_MAX_STR_LEN 5

///(signed)  short の最大値 の文字列
#define SHRT_MAX_STR "32767" 

#define SHRT_MAX_STR_LEN 5

/// (signed)  short の最小値 の文字列
#define SHRT_MIN_STR "-32768"

#define SHRT_MIN_STR_LEN 6

///(signed)  int の最大値の文字列
#define INT_MAX_STR "2147483647"

#define INT_MAX_STR_LEN 10

///(0xffffffff) unsigned int の最大値の文字列 
#define UINT_MAX_STR "4294967295"

#define UINT_MAX_STR_LEN 10

///INT_MINの文字列
#define INT_MIN_STR "-2147483648"


#define INT_MIN_STR_LEN 11

///(0xffffffff) unsigned int の最大値 の文字列
#define ULONG_MAX_STR "4294967295"

#define ULONG_MAX_STR_LEN 10

#define LONGLONG_MAX_STR "9223372036854775807"

#define LONGLONG_MAX_STR_LEN 19

#define LONGLONG_MIN_STR "-9223372036854775808"

#define LONGLONG_MIN_STR_LEN 20

#define ULONGLONG_MAX_STR "18446744073709551615"

#define ULONGLONG_MAX_STR_LEN 20

///doubleが最大のときの文字列
#define DBL_MAX_STR "179769313486231570000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000"
///doubleが最大のときの桁数（文字列の)
#define DBL_MAX_STR_LEN 316

///doubleが最小のときの文字列
#define DBL_MIN_STR "0.000000"
///doubleが最小のときも桁数（文字列の)
#define DBL_MIN_STR_LEN 8 

/*
extern const BYTE SJIS1_S1 ;
extern const BYTE SJIS1_E1 ;
extern const BYTE SJIS1_S2 ;
extern const BYTE SJIS1_E2 ;

// SJIS文字の２バイト目のコード
extern const BYTE SJIS2_S1 ;
extern const BYTE SJIS2_E1 ;
extern const BYTE SJIS2_S2 ;
extern const BYTE SJIS2_E2 ;
*/
///@see dkcIsSJIS1()
#define dkcmIsSJIS1(c) dkcIsSJIS1(c)/*((c ^= 0x20) && (c >= (SJIS1_S1 ^ 0x20) && c <= (SJIS1_E2 ^ 0x20)))*/
///@see dkcIsSJIS2()
#define dkcmIsSJIS2(c) dkcIsSJIS2(c)/*(c >= SJIS2_S1 && c <= SJIS2_E1) || (c >= SJIS2_S2 && c <= SJIS2_E2)*/


enum edkcString{
	edkcCHARString = 1,
	edkcWCHARString,
};
/*!
文字列を格納する構造体（C++のSTLのstd::stringみたいなもの）
*/
typedef struct dkc_String{
	DKC_MEMORYSTREAM *mStream;
	///文字が使用しているbyte数
	size_t mByteSize;
}DKC_STRING;


///@return cがSJISの１バイト目の時TRUE(1)
DKC_EXTERN BOOL dkcIsSJIS1(BYTE c);
///@return cがSJISの２バイト目の時TRUE(1)
DKC_EXTERN BOOL dkcIsSJIS2(BYTE c);
///@return 半角全角を区別なしにSpace系の文字だったらTRUE
DKC_EXTERN BOOL dkcIsSpace(BYTE c);
///@return 可視できる文字定数だったらTRUE
//DKC_EXTERN BOOL dkcIsVisible
//DKC_EXTERN const char *dkcGetSpaceSeparator();



/// SJIS文字を除外して文字を検索 @return 最初に見つかった位置を返す -1なら見つからない
DKC_EXTERN int WINAPI dkcSJIS_StrChrSearch(const char *s, char c);

/// SJIS文字を除外して文字を検索 @return 最後に見つかった位置を返す -1なら見つからない
DKC_EXTERN int WINAPI dkcSJIS_StrChrSearchLast(const char *s, char c);

/*!
SJIS文字を除外して文字を後ろから検索 
@return 後ろから検索して最初に見つかった位置を返す -1なら見つからない
@note It has not tested. 
*/
DKC_EXTERN int WINAPI dkcSJIS_StrChrSearchTail(const char *s,size_t len,char c);

/*!
複数の文字定数を検索指定できるdkcSJIS_StrChrSearch()
///@note It has not tested. 
*/
DKC_EXTERN int WINAPI dkcSJIS_StrChrSearchInStr(const char *s,const char *c);
///@note It has not tested. 	
DKC_EXTERN int WINAPI dkcSJIS_StrChrSearchInStrLast(const char *s, const char *c);
///@return パスセパレータが最初に見つかった位置を返す。@return -1は見つからない
DKC_EXTERN int WINAPI dkcSJIS_SearchPathSep(const char *s);
///@return パスセパレータが最期に見つかった位置を返す。@return -1は見つからない
DKC_EXTERN int WINAPI dkcSJIS_SearchPathSepLast(const char *s);

/*! strstrのWrapper
@return 最初に見つかった位置を返す -1なら見つからない -2だと、あまりにも文字列が離れすぎていて表現できない。
*/
DKC_EXTERN int WINAPI dkcStrStr(const char *dest,const char *src);
/*!
strstrをBM法でやってみたもの
@return 最初に見つかった位置を返す -1なら見つからない
///@note It has not tested. 
*/
DKC_EXTERN int WINAPI dkcBMStrStr(const char *dest,const char *src);


//**********************************************************
//文字列格納ライブラリ（STLのstd::stringみたいな～


DKC_EXTERN DKC_STRING * WINAPI dkcAllocString(size_t size);

DKC_EXTERN int WINAPI dkcFreeString(DKC_STRING **ptr);

DKC_EXTERN int WINAPI dkcStringCopy(DKC_STRING *ptr,const char *str,size_t size);
///http://www.google.com/search?num=50&hl=ja&lr=lang_ja&q=strcat+cat%82%c6%82%cd
DKC_EXTERN int WINAPI dkcStringConcatenate(DKC_STRING *ptr,const char *str,size_t size);
/*!
@param ptr[in][out] dkcAllocString()で確保したポインタ
@param point[in] 何文字目以降に挿入するのか
@param str[in] 挿入したい文字列
@param size[in] strの文字数。
@todo テストをする。この関数の実装は醜いので直す。
*/
DKC_EXTERN int WINAPI dkcStringInsert(DKC_STRING *ptr,size_t point,const char *str,size_t size);
/*!
@param ptr[in][out] dkcAllocString()で確保したポインタ
@param point[in] 何文字目以降を削除するのか
@param len[in] pointから何文字Eraseするか。
@return edk_SUCCEEDEDなら完全成功。<br>
edk_Not_Satisfactoryならlenが大きすぎる等の理由から一応処理は出来たが要求した結果にはならなかった。<br>
その他はエラー
@todo test
*/
DKC_EXTERN int WINAPI dkcStringErase(DKC_STRING *ptr,size_t point,size_t len);
/*!
@param ptr[in][out] dkcAllocString()で確保したポインタ
@param begin_[in] 何処から(何文字目から
@param end_[in] 何処まで(何文字目まで)注意：begin_からn byteまで　とかではない。
@param str[in] 置換したい文字列
@param size[in] strの文字数。
@todo テストをする。この関数の実装は醜いので直す。
*/
DKC_EXTERN int WINAPI dkcStringReplace(DKC_STRING *ptr,size_t begin_,size_t end_,const char *str,size_t size);



///@return 文字列の文字数を返す。内部streamをstrlenにかけたものを保存しておいたものを取得。
DKC_EXTERN size_t dkcStringNum(const DKC_STRING *ptr);

DKC_EXTERN size_t dkcStringSize(const DKC_STRING *ptr);


DKC_EXTERN const char * WINAPI dkcStringPointer(const DKC_STRING *ptr);

///DKC_STRINGの内部文字列をバッファにぶち込む
DKC_EXTERN int WINAPI dkcStringGetBuffer(const DKC_STRING *ptr,char *buffer,size_t buffsize);

#if 0
///@note It has not tested. 
DKC_EXTERN int WINAPI dkcStringSerialize(const DKC_STRING *ptr,DKC_SERIALIZE *);
///@note it has not tested.
DKC_EXTERN DKC_STRING* WINAPI dkcAllocStringDeserialize(DKC_DESERIALIZE *se);
#endif


#endif
