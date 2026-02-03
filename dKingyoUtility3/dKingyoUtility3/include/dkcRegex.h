/*!
@file dkcRegex.h
@brief		正規表現ライブラリ？
@author		d金魚
@since		2004/09/01
@note
C++で正規表現はboost::regexを使うのがイイかも？
またはyaneSDK3rdのDLL。

@section 元ソース
dkcRegex.cおよびdkcRegex.hはGreenPadの実装をC言語に移植しています。
*/
#ifndef dkutil_c_Regex__h
#define dkutil_c_Regex__h



#include "dkcStream.h"
#include "dkcOSIndependent.h"

typedef struct dkc_Replace{
	DKC_STREAM *mStream;
}DKC_REPLACE;

typedef wchar_t DKC_UNICODE;

DKC_EXTERN DKC_REPLACE *WINAPI dkcAllocReplace();

DKC_EXTERN int WINAPI dkcFreeReplace(DKC_REPLACE **);

DKC_EXTERN int WINAPI dkcReplaceRun(	 const BYTE *src,size_t srcsize,
																		 const BYTE *target_data,size_t target_data_size,
																		 const BYTE *replace_data,size_t replace_data_size
															);



///文字の種類
enum edkcRegToken
{
	/// 普通の文字
	R_Char,
	/// '.'
	R_Any, 
  /// '['
	R_Lcl,
	/// ']'
	R_Rcl,
	/// '^'
	R_Ncl,
	/// '-'
	R_Range,
	/// '('
	R_Lbr,
	/// ')'
	R_Rbr,
	/// '|'
	R_Bar,
	/// '*'
	R_Star,
	/// '+'
	R_Plus,
	/// '?'
	R_Quest,
	/// '\0'
	R_End,
};


///構文木のノードに振られる値の種類
enum edkcRegType
{
	N_Char,     // 普通の文字 (ch)
	N_Class,    // [...] など (cls)
	N_Concat,   // 連接       (left, right)
	N_Or,       // |          (left, right)
	N_Closure,  // *          (left)
	N_Closure1, // +          (left)
	N_01,       // ?          (left)
	N_Empty     // 空         (--)
};

///正規表現オブジェクト
typedef struct dkcRegularExpression{
	void *re_;
	BOOL caseS_;
	BOOL down_;
}DKC_Regular_EXPRESSION,DKC_REX,DKC_REGEX;

/**
@param key[in] キー
@param caseS[in] 大文字小文字を区別しないのであればTRUE
@param down[in] 下から検索の場合はTRUE
*/
DKC_EXTERN DKC_REGEX *WINAPI dkcAllocRegex(const DKC_UNICODE* key, BOOL caseS, BOOL down);

DKC_EXTERN int WINAPI dkcFreeRegex(DKC_REGEX **);

DKC_EXTERN BOOL WINAPI dkcRegexSearch(DKC_REGEX *p,const DKC_UNICODE* str, ULONG len, ULONG stt,ULONG* mbg, ULONG* med);


/**
@{
	検索を行う
	@param str 対象文字列
	@param len 対象文字列の長さ
	@param stt 検索開始index。0なら先頭から
	@param mbg マッチ結果の先頭index
	@param med マッチ結果の終端indexの１個後ろ
	@return マッチしたかどうか

	下方向サーチオブジェクトの場合、stt <= *beg の範囲
	上方向サーチオブジェクトの場合、*beg <= stt の範囲を検索
@}
*/
DKC_EXTERN BOOL WINAPI dkcRegularExpressionSearch( const DKC_UNICODE* key, BOOL caseS, BOOL down,const DKC_UNICODE* str, ULONG len, ULONG stt,ULONG* mbg, ULONG* med );
/**

@{ @pkg Gp.Search @}
@{
	超簡易正規表現マッチング関数。

	patとstr全体がマッチするならtrue、ダメならfalseを返す
@}
*/

DKC_EXTERN BOOL WINAPI dkcRegularExpressionMatch( const DKC_UNICODE* pat, const DKC_UNICODE* str, BOOL caseS );


#endif //end of include once