/*! @file
	@brief Threefish Block Cipher 実装
	@author d.Kingyo
	@note
	Skeinハッシュ関数で使用されるThreefish暗号の実装
	参考: The Skein Hash Function Family (Version 1.3)
*/

#include "dkcThreefish.h"
#include <string.h>

/* ====================================================================
 * ローテーション定数
 * ==================================================================== */

/* Threefish-256 rotation constants */
static const int ROT_256[8][2] = {
	{14, 16}, {52, 57}, {23, 40}, { 5, 37},
	{25, 33}, {46, 12}, {58, 22}, {32, 32}
};

/* Threefish-512 rotation constants */
static const int ROT_512[8][4] = {
	{46, 36, 19, 37}, {33, 27, 14, 42},
	{17, 49, 36, 39}, {44,  9, 54, 56},
	{39, 30, 34, 24}, {13, 50, 10, 17},
	{25, 29, 39, 43}, { 8, 35, 56, 22}
};

/* Threefish-1024 rotation constants */
static const int ROT_1024[8][8] = {
	{24, 13,  8, 47,  8, 17, 22, 37},
	{38, 19, 10, 55, 49, 18, 23, 52},
	{33,  4, 51, 13, 34, 41, 59, 17},
	{ 5, 20, 48, 41, 47, 28, 16, 25},
	{41,  9, 37, 31, 12, 47, 44, 30},
	{16, 34, 56, 51,  4, 53, 42, 41},
	{31, 44, 47, 46, 19, 42, 44, 25},
	{ 9, 48, 35, 52, 23, 31, 37, 20}
};

/* ====================================================================
 * ユーティリティマクロ
 * ==================================================================== */

/* 64ビット左ローテーション */
#define ROTL64(x, n) (((x) << (n)) | ((x) >> (64 - (n))))

/* 64ビット右ローテーション */
#define ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

/* リトルエンディアンでの64ビット読み込み */
static DKC_INLINE uint64 load64_le(const unsigned char *p)
{
	return ((uint64)p[0])       | ((uint64)p[1] << 8)  |
	       ((uint64)p[2] << 16) | ((uint64)p[3] << 24) |
	       ((uint64)p[4] << 32) | ((uint64)p[5] << 40) |
	       ((uint64)p[6] << 48) | ((uint64)p[7] << 56);
}

/* リトルエンディアンでの64ビット書き込み */
static DKC_INLINE void store64_le(unsigned char *p, uint64 x)
{
	p[0] = (unsigned char)(x);
	p[1] = (unsigned char)(x >> 8);
	p[2] = (unsigned char)(x >> 16);
	p[3] = (unsigned char)(x >> 24);
	p[4] = (unsigned char)(x >> 32);
	p[5] = (unsigned char)(x >> 40);
	p[6] = (unsigned char)(x >> 48);
	p[7] = (unsigned char)(x >> 56);
}

/* ====================================================================
 * Threefish-256 実装
 * ==================================================================== */

/* MIX関数（暗号化用） */
#define MIX256(x0, x1, r) do { \
	x0 += x1; \
	x1 = ROTL64(x1, r) ^ x0; \
} while(0)

/* MIX逆関数（復号化用） */
#define UNMIX256(x0, x1, r) do { \
	x1 ^= x0; \
	x1 = ROTR64(x1, r); \
	x0 -= x1; \
} while(0)

/* 拡張キーとTweakを設定 */
static void threefish256_set_key_tweak(DKC_THREEFISH256 *p,
	const unsigned char *key, const unsigned char *tweak)
{
	int i;

	/* キーを読み込み */
	for(i = 0; i < 4; i++){
		p->key[i] = load64_le(key + i * 8);
	}

	/* パリティワードを計算 */
	p->key[4] = dkcd_THREEFISH_PARITY;
	for(i = 0; i < 4; i++){
		p->key[4] ^= p->key[i];
	}

	/* Tweakを読み込み */
	if(tweak != NULL){
		p->tweak[0] = load64_le(tweak);
		p->tweak[1] = load64_le(tweak + 8);
	}
	else{
		p->tweak[0] = 0;
		p->tweak[1] = 0;
	}
	p->tweak[2] = p->tweak[0] ^ p->tweak[1];
}

/* 1ブロック暗号化 */
static void threefish256_encrypt_block(DKC_THREEFISH256 *p,
	uint64 *out, const uint64 *in)
{
	uint64 v[4];
	int d, r;

	/* 入力をコピー */
	v[0] = in[0];
	v[1] = in[1];
	v[2] = in[2];
	v[3] = in[3];

	/* 72ラウンド（サブキー注入は4ラウンドごと） */
	for(d = 0; d < 18; d++){
		/* サブキー注入 */
		v[0] += p->key[d % 5];
		v[1] += p->key[(d + 1) % 5] + p->tweak[d % 3];
		v[2] += p->key[(d + 2) % 5] + p->tweak[(d + 1) % 3];
		v[3] += p->key[(d + 3) % 5] + (uint64)d;

		/* 4ラウンド */
		for(r = 0; r < 4; r++){
			int idx = (d * 4 + r) % 8;
			if(r % 2 == 0){
				MIX256(v[0], v[1], ROT_256[idx][0]);
				MIX256(v[2], v[3], ROT_256[idx][1]);
			}
			else{
				MIX256(v[0], v[3], ROT_256[idx][0]);
				MIX256(v[2], v[1], ROT_256[idx][1]);
			}
		}
	}

	/* 最終サブキー注入 */
	out[0] = v[0] + p->key[18 % 5];
	out[1] = v[1] + p->key[(18 + 1) % 5] + p->tweak[18 % 3];
	out[2] = v[2] + p->key[(18 + 2) % 5] + p->tweak[(18 + 1) % 3];
	out[3] = v[3] + p->key[(18 + 3) % 5] + 18;
}

/* 1ブロック復号化 */
static void threefish256_decrypt_block(DKC_THREEFISH256 *p,
	uint64 *out, const uint64 *in)
{
	uint64 v[4];
	int d, r;

	/* 最終サブキー除去 */
	v[0] = in[0] - p->key[18 % 5];
	v[1] = in[1] - p->key[(18 + 1) % 5] - p->tweak[18 % 3];
	v[2] = in[2] - p->key[(18 + 2) % 5] - p->tweak[(18 + 1) % 3];
	v[3] = in[3] - p->key[(18 + 3) % 5] - 18;

	/* 逆順でラウンド処理 */
	for(d = 17; d >= 0; d--){
		/* 4ラウンド逆変換 */
		for(r = 3; r >= 0; r--){
			int idx = (d * 4 + r) % 8;
			if(r % 2 == 0){
				UNMIX256(v[2], v[3], ROT_256[idx][1]);
				UNMIX256(v[0], v[1], ROT_256[idx][0]);
			}
			else{
				UNMIX256(v[2], v[1], ROT_256[idx][1]);
				UNMIX256(v[0], v[3], ROT_256[idx][0]);
			}
		}

		/* サブキー除去 */
		v[0] -= p->key[d % 5];
		v[1] -= p->key[(d + 1) % 5] + p->tweak[d % 3];
		v[2] -= p->key[(d + 2) % 5] + p->tweak[(d + 1) % 3];
		v[3] -= p->key[(d + 3) % 5] + (uint64)d;
	}

	out[0] = v[0];
	out[1] = v[1];
	out[2] = v[2];
	out[3] = v[3];
}

/* ====================================================================
 * Threefish-512 実装
 * ==================================================================== */

/* MIX関数（暗号化用） */
#define MIX512(x0, x1, r) do { \
	x0 += x1; \
	x1 = ROTL64(x1, r) ^ x0; \
} while(0)

/* MIX逆関数（復号化用） */
#define UNMIX512(x0, x1, r) do { \
	x1 ^= x0; \
	x1 = ROTR64(x1, r); \
	x0 -= x1; \
} while(0)

/* 拡張キーとTweakを設定 */
static void threefish512_set_key_tweak(DKC_THREEFISH512 *p,
	const unsigned char *key, const unsigned char *tweak)
{
	int i;

	/* キーを読み込み */
	for(i = 0; i < 8; i++){
		p->key[i] = load64_le(key + i * 8);
	}

	/* パリティワードを計算 */
	p->key[8] = dkcd_THREEFISH_PARITY;
	for(i = 0; i < 8; i++){
		p->key[8] ^= p->key[i];
	}

	/* Tweakを読み込み */
	if(tweak != NULL){
		p->tweak[0] = load64_le(tweak);
		p->tweak[1] = load64_le(tweak + 8);
	}
	else{
		p->tweak[0] = 0;
		p->tweak[1] = 0;
	}
	p->tweak[2] = p->tweak[0] ^ p->tweak[1];
}

/* Threefish-512の置換パターン */
static const int PERM_512[4][8] = {
	{0, 1, 2, 3, 4, 5, 6, 7},  /* even rounds, mix pairs */
	{2, 1, 4, 7, 6, 5, 0, 3},  /* odd rounds, mix pairs */
	{0, 1, 2, 3, 4, 5, 6, 7},  /* placeholder */
	{0, 1, 2, 3, 4, 5, 6, 7}   /* placeholder */
};

/* 1ブロック暗号化 */
static void threefish512_encrypt_block(DKC_THREEFISH512 *p,
	uint64 *out, const uint64 *in)
{
	uint64 v[8];
	int d, r, i;

	/* 入力をコピー */
	for(i = 0; i < 8; i++){
		v[i] = in[i];
	}

	/* 72ラウンド */
	for(d = 0; d < 18; d++){
		/* サブキー注入 */
		for(i = 0; i < 8; i++){
			v[i] += p->key[(d + i) % 9];
		}
		v[5] += p->tweak[d % 3];
		v[6] += p->tweak[(d + 1) % 3];
		v[7] += (uint64)d;

		/* 4ラウンド */
		for(r = 0; r < 4; r++){
			int idx = (d * 4 + r) % 8;

			if(r % 2 == 0){
				/* Even: (0,1), (2,3), (4,5), (6,7) */
				MIX512(v[0], v[1], ROT_512[idx][0]);
				MIX512(v[2], v[3], ROT_512[idx][1]);
				MIX512(v[4], v[5], ROT_512[idx][2]);
				MIX512(v[6], v[7], ROT_512[idx][3]);
			}
			else{
				/* Odd: (0,3), (2,1), (4,7), (6,5) -> permuted (2,1,4,7,6,5,0,3) */
				MIX512(v[0], v[3], ROT_512[idx][0]);
				MIX512(v[2], v[1], ROT_512[idx][1]);
				MIX512(v[4], v[7], ROT_512[idx][2]);
				MIX512(v[6], v[5], ROT_512[idx][3]);
			}
		}
	}

	/* 最終サブキー注入 */
	for(i = 0; i < 8; i++){
		out[i] = v[i] + p->key[(18 + i) % 9];
	}
	out[5] += p->tweak[18 % 3];
	out[6] += p->tweak[(18 + 1) % 3];
	out[7] += 18;
}

/* 1ブロック復号化 */
static void threefish512_decrypt_block(DKC_THREEFISH512 *p,
	uint64 *out, const uint64 *in)
{
	uint64 v[8];
	int d, r, i;

	/* 最終サブキー除去 */
	for(i = 0; i < 8; i++){
		v[i] = in[i] - p->key[(18 + i) % 9];
	}
	v[5] -= p->tweak[18 % 3];
	v[6] -= p->tweak[(18 + 1) % 3];
	v[7] -= 18;

	/* 逆順でラウンド処理 */
	for(d = 17; d >= 0; d--){
		/* 4ラウンド逆変換 */
		for(r = 3; r >= 0; r--){
			int idx = (d * 4 + r) % 8;

			if(r % 2 == 0){
				UNMIX512(v[6], v[7], ROT_512[idx][3]);
				UNMIX512(v[4], v[5], ROT_512[idx][2]);
				UNMIX512(v[2], v[3], ROT_512[idx][1]);
				UNMIX512(v[0], v[1], ROT_512[idx][0]);
			}
			else{
				UNMIX512(v[6], v[5], ROT_512[idx][3]);
				UNMIX512(v[4], v[7], ROT_512[idx][2]);
				UNMIX512(v[2], v[1], ROT_512[idx][1]);
				UNMIX512(v[0], v[3], ROT_512[idx][0]);
			}
		}

		/* サブキー除去 */
		v[7] -= (uint64)d;
		v[6] -= p->tweak[(d + 1) % 3];
		v[5] -= p->tweak[d % 3];
		for(i = 0; i < 8; i++){
			v[i] -= p->key[(d + i) % 9];
		}
	}

	for(i = 0; i < 8; i++){
		out[i] = v[i];
	}
}

/* ====================================================================
 * Threefish-1024 実装
 * ==================================================================== */

/* 拡張キーとTweakを設定 */
static void threefish1024_set_key_tweak(DKC_THREEFISH1024 *p,
	const unsigned char *key, const unsigned char *tweak)
{
	int i;

	/* キーを読み込み */
	for(i = 0; i < 16; i++){
		p->key[i] = load64_le(key + i * 8);
	}

	/* パリティワードを計算 */
	p->key[16] = dkcd_THREEFISH_PARITY;
	for(i = 0; i < 16; i++){
		p->key[16] ^= p->key[i];
	}

	/* Tweakを読み込み */
	if(tweak != NULL){
		p->tweak[0] = load64_le(tweak);
		p->tweak[1] = load64_le(tweak + 8);
	}
	else{
		p->tweak[0] = 0;
		p->tweak[1] = 0;
	}
	p->tweak[2] = p->tweak[0] ^ p->tweak[1];
}

/* 1ブロック暗号化 */
static void threefish1024_encrypt_block(DKC_THREEFISH1024 *p,
	uint64 *out, const uint64 *in)
{
	uint64 v[16];
	int d, r, i;

	/* 入力をコピー */
	for(i = 0; i < 16; i++){
		v[i] = in[i];
	}

	/* 80ラウンド（20 * 4） */
	for(d = 0; d < 20; d++){
		/* サブキー注入 */
		for(i = 0; i < 16; i++){
			v[i] += p->key[(d + i) % 17];
		}
		v[13] += p->tweak[d % 3];
		v[14] += p->tweak[(d + 1) % 3];
		v[15] += (uint64)d;

		/* 4ラウンド */
		for(r = 0; r < 4; r++){
			int idx = (d * 4 + r) % 8;

			if(r % 2 == 0){
				/* Even round pairs: (0,1), (2,3), (4,5), (6,7), (8,9), (10,11), (12,13), (14,15) */
				MIX512(v[0],  v[1],  ROT_1024[idx][0]);
				MIX512(v[2],  v[3],  ROT_1024[idx][1]);
				MIX512(v[4],  v[5],  ROT_1024[idx][2]);
				MIX512(v[6],  v[7],  ROT_1024[idx][3]);
				MIX512(v[8],  v[9],  ROT_1024[idx][4]);
				MIX512(v[10], v[11], ROT_1024[idx][5]);
				MIX512(v[12], v[13], ROT_1024[idx][6]);
				MIX512(v[14], v[15], ROT_1024[idx][7]);
			}
			else{
				/* Odd round pairs: (0,9), (2,13), (4,11), (6,15), (8,7), (10,3), (12,5), (14,1) */
				MIX512(v[0],  v[9],  ROT_1024[idx][0]);
				MIX512(v[2],  v[13], ROT_1024[idx][1]);
				MIX512(v[4],  v[11], ROT_1024[idx][2]);
				MIX512(v[6],  v[15], ROT_1024[idx][3]);
				MIX512(v[8],  v[7],  ROT_1024[idx][4]);
				MIX512(v[10], v[3],  ROT_1024[idx][5]);
				MIX512(v[12], v[5],  ROT_1024[idx][6]);
				MIX512(v[14], v[1],  ROT_1024[idx][7]);
			}
		}
	}

	/* 最終サブキー注入 */
	for(i = 0; i < 16; i++){
		out[i] = v[i] + p->key[(20 + i) % 17];
	}
	out[13] += p->tweak[20 % 3];
	out[14] += p->tweak[(20 + 1) % 3];
	out[15] += 20;
}

/* 1ブロック復号化 */
static void threefish1024_decrypt_block(DKC_THREEFISH1024 *p,
	uint64 *out, const uint64 *in)
{
	uint64 v[16];
	int d, r, i;

	/* 最終サブキー除去 */
	for(i = 0; i < 16; i++){
		v[i] = in[i] - p->key[(20 + i) % 17];
	}
	v[13] -= p->tweak[20 % 3];
	v[14] -= p->tweak[(20 + 1) % 3];
	v[15] -= 20;

	/* 逆順でラウンド処理 */
	for(d = 19; d >= 0; d--){
		/* 4ラウンド逆変換 */
		for(r = 3; r >= 0; r--){
			int idx = (d * 4 + r) % 8;

			if(r % 2 == 0){
				UNMIX512(v[14], v[15], ROT_1024[idx][7]);
				UNMIX512(v[12], v[13], ROT_1024[idx][6]);
				UNMIX512(v[10], v[11], ROT_1024[idx][5]);
				UNMIX512(v[8],  v[9],  ROT_1024[idx][4]);
				UNMIX512(v[6],  v[7],  ROT_1024[idx][3]);
				UNMIX512(v[4],  v[5],  ROT_1024[idx][2]);
				UNMIX512(v[2],  v[3],  ROT_1024[idx][1]);
				UNMIX512(v[0],  v[1],  ROT_1024[idx][0]);
			}
			else{
				UNMIX512(v[14], v[1],  ROT_1024[idx][7]);
				UNMIX512(v[12], v[5],  ROT_1024[idx][6]);
				UNMIX512(v[10], v[3],  ROT_1024[idx][5]);
				UNMIX512(v[8],  v[7],  ROT_1024[idx][4]);
				UNMIX512(v[6],  v[15], ROT_1024[idx][3]);
				UNMIX512(v[4],  v[11], ROT_1024[idx][2]);
				UNMIX512(v[2],  v[13], ROT_1024[idx][1]);
				UNMIX512(v[0],  v[9],  ROT_1024[idx][0]);
			}
		}

		/* サブキー除去 */
		v[15] -= (uint64)d;
		v[14] -= p->tweak[(d + 1) % 3];
		v[13] -= p->tweak[d % 3];
		for(i = 0; i < 16; i++){
			v[i] -= p->key[(d + i) % 17];
		}
	}

	for(i = 0; i < 16; i++){
		out[i] = v[i];
	}
}

/* ====================================================================
 * 外部関数実装 - Threefish-256
 * ==================================================================== */

DKC_EXTERN DKC_THREEFISH256 * WINAPI dkcAllocThreefish256(
	const unsigned char *key, size_t keylen,
	const unsigned char *tweak)
{
	DKC_THREEFISH256 *p;

	if(key == NULL || keylen < 32) return NULL;

	p = (DKC_THREEFISH256 *)dkcAllocateFill(sizeof(DKC_THREEFISH256), 0);
	if(p == NULL) return NULL;

	threefish256_set_key_tweak(p, key, tweak);

	return p;
}

DKC_EXTERN int WINAPI dkcFreeThreefish256(DKC_THREEFISH256 **p)
{
	if(p == NULL || *p == NULL) return edk_FAILED;

	/* センシティブデータをクリア */
	memset(*p, 0, sizeof(DKC_THREEFISH256));
	dkcFree((void **)p);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcThreefish256Encrypt(
	DKC_THREEFISH256 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	uint64 in[4], out[4];
	size_t i;

	if(p == NULL || dest == NULL || src == NULL) return edk_FAILED;
	if(srcsize % 32 != 0) return edk_FAILED;
	if(destsize < srcsize) return edk_FAILED;

	while(srcsize >= 32){
		/* 入力を読み込み */
		for(i = 0; i < 4; i++){
			in[i] = load64_le(src + i * 8);
		}

		/* 暗号化 */
		threefish256_encrypt_block(p, out, in);

		/* 出力を書き込み */
		for(i = 0; i < 4; i++){
			store64_le(dest + i * 8, out[i]);
		}

		src += 32;
		dest += 32;
		srcsize -= 32;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcThreefish256Decrypt(
	DKC_THREEFISH256 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	uint64 in[4], out[4];
	size_t i;

	if(p == NULL || dest == NULL || src == NULL) return edk_FAILED;
	if(srcsize % 32 != 0) return edk_FAILED;
	if(destsize < srcsize) return edk_FAILED;

	while(srcsize >= 32){
		/* 入力を読み込み */
		for(i = 0; i < 4; i++){
			in[i] = load64_le(src + i * 8);
		}

		/* 復号化 */
		threefish256_decrypt_block(p, out, in);

		/* 出力を書き込み */
		for(i = 0; i < 4; i++){
			store64_le(dest + i * 8, out[i]);
		}

		src += 32;
		dest += 32;
		srcsize -= 32;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcThreefish256EncryptNoDest(
	DKC_THREEFISH256 *p,
	unsigned char *dest_and_src, size_t size)
{
	dkcThreefish256Encrypt(p, dest_and_src, size, dest_and_src, size);
}

DKC_EXTERN void WINAPI dkcThreefish256DecryptNoDest(
	DKC_THREEFISH256 *p,
	unsigned char *dest_and_src, size_t size)
{
	dkcThreefish256Decrypt(p, dest_and_src, size, dest_and_src, size);
}

DKC_EXTERN int WINAPI dkcThreefish256SetTweak(
	DKC_THREEFISH256 *p, const unsigned char *tweak)
{
	if(p == NULL || tweak == NULL) return edk_FAILED;

	p->tweak[0] = load64_le(tweak);
	p->tweak[1] = load64_le(tweak + 8);
	p->tweak[2] = p->tweak[0] ^ p->tweak[1];

	return edk_SUCCEEDED;
}

/* ====================================================================
 * 外部関数実装 - Threefish-512
 * ==================================================================== */

DKC_EXTERN DKC_THREEFISH512 * WINAPI dkcAllocThreefish512(
	const unsigned char *key, size_t keylen,
	const unsigned char *tweak)
{
	DKC_THREEFISH512 *p;

	if(key == NULL || keylen < 64) return NULL;

	p = (DKC_THREEFISH512 *)dkcAllocateFill(sizeof(DKC_THREEFISH512), 0);
	if(p == NULL) return NULL;

	threefish512_set_key_tweak(p, key, tweak);

	return p;
}

DKC_EXTERN int WINAPI dkcFreeThreefish512(DKC_THREEFISH512 **p)
{
	if(p == NULL || *p == NULL) return edk_FAILED;

	memset(*p, 0, sizeof(DKC_THREEFISH512));
	dkcFree((void **)p);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcThreefish512Encrypt(
	DKC_THREEFISH512 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	uint64 in[8], out[8];
	size_t i;

	if(p == NULL || dest == NULL || src == NULL) return edk_FAILED;
	if(srcsize % 64 != 0) return edk_FAILED;
	if(destsize < srcsize) return edk_FAILED;

	while(srcsize >= 64){
		for(i = 0; i < 8; i++){
			in[i] = load64_le(src + i * 8);
		}

		threefish512_encrypt_block(p, out, in);

		for(i = 0; i < 8; i++){
			store64_le(dest + i * 8, out[i]);
		}

		src += 64;
		dest += 64;
		srcsize -= 64;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcThreefish512Decrypt(
	DKC_THREEFISH512 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	uint64 in[8], out[8];
	size_t i;

	if(p == NULL || dest == NULL || src == NULL) return edk_FAILED;
	if(srcsize % 64 != 0) return edk_FAILED;
	if(destsize < srcsize) return edk_FAILED;

	while(srcsize >= 64){
		for(i = 0; i < 8; i++){
			in[i] = load64_le(src + i * 8);
		}

		threefish512_decrypt_block(p, out, in);

		for(i = 0; i < 8; i++){
			store64_le(dest + i * 8, out[i]);
		}

		src += 64;
		dest += 64;
		srcsize -= 64;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcThreefish512EncryptNoDest(
	DKC_THREEFISH512 *p,
	unsigned char *dest_and_src, size_t size)
{
	dkcThreefish512Encrypt(p, dest_and_src, size, dest_and_src, size);
}

DKC_EXTERN void WINAPI dkcThreefish512DecryptNoDest(
	DKC_THREEFISH512 *p,
	unsigned char *dest_and_src, size_t size)
{
	dkcThreefish512Decrypt(p, dest_and_src, size, dest_and_src, size);
}

DKC_EXTERN int WINAPI dkcThreefish512SetTweak(
	DKC_THREEFISH512 *p, const unsigned char *tweak)
{
	if(p == NULL || tweak == NULL) return edk_FAILED;

	p->tweak[0] = load64_le(tweak);
	p->tweak[1] = load64_le(tweak + 8);
	p->tweak[2] = p->tweak[0] ^ p->tweak[1];

	return edk_SUCCEEDED;
}

/* ====================================================================
 * 外部関数実装 - Threefish-1024
 * ==================================================================== */

DKC_EXTERN DKC_THREEFISH1024 * WINAPI dkcAllocThreefish1024(
	const unsigned char *key, size_t keylen,
	const unsigned char *tweak)
{
	DKC_THREEFISH1024 *p;

	if(key == NULL || keylen < 128) return NULL;

	p = (DKC_THREEFISH1024 *)dkcAllocateFill(sizeof(DKC_THREEFISH1024), 0);
	if(p == NULL) return NULL;

	threefish1024_set_key_tweak(p, key, tweak);

	return p;
}

DKC_EXTERN int WINAPI dkcFreeThreefish1024(DKC_THREEFISH1024 **p)
{
	if(p == NULL || *p == NULL) return edk_FAILED;

	memset(*p, 0, sizeof(DKC_THREEFISH1024));
	dkcFree((void **)p);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcThreefish1024Encrypt(
	DKC_THREEFISH1024 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	uint64 in[16], out[16];
	size_t i;

	if(p == NULL || dest == NULL || src == NULL) return edk_FAILED;
	if(srcsize % 128 != 0) return edk_FAILED;
	if(destsize < srcsize) return edk_FAILED;

	while(srcsize >= 128){
		for(i = 0; i < 16; i++){
			in[i] = load64_le(src + i * 8);
		}

		threefish1024_encrypt_block(p, out, in);

		for(i = 0; i < 16; i++){
			store64_le(dest + i * 8, out[i]);
		}

		src += 128;
		dest += 128;
		srcsize -= 128;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcThreefish1024Decrypt(
	DKC_THREEFISH1024 *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	uint64 in[16], out[16];
	size_t i;

	if(p == NULL || dest == NULL || src == NULL) return edk_FAILED;
	if(srcsize % 128 != 0) return edk_FAILED;
	if(destsize < srcsize) return edk_FAILED;

	while(srcsize >= 128){
		for(i = 0; i < 16; i++){
			in[i] = load64_le(src + i * 8);
		}

		threefish1024_decrypt_block(p, out, in);

		for(i = 0; i < 16; i++){
			store64_le(dest + i * 8, out[i]);
		}

		src += 128;
		dest += 128;
		srcsize -= 128;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcThreefish1024EncryptNoDest(
	DKC_THREEFISH1024 *p,
	unsigned char *dest_and_src, size_t size)
{
	dkcThreefish1024Encrypt(p, dest_and_src, size, dest_and_src, size);
}

DKC_EXTERN void WINAPI dkcThreefish1024DecryptNoDest(
	DKC_THREEFISH1024 *p,
	unsigned char *dest_and_src, size_t size)
{
	dkcThreefish1024Decrypt(p, dest_and_src, size, dest_and_src, size);
}

DKC_EXTERN int WINAPI dkcThreefish1024SetTweak(
	DKC_THREEFISH1024 *p, const unsigned char *tweak)
{
	if(p == NULL || tweak == NULL) return edk_FAILED;

	p->tweak[0] = load64_le(tweak);
	p->tweak[1] = load64_le(tweak + 8);
	p->tweak[2] = p->tweak[0] ^ p->tweak[1];

	return edk_SUCCEEDED;
}

/* ====================================================================
 * 外部関数実装 - 汎用Threefish
 * ==================================================================== */

DKC_EXTERN DKC_THREEFISH * WINAPI dkcAllocThreefish(
	edkcThreefishVariant variant,
	const unsigned char *key, size_t keylen,
	const unsigned char *tweak)
{
	DKC_THREEFISH *p;
	size_t required_keylen;

	if(key == NULL) return NULL;

	switch(variant){
	case edkcThreefish256:
		required_keylen = 32;
		break;
	case edkcThreefish512:
		required_keylen = 64;
		break;
	case edkcThreefish1024:
		required_keylen = 128;
		break;
	default:
		return NULL;
	}

	if(keylen < required_keylen) return NULL;

	p = (DKC_THREEFISH *)dkcAllocateFill(sizeof(DKC_THREEFISH), 0);
	if(p == NULL) return NULL;

	p->variant = variant;

	switch(variant){
	case edkcThreefish256:
		threefish256_set_key_tweak(&p->ctx.tf256, key, tweak);
		break;
	case edkcThreefish512:
		threefish512_set_key_tweak(&p->ctx.tf512, key, tweak);
		break;
	case edkcThreefish1024:
		threefish1024_set_key_tweak(&p->ctx.tf1024, key, tweak);
		break;
	}

	return p;
}

DKC_EXTERN int WINAPI dkcFreeThreefish(DKC_THREEFISH **p)
{
	if(p == NULL || *p == NULL) return edk_FAILED;

	memset(*p, 0, sizeof(DKC_THREEFISH));
	dkcFree((void **)p);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcThreefishEncrypt(
	DKC_THREEFISH *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	if(p == NULL) return edk_FAILED;

	switch(p->variant){
	case edkcThreefish256:
		return dkcThreefish256Encrypt(&p->ctx.tf256, dest, destsize, src, srcsize);
	case edkcThreefish512:
		return dkcThreefish512Encrypt(&p->ctx.tf512, dest, destsize, src, srcsize);
	case edkcThreefish1024:
		return dkcThreefish1024Encrypt(&p->ctx.tf1024, dest, destsize, src, srcsize);
	default:
		return edk_FAILED;
	}
}

DKC_EXTERN int WINAPI dkcThreefishDecrypt(
	DKC_THREEFISH *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	if(p == NULL) return edk_FAILED;

	switch(p->variant){
	case edkcThreefish256:
		return dkcThreefish256Decrypt(&p->ctx.tf256, dest, destsize, src, srcsize);
	case edkcThreefish512:
		return dkcThreefish512Decrypt(&p->ctx.tf512, dest, destsize, src, srcsize);
	case edkcThreefish1024:
		return dkcThreefish1024Decrypt(&p->ctx.tf1024, dest, destsize, src, srcsize);
	default:
		return edk_FAILED;
	}
}

DKC_EXTERN void WINAPI dkcThreefishEncryptNoDest(
	DKC_THREEFISH *p,
	unsigned char *dest_and_src, size_t size)
{
	dkcThreefishEncrypt(p, dest_and_src, size, dest_and_src, size);
}

DKC_EXTERN void WINAPI dkcThreefishDecryptNoDest(
	DKC_THREEFISH *p,
	unsigned char *dest_and_src, size_t size)
{
	dkcThreefishDecrypt(p, dest_and_src, size, dest_and_src, size);
}

DKC_EXTERN int WINAPI dkcThreefishSetTweak(
	DKC_THREEFISH *p, const unsigned char *tweak)
{
	if(p == NULL) return edk_FAILED;

	switch(p->variant){
	case edkcThreefish256:
		return dkcThreefish256SetTweak(&p->ctx.tf256, tweak);
	case edkcThreefish512:
		return dkcThreefish512SetTweak(&p->ctx.tf512, tweak);
	case edkcThreefish1024:
		return dkcThreefish1024SetTweak(&p->ctx.tf1024, tweak);
	default:
		return edk_FAILED;
	}
}

DKC_EXTERN size_t WINAPI dkcThreefishGetBlockSize(DKC_THREEFISH *p)
{
	if(p == NULL) return 0;

	switch(p->variant){
	case edkcThreefish256:
		return dkcd_THREEFISH256_BLOCK_SIZE;
	case edkcThreefish512:
		return dkcd_THREEFISH512_BLOCK_SIZE;
	case edkcThreefish1024:
		return dkcd_THREEFISH1024_BLOCK_SIZE;
	default:
		return 0;
	}
}
