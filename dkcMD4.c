
/*!
@file dkcMD4.c
@brief MD4 hash algorithm
@note
reconstructed by d‹à‹›

// This is the original introductory comment:

// md4.cpp - modified by Wei Dai from Andrew M. Kuchling's md4.c
// The original code and all modifications are in the public domain.

/*
 *  md4.c : MD4 hash algorithm.
 *
 * Part of the Python Cryptography Toolkit, version 1.1
 *
 * Distribute and use freely; there are no restrictions on further 
 * dissemination and usage except those imposed by the laws of your 
 * country of residence.
 *
*/
#define DKUTIL_C_MD4_C


#include "md_misc.h"
#include "dkcMD4.h"
#include "dkcStdio.h"

//**********************************************************
//GNET‚Ìpublic domain‚ÈMD4‚ÌŽÀ‘•‚©‚çˆø—p

DKC_MD4 *WINAPI dkcAllocMD4(){
	DKC_MD4 *p = dkcAllocate(sizeof(DKC_MD4));
	if(NULL==p) return NULL;
	dkcMD4Init(p);
	return p;
}


int WINAPI dkcFreeMD4(DKC_MD4 **pp){
	if(NULL==pp || NULL==*pp){
		return edk_FAILED;
	}
	return dkcFree((void **)pp);
}



void WINAPI dkcMD4Init(DKC_MD4 *p){
	p->count[0] = p->count[1] = 0;

	p->abcd[0] = 0x67452301;
  p->abcd[1] = 0xefcdab89;
  p->abcd[2] = 0x98badcfe;
  p->abcd[3] = 0x10325476;
	memset(p->a8,0,sizeof(p->a8));
	//p->mByteOrder = (uint8)dkcGetByteOrder();
}





/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define F3(x, y, z) (x ^ y ^ z)

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/// This is the central step in the MD4 algorithm
#define MD4STEP(f,a,b,c,d,x,s,data)\
	(a) += f(b,c,d) + x + data;\
	(a) = ROTATE_LEFT(a,s);


/*
 * The core of the MD4 algorithm, this alters an existing MD4 hash to
 * reflect the addition of 16 longwords of new data.  MD4Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static DKC_INLINE void 
MD4Transform(uint32 buf[4], uint32 const in[16])
{
  register uint32 a, b, c, d;

  a = buf[0];
  b = buf[1];
  c = buf[2];
  d = buf[3];

	MD4STEP(F1,a,b,c,d, in[0], 3,0);
	MD4STEP(F1,d,a,b,c, in[1], 7,0);
	MD4STEP(F1,c,d,a,b, in[2],11,0);
	MD4STEP(F1,b,c,d,a, in[3],19,0);
	MD4STEP(F1,a,b,c,d, in[4], 3,0);
	MD4STEP(F1,d,a,b,c, in[5], 7,0);
	MD4STEP(F1,c,d,a,b, in[6],11,0);
	MD4STEP(F1,b,c,d,a, in[7],19,0);
	MD4STEP(F1,a,b,c,d, in[8], 3,0);
	MD4STEP(F1,d,a,b,c, in[9], 7,0);
	MD4STEP(F1,c,d,a,b,in[10],11,0);
	MD4STEP(F1,b,c,d,a,in[11],19,0);
	MD4STEP(F1,a,b,c,d,in[12], 3,0);
	MD4STEP(F1,d,a,b,c,in[13], 7,0);
	MD4STEP(F1,c,d,a,b,in[14],11,0);
	MD4STEP(F1,b,c,d,a,in[15],19,0);

	MD4STEP(F2,a,b,c,d, in[0], 3,0x5a827999);
	MD4STEP(F2,d,a,b,c, in[4], 5,0x5a827999);
	MD4STEP(F2,c,d,a,b, in[8], 9,0x5a827999);
	MD4STEP(F2,b,c,d,a,in[12],13,0x5a827999);
	MD4STEP(F2,a,b,c,d, in[1], 3,0x5a827999);
	MD4STEP(F2,d,a,b,c, in[5], 5,0x5a827999);
	MD4STEP(F2,c,d,a,b, in[9], 9,0x5a827999);
	MD4STEP(F2,b,c,d,a,in[13],13,0x5a827999);
	MD4STEP(F2,a,b,c,d, in[2], 3,0x5a827999);
	MD4STEP(F2,d,a,b,c, in[6], 5,0x5a827999);
	MD4STEP(F2,c,d,a,b,in[10], 9,0x5a827999);
	MD4STEP(F2,b,c,d,a,in[14],13,0x5a827999);
	MD4STEP(F2,a,b,c,d, in[3], 3,0x5a827999);
	MD4STEP(F2,d,a,b,c, in[7], 5,0x5a827999);
	MD4STEP(F2,c,d,a,b,in[11], 9,0x5a827999);
	MD4STEP(F2,b,c,d,a,in[15],13,0x5a827999);
 
	MD4STEP(F3,a,b,c,d, in[0], 3,0x6ed9eba1);
	MD4STEP(F3,d,a,b,c, in[8], 9,0x6ed9eba1);
	MD4STEP(F3,c,d,a,b, in[4],11,0x6ed9eba1);
	MD4STEP(F3,b,c,d,a,in[12],15,0x6ed9eba1);
	MD4STEP(F3,a,b,c,d, in[2], 3,0x6ed9eba1);
	MD4STEP(F3,d,a,b,c,in[10], 9,0x6ed9eba1);
	MD4STEP(F3,c,d,a,b, in[6],11,0x6ed9eba1);
	MD4STEP(F3,b,c,d,a,in[14],15,0x6ed9eba1);
	MD4STEP(F3,a,b,c,d, in[1], 3,0x6ed9eba1);
	MD4STEP(F3,d,a,b,c, in[9], 9,0x6ed9eba1);
	MD4STEP(F3,c,d,a,b, in[5],11,0x6ed9eba1);
	MD4STEP(F3,b,c,d,a,in[13],15,0x6ed9eba1);
	MD4STEP(F3,a,b,c,d, in[3], 3,0x6ed9eba1);
	MD4STEP(F3,d,a,b,c,in[11], 9,0x6ed9eba1);
	MD4STEP(F3,c,d,a,b, in[7],11,0x6ed9eba1);
	MD4STEP(F3,b,c,d,a,in[15],15,0x6ed9eba1);
	

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}

#if 0

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static DKC_INLINE void 
MD4Update(DKC_MD4 *p, uint8 const *buf, uint32 len)
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
        byteReverse(p->a8, 16);
      MD4Transform(p->abcd, (uint32 *) p->a8);
      buf += t;
      len -= t;
    }
  /* Process data in 64-byte chunks */

  while (len >= 64) 
    {
      memmove(p->a8, buf, 64);
      if ((p->flags) & edkcMD_ByteReverse)
        byteReverse(p->a8, 16);
      MD4Transform(p->abcd, (uint32 *) p->a8);
      buf += 64;
      len -= 64;
    }

  /* Handle any remaining bytes of data. */

  memmove(p->a8, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static DKC_INLINE void 
MD4Final( DKC_MD4 *ctx)
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
        byteReverse(ctx->a8, 16);
      MD4Transform(ctx->abcd, (uint32 *) ctx->a8);

      /* Now fill the next block with 56 bytes */
      memset(ctx->a8, 0, 56);
    } 
  else 
    {
      /* Pad block to 56 bytes */
      memset(p, 0, count - 8);
    }
  if ((ctx->flags) & edkcMD_ByteReverse)
    byteReverse(ctx->a8, 14);

  /* Append length in bits and transform */
  ((uint32 *) ctx->a8)[14] = ctx->count[0];
  ((uint32 *) ctx->a8)[15] = ctx->count[1];

  MD4Transform(ctx->abcd, (uint32 *) ctx->a8);
  if ((ctx->flags) & edkcMD_ByteReverse)
    byteReverse((uint8 *) ctx->abcd, 4);
  //memmove(digest, ctx->abcd, 16);
  //memset(ctx, 0, sizeof(ctx));	/* In case it's sensitive */
}

#endif

void WINAPI dkcMD4Load(DKC_MD4 *p,const BYTE *pBuffer,DWORD dwSize){
	if(p->flags & edkcMD_Finalized){
		return;
	}
	dkcMD_Update(p,pBuffer,dwSize,MD4Transform);
	//MD4Update(p,pBuffer,dwSize);
}



void WINAPI dkcMD4Final(DKC_MD4 *p){
	//uint8 digest[MD4_BIN_BUFFER_SIZE];
	if(p->flags & edkcMD_Finalized){
		return;
	}
	//MD4Final(p);
	dkcMD_Final(p,MD4Transform);
	//memcpy(p->a8,digest,sizeof(digest));
	p->flags |= edkcMD_Finalized;
}

int WINAPI dkcMD4Digest(DKC_MD4 *p,BYTE *buff,size_t size){
	
	if(size < MD4_BIN_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	return dkc_memcpy(buff,size,p->abcd,sizeof(p->abcd));
}

int WINAPI dkcMD4DigestStr(DKC_MD4 *p,char *buff,size_t size){
	register int i;
	uint8 temp[MD4_BIN_BUFFER_SIZE];
	if(size < MD4_STR_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	i = dkcMD4Digest(p,temp,sizeof(temp));
	if(DKUTIL_FAILED(i)){
		return i;
	}
	for (i=0; i<16; i++){
		sprintf(buff+i*2,"%02x", temp[i]);
	}
  buff[32]='\0';
	return edk_SUCCEEDED;
}

int WINAPI dkcMD4FinalDigestStr(DKC_MD4 *p,char *buff,size_t size){
	dkcMD4Final(p);
	return dkcMD4DigestStr(p,buff,size);
}

int WINAPI dkcMD4FinalDigest(DKC_MD4 *p,BYTE *buff,size_t size){
	dkcMD4Final(p);
	return dkcMD4Digest(p,buff,size);

}

