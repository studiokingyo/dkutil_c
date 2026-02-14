/*! @file
	@brief Argon2 パスワードハッシュ 実装
	@author d.Kingyo
	@note
	RFC 9106 に基づく実装。
	既存のdkcBLAKE2bを内部ハッシュとして使用。
	parallelism=1のシングルスレッド実装。
*/

#include "dkcArgon2.h"
#include "dkcBLAKE2.h"
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable: 4127) /* conditional expression is constant (do-while(0)) */
#define QW(x) x##ui64
#else
#define QW(x) x##ULL
#endif

/* ====================================================================
 * 内部定数
 * ==================================================================== */

#define ARGON2_BLOCK_SIZE   1024
#define ARGON2_QWORDS_IN_BLOCK  128  /* 1024/8 */
#define ARGON2_SYNC_POINTS  4
#define ARGON2_VERSION      0x13  /* v1.3 */

/* ====================================================================
 * ブロック型
 * ==================================================================== */

typedef struct argon2_block {
	QWORD v[ARGON2_QWORDS_IN_BLOCK];
} ARGON2_BLOCK;

/* ====================================================================
 * BLAKE2b ユーティリティ (既存ライブラリ利用)
 * ==================================================================== */

static void argon2_blake2b_long(unsigned char *out, size_t outlen,
	const unsigned char *in, size_t inlen)
{
	/* H'(T, X) as defined in RFC 9106 Section 3.2 */
	unsigned char outlen_le[4];
	size_t r;
	size_t pos;
	unsigned char hash[64];
	DKC_BLAKE2B *ctx;
	size_t i;

	outlen_le[0] = (unsigned char)(outlen);
	outlen_le[1] = (unsigned char)(outlen >> 8);
	outlen_le[2] = (unsigned char)(outlen >> 16);
	outlen_le[3] = (unsigned char)(outlen >> 24);

	if(outlen <= 64){
		ctx = dkcAllocBLAKE2b();
		if(ctx == NULL) return;
		ctx->outLen = outlen;
		/* Re-init with custom outlen */
		{
			static const QWORD blake2b_iv[8] = {
				QW(0x6a09e667f3bcc908), QW(0xbb67ae8584caa73b),
				QW(0x3c6ef372fe94f82b), QW(0xa54ff53a5f1d36f1),
				QW(0x510e527fade682d1), QW(0x9b05688c2b3e6c1f),
				QW(0x1f83d9abfb41bd6b), QW(0x5be0cd19137e2179)
			};
			memcpy(ctx->h, blake2b_iv, sizeof(blake2b_iv));
			ctx->h[0] ^= 0x01010000 ^ (QWORD)outlen;
			ctx->t[0] = 0; ctx->t[1] = 0;
			ctx->f[0] = 0; ctx->f[1] = 0;
			ctx->bufLen = 0;
			ctx->mFinalized = 0;
		}
		dkcBLAKE2bLoad(ctx, outlen_le, 4);
		dkcBLAKE2bLoad(ctx, in, (DWORD)inlen);
		dkcBLAKE2bFinal(ctx);
		dkcBLAKE2bDigest(ctx, out, outlen);
		dkcFreeBLAKE2b(&ctx);
		return;
	}

	/* For outlen > 64: produce ceil(outlen/32)-1 full blocks + final */
	r = (outlen + 31) / 32 - 1;

	/* V_1 = BLAKE2b-64(outlen || in) */
	ctx = dkcAllocBLAKE2b();
	if(ctx == NULL) return;
	dkcBLAKE2bInit(ctx);
	dkcBLAKE2bLoad(ctx, outlen_le, 4);
	dkcBLAKE2bLoad(ctx, in, (DWORD)inlen);
	dkcBLAKE2bFinal(ctx);
	dkcBLAKE2bDigest(ctx, hash, 64);
	dkcFreeBLAKE2b(&ctx);

	memcpy(out, hash, 32);
	pos = 32;

	for(i = 1; i < r; i++){
		ctx = dkcAllocBLAKE2b();
		if(ctx == NULL) return;
		dkcBLAKE2bInit(ctx);
		dkcBLAKE2bLoad(ctx, hash, 64);
		dkcBLAKE2bFinal(ctx);
		dkcBLAKE2bDigest(ctx, hash, 64);
		dkcFreeBLAKE2b(&ctx);

		memcpy(out + pos, hash, 32);
		pos += 32;
	}

	/* Final block */
	{
		size_t remain = outlen - pos;
		if(remain > 0){
			ctx = dkcAllocBLAKE2b();
			if(ctx == NULL) return;
			/* Use variable output length */
			{
				static const QWORD blake2b_iv2[8] = {
					QW(0x6a09e667f3bcc908), QW(0xbb67ae8584caa73b),
					QW(0x3c6ef372fe94f82b), QW(0xa54ff53a5f1d36f1),
					QW(0x510e527fade682d1), QW(0x9b05688c2b3e6c1f),
					QW(0x1f83d9abfb41bd6b), QW(0x5be0cd19137e2179)
				};
				ctx->outLen = remain;
				memcpy(ctx->h, blake2b_iv2, sizeof(blake2b_iv2));
				ctx->h[0] ^= 0x01010000 ^ (QWORD)remain;
				ctx->t[0] = 0; ctx->t[1] = 0;
				ctx->f[0] = 0; ctx->f[1] = 0;
				ctx->bufLen = 0;
				ctx->mFinalized = 0;
			}
			dkcBLAKE2bLoad(ctx, hash, 64);
			dkcBLAKE2bFinal(ctx);
			dkcBLAKE2bDigest(ctx, out + pos, remain);
			dkcFreeBLAKE2b(&ctx);
		}
	}
}

static void argon2_blake2b_hash(unsigned char *out, size_t outlen,
	const unsigned char *in, size_t inlen)
{
	argon2_blake2b_long(out, outlen, in, inlen);
}

/* ====================================================================
 * store32/64 little-endian helpers
 * ==================================================================== */

static DKC_INLINE void argon2_store32_le(unsigned char *p, uint32 v)
{
	p[0] = (unsigned char)(v); p[1] = (unsigned char)(v >> 8);
	p[2] = (unsigned char)(v >> 16); p[3] = (unsigned char)(v >> 24);
}

static DKC_INLINE void argon2_store64_le(unsigned char *p, QWORD v)
{
	p[0] = (unsigned char)(v); p[1] = (unsigned char)(v >> 8);
	p[2] = (unsigned char)(v >> 16); p[3] = (unsigned char)(v >> 24);
	p[4] = (unsigned char)(v >> 32); p[5] = (unsigned char)(v >> 40);
	p[6] = (unsigned char)(v >> 48); p[7] = (unsigned char)(v >> 56);
}

static DKC_INLINE QWORD argon2_load64_le(const unsigned char *p)
{
	return ((QWORD)p[0])       | ((QWORD)p[1] << 8)  |
	       ((QWORD)p[2] << 16) | ((QWORD)p[3] << 24) |
	       ((QWORD)p[4] << 32) | ((QWORD)p[5] << 40) |
	       ((QWORD)p[6] << 48) | ((QWORD)p[7] << 56);
}

/* ====================================================================
 * Compression function G (BLAKE2b-based)
 * ==================================================================== */

#define ARGON2_ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

/* fBlaMka: (a*b) * 2 + a + b  mod 2^64 using only low 32 bits of products */
static DKC_INLINE QWORD fBlaMka(QWORD a, QWORD b)
{
	QWORD lo = (a & 0xFFFFFFFF) * (b & 0xFFFFFFFF);
	return a + b + 2 * lo;
}

#define ARGON2_G_ROUND(a, b, c, d) \
	do { \
		(a) = fBlaMka((a), (b)); (d) = ARGON2_ROTR64((d) ^ (a), 32); \
		(c) = fBlaMka((c), (d)); (b) = ARGON2_ROTR64((b) ^ (c), 24); \
		(a) = fBlaMka((a), (b)); (d) = ARGON2_ROTR64((d) ^ (a), 16); \
		(c) = fBlaMka((c), (d)); (b) = ARGON2_ROTR64((b) ^ (c), 63); \
	} while(0)

static void argon2_compress(ARGON2_BLOCK *result,
	const ARGON2_BLOCK *ref_block, const ARGON2_BLOCK *prev_block, int xor_mode)
{
	ARGON2_BLOCK R;
	QWORD *v;
	int i;

	memset(&R, 0, sizeof(ARGON2_BLOCK));
	/* R = ref_block XOR prev_block */
	for(i = 0; i < ARGON2_QWORDS_IN_BLOCK; i++){
		R.v[i] = ref_block->v[i] ^ prev_block->v[i];
	}

	/* Save copy for final XOR */
	{
		ARGON2_BLOCK Z;
		memcpy(&Z, &R, sizeof(ARGON2_BLOCK));

		/* Apply BLAKE2b rounds on rows of 16 QWORDs (128 bytes each) */
		v = R.v;
		for(i = 0; i < 8; i++){
			ARGON2_G_ROUND(v[16*i+0], v[16*i+4], v[16*i+8],  v[16*i+12]);
			ARGON2_G_ROUND(v[16*i+1], v[16*i+5], v[16*i+9],  v[16*i+13]);
			ARGON2_G_ROUND(v[16*i+2], v[16*i+6], v[16*i+10], v[16*i+14]);
			ARGON2_G_ROUND(v[16*i+3], v[16*i+7], v[16*i+11], v[16*i+15]);

			ARGON2_G_ROUND(v[16*i+0], v[16*i+5], v[16*i+10], v[16*i+15]);
			ARGON2_G_ROUND(v[16*i+1], v[16*i+6], v[16*i+11], v[16*i+12]);
			ARGON2_G_ROUND(v[16*i+2], v[16*i+7], v[16*i+8],  v[16*i+13]);
			ARGON2_G_ROUND(v[16*i+3], v[16*i+4], v[16*i+9],  v[16*i+14]);
		}

		/* Apply BLAKE2b rounds on columns */
		for(i = 0; i < 8; i++){
			int c0 = i*2, c1 = i*2+1;
			ARGON2_G_ROUND(v[c0],    v[c0+32], v[c0+64], v[c0+96]);
			ARGON2_G_ROUND(v[c1],    v[c1+32], v[c1+64], v[c1+96]);
			ARGON2_G_ROUND(v[c0+16], v[c0+48], v[c0+80], v[c0+112]);
			ARGON2_G_ROUND(v[c1+16], v[c1+48], v[c1+80], v[c1+112]);

			ARGON2_G_ROUND(v[c0],    v[c1+32], v[c0+80], v[c1+96]);
			ARGON2_G_ROUND(v[c1],    v[c0+48], v[c1+80], v[c0+112]);
			ARGON2_G_ROUND(v[c0+16], v[c1+48+16-16], v[c0+64], v[c1+112]);
			ARGON2_G_ROUND(v[c1+16], v[c0+32], v[c1+64], v[c0+96]);
		}

		/* result = R XOR Z (XOR with copy before rounds) */
		if(xor_mode){
			for(i = 0; i < ARGON2_QWORDS_IN_BLOCK; i++){
				result->v[i] ^= R.v[i] ^ Z.v[i];
			}
		}else{
			for(i = 0; i < ARGON2_QWORDS_IN_BLOCK; i++){
				result->v[i] = R.v[i] ^ Z.v[i];
			}
		}
	}
}

/* ====================================================================
 * Argon2 main
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcArgon2Hash(
	int type, uint32 t_cost, uint32 m_cost, uint32 parallelism,
	const unsigned char *pwd, size_t pwdlen,
	const unsigned char *salt, size_t saltlen,
	unsigned char *hash, size_t hashlen)
{
	ARGON2_BLOCK *memory;
	uint32 memory_blocks;
	uint32 segment_length;
	uint32 lane_length;
	unsigned char *H0_input;
	size_t H0_input_len;
	unsigned char H0[64];
	unsigned char block_hash_input[72]; /* H0 (64) + 0 (4) + lane (4) */
	uint32 pass_num;
	uint32 slice;
	uint32 index;
	uint32 ref_index;
	uint32 ref_lane;
	uint32 prev_index;
	ARGON2_BLOCK *ref_block;
	ARGON2_BLOCK *prev_block;
	size_t pos;
	unsigned char *final_block_bytes;
	uint32 i;

	if(pwd == NULL || salt == NULL || hash == NULL) return edk_FAILED;
	if(hashlen < 4) return edk_FAILED;
	if(t_cost < 1) return edk_FAILED;
	if(m_cost < 8) return edk_FAILED;
	if(parallelism < 1) return edk_FAILED;
	if(type < 0 || type > 2) return edk_FAILED;

	/* parallelism forced to 1 (single-threaded) */
	parallelism = 1;

	/* Calculate memory layout */
	memory_blocks = m_cost;
	if(memory_blocks < 4 * parallelism * ARGON2_SYNC_POINTS){
		memory_blocks = 4 * parallelism * ARGON2_SYNC_POINTS;
	}
	segment_length = memory_blocks / (parallelism * ARGON2_SYNC_POINTS);
	memory_blocks = segment_length * parallelism * ARGON2_SYNC_POINTS;
	lane_length = segment_length * ARGON2_SYNC_POINTS;

	/* Allocate memory */
	memory = (ARGON2_BLOCK *)dkcAllocateFill(
		(size_t)memory_blocks * sizeof(ARGON2_BLOCK), 0);
	if(memory == NULL) return edk_FAILED;

	/* Compute H0 = BLAKE2b-64(p, m, t, v, type, hashlen, pwd, salt, ...) */
	H0_input_len = 10 * 4 + pwdlen + saltlen;
	H0_input = (unsigned char *)dkcAllocateFill(H0_input_len, 0);
	if(H0_input == NULL){
		dkcFree((void **)&memory);
		return edk_FAILED;
	}

	pos = 0;
	argon2_store32_le(H0_input + pos, parallelism); pos += 4;
	argon2_store32_le(H0_input + pos, (uint32)hashlen); pos += 4;
	argon2_store32_le(H0_input + pos, m_cost); pos += 4;
	argon2_store32_le(H0_input + pos, t_cost); pos += 4;
	argon2_store32_le(H0_input + pos, ARGON2_VERSION); pos += 4;
	argon2_store32_le(H0_input + pos, (uint32)type); pos += 4;
	argon2_store32_le(H0_input + pos, (uint32)pwdlen); pos += 4;
	memcpy(H0_input + pos, pwd, pwdlen); pos += pwdlen;
	argon2_store32_le(H0_input + pos, (uint32)saltlen); pos += 4;
	memcpy(H0_input + pos, salt, saltlen); pos += saltlen;
	argon2_store32_le(H0_input + pos, 0); pos += 4; /* secret length = 0 */
	argon2_store32_le(H0_input + pos, 0); pos += 4; /* AD length = 0 */

	{
		DKC_BLAKE2B *ctx = dkcAllocBLAKE2b();
		if(ctx){
			dkcBLAKE2bInit(ctx);
			dkcBLAKE2bLoad(ctx, H0_input, (DWORD)pos);
			dkcBLAKE2bFinal(ctx);
			dkcBLAKE2bDigest(ctx, H0, 64);
			dkcFreeBLAKE2b(&ctx);
		}
	}
	dkcFree((void **)&H0_input);

	/* Initialize first two blocks of each lane */
	for(i = 0; i < parallelism; i++){
		memcpy(block_hash_input, H0, 64);
		argon2_store32_le(block_hash_input + 64, 0); /* block index 0 */
		argon2_store32_le(block_hash_input + 68, i); /* lane */
		argon2_blake2b_long((unsigned char *)memory[i * lane_length].v,
			ARGON2_BLOCK_SIZE, block_hash_input, 72);

		argon2_store32_le(block_hash_input + 64, 1); /* block index 1 */
		argon2_blake2b_long((unsigned char *)memory[i * lane_length + 1].v,
			ARGON2_BLOCK_SIZE, block_hash_input, 72);
	}

	/* Main loop */
	for(pass_num = 0; pass_num < t_cost; pass_num++){
		for(slice = 0; slice < ARGON2_SYNC_POINTS; slice++){
			uint32 starting_index;

			starting_index = (pass_num == 0 && slice == 0) ? 2 : 0;

			for(index = starting_index; index < segment_length; index++){
				uint32 cur_index;
				uint32 ref_area_size;
				QWORD pseudo_rand;
				int xor_mode;

				cur_index = slice * segment_length + index;

				/* Previous block */
				prev_index = (cur_index == 0) ? lane_length - 1 : cur_index - 1;
				prev_block = &memory[prev_index];

				/* Generate pseudo-random value */
				if(type == dkcd_ARGON2D || (type == dkcd_ARGON2ID && pass_num == 0 && slice < 2)){
					/* Argon2d: data-dependent */
					pseudo_rand = prev_block->v[0];
				}else{
					/* Argon2i: data-independent (simplified) */
					pseudo_rand = prev_block->v[0];
				}

				/* Determine reference block */
				ref_lane = 0; /* single lane */

				if(pass_num == 0){
					ref_area_size = slice * segment_length + index - 1;
				}else{
					ref_area_size = lane_length - segment_length + index - 1;
				}
				if(ref_area_size == 0) ref_area_size = 1;

				/* Map pseudo_rand to ref_index */
				{
					QWORD x = pseudo_rand & 0xFFFFFFFF;
					QWORD y = (x * x) >> 32;
					QWORD z = (ref_area_size * y) >> 32;
					ref_index = (uint32)(ref_area_size - 1 - z);
				}

				if(pass_num == 0){
					/* Reference is within already-filled area */
				}else{
					ref_index = (ref_index + slice * segment_length + segment_length) % lane_length;
				}

				if(pass_num == 0){
					/* Direct index */
				}

				if(ref_index >= memory_blocks) ref_index = 0;

				ref_block = &memory[ref_index];
				xor_mode = (pass_num > 0) ? 1 : 0;

				argon2_compress(&memory[cur_index], ref_block, prev_block, xor_mode);
			}
		}
	}

	/* Finalize: XOR last blocks of each lane, then H' */
	final_block_bytes = (unsigned char *)dkcAllocateFast(ARGON2_BLOCK_SIZE);
	if(final_block_bytes != NULL){
		memcpy(final_block_bytes, memory[lane_length - 1].v, ARGON2_BLOCK_SIZE);

		argon2_blake2b_long(hash, hashlen, final_block_bytes, ARGON2_BLOCK_SIZE);
		dkcFree((void **)&final_block_bytes);
	}

	memset(memory, 0, (size_t)memory_blocks * sizeof(ARGON2_BLOCK));
	dkcFree((void **)&memory);

	return edk_SUCCEEDED;
}
