
/*!
@file dkcBLAKE3.c
@brief BLAKE3 hash
@note BLAKE3: 32-bit words, 7 rounds, Merkle tree structure
      Based on BLAKE2s compression with domain separation flags
*/
#define DKUTIL_C_BLAKE3_C

#include "dkcBLAKE3.h"
#include "dkcStdio.h"
#include <stdio.h>

/* BLAKE3 IV (same as SHA-256 IV / BLAKE2s IV) */
static const uint32 blake3_iv[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

/* Message schedule for BLAKE3 (permutation applied iteratively) */
static const unsigned char blake3_msg_schedule[7][16] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15},
	{ 2, 6, 3,10, 7, 0, 4,13, 1,11,12, 5, 9,14,15, 8},
	{ 3, 4,10,12,13, 2, 7,14, 6, 5, 9, 0,11,15, 8, 1},
	{10, 7,12, 9,14, 3,13,15, 4, 0,11, 2, 5, 8, 1, 6},
	{12,13, 9,11,15,10,14, 8, 7, 2, 5, 3, 0, 1, 6, 4},
	{ 9,14,11, 5, 8,12,15, 1,13, 3, 0,10, 2, 6, 4, 7},
	{11,15, 5, 0, 1, 9, 8, 6,14,10, 2,12, 3, 4, 7,13}
};

/* ==============================
 * Internal helpers
 * ============================== */

static DKC_INLINE uint32 blake3_rot(uint32 x, int n){
	return (x >> n) | (x << (32 - n));
}

static DKC_INLINE uint32 blake3_load32le(const BYTE *p){
	return ((uint32)p[0])       | ((uint32)p[1] << 8) |
	       ((uint32)p[2] << 16) | ((uint32)p[3] << 24);
}

static DKC_INLINE void blake3_store32le(BYTE *p, uint32 v){
	p[0] = (BYTE)(v);
	p[1] = (BYTE)(v >> 8);
	p[2] = (BYTE)(v >> 16);
	p[3] = (BYTE)(v >> 24);
}

#define BLAKE3_G(state,a,b,c,d,mx,my) \
	do { \
		state[a] += state[b] + mx; \
		state[d] = blake3_rot(state[d] ^ state[a], 16); \
		state[c] += state[d]; \
		state[b] = blake3_rot(state[b] ^ state[c], 12); \
		state[a] += state[b] + my; \
		state[d] = blake3_rot(state[d] ^ state[a], 8); \
		state[c] += state[d]; \
		state[b] = blake3_rot(state[b] ^ state[c], 7); \
	} while(0)

static void blake3_round(uint32 *state, const uint32 *msg, int round_idx){
	const unsigned char *s = blake3_msg_schedule[round_idx];
	/* Column step */
	BLAKE3_G(state, 0,4, 8,12, msg[s[ 0]], msg[s[ 1]]);
	BLAKE3_G(state, 1,5, 9,13, msg[s[ 2]], msg[s[ 3]]);
	BLAKE3_G(state, 2,6,10,14, msg[s[ 4]], msg[s[ 5]]);
	BLAKE3_G(state, 3,7,11,15, msg[s[ 6]], msg[s[ 7]]);
	/* Diagonal step */
	BLAKE3_G(state, 0,5,10,15, msg[s[ 8]], msg[s[ 9]]);
	BLAKE3_G(state, 1,6,11,12, msg[s[10]], msg[s[11]]);
	BLAKE3_G(state, 2,7, 8,13, msg[s[12]], msg[s[13]]);
	BLAKE3_G(state, 3,4, 9,14, msg[s[14]], msg[s[15]]);
}

static void blake3_compress(const uint32 cv[8], const BYTE block[BLAKE3_BLOCK_LEN],
                            BYTE block_len, QWORD counter, BYTE flags,
                            uint32 out[16]){
	uint32 state[16];
	uint32 msg[16];
	int i;

	for(i = 0; i < 16; i++){
		msg[i] = blake3_load32le(block + i * 4);
	}

	state[ 0] = cv[0];
	state[ 1] = cv[1];
	state[ 2] = cv[2];
	state[ 3] = cv[3];
	state[ 4] = cv[4];
	state[ 5] = cv[5];
	state[ 6] = cv[6];
	state[ 7] = cv[7];
	state[ 8] = blake3_iv[0];
	state[ 9] = blake3_iv[1];
	state[10] = blake3_iv[2];
	state[11] = blake3_iv[3];
	state[12] = (uint32)(counter);
	state[13] = (uint32)(counter >> 32);
	state[14] = (uint32)block_len;
	state[15] = (uint32)flags;

	/* 7 rounds */
	for(i = 0; i < 7; i++){
		blake3_round(state, msg, i);
	}

	for(i = 0; i < 8; i++){
		out[i] = state[i] ^ state[i + 8];
		out[i + 8] = state[i + 8] ^ cv[i];
	}
}

static void blake3_compress_cv(const uint32 cv[8], const BYTE block[BLAKE3_BLOCK_LEN],
                               BYTE block_len, QWORD counter, BYTE flags,
                               uint32 out_cv[8]){
	uint32 full[16];
	int i;
	blake3_compress(cv, block, block_len, counter, flags, full);
	for(i = 0; i < 8; i++) out_cv[i] = full[i];
}

/* ==============================
 * Chunk state
 * ============================== */

static void blake3_chunk_state_init(DKC_BLAKE3_CHUNK_STATE *cs, const uint32 key[8], QWORD chunk_counter){
	int i;
	for(i = 0; i < 8; i++) cs->cv[i] = key[i];
	cs->chunk_counter = chunk_counter;
	cs->buf_len = 0;
	cs->blocks_compressed = 0;
	cs->flags = 0;
}

static size_t blake3_chunk_state_len(const DKC_BLAKE3_CHUNK_STATE *cs){
	return (size_t)cs->blocks_compressed * BLAKE3_BLOCK_LEN + (size_t)cs->buf_len;
}

static void blake3_chunk_state_update(DKC_BLAKE3_CHUNK_STATE *cs, const BYTE *input, size_t input_len){
	size_t take;
	BYTE flags;

	while(input_len > 0){
		/* If block buffer is full, compress */
		if(cs->buf_len == BLAKE3_BLOCK_LEN){
			flags = cs->flags;
			if(cs->blocks_compressed == 0) flags |= BLAKE3_CHUNK_START;
			blake3_compress_cv(cs->cv, cs->buf, BLAKE3_BLOCK_LEN,
			                   cs->chunk_counter, flags, cs->cv);
			cs->blocks_compressed++;
			cs->buf_len = 0;
		}

		take = (size_t)BLAKE3_BLOCK_LEN - (size_t)cs->buf_len;
		if(take > input_len) take = input_len;
		memcpy(cs->buf + cs->buf_len, input, take);
		cs->buf_len += (BYTE)take;
		input += take;
		input_len -= take;
	}
}

static void blake3_chunk_state_output(const DKC_BLAKE3_CHUNK_STATE *cs, uint32 out_cv[8]){
	BYTE flags;
	BYTE padded[BLAKE3_BLOCK_LEN];
	flags = cs->flags | BLAKE3_CHUNK_END;
	if(cs->blocks_compressed == 0) flags |= BLAKE3_CHUNK_START;
	memcpy(padded, cs->buf, cs->buf_len);
	if(cs->buf_len < BLAKE3_BLOCK_LEN){
		memset(padded + cs->buf_len, 0, BLAKE3_BLOCK_LEN - cs->buf_len);
	}
	blake3_compress_cv(cs->cv, padded, cs->buf_len,
	                   cs->chunk_counter, flags, out_cv);
}

/* ==============================
 * Parent node
 * ============================== */

static void blake3_parent_cv(const uint32 left_cv[8], const uint32 right_cv[8],
                             const uint32 key[8], BYTE flags, uint32 out_cv[8]){
	BYTE block[BLAKE3_BLOCK_LEN];
	int i;
	for(i = 0; i < 8; i++) blake3_store32le(block + i * 4, left_cv[i]);
	for(i = 0; i < 8; i++) blake3_store32le(block + (i + 8) * 4, right_cv[i]);
	blake3_compress_cv(key, block, BLAKE3_BLOCK_LEN, 0, flags | BLAKE3_PARENT, out_cv);
}

/* ==============================
 * Merkle tree helpers
 * ============================== */

static void blake3_push_cv(DKC_BLAKE3 *p, const uint32 cv[8]){
	int i;
	int offset = (int)p->cv_stack_len * 8;
	for(i = 0; i < 8; i++) p->cv_stack[offset + i] = cv[i];
	p->cv_stack_len++;
}

static void blake3_pop_cv(DKC_BLAKE3 *p, uint32 cv[8]){
	int i;
	int offset;
	p->cv_stack_len--;
	offset = (int)p->cv_stack_len * 8;
	for(i = 0; i < 8; i++) cv[i] = p->cv_stack[offset + i];
}

static void blake3_add_chunk_cv(DKC_BLAKE3 *p, const uint32 new_cv[8], QWORD total_chunks){
	uint32 cv[8];
	uint32 parent[8];
	int i;

	for(i = 0; i < 8; i++) cv[i] = new_cv[i];

	/* Merge CVs as needed based on total_chunks */
	while(total_chunks > 0 && (total_chunks & 1) == 0){
		uint32 left[8];
		blake3_pop_cv(p, left);
		blake3_parent_cv(left, cv, p->key, p->chunk.flags, parent);
		for(i = 0; i < 8; i++) cv[i] = parent[i];
		total_chunks >>= 1;
	}
	blake3_push_cv(p, cv);
}

/* ==============================
 * BLAKE3 API
 * ============================== */

DKC_BLAKE3 * WINAPI dkcAllocBLAKE3(){
	DKC_BLAKE3 *p = (DKC_BLAKE3 *)dkcAllocate(sizeof(DKC_BLAKE3));
	if(NULL == p) return NULL;
	dkcBLAKE3Init(p);
	return p;
}

void WINAPI dkcBLAKE3Init(DKC_BLAKE3 *p){
	int i;
	for(i = 0; i < 8; i++) p->key[i] = blake3_iv[i];
	blake3_chunk_state_init(&p->chunk, p->key, 0);
	p->cv_stack_len = 0;
	p->mFinalized = FALSE;
	memset(p->output, 0, BLAKE3_OUT_LEN);
}

void WINAPI dkcBLAKE3Load(DKC_BLAKE3 *p, const BYTE *pBuffer, DWORD dwSize){
	size_t take;
	uint32 chunk_cv[8];

	if(p->mFinalized) return;

	while(dwSize > 0){
		/* If current chunk is full, finalize it and start a new one */
		if(blake3_chunk_state_len(&p->chunk) == BLAKE3_CHUNK_LEN){
			blake3_chunk_state_output(&p->chunk, chunk_cv);
			blake3_add_chunk_cv(p, chunk_cv, p->chunk.chunk_counter + 1);
			blake3_chunk_state_init(&p->chunk, p->key, p->chunk.chunk_counter + 1);
		}

		take = BLAKE3_CHUNK_LEN - blake3_chunk_state_len(&p->chunk);
		if(take > (size_t)dwSize) take = (size_t)dwSize;
		blake3_chunk_state_update(&p->chunk, pBuffer, take);
		pBuffer += take;
		dwSize -= (DWORD)take;
	}
}

void WINAPI dkcBLAKE3Final(DKC_BLAKE3 *p){
	uint32 out16[16];
	int i;
	BYTE flags;

	if(p->mFinalized) return;

	if(p->cv_stack_len == 0){
		/* Single chunk (or empty input): output with ROOT flag directly from chunk */
		flags = p->chunk.flags | BLAKE3_CHUNK_END | BLAKE3_ROOT;
		if(p->chunk.blocks_compressed == 0) flags |= BLAKE3_CHUNK_START;
		/* Pad buffer */
		if(p->chunk.buf_len < BLAKE3_BLOCK_LEN){
			memset(p->chunk.buf + p->chunk.buf_len, 0,
			       BLAKE3_BLOCK_LEN - p->chunk.buf_len);
		}
		blake3_compress(p->chunk.cv, p->chunk.buf, p->chunk.buf_len,
		                p->chunk.chunk_counter, flags, out16);
		for(i = 0; i < 8; i++){
			blake3_store32le(p->output + i * 4, out16[i]);
		}
	} else {
		/* Multiple chunks: get current chunk's CV, merge with stack, then root */
		uint32 cv[8];
		uint32 parent[8];
		uint32 left[8];
		BYTE parent_block[BLAKE3_BLOCK_LEN];

		blake3_chunk_state_output(&p->chunk, cv);

		/* Merge all but the last pair without ROOT flag */
		while(p->cv_stack_len > 1){
			blake3_pop_cv(p, left);
			blake3_parent_cv(left, cv, p->key, p->chunk.flags, parent);
			for(i = 0; i < 8; i++) cv[i] = parent[i];
		}

		/* Final merge with ROOT flag */
		blake3_pop_cv(p, left);
		for(i = 0; i < 8; i++) blake3_store32le(parent_block + i * 4, left[i]);
		for(i = 0; i < 8; i++) blake3_store32le(parent_block + (i + 8) * 4, cv[i]);
		blake3_compress(p->key, parent_block, BLAKE3_BLOCK_LEN, 0,
		                p->chunk.flags | BLAKE3_PARENT | BLAKE3_ROOT, out16);
		for(i = 0; i < 8; i++){
			blake3_store32le(p->output + i * 4, out16[i]);
		}
	}

	p->mFinalized = TRUE;
}

int WINAPI dkcBLAKE3DigestStr(DKC_BLAKE3 *p, char *buff, size_t size){
	char s[BLAKE3_STR_BUFFER_SIZE];
	int i;
	if(BLAKE3_STR_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	for(i = 0; i < BLAKE3_BIN_BUFFER_SIZE; i++) sprintf(s + i * 2, "%02x", p->output[i]);
	s[BLAKE3_BIN_BUFFER_SIZE * 2] = '\0';
	return dkc_strcpy(buff, size, s, BLAKE3_BIN_BUFFER_SIZE * 2);
}

int WINAPI dkcBLAKE3Digest(DKC_BLAKE3 *p, BYTE *buff, size_t size){
	if(BLAKE3_BIN_BUFFER_SIZE > size) return edk_BufferOverFlow;
	if(FALSE == p->mFinalized) return edk_LogicError;
	memcpy(buff, p->output, BLAKE3_BIN_BUFFER_SIZE);
	return edk_SUCCEEDED;
}

int WINAPI dkcBLAKE3FinalDigestStr(DKC_BLAKE3 *p, char *buff, size_t size){
	dkcBLAKE3Final(p);
	return dkcBLAKE3DigestStr(p, buff, size);
}

int WINAPI dkcBLAKE3FinalDigest(DKC_BLAKE3 *p, BYTE *buff, size_t size){
	dkcBLAKE3Final(p);
	return dkcBLAKE3Digest(p, buff, size);
}

int WINAPI dkcFreeBLAKE3(DKC_BLAKE3 **p){
	if(NULL == p) return edk_FAILED;
	return dkcFree((void **)p);
}
