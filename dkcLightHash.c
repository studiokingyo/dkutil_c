/*! @file
	@brief 非暗号学的軽量ハッシュ関数実装
	@author d.Kingyo
*/

#include "dkcLightHash.h"
#include <string.h>

/* ====================================================================
 * 内部ユーティリティ
 * ==================================================================== */

/* 回転操作 */
#define ROTL32(x, r) (((x) << (r)) | ((x) >> (32 - (r))))
#define ROTL64(x, r) (((x) << (r)) | ((x) >> (64 - (r))))
#define ROTR32(x, r) (((x) >> (r)) | ((x) << (32 - (r))))
#define ROTR64(x, r) (((x) >> (r)) | ((x) << (64 - (r))))

/* 非アラインメント読み込み */
static uint32 read32_le(const uint8 *p)
{
	return (uint32)p[0] | ((uint32)p[1] << 8) |
	       ((uint32)p[2] << 16) | ((uint32)p[3] << 24);
}

static uint64 read64_le(const uint8 *p)
{
	return (uint64)p[0] | ((uint64)p[1] << 8) |
	       ((uint64)p[2] << 16) | ((uint64)p[3] << 24) |
	       ((uint64)p[4] << 32) | ((uint64)p[5] << 40) |
	       ((uint64)p[6] << 48) | ((uint64)p[7] << 56);
}

static uint16 read16_le(const uint8 *p)
{
	return (uint16)p[0] | ((uint16)p[1] << 8);
}

/* ====================================================================
 * FNV定数
 * ==================================================================== */

#define FNV1_32_INIT  0x811C9DC5UL
#define FNV1_32_PRIME 0x01000193UL
#define FNV1_64_INIT  0xCBF29CE484222325ULL
#define FNV1_64_PRIME 0x00000100000001B3ULL

/* ====================================================================
 * クラシックハッシュ関数
 * ==================================================================== */

uint32 WINAPI dkcHashDJB2(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint32 hash = 5381;
	size_t i;

	for (i = 0; i < len; i++) {
		hash = ((hash << 5) + hash) + p[i]; /* hash * 33 + c */
	}
	return hash;
}

uint32 WINAPI dkcHashDJB2a(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint32 hash = 5381;
	size_t i;

	for (i = 0; i < len; i++) {
		hash = ((hash << 5) + hash) ^ p[i]; /* hash * 33 ^ c */
	}
	return hash;
}

uint32 WINAPI dkcHashSDBM(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint32 hash = 0;
	size_t i;

	for (i = 0; i < len; i++) {
		hash = p[i] + (hash << 6) + (hash << 16) - hash; /* hash * 65599 + c */
	}
	return hash;
}

uint32 WINAPI dkcHashFNV1_32(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint32 hash = FNV1_32_INIT;
	size_t i;

	for (i = 0; i < len; i++) {
		hash *= FNV1_32_PRIME;
		hash ^= p[i];
	}
	return hash;
}

uint32 WINAPI dkcHashFNV1a_32(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint32 hash = FNV1_32_INIT;
	size_t i;

	for (i = 0; i < len; i++) {
		hash ^= p[i];
		hash *= FNV1_32_PRIME;
	}
	return hash;
}

uint64 WINAPI dkcHashFNV1_64(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint64 hash = FNV1_64_INIT;
	size_t i;

	for (i = 0; i < len; i++) {
		hash *= FNV1_64_PRIME;
		hash ^= p[i];
	}
	return hash;
}

uint64 WINAPI dkcHashFNV1a_64(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint64 hash = FNV1_64_INIT;
	size_t i;

	for (i = 0; i < len; i++) {
		hash ^= p[i];
		hash *= FNV1_64_PRIME;
	}
	return hash;
}

uint32 WINAPI dkcHashJenkinsOAAT(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint32 hash = 0;
	size_t i;

	for (i = 0; i < len; i++) {
		hash += p[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

uint32 WINAPI dkcHashELF(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint32 hash = 0;
	uint32 high;
	size_t i;

	for (i = 0; i < len; i++) {
		hash = (hash << 4) + p[i];
		high = hash & 0xF0000000UL;
		if (high) {
			hash ^= high >> 24;
		}
		hash &= ~high;
	}
	return hash;
}

uint32 WINAPI dkcHashPJW(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint32 hash = 0;
	uint32 high;
	size_t i;

	for (i = 0; i < len; i++) {
		hash = (hash << 4) + p[i];
		high = hash & 0xF0000000UL;
		if (high) {
			hash ^= high >> 24;
			hash &= ~high;
		}
	}
	return hash;
}

uint32 WINAPI dkcHashSuperFast(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint32 hash = (uint32)len;
	uint32 tmp;
	size_t rem;

	if (len == 0 || data == NULL) return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	while (len > 0) {
		hash += read16_le(p);
		tmp = (read16_le(p + 2) << 11) ^ hash;
		hash = (hash << 16) ^ tmp;
		p += 4;
		hash += hash >> 11;
		len--;
	}

	/* Handle remaining bytes */
	switch (rem) {
	case 3:
		hash += read16_le(p);
		hash ^= hash << 16;
		hash ^= (int8)p[2] << 18;
		hash += hash >> 11;
		break;
	case 2:
		hash += read16_le(p);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1:
		hash += (int8)*p;
		hash ^= hash << 10;
		hash += hash >> 1;
		break;
	}

	/* Force "avalanching" */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

/* Pearsonハッシュ用テーブル */
static const uint8 pearson_table[256] = {
	98, 6, 85, 150, 36, 23, 112, 164, 135, 207, 169, 5, 26, 64, 165, 219,
	61, 20, 68, 89, 130, 63, 52, 102, 24, 229, 132, 245, 80, 216, 195, 115,
	90, 168, 156, 203, 177, 120, 2, 190, 188, 7, 100, 185, 174, 243, 162, 10,
	237, 18, 253, 225, 8, 208, 172, 244, 255, 126, 101, 79, 145, 235, 228, 121,
	123, 251, 67, 250, 161, 0, 107, 97, 241, 111, 181, 82, 249, 33, 69, 55,
	59, 153, 29, 9, 213, 167, 84, 93, 30, 46, 94, 75, 151, 114, 73, 222,
	197, 96, 210, 45, 16, 227, 248, 202, 51, 152, 252, 125, 81, 206, 215, 186,
	39, 158, 178, 187, 131, 136, 1, 49, 50, 17, 141, 91, 47, 129, 60, 99,
	154, 35, 86, 171, 105, 34, 38, 200, 147, 58, 77, 118, 173, 246, 76, 254,
	133, 232, 196, 144, 198, 124, 53, 4, 108, 74, 223, 234, 134, 230, 157, 139,
	189, 205, 199, 128, 176, 19, 211, 236, 127, 192, 231, 70, 233, 88, 146, 44,
	183, 201, 22, 83, 13, 214, 116, 109, 159, 32, 95, 226, 140, 220, 57, 12,
	221, 31, 209, 182, 143, 92, 149, 184, 148, 62, 113, 65, 37, 27, 106, 166,
	3, 14, 204, 72, 21, 41, 56, 66, 28, 193, 40, 217, 25, 54, 179, 117,
	238, 87, 240, 155, 180, 170, 242, 212, 191, 163, 78, 218, 137, 194, 175, 110,
	43, 119, 224, 71, 122, 142, 42, 160, 104, 48, 247, 103, 15, 11, 138, 239
};

uint8 WINAPI dkcHashPearson(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	uint8 hash = 0;
	size_t i;

	for (i = 0; i < len; i++) {
		hash = pearson_table[hash ^ p[i]];
	}
	return hash;
}

/* ====================================================================
 * MurmurHash2
 * ==================================================================== */

uint32 WINAPI dkcHashMurmur2_32(const void *data, size_t len, uint32 seed)
{
	const uint32 m = 0x5BD1E995;
	const int r = 24;
	const uint8 *p = (const uint8 *)data;
	uint32 h = seed ^ (uint32)len;
	uint32 k;

	while (len >= 4) {
		k = read32_le(p);
		k *= m;
		k ^= k >> r;
		k *= m;
		h *= m;
		h ^= k;
		p += 4;
		len -= 4;
	}

	switch (len) {
	case 3: h ^= (uint32)p[2] << 16; /* fall through */
	case 2: h ^= (uint32)p[1] << 8;  /* fall through */
	case 1: h ^= (uint32)p[0];
		h *= m;
	}

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

uint64 WINAPI dkcHashMurmur2_64(const void *data, size_t len, uint64 seed)
{
	const uint64 m = 0xC6A4A7935BD1E995ULL;
	const int r = 47;
	const uint8 *p = (const uint8 *)data;
	uint64 h = seed ^ (len * m);
	uint64 k;

	while (len >= 8) {
		k = read64_le(p);
		k *= m;
		k ^= k >> r;
		k *= m;
		h ^= k;
		h *= m;
		p += 8;
		len -= 8;
	}

	switch (len) {
	case 7: h ^= (uint64)p[6] << 48; /* fall through */
	case 6: h ^= (uint64)p[5] << 40; /* fall through */
	case 5: h ^= (uint64)p[4] << 32; /* fall through */
	case 4: h ^= (uint64)p[3] << 24; /* fall through */
	case 3: h ^= (uint64)p[2] << 16; /* fall through */
	case 2: h ^= (uint64)p[1] << 8;  /* fall through */
	case 1: h ^= (uint64)p[0];
		h *= m;
	}

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

/* ====================================================================
 * MurmurHash3
 * ==================================================================== */

static uint32 murmur3_fmix32(uint32 h)
{
	h ^= h >> 16;
	h *= 0x85EBCA6B;
	h ^= h >> 13;
	h *= 0xC2B2AE35;
	h ^= h >> 16;
	return h;
}

static uint64 murmur3_fmix64(uint64 k)
{
	k ^= k >> 33;
	k *= 0xFF51AFD7ED558CCDULL;
	k ^= k >> 33;
	k *= 0xC4CEB9FE1A85EC53ULL;
	k ^= k >> 33;
	return k;
}

uint32 WINAPI dkcHashMurmur3_32(const void *data, size_t len, uint32 seed)
{
	const uint8 *p = (const uint8 *)data;
	const size_t nblocks = len / 4;
	uint32 h1 = seed;
	const uint32 c1 = 0xCC9E2D51;
	const uint32 c2 = 0x1B873593;
	const uint8 *tail;
	uint32 k1;
	size_t i;

	/* body */
	for (i = 0; i < nblocks; i++) {
		k1 = read32_le(p + i * 4);
		k1 *= c1;
		k1 = ROTL32(k1, 15);
		k1 *= c2;
		h1 ^= k1;
		h1 = ROTL32(h1, 13);
		h1 = h1 * 5 + 0xE6546B64;
	}

	/* tail */
	tail = p + nblocks * 4;
	k1 = 0;
	switch (len & 3) {
	case 3: k1 ^= (uint32)tail[2] << 16; /* fall through */
	case 2: k1 ^= (uint32)tail[1] << 8;  /* fall through */
	case 1: k1 ^= (uint32)tail[0];
		k1 *= c1;
		k1 = ROTL32(k1, 15);
		k1 *= c2;
		h1 ^= k1;
	}

	/* finalization */
	h1 ^= (uint32)len;
	h1 = murmur3_fmix32(h1);

	return h1;
}

void WINAPI dkcHashMurmur3_128(const void *data, size_t len, uint32 seed, uint64 *out)
{
	const uint8 *p = (const uint8 *)data;
	const size_t nblocks = len / 16;
	uint64 h1 = seed;
	uint64 h2 = seed;
	const uint64 c1 = 0x87C37B91114253D5ULL;
	const uint64 c2 = 0x4CF5AD432745937FULL;
	const uint8 *tail;
	uint64 k1, k2;
	size_t i;

	/* body */
	for (i = 0; i < nblocks; i++) {
		k1 = read64_le(p + i * 16);
		k2 = read64_le(p + i * 16 + 8);

		k1 *= c1;
		k1 = ROTL64(k1, 31);
		k1 *= c2;
		h1 ^= k1;

		h1 = ROTL64(h1, 27);
		h1 += h2;
		h1 = h1 * 5 + 0x52DCE729;

		k2 *= c2;
		k2 = ROTL64(k2, 33);
		k2 *= c1;
		h2 ^= k2;

		h2 = ROTL64(h2, 31);
		h2 += h1;
		h2 = h2 * 5 + 0x38495AB5;
	}

	/* tail */
	tail = p + nblocks * 16;
	k1 = 0;
	k2 = 0;

	switch (len & 15) {
	case 15: k2 ^= (uint64)tail[14] << 48; /* fall through */
	case 14: k2 ^= (uint64)tail[13] << 40; /* fall through */
	case 13: k2 ^= (uint64)tail[12] << 32; /* fall through */
	case 12: k2 ^= (uint64)tail[11] << 24; /* fall through */
	case 11: k2 ^= (uint64)tail[10] << 16; /* fall through */
	case 10: k2 ^= (uint64)tail[9] << 8;   /* fall through */
	case 9:  k2 ^= (uint64)tail[8];
		k2 *= c2;
		k2 = ROTL64(k2, 33);
		k2 *= c1;
		h2 ^= k2;
		/* fall through */
	case 8: k1 ^= (uint64)tail[7] << 56; /* fall through */
	case 7: k1 ^= (uint64)tail[6] << 48; /* fall through */
	case 6: k1 ^= (uint64)tail[5] << 40; /* fall through */
	case 5: k1 ^= (uint64)tail[4] << 32; /* fall through */
	case 4: k1 ^= (uint64)tail[3] << 24; /* fall through */
	case 3: k1 ^= (uint64)tail[2] << 16; /* fall through */
	case 2: k1 ^= (uint64)tail[1] << 8;  /* fall through */
	case 1: k1 ^= (uint64)tail[0];
		k1 *= c1;
		k1 = ROTL64(k1, 31);
		k1 *= c2;
		h1 ^= k1;
	}

	/* finalization */
	h1 ^= (uint64)len;
	h2 ^= (uint64)len;

	h1 += h2;
	h2 += h1;

	h1 = murmur3_fmix64(h1);
	h2 = murmur3_fmix64(h2);

	h1 += h2;
	h2 += h1;

	out[0] = h1;
	out[1] = h2;
}

/* ====================================================================
 * xxHash
 * ==================================================================== */

#define XXH_PRIME32_1 0x9E3779B1U
#define XXH_PRIME32_2 0x85EBCA77U
#define XXH_PRIME32_3 0xC2B2AE3DU
#define XXH_PRIME32_4 0x27D4EB2FU
#define XXH_PRIME32_5 0x165667B1U

#define XXH_PRIME64_1 0x9E3779B185EBCA87ULL
#define XXH_PRIME64_2 0xC2B2AE3D27D4EB4FULL
#define XXH_PRIME64_3 0x165667B19E3779F9ULL
#define XXH_PRIME64_4 0x85EBCA77C2B2AE63ULL
#define XXH_PRIME64_5 0x27D4EB2F165667C5ULL

static uint32 xxh32_round(uint32 acc, uint32 input)
{
	acc += input * XXH_PRIME32_2;
	acc = ROTL32(acc, 13);
	acc *= XXH_PRIME32_1;
	return acc;
}

static uint32 xxh32_avalanche(uint32 h)
{
	h ^= h >> 15;
	h *= XXH_PRIME32_2;
	h ^= h >> 13;
	h *= XXH_PRIME32_3;
	h ^= h >> 16;
	return h;
}

uint32 WINAPI dkcHashXX32(const void *data, size_t len, uint32 seed)
{
	const uint8 *p = (const uint8 *)data;
	const uint8 *end = p + len;
	uint32 h;

	if (len >= 16) {
		const uint8 *limit = end - 16;
		uint32 v1 = seed + XXH_PRIME32_1 + XXH_PRIME32_2;
		uint32 v2 = seed + XXH_PRIME32_2;
		uint32 v3 = seed + 0;
		uint32 v4 = seed - XXH_PRIME32_1;

		do {
			v1 = xxh32_round(v1, read32_le(p)); p += 4;
			v2 = xxh32_round(v2, read32_le(p)); p += 4;
			v3 = xxh32_round(v3, read32_le(p)); p += 4;
			v4 = xxh32_round(v4, read32_le(p)); p += 4;
		} while (p <= limit);

		h = ROTL32(v1, 1) + ROTL32(v2, 7) + ROTL32(v3, 12) + ROTL32(v4, 18);
	} else {
		h = seed + XXH_PRIME32_5;
	}

	h += (uint32)len;

	/* Process remaining bytes */
	while (p + 4 <= end) {
		h += read32_le(p) * XXH_PRIME32_3;
		h = ROTL32(h, 17) * XXH_PRIME32_4;
		p += 4;
	}

	while (p < end) {
		h += (*p++) * XXH_PRIME32_5;
		h = ROTL32(h, 11) * XXH_PRIME32_1;
	}

	return xxh32_avalanche(h);
}

static uint64 xxh64_round(uint64 acc, uint64 input)
{
	acc += input * XXH_PRIME64_2;
	acc = ROTL64(acc, 31);
	acc *= XXH_PRIME64_1;
	return acc;
}

static uint64 xxh64_mergeRound(uint64 acc, uint64 val)
{
	val = xxh64_round(0, val);
	acc ^= val;
	acc = acc * XXH_PRIME64_1 + XXH_PRIME64_4;
	return acc;
}

static uint64 xxh64_avalanche(uint64 h)
{
	h ^= h >> 33;
	h *= XXH_PRIME64_2;
	h ^= h >> 29;
	h *= XXH_PRIME64_3;
	h ^= h >> 32;
	return h;
}

uint64 WINAPI dkcHashXX64(const void *data, size_t len, uint64 seed)
{
	const uint8 *p = (const uint8 *)data;
	const uint8 *end = p + len;
	uint64 h;

	if (len >= 32) {
		const uint8 *limit = end - 32;
		uint64 v1 = seed + XXH_PRIME64_1 + XXH_PRIME64_2;
		uint64 v2 = seed + XXH_PRIME64_2;
		uint64 v3 = seed + 0;
		uint64 v4 = seed - XXH_PRIME64_1;

		do {
			v1 = xxh64_round(v1, read64_le(p)); p += 8;
			v2 = xxh64_round(v2, read64_le(p)); p += 8;
			v3 = xxh64_round(v3, read64_le(p)); p += 8;
			v4 = xxh64_round(v4, read64_le(p)); p += 8;
		} while (p <= limit);

		h = ROTL64(v1, 1) + ROTL64(v2, 7) + ROTL64(v3, 12) + ROTL64(v4, 18);
		h = xxh64_mergeRound(h, v1);
		h = xxh64_mergeRound(h, v2);
		h = xxh64_mergeRound(h, v3);
		h = xxh64_mergeRound(h, v4);
	} else {
		h = seed + XXH_PRIME64_5;
	}

	h += (uint64)len;

	/* Process remaining bytes */
	while (p + 8 <= end) {
		uint64 k1 = xxh64_round(0, read64_le(p));
		h ^= k1;
		h = ROTL64(h, 27) * XXH_PRIME64_1 + XXH_PRIME64_4;
		p += 8;
	}

	if (p + 4 <= end) {
		h ^= (uint64)read32_le(p) * XXH_PRIME64_1;
		h = ROTL64(h, 23) * XXH_PRIME64_2 + XXH_PRIME64_3;
		p += 4;
	}

	while (p < end) {
		h ^= (*p++) * XXH_PRIME64_5;
		h = ROTL64(h, 11) * XXH_PRIME64_1;
	}

	return xxh64_avalanche(h);
}

/* ====================================================================
 * CityHash64
 * ==================================================================== */

/* CityHash内部関数 */
static uint64 city_hash128to64(uint64 u, uint64 v)
{
	const uint64 kMul = 0x9DDFEA08EB382D69ULL;
	uint64 a = (u ^ v) * kMul;
	a ^= (a >> 47);
	uint64 b = (v ^ a) * kMul;
	b ^= (b >> 47);
	b *= kMul;
	return b;
}

static uint64 city_shiftmix(uint64 val)
{
	return val ^ (val >> 47);
}

static uint64 city_hashlen16(uint64 u, uint64 v)
{
	return city_hash128to64(u, v);
}

static uint64 city_hashlen16_mul(uint64 u, uint64 v, uint64 mul)
{
	uint64 a = (u ^ v) * mul;
	a ^= (a >> 47);
	uint64 b = (v ^ a) * mul;
	b ^= (b >> 47);
	b *= mul;
	return b;
}

static uint64 city_hashlen0to16(const uint8 *s, size_t len)
{
	const uint64 k2 = 0x9AE16A3B2F90404FULL;
	const uint64 k3 = 0xC949D7C7509E6557ULL;

	if (len > 8) {
		uint64 a = read64_le(s);
		uint64 b = read64_le(s + len - 8);
		return city_hashlen16(a, ROTR64(b + len, (int)len)) ^ b;
	}
	if (len >= 4) {
		uint64 a = read32_le(s);
		return city_hashlen16(len + (a << 3), read32_le(s + len - 4));
	}
	if (len > 0) {
		uint8 a = s[0];
		uint8 b = s[len >> 1];
		uint8 c = s[len - 1];
		uint32 y = (uint32)a + ((uint32)b << 8);
		uint32 z = (uint32)len + ((uint32)c << 2);
		return city_shiftmix(y * k2 ^ z * k3) * k2;
	}
	return k2;
}

static uint64 city_hashlen17to32(const uint8 *s, size_t len)
{
	const uint64 k0 = 0xC3A5C85C97CB3127ULL;
	const uint64 k1 = 0xB492B66FBE98F273ULL;
	const uint64 k2 = 0x9AE16A3B2F90404FULL;

	uint64 a = read64_le(s) * k1;
	uint64 b = read64_le(s + 8);
	uint64 c = read64_le(s + len - 8) * k2;
	uint64 d = read64_le(s + len - 16) * k0;
	return city_hashlen16(ROTR64(a - b, 43) + ROTR64(c, 30) + d,
	                      a + ROTR64(b ^ k3, 20) - c + len);
}

static const uint64 k3 = 0xC949D7C7509E6557ULL;

static uint64 city_hashlen33to64(const uint8 *s, size_t len)
{
	const uint64 k0 = 0xC3A5C85C97CB3127ULL;
	const uint64 k1 = 0xB492B66FBE98F273ULL;
	const uint64 k2 = 0x9AE16A3B2F90404FULL;

	uint64 z = read64_le(s + 24);
	uint64 a = read64_le(s) + (len + read64_le(s + len - 16)) * k0;
	uint64 b = ROTR64(a + z, 52);
	uint64 c = ROTR64(a, 37);
	uint64 vf, vs, wf, ws;

	a += read64_le(s + 8);
	c += ROTR64(a, 7);
	a += read64_le(s + 16);

	vf = a + z;
	vs = b + ROTR64(a, 31) + c;

	a = read64_le(s + 16) + read64_le(s + len - 32);
	z = read64_le(s + len - 8);
	b = ROTR64(a + z, 52);
	c = ROTR64(a, 37);
	a += read64_le(s + len - 24);
	c += ROTR64(a, 7);
	a += read64_le(s + len - 16);

	wf = a + z;
	ws = b + ROTR64(a, 31) + c;

	uint64 r = city_shiftmix((vf + ws) * k2 + (wf + vs) * k0);
	return city_shiftmix(r * k0 + vs) * k2;
}

uint64 WINAPI dkcHashCity64(const void *data, size_t len)
{
	const uint8 *s = (const uint8 *)data;
	const uint64 k0 = 0xC3A5C85C97CB3127ULL;
	const uint64 k1 = 0xB492B66FBE98F273ULL;
	const uint64 k2 = 0x9AE16A3B2F90404FULL;

	if (len <= 16) return city_hashlen0to16(s, len);
	if (len <= 32) return city_hashlen17to32(s, len);
	if (len <= 64) return city_hashlen33to64(s, len);

	/* For longer strings */
	{
		uint64 x = read64_le(s + len - 40);
		uint64 y = read64_le(s + len - 16) + read64_le(s + len - 56);
		uint64 z = city_hashlen16(read64_le(s + len - 48) + len, read64_le(s + len - 24));
		uint64 v0, v1, w0, w1;
		uint64 temp;

		/* WeakHashLen32WithSeeds */
		{
			const uint8 *ss = s + len - 64;
			uint64 wa = len;
			uint64 wb = z;
			uint64 ww = read64_le(ss);
			uint64 xx = read64_le(ss + 8);
			uint64 yy = read64_le(ss + 16);
			uint64 zz = read64_le(ss + 24);
			wa += ww;
			wb = ROTR64(wb + wa + zz, 21);
			uint64 cc = wa;
			wa += xx;
			wa += yy;
			wb += ROTR64(wa, 44);
			v0 = wa + zz;
			v1 = wb + cc;
		}

		/* WeakHashLen32WithSeeds */
		{
			const uint8 *ss = s + len - 32;
			uint64 wa = y + k1;
			uint64 wb = x;
			uint64 ww = read64_le(ss);
			uint64 xx = read64_le(ss + 8);
			uint64 yy = read64_le(ss + 16);
			uint64 zz = read64_le(ss + 24);
			wa += ww;
			wb = ROTR64(wb + wa + zz, 21);
			uint64 cc = wa;
			wa += xx;
			wa += yy;
			wb += ROTR64(wa, 44);
			w0 = wa + zz;
			w1 = wb + cc;
		}

		temp = z;
		z = x;
		x = temp;

		{
			size_t pos = 0;
			do {
				x = ROTR64(x + y + v0 + read64_le(s + pos + 8), 37) * k1;
				y = ROTR64(y + v1 + read64_le(s + pos + 48), 42) * k1;
				x ^= w1;
				y += v0 + read64_le(s + pos + 40);
				z = ROTR64(z + w0, 33) * k1;

				{
					const uint8 *ss = s + pos;
					uint64 wa = v1 * k1;
					uint64 wb = x + w0;
					uint64 ww = read64_le(ss);
					uint64 xx = read64_le(ss + 8);
					uint64 yy = read64_le(ss + 16);
					uint64 zz = read64_le(ss + 24);
					wa += ww;
					wb = ROTR64(wb + wa + zz, 21);
					uint64 cc = wa;
					wa += xx;
					wa += yy;
					wb += ROTR64(wa, 44);
					v0 = wa + zz;
					v1 = wb + cc;
				}

				{
					const uint8 *ss = s + pos + 32;
					uint64 wa = z + w1;
					uint64 wb = y + read64_le(ss + 16);
					uint64 ww = read64_le(ss);
					uint64 xx = read64_le(ss + 8);
					uint64 yy = read64_le(ss + 16);
					uint64 zz = read64_le(ss + 24);
					wa += ww;
					wb = ROTR64(wb + wa + zz, 21);
					uint64 cc = wa;
					wa += xx;
					wa += yy;
					wb += ROTR64(wa, 44);
					w0 = wa + zz;
					w1 = wb + cc;
				}

				temp = z;
				z = x;
				x = temp;

				pos += 64;
			} while (pos < len - 64);
		}

		return city_hashlen16(city_hashlen16(v0, w0) + city_shiftmix(y) * k1 + z,
		                      city_hashlen16(v1, w1) + x);
	}
}

uint64 WINAPI dkcHashCity64WithSeed(const void *data, size_t len, uint64 seed)
{
	return dkcHashCity64(data, len) ^ seed;
}

/* ====================================================================
 * wyhash
 * ==================================================================== */

static uint64 wymum(uint64 a, uint64 b)
{
	/* 64x64 -> 128 bit multiplication emulation */
	uint64 ha = a >> 32;
	uint64 hb = b >> 32;
	uint64 la = (uint32)a;
	uint64 lb = (uint32)b;
	uint64 hi, lo;
	uint64 rh = ha * hb;
	uint64 rm0 = ha * lb;
	uint64 rm1 = hb * la;
	uint64 rl = la * lb;
	uint64 t = rl + (rm0 << 32);
	uint64 c = t < rl;
	lo = t + (rm1 << 32);
	c += lo < t;
	hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;
	return lo ^ hi;
}

static uint64 wyr8(const uint8 *p)
{
	return read64_le(p);
}

static uint64 wyr4(const uint8 *p)
{
	return read32_le(p);
}

static uint64 wyr3(const uint8 *p, size_t k)
{
	return ((uint64)p[0] << 16) | ((uint64)p[k >> 1] << 8) | p[k - 1];
}

uint64 WINAPI dkcHashWy(const void *data, size_t len, uint64 seed)
{
	const uint8 *p = (const uint8 *)data;
	const uint64 secret[4] = {
		0xA0761D6478BD642FULL, 0xE7037ED1A0B428DBULL,
		0x8EBC6AF09C88C6E3ULL, 0x589965CC75374CC3ULL
	};

	seed ^= secret[0];

	if (len <= 16) {
		uint64 a, b;
		if (len >= 4) {
			a = (wyr4(p) << 32) | wyr4(p + ((len >> 3) << 2));
			b = (wyr4(p + len - 4) << 32) | wyr4(p + len - 4 - ((len >> 3) << 2));
		} else if (len > 0) {
			a = wyr3(p, len);
			b = 0;
		} else {
			a = b = 0;
		}
		return wymum(wymum(a ^ secret[1], b ^ seed), len ^ secret[1]);
	}

	if (len <= 48) {
		uint64 a = wyr8(p) ^ secret[1];
		uint64 b = wyr8(p + 8) ^ seed;
		uint64 c = wyr8(p + len - 16) ^ secret[2];
		uint64 d = wyr8(p + len - 8) ^ secret[3];
		if (len > 32) {
			a ^= wyr8(p + 16);
			b ^= wyr8(p + 24);
		}
		return wymum(wymum(a, b) ^ len, wymum(c, d) ^ seed);
	}

	{
		uint64 see1 = seed;
		uint64 see2 = seed;
		size_t i = len;

		while (i > 48) {
			seed = wymum(wyr8(p) ^ secret[1], wyr8(p + 8) ^ seed);
			see1 = wymum(wyr8(p + 16) ^ secret[2], wyr8(p + 24) ^ see1);
			see2 = wymum(wyr8(p + 32) ^ secret[3], wyr8(p + 40) ^ see2);
			p += 48;
			i -= 48;
		}

		seed ^= see1 ^ see2;
		while (i > 16) {
			seed = wymum(wyr8(p) ^ secret[1], wyr8(p + 8) ^ seed);
			p += 16;
			i -= 16;
		}

		return wymum(wymum(wyr8(p + i - 16) ^ secret[1], wyr8(p + i - 8) ^ seed),
		             len ^ secret[1]);
	}
}

/* ====================================================================
 * FxHash
 * ==================================================================== */

uint32 WINAPI dkcHashFx32(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	const uint32 SEED = 0x9E3779B9U; /* 黄金比 */
	uint32 hash = 0;

	while (len >= 4) {
		uint32 word = read32_le(p);
		hash = ROTL32(hash, 5) ^ word;
		hash *= SEED;
		p += 4;
		len -= 4;
	}

	/* Handle remaining bytes */
	if (len > 0) {
		uint32 word = 0;
		switch (len) {
		case 3: word |= (uint32)p[2] << 16; /* fall through */
		case 2: word |= (uint32)p[1] << 8;  /* fall through */
		case 1: word |= (uint32)p[0];
		}
		hash = ROTL32(hash, 5) ^ word;
		hash *= SEED;
	}

	return hash;
}

uint64 WINAPI dkcHashFx64(const void *data, size_t len)
{
	const uint8 *p = (const uint8 *)data;
	const uint64 SEED = 0x517CC1B727220A95ULL;
	uint64 hash = 0;

	while (len >= 8) {
		uint64 word = read64_le(p);
		hash = ROTL64(hash, 5) ^ word;
		hash *= SEED;
		p += 8;
		len -= 8;
	}

	/* Handle remaining bytes */
	if (len > 0) {
		uint64 word = 0;
		const uint8 *end = p + len;
		while (p < end) {
			word = (word << 8) | *p++;
		}
		hash = ROTL64(hash, 5) ^ word;
		hash *= SEED;
	}

	return hash;
}

/* ====================================================================
 * Jenkins lookup3
 * ==================================================================== */

#define lookup3_mix(a, b, c) \
{ \
	a -= c; a ^= ROTL32(c, 4);  c += b; \
	b -= a; b ^= ROTL32(a, 6);  a += c; \
	c -= b; c ^= ROTL32(b, 8);  b += a; \
	a -= c; a ^= ROTL32(c, 16); c += b; \
	b -= a; b ^= ROTL32(a, 19); a += c; \
	c -= b; c ^= ROTL32(b, 4);  b += a; \
}

#define lookup3_final(a, b, c) \
{ \
	c ^= b; c -= ROTL32(b, 14); \
	a ^= c; a -= ROTL32(c, 11); \
	b ^= a; b -= ROTL32(a, 25); \
	c ^= b; c -= ROTL32(b, 16); \
	a ^= c; a -= ROTL32(c, 4);  \
	b ^= a; b -= ROTL32(a, 14); \
	c ^= b; c -= ROTL32(b, 24); \
}

uint32 WINAPI dkcHashLookup3(const void *data, size_t len, uint32 seed)
{
	const uint8 *k = (const uint8 *)data;
	uint32 a, b, c;

	a = b = c = 0xDEADBEEF + (uint32)len + seed;

	/* Main loop - handle 12 bytes at a time */
	while (len > 12) {
		a += read32_le(k);
		b += read32_le(k + 4);
		c += read32_le(k + 8);
		lookup3_mix(a, b, c);
		k += 12;
		len -= 12;
	}

	/* Handle the last 0-12 bytes */
	switch (len) {
	case 12: c += read32_le(k + 8); b += read32_le(k + 4); a += read32_le(k); break;
	case 11: c += (uint32)k[10] << 16; /* fall through */
	case 10: c += (uint32)k[9] << 8;   /* fall through */
	case 9:  c += (uint32)k[8];        /* fall through */
	case 8:  b += read32_le(k + 4); a += read32_le(k); break;
	case 7:  b += (uint32)k[6] << 16;  /* fall through */
	case 6:  b += (uint32)k[5] << 8;   /* fall through */
	case 5:  b += (uint32)k[4];        /* fall through */
	case 4:  a += read32_le(k); break;
	case 3:  a += (uint32)k[2] << 16;  /* fall through */
	case 2:  a += (uint32)k[1] << 8;   /* fall through */
	case 1:  a += (uint32)k[0]; break;
	case 0:  return c;
	}

	lookup3_final(a, b, c);
	return c;
}

void WINAPI dkcHashLookup3Pair(const void *data, size_t len, uint32 *pc, uint32 *pb)
{
	const uint8 *k = (const uint8 *)data;
	uint32 a, b, c;

	a = b = c = 0xDEADBEEF + (uint32)len + *pc;
	c += *pb;

	while (len > 12) {
		a += read32_le(k);
		b += read32_le(k + 4);
		c += read32_le(k + 8);
		lookup3_mix(a, b, c);
		k += 12;
		len -= 12;
	}

	switch (len) {
	case 12: c += read32_le(k + 8); b += read32_le(k + 4); a += read32_le(k); break;
	case 11: c += (uint32)k[10] << 16;
	case 10: c += (uint32)k[9] << 8;
	case 9:  c += (uint32)k[8];
	case 8:  b += read32_le(k + 4); a += read32_le(k); break;
	case 7:  b += (uint32)k[6] << 16;
	case 6:  b += (uint32)k[5] << 8;
	case 5:  b += (uint32)k[4];
	case 4:  a += read32_le(k); break;
	case 3:  a += (uint32)k[2] << 16;
	case 2:  a += (uint32)k[1] << 8;
	case 1:  a += (uint32)k[0]; break;
	case 0:  *pc = c; *pb = b; return;
	}

	lookup3_final(a, b, c);
	*pc = c;
	*pb = b;
}

/* ====================================================================
 * ユーティリティ関数
 * ==================================================================== */

uint32 WINAPI dkcHashString(const char *str)
{
	if (!str) return 0;
	return dkcHashFNV1a_32(str, strlen(str));
}

uint32 WINAPI dkcHashInt32(uint32 x)
{
	/* Murmur3風の整数ハッシュ */
	x ^= x >> 16;
	x *= 0x85EBCA6B;
	x ^= x >> 13;
	x *= 0xC2B2AE35;
	x ^= x >> 16;
	return x;
}

uint64 WINAPI dkcHashInt64(uint64 x)
{
	/* SplitMix64 */
	x ^= x >> 30;
	x *= 0xBF58476D1CE4E5B9ULL;
	x ^= x >> 27;
	x *= 0x94D049BB133111EBULL;
	x ^= x >> 31;
	return x;
}

size_t WINAPI dkcHashPtr(const void *ptr)
{
#if defined(_WIN64) || defined(__LP64__)
	return (size_t)dkcHashInt64((uint64)(size_t)ptr);
#else
	return (size_t)dkcHashInt32((uint32)(size_t)ptr);
#endif
}

uint32 WINAPI dkcHashCombine32(uint32 h1, uint32 h2)
{
	/* Boost hash_combine方式 */
	h1 ^= h2 + 0x9E3779B9 + (h1 << 6) + (h1 >> 2);
	return h1;
}

uint64 WINAPI dkcHashCombine64(uint64 h1, uint64 h2)
{
	h1 ^= h2 + 0x9E3779B97F4A7C15ULL + (h1 << 12) + (h1 >> 4);
	return h1;
}
