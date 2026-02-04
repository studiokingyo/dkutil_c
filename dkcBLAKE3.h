/*!
@file dkcBLAKE3.h
@brief BLAKE3 hash
@note BLAKE3: 32-bit words, 7 rounds, Merkle tree structure
*/
#ifndef DKUTIL_C_BLAKE3_H
#define DKUTIL_C_BLAKE3_H

#include "dkcOSIndependent.h"

#define BLAKE3_BLOCK_LEN  64
#define BLAKE3_CHUNK_LEN  1024   /* 16 blocks */
#define BLAKE3_MAX_DEPTH  54
#define BLAKE3_OUT_LEN    32
#define BLAKE3_BIN_BUFFER_SIZE 32
#define BLAKE3_STR_BUFFER_SIZE 65  /* 32*2+1 */

/* Domain separation flags */
#define BLAKE3_CHUNK_START  1
#define BLAKE3_CHUNK_END    2
#define BLAKE3_PARENT       4
#define BLAKE3_ROOT         8

typedef struct dkc_BLAKE3_ChunkState{
	uint32 cv[8];
	QWORD  chunk_counter;
	BYTE   buf[BLAKE3_BLOCK_LEN];
	BYTE   buf_len;
	BYTE   blocks_compressed;
	BYTE   flags;
}DKC_BLAKE3_CHUNK_STATE;

typedef struct dkc_BLAKE3{
	uint32 key[8];
	DKC_BLAKE3_CHUNK_STATE chunk;
	uint32 cv_stack[BLAKE3_MAX_DEPTH * 8];
	BYTE   cv_stack_len;
	BYTE   mFinalized;
	BYTE   output[BLAKE3_OUT_LEN];
}DKC_BLAKE3;

/* ==============================
 * BLAKE3
 * ============================== */
DKC_EXTERN DKC_BLAKE3 * WINAPI dkcAllocBLAKE3();
DKC_EXTERN void WINAPI dkcBLAKE3Init(DKC_BLAKE3 *p);
DKC_EXTERN void WINAPI dkcBLAKE3Load(DKC_BLAKE3 *p, const BYTE *pBuffer, DWORD dwSize);
DKC_EXTERN void WINAPI dkcBLAKE3Final(DKC_BLAKE3 *p);
DKC_EXTERN int  WINAPI dkcBLAKE3DigestStr(DKC_BLAKE3 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE3Digest(DKC_BLAKE3 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE3FinalDigestStr(DKC_BLAKE3 *p, char *buff, size_t size);
DKC_EXTERN int  WINAPI dkcBLAKE3FinalDigest(DKC_BLAKE3 *p, BYTE *buff, size_t size);
DKC_EXTERN int  WINAPI dkcFreeBLAKE3(DKC_BLAKE3 **p);

#endif /*end of include once*/
