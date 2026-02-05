/*! @file
	@brief LRUキャッシュ Ver.2（ライブラリコンポーネント再利用版）実装
	@author d.Kingyo
	@note
	dkcDoubleList + dkcHashMap を組み合わせた実装。

	設計ポイント:
	- DKC_DOUBLELIST_OBJECT でアクセス順序を管理
	- DKC_HASHMAP で O(1) キー検索
	- move-to-front は erase + push_front で実現
	  （ノード再作成のため HashMap 更新が必要）
*/

#define DKUTIL_C_LRUCACHE2_C

#include "dkcLRUCache2.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 内部ヘルパー関数
 * ==================================================================== */

/*!
	@brief ノードデータからキーへのポインタを取得
*/
static const void* lru2_get_key_from_node_data(const DKC_LRU2_NODE_DATA *data)
{
	return (const BYTE*)data + sizeof(DKC_LRU2_NODE_DATA);
}

/*!
	@brief リストノードからノードデータを取得
*/
static DKC_LRU2_NODE_DATA* lru2_get_node_data(DKC_DOUBLELIST *node)
{
	return (DKC_LRU2_NODE_DATA*)node->mBuff;
}

/*!
	@brief ノードデータのサイズを計算
*/
static size_t lru2_node_data_size(size_t key_size)
{
	return sizeof(DKC_LRU2_NODE_DATA) + key_size;
}

/*!
	@brief キーを指定してリストノードを検索
	@note HashMapと併用するが、デバッグ・検証用
*/
static DKC_DOUBLELIST* lru2_find_node_by_key(DKC_LRU_CACHE2 *cache, const void *key)
{
	DKC_DOUBLELIST *node;
	DKC_LRU2_NODE_DATA *data;
	const void *node_key;

	if (!cache || !cache->list) return NULL;

	node = cache->list->begin(cache->list);
	while (!cache->list->end(node)) {
		data = lru2_get_node_data(node);
		node_key = lru2_get_key_from_node_data(data);
		if (memcmp(node_key, key, cache->key_size) == 0) {
			return node;
		}
		node = cache->list->next(node);
	}
	return NULL;
}

/*!
	@brief ノードを先頭に移動（erase + push_front で実現）
	@return 新しいノードへのポインタ（HashMapの更新に使用）
*/
static DKC_DOUBLELIST* lru2_move_to_front(DKC_LRU_CACHE2 *cache, DKC_DOUBLELIST *node)
{
	DKC_LRU2_NODE_DATA *old_data;
	size_t data_size;
	BYTE *temp_buffer;
	DKC_DOUBLELIST *new_node;

	if (!cache || !node) return NULL;

	/* 既に先頭なら何もしない */
	if (node == cache->list->mBegin) {
		return node;
	}

	old_data = lru2_get_node_data(node);
	data_size = lru2_node_data_size(cache->key_size);

	/* データを一時コピー */
	temp_buffer = (BYTE*)malloc(data_size);
	if (!temp_buffer) return node;
	memcpy(temp_buffer, old_data, data_size);

	/* 古いノードを削除 */
	cache->list->erase(cache->list, node);

	/* 先頭に新規挿入 */
	if (!cache->list->push_front(cache->list, temp_buffer, data_size)) {
		free(temp_buffer);
		return NULL;
	}
	free(temp_buffer);

	/* 新しい先頭ノードを返す */
	new_node = cache->list->begin(cache->list);
	return new_node;
}

/*!
	@brief 最古のノードを削除（eviction）
*/
static void lru2_evict_oldest(DKC_LRU_CACHE2 *cache)
{
	DKC_DOUBLELIST *tail_node;
	DKC_LRU2_NODE_DATA *data;
	const void *key;

	if (!cache || !cache->list || cache->list->empty(cache->list)) {
		return;
	}

	tail_node = cache->list->tail(cache->list);
	if (!tail_node) return;

	data = lru2_get_node_data(tail_node);
	key = lru2_get_key_from_node_data(data);

	/* コールバック呼び出し */
	if (cache->on_evict) {
		cache->on_evict(key, cache->key_size, data->value, cache->evict_user_data);
	}

	/* HashMapから削除 */
	dkcHashMapErase(cache->map, key);

	/* リストから削除 */
	cache->list->erase(cache->list, tail_node);

	cache->evictions++;
}

/* ====================================================================
 * 基本関数
 * ==================================================================== */

DKC_EXTERN DKC_LRU_CACHE2* WINAPI dkcLRUCache2Create(size_t capacity, size_t key_size)
{
	DKC_LRU_CACHE2 *cache;
	size_t bucket_count;

	if (capacity == 0) capacity = dkcd_LRU2_DEFAULT_CAPACITY;
	if (key_size == 0) return NULL;

	cache = (DKC_LRU_CACHE2*)malloc(sizeof(DKC_LRU_CACHE2));
	if (!cache) return NULL;

	memset(cache, 0, sizeof(DKC_LRU_CACHE2));
	cache->capacity = capacity;
	cache->key_size = key_size;

	/* 空のリストオブジェクトを作成 */
	cache->list = dkcAllocDoubleListObject(NULL, 0);
	if (!cache->list) {
		free(cache);
		return NULL;
	}

	/* ハッシュマップを作成（容量の1.5倍のバケット数） */
	bucket_count = (capacity * 3) / 2;
	if (bucket_count < 16) bucket_count = 16;

	cache->map = dkcAllocHashMap(key_size, bucket_count, NULL, NULL);
	if (!cache->map) {
		dkcFreeDoubleListObject(&cache->list);
		free(cache);
		return NULL;
	}

	return cache;
}

DKC_EXTERN void WINAPI dkcLRUCache2Free(DKC_LRU_CACHE2 **cache)
{
	if (!cache || !*cache) return;

	if ((*cache)->list) {
		dkcFreeDoubleListObject(&(*cache)->list);
	}
	if ((*cache)->map) {
		dkcFreeHashMap(&(*cache)->map);
	}

	free(*cache);
	*cache = NULL;
}

DKC_EXTERN void WINAPI dkcLRUCache2SetEvictCallback(DKC_LRU_CACHE2 *cache,
	dkcLRU2EvictCallback callback, void *user_data)
{
	if (!cache) return;
	cache->on_evict = callback;
	cache->evict_user_data = user_data;
}

/* ====================================================================
 * キャッシュ操作
 * ==================================================================== */

DKC_EXTERN void* WINAPI dkcLRUCache2Get(DKC_LRU_CACHE2 *cache, const void *key)
{
	DKC_DOUBLELIST **node_ptr_storage;
	DKC_DOUBLELIST *node;
	DKC_DOUBLELIST *new_node;
	DKC_LRU2_NODE_DATA *data;

	if (!cache || !key) return NULL;

	/* HashMapでノードポインタを検索 */
	node_ptr_storage = (DKC_DOUBLELIST**)dkcHashMapFind(cache->map, key, NULL);
	if (!node_ptr_storage) {
		cache->misses++;
		return NULL;
	}

	node = *node_ptr_storage;
	cache->hits++;

	/* 先頭に移動 */
	new_node = lru2_move_to_front(cache, node);
	if (new_node && new_node != node) {
		/* ノードポインタが変わったのでHashMapを更新 */
		dkcHashMapSetBuffer(cache->map, key, &new_node, sizeof(DKC_DOUBLELIST*));
	}

	data = lru2_get_node_data(new_node ? new_node : node);
	return data->value;
}

DKC_EXTERN void* WINAPI dkcLRUCache2GetString(DKC_LRU_CACHE2 *cache, const char *key)
{
	char *key_buffer;
	void *result;
	size_t key_len;

	if (!cache || !key) return NULL;

	/* 固定サイズキーバッファにコピー */
	key_buffer = (char*)malloc(cache->key_size);
	if (!key_buffer) return NULL;

	memset(key_buffer, 0, cache->key_size);
	key_len = strlen(key);
	if (key_len >= cache->key_size) {
		key_len = cache->key_size - 1;
	}
	memcpy(key_buffer, key, key_len);

	result = dkcLRUCache2Get(cache, key_buffer);
	free(key_buffer);
	return result;
}

DKC_EXTERN int WINAPI dkcLRUCache2Put(DKC_LRU_CACHE2 *cache,
	const void *key, void *value)
{
	DKC_DOUBLELIST **node_ptr_storage;
	DKC_DOUBLELIST *node;
	DKC_DOUBLELIST *new_node;
	DKC_LRU2_NODE_DATA *data;
	BYTE *node_data_buffer;
	size_t data_size;

	if (!cache || !key) return edk_FAILED;

	/* 既存キーの確認 */
	node_ptr_storage = (DKC_DOUBLELIST**)dkcHashMapFind(cache->map, key, NULL);

	if (node_ptr_storage) {
		/* 既存キー: 値を更新して先頭に移動 */
		node = *node_ptr_storage;
		data = lru2_get_node_data(node);
		data->value = value;

		new_node = lru2_move_to_front(cache, node);
		if (new_node && new_node != node) {
			dkcHashMapSetBuffer(cache->map, key, &new_node, sizeof(DKC_DOUBLELIST*));
		}
		return edk_SUCCEEDED;
	}

	/* 容量チェック: 満杯なら最古を削除 */
	while (cache->list->size(cache->list) >= cache->capacity) {
		lru2_evict_oldest(cache);
	}

	/* 新規ノードデータを作成 */
	data_size = lru2_node_data_size(cache->key_size);
	node_data_buffer = (BYTE*)malloc(data_size);
	if (!node_data_buffer) return edk_FAILED;

	data = (DKC_LRU2_NODE_DATA*)node_data_buffer;
	data->value = value;
	data->key_size = cache->key_size;
	memcpy(node_data_buffer + sizeof(DKC_LRU2_NODE_DATA), key, cache->key_size);

	/* リスト先頭に追加 */
	if (!cache->list->push_front(cache->list, node_data_buffer, data_size)) {
		free(node_data_buffer);
		return edk_FAILED;
	}
	free(node_data_buffer);

	/* HashMapに追加 */
	new_node = cache->list->begin(cache->list);
	if (dkcHashMapInsert(cache->map, key, &new_node, sizeof(DKC_DOUBLELIST*)) != edk_SUCCEEDED) {
		/* 挿入失敗: リストからも削除 */
		cache->list->erase(cache->list, new_node);
		return edk_FAILED;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcLRUCache2PutString(DKC_LRU_CACHE2 *cache,
	const char *key, void *value)
{
	char *key_buffer;
	int result;
	size_t key_len;

	if (!cache || !key) return edk_FAILED;

	/* 固定サイズキーバッファにコピー */
	key_buffer = (char*)malloc(cache->key_size);
	if (!key_buffer) return edk_FAILED;

	memset(key_buffer, 0, cache->key_size);
	key_len = strlen(key);
	if (key_len >= cache->key_size) {
		key_len = cache->key_size - 1;
	}
	memcpy(key_buffer, key, key_len);

	result = dkcLRUCache2Put(cache, key_buffer, value);
	free(key_buffer);
	return result;
}

DKC_EXTERN int WINAPI dkcLRUCache2Remove(DKC_LRU_CACHE2 *cache, const void *key)
{
	DKC_DOUBLELIST **node_ptr_storage;
	DKC_DOUBLELIST *node;

	if (!cache || !key) return edk_FAILED;

	node_ptr_storage = (DKC_DOUBLELIST**)dkcHashMapFind(cache->map, key, NULL);
	if (!node_ptr_storage) {
		return edk_FAILED;
	}

	node = *node_ptr_storage;

	/* HashMapから削除 */
	dkcHashMapErase(cache->map, key);

	/* リストから削除 */
	cache->list->erase(cache->list, node);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcLRUCache2RemoveString(DKC_LRU_CACHE2 *cache, const char *key)
{
	char *key_buffer;
	int result;
	size_t key_len;

	if (!cache || !key) return edk_FAILED;

	key_buffer = (char*)malloc(cache->key_size);
	if (!key_buffer) return edk_FAILED;

	memset(key_buffer, 0, cache->key_size);
	key_len = strlen(key);
	if (key_len >= cache->key_size) {
		key_len = cache->key_size - 1;
	}
	memcpy(key_buffer, key, key_len);

	result = dkcLRUCache2Remove(cache, key_buffer);
	free(key_buffer);
	return result;
}

DKC_EXTERN void WINAPI dkcLRUCache2Clear(DKC_LRU_CACHE2 *cache)
{
	DKC_DOUBLELIST *node;
	DKC_DOUBLELIST *next;

	if (!cache) return;

	/* リストの全ノードを削除 */
	node = cache->list->begin(cache->list);
	while (!cache->list->end(node)) {
		next = cache->list->next(node);
		cache->list->erase(cache->list, node);
		node = next;
	}

	/* HashMapをクリア */
	dkcHashMapClear(cache->map);
}

/* ====================================================================
 * 情報取得
 * ==================================================================== */

DKC_EXTERN size_t WINAPI dkcLRUCache2Count(const DKC_LRU_CACHE2 *cache)
{
	if (!cache || !cache->list) return 0;
	return cache->list->size((DKC_DOUBLELIST_OBJECT*)cache->list);
}

DKC_EXTERN size_t WINAPI dkcLRUCache2Capacity(const DKC_LRU_CACHE2 *cache)
{
	if (!cache) return 0;
	return cache->capacity;
}

DKC_EXTERN BOOL WINAPI dkcLRUCache2IsFull(const DKC_LRU_CACHE2 *cache)
{
	if (!cache) return FALSE;
	return dkcLRUCache2Count(cache) >= cache->capacity;
}

DKC_EXTERN double WINAPI dkcLRUCache2HitRate(const DKC_LRU_CACHE2 *cache)
{
	uint64 total;
	if (!cache) return 0.0;
	total = cache->hits + cache->misses;
	if (total == 0) return 0.0;
	return (double)cache->hits / (double)total;
}

DKC_EXTERN void WINAPI dkcLRUCache2GetStats(const DKC_LRU_CACHE2 *cache,
	uint64 *hits, uint64 *misses, uint64 *evictions)
{
	if (!cache) return;
	if (hits) *hits = cache->hits;
	if (misses) *misses = cache->misses;
	if (evictions) *evictions = cache->evictions;
}

DKC_EXTERN void WINAPI dkcLRUCache2ResetStats(DKC_LRU_CACHE2 *cache)
{
	if (!cache) return;
	cache->hits = 0;
	cache->misses = 0;
	cache->evictions = 0;
}
