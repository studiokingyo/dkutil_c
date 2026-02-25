/*!
@file dkcRangeCoder.h
@author dKingyo
@since 2026/02/25
@brief Range Coder compression / decompression
@note
Carryless Range Coder with adaptive order-0 model.
256-symbol frequency model. Rescaling uses power-of-2 range.
Integer arithmetic only, no floating point.
Best O(n), worst O(n).
*/

#ifndef DKUTIL_C_RANGECODER_H
#define DKUTIL_C_RANGECODER_H

#include "dkcOSIndependent.h"

/*! RangeCoder signature ('RANG') */
#define dkcdRANGECODER_SIGNATURE  0x52414E47UL

/*! Total frequency (must be <= 2^14 = 16384 for safe arithmetic with ULONG) */
#define dkcdRC_TOTAL_FREQ  16384u

/*! Number of alphabet symbols */
#define dkcdRC_SYMBOLS     256

/*!
@brief Working context for Range Coder.
Holds the adaptive frequency model.
*/
typedef struct dkc_RangeCoder {
    unsigned long cum_freq[dkcdRC_SYMBOLS + 1]; /* cumulative freq[0]=0, cum_freq[256]=TOTAL */
    unsigned long freq[dkcdRC_SYMBOLS];
} DKC_RANGECODER;

/*!
@brief Header stored at the beginning of compressed data.
*/
typedef struct dkc_RangeCoder_Header {
    unsigned long mSignature;
    size_t        mOriginSize;
    size_t        mCompressedSize;
} DKC_RANGECODER_HEADER;


DKC_EXTERN DKC_RANGECODER * WINAPI dkcAllocRangeCoder(void);

DKC_EXTERN int WINAPI dkcFreeRangeCoder(DKC_RANGECODER **pp);

/*!
@brief Compress src into dest using adaptive Range Coder.
@param ptr   [in]  context from dkcAllocRangeCoder()
@param ph    [out] header filled on success
@param dest  [out] output buffer
@param dsize [in]  capacity of dest
@param src   [in]  input data
@param ssize [in]  input size in bytes
@param sig   [in]  signature written to ph->mSignature
@return edk_SUCCEEDED, edk_NoValueToProcess, edk_FAILED, edk_ArgumentException
*/
DKC_EXTERN int WINAPI dkcRangeCoderEncode(DKC_RANGECODER *ptr, DKC_RANGECODER_HEADER *ph,
    BYTE *dest, size_t dsize,
    const BYTE *src, size_t ssize,
    ULONG sig);

/*!
@brief Decompress src into dest.
@param ptr   [in]  context from dkcAllocRangeCoder()
@param ph    [in]  header from dkcRangeCoderEncode()
@param dest  [out] output buffer (>= ph->mOriginSize bytes)
@param dsize [in]  capacity of dest
@param src   [in]  compressed payload (without header struct)
@param ssize [in]  size of compressed payload
@param sig   [in]  expected signature
@return edk_SUCCEEDED, edk_FAILED, edk_ArgumentException
*/
DKC_EXTERN int WINAPI dkcRangeCoderDecode(DKC_RANGECODER *ptr, const DKC_RANGECODER_HEADER *ph,
    BYTE *dest, size_t dsize,
    const BYTE *src, size_t ssize,
    ULONG sig);

#if !defined( DKUTIL_C_RANGECODER_C ) && defined(USE_DKC_INDEPENDENT_INCLUDE)
#   include "dkcRangeCoder.c"
#endif

#endif /* DKUTIL_C_RANGECODER_H */
