/*! @file
	@brief Suffix Array (接尾辞配列)
	@author d.Kingyo
	@note
	接尾辞配列はManber & Myers (1993)が設計した文字列検索用データ構造です。

	特徴:
	- 構築: O(n log^2 n)
	- パターン検索: O(m log n)
	- LCP配列: Kasai's algorithm O(n)
	- メモリ効率が良い（接尾辞木より省メモリ）

	参考: U. Manber & G. Myers, "Suffix Arrays: A New Method for On-Line String Searches", 1993
*/

#ifndef DKC_SUFFIXARRAY_INC_
#define DKC_SUFFIXARRAY_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief 接尾辞配列構造体 */
typedef struct dkc_SuffixArray{
	char *text;          /*!< テキストのコピー */
	size_t length;       /*!< テキスト長 */
	size_t *sa;          /*!< 接尾辞配列 */
	size_t *lcp;         /*!< LCP配列（NULL = 未構築） */
	size_t *rank;        /*!< ランク配列（構築用） */
}DKC_SUFFIXARRAY;

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

/*!
	@brief 接尾辞配列を構築する
	@param text テキスト
	@param length テキスト長
	@return 構築された接尾辞配列、失敗時はNULL
*/
DKC_EXTERN DKC_SUFFIXARRAY * WINAPI dkcAllocSuffixArray(
	const char *text, size_t length);

/*!
	@brief 接尾辞配列を解放する
	@param ptr 接尾辞配列へのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeSuffixArray(DKC_SUFFIXARRAY **ptr);

/*!
	@brief パターンを検索する（二分探索）
	@param ptr 接尾辞配列
	@param pattern 検索パターン
	@param patlen パターン長
	@param first 最初のマッチ位置（出力）
	@param count マッチ数（出力）
	@return edk_SUCCEEDED:見つかった edk_Not_Found:見つからない
*/
DKC_EXTERN int WINAPI dkcSuffixArraySearch(
	DKC_SUFFIXARRAY *ptr,
	const char *pattern, size_t patlen,
	size_t *first, size_t *count);

/*!
	@brief LCP配列を構築する（Kasai's algorithm）
	@param ptr 接尾辞配列
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcSuffixArrayBuildLCP(DKC_SUFFIXARRAY *ptr);

#ifdef __cplusplus
}
#endif

#endif /* DKC_SUFFIXARRAY_INC_ */
