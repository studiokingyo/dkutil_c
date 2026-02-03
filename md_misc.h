/**
@auther d金魚
@filename md_misc.h
@brief Message Digest algorithm misc

*/
#ifndef DKUTIL_C_MD_MISC_H
#define DKUTIL_C_MD_MISC_H

#include "dkcOSIndependent.h"

enum edkcMessageDigestFlag{
		///処理終了フラグ
	edkcMD_Finalized = 1,
	///エンディアンを変える
	edkcMD_ByteReverse = 2,
	///最適なアルゴリズムを選ぶ
	edkcMD_Optimize = 4,
};

typedef void (*DKC_MD_TRANSFORM_F_TYPE)(uint32 buf[4],uint32 const in[16]);

///MD5 and MD4 structure
typedef struct{
	uint32 count[2];
	uint32 abcd[4];
	/*union{
		uint64 a64[8];
		uint32 a32[16];
		uint16 a16[32];
		uint8  a8[64];
	}u_store;*/
	uint8 a8[64];
	uint8 flags;
}DKC_MD5,DKC_MD4;

/**
 * Note: this code is harmless on little-endian machines.
 */
DKC_INLINE void dkcMD_ByteReverse(uint8 buf[64], uint32 longs)
{
  uint32 t;
  do 
    {
      t = (uint32) ((uint32) buf[3] << 8 | buf[2]) << 16 |
          ((uint32) buf[1] << 8 | buf[0]);
      *(uint32 *) buf = t;
      buf += 4;
    } 
  while (--longs);
}

/**
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
DKC_INLINE void dkcMD_Update(DKC_MD5 *p, uint8 const *buf, uint32 len,DKC_MD_TRANSFORM_F_TYPE transform)
{
  uint32 t;

  /* Update bitcount */

  t = p->count[0];
  if ((p->count[0] = t + ((uint32) len << 3)) < t)
    p->count[1]++;		/* Carry from low to high */
  p->count[1] += len >> 29;

  t = (t >> 3) & 0x3f;	/* Bytes already in shsInfo->data */

  /* Handle any leading odd-sized chunks */

  if (t) 
    {
      //uint8 *p = (uint8 *) p->a8 + t;
			uint8 *pc = (uint8 *)&(p->a8[t]);
      t = 64 - t;
      if (len < t) 
        {
          memmove(pc, buf, len);
          return;
	}
      memmove(pc, buf, t);
      if ((p->flags) & edkcMD_ByteReverse)
        dkcMD_ByteReverse(p->a8, 16);
      //MD5Transform(p->abcd, (uint32 *) p->a8);
			transform(p->abcd,(uint32 *) p->a8);
      buf += t;
      len -= t;
    }
  /* Process data in 64-byte chunks */

  while (len >= 64) 
    {
      memmove(p->a8, buf, 64);
      if ((p->flags) & edkcMD_ByteReverse)
        dkcMD_ByteReverse(p->a8, 16);
      //MD5Transform(p->abcd, (uint32 *) p->a8);
			transform(p->abcd,(uint32 *) p->a8);
      buf += 64;
      len -= 64;
    }

  /* Handle any remaining bytes of data. */

  memmove(p->a8, buf, len);
}

/**
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
DKC_INLINE void dkcMD_Final( DKC_MD5 *ctx,DKC_MD_TRANSFORM_F_TYPE transform)
{
  uint32 count;
  uint8 *p;

  /* Compute number of bytes mod 64 */
  count = (ctx->count[0] >> 3) & 0x3F;

  /* Set the first char of padding to 0x80.  This is safe since there is
     always at least one byte free */
  p = ctx->a8 + count;
  *p++ = 0x80;

  /* Bytes of padding needed to make 64 bytes */
  count = 64 - 1 - count;

  /* Pad out to 56 mod 64 */
  if (count < 8) 
    {
	/* Two lots of padding:  Pad the first block to 64 bytes */
      memset(p, 0, count);
      if ((ctx->flags) & edkcMD_ByteReverse)
        dkcMD_ByteReverse(ctx->a8, 16);
      //MD5Transform(ctx->abcd, (uint32 *) ctx->a8);
			transform(ctx->abcd,(uint32 *) ctx->a8);
      /* Now fill the next block with 56 bytes */
      memset(ctx->a8, 0, 56);
    } 
  else 
    {
      /* Pad block to 56 bytes */
      memset(p, 0, count - 8);
    }
  if ((ctx->flags) & edkcMD_ByteReverse)
    dkcMD_ByteReverse(ctx->a8, 14);

  /* Append length in bits and transform */
  ((uint32 *) ctx->a8)[14] = ctx->count[0];
  ((uint32 *) ctx->a8)[15] = ctx->count[1];

  //MD5Transform(ctx->abcd, (uint32 *) ctx->a8);
	transform(ctx->abcd,(uint32 *) ctx->a8);
  if ((ctx->flags) & edkcMD_ByteReverse)
    dkcMD_ByteReverse((uint8 *) ctx->abcd, 4);
  //memmove(digest, ctx->abcd, 16);
  //memset(ctx, 0, sizeof(ctx));	/* In case it's sensitive */
}

#endif 
