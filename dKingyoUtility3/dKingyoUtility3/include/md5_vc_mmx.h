/**
@auther d金魚
@brief only for VC md5 MMX
@note
licence is NYSL
@section MD5_VC_MMX_H_history history
2005/12/16:製作開始。
*/
#ifndef MD5_VC_MMX_H
#define MD5_VC_MMX_H



#ifdef INDEPENDENT_MD5_VC_MMX_H
#include <stdio.h>
#define uint32 unsigned int
#define uint16 unsigned short
#define uint8 unsigned char
#else
#include "dkcOSIndependent.h"
#endif



/* #define MD5_VC_MMX_F1(x, y, z) (x & y | ~x & z) non optimize*/

/**
#define MD5_VC_MMX_F1(x, y, z) (z ^ (x & (y ^ z)))
d:dest x:mm0 y:mm1 z:mm2
*/
#define MD5_VC_MMX_F1(d,x,y,z)\
	_asm pxor y,z\
	_asm pand x,y\
	_asm pxor z,x\
	_asm movq d,z

#define MD5_VC_MMX_F2(d,x, y, z) MD5_VC_MMX_F1(d,z, x, y)

///#define MD5_VC_MMX_F3(x, y, z) (x ^ y ^ z)
#define MD5_VC_MMX_F3(d,x,y,z)\
	_asm pxor y,z\
	_asm pxor x,y\
	_asm movq d,x



///#define MD5_VC_MMX_F4(x, y, z) (y ^ (x | ~z))	/*_asm pandn z,0xFFFFFFFFFFFFFFFF\*/
#define MD5_VC_MMX_F4(d,x,y,z)\
	_asm pandn x,mm5\
	_asm por		x,z\
	_asm pxor	y,x\
	_asm movq	d,y




/* This is the central step in the MD5 algorithm. 
#define MD5_VC_MMX_STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )
		core										rotate									plus
*/

///out w
///w += f(x, y, z) + data
#define MD5_VC_MMX_CORE(temp,w,f,x,y,z,data)\
	f(temp,x,y,z)\
	_asm paddd temp,data\
	_asm paddd	w,temp

///out d
///w = w<<s | w>>(32-s)
#define MD5_VC_MMX_ROTATE(d,w,s)\
	_asm movq	d,w\
	_asm psllq d,s\
	_asm psrlq w,32-s\
	_asm por		d,w

///out w
#define MD5_VC_MMX_PLUS(w,x)\
	_asm paddd	w,x

///lastout w,temp
#define MD5_VC_MMX_STEP(f, w, x, y, z, data, s,temp) \
	MD5_VC_MMX_CORE(temp,w,f,x,y,z,(data))\
	MD5_VC_MMX_ROTATE(temp,w,s)\
	MD5_VC_MMX_PLUS(temp,x)\
	_asm movq w,temp


/*#define MD5_VC_MMX_UINT32_TO_UINT64(a,b)
	dkcTwoDWORDToULONGLONG(dest,a,b);

	//(uint64)((uint64)(((uint64)a * MAXDWORD)) + (uint64)b )
	//( high * MAXDWORD ) + low*/

DKC_INLINE uint64 MD5_VC_MMX_UINT32_TO_UINT64(uint32 a,uint32 b){
	ULARGE_INTEGER inte;
	inte.LowPart = b;
	inte.HighPart = a;
	return inte.QuadPart;

}
/**
mmxが使える状態かどうか各自でチェックしてください。
sizeは64にしてください。
*/
static DKC_INLINE md5_mmx_double_update(
	uint32 abcd0[4],
	uint32 abcd1[4],
 const uint32 in0[16],
 const uint32 in1[16])
{
	uint64 abcd[4];
	uint64 tinpo[16];
	
	const uint64 tempv = 0xFFFFFFFFFFFFFFFF;
	int i;
	abcd[0] = MD5_VC_MMX_UINT32_TO_UINT64(1,1);
	
	abcd[0] = MD5_VC_MMX_UINT32_TO_UINT64(abcd0[0],abcd1[0]);
	abcd[1] = MD5_VC_MMX_UINT32_TO_UINT64(abcd0[1],abcd1[1]);
	abcd[2] = MD5_VC_MMX_UINT32_TO_UINT64(abcd0[2],abcd1[2]);
	abcd[3] = MD5_VC_MMX_UINT32_TO_UINT64(abcd0[3],abcd1[3]);

	for(i=0;i<16;i++){
		tinpo[i] = MD5_VC_MMX_UINT32_TO_UINT64(in0[i],in1[i]);
	}
	
	_asm	movq mm0,abcd[0]
	_asm	movq mm1,abcd[1]
	_asm	movq mm2,abcd[2]
	_asm	movq mm3,abcd[3]
	
#define a mm0
#define b mm1
#define c mm2
#define d mm3
#define temp_mm mm4
//mm5 = 0xFFFFFFFFFFFFFFFF
	_asm	movq mm5, tempv

///out mm6 / use mm7 : @note MMX複雑すぎて壊れました。
#define INSERT_TINPOW(tinpo,data)\
	_asm	mov eax,data
	_asm	movd mm6,eax\
	_asm	movd mm7,eax\
	_asm	punpckldq mm7,mm6\
	_asm	movq mm6,tinpo\
	_asm	paddd mm6,mm7

#define tinpow mm6

	_asm	mov eax,0xd76aa478
	_asm	movd mm6,eax
	_asm	movd mm7,eax
	_asm	punpckldq mm7,mm6
	_asm	movq mm6,tinpo[0]
	_asm	paddd mm6,mm7
	INSERT_TINPOW(tinpo[0] ,0xd76aa478);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, a, b, c, d, tinpow, 7,temp_mm);
  INSERT_TINPOW(tinpo[1] ,0xe8c7b756);
	MD5_VC_MMX_STEP(MD5_VC_MMX_F1, d, a, b, c, tinpow , 12,temp_mm);
	INSERT_TINPOW(tinpo[2] ,0x242070db);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, c, d, a, b, tinpow, 17,temp_mm);
	INSERT_TINPOW(tinpo[3] ,0xc1bdceee);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, b, c, d, a, tinpow, 22,temp_mm);
	INSERT_TINPOW(tinpo[4] , 0xf57c0faf);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, a, b, c, d, tinpow, 7,temp_mm);
	INSERT_TINPOW(tinpo[5] , 0x4787c62a);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, d, a, b, c, tinpow, 12,temp_mm);
	INSERT_TINPOW(tinpo[6] , 0xa8304613);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, c, d, a, b, tinpow, 17,temp_mm);
	INSERT_TINPOW(tinpo[7] , 0xfd469501);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, b, c, d, a, tinpow, 22,temp_mm);
	INSERT_TINPOW(tinpo[8] , 0x698098d8);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, a, b, c, d, tinpow, 7,temp_mm);
	INSERT_TINPOW(tinpo[9] , 0x8b44f7af);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, d, a, b, c, tinpow, 12,temp_mm);
	INSERT_TINPOW(tinpo[10] , 0xffff5bb1);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, c, d, a, b, tinpow, 17,temp_mm);
	INSERT_TINPOW(tinpo[11] , 0x895cd7be);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, b, c, d, a, tinpow, 22,temp_mm);
	INSERT_TINPOW(tinpo[12] , 0x6b901122);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, a, b, c, d, tinpow, 7,temp_mm);
	INSERT_TINPOW(tinpo[13] , 0xfd987193);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, d, a, b, c, tinpow, 12,temp_mm);
	INSERT_TINPOW(tinpo[14] , 0xa679438e);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, c, d, a, b, tinpow, 17,temp_mm);
	INSERT_TINPOW(tinpo[15] , 0x49b40821);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, b, c, d, a, tinpow, 22,temp_mm);

	INSERT_TINPOW(tinpo[1] , 0xf61e2562);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, a, b, c, d, tinpow, 5,temp_mm);
  INSERT_TINPOW(tinpo[6] , 0xc040b340);
	MD5_VC_MMX_STEP(MD5_VC_MMX_F2, d, a, b, c, tinpow, 9,temp_mm);
	INSERT_TINPOW(tinpo[11] , 0x265e5a51);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, c, d, a, b, tinpow, 14,temp_mm);
	INSERT_TINPOW(tinpo[0] , 0xe9b6c7aa);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, b, c, d, a, tinpow, 20,temp_mm);
	INSERT_TINPOW(tinpo[5] , 0xd62f105d);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, a, b, c, d, tinpow, 5,temp_mm);
	INSERT_TINPOW(tinpo[10] , 0x02441453);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, d, a, b, c, tinpow, 9,temp_mm);
	INSERT_TINPOW(tinpo[15] , 0xd8a1e681);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, c, d, a, b, tinpow, 14,temp_mm);
	INSERT_TINPOW(tinpo[4] , 0xe7d3fbc8);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, b, c, d, a, tinpow, 20,temp_mm);
	INSERT_TINPOW(tinpo[9] , 0x21e1cde6);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, a, b, c, d, tinpow, 5,temp_mm);
	INSERT_TINPOW(tinpo[14] , 0xc33707d6);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, d, a, b, c, tinpow, 9,temp_mm);
	INSERT_TINPOW(tinpo[3] , 0xf4d50d87);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, c, d, a, b, tinpow, 14,temp_mm);
	INSERT_TINPOW( tinpo[8] , 0x455a14ed);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, b, c, d, a, tinpow, 20,temp_mm);
	INSERT_TINPOW(tinpo[13] , 0xa9e3e905);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, a, b, c, d, tinpow, 5,temp_mm);
	INSERT_TINPOW(tinpo[2] , 0xfcefa3f8);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, d, a, b, c, tinpow, 9,temp_mm);
	INSERT_TINPOW(tinpo[7] , 0x676f02d9);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, c, d, a, b, tinpow, 14,temp_mm);
	INSERT_TINPOW(tinpo[12] , 0x8d2a4c8a);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, b, c, d, a, tinpow, 20,temp_mm);

	INSERT_TINPOW(tinpo[5] , 0xfffa3942);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, a, b, c, d, tinpow, 4,temp_mm);
	INSERT_TINPOW(tinpo[8] , 0x8771f681);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, d, a, b, c, tinpow, 11,temp_mm);
	INSERT_TINPOW(tinpo[11] , 0x6d9d6122);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, c, d, a, b, tinpow, 16,temp_mm);
	INSERT_TINPOW(tinpo[14] , 0xfde5380c);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, b, c, d, a, tinpow, 23,temp_mm);
	INSERT_TINPOW(tinpo[1] , 0xa4beea44);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, a, b, c, d, tinpow, 4,temp_mm);
	INSERT_TINPOW(tinpo[4] , 0x4bdecfa9);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, d, a, b, c, tinpow, 11,temp_mm);
	INSERT_TINPOW(tinpo[7] , 0xf6bb4b60);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, c, d, a, b, tinpow, 16,temp_mm);
	INSERT_TINPOW(tinpo[10] , 0xbebfbc70);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, b, c, d, a, tinpow, 23,temp_mm);
	INSERT_TINPOW(tinpo[13] , 0x289b7ec6);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, a, b, c, d, tinpow, 4,temp_mm);
	INSERT_TINPOW(tinpo[0] , 0xeaa127fa);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, d, a, b, c, tinpow, 11,temp_mm);
	INSERT_TINPOW(tinpo[3] , 0xd4ef3085);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, c, d, a, b, tinpow, 16,temp_mm);
	INSERT_TINPOW(tinpo[6] , 0x04881d05);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, b, c, d, a, tinpow, 23,temp_mm);
	INSERT_TINPOW(tinpo[9] , 0xd9d4d039);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, a, b, c, d, tinpow, 4,temp_mm);
	INSERT_TINPOW(tinpo[12] , 0xe6db99e5);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, d, a, b, c, tinpow, 11,temp_mm);
	INSERT_TINPOW(tinpo[15] , 0x1fa27cf8);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, c, d, a, b, tinpow, 16,temp_mm);
	INSERT_TINPOW(tinpo[2] , 0xc4ac5665);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, b, c, d, a, tinpow, 23,temp_mm);

	INSERT_TINPOW(tinpo[0] , 0xf4292244);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, a, b, c, d, tinpow, 6,temp_mm);
	INSERT_TINPOW(tinpo[7] , 0x432aff97);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, d, a, b, c, tinpow, 10,temp_mm);
	INSERT_TINPOW(tinpo[14] , 0xab9423a7);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, c, d, a, b, tinpow, 15,temp_mm);
	INSERT_TINPOW(tinpo[5] , 0xfc93a039);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, b, c, d, a, tinpow, 21,temp_mm);
	INSERT_TINPOW(tinpo[12] , 0x655b59c3);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, a, b, c, d, tinpow, 6,temp_mm);
	INSERT_TINPOW(tinpo[3] , 0x8f0ccc92);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, d, a, b, c, tinpow, 10,temp_mm);
	INSERT_TINPOW(tinpo[10] , 0xffeff47d);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, c, d, a, b, tinpow, 15,temp_mm);
	INSERT_TINPOW(tinpo[1] , 0x85845dd1);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, b, c, d, a, tinpow, 21,temp_mm);
	INSERT_TINPOW(tinpo[8] , 0x6fa87e4f);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, a, b, c, d, tinpow, 6,temp_mm);
	INSERT_TINPOW(tinpo[15] , 0xfe2ce6e0);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, d, a, b, c, tinpow, 10,temp_mm);
	INSERT_TINPOW(tinpo[6] , 0xa3014314);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, c, d, a, b, tinpow, 15,temp_mm);
	INSERT_TINPOW(tinpo[13] , 0x4e0811a1);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, b, c, d, a, tinpow, 21,temp_mm);
	INSERT_TINPOW(tinpo[4] , 0xf7537e82);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, a, b, c, d, tinpow, 6,temp_mm);
	INSERT_TINPOW(tinpo[11] , 0xbd3af235);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, d, a, b, c, tinpow, 10,temp_mm);
	INSERT_TINPOW(tinpo[2] , 0x2ad7d2bb);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, c, d, a, b, tinpow, 15,temp_mm);
	INSERT_TINPOW( tinpo[9] , 0xeb86d391);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, b, c, d, a, tinpow, 21,temp_mm);
	_asm emms

#undef a 
#undef b 
#undef c 
#undef d 
#undef temp_mm

}

#endif 
