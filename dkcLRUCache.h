/*! @file
	@brief LRUキャッシュ（Least Recently Used Cache）ライブラリ
	@author d.Kingyo
	@note
	固定容量のキャッシュで、容量超過時は最も長く使われていない要素を削除

	特徴:
	- Get: O(1)
	- Put: O(1)
	- 双方向リンクリスト + ハッシュマップで実装

	用途:
	- メモリキャッシュ
	- ページキャッシュ
	- データベースクエリキャッシュ
	- Webキャッシュ
*/

#ifndef DKC_LRUCACHE_INC_
#define DKC_LRUCACHE_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数
 * ==================================================================== */

#define dkcd_LRU_DEFAULT_CAPACITY    16      /*!< デフォルト容量 */
#define dkcd_LRU_HASH_LOAD_FACTOR    0.75    /*!< ハッシュテーブル負荷率 */

/* ====================================================================
 * 構造体
 * ==================================================================== */

/*! @brief LRUキャッシュノード（双方向リンクリスト） */
typedef struct dkc_LRUNode {
	void *key;                      /*!< キー */
	size_t key_len;                 /*!< キー長 */
	void *value;                    /*!< 値 */
	struct dkc_LRUNode *prev;       /*!< 前のノード（より最近使用） */
	struct dkc_LRUNode *next;       /*!< 次のノード（より古い） */
	uint32 hash;                    /*!< キーのハッシュ値（検索高速化） */
} DKC_LRU_NODE;

/*! @brief LRUキャッシュ構造体 */
typedef struct dkc_LRUCache {
	DKC_LRU_NODE **hash_table;      /*!< ハッシュテーブル */
	size_t hash_size;               /*!< ハッシュテーブルサイズ */
	DKC_LRU_NODE *head;             /*!< 最も最近使用されたノード */
	DKC_LRU_NODE *tail;             /*!< 最も古いノード（削除候補） */
	size_t capacity;                /*!< 最大容量 */
	size_t count;                   /*!< 現在の要素数 */
	int owns_keys;                  /*!< キーを所有するか */
	int owns_values;                /*!< 値を所有するか */

	/* 統計情報 */
	uint64 hits;                    /*!< キャッシュヒット数 */
	uint64 misses;                  /*!< キャッシュミス数 */
	uint64 evictions;               /*!< 削除数 */

	/* コールバック */
	void (*on_evict)(void *key, size_t key_len, void *value, void *user_data);
	void *evict_user_data;
} DKC_LRU_CACHE;

/*! @brief 削除コールバック関数型
	@param key      削除されるキー
	@param key_len  キー長
	@param value    削除される値
	@param user_data ユーザーデータ
*/
typedef void (*dkcLRUEvictCallback)(void *key, size_t key_len,
	void *value, void *user_data);

/* ====================================================================
 * 基本関数
 * ==================================================================== */

/*!
	@brief LRUキャッシュを作成
	@param cache    キャッシュ構造体
	@param capacity 最大容量
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcLRUCacheCreate(DKC_LRU_CACHE *cache, size_t capacity);

/*!
	@brief LRUキャッシュを解放
	@param cache キャッシュ構造体
*/
DKC_EXTERN void WINAPI dkcLRUCacheFree(DKC_LRU_CACHE *cache);

/*!
	@brief キー/値の所有権を設定
	@param cache       キャッシュ構造体
	@param owns_keys   キーを複製して所有するか
	@param owns_values 値を複製して所有するか（サイズ不明のため通常は0）
*/
DKC_EXTERN void WINAPI dkcLRUCacheSetOwnership(DKC_LRU_CACHE *cache,
	int owns_keys, int owns_values);

/*!
	@brief 削除時コールバックを設定
	@param cache     キャッシュ構造体
	@param callback  コールバック関数
	@param user_data コールバックに渡すユーザーデータ
*/
DKC_EXTERN void WINAPI dkcLRUCacheSetEvictCallback(DKC_LRU_CACHE *cache,
	dkcLRUEvictCallback callback, void *user_data);

/* ====================================================================
 * キャッシュ操作
 * ==================================================================== */

/*!
	@brief 値を取得（ヒット時は最近使用に移動）
	@param cache   キャッシュ構造体
	@param key     キー
	@param key_len キー長
	@return 値へのポインタ、見つからない場合はNULL
	@note O(1)
*/
DKC_EXTERN void* WINAPI dkcLRUCacheGet(DKC_LRU_CACHE *cache,
	const void *key, size_t key_len);

/*!
	@brief 文字列キーで値を取得
*/
DKC_EXTERN void* WINAPI dkcLRUCacheGetString(DKC_LRU_CACHE *cache, const char *key);

/*!
	@brief 値を格納（容量超過時は最古を削除）
	@param cache   キャッシュ構造体
	@param key     キー
	@param key_len キー長
	@param value   値
	@return edk_SUCCEEDED または edk_FAILED
	@note O(1)
*/
DKC_EXTERN int WINAPI dkcLRUCachePut(DKC_LRU_CACHE *cache,
	const void *key, size_t key_len, void *value);

/*!
	@brief 文字列キーで値を格納
*/
DKC_EXTERN int WINAPI dkcLRUCachePutString(DKC_LRU_CACHE *cache,
	const char *key, void *value);

/*!
	@brief 値を取得せずに存在確認のみ（位置は移動しない）
	@return TRUE: 存在する, FALSE: 存在しない
*/
DKC_EXTERN BOOL WINAPI dkcLRUCacheContains(const DKC_LRU_CACHE *cache,
	const void *key, size_t key_len);

/*!
	@brief キーを削除
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcLRUCacheRemove(DKC_LRU_CACHE *cache,
	const void *key, size_t key_len);

/*!
	@brief 文字列キーを削除
*/
DKC_EXTERN int WINAPI dkcLRUCacheRemoveString(DKC_LRU_CACHE *cache, const char *key);

/*!
	@brief キャッシュをクリア
*/
DKC_EXTERN void WINAPI dkcLRUCacheClear(DKC_LRU_CACHE *cache);

/* ====================================================================
 * 情報取得
 * ==================================================================== */

/*!
	@brief 現在の要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcLRUCacheCount(const DKC_LRU_CACHE *cache);

/*!
	@brief 容量を取得
*/
DKC_EXTERN size_t WINAPI dkcLRUCacheCapacity(const DKC_LRU_CACHE *cache);

/*!
	@brief キャッシュが満杯か確認
*/
DKC_EXTERN BOOL WINAPI dkcLRUCacheIsFull(const DKC_LRU_CACHE *cache);

/*!
	@brief ヒット率を取得 (0.0〜1.0)
*/
DKC_EXTERN double WINAPI dkcLRUCacheHitRate(const DKC_LRU_CACHE *cache);

/*!
	@brief 統計情報を取得
	@param cache     キャッシュ構造体
	@param hits      [out] ヒット数（NULLでも可）
	@param misses    [out] ミス数（NULLでも可）
	@param evictions [out] 削除数（NULLでも可）
*/
DKC_EXTERN void WINAPI dkcLRUCacheGetStats(const DKC_LRU_CACHE *cache,
	uint64 *hits, uint64 *misses, uint64 *evictions);

/*!
	@brief 統計情報をリセット
*/
DKC_EXTERN void WINAPI dkcLRUCacheResetStats(DKC_LRU_CACHE *cache);

/* ====================================================================
 * 高度な操作
 * ==================================================================== */

/*!
	@brief 最も古い要素のキーを取得（削除はしない）
	@param cache   キャッシュ構造体
	@param key_len [out] キー長（NULLでも可）
	@return キーへのポインタ、空の場合はNULL
*/
DKC_EXTERN const void* WINAPI dkcLRUCachePeekOldest(const DKC_LRU_CACHE *cache,
	size_t *key_len);

/*!
	@brief 最も新しい要素のキーを取得（削除はしない）
*/
DKC_EXTERN const void* WINAPI dkcLRUCachePeekNewest(const DKC_LRU_CACHE *cache,
	size_t *key_len);

/*!
	@brief キャパシティを変更（縮小時は古い要素を削除）
	@param cache        キャッシュ構造体
	@param new_capacity 新しい容量
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcLRUCacheResize(DKC_LRU_CACHE *cache, size_t new_capacity);

/*!
	@brief 全要素に対してコールバックを実行（新しい順）
	@param cache    キャッシュ構造体
	@param callback コールバック関数
	@param user_data ユーザーデータ
	@note コールバック内でキャッシュを変更しないこと
*/
typedef void (*dkcLRUIterCallback)(const void *key, size_t key_len,
	void *value, void *user_data);

DKC_EXTERN void WINAPI dkcLRUCacheForEach(const DKC_LRU_CACHE *cache,
	dkcLRUIterCallback callback, void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* DKC_LRUCACHE_INC_ */
