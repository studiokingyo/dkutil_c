/*!
@file dkcHuffman.h
@author dKingyo
@since 2026/02/25
@brief Huffman coding compression / decompression
@note
Canonical Huffman coding.
Best O(n), worst O(n log n) (dominated by sort).
Symbols: 256 byte values.
*/

#ifndef DKUTIL_C_HUFFMAN_H
#define DKUTIL_C_HUFFMAN_H

#include "dkcOSIndependent.h"

/*! Huffman signature ('HUFF') */
#define dkcdHUFFMAN_SIGNATURE  0x48554646UL

/*! Maximum code length (256 symbols -> depth <= 255, but we cap at 32) */
#define dkcdHUFFMAN_MAX_BITS   16

/*! Number of alphabet symbols */
#define dkcdHUFFMAN_SYMBOLS    256

/*! Internal node pool size (2*256-1) */
#define dkcdHUFFMAN_NODE_POOL  511

typedef struct dkc_HuffNode {
    unsigned long  freq;
    int            left;   /* index into node pool, -1 = leaf */
    int            right;
    int            symbol; /* valid when left==-1 */
} DKC_HUFF_NODE;

/*!
@brief Working context for Huffman encoder/decoder.
*/
typedef struct dkc_Huffman {
    DKC_HUFF_NODE nodes[dkcdHUFFMAN_NODE_POOL];
    int           heap[dkcdHUFFMAN_NODE_POOL]; /* min-heap of node indices */
    int           heap_size;
    /* canonical code tables */
    int           code_len[dkcdHUFFMAN_SYMBOLS];  /* bit-length per symbol */
    unsigned long code_val[dkcdHUFFMAN_SYMBOLS];  /* canonical code value */
} DKC_HUFFMAN;

/*!
@brief Header stored at the beginning of compressed data.
*/
typedef struct dkc_Huffman_Header {
    unsigned long mSignature;
    size_t        mOriginSize;
    size_t        mCompressedSize;
    /*! code_len table (256 bytes) follows immediately in bitstream preamble */
} DKC_HUFFMAN_HEADER;


DKC_EXTERN DKC_HUFFMAN * WINAPI dkcAllocHuffman(void);

DKC_EXTERN int WINAPI dkcFreeHuffman(DKC_HUFFMAN **pp);

/*!
@brief Compress src[0..ssize) into dest[0..dsize).
@param ptr     [in]  context from dkcAllocHuffman()
@param ph      [out] header filled on success
@param dest    [out] output buffer
@param dsize   [in]  size of dest
@param src     [in]  input data
@param ssize   [in]  input size
@param sig     [in]  signature written to ph->mSignature
@return edk_SUCCEEDED, edk_NoValueToProcess (incompressible), edk_FAILED, edk_ArgumentException
*/
DKC_EXTERN int WINAPI dkcHuffmanEncode(DKC_HUFFMAN *ptr, DKC_HUFFMAN_HEADER *ph,
    BYTE *dest, size_t dsize,
    const BYTE *src, size_t ssize,
    ULONG sig);

/*!
@brief Decompress src[0..ssize) into dest[0..dsize).
@param ptr     [in]  context from dkcAllocHuffman()
@param ph      [in]  header produced by dkcHuffmanEncode()
@param dest    [out] output buffer (must be >= ph->mOriginSize)
@param dsize   [in]  size of dest
@param src     [in]  compressed data (does NOT include header struct itself)
@param ssize   [in]  size of compressed data
@param sig     [in]  expected signature
@return edk_SUCCEEDED, edk_FAILED, edk_ArgumentException
*/
DKC_EXTERN int WINAPI dkcHuffmanDecode(DKC_HUFFMAN *ptr, const DKC_HUFFMAN_HEADER *ph,
    BYTE *dest, size_t dsize,
    const BYTE *src, size_t ssize,
    ULONG sig);

#if !defined( DKUTIL_C_HUFFMAN_C ) && defined(USE_DKC_INDEPENDENT_INCLUDE)
#   include "dkcHuffman.c"
#endif

#endif /* DKUTIL_C_HUFFMAN_H */
