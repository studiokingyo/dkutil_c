/*! @file
	@brief LRUキャッシュ Ver.2（ライブラリコンポーネント再利用版）
	@author d.Kingyo
	@note
	dkcDoubleList + dkcHashMap を組み合わせた実装。
	dkcLRUCache.c がフルスクラッチ実装なのに対し、
	こちらは既存ライブラリコンポーネントの再利用を示すデモ実装。

	構成:
	- DKC_DOUBLELIST_OBJECT: アクセス順序管理（head=最新, tail=最古）
	- DKC_HASHMAP: O(1)キー検索

	特徴:
	- Get: O(1) 平均
	- Put: O(1) 平均
	- 固定サイズキー（作成時に指定）
*/

#ifndef DKC_LRUCACHE2_INC_
#define DKC_LRUCACHE2_INC_

#include "dkcOSIndependent.h"
#include "dkcDoubleList.h"
#include "dkcHashMap.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数
 * ==================================================================== */

#define dkcd_LRU2_DEFAULT_CAPACITY    16      /*!< デフォルト容量 */

/* ====================================================================
 * 構造体
 * ==================================================================== */

/*! @brief LRUキャッシュ2構造体 */
typedef struct dkc_LRUCache2 {
	DKC_DOUBLELIST_OBJECT *list;    /*!< アクセス順序リスト */
	DKC_HASHMAP *map;               /*!< キー→ノードマッピング */
	size_t capacity;                /*!< 最大容量 */
	size_t key_size;                /*!< キーサイズ（バイト） */

	/* 統計情報 */
	uint64 hits;                    /*!< キャッシュヒット数 */
	uint64 misses;                  /*!< キャッシュミス数 */
	uint64 evictions;               /*!< 削除数 */

	/* コールバック */
	void (*on_evict)(const void *key, size_t key_size, void *value, void *user_data);
	void *evict_user_data;
} DKC_LRU_CACHE2;

/*! @brief ノードデータ構造（リスト内に格納）
	@note キーデータはこの構造体の直後に格納される
*/
typedef struct dkc_LRU2NodeData {
	void *value;                    /*!< ユーザーの値ポインタ */
	size_t key_size;                /*!< キーサイズ */
	/* キーデータはここに続く: BYTE key[key_size] */
} DKC_LRU2_NODE_DATA;

/*! @brief 削除コールバック関数型 */
typedef void (*dkcLRU2EvictCallback)(const void *key, size_t key_size,
	void *value, void *user_data);

/* ====================================================================
 * 基本関数
 * ==================================================================== */

/*!
	@brief LRUキャッシュ2を作成
	@param capacity 最大容量
	@param key_size キーサイズ（バイト）
	@return キャッシュへのポインタ、失敗時はNULL
*/
DKC_EXTERN DKC_LRU_CACHE2* WINAPI dkcLRUCache2Create(size_t capacity, size_t key_size);

/*!
	@brief LRUキャッシュ2を解放
	@param cache キャッシュへのポインタのポインタ
*/
DKC_EXTERN void WINAPI dkcLRUCache2Free(DKC_LRU_CACHE2 **cache);

/*!
	@brief 削除時コールバックを設定
	@param cache     キャッシュ
	@param callback  コールバック関数
	@param user_data コールバックに渡すユーザーデータ
*/
DKC_EXTERN void WINAPI dkcLRUCache2SetEvictCallback(DKC_LRU_CACHE2 *cache,
	dkcLRU2EvictCallback callback, void *user_data);

/* ====================================================================
 * キャッシュ操作
 * ==================================================================== */

/*!
	@brief 値を取得（ヒット時は最近使用に移動）
	@param cache キャッシュ
	@param key   キー
	@return 値へのポインタ、見つからない場合はNULL
*/
DKC_EXTERN void* WINAPI dkcLRUCache2Get(DKC_LRU_CACHE2 *cache, const void *key);

/*!
	@brief 文字列キーで値を取得
	@note キーサイズはstrlen(key)+1として扱う
*/
DKC_EXTERN void* WINAPI dkcLRUCache2GetString(DKC_LRU_CACHE2 *cache, const char *key);

/*!
	@brief 値を格納（容量超過時は最古を削除）
	@param cache キャッシュ
	@param key   キー
	@param value 値
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcLRUCache2Put(DKC_LRU_CACHE2 *cache,
	const void *key, void *value);

/*!
	@brief 文字列キーで値を格納
*/
DKC_EXTERN int WINAPI dkcLRUCache2PutString(DKC_LRU_CACHE2 *cache,
	const char *key, void *value);

/*!
	@brief キーを削除
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcLRUCache2Remove(DKC_LRU_CACHE2 *cache, const void *key);

/*!
	@brief 文字列キーを削除
*/
DKC_EXTERN int WINAPI dkcLRUCache2RemoveString(DKC_LRU_CACHE2 *cache, const char *key);

/*!
	@brief キャッシュをクリア
*/
DKC_EXTERN void WINAPI dkcLRUCache2Clear(DKC_LRU_CACHE2 *cache);

/* ====================================================================
 * 情報取得
 * ==================================================================== */

/*!
	@brief 現在の要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcLRUCache2Count(const DKC_LRU_CACHE2 *cache);

/*!
	@brief 容量を取得
*/
DKC_EXTERN size_t WINAPI dkcLRUCache2Capacity(const DKC_LRU_CACHE2 *cache);

/*!
	@brief キャッシュが満杯か確認
*/
DKC_EXTERN BOOL WINAPI dkcLRUCache2IsFull(const DKC_LRU_CACHE2 *cache);

/*!
	@brief ヒット率を取得 (0.0〜1.0)
*/
DKC_EXTERN double WINAPI dkcLRUCache2HitRate(const DKC_LRU_CACHE2 *cache);

/*!
	@brief 統計情報を取得
*/
DKC_EXTERN void WINAPI dkcLRUCache2GetStats(const DKC_LRU_CACHE2 *cache,
	uint64 *hits, uint64 *misses, uint64 *evictions);

/*!
	@brief 統計情報をリセット
*/
DKC_EXTERN void WINAPI dkcLRUCache2ResetStats(DKC_LRU_CACHE2 *cache);

#ifdef __cplusplus
}
#endif

#endif /* DKC_LRUCACHE2_INC_ */
