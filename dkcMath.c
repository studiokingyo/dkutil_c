
/*!
@file dkcMath.c
@author d金魚
@since 2004/04/29
@note

NVIDIAのfastmath.cppの一部を利用させていただきました。
以下はfastmath.cppのオリジナルの文章です。
<PRE>
*****************************************************************************
File:  fastmath.cpp

This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************
</PRE>
*/

#define DKUTIL_C_MATH_C
#include "dkcMath.h"


DKC_INLINE void WINAPI dkcSrand(ULONG *seed,ULONG num)
{
	*seed = num;
}

DKC_INLINE int WINAPI dkcRand(ULONG *seed)
{
	(*seed) = (*seed) * 1103515245L + 12345;
	return (unsigned)((*seed) / 65536L) % 32768U;
}

DKC_INLINE int WINAPI dkcRandom(ULONG *seed,ULONG Max_)
{
	return dkcRand(seed) * (Max_ ) / SHRT_MAX;
}

DKC_INLINE uint32 WINAPI dkcPrime79(uint32 n)
{
	dkcmFORCE_NOT_ASSERT(n > 79);
	return dkcmPrime79(n);
}

/*
const static double sin360[]={
0.000000,
0.017452,0.034899,0.052336,0.069756,0.087156,0.104528,0.121869,0.139173,
0.156434,0.173648,0.190809,0.207912,0.224951,0.241922,0.258819,0.275637,
0.292372,0.309017,0.325568,0.342020,0.358368,0.374607,0.390731,0.406737,
0.422618,0.438371,0.453990,0.469472,0.484810,0.500000,0.515038,0.529919,
0.544639,0.559193,0.573576,0.587785,0.601815,0.615661,0.629320,0.642788,
0.656059,0.669131,0.681998,0.694658,0.707107,0.719340,0.731354,0.743145,
0.754710,0.766044,0.777146,0.788011,0.798636,0.809017,0.819152,0.829038,
0.838671,0.848048,0.857167,0.866025,0.874620,0.882948,0.891007,0.898794,
0.906308,0.913545,0.920505,0.927184,0.933580,0.939693,0.945519,0.951057,
0.956305,0.961262,0.965926,0.970296,0.974370,0.978148,0.981627,0.984808,
0.987688,0.990268,0.992546,0.994522,0.996195,0.997564,0.998630,0.999391,
0.999848,1.000000,0.999848,0.999391,0.998630,0.997564,0.996195,0.994522,
0.992546,0.990268,0.987688,0.984808,0.981627,0.978148,0.974370,0.970296,
0.965926,0.961262,0.956305,0.951057,0.945519,0.939693,0.933580,0.927184,
0.920505,0.913545,0.906308,0.898794,0.891007,0.882948,0.874620,0.866025,
0.857167,0.848048,0.838671,0.829038,0.819152,0.809017,0.798636,0.788011,
0.777146,0.766044,0.754710,0.743145,0.731354,0.719340,0.707107,0.694658,
0.681998,0.669131,0.656059,0.642788,0.629320,0.615661,0.601815,0.587785,
0.573576,0.559193,0.544639,0.529919,0.515038,0.500000,0.484810,0.469472,
0.453990,0.438371,0.422618,0.406737,0.390731,0.374607,0.358368,0.342020,
0.325568,0.309017,0.292372,0.275637,0.258819,0.241922,0.224951,0.207912,
0.190809,0.173648,0.156434,0.139173,0.121869,0.104528,0.087156,0.069756,
0.052336,0.034899,0.017452,0.000000,-0.017452,-0.034899,-0.052336,-0.069756,
-0.087156,-0.104528,-0.121869,-0.139173,-0.156434,-0.173648,-0.190809,-0.207912,
-0.224951,-0.241922,-0.258819,-0.275637,-0.292372,-0.309017,-0.325568,-0.342020,
-0.358368,-0.374607,-0.390731,-0.406737,-0.422618,-0.438371,-0.453990,-0.469472,
-0.484810,-0.500000,-0.515038,-0.529919,-0.544639,-0.559193,-0.573576,-0.587785,
-0.601815,-0.615661,-0.629320,-0.642788,-0.656059,-0.669131,-0.681998,-0.694658,
-0.707107,-0.719340,-0.731354,-0.743145,-0.754710,-0.766044,-0.777146,-0.788011,
-0.798636,-0.809017,-0.819152,-0.829038,-0.838671,-0.848048,-0.857167,-0.866025,
-0.874620,-0.882948,-0.891007,-0.898794,-0.906308,-0.913545,-0.920505,-0.927184,
-0.933580,-0.939693,-0.945519,-0.951057,-0.956305,-0.961262,-0.965926,-0.970296,
-0.974370,-0.978148,-0.981627,-0.984808,-0.987688,-0.990268,-0.992546,-0.994522,
-0.996195,-0.997564,-0.998630,-0.999391,-0.999848,-1.000000,-0.999848,-0.999391,
-0.998630,-0.997564,-0.996195,-0.994522,-0.992546,-0.990268,-0.987688,-0.984808,
-0.981627,-0.978148,-0.974370,-0.970296,-0.965926,-0.961262,-0.956305,-0.951057,
-0.945519,-0.939693,-0.933580,-0.927184,-0.920505,-0.913545,-0.906308,-0.898794,
-0.891007,-0.882948,-0.874620,-0.866025,-0.857167,-0.848048,-0.838671,-0.829038,
-0.819152,-0.809017,-0.798636,-0.788011,-0.777146,-0.766044,-0.754710,-0.743145,
-0.731354,-0.719340,-0.707107,-0.694658,-0.681998,-0.669131,-0.656059,-0.642788,
-0.629320,-0.615661,-0.601815,-0.587785,-0.573576,-0.559193,-0.544639,-0.529919,
-0.515038,-0.500000,-0.484810,-0.469472,-0.453990,-0.438371,-0.422618,-0.406737,
-0.390731,-0.374607,-0.358368,-0.342020,-0.325568,-0.309017,-0.292372,-0.275637,
-0.258819,-0.241922,-0.224951,-0.207912,-0.190809,-0.173648,-0.156434,-0.139173,
-0.121869,-0.104528,-0.087156,-0.069756,-0.052336,-0.034899,-0.017452,};//end of sin360



DKC_INLINE int dkcRadianToDegree(double rad){
	
}


DKC_INLINE double dkcSin256(double f){
	return sin256[dkcRadianToDegree(f)];
}

DKC_INLINE *dkc

*/

#if 0
//以下fastmath.cppより引用　ライセンスはオリジナルに準拠する



#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000


// r = 1/p
#define FP_INV(r,p)                                                          \
{                                                                            \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
    r = *(float *)&_i;                                                       \
    r = r * (2.0f - (p) * r);                                                \
}

/////////////////////////////////////////////////
// The following comes from Vincent Van Eeckhout
// Thanks for sending us the code!
// It's the same thing in assembly but without this C-needed line:
//    r = *(float *)&_i;

static float   two = 2.0f;

#define FP_INV2(r,p)                     \
{                                        \
    __asm { mov     eax,0x7F000000    }; \
    __asm { sub     eax,dword ptr [p] }; \
    __asm { mov     dword ptr [r],eax }; \
    __asm { fld     dword ptr [p]     }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fsubr   [two]             }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fstp    dword ptr [r]     }; \
}

/////////////////////////////////////////////////


#define FP_EXP(e,p)                                                          \
{                                                                            \
    int _i;                                                                  \
    e = -1.44269504f * (float)0x00800000 * (p);                              \
    _i = (int)e + 0x3F800000;                                                \
    e = *(float *)&_i;                                                       \
}

#define FP_NORM_TO_BYTE(i,p)                                                 \
{                                                                            \
    float _n = (p) + 1.0f;                                                   \
    i = *(int *)&_n;                                                         \
    if (i >= 0x40000000)     i = 0xFF;                                       \
    else if (i <=0x3F800000) i = 0;                                          \
    else i = ((i) >> 15) & 0xFF;                                             \
}



DKC_INLINE unsigned long FP_NORM_TO_BYTE2(float p)                                                 
{                                                                            
  float fpTmp = p + 1.0f;                                                      
  return ((*(unsigned *)&fpTmp) >> 15) & 0xFF;  
}


DKC_INLINE unsigned long FP_NORM_TO_BYTE3(float p)     
{
  float ftmp = p + 12582912.0f;                                                      
  return ((*(unsigned long *)&ftmp) & 0xFF);
}

static unsigned int fast_sqrt_table[0x10000];  // declare table of square roots 


static void  build_sqrt_table()
{
  unsigned int i;
  FastSqrtUnion s;
  
  for (i = 0; i <= 0x7FFF; i++)
  {
    
    // Build a float with the bit pattern i as mantissa
    //  and an exponent of 0, stored as 127
    
    s.i = (i << 8) | (0x7F << 23);
    s.f = (float)sqrt(s.f);
    
    // Take the square root then strip the first 7 bits of
    //  the mantissa into the table
    
    fast_sqrt_table[i + 0x8000] = (s.i & 0x7FFFFF);
    
    // Repeat the process, this time with an exponent of 1, 
    //  stored as 128
    
    s.i = (i << 8) | (0x80 << 23);
    s.f = (float)sqrt(s.f);
    
    fast_sqrt_table[i] = (s.i & 0x7FFFFF);
  }
}


typedef union FastSqrtUnion
{
  float f;
  unsigned int i;
} FastSqrtUnion;


static DKC_INLINE float fastsqrt(float n)
{
  
  if (FP_BITS(n) == 0)
    return 0.0;                 // check for square root of 0
  
  FP_BITS(n) = fast_sqrt_table[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
  
  return n;
}


// At the assembly level the recommended workaround for the second FIST bug is the same for the first; 
// inserting the FRNDINT instruction immediately preceding the FIST instruction. 

static DKC_FORCE_INLINE void FloatToInt(int *int_pointer, float f) 
{
	__asm  fld  f
  __asm  mov  edx,int_pointer
  __asm  FRNDINT
  __asm  fistp dword ptr [edx];

}


//こういうのはしたくないのだが･･･
static BYTE f = FALSE;

DKC_FORCE_INLINE void WINAPI dkcSqrtInit(){
	if(FALSE==f){
		build_sqrt_table();
		f=TRUE;
	}
}

DKC_FORCE_INLINE float WINAPI dkcSqrtFast(float a){
	dkcmNOT_ASSERT(FALSE==f);
	return fastsqrt(a);
}

DKC_FORCE_INLINE int WINAPI dkcFloatToInt(float a){

}
#endif

//**********************************************************
