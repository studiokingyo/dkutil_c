
#ifndef AAAA
#define AAAA

#include "dkcOSIndependent.h"


static DKC_INLINE md5_mmx_double_update(
/* unsigned int abcd0[4],
 unsigned int abcd1[4],
 const unsigned int in0[16],
 const unsigned int in1[16]*/
 	uint32 abcd0[4],
	uint32 abcd1[4],
 const uint32 in0[16],
 const uint32 in1[16])
{
 uint64 abcd[4];
 uint64 tinpo[16];
 const uint64 tempv = 0xFFFFFFFFFFFFFFFF;
 int i;

 abcd[0] = ((abcd0[0] << 32) | abcd1[0]);
 abcd[1] = ((abcd0[1] << 32) | abcd1[1]);
 abcd[2] = ((abcd0[2] << 32) | abcd1[2]);
 abcd[3] = ((abcd0[3] << 32) | abcd1[3]);

 for(i=0;i<16;i++){
  tinpo[i] = ((in0[i] << 32) | in1[i]);
 }

 
_asm movq mm0,abcd[0]
 
_asm movq mm1,abcd[1]
 
_asm movq mm2,abcd[2]
 
_asm movq mm3,abcd[3]

 
_asm movq mm5, tempv

  
_asm pxor mm2,mm3 
_asm pand mm1,mm2 
_asm pxor mm3,mm1 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[0] + 0xd76aa478) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,7 
_asm psrlq mm0,32-7 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm1,mm2 
_asm pand mm0,mm1 
_asm pxor mm2,mm0 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[1] + 0xe8c7b756) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,12 
_asm psrlq mm3,32-12 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm0,mm1 
_asm pand mm3,mm0 
_asm pxor mm1,mm3 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[2] + 0x242070db) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,17 
_asm psrlq mm2,32-17 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm3,mm0 
_asm pand mm2,mm3 
_asm pxor mm0,mm2 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[3] + 0xc1bdceee) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,22 
_asm psrlq mm1,32-22 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pxor mm2,mm3 
_asm pand mm1,mm2 
_asm pxor mm3,mm1 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[4] + 0xf57c0faf) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,7 
_asm psrlq mm0,32-7 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm1,mm2 
_asm pand mm0,mm1 
_asm pxor mm2,mm0 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[5] + 0x4787c62a) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,12 
_asm psrlq mm3,32-12 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm0,mm1 
_asm pand mm3,mm0 
_asm pxor mm1,mm3 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[6] + 0xa8304613) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,17 
_asm psrlq mm2,32-17 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm3,mm0 
_asm pand mm2,mm3 
_asm pxor mm0,mm2 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[7] + 0xfd469501) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,22 
_asm psrlq mm1,32-22 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pxor mm2,mm3 
_asm pand mm1,mm2 
_asm pxor mm3,mm1 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[8] + 0x698098d8) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,7 
_asm psrlq mm0,32-7 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm1,mm2 
_asm pand mm0,mm1 
_asm pxor mm2,mm0 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[9] + 0x8b44f7af) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,12 
_asm psrlq mm3,32-12 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm0,mm1 
_asm pand mm3,mm0 
_asm pxor mm1,mm3 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[10] + 0xffff5bb1) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,17 
_asm psrlq mm2,32-17 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm3,mm0 
_asm pand mm2,mm3 
_asm pxor mm0,mm2 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[11] + 0x895cd7be) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,22 
_asm psrlq mm1,32-22 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pxor mm2,mm3 
_asm pand mm1,mm2 
_asm pxor mm3,mm1 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[12] + 0x6b901122) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,7 
_asm psrlq mm0,32-7 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm1,mm2 
_asm pand mm0,mm1 
_asm pxor mm2,mm0 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[13] + 0xfd987193) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,12 
_asm psrlq mm3,32-12 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm0,mm1 
_asm pand mm3,mm0 
_asm pxor mm1,mm3 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[14] + 0xa679438e) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,17 
_asm psrlq mm2,32-17 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm3,mm0 
_asm pand mm2,mm3 
_asm pxor mm0,mm2 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[15] + 0x49b40821) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,22 
_asm psrlq mm1,32-22 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;

  
_asm pxor mm1,mm2 
_asm pand mm3,mm1 
_asm pxor mm2,mm3 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[1] + 0xf61e2562) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,5 
_asm psrlq mm0,32-5 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm0,mm1 
_asm pand mm2,mm0 
_asm pxor mm1,mm2 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[6] + 0xc040b340) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,9 
_asm psrlq mm3,32-9 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm3,mm0 
_asm pand mm1,mm3 
_asm pxor mm0,mm1 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[11] + 0x265e5a51) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,14 
_asm psrlq mm2,32-14 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm2,mm3 
_asm pand mm0,mm2 
_asm pxor mm3,mm0 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[0] + 0xe9b6c7aa) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,20 
_asm psrlq mm1,32-20 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pxor mm1,mm2 
_asm pand mm3,mm1 
_asm pxor mm2,mm3 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[5] + 0xd62f105d) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,5 
_asm psrlq mm0,32-5 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm0,mm1 
_asm pand mm2,mm0 
_asm pxor mm1,mm2 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[10] + 0x02441453) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,9 
_asm psrlq mm3,32-9 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm3,mm0 
_asm pand mm1,mm3 
_asm pxor mm0,mm1 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[15] + 0xd8a1e681) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,14 
_asm psrlq mm2,32-14 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm2,mm3 
_asm pand mm0,mm2 
_asm pxor mm3,mm0 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[4] + 0xe7d3fbc8) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,20 
_asm psrlq mm1,32-20 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pxor mm1,mm2 
_asm pand mm3,mm1 
_asm pxor mm2,mm3 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[9] + 0x21e1cde6) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,5 
_asm psrlq mm0,32-5 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm0,mm1 
_asm pand mm2,mm0 
_asm pxor mm1,mm2 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[14] + 0xc33707d6) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,9 
_asm psrlq mm3,32-9 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm3,mm0 
_asm pand mm1,mm3 
_asm pxor mm0,mm1 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[3] + 0xf4d50d87) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,14 
_asm psrlq mm2,32-14 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm2,mm3 
_asm pand mm0,mm2 
_asm pxor mm3,mm0 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[8] + 0x455a14ed) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,20 
_asm psrlq mm1,32-20 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pxor mm1,mm2 
_asm pand mm3,mm1 
_asm pxor mm2,mm3 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[13] + 0xa9e3e905) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,5 
_asm psrlq mm0,32-5 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm0,mm1 
_asm pand mm2,mm0 
_asm pxor mm1,mm2 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[2] + 0xfcefa3f8) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,9 
_asm psrlq mm3,32-9 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm3,mm0 
_asm pand mm1,mm3 
_asm pxor mm0,mm1 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[7] + 0x676f02d9) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,14 
_asm psrlq mm2,32-14 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm2,mm3 
_asm pand mm0,mm2 
_asm pxor mm3,mm0 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[12] + 0x8d2a4c8a) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,20 
_asm psrlq mm1,32-20 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;

  
_asm pxor mm2,mm3 
_asm pxor mm1,mm2 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[5] + 0xfffa3942) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,4 
_asm psrlq mm0,32-4 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm1,mm2 
_asm pxor mm0,mm1 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[8] + 0x8771f681) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,11 
_asm psrlq mm3,32-11 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm0,mm1 
_asm pxor mm3,mm0 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[11] + 0x6d9d6122) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,16 
_asm psrlq mm2,32-16 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm3,mm0 
_asm pxor mm2,mm3 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[14] + 0xfde5380c) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,23 
_asm psrlq mm1,32-23 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pxor mm2,mm3 
_asm pxor mm1,mm2 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[1] + 0xa4beea44) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,4 
_asm psrlq mm0,32-4 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm1,mm2 
_asm pxor mm0,mm1 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[4] + 0x4bdecfa9) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,11 
_asm psrlq mm3,32-11 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm0,mm1 
_asm pxor mm3,mm0 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[7] + 0xf6bb4b60) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,16 
_asm psrlq mm2,32-16 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm3,mm0 
_asm pxor mm2,mm3 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[10] + 0xbebfbc70) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,23 
_asm psrlq mm1,32-23 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pxor mm2,mm3 
_asm pxor mm1,mm2 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[13] + 0x289b7ec6) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,4 
_asm psrlq mm0,32-4 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm1,mm2 
_asm pxor mm0,mm1 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[0] + 0xeaa127fa) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,11 
_asm psrlq mm3,32-11 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm0,mm1 
_asm pxor mm3,mm0 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[3] + 0xd4ef3085) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,16 
_asm psrlq mm2,32-16 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm3,mm0 
_asm pxor mm2,mm3 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[6] + 0x04881d05) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,23 
_asm psrlq mm1,32-23 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pxor mm2,mm3 
_asm pxor mm1,mm2 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[9] + 0xd9d4d039) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,4 
_asm psrlq mm0,32-4 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pxor mm1,mm2 
_asm pxor mm0,mm1 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[12] + 0xe6db99e5) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,11 
_asm psrlq mm3,32-11 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pxor mm0,mm1 
_asm pxor mm3,mm0 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[15] + 0x1fa27cf8) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,16 
_asm psrlq mm2,32-16 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pxor mm3,mm0 
_asm pxor mm2,mm3 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[2] + 0xc4ac5665) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,23 
_asm psrlq mm1,32-23 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;

  
_asm pandn mm1,mm5 
_asm por mm1,mm3 
_asm pxor mm2,mm1 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[0] + 0xf4292244) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,6 
_asm psrlq mm0,32-6 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pandn mm0,mm5 
_asm por mm0,mm2 
_asm pxor mm1,mm0 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[7] + 0x432aff97) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,10 
_asm psrlq mm3,32-10 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pandn mm3,mm5 
_asm por mm3,mm1 
_asm pxor mm0,mm3 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[14] + 0xab9423a7) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,15 
_asm psrlq mm2,32-15 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pandn mm2,mm5 
_asm por mm2,mm0 
_asm pxor mm3,mm2 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[5] + 0xfc93a039) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,21 
_asm psrlq mm1,32-21 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pandn mm1,mm5 
_asm por mm1,mm3 
_asm pxor mm2,mm1 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[12] + 0x655b59c3) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,6 
_asm psrlq mm0,32-6 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pandn mm0,mm5 
_asm por mm0,mm2 
_asm pxor mm1,mm0 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[3] + 0x8f0ccc92) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,10 
_asm psrlq mm3,32-10 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pandn mm3,mm5 
_asm por mm3,mm1 
_asm pxor mm0,mm3 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[10] + 0xffeff47d) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,15 
_asm psrlq mm2,32-15 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pandn mm2,mm5 
_asm por mm2,mm0 
_asm pxor mm3,mm2 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[1] + 0x85845dd1) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,21 
_asm psrlq mm1,32-21 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pandn mm1,mm5 
_asm por mm1,mm3 
_asm pxor mm2,mm1 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[8] + 0x6fa87e4f) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,6 
_asm psrlq mm0,32-6 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pandn mm0,mm5 
_asm por mm0,mm2 
_asm pxor mm1,mm0 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[15] + 0xfe2ce6e0) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,10 
_asm psrlq mm3,32-10 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pandn mm3,mm5 
_asm por mm3,mm1 
_asm pxor mm0,mm3 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[6] + 0xa3014314) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,15 
_asm psrlq mm2,32-15 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pandn mm2,mm5 
_asm por mm2,mm0 
_asm pxor mm3,mm2 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[13] + 0x4e0811a1) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,21 
_asm psrlq mm1,32-21 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
  
_asm pandn mm1,mm5 
_asm por mm1,mm3 
_asm pxor mm2,mm1 
_asm movq mm4,mm2 
_asm paddd mm4,(tinpo[4] + 0xf7537e82) 
_asm paddd mm0,mm4 
_asm movq mm4,mm0 
_asm psllq mm4,6 
_asm psrlq mm0,32-6 
_asm por mm4,mm0 
_asm paddd mm4,mm1 
_asm movq mm0,mm4;
  
_asm pandn mm0,mm5 
_asm por mm0,mm2 
_asm pxor mm1,mm0 
_asm movq mm4,mm1 
_asm paddd mm4,(tinpo[11] + 0xbd3af235) 
_asm paddd mm3,mm4 
_asm movq mm4,mm3 
_asm psllq mm4,10 
_asm psrlq mm3,32-10 
_asm por mm4,mm3 
_asm paddd mm4,mm0 
_asm movq mm3,mm4;
  
_asm pandn mm3,mm5 
_asm por mm3,mm1 
_asm pxor mm0,mm3 
_asm movq mm4,mm0 
_asm paddd mm4,(tinpo[2] + 0x2ad7d2bb) 
_asm paddd mm2,mm4 
_asm movq mm4,mm2 
_asm psllq mm4,15 
_asm psrlq mm2,32-15 
_asm por mm4,mm2 
_asm paddd mm4,mm3 
_asm movq mm2,mm4;
  
_asm pandn mm2,mm5 
_asm por mm2,mm0 
_asm pxor mm3,mm2 
_asm movq mm4,mm3 
_asm paddd mm4,(tinpo[9] + 0xeb86d391) 
_asm paddd mm1,mm4 
_asm movq mm4,mm1 
_asm psllq mm4,21 
_asm psrlq mm1,32-21 
_asm por mm4,mm1 
_asm paddd mm4,mm2 
_asm movq mm1,mm4;
 
_asm emms



}
#endif