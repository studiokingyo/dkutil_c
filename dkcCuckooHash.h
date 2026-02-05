/*! @file
	@brief Cuckoo Hashing（カッコウハッシュ）ライブラリ
	@author d.Kingyo
	@note
	O(1)最悪ケースルックアップを保証するハッシュテーブル

	特徴:
	- ルックアップ: O(1) 最悪ケース（2回のハッシュ計算）
	- 挿入: O(1) 償却（リハッシュ時はO(n)）
	- 削除: O(1) 最悪ケース
	- 高いキャッシュ効率

	アルゴリズム:
	- 2つのハッシュ関数 h1, h2 を使用
	- 各キーは h1(k) または h2(k) の位置に格納
	- 衝突時は既存要素を追い出して再配置
	- 追い出しがループする場合はリハッシュ

	変種:
	- 標準 Cuckoo Hash (2テーブル)
	- Cuckoo Filter (メンバーシップテスト用、削除可能)
*/

#ifndef DKC_CUCKOOHASH_INC_
#define DKC_CUCKOOHASH_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数
 * ==================================================================== */

#define dkcd_CUCKOO_MAX_KICKS      500     /*!< 最大追い出し回数 */
#define dkcd_CUCKOO_DEFAULT_SIZE   16      /*!< デフォルト初期サイズ */
#define dkcd_CUCKOO_LOAD_FACTOR    0.5     /*!< 負荷率上限 */

/* Cuckoo Filter用 */
#define dkcd_CUCKOO_FILTER_BUCKET_SIZE  4  /*!< バケットあたりのスロット数 */
#define dkcd_CUCKOO_FILTER_FP_SIZE      2  /*!< フィンガープリントサイズ(バイト) */

/* ====================================================================
 * Cuckoo Hash Table 構造体
 * ==================================================================== */

/*! @brief Cuckoo Hashエントリ */
typedef struct dkc_CuckooEntry {
	void *key;              /*!< キー（NULL=空） */
	size_t key_len;         /*!< キー長 */
	void *value;            /*!< 値 */
} DKC_CUCKOO_ENTRY;

/*! @brief Cuckoo Hash Table構造体 */
typedef struct dkc_CuckooHash {
	DKC_CUCKOO_ENTRY *table1;   /*!< テーブル1 */
	DKC_CUCKOO_ENTRY *table2;   /*!< テーブル2 */
	size_t capacity;            /*!< 各テーブルの容量 */
	size_t num_items;           /*!< 格納要素数 */
	uint32 seed1;               /*!< ハッシュシード1 */
	uint32 seed2;               /*!< ハッシュシード2 */
	int owns_keys;              /*!< キーを所有するか */
	int owns_values;            /*!< 値を所有するか */
} DKC_CUCKOO_HASH;

/* ====================================================================
 * Cuckoo Filter 構造体（メンバーシップテスト用）
 * ==================================================================== */

/*! @brief Cuckoo Filterバケット */
typedef struct dkc_CuckooFilterBucket {
	uint16 fingerprints[dkcd_CUCKOO_FILTER_BUCKET_SIZE];  /*!< フィンガープリント配列 */
} DKC_CUCKOO_BUCKET;

/*! @brief Cuckoo Filter構造体
	@note Bloom Filterの代替。削除可能で、より低い偽陽性率
*/
typedef struct dkc_CuckooFilter {
	DKC_CUCKOO_BUCKET *buckets; /*!< バケット配列 */
	size_t num_buckets;         /*!< バケット数 */
	size_t num_items;           /*!< 格納要素数 */
	size_t capacity;            /*!< 最大容量 */
	uint32 seed;                /*!< ハッシュシード */
} DKC_CUCKOO_FILTER;

/* ====================================================================
 * Cuckoo Hash Table 基本関数
 * ==================================================================== */

/*!
	@brief Cuckoo Hash Tableを作成
	@param hash     ハッシュテーブル構造体
	@param capacity 初期容量
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcCuckooHashCreate(DKC_CUCKOO_HASH *hash, size_t capacity);

/*!
	@brief Cuckoo Hash Tableを解放
	@param hash ハッシュテーブル構造体
*/
DKC_EXTERN void WINAPI dkcCuckooHashFree(DKC_CUCKOO_HASH *hash);

/*!
	@brief キー所有権設定
	@param hash ハッシュテーブル
	@param owns_keys キーを複製して所有するか
	@param owns_values 値を複製して所有するか
*/
DKC_EXTERN void WINAPI dkcCuckooHashSetOwnership(DKC_CUCKOO_HASH *hash,
	int owns_keys, int owns_values);

/*!
	@brief キーと値を挿入
	@param hash     ハッシュテーブル
	@param key      キー
	@param key_len  キー長
	@param value    値
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcCuckooHashInsert(DKC_CUCKOO_HASH *hash,
	const void *key, size_t key_len, void *value);

/*!
	@brief 文字列キーで挿入
*/
DKC_EXTERN int WINAPI dkcCuckooHashInsertString(DKC_CUCKOO_HASH *hash,
	const char *key, void *value);

/*!
	@brief キーで検索
	@param hash     ハッシュテーブル
	@param key      キー
	@param key_len  キー長
	@return 値へのポインタ、見つからない場合はNULL
*/
DKC_EXTERN void* WINAPI dkcCuckooHashLookup(const DKC_CUCKOO_HASH *hash,
	const void *key, size_t key_len);

/*!
	@brief 文字列キーで検索
*/
DKC_EXTERN void* WINAPI dkcCuckooHashLookupString(const DKC_CUCKOO_HASH *hash,
	const char *key);

/*!
	@brief キーの存在確認
	@return TRUE: 存在する, FALSE: 存在しない
*/
DKC_EXTERN BOOL WINAPI dkcCuckooHashContains(const DKC_CUCKOO_HASH *hash,
	const void *key, size_t key_len);

/*!
	@brief キーを削除
	@param hash     ハッシュテーブル
	@param key      キー
	@param key_len  キー長
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcCuckooHashRemove(DKC_CUCKOO_HASH *hash,
	const void *key, size_t key_len);

/*!
	@brief 文字列キーを削除
*/
DKC_EXTERN int WINAPI dkcCuckooHashRemoveString(DKC_CUCKOO_HASH *hash,
	const char *key);

/*!
	@brief ハッシュテーブルをクリア
*/
DKC_EXTERN void WINAPI dkcCuckooHashClear(DKC_CUCKOO_HASH *hash);

/*!
	@brief 要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcCuckooHashCount(const DKC_CUCKOO_HASH *hash);

/*!
	@brief 負荷率を取得
*/
DKC_EXTERN double WINAPI dkcCuckooHashLoadFactor(const DKC_CUCKOO_HASH *hash);

/* ====================================================================
 * Cuckoo Filter 基本関数
 * ==================================================================== */

/*!
	@brief Cuckoo Filterを作成
	@param filter   フィルタ構造体
	@param capacity 想定最大要素数
	@return edk_SUCCEEDED または edk_FAILED
	@note Bloom Filterより省メモリで削除可能
*/
DKC_EXTERN int WINAPI dkcCuckooFilterCreate(DKC_CUCKOO_FILTER *filter, size_t capacity);

/*!
	@brief Cuckoo Filterを解放
*/
DKC_EXTERN void WINAPI dkcCuckooFilterFree(DKC_CUCKOO_FILTER *filter);

/*!
	@brief 要素を追加
	@return edk_SUCCEEDED, edk_FAILED (フル時)
*/
DKC_EXTERN int WINAPI dkcCuckooFilterAdd(DKC_CUCKOO_FILTER *filter,
	const void *data, size_t len);

/*!
	@brief 文字列を追加
*/
DKC_EXTERN int WINAPI dkcCuckooFilterAddString(DKC_CUCKOO_FILTER *filter,
	const char *str);

/*!
	@brief 要素の存在確認
	@return TRUE: 存在する可能性あり, FALSE: 確実に存在しない
*/
DKC_EXTERN BOOL WINAPI dkcCuckooFilterContains(const DKC_CUCKOO_FILTER *filter,
	const void *data, size_t len);

/*!
	@brief 文字列の存在確認
*/
DKC_EXTERN BOOL WINAPI dkcCuckooFilterContainsString(const DKC_CUCKOO_FILTER *filter,
	const char *str);

/*!
	@brief 要素を削除
	@return edk_SUCCEEDED または edk_FAILED
	@note Bloom Filterと違い削除可能
*/
DKC_EXTERN int WINAPI dkcCuckooFilterRemove(DKC_CUCKOO_FILTER *filter,
	const void *data, size_t len);

/*!
	@brief 文字列を削除
*/
DKC_EXTERN int WINAPI dkcCuckooFilterRemoveString(DKC_CUCKOO_FILTER *filter,
	const char *str);

/*!
	@brief Cuckoo Filterをクリア
*/
DKC_EXTERN void WINAPI dkcCuckooFilterClear(DKC_CUCKOO_FILTER *filter);

/*!
	@brief 要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcCuckooFilterCount(const DKC_CUCKOO_FILTER *filter);

/*!
	@brief 負荷率を取得
*/
DKC_EXTERN double WINAPI dkcCuckooFilterLoadFactor(const DKC_CUCKOO_FILTER *filter);

/*!
	@brief メモリ使用量を取得（バイト）
*/
DKC_EXTERN size_t WINAPI dkcCuckooFilterMemoryUsage(const DKC_CUCKOO_FILTER *filter);

#ifdef __cplusplus
}
#endif

#endif /* DKC_CUCKOOHASH_INC_ */
