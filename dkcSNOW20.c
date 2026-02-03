
/**
@file dkcSNOW20.c
@brief SNOW2.0 steram cipher Algorithm
@since 2004/12/31
@note
2005/09/06:内部実装を一新 本家では出来なかった1byte単位の暗号化を可能にする。

*/



#include "dkcSNOW20.h"
#include "snow2tab.h"

#include <stdlib.h>




#ifdef ORDER_DCBA //little endian win32
#define snow2_key_shake(key,begin_offset) \
	(((uint32)*(key+begin_offset))<<24) |		\
	(((uint32)*(key+1+begin_offset))<<16) |	\
	(((uint32)*(key+2+begin_offset))<<8) |		\
	(((uint32)*(key+3+begin_offset)))				\

#else if defined(ORDER_ABCD) // big endian motorola

#define snow2_key_shake(key,begin_offset) \
	(((uint32)*(key+begin_offset))<<24) |		\
	(((uint32)*(key+1+begin_offset))<<16) |	\
	(((uint32)*(key+2+begin_offset))<<8) |		\
	(((uint32)*(key+3+begin_offset)))				\
#else if defined(ORDER_BADC) // vix ?

#endif


#define ainv_mul(w) (((w)>>8)^(snow_alphainv_mul[w&0xff]))
#define a_mul(w)    (((w)<<8)^(snow_alpha_mul[w>>24]))


#define snow_byte(n,w) (dkcmGETBYTE(w,n) & 0xff)


DKC_INLINE int WINAPI dkcSNOW2_64Byte(DKC_SNOW2 *p,uint32 *keystream_block){
	//uint32 *snow_ptr = p->snow_ptr;
	uint32 fsmtmp,*sw = p->sw;
	uint32 r1 = p->r1;
	uint32 r2 = p->r2;

	dkcmASSERT(p->store_size == 0);

  sw[0] =a_mul(sw[0] )^ sw[2] ^ainv_mul(sw[11] );
  fsmtmp=r2+ sw[5] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[0]=(r1+ sw[0] )^r2^ sw[1] ;

  sw[1] =a_mul(sw[1] )^ sw[3] ^ainv_mul(sw[12]);
  fsmtmp=r2+ sw[6] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[1]=(r1+ sw[1] )^r2^ sw[2] ;

  sw[2] =a_mul(sw[2] )^ sw[4] ^ainv_mul(sw[13] );
  fsmtmp=r2+ sw[7] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[2]=(r1+ sw[2] )^r2^ sw[3] ;

  sw[3] =a_mul(sw[3] )^ sw[5] ^ainv_mul(sw[14] );
  fsmtmp=r2+ sw[8] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[3]=(r1+ sw[3] )^r2^ sw[4] ;

  sw[4 ]=a_mul(sw[4] )^ sw[6] ^ainv_mul(sw[15] );
  fsmtmp=r2+ sw[9] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[4]=(r1+ sw[4] )^r2^ sw[5] ;

  sw[5] =a_mul(sw[5] )^ sw[7] ^ainv_mul(sw[0] );
  fsmtmp=r2+ sw[10] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[5]=(r1+ sw[5] )^r2^ sw[6] ;

  sw[6] =a_mul(sw[6] )^ sw[8] ^ainv_mul(sw[1] );
  fsmtmp=r2+ sw[11] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[6]=(r1+ sw[6] )^r2^ sw[7] ;

  sw[7] =a_mul(sw[7] )^ sw[9] ^ainv_mul(sw[2] );
  fsmtmp=r2+ sw[12 ];
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[7]=(r1+ sw[7] )^r2^ sw[8] ;

  sw[8] =a_mul(sw[8] )^ sw[10] ^ainv_mul(sw[3] );
  fsmtmp=r2+ sw[13] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp; 
  keystream_block[8]=(r1+ sw[8] )^r2^ sw[9] ;

  sw[9] =a_mul(sw[9] )^ sw[11] ^ainv_mul(sw[4] );
  fsmtmp=r2+ sw[14] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[9]=(r1+ sw[9] )^r2^ sw[10] ;

  sw[10] =a_mul(sw[10] )^ sw[12] ^ainv_mul(sw[5] );
  fsmtmp=r2+ sw[15] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[10]=(r1+ sw[10] )^r2^ sw[11] ;

  sw[11] =a_mul(sw[11] )^ sw[13] ^ainv_mul(sw[6] );
  fsmtmp=r2+ sw[0] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;    
  keystream_block[11]=(r1+ sw[11] )^r2^ sw[12] ;

  sw[12] =a_mul(sw[12] )^ sw[14] ^ainv_mul(sw[7] );
  fsmtmp=r2+ sw[1 ];
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;
  keystream_block[12]=(r1+ sw[12] )^r2^ sw[13] ;

  sw[13] =a_mul(sw[13] )^ sw[15] ^ainv_mul(sw[8] );
  fsmtmp=r2+ sw[2] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;   
  keystream_block[13]=(r1+ sw[13] )^r2^ sw[14] ;

  sw[14] =a_mul(sw[14] )^ sw[0] ^ainv_mul(sw[9] );
  fsmtmp=r2+ sw[3] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp; 
  keystream_block[14]=(r1+ sw[14] )^r2^ sw[15] ;

  sw[15] =a_mul(sw[15] )^ sw[1] ^ainv_mul(sw[10] );
  fsmtmp=r2+ sw[4] ;
  r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
  r1=fsmtmp;    
  keystream_block[15]=(r1+ sw[15] )^r2^ sw[0] ;
	//update
	p->r1 = r1;
	p->r2 = r2;
	//p->snow_ptr = snow_ptr;
	//update_sw512(p);
	return edk_SUCCEEDED;
}

static DKC_INLINE int store(DKC_SNOW2 *p){
	int r;
	dkcmNOT_ASSERT(0 != p->store_size);
	if(0 != p->store_size) return edk_LogicError;
	r = dkcSNOW2_64Byte(p,p->u_store.a32);
	p->store_size = 64;
	return r;
}

int WINAPI dkcSNOW2Init(DKC_SNOW2 *p,uint8 *key,size_t keysize,uint32 IV3,uint32 IV2,uint32 IV1,uint32 IV0)
{
	int i;

	uint32 s15,s14,s13,s12,s11,s10,s9,s8,s7,s6,s5,s4,s3,s2,s1,s0;
	uint32 outfrom_fsm,fsmtmp,r1,r2;

	if(NULL==p || NULL==key){
		return edk_ArgumentException;
	}
	switch(keysize)
	{
		case 16:
			p->sw[15] = snow2_key_shake(key,0);
			p->sw[14] = snow2_key_shake(key,4);
			p->sw[13] = snow2_key_shake(key,8);
			p->sw[12] = snow2_key_shake(key,12);
			p->sw[11] = ~p->sw[15]; /* bitwip->sw[e inverp->sw[e */
			p->sw[10] = ~p->sw[14];
			p->sw[9]  = ~p->sw[13];
			p->sw[8]  = ~p->sw[12];
			p->sw[7]  = p->sw[15]; /* jup->sw[t copy */ 
			p->sw[6]  = p->sw[14];
			p->sw[5]  = p->sw[13];
			p->sw[4] = p->sw[12];
			p->sw[3]  =~p->sw[15]; /* bitwip->sw[e inverp->sw[e */
			p->sw[2]  =~p->sw[14];
			p->sw[1]  =~p->sw[13];
			p->sw[0]  =~p->sw[12];
			break;
		case 32:
			p->sw[15] = snow2_key_shake(key,0);
			p->sw[14] = snow2_key_shake(key,4);
			p->sw[13] = snow2_key_shake(key,8);
			p->sw[12] = snow2_key_shake(key,12);
			p->sw[11] = snow2_key_shake(key,16);
			p->sw[10] = snow2_key_shake(key,20);
			p->sw[9]  = snow2_key_shake(key,24);
			p->sw[8]  = snow2_key_shake(key,28);
			p->sw[7] =~p->sw[15]; // bitwip->sw[e inverp->sw[e 
			p->sw[6] =~p->sw[14];
			p->sw[5] =~p->sw[13];
			p->sw[4] =~p->sw[12];
			p->sw[3] =~p->sw[11]; 
			p->sw[2] =~p->sw[10];
			p->sw[1] =~p->sw[9];
			p->sw[0] =~p->sw[8];
		break;
		default:
			return edk_ArgumentException;
	}
 
	
	// XOR IV values 
	p->sw[15]^=IV0;
	p->sw[12]^=IV1;
	p->sw[10]^=IV2;
	p->sw[9] ^=IV3;
 


  p->r1=0;
  p->r2=0;

	
	s15 = p->sw[15];
	s14 = p->sw[14];
	s13 = p->sw[13];
	s12 = p->sw[12];
	s11 = p->sw[11];
	s10 = p->sw[10];
	s9 = p->sw[9];
	s8 = p->sw[8];
	s7 = p->sw[7];
	s6 = p->sw[6];
	s5 = p->sw[5];
	s4 = p->sw[4];
	s3 = p->sw[3];
	s2 = p->sw[2];
	s1 = p->sw[1];
	s0 = p->sw[0];
	outfrom_fsm = p->outfrom_fsm;
	r1 = p->r1;
	r2 = p->r2;
	

	for(i=0;i<2;i++)
	{

    outfrom_fsm=(r1+ s15 )^r2;
    s0 =a_mul(s0 )^ s2 ^ainv_mul(s11 )^outfrom_fsm;
    fsmtmp=r2+ s5 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s0 )^r2;
    s1 =a_mul(s1 )^ s3 ^ainv_mul(s12 )^outfrom_fsm;
    fsmtmp=r2+ s6 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s1 )^r2;
    s2 =a_mul(s2 )^ s4 ^ainv_mul(s13 )^outfrom_fsm;
    fsmtmp=r2+ s7 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s2 )^r2;
    s3 =a_mul(s3 )^ s5 ^ainv_mul(s14 )^outfrom_fsm;
    fsmtmp=r2+ s8 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s3 )^r2;
    s4 =a_mul(s4 )^ s6 ^ainv_mul(s15 )^outfrom_fsm;
    fsmtmp=r2+ s9 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s4 )^r2;
    s5 =a_mul(s5 )^ s7 ^ainv_mul(s0 )^outfrom_fsm;
    fsmtmp=r2+ s10 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s5 )^r2;
    s6 =a_mul(s6 )^ s8 ^ainv_mul(s1 )^outfrom_fsm;
    fsmtmp=r2+ s11 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s6 )^r2;
    s7 =a_mul(s7 )^ s9 ^ainv_mul(s2 )^outfrom_fsm;
    fsmtmp=r2+ s12 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s7 )^r2;
    s8 =a_mul(s8 )^ s10 ^ainv_mul(s3 )^outfrom_fsm;
    fsmtmp=r2+ s13 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s8 )^r2;
    s9 =a_mul(s9 )^ s11 ^ainv_mul(s4 )^outfrom_fsm;
    fsmtmp=r2+ s14 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s9 )^r2;
    s10 =a_mul(s10 )^ s12 ^ainv_mul(s5 )^outfrom_fsm;
    fsmtmp=r2+ s15 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s10 )^r2;
    s11 =a_mul(s11 )^ s13 ^ainv_mul(s6 )^outfrom_fsm;
    fsmtmp=r2+ s0 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s11 )^r2;
    s12 =a_mul(s12 )^ s14 ^ainv_mul(s7 )^outfrom_fsm;
    fsmtmp=r2+ s1 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s12 )^r2;
    s13 =a_mul(s13 )^ s15 ^ainv_mul(s8 )^outfrom_fsm;
    fsmtmp=r2+ s2 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s13 )^r2;
    s14 =a_mul(s14 )^ s0 ^ainv_mul(s9 )^outfrom_fsm;
    fsmtmp=r2+ s3 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;

    outfrom_fsm=(r1+ s14 )^r2;
    s15 =a_mul(s15 )^ s1 ^ainv_mul(s10 )^outfrom_fsm;
    fsmtmp=r2+ s4 ;
    r2=snow_T0[snow_byte(0,r1)]^snow_T1[snow_byte(1,r1)]^snow_T2[snow_byte(2,r1)]^snow_T3[snow_byte(3,r1)];
    r1=fsmtmp;
  }
	p->sw[15] = s15  ;
	p->sw[14] = s14  ;
	p->sw[13] = s13  ;
	p->sw[12] = s12  ;
	p->sw[11] = s11  ;
	p->sw[10] = s10  ;
	p->sw[9] = s9  ;
	p->sw[8] = s8  ;
	p->sw[7] = s7  ;
	p->sw[6] = s6 ;
	p->sw[5] = s5;
	p->sw[4] = s4  ;
	p->sw[3] = s3  ;
	p->sw[2] = s2 ;
	p->sw[1] = s1  ;
	p->sw[0] = s0  ;
	p->outfrom_fsm = outfrom_fsm ;
	p->r1 = r1  ;
	p->r2 = r2;
	return store(p);
}


DKC_SNOW2 *WINAPI dkcAllocSNOW2(BYTE *key,size_t keysize,
																uint32 IV3,uint32 IV2,uint32 IV1,uint32 IV0)
{
	DKC_SNOW2 *p;
	int r;
	p = dkcAllocate(sizeof(DKC_SNOW2));
	if(NULL==p){
		return NULL;
	}
	p->store_size = 0;
	r = dkcSNOW2Init(p,key,keysize,IV3,IV2,IV1,IV0);
	if(DKUTIL_FAILED(r)){
		goto Error;
	}

	return p;
Error:
	dkcFreeSNOW2(&p);
	return NULL;
}



int WINAPI dkcFreeSNOW2(DKC_SNOW2 **pp){
	int r;
	r = dkcFree(pp);
	return r;
}

#define get_offset(p) (64 - p->store_size)

DKC_INLINE uint8 WINAPI dkcSNOW2Process(DKC_SNOW2 *p){
	int r = 0;
	size_t offset; 
	//DKC_4BYTE_LITTLE_ENDIAN_BASE a;
	if(p->store_size <=0){
		r = store(p);
		dkcmNOT_ASSERT(DKUTIL_FAILED(r));
	}
	offset = get_offset(p);
	p->store_size--;
	return p->u_store.a8[offset];
}



int WINAPI dkcSNOW2EncryptDOE(DKC_SNOW2 *p,uint8 *dest,size_t dsize,const uint8 *src,size_t ssize)
{
	if(dsize < ssize){
		return edk_ArgumentException;
	}
	if(dest != src){
		memcpy(dest,src,ssize);
	}
	return dkcSNOW2EncryptNoDestDOE(p,dest,ssize);

}



static DKC_INLINE int encode_byte(DKC_SNOW2 *p,uint8 *inout,size_t size,size_t *nextoffset)
{
	size_t i=0;
	int r;
	if(size <= 0){
		r =  edk_SUCCEEDED;
		goto End;
	}
	if((size > 64) && ( p->store_size == 0 || 64==p->store_size)){
	//if((size > 8) && ( p->store_size == 0 || 8==p->store_size)){
		r = edk_NoValueToProcess;
		goto End;
	}
	if(p->store_size ==0){
		r = store(p);
		dkcmNOT_ASSERT(DKUTIL_FAILED(r));
	}
	for(i=0;i<size;i++){
		inout[i] ^= dkcSNOW2_Byte(p);
		if(p->store_size ==0){
			//if(size - (i+1) >= 64){//(i + 1)かも？
			if(size - i > 64){
				r = edk_NoValueToProcess;
				i++;
				goto End;
			}
			r = store(p);
			dkcmNOT_ASSERT(DKUTIL_FAILED(r));
		}
	}
	r = edk_SUCCEEDED;
End:
	*nextoffset = i;
	return r;
}


///この処理バイトを分割していく
int WINAPI dkcSNOW2EncryptNoDestDOE(DKC_SNOW2 *p,uint8 *inout,size_t size)
{

	size_t i = 0;
	int r = edk_FAILED;
	size_t count,rest;
	uint64 *io64 = NULL;

	r = encode_byte(p,inout,size,&rest);
	switch(r){
	case edk_SUCCEEDED:
		return r;
	case edk_NoValueToProcess:
		break;
	default:
		dkcmNOT_ASSERT(DKUTIL_FAILED(r));
	}
	size = size - rest;
	inout += rest;
	if(rest == size) return edk_SUCCEEDED;
	//if(size <= 0) return edk_SUCCEEDED;

	count = size / 64;
	rest = size % 64;//最適化に任せよう（笑）
	
	if(64!=p->store_size){
		dkcmASSERT(p->store_size ==0);
		r = store(p);
		dkcmNOT_ASSERT(DKUTIL_FAILED(r));
	}
	io64 = (uint64 *)inout;
	for(i=0;i<count;i++){
		//64bit単位も良いかも？
		io64[0] ^= p->u_store.a64[0];
		io64[1] ^= p->u_store.a64[1];
		io64[2] ^= p->u_store.a64[2];
		io64[3] ^= p->u_store.a64[3];
		io64[4] ^= p->u_store.a64[4];
		io64[5] ^= p->u_store.a64[5];
		io64[6] ^= p->u_store.a64[6];
		io64[7] ^= p->u_store.a64[7];

		io64+=8;
		p->store_size = 0;
		store(p);
	}
	inout =(uint8*)io64;
	return dkcSNOW2EncryptNoDestDOE(p,inout,rest);

}
#if 0

int WINAPI dkcSNOW2EncryptNoDestDOE(DKC_SNOW2 *p,uint8 *inout,size_t size)
{



	size_t i;
	int r;
	size_t count,rest;
	//uint32 *io32;
	uint64 *io64;
	//残り物処分
	if(size <= 0) return edk_SUCCEEDED;
	for(i=0;;){//for scope
		if((size > 64) && ( p->store_size == 0 || 64==p->store_size)){
		//if((size > 8) && ( p->store_size == 0 || 8==p->store_size)){
			break;
		}
		for(;i<size;i++){
			inout[i] ^= dkcSNOW2_Byte(p);
			if(p->store_size == 0){
				break;
			}
		}
		break;
	}
	size-=i;
	inout+=i;
	if(size <= 0) return edk_SUCCEEDED;

	count = size / 64;
	rest = size % 64;//最適化に任せよう（笑）

	if(64!=p->store_size){
	//if(8!=p->store_size){
		r = store(p);
		dkcmNOT_ASSERT(DKUTIL_FAILED(r));
	}
	//io32 = (uint32 *)inout;
	io64 =(uint64*)inout;
	for(;i<count;i++){
		//64bit単位も良いかも？
		/*io32[0] ^= p->u_store.a32[0];
		io32[1] ^= p->u_store.a32[1];
		io32[2] ^= p->u_store.a32[2];
		io32[3] ^= p->u_store.a32[3];
		io32[4] ^= p->u_store.a32[4];
		io32[5] ^= p->u_store.a32[5];
		io32[6] ^= p->u_store.a32[6];
		io32[7] ^= p->u_store.a32[7];
		io32[8] ^= p->u_store.a32[8];
		io32[9] ^= p->u_store.a32[9];
		io32[10] ^= p->u_store.a32[10];
		io32[11] ^= p->u_store.a32[11];
		io32[12] ^= p->u_store.a32[12];
		io32[13] ^= p->u_store.a32[13];
		io32[14] ^= p->u_store.a32[14];
		io32[15] ^= p->u_store.a32[15];
		
		io32+=16;*/
		io64[0] ^= p->u_store.a64[0];
		io64[1] ^= p->u_store.a64[1];
		io64[2] ^= p->u_store.a64[2];
		io64[3] ^= p->u_store.a64[3];
		io64[4] ^= p->u_store.a64[4];
		io64[5] ^= p->u_store.a64[5];
		io64[6] ^= p->u_store.a64[6];
		io64[7] ^= p->u_store.a64[7];
		io64+=8;
		p->store_size = 0;
		store(p);
	}
	//inout =(uint8*)io32;
		inout =(uint8*)io64;
	return dkcSNOW2EncryptNoDestDOE(p,inout,rest);

}

#endif