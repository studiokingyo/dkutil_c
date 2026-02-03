
/*!
@file dkcCRC.c
@author d金魚
@since 2004/08/24
*/

#define DKUTIL_C_CRC_C
#include "dkcCRC.h"

typedef ULONG value_type;
typedef USHORT value_type16;

/*size_t a = (0x00000001 << Bits);
size_t b = (0xFFFFFFFF >> 32 - Bits);
size_t result = a & b;
(0x00000001 << Bits) & (0xFFFFFFFF >> 32 - Bits)
*/
static DKC_INLINE value_type mask_value( size_t Bits){
	//return (~(0x80000000 >> (Bits )) << 1) + 1;
	//size_t a = (0x00000001 << Bits);
	size_t b = (0xFFFFFFFF >> (32 - Bits));
	//size_t result = a & b;
	return b;

	//return (0x00000001 << Bits) & (0xFFFFFFFF >> (32 - Bits));
}


/// Compare a byte to the remainder's highest byte
static  unsigned char  index( value_type rem, unsigned char x ,size_t Bits,BOOL DoReflect)
{
	return (BYTE)(x ^ ( rem >> (DoReflect ? 0u : Bits - CHAR_BIT) ));
}

/// Shift out the remainder's highest byte
static  value_type  shift( value_type rem ,BOOL DoReflect)
{
	return DoReflect ? rem >> CHAR_BIT : rem << CHAR_BIT; 
}
/*
static  value_type  shift( value_type rem )
{ 
	return rem << CHAR_BIT; 
}

static  unsigned char  index( value_type rem, unsigned char x ,size_t Bits)
{ 
	return (unsigned char)(x ^ ( rem >> (Bits - CHAR_BIT) )); 
}
*/
static value_type reflector(size_t Bits,value_type x){
	value_type        reflection = 0;
  value_type const  one = 1;
	size_t i;

  for ( i = 0 ; i < Bits ; ++i, x >>= 1 )
  {
      if ( x & one )
      {
          reflection |= ( one << (Bits - 1u - i) );
      }
  }


  //return reflection;
	//return reflection & (~(0x80000000 >> (Bits )) << 1) + 1;
	
	return reflection & mask_value(Bits);
}

static value_type reflection(BOOL DoReflect,size_t Bits,value_type x){
	return DoReflect ? reflector( Bits,x ) : x;
	
}

/// CRC table generator 32bit version
static int makecrctable32(ULONG *crctable,size_t Bits,ULONG TruncPoly,BOOL Reflect)
{

  // factor-out constants to avoid recalculation
  const value_type     fast_hi_bit = 1ul << ( Bits - 1u );
  unsigned char const  byte_hi_bit = 1u << (CHAR_BIT - 1u);

  // loop over every possible dividend value
  unsigned char  dividend = 0;
	value_type  remainder ;
	unsigned char mask;

	if(Bits > 32){
		return edk_FAILED;
	}

  do
  {
		remainder = 0;
    

    // go through all the dividend's bits
    for ( mask = byte_hi_bit ; mask ; mask >>= 1 )
    {
      // check if divisor fits
      if ( dividend & mask )
      {
          remainder ^= fast_hi_bit;
      }

      // do polynominal division
      if ( remainder & fast_hi_bit )
      {
          remainder <<= 1;
          remainder ^= TruncPoly;
      }
      else
      {
          remainder <<= 1;
      }
    }

    //crctable[ crc_helper<CHAR_BIT, Reflect>::reflect(dividend) ]
    // = crc_helper<Bits, Reflect>::reflect( remainder );
		crctable[ reflection(Reflect,CHAR_BIT,dividend) ]
			= reflection(Reflect,Bits,remainder);
  }
  while ( ++dividend );

	return edk_SUCCEEDED;
}

/// CRC table generator 16bit version
static int makecrctable16(USHORT *crctable,size_t Bits,ULONG TruncPoly,BOOL Reflect)
{


  // factor-out constants to avoid recalculation
  const value_type16     fast_hi_bit = (const value_type16)(1u << ( Bits - 1u ));
  unsigned char const  byte_hi_bit = 1u << (CHAR_BIT - 1u);

  // loop over every possible dividend value
  unsigned char  dividend = 0;
	value_type16  remainder ;
	unsigned char mask;

	if(Bits > 16){
		return edk_FAILED;
	}

  do
  {
		remainder = 0;
    

    // go through all the dividend's bits
    for ( mask = byte_hi_bit ; mask ; mask >>= 1 )
    {
      // check if divisor fits
      if ( dividend & mask )
      {
          remainder ^= fast_hi_bit;
      }

      // do polynominal division
      if ( remainder & fast_hi_bit )
      {
          remainder <<= 1;
          remainder ^= TruncPoly;
      }
      else
      {
          remainder <<= 1;
      }
    }

    //crctable[ crc_helper<CHAR_BIT, Reflect>::reflect(dividend) ]
    // = crc_helper<Bits, Reflect>::reflect( remainder );
		crctable[ reflection(Reflect,CHAR_BIT,dividend) ]
			= (USHORT)reflection(Reflect,Bits,remainder);
  }
  while ( ++dividend );

	return edk_SUCCEEDED;
}
/*
static Loader32(DKC_CRC *p,const BYTE *pBuffer,size_t size){

}
static Loader16(DKC_CRC *p,const BYTE *pBuffer,size_t size){

}*/

/* 勝手にプール */

static USHORT *get_crc16left(){

}

static USHORT *get_crc16right(){

}
static ULONG *get_crc32left(){

}
static ULONG *get_crc32right(){

}

static USHORT *get_crc_ccitt(){

}
static USHORT *get_crc_xmodem(){

}

size_t WINAPI dkcCRCGetUseTableSize(size_t Bits){
	//手抜き演算～～
	size_t t; 
	if(Bits <= 16){
		t = sizeof(USHORT) * 0x100;
	}else if(Bits <= 32){
		t = sizeof(ULONG) * 0x100;
	}else{
		t = 0;
	}
	return t;
}

DKC_CRC *WINAPI dkcAllocCRC( size_t Bits, ULONG TruncPoly,
           ULONG InitRem, ULONG FinalXor,
           dkctBOOL ReflectIn, dkctBOOL ReflectRem)
{
	DKC_CRC *p = (DKC_CRC *)dkcAllocate(sizeof(DKC_CRC));
	void *pt;

	if(NULL==p){
		return NULL;
	}
	pt = dkcAllocate(dkcCRCGetUseTableSize(Bits));
	if(NULL==pt){
		goto Error;
	}
	dkcCRCInit(p,pt,Bits,TruncPoly,InitRem,FinalXor,ReflectIn,ReflectRem);
	
	return p;

Error:
	dkcFree((void **)&p);
	return NULL;
}

DKC_CRC* WINAPI dkcAllocCRC_32left(){
	return dkcAllocCRC(32, 0xEDB88320, 0xFFFFFFFF, 0xFFFFFFFF, FALSE, FALSE);

}

DKC_CRC* WINAPI dkcAllocCRC_32right(){
	//バグ ... 値を治す
	return dkcAllocCRC(32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, TRUE);
}
DKC_CRC* WINAPI dkcAllocCRC_ansi16left(){
	return dkcAllocCRC(16, 0x8005, 0, 0, TRUE,TRUE);
}

DKC_CRC* WINAPI dkcAllocCRC_ansi16right(){
	return dkcAllocCRC(16, 0xa001, 0, 0, FALSE,FALSE);
}

DKC_CRC* WINAPI dkcAllocCRC_ccitt_left(){
	return dkcAllocCRC(16, 0x1021, 0xFFFF, 0, FALSE,FALSE);
}
DKC_CRC* WINAPI dkcAllocCRC_ccitt_right(){
	return dkcAllocCRC(16, 0x8005, 0xFFFF, 0, TRUE,TRUE);
}
DKC_CRC *WINAPI dkcAllocCRC_7left(){
	return dkcAllocCRC(16, 0x51, 0xFFFF, 0, FALSE,FALSE);
}
DKC_CRC *WINAPI dkcAllocCRC_7right(){
	return dkcAllocCRC(16, 0x8a, 0xFFFF, 0, TRUE,TRUE);
}

DKC_CRC* WINAPI dkcAllocCRC_xmodem(){
	return dkcAllocCRC(16, 0x8408, 0, 0, TRUE,TRUE);
}
/*
bool CheckTable(DKC_CRC *p)
{
	int i;
		//手抜き演算～～
	size_t t; 
	if(Bits <= 16){
		t = sizeof(USHORT) * 0x100;
	}else if(Bits <= 32){
		t = sizeof(ULONG) * 0x100;
	}else{
		t = 0;
	}
	dkcCRCGetUseTableSize(p->mBits)
	for(i=0;i<;i++){
		DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_32left();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_32right();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_ansi16left();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_ansi16right();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_ccitt_left();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_ccitt_right();

DKC_EXTERN DKC_CRC* WINAPI dkcAllocCRC_xmodem();	

	}

	dkcCRCFree(&p);
}

dkcCheckTable(HOGE){
	
}

DKC_CRC *WINAPI dkcAllocCRC_
*/

int WINAPI dkcFreeCRC(DKC_CRC **pp){
	dkcFree((void **)&((*pp)->mTable));
	return dkcFree((void **)pp);
}

//int WINAPI dkcCRCInit(DKC_CRC *p,ULONG buffer[256],
int WINAPI dkcCRCInit(DKC_CRC *p,void *buffer,
					 size_t Bits, ULONG TruncPoly,
           ULONG InitRem, ULONG FinalXor,
           dkctBOOL ReflectIn, dkctBOOL ReflectRem )
{
	int r;

	//初期値
	p->mR = reflection(ReflectIn,Bits,InitRem);
	//処理結果
	p->mResult = 0;
	//テーブルへのポインタ
	p->mTable = buffer;
	
	p->mBits = Bits;
	p->mTruncPoly = TruncPoly;
	p->mInitRem = InitRem;
	p->mFinalXor = FinalXor;
	p->mReflectIn = ReflectIn;
	p->mReflectRem = ReflectRem;



	if(Bits <= 16){
		r = makecrctable16((USHORT *)buffer,Bits,TruncPoly,ReflectIn);
		//p->mLoader = Loader16;
	}else if(Bits <= 32){
		r = makecrctable32((ULONG *)buffer,Bits,TruncPoly,ReflectIn);
		//p->mLoader = Loader32;
	}else{
		r = edk_FAILED;
	}
	return r;

}

void WINAPI dkcCRCLoad(DKC_CRC *p,const BYTE *pBuffer,size_t size)
{
    unsigned char const * const  b = (unsigned char const *)pBuffer;
    dkcCRCLoadBlock(p, b, b + size );

}

typedef union dkc_local_ptr_offset_union{
	ULONG *pt32;
	USHORT *pt16;
}OFFSET_PTR_UNION;

typedef union dkc_local_offset_union{
	ULONG t32;
	USHORT t16;
}OFFSET_UNION;


void WINAPI dkcCRCLoadBlock(DKC_CRC *pobj,const void *bytes_begin,const void *bytes_end)
{
	unsigned char const * p = (unsigned char const *)bytes_begin;

  unsigned char byte_index;
	//for 32bit
	//ULONG *pTable32 = (ULONG *)pobj->mTable;
	//for 16bit
	//USHORT *pTable16 = (ULONG *)pobj->mTable;



	//general purpose
	size_t Bits = pobj->mBits;
	BOOL DoReflect = pobj->mReflectIn;

	USHORT *usp;
	ULONG *ulp;
	ULONG rem;

	ulp = (ULONG *)pobj->mTable;
	usp = (USHORT *)ulp;

	//もし、おかしな値になったらエンディアン系を疑ってネ＾＾；
	rem = pobj->mR;


	if(Bits <= 16)
	{//16bit filter
		// Recompute the CRC for each byte passed
		for (; p < (const BYTE *)bytes_end ; ++p )
		{
			// Compare the new byte with the remainder's higher bits to
			// get the new bits, shift out the remainder's current higher
			// bits, and update the remainder with the polynominal division
			// of the new bits.
			byte_index = index( rem, *p ,Bits,DoReflect);
			rem = (USHORT)shift( rem , DoReflect);
			rem ^= usp[ byte_index ];
		}
		
		pobj->mR = rem & mask_value(Bits);

	}else if(Bits <= 32)
	{//32bit filter
		for (; p < (const BYTE *)bytes_end ; ++p )
		{

			byte_index = index( rem, *p ,Bits,DoReflect);
			rem = shift( rem ,DoReflect);
			rem ^= ulp[ byte_index ];
		}
		pobj->mR = rem;
	}else{
		dkcmFORCE_NOT_ASSERT("dkcCRCLoadBlock中に32ビット以上の計算を要求された");
	}
	/*
	OFFSET_PTR_UNION uni;
	OFFSET_UNION rem;

	uni.pt32 = (ULONG *)pobj->mTable;
	//もし、おかしな値になったらエンディアン系を疑ってネ＾＾；
	rem.t32 = pobj->mR;


	if(Bits <= 16)
	{//16bit filter
		// Recompute the CRC for each byte passed
		for (; p < (const BYTE *)bytes_end ; ++p )
		{
			// Compare the new byte with the remainder's higher bits to
			// get the new bits, shift out the remainder's current higher
			// bits, and update the remainder with the polynominal division
			// of the new bits.
			byte_index = index( rem.t16, *p ,Bits,DoReflect);
			rem.t16 = (USHORT)shift( rem.t16 , DoReflect);
			rem.t16 ^= uni.pt16[ byte_index ];
		}

		pobj->mR = rem.t16;

	}else if(Bits <= 32)
	{//32bit filter
		for (; p < (const BYTE *)bytes_end ; ++p )
		{

			byte_index = index( rem.t32, *p ,Bits,DoReflect);
			rem.t32 = shift( rem.t32 ,DoReflect);
			rem.t32 ^= uni.pt32[ byte_index ];
		}
		pobj->mR = rem.t32;
	}else{
		dkcmFORCE_NOT_ASSERT("dkcCRCLoadBlock中に32ビット以上の計算を要求された");
	}
	*/

	
	//	rem.t32 & mask_value(Bits);
}

static DKC_INLINE ULONG get_sigbit(size_t Bits)
{
	return (( (BYTE)1u ) << ( Bits - 1u )) ;
	//return (~( ~( 0ul ) << Bits )) ;
	//return (~( ~(least( 0u )) << Bits ))
}
ULONG WINAPI dkcCRCFinal(DKC_CRC *p){
	BOOL b = (p->mReflectRem != p->mReflectIn);
	size_t Bits = p->mBits;
	ULONG ult = mask_value(Bits);
		//~(0x80000000 >> (Bits ));//( ( 1u ) << ( get_sigbit(Bits) - 1u ) );
	//変数の再利用･･･。本当はしたくないのだが･･･。
	Bits = ( reflection(b,Bits,p->mR) ^ p->mFinalXor ) & ult;
	
	p->mResult = Bits;

	return Bits;
	
	
	//reflect_out_type::reflect(rem_) ^ get_final_xor_value() )
  //   & masking_type::sig_bits_fast;
}

ULONG WINAPI dkcCRCResult(const DKC_CRC *p){
	return p->mResult;
}
