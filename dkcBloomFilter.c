/*! @file
	@brief Bloom Filter（ブルームフィルタ）実装
	@author d.Kingyo
	@note
	Double Hashing技法を使用:
	hash_i(x) = hash1(x) + i * hash2(x)
	これにより2つのハッシュ関数でk個のハッシュ値を生成
*/

#include "dkcBloomFilter.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ====================================================================
 * 内部定数
 * ==================================================================== */

#define BLOOM_MAGIC     0x424C4F4D  /* "BLOM" */
#define BLOOM_VERSION   1

/* ====================================================================
 * シリアライズヘッダ
 * ==================================================================== */

typedef struct {
	uint32 magic;
	uint32 version;
	uint64 num_bits;
	uint32 num_hashes;
	uint64 num_items;
	uint64 capacity;
	uint32 seed1;
	uint32 seed2;
} BloomSerializeHeader;

/* ====================================================================
 * 内部ハッシュ関数（MurmurHash3ベース）
 * ==================================================================== */

static uint32 rotl32(uint32 x, int r)
{
	return (x << r) | (x >> (32 - r));
}

static uint32 fmix32(uint32 h)
{
	h ^= h >> 16;
	h *= 0x85EBCA6B;
	h ^= h >> 13;
	h *= 0xC2B2AE35;
	h ^= h >> 16;
	return h;
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
	h1 = fmix32(h1);
	return h1;
}

/* ====================================================================
 * ビット操作
 * ==================================================================== */

static void set_bit(uint8 *bits, size_t index)
{
	bits[index >> 3] |= (1 << (index & 7));
}

static int get_bit(const uint8 *bits, size_t index)
{
	return (bits[index >> 3] >> (index & 7)) & 1;
}

/* ====================================================================
 * ユーティリティ関数
 * ==================================================================== */

size_t WINAPI dkcBloomOptimalBits(size_t n, double fpr)
{
	/* m = -n * ln(fpr) / (ln(2)^2) */
	if (n == 0 || fpr <= 0.0 || fpr >= 1.0) return 0;
	return (size_t)ceil(-((double)n * log(fpr)) / (log(2.0) * log(2.0)));
}

uint32 WINAPI dkcBloomOptimalHashes(size_t m, size_t n)
{
	/* k = (m/n) * ln(2) */
	uint32 k;
	if (n == 0) return 1;
	k = (uint32)ceil(((double)m / (double)n) * log(2.0));
	if (k < 1) k = 1;
	if (k > dkcd_BLOOM_MAX_HASHES) k = dkcd_BLOOM_MAX_HASHES;
	return k;
}

/* ====================================================================
 * Bloom Filter 基本関数
 * ==================================================================== */

int WINAPI dkcBloomCreate(DKC_BLOOM *bloom, size_t capacity, double fpr)
{
	size_t num_bits;
	uint32 num_hashes;

	if (!bloom || capacity == 0) return edk_FAILED;
	if (fpr <= 0.0 || fpr >= 1.0) fpr = dkcd_BLOOM_DEFAULT_FPR;

	num_bits = dkcBloomOptimalBits(capacity, fpr);
	num_hashes = dkcBloomOptimalHashes(num_bits, capacity);

	return dkcBloomCreateDirect(bloom, num_bits, num_hashes);
}

int WINAPI dkcBloomCreateDirect(DKC_BLOOM *bloom, size_t num_bits, uint32 num_hashes)
{
	if (!bloom || num_bits == 0) return edk_FAILED;
	if (num_hashes < 1) num_hashes = 1;
	if (num_hashes > dkcd_BLOOM_MAX_HASHES) num_hashes = dkcd_BLOOM_MAX_HASHES;

	/* 8の倍数に切り上げ */
	num_bits = (num_bits + 7) & ~(size_t)7;

	bloom->num_bits = num_bits;
	bloom->num_bytes = num_bits / 8;
	bloom->num_hashes = num_hashes;
	bloom->num_items = 0;
	bloom->capacity = (size_t)(num_bits / (num_hashes * log(2.0)));
	bloom->seed1 = 0x9E3779B9;
	bloom->seed2 = 0x85EBCA6B;

	bloom->bits = (uint8 *)calloc(bloom->num_bytes, 1);
	if (!bloom->bits) return edk_FAILED;

	return edk_SUCCEEDED;
}

void WINAPI dkcBloomFree(DKC_BLOOM *bloom)
{
	if (!bloom) return;
	if (bloom->bits) {
		free(bloom->bits);
		bloom->bits = NULL;
	}
	bloom->num_bits = 0;
	bloom->num_bytes = 0;
	bloom->num_items = 0;
}

int WINAPI dkcBloomAdd(DKC_BLOOM *bloom, const void *data, size_t len)
{
	uint32 h1, h2;
	uint32 i;

	if (!bloom || !bloom->bits || !data) return edk_FAILED;

	h1 = murmur3_32(data, len, bloom->seed1);
	h2 = murmur3_32(data, len, bloom->seed2);

	for (i = 0; i < bloom->num_hashes; i++) {
		size_t index = (h1 + i * h2) % bloom->num_bits;
		set_bit(bloom->bits, index);
	}

	bloom->num_items++;
	return edk_SUCCEEDED;
}

int WINAPI dkcBloomAddString(DKC_BLOOM *bloom, const char *str)
{
	if (!str) return edk_FAILED;
	return dkcBloomAdd(bloom, str, strlen(str));
}

BOOL WINAPI dkcBloomContains(const DKC_BLOOM *bloom, const void *data, size_t len)
{
	uint32 h1, h2;
	uint32 i;

	if (!bloom || !bloom->bits || !data) return FALSE;

	h1 = murmur3_32(data, len, bloom->seed1);
	h2 = murmur3_32(data, len, bloom->seed2);

	for (i = 0; i < bloom->num_hashes; i++) {
		size_t index = (h1 + i * h2) % bloom->num_bits;
		if (!get_bit(bloom->bits, index)) {
			return FALSE;
		}
	}

	return TRUE;
}

BOOL WINAPI dkcBloomContainsString(const DKC_BLOOM *bloom, const char *str)
{
	if (!str) return FALSE;
	return dkcBloomContains(bloom, str, strlen(str));
}

void WINAPI dkcBloomClear(DKC_BLOOM *bloom)
{
	if (!bloom || !bloom->bits) return;
	memset(bloom->bits, 0, bloom->num_bytes);
	bloom->num_items = 0;
}

int WINAPI dkcBloomMerge(DKC_BLOOM *dest, const DKC_BLOOM *src)
{
	size_t i;

	if (!dest || !src) return edk_FAILED;
	if (!dest->bits || !src->bits) return edk_FAILED;
	if (dest->num_bits != src->num_bits) return edk_FAILED;
	if (dest->num_hashes != src->num_hashes) return edk_FAILED;

	for (i = 0; i < dest->num_bytes; i++) {
		dest->bits[i] |= src->bits[i];
	}

	dest->num_items += src->num_items;
	return edk_SUCCEEDED;
}

/* ====================================================================
 * Bloom Filter 情報取得
 * ==================================================================== */

double WINAPI dkcBloomGetFPR(const DKC_BLOOM *bloom)
{
	/* FPR = (1 - e^(-kn/m))^k */
	double exp_val;
	if (!bloom || bloom->num_bits == 0) return 1.0;

	exp_val = exp(-(double)(bloom->num_hashes * bloom->num_items) /
	              (double)bloom->num_bits);
	return pow(1.0 - exp_val, (double)bloom->num_hashes);
}

double WINAPI dkcBloomGetFillRatio(const DKC_BLOOM *bloom)
{
	size_t count = 0;
	size_t i;

	if (!bloom || !bloom->bits || bloom->num_bits == 0) return 0.0;

	for (i = 0; i < bloom->num_bits; i++) {
		if (get_bit(bloom->bits, i)) count++;
	}

	return (double)count / (double)bloom->num_bits;
}

size_t WINAPI dkcBloomMemoryUsage(const DKC_BLOOM *bloom)
{
	if (!bloom) return 0;
	return sizeof(DKC_BLOOM) + bloom->num_bytes;
}

/* ====================================================================
 * Bloom Filter シリアライズ
 * ==================================================================== */

size_t WINAPI dkcBloomSerializedSize(const DKC_BLOOM *bloom)
{
	if (!bloom) return 0;
	return sizeof(BloomSerializeHeader) + bloom->num_bytes;
}

int WINAPI dkcBloomSerialize(const DKC_BLOOM *bloom, void *buffer, size_t buffer_size)
{
	BloomSerializeHeader *header;
	uint8 *data;

	if (!bloom || !buffer) return edk_FAILED;
	if (buffer_size < dkcBloomSerializedSize(bloom)) return edk_FAILED;

	header = (BloomSerializeHeader *)buffer;
	header->magic = BLOOM_MAGIC;
	header->version = BLOOM_VERSION;
	header->num_bits = (uint64)bloom->num_bits;
	header->num_hashes = bloom->num_hashes;
	header->num_items = (uint64)bloom->num_items;
	header->capacity = (uint64)bloom->capacity;
	header->seed1 = bloom->seed1;
	header->seed2 = bloom->seed2;

	data = (uint8 *)buffer + sizeof(BloomSerializeHeader);
	memcpy(data, bloom->bits, bloom->num_bytes);

	return edk_SUCCEEDED;
}

int WINAPI dkcBloomDeserialize(DKC_BLOOM *bloom, const void *buffer, size_t buffer_size)
{
	const BloomSerializeHeader *header;
	const uint8 *data;
	size_t expected_size;

	if (!bloom || !buffer) return edk_FAILED;
	if (buffer_size < sizeof(BloomSerializeHeader)) return edk_FAILED;

	header = (const BloomSerializeHeader *)buffer;

	if (header->magic != BLOOM_MAGIC) return edk_FAILED;
	if (header->version != BLOOM_VERSION) return edk_FAILED;

	expected_size = sizeof(BloomSerializeHeader) + (size_t)(header->num_bits / 8);
	if (buffer_size < expected_size) return edk_FAILED;

	/* 既存データを解放 */
	dkcBloomFree(bloom);

	bloom->num_bits = (size_t)header->num_bits;
	bloom->num_bytes = bloom->num_bits / 8;
	bloom->num_hashes = header->num_hashes;
	bloom->num_items = (size_t)header->num_items;
	bloom->capacity = (size_t)header->capacity;
	bloom->seed1 = header->seed1;
	bloom->seed2 = header->seed2;

	bloom->bits = (uint8 *)malloc(bloom->num_bytes);
	if (!bloom->bits) return edk_FAILED;

	data = (const uint8 *)buffer + sizeof(BloomSerializeHeader);
	memcpy(bloom->bits, data, bloom->num_bytes);

	return edk_SUCCEEDED;
}

/* ====================================================================
 * Counting Bloom Filter
 * ==================================================================== */

/* 4bitカウンタ操作 */
static uint8 get_counter(const uint8 *counters, size_t index)
{
	size_t byte_idx = index / 2;
	if (index & 1) {
		return (counters[byte_idx] >> 4) & 0x0F;
	} else {
		return counters[byte_idx] & 0x0F;
	}
}

static void set_counter(uint8 *counters, size_t index, uint8 value)
{
	size_t byte_idx = index / 2;
	if (value > 15) value = 15; /* 4bit最大値 */
	if (index & 1) {
		counters[byte_idx] = (counters[byte_idx] & 0x0F) | (value << 4);
	} else {
		counters[byte_idx] = (counters[byte_idx] & 0xF0) | value;
	}
}

static void inc_counter(uint8 *counters, size_t index)
{
	uint8 val = get_counter(counters, index);
	if (val < 15) {
		set_counter(counters, index, val + 1);
	}
}

static int dec_counter(uint8 *counters, size_t index)
{
	uint8 val = get_counter(counters, index);
	if (val > 0) {
		set_counter(counters, index, val - 1);
		return 1;
	}
	return 0;
}

int WINAPI dkcCountingBloomCreate(DKC_COUNTING_BLOOM *bloom, size_t capacity, double fpr)
{
	size_t num_counters;
	uint32 num_hashes;

	if (!bloom || capacity == 0) return edk_FAILED;
	if (fpr <= 0.0 || fpr >= 1.0) fpr = dkcd_BLOOM_DEFAULT_FPR;

	num_counters = dkcBloomOptimalBits(capacity, fpr);
	num_hashes = dkcBloomOptimalHashes(num_counters, capacity);

	/* 8の倍数に切り上げ（2カウンタ/バイト） */
	num_counters = (num_counters + 7) & ~(size_t)7;

	bloom->num_counters = num_counters;
	bloom->num_bytes = (num_counters + 1) / 2; /* 4bit per counter */
	bloom->num_hashes = num_hashes;
	bloom->num_items = 0;
	bloom->capacity = capacity;
	bloom->seed1 = 0x9E3779B9;
	bloom->seed2 = 0x85EBCA6B;

	bloom->counters = (uint8 *)calloc(bloom->num_bytes, 1);
	if (!bloom->counters) return edk_FAILED;

	return edk_SUCCEEDED;
}

void WINAPI dkcCountingBloomFree(DKC_COUNTING_BLOOM *bloom)
{
	if (!bloom) return;
	if (bloom->counters) {
		free(bloom->counters);
		bloom->counters = NULL;
	}
	bloom->num_counters = 0;
	bloom->num_bytes = 0;
	bloom->num_items = 0;
}

int WINAPI dkcCountingBloomAdd(DKC_COUNTING_BLOOM *bloom, const void *data, size_t len)
{
	uint32 h1, h2;
	uint32 i;

	if (!bloom || !bloom->counters || !data) return edk_FAILED;

	h1 = murmur3_32(data, len, bloom->seed1);
	h2 = murmur3_32(data, len, bloom->seed2);

	for (i = 0; i < bloom->num_hashes; i++) {
		size_t index = (h1 + i * h2) % bloom->num_counters;
		inc_counter(bloom->counters, index);
	}

	bloom->num_items++;
	return edk_SUCCEEDED;
}

int WINAPI dkcCountingBloomRemove(DKC_COUNTING_BLOOM *bloom, const void *data, size_t len)
{
	uint32 h1, h2;
	uint32 i;

	if (!bloom || !bloom->counters || !data) return edk_FAILED;

	/* まず存在確認 */
	if (!dkcCountingBloomContains(bloom, data, len)) {
		return edk_FAILED;
	}

	h1 = murmur3_32(data, len, bloom->seed1);
	h2 = murmur3_32(data, len, bloom->seed2);

	for (i = 0; i < bloom->num_hashes; i++) {
		size_t index = (h1 + i * h2) % bloom->num_counters;
		dec_counter(bloom->counters, index);
	}

	if (bloom->num_items > 0) bloom->num_items--;
	return edk_SUCCEEDED;
}

BOOL WINAPI dkcCountingBloomContains(const DKC_COUNTING_BLOOM *bloom,
	const void *data, size_t len)
{
	uint32 h1, h2;
	uint32 i;

	if (!bloom || !bloom->counters || !data) return FALSE;

	h1 = murmur3_32(data, len, bloom->seed1);
	h2 = murmur3_32(data, len, bloom->seed2);

	for (i = 0; i < bloom->num_hashes; i++) {
		size_t index = (h1 + i * h2) % bloom->num_counters;
		if (get_counter(bloom->counters, index) == 0) {
			return FALSE;
		}
	}

	return TRUE;
}

void WINAPI dkcCountingBloomClear(DKC_COUNTING_BLOOM *bloom)
{
	if (!bloom || !bloom->counters) return;
	memset(bloom->counters, 0, bloom->num_bytes);
	bloom->num_items = 0;
}
