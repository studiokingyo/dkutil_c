/*! @file
	@brief Rope (ロープ: 効率的な文字列データ構造)
	@author d.Kingyo
	@note
	ロープはBoehm et al. (1995)が設計した文字列表現で、
	二分木ベースで長い文字列の編集を効率的に行えます。

	特徴:
	- 連結: O(1)
	- 挿入/削除: O(log n)
	- ランダムアクセス: O(log n)
	- 部分文字列: O(log n + m)

	参考: H. Boehm et al., "Ropes: an Alternative to Strings", 1995
*/

#ifndef DKC_ROPE_INC_
#define DKC_ROPE_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/*! @brief 葉ノードの最大文字列長 */
#define dkcd_ROPE_LEAF_MAX  64

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief ロープノード */
typedef struct dkc_RopeNode{
	size_t weight;       /*!< 左部分木の文字数（葉なら文字列長） */
	size_t length;       /*!< このノード以下の全文字数 */
	char *str;           /*!< 葉ノードの文字列（NULLなら内部ノード） */
	struct dkc_RopeNode *left;
	struct dkc_RopeNode *right;
}DKC_ROPE_NODE;

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

/*!
	@brief 文字列からロープを作成する
	@param str 文字列
	@return ロープのルート、失敗時はNULL
*/
DKC_EXTERN DKC_ROPE_NODE * WINAPI dkcAllocRope(const char *str);

/*!
	@brief ロープを解放する
	@param ptr ロープへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeRope(DKC_ROPE_NODE **ptr);

/*!
	@brief 2つのロープを連結する
	@param left 左ロープ（連結後は使用不可）
	@param right 右ロープ（連結後は使用不可）
	@return 連結されたロープ
*/
DKC_EXTERN DKC_ROPE_NODE * WINAPI dkcRopeConcat(
	DKC_ROPE_NODE *left, DKC_ROPE_NODE *right);

/*!
	@brief 位置posに文字列を挿入する
	@param ptr ロープへのポインタのアドレス（更新される）
	@param pos 挿入位置
	@param str 挿入文字列
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcRopeInsert(DKC_ROPE_NODE **ptr,
	size_t pos, const char *str);

/*!
	@brief 範囲を削除する
	@param ptr ロープへのポインタのアドレス（更新される）
	@param pos 削除開始位置
	@param len 削除長
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcRopeDelete(DKC_ROPE_NODE **ptr,
	size_t pos, size_t len);

/*!
	@brief 指定位置の文字を取得する
	@param ptr ロープ
	@param index 位置
	@return 文字（範囲外なら-1）
*/
DKC_EXTERN int WINAPI dkcRopeCharAt(DKC_ROPE_NODE *ptr, size_t index);

/*!
	@brief ロープを文字列に変換する
	@param ptr ロープ
	@param buf 出力バッファ
	@param bufsize バッファサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcRopeToString(DKC_ROPE_NODE *ptr,
	char *buf, size_t bufsize);

/*!
	@brief 部分文字列を取得する
	@param ptr ロープ
	@param pos 開始位置
	@param len 長さ
	@param buf 出力バッファ
	@param bufsize バッファサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcRopeSubstring(DKC_ROPE_NODE *ptr,
	size_t pos, size_t len, char *buf, size_t bufsize);

/*!
	@brief ロープの長さを取得する
	@param ptr ロープ
	@return 文字数
*/
DKC_EXTERN size_t WINAPI dkcRopeLength(DKC_ROPE_NODE *ptr);

#ifdef __cplusplus
}
#endif

#endif /* DKC_ROPE_INC_ */
