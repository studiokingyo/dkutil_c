
/*!
@file dkcMD5.c
@brief MD5 hash algorithm
@note
original: http://sourceforge.net/projects/libmd5-rfc/
reconstruct: d金魚

*/
#define DKUTIL_C_MD5_C

#include "md5.h"
#include "md_misc.h"
//#include "md5_vc_mmx.h"
#include "dkcMD5.h"
#include "dkcStdio.h"

//**********************************************************
//GNETのpublic domainなMD5の実装から引用

DKC_MD5 *WINAPI dkcAllocMD5(){
	DKC_MD5 *p = dkcAllocate(sizeof(DKC_MD5));
	if(NULL==p) return NULL;
	dkcMD5Init(p);
	return p;
}


int WINAPI dkcFreeMD5(DKC_MD5 **pp){
	//DKC_MD5 *p = *pp;
	if(NULL==pp || NULL==*pp){
		return edk_FAILED;
	}
	return dkcFree((void **)pp);
}

void WINAPI dkcMD5Init(DKC_MD5 *p){
	p->count[0] = p->count[1] = 0;
	p->abcd[0] = 0x67452301;
  p->abcd[1] = 0xefcdab89;
  p->abcd[2] = 0x98badcfe;
  p->abcd[3] = 0x10325476;
	memset(p->a8,0,sizeof(p->a8));
	p->flags = edkcMD_Optimize;
	//p->mByteOrder = (uint8)dkcGetByteOrder();
}





/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static DKC_INLINE void 
MD5Transform(uint32 buf[4], uint32 const in[16])
{
  register uint32 a, b, c, d;

  a = buf[0];
  b = buf[1];
  c = buf[2];
  d = buf[3];

  MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
  MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
  MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
  MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
  MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
  MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
  MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
  MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
  MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
  MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
  MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
  MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
  MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
  MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
  MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
  MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

  MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
  MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
  MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
  MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
  MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
  MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
  MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
  MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
  MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
  MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
  MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
  MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
  MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
  MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
  MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
  MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

  MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
  MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
  MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
  MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
  MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
  MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
  MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
  MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
  MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
  MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
  MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
  MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
  MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
  MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
  MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
  MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

  MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
  MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
  MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
  MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
  MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
  MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
  MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
  MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
  MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
  MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
  MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
  MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
  MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
  MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
  MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
  MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

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
MD5Update(DKC_MD5 *p, uint8 const *buf, uint32 len)
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
      MD5Transform(p->abcd, (uint32 *) p->a8);
      buf += t;
      len -= t;
    }
  /* Process data in 64-byte chunks */

  while (len >= 64) 
    {
      memmove(p->a8, buf, 64);
      if ((p->flags) & edkcMD_ByteReverse)
        byteReverse(p->a8, 16);
      MD5Transform(p->abcd, (uint32 *) p->a8);
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
MD5Final( DKC_MD5 *ctx)
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
      MD5Transform(ctx->abcd, (uint32 *) ctx->a8);

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

  MD5Transform(ctx->abcd, (uint32 *) ctx->a8);
  if ((ctx->flags) & edkcMD_ByteReverse)
    byteReverse((uint8 *) ctx->abcd, 4);
  //memmove(digest, ctx->abcd, 16);
  //memset(ctx, 0, sizeof(ctx));	/* In case it's sensitive */
}
#endif
void WINAPI dkcMD5Load(DKC_MD5 *p,const BYTE *pBuffer,DWORD dwSize){
	if(p->flags & edkcMD_Finalized){
		return;
	}
	dkcMD_Update(p,pBuffer,dwSize,MD5Transform);
}



void WINAPI dkcMD5Final(DKC_MD5 *p){
	//uint8 digest[MD5_BIN_BUFFER_SIZE];
	if(p->flags & edkcMD_Finalized){
		return;
	}
	//MD5Final(p);
	dkcMD_Final(p,MD5Transform);
	//memcpy(p->a8,digest,sizeof(digest));
	p->flags |= edkcMD_Finalized;
}

int WINAPI dkcMD5Digest(DKC_MD5 *p,BYTE *buff,size_t size){
	
	if(size < MD5_BIN_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	return dkc_memcpy(buff,size,p->abcd,sizeof(p->abcd));
}

int WINAPI dkcMD5DigestStr(DKC_MD5 *p,char *buff,size_t size){
	register int i;
	uint8 temp[MD5_BIN_BUFFER_SIZE];
	if(size < MD5_STR_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	i = dkcMD5Digest(p,temp,sizeof(temp));
	if(DKUTIL_FAILED(i)){
		return i;
	}
	for (i=0; i<16; i++){
		sprintf(buff+i*2,"%02x", temp[i]);
	}
  buff[32]='\0';
	return edk_SUCCEEDED;
}

int WINAPI dkcMD5FinalDigestStr(DKC_MD5 *p,char *buff,size_t size){
	dkcMD5Final(p);
	return dkcMD5DigestStr(p,buff,size);
}

int WINAPI dkcMD5FinalDigest(DKC_MD5 *p,BYTE *buff,size_t size){
	dkcMD5Final(p);
	return dkcMD5Digest(p,buff,size);

}





//**********************************************************
//以下ADAPTER処理


DKC_MD5_ADAPTER *WINAPI dkcAllocMD5Adapter(uint32 initflag)
{
	DKC_MD5_ADAPTER *p = (DKC_MD5_ADAPTER *)dkcAllocate(sizeof(DKC_MD5_ADAPTER));
	if(NULL==p){
		return NULL;
	}
	switch(initflag){
		case edkcMD5_Default:
		case edkcMD5_Aladdin:
		default:
		p->mpObj = dkcAllocate(sizeof(md5_state_t));
	};
	if(NULL==p->mpObj)
	{
		dkcFree(&p);
		return NULL;
	}
	p->mObjFlag = initflag;
	dkcMD5AdapterInit(p);
	return p;
}

int WINAPI dkcFreeMD5Adapter(DKC_MD5_ADAPTER **pp){
	DKC_MD5_ADAPTER *p = *pp;
	if(NULL==pp || NULL==p){
		return edk_FAILED;
	}
	if(!p->mpObj) return edk_FAILED;
	switch(p->mObjFlag){
		case edkcMD5_Default:
		case edkcMD5_Aladdin:
		default:
		dkcFree(&(p->mpObj));
	}
	return dkcFree((void **)p);
}

void WINAPI dkcMD5AdapterInit(DKC_MD5_ADAPTER *p){
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		md5_init((md5_state_t *)p->mpObj);
	}
	p->mFinalized = FALSE;
}

int WINAPI dkcMD5AdapterLoad(DKC_MD5_ADAPTER *p,const BYTE *pBuffer,DWORD dwSize){
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		if(dwSize > INT_MAX){
			return edk_FAILED;
		}
	};

	dkcMD5AdapterLoadStandard(p,pBuffer,dwSize);
	return edk_SUCCEEDED;
}

void WINAPI dkcMD5AdapterLoadStandard(DKC_MD5_ADAPTER *p,const BYTE *pBuffer,DWORD dwSize){
	if(p->mFinalized){
		return;
	}
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		md5_append((md5_state_t *)p->mpObj,pBuffer,(int)dwSize);
	};
}



void WINAPI dkcMD5AdapterFinal(DKC_MD5_ADAPTER *p){
	if(p->mFinalized){
		return;
	}
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		md5_finalize((md5_state_t *)p->mpObj);
	}
	p->mFinalized = TRUE;
}

int WINAPI dkcMD5AdapterDigest(DKC_MD5_ADAPTER *p,BYTE *buff,size_t size){
	//if(size < 16){
	if(size < MD5_BIN_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		//第二引数のbuffのキャストがバグかもしれない。
		md5_get_digest((md5_state_t *)p->mpObj,(BYTE *)buff);
	}
	return edk_SUCCEEDED;
}

int WINAPI dkcMD5AdapterDigestStr(DKC_MD5_ADAPTER *p,char *buff,size_t size){
	//if(size < 32 + 1){
	if(size < MD5_STR_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		md5_get_str_digest((md5_state_t *)p->mpObj,(char *)buff);
	}
	return edk_SUCCEEDED;
}

int WINAPI dkcMD5AdapterFinalDigestStr(DKC_MD5_ADAPTER *p,char *buff,size_t size){
	dkcMD5AdapterFinal(p);
	return dkcMD5AdapterDigestStr(p,buff,size);
}

int WINAPI dkcMD5AdapterFinalDigest(DKC_MD5_ADAPTER *p,BYTE *buff,size_t size){
	dkcMD5AdapterFinal(p);
	return dkcMD5AdapterDigest(p,buff,size);

}


