/*! @file
	@brief 最小完全ハッシュ（Minimal Perfect Hash）実装
	@author d.Kingyo
	@note
	CHD (Compress, Hash, Displace) アルゴリズムによる実装

	アルゴリズム概要:
	1. 2つの独立したハッシュ関数 h1, h2 を使用
	2. h1(key) でキーをバケットに割り当て
	3. バケットをサイズ降順でソート（大きいものから処理）
	4. 各バケットに対して変位値 d を探索:
	   - バケット内の全キー k について (h2(k) + d) mod n が衝突しないように
	5. 変位値を配列 g に格納

	参考文献:
	- Belazzougui, Botelho, Dietzfelbinger (2009)
	  "Hash, Displace, and Compress"
*/

#include "dkcMPH.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 内部定数
 * ==================================================================== */

#define MPH_MAGIC           0x4D504843  /* "MPHC" */
#define MPH_VERSION         1
#define MPH_MAX_TRIES       100         /* 変位値探索の最大試行回数 */
#define MPH_SEED_INCREMENT  0x9E3779B9  /* 黄金比由来の定数 */

/* ====================================================================
 * 内部構造体
 * ==================================================================== */

/* バケット情報 */
typedef struct {
	size_t bucket_id;       /* バケットID */
	size_t *key_indices;    /* キーインデックス配列 */
	size_t num_keys;        /* バケット内キー数 */
	size_t capacity;        /* 配列容量 */
} MPH_Bucket;

/* シリアライズヘッダ */
typedef struct {
	uint32 magic;
	uint32 version;
	uint32 num_keys;
	uint32 g_size;
	uint32 seed1;
	uint32 seed2;
	int32 lambda;
	uint32 reserved;
} MPH_SerializeHeader;

/* ====================================================================
 * ハッシュ関数（MurmurHash3 ベース）
 * ==================================================================== */

static uint32 mph_rotl32(uint32 x, int r)
{
	return (x << r) | (x >> (32 - r));
}

static uint32 mph_fmix32(uint32 h)
{
	h ^= h >> 16;
	h *= 0x85EBCA6B;
	h ^= h >> 13;
	h *= 0xC2B2AE35;
	h ^= h >> 16;
	return h;
}

/*! @brief MurmurHash3風ハッシュ関数 */
static uint32 mph_hash(const void *key, size_t len, uint32 seed)
{
	const uint8 *data = (const uint8 *)key;
	const size_t nblocks = len / 4;
	uint32 h1 = seed;
	const uint32 c1 = 0xCC9E2D51;
	const uint32 c2 = 0x1B873593;
	const uint32 *blocks;
	const uint8 *tail;
	uint32 k1;
	size_t i;

	/* body */
	blocks = (const uint32 *)(data + nblocks * 4);
	for (i = 0; i < nblocks; i++) {
		/* 非アラインメント対応の読み込み */
		const uint8 *p = (const uint8 *)(blocks - nblocks + i);
		k1 = (uint32)p[0] | ((uint32)p[1] << 8) |
			 ((uint32)p[2] << 16) | ((uint32)p[3] << 24);

		k1 *= c1;
		k1 = mph_rotl32(k1, 15);
		k1 *= c2;

		h1 ^= k1;
		h1 = mph_rotl32(h1, 13);
		h1 = h1 * 5 + 0xE6546B64;
	}

	/* tail */
	tail = data + nblocks * 4;
	k1 = 0;
	switch (len & 3) {
	case 3: k1 ^= (uint32)tail[2] << 16; /* fall through */
	case 2: k1 ^= (uint32)tail[1] << 8;  /* fall through */
	case 1: k1 ^= (uint32)tail[0];
		k1 *= c1;
		k1 = mph_rotl32(k1, 15);
		k1 *= c2;
		h1 ^= k1;
	}

	/* finalization */
	h1 ^= (uint32)len;
	h1 = mph_fmix32(h1);
	return h1;
}

/* ====================================================================
 * 内部ユーティリティ
 * ==================================================================== */

/* バケットサイズ比較（降順） */
static int mph_bucket_compare(const void *a, const void *b)
{
	const MPH_Bucket *ba = (const MPH_Bucket *)a;
	const MPH_Bucket *bb = (const MPH_Bucket *)b;
	if (ba->num_keys > bb->num_keys) return -1;
	if (ba->num_keys < bb->num_keys) return 1;
	return 0;
}

/* バケットにキーを追加 */
static int mph_bucket_add(MPH_Bucket *bucket, size_t key_index)
{
	if (bucket->num_keys >= bucket->capacity) {
		size_t new_capacity = bucket->capacity == 0 ? 4 : bucket->capacity * 2;
		size_t *new_indices = (size_t *)realloc(bucket->key_indices,
			new_capacity * sizeof(size_t));
		if (!new_indices) return edk_FAILED;
		bucket->key_indices = new_indices;
		bucket->capacity = new_capacity;
	}
	bucket->key_indices[bucket->num_keys++] = key_index;
	return edk_SUCCEEDED;
}

/* バケット配列を解放 */
static void mph_free_buckets(MPH_Bucket *buckets, size_t num_buckets)
{
	size_t i;
	if (!buckets) return;
	for (i = 0; i < num_buckets; i++) {
		if (buckets[i].key_indices) {
			free(buckets[i].key_indices);
		}
	}
	free(buckets);
}

/* ====================================================================
 * 基本関数
 * ==================================================================== */

int WINAPI dkcMPHInit(DKC_MPH *mph)
{
	if (!mph) return edk_FAILED;

	mph->g = NULL;
	mph->g_size = 0;
	mph->num_keys = 0;
	mph->seed1 = 0;
	mph->seed2 = 0;
	mph->lambda = dkcd_MPH_DEFAULT_LAMBDA;

	return edk_SUCCEEDED;
}

void WINAPI dkcMPHFree(DKC_MPH *mph)
{
	if (!mph) return;

	if (mph->g) {
		free(mph->g);
		mph->g = NULL;
	}
	mph->g_size = 0;
	mph->num_keys = 0;
}

/* ====================================================================
 * 構築関数（CHDアルゴリズム）
 * ==================================================================== */

/*!
	@brief CHDアルゴリズムでMPHを構築（内部関数）
	@note
	1. バケット数 = ceil(n / lambda)
	2. 各キーをh1でバケットに割り当て
	3. バケットをサイズ降順でソート
	4. 各バケットに変位値を割り当て
*/
static int mph_build_internal(DKC_MPH *mph, dkcMPHKeyFunc key_func,
	void *context, size_t num_keys)
{
	MPH_Bucket *buckets = NULL;
	size_t num_buckets;
	uint8 *occupied = NULL;     /* スロット占有フラグ */
	uint32 *h2_values = NULL;   /* h2ハッシュ値キャッシュ */
	size_t i, j, try_count;
	uint32 seed1, seed2;
	int success = 0;

	if (num_keys == 0 || num_keys > dkcd_MPH_MAX_KEYS) {
		return edk_FAILED;
	}

	/* バケット数を計算 */
	num_buckets = (num_keys + mph->lambda - 1) / mph->lambda;
	if (num_buckets == 0) num_buckets = 1;

	/* メモリ確保 */
	buckets = (MPH_Bucket *)calloc(num_buckets, sizeof(MPH_Bucket));
	occupied = (uint8 *)calloc(num_keys, sizeof(uint8));
	h2_values = (uint32 *)malloc(num_keys * sizeof(uint32));
	mph->g = (uint32 *)calloc(num_buckets, sizeof(uint32));

	if (!buckets || !occupied || !h2_values || !mph->g) {
		goto cleanup;
	}

	/* バケットIDを初期化 */
	for (i = 0; i < num_buckets; i++) {
		buckets[i].bucket_id = i;
	}

	/* シード探索ループ */
	for (seed1 = 0x12345678, try_count = 0; try_count < MPH_MAX_TRIES; try_count++) {
		const void *key;
		size_t key_len;
		int build_failed = 0;

		seed2 = seed1 + MPH_SEED_INCREMENT;

		/* バケットをリセット */
		for (i = 0; i < num_buckets; i++) {
			buckets[i].num_keys = 0;
		}
		memset(occupied, 0, num_keys);

		/* 各キーをバケットに割り当て、h2値を計算 */
		for (i = 0; i < num_keys; i++) {
			uint32 h1, bucket_idx;

			if (key_func(context, i, &key, &key_len) != edk_SUCCEEDED) {
				goto cleanup;
			}

			h1 = mph_hash(key, key_len, seed1);
			bucket_idx = h1 % num_buckets;
			h2_values[i] = mph_hash(key, key_len, seed2);

			if (mph_bucket_add(&buckets[bucket_idx], i) != edk_SUCCEEDED) {
				goto cleanup;
			}
		}

		/* バケットをサイズ降順でソート */
		qsort(buckets, num_buckets, sizeof(MPH_Bucket), mph_bucket_compare);

		/* 各バケットに変位値を割り当て */
		for (i = 0; i < num_buckets; i++) {
			MPH_Bucket *bucket = &buckets[i];
			uint32 d;
			int found = 0;

			if (bucket->num_keys == 0) {
				mph->g[bucket->bucket_id] = 0;
				continue;
			}

			/* 変位値を探索 */
			for (d = 0; d < num_keys * 2; d++) {
				int collision = 0;

				/* このdで衝突が起きるか確認 */
				for (j = 0; j < bucket->num_keys; j++) {
					size_t key_idx = bucket->key_indices[j];
					size_t slot = (h2_values[key_idx] + d) % num_keys;
					if (occupied[slot]) {
						collision = 1;
						break;
					}
				}

				if (!collision) {
					/* このdで成功、スロットを占有 */
					for (j = 0; j < bucket->num_keys; j++) {
						size_t key_idx = bucket->key_indices[j];
						size_t slot = (h2_values[key_idx] + d) % num_keys;
						occupied[slot] = 1;
					}
					mph->g[bucket->bucket_id] = d;
					found = 1;
					break;
				}
			}

			if (!found) {
				build_failed = 1;
				break;
			}
		}

		if (!build_failed) {
			/* 成功 */
			mph->seed1 = seed1;
			mph->seed2 = seed2;
			mph->g_size = num_buckets;
			mph->num_keys = num_keys;
			success = 1;
			break;
		}

		/* 次のシードを試す */
		seed1 += MPH_SEED_INCREMENT;
	}

cleanup:
	mph_free_buckets(buckets, num_buckets);
	if (occupied) free(occupied);
	if (h2_values) free(h2_values);

	if (!success) {
		if (mph->g) {
			free(mph->g);
			mph->g = NULL;
		}
		return edk_FAILED;
	}

	return edk_SUCCEEDED;
}

/* 配列用コールバックコンテキスト */
typedef struct {
	const void **keys;
	const size_t *key_lens;
} MPH_ArrayContext;

static int WINAPI mph_array_key_func(void *context, size_t index,
	const void **key, size_t *key_len)
{
	MPH_ArrayContext *ctx = (MPH_ArrayContext *)context;
	*key = ctx->keys[index];
	*key_len = ctx->key_lens[index];
	return edk_SUCCEEDED;
}

int WINAPI dkcMPHBuild(DKC_MPH *mph, const void **keys,
	const size_t *key_lens, size_t num_keys)
{
	MPH_ArrayContext ctx;

	if (!mph || !keys || !key_lens) return edk_FAILED;

	ctx.keys = keys;
	ctx.key_lens = key_lens;

	return mph_build_internal(mph, mph_array_key_func, &ctx, num_keys);
}

/* 文字列配列用コールバックコンテキスト */
typedef struct {
	const char **strings;
} MPH_StringContext;

static int WINAPI mph_string_key_func(void *context, size_t index,
	const void **key, size_t *key_len)
{
	MPH_StringContext *ctx = (MPH_StringContext *)context;
	*key = ctx->strings[index];
	*key_len = strlen(ctx->strings[index]);
	return edk_SUCCEEDED;
}

int WINAPI dkcMPHBuildStrings(DKC_MPH *mph, const char **strings,
	size_t num_strings)
{
	MPH_StringContext ctx;

	if (!mph || !strings) return edk_FAILED;

	ctx.strings = strings;

	return mph_build_internal(mph, mph_string_key_func, &ctx, num_strings);
}

int WINAPI dkcMPHBuildCallback(DKC_MPH *mph, dkcMPHKeyFunc key_func,
	void *context, size_t num_keys)
{
	if (!mph || !key_func) return edk_FAILED;

	return mph_build_internal(mph, key_func, context, num_keys);
}

/* ====================================================================
 * ルックアップ関数
 * ==================================================================== */

size_t WINAPI dkcMPHLookup(const DKC_MPH *mph, const void *key, size_t key_len)
{
	uint32 h1, h2, bucket_idx, d;

	if (!mph || !mph->g || !key) return 0;

	h1 = mph_hash(key, key_len, mph->seed1);
	h2 = mph_hash(key, key_len, mph->seed2);

	bucket_idx = h1 % mph->g_size;
	d = mph->g[bucket_idx];

	return (h2 + d) % mph->num_keys;
}

size_t WINAPI dkcMPHLookupString(const DKC_MPH *mph, const char *str)
{
	if (!mph || !str) return 0;
	return dkcMPHLookup(mph, str, strlen(str));
}

/* ====================================================================
 * シリアライズ
 * ==================================================================== */

size_t WINAPI dkcMPHSerializedSize(const DKC_MPH *mph)
{
	if (!mph) return 0;
	return sizeof(MPH_SerializeHeader) + mph->g_size * sizeof(uint32);
}

int WINAPI dkcMPHSerialize(const DKC_MPH *mph, void *buffer, size_t buffer_size)
{
	MPH_SerializeHeader *header;
	uint32 *g_data;
	size_t required_size;

	if (!mph || !buffer) return edk_FAILED;

	required_size = dkcMPHSerializedSize(mph);
	if (buffer_size < required_size) return edk_FAILED;

	header = (MPH_SerializeHeader *)buffer;
	header->magic = MPH_MAGIC;
	header->version = MPH_VERSION;
	header->num_keys = (uint32)mph->num_keys;
	header->g_size = (uint32)mph->g_size;
	header->seed1 = mph->seed1;
	header->seed2 = mph->seed2;
	header->lambda = mph->lambda;
	header->reserved = 0;

	g_data = (uint32 *)((uint8 *)buffer + sizeof(MPH_SerializeHeader));
	memcpy(g_data, mph->g, mph->g_size * sizeof(uint32));

	return edk_SUCCEEDED;
}

int WINAPI dkcMPHDeserialize(DKC_MPH *mph, const void *buffer, size_t buffer_size)
{
	const MPH_SerializeHeader *header;
	const uint32 *g_data;
	size_t expected_size;

	if (!mph || !buffer) return edk_FAILED;
	if (buffer_size < sizeof(MPH_SerializeHeader)) return edk_FAILED;

	header = (const MPH_SerializeHeader *)buffer;

	/* マジックナンバーとバージョンを確認 */
	if (header->magic != MPH_MAGIC) return edk_FAILED;
	if (header->version != MPH_VERSION) return edk_FAILED;

	expected_size = sizeof(MPH_SerializeHeader) + header->g_size * sizeof(uint32);
	if (buffer_size < expected_size) return edk_FAILED;

	/* 既存のデータを解放 */
	dkcMPHFree(mph);

	/* メモリ確保とコピー */
	mph->g = (uint32 *)malloc(header->g_size * sizeof(uint32));
	if (!mph->g) return edk_FAILED;

	g_data = (const uint32 *)((const uint8 *)buffer + sizeof(MPH_SerializeHeader));
	memcpy(mph->g, g_data, header->g_size * sizeof(uint32));

	mph->num_keys = header->num_keys;
	mph->g_size = header->g_size;
	mph->seed1 = header->seed1;
	mph->seed2 = header->seed2;
	mph->lambda = header->lambda;

	return edk_SUCCEEDED;
}

/* ====================================================================
 * ユーティリティ
 * ==================================================================== */

size_t WINAPI dkcMPHMemoryUsage(const DKC_MPH *mph)
{
	if (!mph) return 0;
	return sizeof(DKC_MPH) + mph->g_size * sizeof(uint32);
}

double WINAPI dkcMPHBitsPerKey(const DKC_MPH *mph)
{
	if (!mph || mph->num_keys == 0) return 0.0;
	return (double)(mph->g_size * sizeof(uint32) * 8) / (double)mph->num_keys;
}
