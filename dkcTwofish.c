/*!
@file dkcTwofish.c
@brief Twofish block cipher algorithm (128/192/256-bit key, 128-bit block)
@author d-kingyo / Twofish algorithm by Bruce Schneier et al.
@see dkcTwofish.h
*/

#define DKUTIL_C_TWOFISH_C
#include "dkcTwofish.h"
#include "dkcStdio.h"

/* ------------------------------------------------------------ */
/* Twofish core algorithm                                       */
/* ------------------------------------------------------------ */

/* Rotate left / right macros */
#define TF_ROL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define TF_ROR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

/* q0 permutation nibble tables */
static const unsigned char Q0_T0[16] = {
	8,1,7,13,6,15,3,2,0,11,5,9,14,12,10,4
};
static const unsigned char Q0_T1[16] = {
	14,12,11,8,1,2,3,5,15,4,10,6,7,0,9,13
};
static const unsigned char Q0_T2[16] = {
	11,10,5,14,6,13,9,0,12,8,15,3,2,4,7,1
};
static const unsigned char Q0_T3[16] = {
	13,7,15,4,1,2,6,14,9,11,3,0,8,5,12,10
};

/* q1 permutation nibble tables */
static const unsigned char Q1_T0[16] = {
	2,8,11,13,15,7,6,14,3,1,9,4,0,10,12,5
};
static const unsigned char Q1_T1[16] = {
	1,14,2,11,4,12,3,7,6,13,10,5,15,9,0,8
};
static const unsigned char Q1_T2[16] = {
	4,12,7,5,1,6,9,10,0,14,13,8,2,11,3,15
};
static const unsigned char Q1_T3[16] = {
	11,9,5,1,12,3,13,14,6,4,7,15,2,0,8,10
};

/* Compute q0 permutation on a single byte */
static unsigned char tf_q0(unsigned char x)
{
	unsigned char a0, b0, a1, b1, a2, b2, a3, b3, a4, b4;

	a0 = (unsigned char)((x >> 4) & 0x0F);
	b0 = (unsigned char)(x & 0x0F);

	a1 = (unsigned char)(a0 ^ b0);
	b1 = (unsigned char)((a0 ^ ((b0 >> 1) | (b0 << 3)) ^ ((a0 << 3) & 0x0F)) & 0x0F);

	a2 = Q0_T0[a1];
	b2 = Q0_T1[b1];

	a3 = (unsigned char)(a2 ^ b2);
	b3 = (unsigned char)((a2 ^ ((b2 >> 1) | (b2 << 3)) ^ ((a2 << 3) & 0x0F)) & 0x0F);

	a4 = Q0_T2[a3];
	b4 = Q0_T3[b3];

	return (unsigned char)((b4 << 4) | a4);
}

/* Compute q1 permutation on a single byte */
static unsigned char tf_q1(unsigned char x)
{
	unsigned char a0, b0, a1, b1, a2, b2, a3, b3, a4, b4;

	a0 = (unsigned char)((x >> 4) & 0x0F);
	b0 = (unsigned char)(x & 0x0F);

	a1 = (unsigned char)(a0 ^ b0);
	b1 = (unsigned char)((a0 ^ ((b0 >> 1) | (b0 << 3)) ^ ((a0 << 3) & 0x0F)) & 0x0F);

	a2 = Q1_T0[a1];
	b2 = Q1_T1[b1];

	a3 = (unsigned char)(a2 ^ b2);
	b3 = (unsigned char)((a2 ^ ((b2 >> 1) | (b2 << 3)) ^ ((a2 << 3) & 0x0F)) & 0x0F);

	a4 = Q1_T2[a3];
	b4 = Q1_T3[b3];

	return (unsigned char)((b4 << 4) | a4);
}

/* GF(2^8) multiplication for MDS matrix
   Primitive polynomial: x^8 + x^6 + x^5 + x^3 + 1 = 0x169 */
static unsigned char tf_gf_mult(unsigned char a, unsigned char b)
{
	unsigned char result = 0;
	unsigned char aa = a;
	unsigned char bb = b;
	int i;

	for (i = 0; i < 8; i++) {
		if (bb & 1) {
			result = (unsigned char)(result ^ aa);
		}
		bb = (unsigned char)(bb >> 1);
		if (aa & 0x80) {
			aa = (unsigned char)((aa << 1) ^ 0x69);
		} else {
			aa = (unsigned char)(aa << 1);
		}
	}
	return result;
}

/* MDS matrix multiplication: input 4 bytes, output 1 uint32
   MDS matrix (row-major):
   01 EF 5B 5B
   5B EF EF 01
   EF 5B 01 EF
   EF 01 EF 5B
*/
static uint32 tf_mds_column_mult(unsigned char b0, unsigned char b1,
	unsigned char b2, unsigned char b3)
{
	uint32 r0, r1, r2, r3;

	r0 = (uint32)( tf_gf_mult(0x01, b0) ^ tf_gf_mult(0xEF, b1)
	             ^ tf_gf_mult(0x5B, b2) ^ tf_gf_mult(0x5B, b3) );
	r1 = (uint32)( tf_gf_mult(0x5B, b0) ^ tf_gf_mult(0xEF, b1)
	             ^ tf_gf_mult(0xEF, b2) ^ tf_gf_mult(0x01, b3) );
	r2 = (uint32)( tf_gf_mult(0xEF, b0) ^ tf_gf_mult(0x5B, b1)
	             ^ tf_gf_mult(0x01, b2) ^ tf_gf_mult(0xEF, b3) );
	r3 = (uint32)( tf_gf_mult(0xEF, b0) ^ tf_gf_mult(0x01, b1)
	             ^ tf_gf_mult(0xEF, b2) ^ tf_gf_mult(0x5B, b3) );

	return (r0) | (r1 << 8) | (r2 << 16) | (r3 << 24);
}

/* RS GF(2^8) multiplication for key schedule
   Primitive polynomial: x^8+x^6+x^3+x^2+1 = 0x14D */
static unsigned char tf_rs_gf_mult(unsigned char a, unsigned char b)
{
	unsigned char result = 0;
	unsigned char aa = a;
	unsigned char bb = b;
	int i;

	for (i = 0; i < 8; i++) {
		if (bb & 1) {
			result = (unsigned char)(result ^ aa);
		}
		bb = (unsigned char)(bb >> 1);
		if (aa & 0x80) {
			aa = (unsigned char)((aa << 1) ^ 0x4D);
		} else {
			aa = (unsigned char)(aa << 1);
		}
	}
	return result;
}

/* RS matrix (4x8) used in key schedule */
static const unsigned char RS[4][8] = {
	{0x01, 0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E},
	{0xA4, 0x56, 0x82, 0xF3, 0x1E, 0xC6, 0x68, 0xE5},
	{0x02, 0xA1, 0xFC, 0xC1, 0x47, 0xAE, 0x3D, 0x19},
	{0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E, 0x03}
};

/* Compute RS matrix * 8-byte vector -> 4-byte vector */
static void tf_rs_mult(const unsigned char *in8, unsigned char *out4)
{
	int i, j;
	for (i = 0; i < 4; i++) {
		out4[i] = 0;
		for (j = 0; j < 8; j++) {
			out4[i] = (unsigned char)(out4[i] ^ tf_rs_gf_mult(RS[i][j], in8[j]));
		}
	}
}

/* h function: applies q-permutations and MDS matrix
   X is the 32-bit input, L is array of key-derived 32-bit words,
   k is the number of key words (k=2 for 128-bit, k=3 for 192, k=4 for 256) */
static uint32 tf_h(uint32 X, const uint32 *L, int k)
{
	unsigned char y[4];

	y[0] = (unsigned char)(X);
	y[1] = (unsigned char)(X >> 8);
	y[2] = (unsigned char)(X >> 16);
	y[3] = (unsigned char)(X >> 24);

	if (k == 4) {
		y[0] = (unsigned char)(tf_q1(y[0]) ^ (unsigned char)(L[3]));
		y[1] = (unsigned char)(tf_q0(y[1]) ^ (unsigned char)(L[3] >> 8));
		y[2] = (unsigned char)(tf_q0(y[2]) ^ (unsigned char)(L[3] >> 16));
		y[3] = (unsigned char)(tf_q1(y[3]) ^ (unsigned char)(L[3] >> 24));
	}
	if (k >= 3) {
		y[0] = (unsigned char)(tf_q1(y[0]) ^ (unsigned char)(L[2]));
		y[1] = (unsigned char)(tf_q1(y[1]) ^ (unsigned char)(L[2] >> 8));
		y[2] = (unsigned char)(tf_q0(y[2]) ^ (unsigned char)(L[2] >> 16));
		y[3] = (unsigned char)(tf_q0(y[3]) ^ (unsigned char)(L[2] >> 24));
	}

	/* Always at least k=2 */
	y[0] = (unsigned char)(tf_q0((unsigned char)(tf_q0(y[0]) ^ (unsigned char)(L[1]))) ^ (unsigned char)(L[0]));
	y[1] = (unsigned char)(tf_q0((unsigned char)(tf_q1(y[1]) ^ (unsigned char)(L[1] >> 8))) ^ (unsigned char)(L[0] >> 8));
	y[2] = (unsigned char)(tf_q1((unsigned char)(tf_q0(y[2]) ^ (unsigned char)(L[1] >> 16))) ^ (unsigned char)(L[0] >> 16));
	y[3] = (unsigned char)(tf_q1((unsigned char)(tf_q1(y[3]) ^ (unsigned char)(L[1] >> 24))) ^ (unsigned char)(L[0] >> 24));

	return tf_mds_column_mult(y[0], y[1], y[2], y[3]);
}

/* Load a 32-bit little-endian word from byte array */
static uint32 tf_le32(const unsigned char *b)
{
	return (uint32)b[0] | ((uint32)b[1] << 8) |
	       ((uint32)b[2] << 16) | ((uint32)b[3] << 24);
}

/* Store a 32-bit little-endian word to byte array */
static void tf_st_le32(unsigned char *b, uint32 v)
{
	b[0] = (unsigned char)(v);
	b[1] = (unsigned char)(v >> 8);
	b[2] = (unsigned char)(v >> 16);
	b[3] = (unsigned char)(v >> 24);
}

/* Key schedule: generate subkeys K[0..39] and S-box tables S[0..3][0..255]
   S[col][x] stores the full uint32 MDS result for input byte x at
   position col (other byte positions zero), enabling fast g function
   via XOR of four table lookups. */
static void Twofish_KeySchedule(DKC_TWOFISH *ctx,
	const unsigned char *key, int keyLen)
{
	int k;
	int i, j;
	unsigned char Me[32];
	unsigned char Mo[32];
	uint32 Me32[4];
	uint32 Mo32[4];
	uint32 S_vec[4];
	unsigned char s_bytes[4];
	uint32 rho;
	uint32 A, B;

	ctx->keyLen = keyLen;
	k = keyLen / 8;

	memset(Me, 0, sizeof(Me));
	memset(Mo, 0, sizeof(Mo));
	for (i = 0; i < k; i++) {
		for (j = 0; j < 4; j++) {
			Me[i * 4 + j] = key[i * 8 + j * 2];
			Mo[i * 4 + j] = key[i * 8 + j * 2 + 1];
		}
	}

	for (i = 0; i < k; i++) {
		Me32[i] = tf_le32(Me + i * 4);
		Mo32[i] = tf_le32(Mo + i * 4);
	}

	/* Compute S vector using RS matrix (stored in reverse order) */
	for (i = 0; i < k; i++) {
		tf_rs_mult(key + i * 8, s_bytes);
		S_vec[k - 1 - i] = tf_le32(s_bytes);
	}

	/* Compute subkeys K[0..39] using h function and PHT */
	rho = 0x01010101UL;
	for (i = 0; i < 20; i++) {
		A = tf_h(rho * (uint32)(2 * i), Me32, k);
		B = tf_h(rho * (uint32)(2 * i + 1), Mo32, k);
		B = TF_ROL(B, 8);
		ctx->K[2 * i] = A + B;
		ctx->K[2 * i + 1] = TF_ROL(A + 2 * B, 9);
	}

	/* Precompute S-boxes: S[col][x] = full uint32 MDS result */
	for (i = 0; i < 256; i++) {
		unsigned char b[4];

		/* Column 0 */
		b[0] = (unsigned char)i;
		if (k == 4) b[0] = (unsigned char)(tf_q1(b[0]) ^ (unsigned char)(S_vec[3]));
		if (k >= 3) b[0] = (unsigned char)(tf_q1(b[0]) ^ (unsigned char)(S_vec[2]));
		b[0] = (unsigned char)(tf_q0((unsigned char)(tf_q0(b[0]) ^ (unsigned char)(S_vec[1]))) ^ (unsigned char)(S_vec[0]));

		/* Column 1 */
		b[1] = (unsigned char)i;
		if (k == 4) b[1] = (unsigned char)(tf_q0(b[1]) ^ (unsigned char)(S_vec[3] >> 8));
		if (k >= 3) b[1] = (unsigned char)(tf_q1(b[1]) ^ (unsigned char)(S_vec[2] >> 8));
		b[1] = (unsigned char)(tf_q0((unsigned char)(tf_q1(b[1]) ^ (unsigned char)(S_vec[1] >> 8))) ^ (unsigned char)(S_vec[0] >> 8));

		/* Column 2 */
		b[2] = (unsigned char)i;
		if (k == 4) b[2] = (unsigned char)(tf_q0(b[2]) ^ (unsigned char)(S_vec[3] >> 16));
		if (k >= 3) b[2] = (unsigned char)(tf_q0(b[2]) ^ (unsigned char)(S_vec[2] >> 16));
		b[2] = (unsigned char)(tf_q1((unsigned char)(tf_q0(b[2]) ^ (unsigned char)(S_vec[1] >> 16))) ^ (unsigned char)(S_vec[0] >> 16));

		/* Column 3 */
		b[3] = (unsigned char)i;
		if (k == 4) b[3] = (unsigned char)(tf_q1(b[3]) ^ (unsigned char)(S_vec[3] >> 24));
		if (k >= 3) b[3] = (unsigned char)(tf_q0(b[3]) ^ (unsigned char)(S_vec[2] >> 24));
		b[3] = (unsigned char)(tf_q1((unsigned char)(tf_q1(b[3]) ^ (unsigned char)(S_vec[1] >> 24))) ^ (unsigned char)(S_vec[0] >> 24));

		/* Store per-column MDS results */
		ctx->S[0][i] = tf_mds_column_mult(b[0], 0, 0, 0);
		ctx->S[1][i] = tf_mds_column_mult(0, b[1], 0, 0);
		ctx->S[2][i] = tf_mds_column_mult(0, 0, b[2], 0);
		ctx->S[3][i] = tf_mds_column_mult(0, 0, 0, b[3]);
	}
}

/* g function: 4 S-box lookups XORed together */
static uint32 tf_g(const DKC_TWOFISH *ctx, uint32 X)
{
	return ctx->S[0][(unsigned char)(X)] ^
	       ctx->S[1][(unsigned char)(X >> 8)] ^
	       ctx->S[2][(unsigned char)(X >> 16)] ^
	       ctx->S[3][(unsigned char)(X >> 24)];
}

/* Encrypt a single 16-byte block */
static void Twofish_EncryptBlock(const DKC_TWOFISH *ctx,
	const unsigned char *in, unsigned char *out)
{
	uint32 R[4];
	uint32 T0, T1;
	int r;

	/* Input whitening */
	R[0] = tf_le32(in + 0) ^ ctx->K[0];
	R[1] = tf_le32(in + 4) ^ ctx->K[1];
	R[2] = tf_le32(in + 8) ^ ctx->K[2];
	R[3] = tf_le32(in + 12) ^ ctx->K[3];

	/* 16 Feistel rounds (processed 2 at a time) */
	for (r = 0; r < dkcd_TWOFISH_ROUNDS; r += 2) {
		T0 = tf_g(ctx, R[0]);
		T1 = tf_g(ctx, TF_ROL(R[1], 8));
		T0 += T1;
		T1 += T0;
		R[2] = TF_ROR(R[2] ^ (T0 + ctx->K[2 * r + 8]), 1);
		R[3] = TF_ROL(R[3], 1) ^ (T1 + ctx->K[2 * r + 9]);

		T0 = tf_g(ctx, R[2]);
		T1 = tf_g(ctx, TF_ROL(R[3], 8));
		T0 += T1;
		T1 += T0;
		R[0] = TF_ROR(R[0] ^ (T0 + ctx->K[2 * r + 10]), 1);
		R[1] = TF_ROL(R[1], 1) ^ (T1 + ctx->K[2 * r + 11]);
	}

	/* Output whitening (undo last swap) */
	tf_st_le32(out + 0, R[2] ^ ctx->K[4]);
	tf_st_le32(out + 4, R[3] ^ ctx->K[5]);
	tf_st_le32(out + 8, R[0] ^ ctx->K[6]);
	tf_st_le32(out + 12, R[1] ^ ctx->K[7]);
}

/* Decrypt a single 16-byte block */
static void Twofish_DecryptBlock(const DKC_TWOFISH *ctx,
	const unsigned char *in, unsigned char *out)
{
	uint32 R[4];
	uint32 T0, T1;
	int r;

	/* Undo output whitening */
	R[2] = tf_le32(in + 0) ^ ctx->K[4];
	R[3] = tf_le32(in + 4) ^ ctx->K[5];
	R[0] = tf_le32(in + 8) ^ ctx->K[6];
	R[1] = tf_le32(in + 12) ^ ctx->K[7];

	/* 16 rounds in reverse */
	for (r = dkcd_TWOFISH_ROUNDS - 2; r >= 0; r -= 2) {
		T0 = tf_g(ctx, R[2]);
		T1 = tf_g(ctx, TF_ROL(R[3], 8));
		T0 += T1;
		T1 += T0;
		R[0] = TF_ROL(R[0], 1) ^ (T0 + ctx->K[2 * r + 10]);
		R[1] = TF_ROR(R[1] ^ (T1 + ctx->K[2 * r + 11]), 1);

		T0 = tf_g(ctx, R[0]);
		T1 = tf_g(ctx, TF_ROL(R[1], 8));
		T0 += T1;
		T1 += T0;
		R[2] = TF_ROL(R[2], 1) ^ (T0 + ctx->K[2 * r + 8]);
		R[3] = TF_ROR(R[3] ^ (T1 + ctx->K[2 * r + 9]), 1);
	}

	/* Undo input whitening */
	tf_st_le32(out + 0, R[0] ^ ctx->K[0]);
	tf_st_le32(out + 4, R[1] ^ ctx->K[1]);
	tf_st_le32(out + 8, R[2] ^ ctx->K[2]);
	tf_st_le32(out + 12, R[3] ^ ctx->K[3]);
}

/* ------------------------------------------------------------ */
/* Library wrapper functions                                    */
/* ------------------------------------------------------------ */

DKC_TWOFISH *WINAPI dkcAllocTwofish(
	const unsigned char *key, size_t keylen)
{
	DKC_TWOFISH *p;

	if(NULL == key || 0 == keylen){
		return NULL;
	}

	if(keylen != 16 && keylen != 24 && keylen != 32){
		return NULL;
	}

	p = (DKC_TWOFISH *)dkcAllocate(sizeof(DKC_TWOFISH));
	if(NULL == p){
		return NULL;
	}

	Twofish_KeySchedule(p, key, (int)keylen);

	return p;
}

int WINAPI dkcFreeTwofish(DKC_TWOFISH **p){
	if(NULL == p){
		return edk_FAILED;
	}
	return dkcFree((void **)p);
}

int WINAPI dkcTwofishEncrypt(DKC_TWOFISH *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	size_t offset;

	if(NULL == p || NULL == dest || NULL == src){
		return edk_FAILED;
	}
	if(destsize < srcsize){
		return edk_BufferOverFlow;
	}
	if(srcsize % dkcd_TWOFISH_BLOCK_SIZE != 0){
		return edk_ArgumentException;
	}

	for(offset = 0; offset < srcsize; offset += dkcd_TWOFISH_BLOCK_SIZE){
		Twofish_EncryptBlock(p, src + offset, dest + offset);
	}

	return edk_SUCCEEDED;
}

int WINAPI dkcTwofishDecrypt(DKC_TWOFISH *p,
	unsigned char *dest, size_t destsize,
	const unsigned char *src, size_t srcsize)
{
	size_t offset;

	if(NULL == p || NULL == dest || NULL == src){
		return edk_FAILED;
	}
	if(destsize < srcsize){
		return edk_BufferOverFlow;
	}
	if(srcsize % dkcd_TWOFISH_BLOCK_SIZE != 0){
		return edk_ArgumentException;
	}

	for(offset = 0; offset < srcsize; offset += dkcd_TWOFISH_BLOCK_SIZE){
		Twofish_DecryptBlock(p, src + offset, dest + offset);
	}

	return edk_SUCCEEDED;
}

void WINAPI dkcTwofishEncryptNoDest(DKC_TWOFISH *p,
	unsigned char *dest_and_src, size_t dest_and_srcsize)
{
	size_t offset;
	unsigned char tmp[dkcd_TWOFISH_BLOCK_SIZE];

	if(NULL == p || NULL == dest_and_src){
		return;
	}

	for(offset = 0; offset + dkcd_TWOFISH_BLOCK_SIZE <= dest_and_srcsize;
		offset += dkcd_TWOFISH_BLOCK_SIZE)
	{
		Twofish_EncryptBlock(p, dest_and_src + offset, tmp);
		memcpy(dest_and_src + offset, tmp, dkcd_TWOFISH_BLOCK_SIZE);
	}
}

void WINAPI dkcTwofishDecryptNoDest(DKC_TWOFISH *p,
	unsigned char *dest_and_src, size_t dest_and_srcsize)
{
	size_t offset;
	unsigned char tmp[dkcd_TWOFISH_BLOCK_SIZE];

	if(NULL == p || NULL == dest_and_src){
		return;
	}

	for(offset = 0; offset + dkcd_TWOFISH_BLOCK_SIZE <= dest_and_srcsize;
		offset += dkcd_TWOFISH_BLOCK_SIZE)
	{
		Twofish_DecryptBlock(p, dest_and_src + offset, tmp);
		memcpy(dest_and_src + offset, tmp, dkcd_TWOFISH_BLOCK_SIZE);
	}
}
