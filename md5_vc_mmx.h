/**
@auther d金魚
@brief MMX md5 routine to load with double / only for VC
@note
A license of this source code is NYSL.
http://www.kmonos.net/nysl/
http://www.kmonos.net/nysl/index.en.html

@section MD5_VC_MMX_H_history history
2005/12/28:完成！
2005/12/16:製作開始。
  */
#ifndef MD5_VC_MMX_H
#define MD5_VC_MMX_H


//#define INDEPENDENT_MD5_VC_MMX_H

#ifdef INDEPENDENT_MD5_VC_MMX_H
#include <stdio.h>
#define uint32 unsigned long
#define uint16 unsigned short
#define uint8 unsigned char
/**
@param m[out] MMX register
@param a[in] double word 
  */
#define MMX_REGISTER_PUSH32(m,a)\
	_asm	movd m,a\
	_asm	punpckldq m,m
#else
#include "dkcOSIndependent.h"
#include "vc_asm_misc.h"
#endif



/* #define MD5_VC_MMX_F1(x, y, z) (x & y | ~x & z) non optimize  */

/**
#define MD5_VC_MMX_F1(x, y, z) (z ^ (x & (y ^ z)))
d:dest x:mm0 y:mm1 z:mm2
   */
#define MD5_VC_MMX_F1(d,x,y,z)\
	_asm movq d,y\
	_asm pxor d,z\
	_asm pand d,x\
	_asm pxor d,z

#define MD5_VC_MMX_F2(d,x, y, z) MD5_VC_MMX_F1(d,z, x, y)

///#define MD5_VC_MMX_F3(x, y, z) (x ^ y ^ z)
#define MD5_VC_MMX_F3(d,x,y,z)\
	_asm movq d,y\
	_asm pxor d,z\
	_asm pxor d,x



///#define MD5_VC_MMX_F4(x, y, z) (y ^ (x | ~z))
	/*_asm pandn z,0xFFFFFFFFFFFFFFFF\  */
#define MD5_VC_MMX_F4(d,x,y,z)\
	_asm movq d,z\
	_asm pandn d,mm5\
	_asm por		d,x\
	_asm pxor	d,y



/* This is the central step in the MD5 algorithm. 
#define MD5_VC_MMX_STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )
		core										rotate									plus
  */

///out w
#define MD5_VC_MMX_CORE(temp,w,f,x,y,z,data)\
	f(temp,x,y,z)\
	_asm paddd temp,data\
	_asm paddd w,temp


///out d
///w = w<<s | w>>(32-s)
#define MD5_VC_MMX_ROTATE(d,w,s)\
	_asm movq	d,w\
	_asm psllq d,s\
	_asm psrlq w,32-s\
	_asm por		d,w

#define MD5_VC_MMX_ROTATE2(d,a,s)\
	_asm movq d,a\
	_asm pslld d,s\
	_asm psrld a,32-s\
	_asm por d,a

///lastout w,temp
#define MD5_VC_MMX_STEP(f, w, x, y, z, data, s,temp) \
	MD5_VC_MMX_CORE(temp,w,f,x,y,z,(data))\
	MD5_VC_MMX_ROTATE2(temp,w,s)\
	_asm paddd temp,x\
	_asm movq w,temp


DKC_INLINE uint64 MD5_VC_MMX_UINT32_TO_UINT64(uint32 a,uint32 b){
	ULARGE_INTEGER inte;
	inte.LowPart = a;
	inte.HighPart = b;
	return inte.QuadPart;
}
DKC_INLINE void MD5_VC_MMX_UINT64_TO_UINT32(uint64 s,uint32 *a,uint32 *b)
{
	ULARGE_INTEGER inte;
	inte.QuadPart = s;
	*a = inte.LowPart;
	*b = inte.HighPart;

}
///out mm6 / use mm7 : @note MMX複雑すぎて壊れました。
#define INSERT_TINPOW(mm,tempmm,tinpo,data)\
	_asm mov eax,data\
	MMX_REGISTER_PUSH32(mm,eax)\
	_asm movq tempmm,tinpo\
	_asm paddd mm,tempmm

	/*_asm	mov eax,data\
	_asm	movd mm6,eax\
	_asm	movd mm7,eax\
	_asm	punpckldq mm7,mm6\
	_asm	movq mm6,tinpo\
	_asm	paddd mm6,mm7 */
/**
mmxが使える状態かどうか各自でチェックしてください。
sizeは64にしてください。
 */
static DKC_INLINE void md5_mmx_double_update(
	/*uint32 abcd0[4],
	uint32 abcd1[4], */
	uint32 *abcd0,
	uint32 *abcd1,
 const uint32 in0[16],
 const uint32 in1[16])
{
	uint64 a,b,c,d;
	uint64 tinpo[16];
	
	const uint64 tempv = 0xFFFFFFFFFFFFFFFF;
	int i;
	uint64 *ptinpo;
	//abcd[0] = MD5_VC_MMX_UINT32_TO_UINT64(1,1);
	
	a = MD5_VC_MMX_UINT32_TO_UINT64(abcd0[0],abcd1[0]);
	b = MD5_VC_MMX_UINT32_TO_UINT64(abcd0[1],abcd1[1]);
	c = MD5_VC_MMX_UINT32_TO_UINT64(abcd0[2],abcd1[2]);
	d = MD5_VC_MMX_UINT32_TO_UINT64(abcd0[3],abcd1[3]);

	for(i=0;i<16;i++){
		tinpo[i] = MD5_VC_MMX_UINT32_TO_UINT64(in0[i],in1[i]);
	}
	ptinpo = tinpo;
	
	__asm{

	_asm	movq mm0,a
	_asm	movq mm1,b
	_asm	movq mm2,c
	_asm	movq mm3,d
	//_asm movq mm4,tinpo[3]

	
#define tinpow mm4
#define temp_mm mm6
//mm5 = 0xFFFFFFFFFFFFFFFF
	_asm	movq mm5, tempv


	mov		esi,ptinpo
	//_asm movq mm4,[esi+3*8]
	//[esi+0*8]
	

	INSERT_TINPOW(tinpow,temp_mm,[esi+0*8],0xd76aa478);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm0, mm1, mm2, mm3, tinpow, 7,temp_mm);
  INSERT_TINPOW(tinpow,temp_mm,[esi+1*8] ,0xe8c7b756);
	MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm3, mm0, mm1, mm2, tinpow , 12,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+2*8] ,0x242070db);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm2, mm3, mm0, mm1, tinpow, 17,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+3*8] ,0xc1bdceee);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm1, mm2, mm3, mm0, tinpow, 22,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+4*8] , 0xf57c0faf);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm0, mm1, mm2, mm3, tinpow, 7,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+5*8] , 0x4787c62a);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm3, mm0, mm1, mm2, tinpow, 12,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+6*8] , 0xa8304613);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm2, mm3, mm0, mm1, tinpow, 17,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+7*8] , 0xfd469501);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm1, mm2, mm3, mm0, tinpow, 22,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+8*8] , 0x698098d8);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm0, mm1, mm2, mm3, tinpow, 7,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+9*8] , 0x8b44f7af);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm3, mm0, mm1, mm2, tinpow, 12,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+10*8] , 0xffff5bb1);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm2, mm3, mm0, mm1, tinpow, 17,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+11*8] , 0x895cd7be);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm1, mm2, mm3, mm0, tinpow, 22,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+12*8] , 0x6b901122);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm0, mm1, mm2, mm3, tinpow, 7,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+13*8] , 0xfd987193);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm3, mm0, mm1, mm2, tinpow, 12,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+14*8] , 0xa679438e);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm2, mm3, mm0, mm1, tinpow, 17,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+15*8] , 0x49b40821);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F1, mm1, mm2, mm3, mm0, tinpow, 22,temp_mm);

	INSERT_TINPOW(tinpow,temp_mm,[esi+1*8] , 0xf61e2562);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm0, mm1, mm2, mm3, tinpow, 5,temp_mm);
  INSERT_TINPOW(tinpow,temp_mm,[esi+6*8] , 0xc040b340);
	MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm3, mm0, mm1, mm2, tinpow, 9,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+11*8] , 0x265e5a51);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm2, mm3, mm0, mm1, tinpow, 14,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+0*8] , 0xe9b6c7aa);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm1, mm2, mm3, mm0, tinpow, 20,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+5*8] , 0xd62f105d);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm0, mm1, mm2, mm3, tinpow, 5,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+10*8] , 0x02441453);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm3, mm0, mm1, mm2, tinpow, 9,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+15*8] , 0xd8a1e681);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm2, mm3, mm0, mm1, tinpow, 14,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+4*8] , 0xe7d3fbc8);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm1, mm2, mm3, mm0, tinpow, 20,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+9*8] , 0x21e1cde6);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm0, mm1, mm2, mm3, tinpow, 5,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+14*8] , 0xc33707d6);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm3, mm0, mm1, mm2, tinpow, 9,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+3*8] , 0xf4d50d87);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm2, mm3, mm0, mm1, tinpow, 14,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm, [esi+8*8] , 0x455a14ed);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm1, mm2, mm3, mm0, tinpow, 20,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+13*8] , 0xa9e3e905);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm0, mm1, mm2, mm3, tinpow, 5,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+2*8] , 0xfcefa3f8);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm3, mm0, mm1, mm2, tinpow, 9,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+7*8] , 0x676f02d9);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm2, mm3, mm0, mm1, tinpow, 14,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+12*8] , 0x8d2a4c8a);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F2, mm1, mm2, mm3, mm0, tinpow, 20,temp_mm);

	INSERT_TINPOW(tinpow,temp_mm,[esi+5*8] , 0xfffa3942);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm0, mm1, mm2, mm3, tinpow, 4,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+8*8] , 0x8771f681);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm3, mm0, mm1, mm2, tinpow, 11,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+11*8] , 0x6d9d6122);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm2, mm3, mm0, mm1, tinpow, 16,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+14*8] , 0xfde5380c);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm1, mm2, mm3, mm0, tinpow, 23,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+1*8] , 0xa4beea44);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm0, mm1, mm2, mm3, tinpow, 4,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+4*8] , 0x4bdecfa9);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm3, mm0, mm1, mm2, tinpow, 11,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+7*8] , 0xf6bb4b60);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm2, mm3, mm0, mm1, tinpow, 16,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+10*8] , 0xbebfbc70);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm1, mm2, mm3, mm0, tinpow, 23,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+13*8] , 0x289b7ec6);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm0, mm1, mm2, mm3, tinpow, 4,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+0*8] , 0xeaa127fa);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm3, mm0, mm1, mm2, tinpow, 11,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+3*8] , 0xd4ef3085);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm2, mm3, mm0, mm1, tinpow, 16,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+6*8] , 0x04881d05);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm1, mm2, mm3, mm0, tinpow, 23,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+9*8] , 0xd9d4d039);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm0, mm1, mm2, mm3, tinpow, 4,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+12*8] , 0xe6db99e5);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm3, mm0, mm1, mm2, tinpow, 11,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+15*8] , 0x1fa27cf8);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm2, mm3, mm0, mm1, tinpow, 16,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+2*8] , 0xc4ac5665);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F3, mm1, mm2, mm3, mm0, tinpow, 23,temp_mm);

	INSERT_TINPOW(tinpow,temp_mm,[esi+0*8] , 0xf4292244);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm0, mm1, mm2, mm3, tinpow, 6,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+7*8] , 0x432aff97);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm3, mm0, mm1, mm2, tinpow, 10,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+14*8] , 0xab9423a7);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm2, mm3, mm0, mm1, tinpow, 15,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+5*8] , 0xfc93a039);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm1, mm2, mm3, mm0, tinpow, 21,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+12*8] , 0x655b59c3);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm0, mm1, mm2, mm3, tinpow, 6,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+3*8] , 0x8f0ccc92);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm3, mm0, mm1, mm2, tinpow, 10,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+10*8] , 0xffeff47d);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm2, mm3, mm0, mm1, tinpow, 15,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+1*8] , 0x85845dd1);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm1, mm2, mm3, mm0, tinpow, 21,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+8*8] , 0x6fa87e4f);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm0, mm1, mm2, mm3, tinpow, 6,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+15*8] , 0xfe2ce6e0);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm3, mm0, mm1, mm2, tinpow, 10,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+6*8] , 0xa3014314);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm2, mm3, mm0, mm1, tinpow, 15,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+13*8] , 0x4e0811a1);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm1, mm2, mm3, mm0, tinpow, 21,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+4*8] , 0xf7537e82);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm0, mm1, mm2, mm3, tinpow, 6,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+11*8], 0xbd3af235);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm3, mm0, mm1, mm2, tinpow, 10,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm,[esi+2*8], 0x2ad7d2bb);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm2, mm3, mm0, mm1, tinpow, 15,temp_mm);
	INSERT_TINPOW(tinpow,temp_mm, [esi+9*8] , 0xeb86d391);
  MD5_VC_MMX_STEP(MD5_VC_MMX_F4, mm1, mm2, mm3, mm0, tinpow, 21,temp_mm);


	paddd mm0,a
	paddd mm1,b
	paddd mm2,c
	paddd mm3,d
	movq a,mm0
	movq b,mm1
	movq c,mm2
	movq d,mm3


	//_asm emms
	}
	MD5_VC_MMX_UINT64_TO_UINT32(a,&abcd0[0],&abcd1[0]);
	MD5_VC_MMX_UINT64_TO_UINT32(b,&abcd0[1],&abcd1[1]);
	MD5_VC_MMX_UINT64_TO_UINT32(c,&abcd0[2],&abcd1[2]);
	MD5_VC_MMX_UINT64_TO_UINT32(d,&abcd0[3],&abcd1[3]);

#undef temp_mm
#undef tinpow
}

#endif 
