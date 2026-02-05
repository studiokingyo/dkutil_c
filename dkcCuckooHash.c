/*! @file
	@brief Cuckoo Hashing（カッコウハッシュ）実装
	@author d.Kingyo
	@note
	2つのハッシュテーブルを使用したCuckoo Hashing実装

	挿入アルゴリズム:
	1. h1(key)の位置が空なら挿入
	2. 空でなければ既存要素を追い出してh1(key)に挿入
	3. 追い出された要素をもう一方のテーブルに挿入
	4. ループが検出されたらリハッシュ
*/

#include "dkcCuckooHash.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 内部ハッシュ関数
 * ==================================================================== */

static uint32 rotl32(uint32 x, int r)
{
	return (x << r) | (x >> (32 - r));
}

static uint32 murmur3_32(const void *key, size_t len, uint32 seed)
{
	const uint8 *data = (const uint8 *)key;
	const size_t nblocks = len / 4;
	uint32 h1 = seed;
	const uint32 c1 = 0xCC9E2D51;
	const uint32 c2 = 0x1B873593;
	const uint8 *tail;
	uint32 k1;
	size_t i;

	for (i = 0; i < nblocks; i++) {
		const uint8 *p = data + i * 4;
		k1 = (uint32)p[0] | ((uint32)p[1] << 8) |
			 ((uint32)p[2] << 16) | ((uint32)p[3] << 24);

		k1 *= c1;
		k1 = rotl32(k1, 15);
		k1 *= c2;

		h1 ^= k1;
		h1 = rotl32(h1, 13);
		h1 = h1 * 5 + 0xE6546B64;
	}

	tail = data + nblocks * 4;
	k1 = 0;
	switch (len & 3) {
	case 3: k1 ^= (uint32)tail[2] << 16; /* fall through */
	case 2: k1 ^= (uint32)tail[1] << 8;  /* fall through */
	case 1: k1 ^= (uint32)tail[0];
		k1 *= c1;
		k1 = rotl32(k1, 15);
		k1 *= c2;
		h1 ^= k1;
	}

	h1 ^= (uint32)len;
	h1 ^= h1 >> 16;
	h1 *= 0x85EBCA6B;
	h1 ^= h1 >> 13;
	h1 *= 0xC2B2AE35;
	h1 ^= h1 >> 16;

	return h1;
}

/* フィンガープリント計算 */
static uint16 fingerprint(const void *data, size_t len, uint32 seed)
{
	uint32 h = murmur3_32(data, len, seed);
	uint16 fp = (uint16)(h & 0xFFFF);
	/* 0は空を表すので避ける */
	if (fp == 0) fp = 1;
	return fp;
}

/* キー比較 */
static int key_equals(const void *k1, size_t len1, const void *k2, size_t len2)
{
	if (len1 != len2) return 0;
	return memcmp(k1, k2, len1) == 0;
}

/* ====================================================================
 * Cuckoo Hash Table 実装
 * ==================================================================== */

int WINAPI dkcCuckooHashCreate(DKC_CUCKOO_HASH *hash, size_t capacity)
{
	if (!hash) return edk_FAILED;

	if (capacity < dkcd_CUCKOO_DEFAULT_SIZE) {
		capacity = dkcd_CUCKOO_DEFAULT_SIZE;
	}

	/* 2の冪乗に切り上げ */
	{
		size_t n = 1;
		while (n < capacity) n <<= 1;
		capacity = n;
	}

	hash->table1 = (DKC_CUCKOO_ENTRY *)calloc(capacity, sizeof(DKC_CUCKOO_ENTRY));
	hash->table2 = (DKC_CUCKOO_ENTRY *)calloc(capacity, sizeof(DKC_CUCKOO_ENTRY));

	if (!hash->table1 || !hash->table2) {
		if (hash->table1) free(hash->table1);
		if (hash->table2) free(hash->table2);
		hash->table1 = hash->table2 = NULL;
		return edk_FAILED;
	}

	hash->capacity = capacity;
	hash->num_items = 0;
	hash->seed1 = 0x9E3779B9;
	hash->seed2 = 0x85EBCA6B;
	hash->owns_keys = 0;
	hash->owns_values = 0;

	return edk_SUCCEEDED;
}

void WINAPI dkcCuckooHashFree(DKC_CUCKOO_HASH *hash)
{
	if (!hash) return;

	dkcCuckooHashClear(hash);

	if (hash->table1) {
		free(hash->table1);
		hash->table1 = NULL;
	}
	if (hash->table2) {
		free(hash->table2);
		hash->table2 = NULL;
	}
	hash->capacity = 0;
	hash->num_items = 0;
}

void WINAPI dkcCuckooHashSetOwnership(DKC_CUCKOO_HASH *hash, int owns_keys, int owns_values)
{
	if (!hash) return;
	hash->owns_keys = owns_keys;
	hash->owns_values = owns_values;
}

static void free_entry(DKC_CUCKOO_HASH *hash, DKC_CUCKOO_ENTRY *entry)
{
	if (!entry || !entry->key) return;

	if (hash->owns_keys && entry->key) {
		free(entry->key);
	}
	if (hash->owns_values && entry->value) {
		free(entry->value);
	}
	entry->key = NULL;
	entry->key_len = 0;
	entry->value = NULL;
}

/* 前方宣言 */
static int cuckoo_rehash(DKC_CUCKOO_HASH *hash);

static int cuckoo_insert_internal(DKC_CUCKOO_HASH *hash,
	void *key, size_t key_len, void *value, int depth)
{
	uint32 h1, h2;
	size_t idx;
	DKC_CUCKOO_ENTRY temp;
	int i;

	if (depth > dkcd_CUCKOO_MAX_KICKS) {
		/* リハッシュが必要 */
		if (cuckoo_rehash(hash) != edk_SUCCEEDED) {
			return edk_FAILED;
		}
		/* リハッシュ後に再挿入 */
		return cuckoo_insert_internal(hash, key, key_len, value, 0);
	}

	h1 = murmur3_32(key, key_len, hash->seed1);
	idx = h1 & (hash->capacity - 1);

	/* テーブル1が空なら挿入 */
	if (hash->table1[idx].key == NULL) {
		hash->table1[idx].key = key;
		hash->table1[idx].key_len = key_len;
		hash->table1[idx].value = value;
		hash->num_items++;
		return edk_SUCCEEDED;
	}

	h2 = murmur3_32(key, key_len, hash->seed2);
	idx = h2 & (hash->capacity - 1);

	/* テーブル2が空なら挿入 */
	if (hash->table2[idx].key == NULL) {
		hash->table2[idx].key = key;
		hash->table2[idx].key_len = key_len;
		hash->table2[idx].value = value;
		hash->num_items++;
		return edk_SUCCEEDED;
	}

	/* 両方埋まっている場合、追い出し開始 */
	/* テーブル1から追い出し */
	idx = h1 & (hash->capacity - 1);
	temp = hash->table1[idx];
	hash->table1[idx].key = key;
	hash->table1[idx].key_len = key_len;
	hash->table1[idx].value = value;

	/* 追い出された要素をテーブル2へ */
	h2 = murmur3_32(temp.key, temp.key_len, hash->seed2);
	idx = h2 & (hash->capacity - 1);

	if (hash->table2[idx].key == NULL) {
		hash->table2[idx] = temp;
		hash->num_items++;
		return edk_SUCCEEDED;
	}

	/* テーブル2からも追い出し、再帰的に処理 */
	{
		DKC_CUCKOO_ENTRY temp2 = hash->table2[idx];
		hash->table2[idx] = temp;
		return cuckoo_insert_internal(hash, temp2.key, temp2.key_len,
			temp2.value, depth + 1);
	}
}

static int cuckoo_rehash(DKC_CUCKOO_HASH *hash)
{
	DKC_CUCKOO_ENTRY *old_table1 = hash->table1;
	DKC_CUCKOO_ENTRY *old_table2 = hash->table2;
	size_t old_capacity = hash->capacity;
	size_t new_capacity = old_capacity * 2;
	size_t i;

	/* 新しいテーブルを作成 */
	hash->table1 = (DKC_CUCKOO_ENTRY *)calloc(new_capacity, sizeof(DKC_CUCKOO_ENTRY));
	hash->table2 = (DKC_CUCKOO_ENTRY *)calloc(new_capacity, sizeof(DKC_CUCKOO_ENTRY));

	if (!hash->table1 || !hash->table2) {
		if (hash->table1) free(hash->table1);
		if (hash->table2) free(hash->table2);
		hash->table1 = old_table1;
		hash->table2 = old_table2;
		return edk_FAILED;
	}

	hash->capacity = new_capacity;
	hash->num_items = 0;

	/* シードを変更（サイクル回避） */
	hash->seed1 += 0x12345678;
	hash->seed2 += 0x87654321;

	/* 既存要素を再挿入 */
	for (i = 0; i < old_capacity; i++) {
		if (old_table1[i].key) {
			if (cuckoo_insert_internal(hash, old_table1[i].key,
				old_table1[i].key_len, old_table1[i].value, 0) != edk_SUCCEEDED) {
				/* 再挿入失敗（通常は起きない） */
				free(hash->table1);
				free(hash->table2);
				hash->table1 = old_table1;
				hash->table2 = old_table2;
				hash->capacity = old_capacity;
				return edk_FAILED;
			}
		}
		if (old_table2[i].key) {
			if (cuckoo_insert_internal(hash, old_table2[i].key,
				old_table2[i].key_len, old_table2[i].value, 0) != edk_SUCCEEDED) {
				free(hash->table1);
				free(hash->table2);
				hash->table1 = old_table1;
				hash->table2 = old_table2;
				hash->capacity = old_capacity;
				return edk_FAILED;
			}
		}
	}

	free(old_table1);
	free(old_table2);
	return edk_SUCCEEDED;
}

int WINAPI dkcCuckooHashInsert(DKC_CUCKOO_HASH *hash,
	const void *key, size_t key_len, void *value)
{
	void *key_copy;
	uint32 h1, h2;
	size_t idx;

	if (!hash || !hash->table1 || !key) return edk_FAILED;

	/* 既存キーの更新チェック */
	h1 = murmur3_32(key, key_len, hash->seed1);
	idx = h1 & (hash->capacity - 1);
	if (hash->table1[idx].key &&
		key_equals(hash->table1[idx].key, hash->table1[idx].key_len, key, key_len)) {
		if (hash->owns_values && hash->table1[idx].value) {
			free(hash->table1[idx].value);
		}
		hash->table1[idx].value = value;
		return edk_SUCCEEDED;
	}

	h2 = murmur3_32(key, key_len, hash->seed2);
	idx = h2 & (hash->capacity - 1);
	if (hash->table2[idx].key &&
		key_equals(hash->table2[idx].key, hash->table2[idx].key_len, key, key_len)) {
		if (hash->owns_values && hash->table2[idx].value) {
			free(hash->table2[idx].value);
		}
		hash->table2[idx].value = value;
		return edk_SUCCEEDED;
	}

	/* 負荷率チェック */
	if ((double)hash->num_items / (double)(hash->capacity * 2) >= dkcd_CUCKOO_LOAD_FACTOR) {
		if (cuckoo_rehash(hash) != edk_SUCCEEDED) {
			return edk_FAILED;
		}
	}

	/* キーをコピー（所有する場合） */
	if (hash->owns_keys) {
		key_copy = malloc(key_len);
		if (!key_copy) return edk_FAILED;
		memcpy(key_copy, key, key_len);
	} else {
		key_copy = (void *)key;
	}

	return cuckoo_insert_internal(hash, key_copy, key_len, value, 0);
}

int WINAPI dkcCuckooHashInsertString(DKC_CUCKOO_HASH *hash, const char *key, void *value)
{
	if (!key) return edk_FAILED;
	return dkcCuckooHashInsert(hash, key, strlen(key) + 1, value);
}

void* WINAPI dkcCuckooHashLookup(const DKC_CUCKOO_HASH *hash,
	const void *key, size_t key_len)
{
	uint32 h1, h2;
	size_t idx;

	if (!hash || !hash->table1 || !key) return NULL;

	/* テーブル1をチェック */
	h1 = murmur3_32(key, key_len, hash->seed1);
	idx = h1 & (hash->capacity - 1);
	if (hash->table1[idx].key &&
		key_equals(hash->table1[idx].key, hash->table1[idx].key_len, key, key_len)) {
		return hash->table1[idx].value;
	}

	/* テーブル2をチェック */
	h2 = murmur3_32(key, key_len, hash->seed2);
	idx = h2 & (hash->capacity - 1);
	if (hash->table2[idx].key &&
		key_equals(hash->table2[idx].key, hash->table2[idx].key_len, key, key_len)) {
		return hash->table2[idx].value;
	}

	return NULL;
}

void* WINAPI dkcCuckooHashLookupString(const DKC_CUCKOO_HASH *hash, const char *key)
{
	if (!key) return NULL;
	return dkcCuckooHashLookup(hash, key, strlen(key) + 1);
}

BOOL WINAPI dkcCuckooHashContains(const DKC_CUCKOO_HASH *hash,
	const void *key, size_t key_len)
{
	uint32 h1, h2;
	size_t idx;

	if (!hash || !hash->table1 || !key) return FALSE;

	h1 = murmur3_32(key, key_len, hash->seed1);
	idx = h1 & (hash->capacity - 1);
	if (hash->table1[idx].key &&
		key_equals(hash->table1[idx].key, hash->table1[idx].key_len, key, key_len)) {
		return TRUE;
	}

	h2 = murmur3_32(key, key_len, hash->seed2);
	idx = h2 & (hash->capacity - 1);
	if (hash->table2[idx].key &&
		key_equals(hash->table2[idx].key, hash->table2[idx].key_len, key, key_len)) {
		return TRUE;
	}

	return FALSE;
}

int WINAPI dkcCuckooHashRemove(DKC_CUCKOO_HASH *hash, const void *key, size_t key_len)
{
	uint32 h1, h2;
	size_t idx;

	if (!hash || !hash->table1 || !key) return edk_FAILED;

	h1 = murmur3_32(key, key_len, hash->seed1);
	idx = h1 & (hash->capacity - 1);
	if (hash->table1[idx].key &&
		key_equals(hash->table1[idx].key, hash->table1[idx].key_len, key, key_len)) {
		free_entry(hash, &hash->table1[idx]);
		hash->num_items--;
		return edk_SUCCEEDED;
	}

	h2 = murmur3_32(key, key_len, hash->seed2);
	idx = h2 & (hash->capacity - 1);
	if (hash->table2[idx].key &&
		key_equals(hash->table2[idx].key, hash->table2[idx].key_len, key, key_len)) {
		free_entry(hash, &hash->table2[idx]);
		hash->num_items--;
		return edk_SUCCEEDED;
	}

	return edk_FAILED;
}

int WINAPI dkcCuckooHashRemoveString(DKC_CUCKOO_HASH *hash, const char *key)
{
	if (!key) return edk_FAILED;
	return dkcCuckooHashRemove(hash, key, strlen(key) + 1);
}

void WINAPI dkcCuckooHashClear(DKC_CUCKOO_HASH *hash)
{
	size_t i;

	if (!hash) return;

	if (hash->table1) {
		for (i = 0; i < hash->capacity; i++) {
			free_entry(hash, &hash->table1[i]);
		}
	}
	if (hash->table2) {
		for (i = 0; i < hash->capacity; i++) {
			free_entry(hash, &hash->table2[i]);
		}
	}
	hash->num_items = 0;
}

size_t WINAPI dkcCuckooHashCount(const DKC_CUCKOO_HASH *hash)
{
	if (!hash) return 0;
	return hash->num_items;
}

double WINAPI dkcCuckooHashLoadFactor(const DKC_CUCKOO_HASH *hash)
{
	if (!hash || hash->capacity == 0) return 0.0;
	return (double)hash->num_items / (double)(hash->capacity * 2);
}

/* ====================================================================
 * Cuckoo Filter 実装
 * ==================================================================== */

int WINAPI dkcCuckooFilterCreate(DKC_CUCKOO_FILTER *filter, size_t capacity)
{
	size_t num_buckets;

	if (!filter || capacity == 0) return edk_FAILED;

	/* バケット数を計算（負荷率95%を想定） */
	num_buckets = (capacity + dkcd_CUCKOO_FILTER_BUCKET_SIZE - 1) /
	              dkcd_CUCKOO_FILTER_BUCKET_SIZE;
	num_buckets = (num_buckets * 100) / 95; /* 余裕を持たせる */

	/* 2の冪乗に切り上げ */
	{
		size_t n = 1;
		while (n < num_buckets) n <<= 1;
		num_buckets = n;
	}

	filter->buckets = (DKC_CUCKOO_BUCKET *)calloc(num_buckets, sizeof(DKC_CUCKOO_BUCKET));
	if (!filter->buckets) return edk_FAILED;

	filter->num_buckets = num_buckets;
	filter->num_items = 0;
	filter->capacity = num_buckets * dkcd_CUCKOO_FILTER_BUCKET_SIZE;
	filter->seed = 0x9E3779B9;

	return edk_SUCCEEDED;
}

void WINAPI dkcCuckooFilterFree(DKC_CUCKOO_FILTER *filter)
{
	if (!filter) return;
	if (filter->buckets) {
		free(filter->buckets);
		filter->buckets = NULL;
	}
	filter->num_buckets = 0;
	filter->num_items = 0;
	filter->capacity = 0;
}

/* バケットのインデックス計算 */
static size_t get_bucket_index(const DKC_CUCKOO_FILTER *filter,
	const void *data, size_t len)
{
	uint32 h = murmur3_32(data, len, filter->seed);
	return h & (filter->num_buckets - 1);
}

/* 代替バケットのインデックス計算 */
static size_t get_alt_bucket_index(const DKC_CUCKOO_FILTER *filter,
	size_t index, uint16 fp)
{
	uint32 h = murmur3_32(&fp, sizeof(fp), filter->seed);
	return (index ^ h) & (filter->num_buckets - 1);
}

/* バケットに空きスロットがあるか */
static int bucket_has_empty(const DKC_CUCKOO_BUCKET *bucket)
{
	int i;
	for (i = 0; i < dkcd_CUCKOO_FILTER_BUCKET_SIZE; i++) {
		if (bucket->fingerprints[i] == 0) return 1;
	}
	return 0;
}

/* バケットに挿入 */
static int bucket_insert(DKC_CUCKOO_BUCKET *bucket, uint16 fp)
{
	int i;
	for (i = 0; i < dkcd_CUCKOO_FILTER_BUCKET_SIZE; i++) {
		if (bucket->fingerprints[i] == 0) {
			bucket->fingerprints[i] = fp;
			return 1;
		}
	}
	return 0;
}

/* バケットに含まれるか */
static int bucket_contains(const DKC_CUCKOO_BUCKET *bucket, uint16 fp)
{
	int i;
	for (i = 0; i < dkcd_CUCKOO_FILTER_BUCKET_SIZE; i++) {
		if (bucket->fingerprints[i] == fp) return 1;
	}
	return 0;
}

/* バケットから削除 */
static int bucket_remove(DKC_CUCKOO_BUCKET *bucket, uint16 fp)
{
	int i;
	for (i = 0; i < dkcd_CUCKOO_FILTER_BUCKET_SIZE; i++) {
		if (bucket->fingerprints[i] == fp) {
			bucket->fingerprints[i] = 0;
			return 1;
		}
	}
	return 0;
}

/* ランダムなスロットを選択して追い出し */
static uint16 bucket_swap(DKC_CUCKOO_BUCKET *bucket, uint16 fp, uint32 *rand_state)
{
	int i;
	uint16 old_fp;

	/* 簡易乱数 */
	*rand_state = *rand_state * 1103515245 + 12345;
	i = (*rand_state >> 16) % dkcd_CUCKOO_FILTER_BUCKET_SIZE;

	old_fp = bucket->fingerprints[i];
	bucket->fingerprints[i] = fp;
	return old_fp;
}

int WINAPI dkcCuckooFilterAdd(DKC_CUCKOO_FILTER *filter, const void *data, size_t len)
{
	uint16 fp;
	size_t i1, i2;
	int n;
	uint32 rand_state;

	if (!filter || !filter->buckets || !data) return edk_FAILED;

	fp = fingerprint(data, len, filter->seed + 0x12345678);
	i1 = get_bucket_index(filter, data, len);
	i2 = get_alt_bucket_index(filter, i1, fp);

	/* バケット1に空きがあれば挿入 */
	if (bucket_insert(&filter->buckets[i1], fp)) {
		filter->num_items++;
		return edk_SUCCEEDED;
	}

	/* バケット2に空きがあれば挿入 */
	if (bucket_insert(&filter->buckets[i2], fp)) {
		filter->num_items++;
		return edk_SUCCEEDED;
	}

	/* 追い出しループ */
	rand_state = filter->seed;
	for (n = 0; n < dkcd_CUCKOO_MAX_KICKS; n++) {
		/* ランダムにi1かi2を選択 */
		size_t i = (rand_state & 1) ? i1 : i2;
		rand_state = rand_state * 1103515245 + 12345;

		fp = bucket_swap(&filter->buckets[i], fp, &rand_state);
		i = get_alt_bucket_index(filter, i, fp);

		if (bucket_insert(&filter->buckets[i], fp)) {
			filter->num_items++;
			return edk_SUCCEEDED;
		}

		i1 = i;
		i2 = get_alt_bucket_index(filter, i1, fp);
	}

	/* フル */
	return edk_FAILED;
}

int WINAPI dkcCuckooFilterAddString(DKC_CUCKOO_FILTER *filter, const char *str)
{
	if (!str) return edk_FAILED;
	return dkcCuckooFilterAdd(filter, str, strlen(str));
}

BOOL WINAPI dkcCuckooFilterContains(const DKC_CUCKOO_FILTER *filter,
	const void *data, size_t len)
{
	uint16 fp;
	size_t i1, i2;

	if (!filter || !filter->buckets || !data) return FALSE;

	fp = fingerprint(data, len, filter->seed + 0x12345678);
	i1 = get_bucket_index(filter, data, len);
	i2 = get_alt_bucket_index(filter, i1, fp);

	return bucket_contains(&filter->buckets[i1], fp) ||
	       bucket_contains(&filter->buckets[i2], fp);
}

BOOL WINAPI dkcCuckooFilterContainsString(const DKC_CUCKOO_FILTER *filter, const char *str)
{
	if (!str) return FALSE;
	return dkcCuckooFilterContains(filter, str, strlen(str));
}

int WINAPI dkcCuckooFilterRemove(DKC_CUCKOO_FILTER *filter, const void *data, size_t len)
{
	uint16 fp;
	size_t i1, i2;

	if (!filter || !filter->buckets || !data) return edk_FAILED;

	fp = fingerprint(data, len, filter->seed + 0x12345678);
	i1 = get_bucket_index(filter, data, len);
	i2 = get_alt_bucket_index(filter, i1, fp);

	if (bucket_remove(&filter->buckets[i1], fp)) {
		filter->num_items--;
		return edk_SUCCEEDED;
	}

	if (bucket_remove(&filter->buckets[i2], fp)) {
		filter->num_items--;
		return edk_SUCCEEDED;
	}

	return edk_FAILED;
}

int WINAPI dkcCuckooFilterRemoveString(DKC_CUCKOO_FILTER *filter, const char *str)
{
	if (!str) return edk_FAILED;
	return dkcCuckooFilterRemove(filter, str, strlen(str));
}

void WINAPI dkcCuckooFilterClear(DKC_CUCKOO_FILTER *filter)
{
	if (!filter || !filter->buckets) return;
	memset(filter->buckets, 0, filter->num_buckets * sizeof(DKC_CUCKOO_BUCKET));
	filter->num_items = 0;
}

size_t WINAPI dkcCuckooFilterCount(const DKC_CUCKOO_FILTER *filter)
{
	if (!filter) return 0;
	return filter->num_items;
}

double WINAPI dkcCuckooFilterLoadFactor(const DKC_CUCKOO_FILTER *filter)
{
	if (!filter || filter->capacity == 0) return 0.0;
	return (double)filter->num_items / (double)filter->capacity;
}

size_t WINAPI dkcCuckooFilterMemoryUsage(const DKC_CUCKOO_FILTER *filter)
{
	if (!filter) return 0;
	return sizeof(DKC_CUCKOO_FILTER) +
	       filter->num_buckets * sizeof(DKC_CUCKOO_BUCKET);
}
