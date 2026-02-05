/*! @file
	@brief Trie（トライ木）ライブラリ
	@author d.Kingyo, threeBrane
	@note
	文字列の効率的な格納・検索を行うデータ構造

	参考: TAOCP Vol.3 "Digital Searching"

	特徴:
	- 挿入: O(m) - mは文字列長
	- 検索: O(m)
	- 接頭辞検索: O(m + k) - kは結果数
	- 削除: O(m)

	実装:
	- 基本Trie: 配列ベースの子ノード（高速だがメモリ大）
	- Compact Trie: パス圧縮版（Patricia Trie相当）

	用途:
	- 辞書・スペルチェッカー
	- オートコンプリート
	- IPルーティングテーブル
	- 接頭辞マッチング
*/

#ifndef DKC_TRIE_INC_
#define DKC_TRIE_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数
 * ==================================================================== */

#define dkcd_TRIE_ALPHABET_SIZE  256    /*!< アルファベットサイズ（バイト） */
#define dkcd_TRIE_SMALL_ALPHABET 26     /*!< 小文字英字のみ */

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief Trieノード構造体 */
typedef struct dkc_TrieNode {
	struct dkc_TrieNode **children; /*!< 子ノード配列 */
	void *value;                     /*!< 格納値（終端ノードのみ） */
	size_t alphabet_size;            /*!< アルファベットサイズ */
	BOOL is_end;                     /*!< 文字列の終端か */
	size_t prefix_count;             /*!< この接頭辞を持つ文字列数 */
} DKC_TRIE_NODE;

/*! @brief Trie構造体 */
typedef struct dkc_Trie {
	DKC_TRIE_NODE *root;            /*!< ルートノード */
	size_t count;                    /*!< 格納文字列数 */
	size_t alphabet_size;            /*!< アルファベットサイズ */
	size_t node_count;               /*!< 総ノード数 */
} DKC_TRIE;

/*! @brief 接頭辞検索コールバック */
typedef BOOL (*DKC_TRIE_CALLBACK)(const char *key, void *value, void *user_data);

/* ====================================================================
 * 基本関数
 * ==================================================================== */

/*!
	@brief Trieを作成
	@param trie          構造体へのポインタ
	@param alphabet_size アルファベットサイズ（0でデフォルト256）
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcTrieCreate(DKC_TRIE *trie, size_t alphabet_size);

/*!
	@brief Trieを解放
*/
DKC_EXTERN void WINAPI dkcTrieFree(DKC_TRIE *trie);

/*!
	@brief 文字列を挿入
	@param trie  構造体へのポインタ
	@param key   挿入する文字列
	@param value 関連付ける値（NULLでも可）
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcTrieInsert(DKC_TRIE *trie, const char *key, void *value);

/*!
	@brief 文字列を検索
	@param trie 構造体へのポインタ
	@param key  検索する文字列
	@return 関連付けられた値、見つからない場合はNULL
*/
DKC_EXTERN void* WINAPI dkcTrieSearch(const DKC_TRIE *trie, const char *key);

/*!
	@brief 文字列が存在するか確認
	@param trie 構造体へのポインタ
	@param key  確認する文字列
	@return TRUE: 存在する, FALSE: 存在しない
*/
DKC_EXTERN BOOL WINAPI dkcTrieContains(const DKC_TRIE *trie, const char *key);

/*!
	@brief 文字列を削除
	@param trie 構造体へのポインタ
	@param key  削除する文字列
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcTrieRemove(DKC_TRIE *trie, const char *key);

/* ====================================================================
 * 接頭辞操作
 * ==================================================================== */

/*!
	@brief 指定した接頭辞で始まる文字列が存在するか
	@param trie   構造体へのポインタ
	@param prefix 接頭辞
	@return TRUE: 存在する, FALSE: 存在しない
*/
DKC_EXTERN BOOL WINAPI dkcTrieStartsWith(const DKC_TRIE *trie, const char *prefix);

/*!
	@brief 指定した接頭辞を持つ文字列の数を取得
	@param trie   構造体へのポインタ
	@param prefix 接頭辞
	@return 該当する文字列数
*/
DKC_EXTERN size_t WINAPI dkcTrieCountPrefix(const DKC_TRIE *trie, const char *prefix);

/*!
	@brief 指定した接頭辞で始まる全文字列を列挙
	@param trie      構造体へのポインタ
	@param prefix    接頭辞
	@param callback  コールバック関数
	@param user_data ユーザーデータ
	@return 列挙した文字列数
*/
DKC_EXTERN size_t WINAPI dkcTrieEnumPrefix(const DKC_TRIE *trie, const char *prefix,
	DKC_TRIE_CALLBACK callback, void *user_data);

/* ====================================================================
 * 情報取得
 * ==================================================================== */

/*!
	@brief 格納文字列数を取得
*/
DKC_EXTERN size_t WINAPI dkcTrieCount(const DKC_TRIE *trie);

/*!
	@brief Trieが空か確認
*/
DKC_EXTERN BOOL WINAPI dkcTrieIsEmpty(const DKC_TRIE *trie);

/*!
	@brief 総ノード数を取得
*/
DKC_EXTERN size_t WINAPI dkcTrieNodeCount(const DKC_TRIE *trie);

/* ====================================================================
 * 高度な操作
 * ==================================================================== */

/*!
	@brief 最長共通接頭辞を取得
	@param trie   構造体へのポインタ
	@param key    検索キー
	@param result 結果を格納するバッファ
	@param size   バッファサイズ
	@return 最長共通接頭辞の長さ
*/
DKC_EXTERN size_t WINAPI dkcTrieLongestCommonPrefix(const DKC_TRIE *trie,
	const char *key, char *result, size_t size);

/*!
	@brief 全文字列を列挙
	@param trie      構造体へのポインタ
	@param callback  コールバック関数
	@param user_data ユーザーデータ
	@return 列挙した文字列数
*/
DKC_EXTERN size_t WINAPI dkcTrieForEach(const DKC_TRIE *trie,
	DKC_TRIE_CALLBACK callback, void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* DKC_TRIE_INC_ */
