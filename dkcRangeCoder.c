/*!
@file dkcRangeCoder.c
@author dKingyo
@since 2026/02/25
@brief Adaptive order-0 Range Coder (C89 / VC6 compatible)

Implementation notes:
  Subbotin-style carryless 32-bit range coder.
  Two thresholds: RC_TOP (2^24) and RC_BOT (2^16).
  Normalization emits a byte only when the top byte of low is determined
  (low and low+range agree on top 8 bits), or forces range alignment
  when range < RC_BOT to avoid indefinite stall.
  Model: uniform init (freq[s]=1, total=256), additive update with halving
  when cumulative total reaches RC_TOTAL.
*/

#define DKUTIL_C_RANGECODER_C

#include "dkcRangeCoder.h"
#include "dkcStdio.h"

#include <string.h>
#include <stdlib.h>

/* -----------------------------------------------------------------------
   Constants
   ----------------------------------------------------------------------- */
#define RC_TOP    (1UL << 24)
#define RC_BOT    (1UL << 16)
#define RC_TOTAL  dkcdRC_TOTAL_FREQ   /* 16384 */

/* -----------------------------------------------------------------------
   Model helpers
   ----------------------------------------------------------------------- */

static void rc_model_init(DKC_RANGECODER *m)
{
    int i;
    for (i = 0; i < dkcdRC_SYMBOLS; i++) m->freq[i] = 1;
    m->cum_freq[0] = 0;
    for (i = 0; i < dkcdRC_SYMBOLS; i++)
        m->cum_freq[i + 1] = m->cum_freq[i] + m->freq[i];
}

static void rc_rebuild_cum(DKC_RANGECODER *m)
{
    int i;
    m->cum_freq[0] = 0;
    for (i = 0; i < dkcdRC_SYMBOLS; i++)
        m->cum_freq[i + 1] = m->cum_freq[i] + m->freq[i];
}

static void rc_model_update(DKC_RANGECODER *m, int sym)
{
    unsigned long total;
    int i;
    m->freq[sym]++;
    total = m->cum_freq[dkcdRC_SYMBOLS] + 1;
    if (total >= RC_TOTAL) {
        for (i = 0; i < dkcdRC_SYMBOLS; i++) {
            m->freq[i] = (m->freq[i] + 1) >> 1;
            if (m->freq[i] == 0) m->freq[i] = 1;
        }
    }
    rc_rebuild_cum(m);
}

/* Binary search: find largest sym where cum_freq[sym] <= v */
static int rc_find_sym(const DKC_RANGECODER *m, unsigned long v)
{
    int lo = 0, hi = dkcdRC_SYMBOLS - 1, mid;
    while (lo < hi) {
        mid = (lo + hi + 1) / 2;
        if (m->cum_freq[mid] <= v)
            lo = mid;
        else
            hi = mid - 1;
    }
    return lo;
}

/* -----------------------------------------------------------------------
   Encoder  (Subbotin carryless)
   ----------------------------------------------------------------------- */
typedef struct {
    BYTE         *out;
    size_t        cap;
    size_t        pos;
    unsigned long low;
    unsigned long range;
} RC_ENC;

static void rce_init(RC_ENC *e, BYTE *out, size_t cap)
{
    e->out   = out;
    e->cap   = cap;
    e->pos   = 0;
    e->low   = 0;
    e->range = 0xFFFFFFFFUL;
}

static int rce_write_byte(RC_ENC *e, BYTE b)
{
    if (e->pos >= e->cap) return -1;
    e->out[e->pos++] = b;
    return 0;
}

/*
 * Subbotin carryless normalization (encoder).
 *
 * Condition 1: (low ^ (low + range)) < RC_TOP
 *   Top 8 bits of low and low+range agree -> safe to emit top byte.
 *
 * Condition 2: range < RC_BOT
 *   Range is too small and top bytes disagree -> force-align range to
 *   the next RC_BOT boundary so the top byte becomes determined.
 *   Small precision loss, but avoids indefinite carry ambiguity.
 *
 * If neither condition holds (top bytes disagree but range >= RC_BOT),
 * we simply stop and let the next encode step shrink range further.
 */
static int rce_normalize(RC_ENC *e)
{
    while ((e->low ^ (e->low + e->range)) < RC_TOP ||
           (e->range < RC_BOT &&
            ((e->range = (0UL - e->low) & (RC_BOT - 1)), 1)))
    {
        if (rce_write_byte(e, (BYTE)(e->low >> 24)) != 0) return -1;
        e->range <<= 8;
        e->low   <<= 8;
    }
    return 0;
}

static int rce_encode(RC_ENC *e, const DKC_RANGECODER *m, int sym)
{
    unsigned long total = m->cum_freq[dkcdRC_SYMBOLS];
    unsigned long step  = e->range / total;
    e->low   += step * m->cum_freq[sym];
    e->range  = step * m->freq[sym];
    return rce_normalize(e);
}

/* Flush: emit enough bytes to uniquely identify the final interval */
static int rce_flush(RC_ENC *e)
{
    int i;
    for (i = 0; i < 4; i++) {
        if (rce_write_byte(e, (BYTE)(e->low >> 24)) != 0) return -1;
        e->low <<= 8;
    }
    return 0;
}

/* -----------------------------------------------------------------------
   Decoder  (Subbotin carryless)

   Tracks low/range in lockstep with the encoder.
   code holds the actual bitstream value read so far.
   Decoding uses (code - low) / step to recover the frequency bin.
   ----------------------------------------------------------------------- */
typedef struct {
    const BYTE   *in;
    size_t        size;
    size_t        pos;
    unsigned long low;
    unsigned long range;
    unsigned long code;
} RC_DEC;

static BYTE rcd_read_byte(RC_DEC *d)
{
    if (d->pos >= d->size) return 0;
    return d->in[d->pos++];
}

static void rcd_init(RC_DEC *d, const BYTE *in, size_t size)
{
    int i;
    d->in    = in;
    d->size  = size;
    d->pos   = 0;
    d->low   = 0;
    d->range = 0xFFFFFFFFUL;
    d->code  = 0;
    for (i = 0; i < 4; i++)
        d->code = (d->code << 8) | (unsigned long)rcd_read_byte(d);
}

/* Mirror of rce_normalize: same conditions, shifts code instead of emitting */
static void rcd_normalize(RC_DEC *d)
{
    while ((d->low ^ (d->low + d->range)) < RC_TOP ||
           (d->range < RC_BOT &&
            ((d->range = (0UL - d->low) & (RC_BOT - 1)), 1)))
    {
        d->code = (d->code << 8) | (unsigned long)rcd_read_byte(d);
        d->range <<= 8;
        d->low   <<= 8;
    }
}

static int rcd_decode(RC_DEC *d, DKC_RANGECODER *m, int *out_sym)
{
    unsigned long total = m->cum_freq[dkcdRC_SYMBOLS];
    unsigned long step  = d->range / total;
    unsigned long v     = (d->code - d->low) / step;
    int sym;

    if (v >= total) v = total - 1;
    sym = rc_find_sym(m, v);

    d->low   += step * m->cum_freq[sym];
    d->range  = step * m->freq[sym];
    rcd_normalize(d);

    *out_sym = sym;
    return 0;
}

/* -----------------------------------------------------------------------
   Public API
   ----------------------------------------------------------------------- */

DKC_RANGECODER * WINAPI dkcAllocRangeCoder(void)
{
    DKC_RANGECODER *p = (DKC_RANGECODER *)dkcAllocateFill(sizeof(DKC_RANGECODER), 0);
    return p;
}

int WINAPI dkcFreeRangeCoder(DKC_RANGECODER **pp)
{
    if (!pp || !*pp) return edk_ArgumentException;
    return dkcFree((void **)pp);
}

/* ---------------------------------------------------------------------- */
int WINAPI dkcRangeCoderEncode(DKC_RANGECODER *ptr, DKC_RANGECODER_HEADER *ph,
    BYTE *dest, size_t dsize,
    const BYTE *src, size_t ssize,
    ULONG sig)
{
    RC_ENC enc;
    size_t i;
    int sym;

    if (!ptr || !ph || !dest || !src) return edk_ArgumentException;
    if (ssize == 0)                   return edk_NoValueToProcess;

    rc_model_init(ptr);
    rce_init(&enc, dest, dsize);

    for (i = 0; i < ssize; i++) {
        sym = (int)(unsigned char)src[i];
        if (rce_encode(&enc, ptr, sym) != 0) return edk_NoValueToProcess;
        rc_model_update(ptr, sym);
    }
    if (rce_flush(&enc) != 0) return edk_NoValueToProcess;

    if (enc.pos >= ssize) return edk_NoValueToProcess;

    ph->mSignature      = sig;
    ph->mOriginSize     = ssize;
    ph->mCompressedSize = enc.pos;
    return edk_SUCCEEDED;
}

/* ---------------------------------------------------------------------- */
int WINAPI dkcRangeCoderDecode(DKC_RANGECODER *ptr, const DKC_RANGECODER_HEADER *ph,
    BYTE *dest, size_t dsize,
    const BYTE *src, size_t ssize,
    ULONG sig)
{
    RC_DEC dec;
    size_t i;
    int sym;

    if (!ptr || !ph || !dest || !src) return edk_ArgumentException;
    if (ph->mSignature != sig)        return edk_FAILED;
    if (dsize < ph->mOriginSize)      return edk_FAILED;

    rc_model_init(ptr);
    rcd_init(&dec, src, ssize);

    for (i = 0; i < ph->mOriginSize; i++) {
        if (rcd_decode(&dec, ptr, &sym) != 0) return edk_FAILED;
        dest[i] = (BYTE)sym;
        rc_model_update(ptr, sym);
    }
    return edk_SUCCEEDED;
}
