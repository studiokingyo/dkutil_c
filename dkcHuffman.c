/*!
@file dkcHuffman.c
@author dKingyo
@since 2026/02/25
@brief Canonical Huffman coding (C89 / VC6 compatible)
*/

#define DKUTIL_C_HUFFMAN_C

#include "dkcHuffman.h"
#include "dkcStdio.h"

#include <string.h>
#include <stdlib.h>

/* -----------------------------------------------------------------------
   Bit-stream helpers (write)
   ----------------------------------------------------------------------- */
typedef struct {
    BYTE  *buf;
    size_t cap;
    size_t byte_pos;
    int    bit_pos;  /* bits written into current byte (0-7) */
} HUFF_BSWRITE;

static void hbsw_init(HUFF_BSWRITE *bs, BYTE *buf, size_t cap)
{
    bs->buf      = buf;
    bs->cap      = cap;
    bs->byte_pos = 0;
    bs->bit_pos  = 0;
}

/* Returns 0 on success, -1 on overflow */
static int hbsw_write_bit(HUFF_BSWRITE *bs, int bit)
{
    if (bs->byte_pos >= bs->cap) return -1;
    if (bs->bit_pos == 0) bs->buf[bs->byte_pos] = 0;
    if (bit) bs->buf[bs->byte_pos] |= (BYTE)(1u << (7 - bs->bit_pos));
    bs->bit_pos++;
    if (bs->bit_pos == 8) { bs->bit_pos = 0; bs->byte_pos++; }
    return 0;
}

/* Write 'nbits' MSB-first from 'val' */
static int hbsw_write_bits(HUFF_BSWRITE *bs, unsigned long val, int nbits)
{
    int i;
    for (i = nbits - 1; i >= 0; i--) {
        if (hbsw_write_bit(bs, (int)((val >> i) & 1u)) != 0) return -1;
    }
    return 0;
}

/* Flush remaining bits (pad with 0), return total bytes used */
static size_t hbsw_flush(HUFF_BSWRITE *bs)
{
    if (bs->bit_pos > 0) { bs->byte_pos++; }
    return bs->byte_pos;
}

/* -----------------------------------------------------------------------
   Bit-stream helpers (read)
   ----------------------------------------------------------------------- */
typedef struct {
    const BYTE *buf;
    size_t      size;
    size_t      byte_pos;
    int         bit_pos; /* bits consumed in current byte (0-7) */
} HUFF_BSREAD;

static void hbsr_init(HUFF_BSREAD *bs, const BYTE *buf, size_t size)
{
    bs->buf      = buf;
    bs->size     = size;
    bs->byte_pos = 0;
    bs->bit_pos  = 0;
}

/* Returns 0 or 1, or -1 on end-of-stream */
static int hbsr_read_bit(HUFF_BSREAD *bs)
{
    int bit;
    if (bs->byte_pos >= bs->size) return -1;
    bit = (bs->buf[bs->byte_pos] >> (7 - bs->bit_pos)) & 1;
    bs->bit_pos++;
    if (bs->bit_pos == 8) { bs->bit_pos = 0; bs->byte_pos++; }
    return bit;
}

/* -----------------------------------------------------------------------
   Min-heap operations (on DKC_HUFFMAN.heap / nodes)
   ----------------------------------------------------------------------- */
static void huff_heap_swap(int *heap, int a, int b)
{
    int t = heap[a]; heap[a] = heap[b]; heap[b] = t;
}

static void huff_heap_up(DKC_HUFFMAN *h, int i)
{
    int p;
    while (i > 0) {
        p = (i - 1) / 2;
        if (h->nodes[h->heap[p]].freq <= h->nodes[h->heap[i]].freq) break;
        huff_heap_swap(h->heap, p, i);
        i = p;
    }
}

static void huff_heap_down(DKC_HUFFMAN *h, int i)
{
    int left, right, smallest;
    for (;;) {
        left     = 2 * i + 1;
        right    = 2 * i + 2;
        smallest = i;
        if (left  < h->heap_size &&
            h->nodes[h->heap[left]].freq  < h->nodes[h->heap[smallest]].freq)
            smallest = left;
        if (right < h->heap_size &&
            h->nodes[h->heap[right]].freq < h->nodes[h->heap[smallest]].freq)
            smallest = right;
        if (smallest == i) break;
        huff_heap_swap(h->heap, i, smallest);
        i = smallest;
    }
}

static void huff_heap_push(DKC_HUFFMAN *h, int node_idx)
{
    h->heap[h->heap_size] = node_idx;
    huff_heap_up(h, h->heap_size);
    h->heap_size++;
}

static int huff_heap_pop(DKC_HUFFMAN *h)
{
    int top = h->heap[0];
    h->heap_size--;
    h->heap[0] = h->heap[h->heap_size];
    if (h->heap_size > 0) huff_heap_down(h, 0);
    return top;
}

/* -----------------------------------------------------------------------
   Assign code lengths by DFS, then build canonical codes
   ----------------------------------------------------------------------- */
static void huff_assign_len(DKC_HUFFMAN *h, int node, int depth)
{
    if (h->nodes[node].left == -1) {
        /* leaf */
        h->code_len[h->nodes[node].symbol] = depth ? depth : 1;
        return;
    }
    huff_assign_len(h, h->nodes[node].left,  depth + 1);
    huff_assign_len(h, h->nodes[node].right, depth + 1);
}

/* Sort symbols by (code_len, symbol) for canonical assignment.
   Simple insertion sort (256 elements is small). */
static void huff_sort_symbols(int *order, const int *code_len)
{
    int i, j, tmp;
    for (i = 0; i < dkcdHUFFMAN_SYMBOLS; i++) order[i] = i;
    for (i = 1; i < dkcdHUFFMAN_SYMBOLS; i++) {
        tmp = order[i];
        j   = i - 1;
        while (j >= 0 &&
               (code_len[order[j]] > code_len[tmp] ||
                (code_len[order[j]] == code_len[tmp] && order[j] > tmp))) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = tmp;
    }
}

/* Build canonical code values from sorted symbols */
static void huff_build_canonical(DKC_HUFFMAN *h, const int *order)
{
    unsigned long code = 0;
    int prev_len = 0;
    int i, sym, len;
    for (i = 0; i < dkcdHUFFMAN_SYMBOLS; i++) {
        sym = order[i];
        len = h->code_len[sym];
        if (len == 0) continue;
        if (len > prev_len) {
            code   <<= (len - prev_len);
            prev_len = len;
        }
        h->code_val[sym] = code;
        code++;
    }
}

/* -----------------------------------------------------------------------
   Build Huffman tree from frequency table
   Returns root node index, or -1 on error.
   ----------------------------------------------------------------------- */
static int huff_build_tree(DKC_HUFFMAN *h, const unsigned long *freq)
{
    int n_nodes = 0;
    int i, a, b, parent;

    h->heap_size = 0;

    /* Create leaf nodes for symbols with freq > 0 */
    for (i = 0; i < dkcdHUFFMAN_SYMBOLS; i++) {
        if (freq[i] > 0) {
            if (n_nodes >= dkcdHUFFMAN_NODE_POOL) return -1;
            h->nodes[n_nodes].freq   = freq[i];
            h->nodes[n_nodes].left   = -1;
            h->nodes[n_nodes].right  = -1;
            h->nodes[n_nodes].symbol = i;
            huff_heap_push(h, n_nodes);
            n_nodes++;
        }
    }

    if (h->heap_size == 0) return -1;

    /* Special case: only one distinct symbol */
    if (h->heap_size == 1) {
        int leaf = huff_heap_pop(h);
        if (n_nodes >= dkcdHUFFMAN_NODE_POOL) return -1;
        h->nodes[n_nodes].freq   = h->nodes[leaf].freq;
        h->nodes[n_nodes].left   = leaf;
        h->nodes[n_nodes].right  = leaf; /* point to same leaf */
        h->nodes[n_nodes].symbol = -1;
        return n_nodes;
    }

    /* Build tree */
    while (h->heap_size > 1) {
        a = huff_heap_pop(h);
        b = huff_heap_pop(h);
        if (n_nodes >= dkcdHUFFMAN_NODE_POOL) return -1;
        parent = n_nodes;
        h->nodes[parent].freq   = h->nodes[a].freq + h->nodes[b].freq;
        h->nodes[parent].left   = a;
        h->nodes[parent].right  = b;
        h->nodes[parent].symbol = -1;
        huff_heap_push(h, parent);
        n_nodes++;
    }
    return huff_heap_pop(h);
}

/* -----------------------------------------------------------------------
   Decode tree rebuild from code_len table (canonical)
   We use a simple bit-by-bit decode loop during decompression.
   ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
   Public API
   ----------------------------------------------------------------------- */

DKC_HUFFMAN * WINAPI dkcAllocHuffman(void)
{
    DKC_HUFFMAN *p = (DKC_HUFFMAN *)dkcAllocateFill(sizeof(DKC_HUFFMAN), 0);
    return p;
}

int WINAPI dkcFreeHuffman(DKC_HUFFMAN **pp)
{
    if (!pp || !*pp) return edk_ArgumentException;
    return dkcFree((void **)pp);
}

/* ---------------------------------------------------------------------- */
int WINAPI dkcHuffmanEncode(DKC_HUFFMAN *ptr, DKC_HUFFMAN_HEADER *ph,
    BYTE *dest, size_t dsize,
    const BYTE *src, size_t ssize,
    ULONG sig)
{
    unsigned long freq[dkcdHUFFMAN_SYMBOLS];
    int order[dkcdHUFFMAN_SYMBOLS];
    HUFF_BSWRITE bs;
    int root, i, sym, len;
    size_t out_bytes;

    if (!ptr || !ph || !dest || !src) return edk_ArgumentException;
    if (ssize == 0) return edk_NoValueToProcess;

    /* Count frequencies */
    memset(freq, 0, sizeof(freq));
    for (i = 0; i < (int)ssize; i++) freq[src[i]]++;

    /* Build tree */
    memset(ptr->code_len, 0, sizeof(ptr->code_len));
    memset(ptr->code_val, 0, sizeof(ptr->code_val));
    root = huff_build_tree(ptr, freq);
    if (root < 0) return edk_FAILED;

    /* Assign lengths and canonical codes */
    huff_assign_len(ptr, root, 0);
    huff_sort_symbols(order, ptr->code_len);
    huff_build_canonical(ptr, order);

    /* Serialize: preamble = 256 bytes of code_len (0 = absent) */
    /* then the encoded bit stream */
    if (dsize < dkcdHUFFMAN_SYMBOLS) return edk_FAILED;

    for (i = 0; i < dkcdHUFFMAN_SYMBOLS; i++) {
        dest[i] = (BYTE)ptr->code_len[i];
    }

    hbsw_init(&bs, dest + dkcdHUFFMAN_SYMBOLS, dsize - dkcdHUFFMAN_SYMBOLS);

    for (i = 0; i < (int)ssize; i++) {
        sym = src[i];
        len = ptr->code_len[sym];
        if (len == 0) return edk_FAILED; /* shouldn't happen */
        if (hbsw_write_bits(&bs, ptr->code_val[sym], len) != 0) {
            return edk_NoValueToProcess; /* output overflow => incompressible */
        }
    }

    out_bytes = dkcdHUFFMAN_SYMBOLS + hbsw_flush(&bs);

    /* If compressed >= original, report incompressible */
    if (out_bytes >= ssize) return edk_NoValueToProcess;

    ph->mSignature      = sig;
    ph->mOriginSize     = ssize;
    ph->mCompressedSize = out_bytes;

    return edk_SUCCEEDED;
}

/* ---------------------------------------------------------------------- */
int WINAPI dkcHuffmanDecode(DKC_HUFFMAN *ptr, const DKC_HUFFMAN_HEADER *ph,
    BYTE *dest, size_t dsize,
    const BYTE *src, size_t ssize,
    ULONG sig)
{
    int order[dkcdHUFFMAN_SYMBOLS];
    /* decode table: for each (len, code) -> symbol.
       We use a simple canonical decode: walk sorted symbols. */
    HUFF_BSREAD bs;
    size_t out_pos;
    int i, bit;
    unsigned long cur_code;
    int cur_len;
    /* sorted symbols for decode */
    int sorted_sym[dkcdHUFFMAN_SYMBOLS];
    int sorted_len[dkcdHUFFMAN_SYMBOLS];
    unsigned long sorted_val[dkcdHUFFMAN_SYMBOLS];
    int n_sym;

    if (!ptr || !ph || !dest || !src) return edk_ArgumentException;
    if (ph->mSignature != sig)        return edk_FAILED;
    if (dsize < ph->mOriginSize)      return edk_FAILED;
    if (ssize < dkcdHUFFMAN_SYMBOLS)  return edk_FAILED;

    /* Restore code_len from preamble */
    memset(ptr->code_len, 0, sizeof(ptr->code_len));
    memset(ptr->code_val, 0, sizeof(ptr->code_val));
    for (i = 0; i < dkcdHUFFMAN_SYMBOLS; i++) {
        ptr->code_len[i] = (int)src[i];
    }

    /* Rebuild canonical codes */
    huff_sort_symbols(order, ptr->code_len);
    huff_build_canonical(ptr, order);

    /* Build sorted decode table */
    n_sym = 0;
    for (i = 0; i < dkcdHUFFMAN_SYMBOLS; i++) {
        int s = order[i];
        if (ptr->code_len[s] == 0) continue;
        sorted_sym[n_sym] = s;
        sorted_len[n_sym] = ptr->code_len[s];
        sorted_val[n_sym] = ptr->code_val[s];
        n_sym++;
    }

    /* Decode bit stream */
    hbsr_init(&bs, src + dkcdHUFFMAN_SYMBOLS, ssize - dkcdHUFFMAN_SYMBOLS);

    out_pos  = 0;
    cur_code = 0;
    cur_len  = 0;

    while (out_pos < ph->mOriginSize) {
        int found;
        int lo, mid, hi;

        /* Read one bit */
        bit = hbsr_read_bit(&bs);
        if (bit < 0) return edk_FAILED;
        cur_code = (cur_code << 1) | (unsigned long)bit;
        cur_len++;

        /* Binary search in sorted table for matching (len, code) */
        found = 0;
        lo = 0; hi = n_sym - 1;
        while (lo <= hi) {
            mid = (lo + hi) / 2;
            if (sorted_len[mid] == cur_len && sorted_val[mid] == cur_code) {
                found = 1;
                if (out_pos >= dsize) return edk_FAILED;
                dest[out_pos++] = (BYTE)sorted_sym[mid];
                cur_code = 0;
                cur_len  = 0;
                break;
            } else if (sorted_len[mid] < cur_len ||
                       (sorted_len[mid] == cur_len && sorted_val[mid] < cur_code)) {
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
        (void)found;

        if (cur_len > dkcdHUFFMAN_MAX_BITS) return edk_FAILED;
    }

    return edk_SUCCEEDED;
}
