/*! @file
	@brief Patricia Trie (PATRICIA木)
	@author d.Kingyo
	@note
	PATRICIA: Practical Algorithm to Retrieve Information Coded in Alphanumeric
	ビット単位で分岐するコンパクトなトライ木。
	Sedgewick型 (上方リンク方式) を採用。

	参考: TAOCP Vol.3 §6.3 Digital Searching
	      D.R. Morrison "PATRICIA" (1968)

	特徴:
	- 挿入: O(m) - mはキーのビット長
	- 検索: O(m)
	- 削除: O(m)
	- メモリ効率が良い (パス圧縮)
	- バイナリデータ対応
*/

#ifndef DKC_PATRICIA_INC_
#define DKC_PATRICIA_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief Patricia Trieノード構造体 */
typedef struct dkc_PatriciaNode {
	struct dkc_PatriciaNode *left;   /*!< bit=0 の枝 */
	struct dkc_PatriciaNode *right;  /*!< bit=1 の枝 */
	size_t bit_index;                /*!< 検査ビット位置 */
	BYTE *key;                       /*!< キー (コピー) */
	size_t key_len;                  /*!< キー長 (バイト) */
	void *data;                      /*!< データポインタ */
	size_t data_size;                /*!< データサイズ */
} DKC_PATRICIA_NODE;

/*! @brief Patricia Trie構造体 */
typedef struct dkc_Patricia {
	DKC_PATRICIA_NODE *root;         /*!< ルートノード (ヘッダ) */
	size_t count;                    /*!< 格納キー数 */
} DKC_PATRICIA;

/*! @brief Patricia Trie走査コールバック */
typedef BOOL (*DKC_PATRICIA_CALLBACK)(
	const BYTE *key, size_t key_len, void *data, size_t data_size, void *user);

/* ====================================================================
 * 生成・解放
 * ==================================================================== */

/*!
	@brief Patricia Trieを作成する
	@param trie 構造体へのポインタ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcPatriciaCreate(DKC_PATRICIA *trie);

/*!
	@brief Patricia Trieを解放する
	@param trie 構造体へのポインタ
*/
DKC_EXTERN void WINAPI dkcPatriciaFree(DKC_PATRICIA *trie);

/* ====================================================================
 * 操作
 * ==================================================================== */

/*!
	@brief キーとデータを挿入する
	@param trie 構造体へのポインタ
	@param key キー (バイト列)
	@param key_len キー長 (バイト)
	@param data データ (NULLでも可)
	@param data_size データサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcPatriciaInsert(DKC_PATRICIA *trie,
	const BYTE *key, size_t key_len, const void *data, size_t data_size);

/*!
	@brief キーを検索する
	@param trie 構造体へのポインタ
	@param key 検索キー
	@param key_len キー長
	@param data データ格納先 (NULLでも可)
	@param data_size データサイズ格納先 (NULLでも可)
	@return edk_SUCCEEDED:見つかった edk_Not_Found:見つからない edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcPatriciaSearch(const DKC_PATRICIA *trie,
	const BYTE *key, size_t key_len, void **data, size_t *data_size);

/*!
	@brief キーを削除する
	@param trie 構造体へのポインタ
	@param key 削除キー
	@param key_len キー長
	@return edk_SUCCEEDED:成功 edk_Not_Found:見つからない edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcPatriciaRemove(DKC_PATRICIA *trie,
	const BYTE *key, size_t key_len);

/* ====================================================================
 * 情報
 * ==================================================================== */

/*!
	@brief 格納キー数を取得する
	@param trie 構造体へのポインタ
	@return 格納キー数
*/
DKC_EXTERN size_t WINAPI dkcPatriciaCount(const DKC_PATRICIA *trie);

/*!
	@brief Trieが空かどうか確認する
	@param trie 構造体へのポインタ
	@return TRUE:空 FALSE:空でない
*/
DKC_EXTERN BOOL WINAPI dkcPatriciaIsEmpty(const DKC_PATRICIA *trie);

/* ====================================================================
 * 走査
 * ==================================================================== */

/*!
	@brief 全キーを走査する
	@param trie 構造体へのポインタ
	@param callback コールバック関数
	@param user ユーザーデータ
	@return 走査したキー数
*/
DKC_EXTERN size_t WINAPI dkcPatriciaForEach(const DKC_PATRICIA *trie,
	DKC_PATRICIA_CALLBACK callback, void *user);

/*!
	@brief 前方一致検索
	@param trie 構造体へのポインタ
	@param prefix 前方一致キー
	@param prefix_len 前方一致キー長
	@param callback コールバック関数
	@param user ユーザーデータ
	@return 一致したキー数
*/
DKC_EXTERN size_t WINAPI dkcPatriciaSearchPrefix(const DKC_PATRICIA *trie,
	const BYTE *prefix, size_t prefix_len,
	DKC_PATRICIA_CALLBACK callback, void *user);

#ifdef __cplusplus
}
#endif

#endif /* DKC_PATRICIA_INC_ */
