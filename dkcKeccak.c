
/*!
@file dkcKeccak.c
@brief Keccak sponge construction (Keccak-f[1600])
@note FIPS 202 compliant Keccak-f[1600] permutation
*/
#define DKUTIL_C_KECCAK_C

#include "dkcKeccak.h"
#include <string.h>

#ifdef _MSC_VER
#define QW(x) x##ui64
#else
#define QW(x) x##ULL
#endif

/* ==============================
 * Keccak-f[1600] round constants
 * ============================== */
static const QWORD keccak_rc[24] = {
	QW(0x0000000000000001), QW(0x0000000000008082),
	QW(0x800000000000808A), QW(0x8000000080008000),
	QW(0x000000000000808B), QW(0x0000000080000001),
	QW(0x8000000080008081), QW(0x8000000000008009),
	QW(0x000000000000008A), QW(0x0000000000000088),
	QW(0x0000000080008009), QW(0x000000008000000A),
	QW(0x000000008000808B), QW(0x800000000000008B),
	QW(0x8000000000008089), QW(0x8000000000008003),
	QW(0x8000000000008002), QW(0x8000000000000080),
	QW(0x000000000000800A), QW(0x800000008000000A),
	QW(0x8000000080008081), QW(0x8000000000008080),
	QW(0x0000000080000001), QW(0x8000000080008008)
};

/* ==============================
 * Rho rotation offsets
 * ============================== */
static const int keccak_rho[25] = {
	 0,  1, 62, 28, 27,
	36, 44,  6, 55, 20,
	 3, 10, 43, 25, 39,
	41, 45, 15, 21,  8,
	18,  2, 61, 56, 14
};

/* ==============================
 * Pi permutation indices
 * ============================== */
static const int keccak_pi[25] = {
	 0, 10, 20,  5, 15,
	16,  1, 11, 21,  6,
	 7, 17,  2, 12, 22,
	23,  8, 18,  3, 13,
	14, 24,  9, 19,  4
};

/* ==============================
 * 64-bit rotate left
 * ============================== */
static DKC_INLINE QWORD keccak_rotl64(QWORD x, int n)
{
	return (x << n) | (x >> (64 - n));
}

/* ==============================
 * Keccak-f[1600] permutation
 * ============================== */
static void keccak_f1600(QWORD state[25])
{
	QWORD C[5], D[5], B[25];
	int round, x, y;
	for(round = 0; round < 24; round++){

		/* theta */
		for(x = 0; x < 5; x++){
			C[x] = state[x] ^ state[x + 5] ^ state[x + 10] ^ state[x + 15] ^ state[x + 20];
		}
		for(x = 0; x < 5; x++){
			D[x] = C[(x + 4) % 5] ^ keccak_rotl64(C[(x + 1) % 5], 1);
		}
		for(x = 0; x < 25; x++){
			state[x] ^= D[x % 5];
		}

		/* rho + pi */
		for(x = 0; x < 25; x++){
			B[keccak_pi[x]] = keccak_rotl64(state[x], keccak_rho[x]);
		}

		/* chi */
		for(y = 0; y < 25; y += 5){
			for(x = 0; x < 5; x++){
				state[y + x] = B[y + x] ^ ((~B[y + (x + 1) % 5]) & B[y + (x + 2) % 5]);
			}
		}

		/* iota */
		state[0] ^= keccak_rc[round];
	}
}

/* ==============================
 * XOR data into state (little-endian)
 * ============================== */
static void keccak_xor_state(QWORD state[25], const unsigned char *data, size_t len)
{
	size_t i, j;
	size_t lanes = len / 8;
	size_t remainder = len % 8;
	QWORD lane;

	for(i = 0; i < lanes; i++){
		lane = 0;
		lane |= (QWORD)data[i * 8 + 0];
		lane |= (QWORD)data[i * 8 + 1] << 8;
		lane |= (QWORD)data[i * 8 + 2] << 16;
		lane |= (QWORD)data[i * 8 + 3] << 24;
		lane |= (QWORD)data[i * 8 + 4] << 32;
		lane |= (QWORD)data[i * 8 + 5] << 40;
		lane |= (QWORD)data[i * 8 + 6] << 48;
		lane |= (QWORD)data[i * 8 + 7] << 56;
		state[i] ^= lane;
	}
	if(remainder > 0){
		lane = 0;
		for(j = 0; j < remainder; j++){
			lane |= (QWORD)data[lanes * 8 + j] << (j * 8);
		}
		state[lanes] ^= lane;
	}
}

/* ==============================
 * Read bytes from state (little-endian)
 * ============================== */
static void keccak_read_state(const QWORD state[25], unsigned char *out, size_t len)
{
	size_t i;
	for(i = 0; i < len; i++){
		out[i] = (unsigned char)(state[i / 8] >> (8 * (i % 8)));
	}
}

/* ==============================
 * Public API
 * ============================== */

void WINAPI dkcKeccakInit(DKC_KECCAK *p, size_t rate, size_t capacity, unsigned char suffix)
{
	size_t i;
	for(i = 0; i < 25; i++){
		p->state[i] = 0;
	}
	memset(p->buf, 0, sizeof(p->buf));
	p->bufLen = 0;
	p->rate = rate;
	p->capacity = capacity;
	p->suffix = suffix;
	p->finalized = FALSE;
}

void WINAPI dkcKeccakAbsorb(DKC_KECCAK *p, const BYTE *data, size_t len)
{
	size_t i, copyLen;
	if(p->finalized) return;

	while(len > 0){
		copyLen = p->rate - p->bufLen;
		if(copyLen > len) copyLen = len;

		for(i = 0; i < copyLen; i++){
			p->buf[p->bufLen + i] = data[i];
		}
		p->bufLen += copyLen;
		data += copyLen;
		len -= copyLen;

		if(p->bufLen == p->rate){
			keccak_xor_state(p->state, p->buf, p->rate);
			keccak_f1600(p->state);
			p->bufLen = 0;
		}
	}
}

void WINAPI dkcKeccakSqueeze(DKC_KECCAK *p, BYTE *out, size_t outLen)
{
	size_t offset, available, todo;

	if(!p->finalized){
		/* Pad: suffix || 10*1 padding */
		/* Add domain separation suffix */
		p->buf[p->bufLen] = p->suffix;
		p->bufLen++;
		/* Zero-fill remaining */
		while(p->bufLen < p->rate){
			p->buf[p->bufLen] = 0x00;
			p->bufLen++;
		}
		/* Set last byte high bit (multi-rate padding) */
		p->buf[p->rate - 1] |= 0x80;

		/* Final absorb */
		keccak_xor_state(p->state, p->buf, p->rate);
		keccak_f1600(p->state);

		p->finalized = TRUE;
		p->bufLen = 0;
	}

	/* Extract output bytes from state */
	if(out != NULL && outLen > 0){
		offset = 0;
		while(offset < outLen){
			available = p->rate - p->bufLen;
			todo = outLen - offset;
			if(todo > available) todo = available;

			keccak_read_state(p->state, out + offset, todo);
			offset += todo;
			p->bufLen += todo;

			if(p->bufLen == p->rate && offset < outLen){
				keccak_f1600(p->state);
				p->bufLen = 0;
			}
		}
	}
}
