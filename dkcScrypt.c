/*! @file
	@brief scrypt パスワードハッシュ 実装
	@author d.Kingyo
	@note
	RFC 7914 に基づく実装。
	内部でPBKDF2-HMAC-SHA256とSalsa20/8を使用。
*/

#include "dkcScrypt.h"
#include "dkcHMAC.h"
#include "dkcSHA256.h"
#include <string.h>

/* ====================================================================
 * PBKDF2-HMAC-SHA256
 * ==================================================================== */

#define SCRYPT_SHA256_BLOCK_SIZE  64
#define SCRYPT_SHA256_HASH_SIZE   32

static void pbkdf2_hmac_sha256(
	const unsigned char *pwd, size_t pwdlen,
	const unsigned char *salt, size_t saltlen,
	uint32 iterations,
	unsigned char *output, size_t outlen)
{
	size_t pos;
	uint32 block_num;
	unsigned char U[SCRYPT_SHA256_HASH_SIZE];
	unsigned char T[SCRYPT_SHA256_HASH_SIZE];
	unsigned char salt_block[4];
	size_t copy_len;
	uint32 iter;
	int j;

	pos = 0;
	block_num = 1;

	while(pos < outlen){
		/* U_1 = HMAC-SHA256(pwd, salt || INT_32_BE(block_num)) */
		salt_block[0] = (unsigned char)(block_num >> 24);
		salt_block[1] = (unsigned char)(block_num >> 16);
		salt_block[2] = (unsigned char)(block_num >> 8);
		salt_block[3] = (unsigned char)(block_num);

		{
			DKC_HMAC *hmac = dkcAllocHMAC(edk_SecureHash_SHA256);
			if(hmac == NULL) return;

			dkcHMACInit(hmac, pwd, pwdlen);
			dkcHMACLoad(hmac, salt, (DWORD)saltlen);
			dkcHMACLoad(hmac, salt_block, 4);
			dkcHMACFinal(hmac);
			dkcHMACDigest(hmac, U, sizeof(U));
			dkcFreeHMAC(&hmac);
		}

		memcpy(T, U, SCRYPT_SHA256_HASH_SIZE);

		for(iter = 1; iter < iterations; iter++){
			DKC_HMAC *hmac = dkcAllocHMAC(edk_SecureHash_SHA256);
			if(hmac == NULL) return;

			dkcHMACInit(hmac, pwd, pwdlen);
			dkcHMACLoad(hmac, U, SCRYPT_SHA256_HASH_SIZE);
			dkcHMACFinal(hmac);
			dkcHMACDigest(hmac, U, sizeof(U));
			dkcFreeHMAC(&hmac);

			for(j = 0; j < SCRYPT_SHA256_HASH_SIZE; j++){
				T[j] ^= U[j];
			}
		}

		copy_len = outlen - pos;
		if(copy_len > SCRYPT_SHA256_HASH_SIZE) copy_len = SCRYPT_SHA256_HASH_SIZE;
		memcpy(output + pos, T, copy_len);
		pos += copy_len;
		block_num++;
	}
}

/* ====================================================================
 * Salsa20/8 core
 * ==================================================================== */

#define SALSA_ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

static DKC_INLINE uint32 scrypt_load32_le(const unsigned char *p)
{
	return ((uint32)p[0]) | ((uint32)p[1] << 8) |
	       ((uint32)p[2] << 16) | ((uint32)p[3] << 24);
}

static DKC_INLINE void scrypt_store32_le(unsigned char *p, uint32 v)
{
	p[0] = (unsigned char)(v);
	p[1] = (unsigned char)(v >> 8);
	p[2] = (unsigned char)(v >> 16);
	p[3] = (unsigned char)(v >> 24);
}

static void salsa20_8_core(unsigned char *B)
{
	uint32 x[16];
	uint32 orig[16];
	int i;

	for(i = 0; i < 16; i++){
		x[i] = scrypt_load32_le(B + i * 4);
		orig[i] = x[i];
	}

	/* 8 rounds (4 double-rounds) */
	for(i = 0; i < 4; i++){
		x[ 4] ^= SALSA_ROTL32(x[ 0] + x[12],  7);
		x[ 8] ^= SALSA_ROTL32(x[ 4] + x[ 0],  9);
		x[12] ^= SALSA_ROTL32(x[ 8] + x[ 4], 13);
		x[ 0] ^= SALSA_ROTL32(x[12] + x[ 8], 18);
		x[ 9] ^= SALSA_ROTL32(x[ 5] + x[ 1],  7);
		x[13] ^= SALSA_ROTL32(x[ 9] + x[ 5],  9);
		x[ 1] ^= SALSA_ROTL32(x[13] + x[ 9], 13);
		x[ 5] ^= SALSA_ROTL32(x[ 1] + x[13], 18);
		x[14] ^= SALSA_ROTL32(x[10] + x[ 6],  7);
		x[ 2] ^= SALSA_ROTL32(x[14] + x[10],  9);
		x[ 6] ^= SALSA_ROTL32(x[ 2] + x[14], 13);
		x[10] ^= SALSA_ROTL32(x[ 6] + x[ 2], 18);
		x[ 3] ^= SALSA_ROTL32(x[15] + x[11],  7);
		x[ 7] ^= SALSA_ROTL32(x[ 3] + x[15],  9);
		x[11] ^= SALSA_ROTL32(x[ 7] + x[ 3], 13);
		x[15] ^= SALSA_ROTL32(x[11] + x[ 7], 18);

		x[ 1] ^= SALSA_ROTL32(x[ 0] + x[ 3],  7);
		x[ 2] ^= SALSA_ROTL32(x[ 1] + x[ 0],  9);
		x[ 3] ^= SALSA_ROTL32(x[ 2] + x[ 1], 13);
		x[ 0] ^= SALSA_ROTL32(x[ 3] + x[ 2], 18);
		x[ 6] ^= SALSA_ROTL32(x[ 5] + x[ 4],  7);
		x[ 7] ^= SALSA_ROTL32(x[ 6] + x[ 5],  9);
		x[ 4] ^= SALSA_ROTL32(x[ 7] + x[ 6], 13);
		x[ 5] ^= SALSA_ROTL32(x[ 4] + x[ 7], 18);
		x[11] ^= SALSA_ROTL32(x[10] + x[ 9],  7);
		x[ 8] ^= SALSA_ROTL32(x[11] + x[10],  9);
		x[ 9] ^= SALSA_ROTL32(x[ 8] + x[11], 13);
		x[10] ^= SALSA_ROTL32(x[ 9] + x[ 8], 18);
		x[12] ^= SALSA_ROTL32(x[15] + x[14],  7);
		x[13] ^= SALSA_ROTL32(x[12] + x[15],  9);
		x[14] ^= SALSA_ROTL32(x[13] + x[12], 13);
		x[15] ^= SALSA_ROTL32(x[14] + x[13], 18);
	}

	for(i = 0; i < 16; i++){
		scrypt_store32_le(B + i * 4, x[i] + orig[i]);
	}
}

/* ====================================================================
 * scryptBlockMix
 * ==================================================================== */

static void scrypt_block_mix(unsigned char *B, unsigned char *Y, uint32 r)
{
	unsigned char X[64];
	uint32 block_count;
	uint32 i, j;

	block_count = 2 * r;

	/* X = B[2r-1] (last 64-byte block) */
	memcpy(X, B + (block_count - 1) * 64, 64);

	for(i = 0; i < block_count; i++){
		/* X = X xor B[i] */
		for(j = 0; j < 64; j++){
			X[j] ^= B[i * 64 + j];
		}

		/* X = Salsa20/8(X) */
		salsa20_8_core(X);

		/* Y[i] = X */
		memcpy(Y + i * 64, X, 64);
	}

	/* B' = (Y[0], Y[2], ..., Y[2r-2], Y[1], Y[3], ..., Y[2r-1]) */
	for(i = 0; i < r; i++){
		memcpy(B + i * 64, Y + (2 * i) * 64, 64);
	}
	for(i = 0; i < r; i++){
		memcpy(B + (r + i) * 64, Y + (2 * i + 1) * 64, 64);
	}
}

/* ====================================================================
 * scryptROMix
 * ==================================================================== */

static uint32 scrypt_integerify(const unsigned char *B, uint32 r)
{
	/* Integerify = B'[2r-1] mod N (first 4 bytes of last block, LE) */
	size_t offset;
	offset = (2 * r - 1) * 64;
	return scrypt_load32_le(B + offset);
}

static void scrypt_romix(unsigned char *B, uint32 r, uint32 N)
{
	size_t block_size;
	unsigned char *V;
	unsigned char *Y;
	uint32 i, j, k;

	block_size = (size_t)128 * r;

	V = (unsigned char *)dkcAllocateFast(block_size * N);
	if(V == NULL) return;
	Y = (unsigned char *)dkcAllocateFast(block_size);
	if(Y == NULL){
		dkcFree((void **)&V);
		return;
	}

	/* Step 1: V[i] = X, X = BlockMix(X) */
	for(i = 0; i < N; i++){
		memcpy(V + (size_t)i * block_size, B, block_size);
		scrypt_block_mix(B, Y, r);
	}

	/* Step 2: randomly access V */
	for(i = 0; i < N; i++){
		j = scrypt_integerify(B, r) & (N - 1);
		for(k = 0; k < block_size; k++){
			B[k] ^= V[(size_t)j * block_size + k];
		}
		scrypt_block_mix(B, Y, r);
	}

	dkcFree((void **)&Y);
	dkcFree((void **)&V);
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcScrypt(
	const unsigned char *pwd, size_t pwdlen,
	const unsigned char *salt, size_t saltlen,
	uint32 N, uint32 r, uint32 p,
	unsigned char *output, size_t outlen)
{
	unsigned char *B;
	size_t block_size;
	uint32 i;

	if(pwd == NULL || salt == NULL || output == NULL) return edk_FAILED;
	if(N == 0 || r == 0 || p == 0) return edk_FAILED;
	/* N must be power of 2 */
	if((N & (N - 1)) != 0) return edk_FAILED;

	block_size = (size_t)128 * r;

	/* B = PBKDF2-HMAC-SHA256(pwd, salt, 1, p * 128 * r) */
	B = (unsigned char *)dkcAllocateFill(block_size * p, 0);
	if(B == NULL) return edk_FAILED;

	pbkdf2_hmac_sha256(pwd, pwdlen, salt, saltlen, 1, B, block_size * p);

	/* ROMix each block */
	for(i = 0; i < p; i++){
		scrypt_romix(B + (size_t)i * block_size, r, N);
	}

	/* output = PBKDF2-HMAC-SHA256(pwd, B, 1, outlen) */
	pbkdf2_hmac_sha256(pwd, pwdlen, B, block_size * p, 1, output, outlen);

	memset(B, 0, block_size * p);
	dkcFree((void **)&B);

	return edk_SUCCEEDED;
}
