/*!
@file dkcMath.h
@since 2004/04/29
@brief 超簡易　数学系ライブラリ
@note 
結果が間違っている可能性あり。
精度が悪い可能性あり。
NVIDIAのfastmath.cppを一部に使用
@todo How to do?
*/

#ifndef DKUTIL_C_MATH_H
#define DKUTIL_C_MATH_H

#include "dkcOSIndependent.h"
#include <math.h>

#ifndef M_PI
///何故定義されとらんのだ!!!ヽ(`Д´)ノムキィ
#	define M_PI 3.14159265358979323846
#endif


///C言語の標準srand()を種値（ステート）を外部に保存できるようにしたもの
DKC_EXTERN void WINAPI dkcSrand(ULONG *seed,ULONG num);
///C言語の標準rand()を種値（ステート）を外部に保存できるようにしたもの
DKC_EXTERN int WINAPI dkcRand(ULONG *seed);
///0からMax_までの乱数を出力する。
DKC_EXTERN int WINAPI dkcRandom(ULONG *seed,ULONG Max_);

DKC_EXTERN int WINAPI dkcSqrtInit();


DKC_EXTERN float WINAPI dkcSqrtFast(float a);

DKC_EXTERN int WINAPI dkcFloatToInt(float a);


typedef struct dkc_XorShiftRandomNumberGenerator{
	uint32 x,y,z,w;
}DKC_XORSHIFT_RNG;

DKC_INLINE void dkcXorShiftRNG128DefaultInit(DKC_XORSHIFT_RNG *p){
	//static unsigned long x=123456789,y=362436069,z=521288629,w=88675123;
	p->x = 123456789;
	p->y = 362436069;
	p->z = 521288629;
	p->w = 88675123;
}

///この関数が正解かどうかは不明
DKC_INLINE void dkcXorShiftRNG128Init(DKC_XORSHIFT_RNG *p,uint32 seed){
	
	p->x = seed;
	p->y = 362436069;
	p->z = 521288629;
	p->w = 88675123;
}

DKC_INLINE uint32 dkcXorShiftRNG128(DKC_XORSHIFT_RNG *p){
  uint32 t;
	uint32 x,y,z,w;
	x = p->x;y = p->y;z = p->z;w = p->w;
  t=(x^(x<<11));
	x=y;
	y=z;
	z=w;
	( w=(w^(w>>19))^(t^(t>>8)) );

	p->x = x;p->y =y;p->z = z;p->w = w;
	return w;
}




///ハッカーの楽しみ (  日本語版 Hacker's Delight ) 第16章 素数に関する式 p.290より
#define dkcmPrime79(n) ((n * n) - (79 * n) + 1601)

///param n[in] 0から79までの数を入れる @return 素数を返す @note ハッシュ関数に使うのかな？
DKC_EXTERN uint32 WINAPI dkcPrime79(uint32 n);

//**********************************************************
//三角関数系

/*
typedef struct dkc_Integer_Trigonomic_Function{
	


}DKC_INT_TRIG_FUNC;

typedef struct dkc_Decimal_Trigonomic_Function{


}DKC_DEC_TRIG_FUNC;
*/

//**********************************************************





#if !defined( DKUTIL_C_MATH_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)

#	include "dkcMath.c"
#endif

#endif