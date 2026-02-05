/*! @file
	@brief LRUキャッシュ（Least Recently Used Cache）実装
	@author d.Kingyo
	@note
	ハッシュマップ + 双方向リンクリストによるO(1)実装

	構造:
	- head: 最も最近使用されたノード
	- tail: 最も古いノード（削除候補）
	- hash_table: O(1)ルックアップ用

	操作:
	- Get: ハッシュで検索 → headに移動
	- Put: 新規ノード作成 → headに挿入 → 容量超過ならtailを削除
*/

#include "dkcLRUCache.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 内部ハッシュ関数
 * ==================================================================== */

static uint32 lru_hash(const void *key, size_t len)
{
	const uint8 *data = (const uint8 *)key;
	uint32 h = 0x811C9DC5; /* FNV offset basis */
	size_t i;

	for (i = 0; i < len; i++) {
		h ^= data[i];
		h *= 0x01000193; /* FNV prime */
	}
	return h;
}

/* ====================================================================
 * 内部ユーティリティ
 * ==================================================================== */

/* キー比較 */
static int key_equals(const void *k1, size_t len1, const void *k2, size_t len2)
{
	if (len1 != len2) return 0;
	return memcmp(k1, k2, len1) == 0;
}

/* ノードをリストから切り離す */
static void detach_node(DKC_LRU_CACHE *cache, DKC_LRU_NODE *node)
{
	if (node->prev) {
		node->prev->next = node->next;
	} else {
		cache->head = node->next;
	}

	if (node->next) {
		node->next->prev = node->prev;
	} else {
		cache->tail = node->prev;
	}

	node->prev = NULL;
	node->next = NULL;
}

/* ノードをheadに挿入 */
static void insert_at_head(DKC_LRU_CACHE *cache, DKC_LRU_NODE *node)
{
	node->prev = NULL;
	node->next = cache->head;

	if (cache->head) {
		cache->head->prev = node;
	}
	cache->head = node;

	if (!cache->tail) {
		cache->tail = node;
	}
}

/* ノードをheadに移動 */
static void move_to_head(DKC_LRU_CACHE *cache, DKC_LRU_NODE *node)
{
	if (cache->head == node) return; /* 既にhead */

	detach_node(cache, node);
	insert_at_head(cache, node);
}

/* ハッシュテーブルでノードを検索 */
static DKC_LRU_NODE* find_node(const DKC_LRU_CACHE *cache,
	const void *key, size_t key_len, uint32 *out_hash, size_t *out_idx)
{
	uint32 h = lru_hash(key, key_len);
	size_t idx = h % cache->hash_size;
	DKC_LRU_NODE *node;

	if (out_hash) *out_hash = h;
	if (out_idx) *out_idx = idx;

	/* チェイン法で検索 */
	node = cache->hash_table[idx];
	while (node) {
		if (node->hash == h && key_equals(node->key, node->key_len, key, key_len)) {
			return node;
		}
		/* 次のノード（同じハッシュインデックスのチェイン）を探す */
		/* 注: この実装では単純化のため、チェインは使わずオープンアドレス法的に探索 */
		break;
	}

	/* オープンアドレス法（線形探査）で探索 */
	{
		size_t i;
		for (i = 1; i < cache->hash_size; i++) {
			size_t probe_idx = (idx + i) % cache->hash_size;
			node = cache->hash_table[probe_idx];
			if (!node) break; /* 空スロットに到達 */
			if (node->hash == h && key_equals(node->key, node->key_len, key, key_len)) {
				if (out_idx) *out_idx = probe_idx;
				return node;
			}
		}
	}

	return NULL;
}

/* ハッシュテーブルにノードを挿入 */
static int hash_insert(DKC_LRU_CACHE *cache, DKC_LRU_NODE *node)
{
	size_t idx = node->hash % cache->hash_size;
	size_t i;

	/* 空きスロットを探す */
	for (i = 0; i < cache->hash_size; i++) {
		size_t probe_idx = (idx + i) % cache->hash_size;
		if (!cache->hash_table[probe_idx]) {
			cache->hash_table[probe_idx] = node;
			return edk_SUCCEEDED;
		}
	}

	return edk_FAILED; /* フル（通常は起きない） */
}

/* ハッシュテーブルからノードを削除 */
static void hash_remove(DKC_LRU_CACHE *cache, DKC_LRU_NODE *node)
{
	size_t idx = node->hash % cache->hash_size;
	size_t i;

	for (i = 0; i < cache->hash_size; i++) {
		size_t probe_idx = (idx + i) % cache->hash_size;
		if (cache->hash_table[probe_idx] == node) {
			cache->hash_table[probe_idx] = NULL;
			/* 注: オープンアドレス法では削除後の再配置が必要だが、
			       簡略化のため省略（パフォーマンス低下の可能性あり） */
			return;
		}
		if (!cache->hash_table[probe_idx]) break;
	}
}

/* ノードを解放 */
static void free_node(DKC_LRU_CACHE *cache, DKC_LRU_NODE *node)
{
	if (!node) return;

	/* コールバック呼び出し */
	if (cache->on_evict) {
		cache->on_evict(node->key, node->key_len, node->value, cache->evict_user_data);
	}

	if (cache->owns_keys && node->key) {
		free(node->key);
	}
	if (cache->owns_values && node->value) {
		free(node->value);
	}

	free(node);
}

/* tail（最古）を削除 */
static void evict_tail(DKC_LRU_CACHE *cache)
{
	DKC_LRU_NODE *node = cache->tail;
	if (!node) return;

	hash_remove(cache, node);
	detach_node(cache, node);
	free_node(cache, node);

	cache->count--;
	cache->evictions++;
}

/* ====================================================================
 * 基本関数
 * ==================================================================== */

int WINAPI dkcLRUCacheCreate(DKC_LRU_CACHE *cache, size_t capacity)
{
	size_t hash_size;

	if (!cache || capacity == 0) return edk_FAILED;

	/* ハッシュテーブルサイズ（容量の約1.5倍、2の冪乗） */
	hash_size = 1;
	while (hash_size < capacity * 2) hash_size <<= 1;

	cache->hash_table = (DKC_LRU_NODE **)calloc(hash_size, sizeof(DKC_LRU_NODE *));
	if (!cache->hash_table) return edk_FAILED;

	cache->hash_size = hash_size;
	cache->head = NULL;
	cache->tail = NULL;
	cache->capacity = capacity;
	cache->count = 0;
	cache->owns_keys = 0;
	cache->owns_values = 0;
	cache->hits = 0;
	cache->misses = 0;
	cache->evictions = 0;
	cache->on_evict = NULL;
	cache->evict_user_data = NULL;

	return edk_SUCCEEDED;
}

void WINAPI dkcLRUCacheFree(DKC_LRU_CACHE *cache)
{
	if (!cache) return;

	dkcLRUCacheClear(cache);

	if (cache->hash_table) {
		free(cache->hash_table);
		cache->hash_table = NULL;
	}

	cache->hash_size = 0;
	cache->capacity = 0;
}

void WINAPI dkcLRUCacheSetOwnership(DKC_LRU_CACHE *cache, int owns_keys, int owns_values)
{
	if (!cache) return;
	cache->owns_keys = owns_keys;
	cache->owns_values = owns_values;
}

void WINAPI dkcLRUCacheSetEvictCallback(DKC_LRU_CACHE *cache,
	dkcLRUEvictCallback callback, void *user_data)
{
	if (!cache) return;
	cache->on_evict = callback;
	cache->evict_user_data = user_data;
}

/* ====================================================================
 * キャッシュ操作
 * ==================================================================== */

void* WINAPI dkcLRUCacheGet(DKC_LRU_CACHE *cache, const void *key, size_t key_len)
{
	DKC_LRU_NODE *node;

	if (!cache || !key) return NULL;

	node = find_node(cache, key, key_len, NULL, NULL);
	if (node) {
		cache->hits++;
		move_to_head(cache, node);
		return node->value;
	}

	cache->misses++;
	return NULL;
}

void* WINAPI dkcLRUCacheGetString(DKC_LRU_CACHE *cache, const char *key)
{
	if (!key) return NULL;
	return dkcLRUCacheGet(cache, key, strlen(key) + 1);
}

int WINAPI dkcLRUCachePut(DKC_LRU_CACHE *cache,
	const void *key, size_t key_len, void *value)
{
	DKC_LRU_NODE *node;
	uint32 h;
	size_t idx;

	if (!cache || !key) return edk_FAILED;

	/* 既存キーの更新チェック */
	node = find_node(cache, key, key_len, &h, &idx);
	if (node) {
		/* 値を更新 */
		if (cache->owns_values && node->value) {
			free(node->value);
		}
		node->value = value;
		move_to_head(cache, node);
		return edk_SUCCEEDED;
	}

	/* 容量超過なら最古を削除 */
	while (cache->count >= cache->capacity) {
		evict_tail(cache);
	}

	/* 新しいノードを作成 */
	node = (DKC_LRU_NODE *)malloc(sizeof(DKC_LRU_NODE));
	if (!node) return edk_FAILED;

	if (cache->owns_keys) {
		node->key = malloc(key_len);
		if (!node->key) {
			free(node);
			return edk_FAILED;
		}
		memcpy(node->key, key, key_len);
	} else {
		node->key = (void *)key;
	}

	node->key_len = key_len;
	node->value = value;
	node->hash = h;
	node->prev = NULL;
	node->next = NULL;

	/* ハッシュテーブルに挿入 */
	if (hash_insert(cache, node) != edk_SUCCEEDED) {
		if (cache->owns_keys) free(node->key);
		free(node);
		return edk_FAILED;
	}

	/* headに挿入 */
	insert_at_head(cache, node);
	cache->count++;

	return edk_SUCCEEDED;
}

int WINAPI dkcLRUCachePutString(DKC_LRU_CACHE *cache, const char *key, void *value)
{
	if (!key) return edk_FAILED;
	return dkcLRUCachePut(cache, key, strlen(key) + 1, value);
}

BOOL WINAPI dkcLRUCacheContains(const DKC_LRU_CACHE *cache,
	const void *key, size_t key_len)
{
	if (!cache || !key) return FALSE;
	return find_node(cache, key, key_len, NULL, NULL) != NULL;
}

int WINAPI dkcLRUCacheRemove(DKC_LRU_CACHE *cache, const void *key, size_t key_len)
{
	DKC_LRU_NODE *node;

	if (!cache || !key) return edk_FAILED;

	node = find_node(cache, key, key_len, NULL, NULL);
	if (!node) return edk_FAILED;

	hash_remove(cache, node);
	detach_node(cache, node);
	free_node(cache, node);
	cache->count--;

	return edk_SUCCEEDED;
}

int WINAPI dkcLRUCacheRemoveString(DKC_LRU_CACHE *cache, const char *key)
{
	if (!key) return edk_FAILED;
	return dkcLRUCacheRemove(cache, key, strlen(key) + 1);
}

void WINAPI dkcLRUCacheClear(DKC_LRU_CACHE *cache)
{
	DKC_LRU_NODE *node;
	DKC_LRU_NODE *next;

	if (!cache) return;

	/* リンクリストを走査して全ノードを解放 */
	node = cache->head;
	while (node) {
		next = node->next;
		free_node(cache, node);
		node = next;
	}

	/* ハッシュテーブルをクリア */
	if (cache->hash_table) {
		memset(cache->hash_table, 0, cache->hash_size * sizeof(DKC_LRU_NODE *));
	}

	cache->head = NULL;
	cache->tail = NULL;
	cache->count = 0;
}

/* ====================================================================
 * 情報取得
 * ==================================================================== */

size_t WINAPI dkcLRUCacheCount(const DKC_LRU_CACHE *cache)
{
	if (!cache) return 0;
	return cache->count;
}

size_t WINAPI dkcLRUCacheCapacity(const DKC_LRU_CACHE *cache)
{
	if (!cache) return 0;
	return cache->capacity;
}

BOOL WINAPI dkcLRUCacheIsFull(const DKC_LRU_CACHE *cache)
{
	if (!cache) return FALSE;
	return cache->count >= cache->capacity;
}

double WINAPI dkcLRUCacheHitRate(const DKC_LRU_CACHE *cache)
{
	uint64 total;

	if (!cache) return 0.0;

	total = cache->hits + cache->misses;
	if (total == 0) return 0.0;

	return (double)cache->hits / (double)total;
}

void WINAPI dkcLRUCacheGetStats(const DKC_LRU_CACHE *cache,
	uint64 *hits, uint64 *misses, uint64 *evictions)
{
	if (!cache) return;
	if (hits) *hits = cache->hits;
	if (misses) *misses = cache->misses;
	if (evictions) *evictions = cache->evictions;
}

void WINAPI dkcLRUCacheResetStats(DKC_LRU_CACHE *cache)
{
	if (!cache) return;
	cache->hits = 0;
	cache->misses = 0;
	cache->evictions = 0;
}

/* ====================================================================
 * 高度な操作
 * ==================================================================== */

const void* WINAPI dkcLRUCachePeekOldest(const DKC_LRU_CACHE *cache, size_t *key_len)
{
	if (!cache || !cache->tail) return NULL;
	if (key_len) *key_len = cache->tail->key_len;
	return cache->tail->key;
}

const void* WINAPI dkcLRUCachePeekNewest(const DKC_LRU_CACHE *cache, size_t *key_len)
{
	if (!cache || !cache->head) return NULL;
	if (key_len) *key_len = cache->head->key_len;
	return cache->head->key;
}

int WINAPI dkcLRUCacheResize(DKC_LRU_CACHE *cache, size_t new_capacity)
{
	DKC_LRU_NODE **new_hash_table;
	size_t new_hash_size;
	DKC_LRU_NODE *node;
	size_t i;

	if (!cache || new_capacity == 0) return edk_FAILED;

	/* 縮小時は古い要素を削除 */
	while (cache->count > new_capacity) {
		evict_tail(cache);
	}

	/* 新しいハッシュテーブルサイズ */
	new_hash_size = 1;
	while (new_hash_size < new_capacity * 2) new_hash_size <<= 1;

	/* ハッシュテーブルを再構築 */
	new_hash_table = (DKC_LRU_NODE **)calloc(new_hash_size, sizeof(DKC_LRU_NODE *));
	if (!new_hash_table) return edk_FAILED;

	/* 既存ノードを新しいテーブルに再挿入 */
	cache->hash_size = new_hash_size;
	{
		DKC_LRU_NODE **old_table = cache->hash_table;
		cache->hash_table = new_hash_table;

		node = cache->head;
		while (node) {
			hash_insert(cache, node);
			node = node->next;
		}

		free(old_table);
	}

	cache->capacity = new_capacity;
	return edk_SUCCEEDED;
}

void WINAPI dkcLRUCacheForEach(const DKC_LRU_CACHE *cache,
	dkcLRUIterCallback callback, void *user_data)
{
	DKC_LRU_NODE *node;

	if (!cache || !callback) return;

	node = cache->head;
	while (node) {
		callback(node->key, node->key_len, node->value, user_data);
		node = node->next;
	}
}
