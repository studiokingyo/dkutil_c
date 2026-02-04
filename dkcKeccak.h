/*!
@file dkcKeccak.h
@brief Keccak sponge construction (Keccak-f[1600])
@note FIPS 202 compliant Keccak core algorithm
*/
#ifndef DKUTIL_C_KECCAK_H
#define DKUTIL_C_KECCAK_H

#include "dkcOSIndependent.h"

/* Keccak state: 5x5 lanes of 64 bits = 1600 bits */
typedef struct dkc_Keccak{
	QWORD state[25];        /* 5x5 state array (1600 bits) */
	unsigned char buf[200]; /* absorption buffer (rate bytes) */
	size_t bufLen;          /* current bytes in buf */
	size_t rate;            /* rate in bytes (r/8) */
	size_t capacity;        /* capacity in bytes (c/8), rate+capacity=200 */
	unsigned char suffix;   /* domain separation suffix (0x06 for SHA3, 0x1F for SHAKE) */
	BYTE finalized;         /* squeeze phase entered */
}DKC_KECCAK;

/*!
@param p[in][out] Keccak state
@param rate[in] rate in bytes
@param capacity[in] capacity in bytes
@param suffix[in] domain separation suffix
*/
DKC_EXTERN void WINAPI dkcKeccakInit(DKC_KECCAK *p, size_t rate, size_t capacity, unsigned char suffix);

/*!
@param p[in][out] Keccak state
@param data[in] input data
@param len[in] input data length in bytes
*/
DKC_EXTERN void WINAPI dkcKeccakAbsorb(DKC_KECCAK *p, const BYTE *data, size_t len);

/*!
@param p[in][out] Keccak state
@param out[out] output buffer (NULL to finalize only)
@param outLen[in] output length in bytes
*/
DKC_EXTERN void WINAPI dkcKeccakSqueeze(DKC_KECCAK *p, BYTE *out, size_t outLen);

#endif /*end of include once*/
