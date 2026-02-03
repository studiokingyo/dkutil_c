
/**
@file dkcHC256.c
@brief HC256 stream cipher Algorithm
@note


これ（以下）はオリジナルの紹介（表記）していたコメント（文章）です。
this is the original introductory comment:
<PRE>
//Stream cipher HC-256 was published at Fast Software Encryption 2004 
//This optimized C code of stream cipher HC-256 is implemented by
//Hongjun Wu in 2003.
//This code runs at about 1.9 bit/cycle on the Intel Pentium 4 processor
//with the Intel C++ 7.1 compiler.
</PRE>
*/





#include "dkcHC256.h"

#include <stdlib.h>

/*
uint32 P[1024],Q[1024];
uint32 X[16],Y[16];
uint32 counter2048; // counter2048 = i mod 2048;
*/
#ifndef _MSC_VER
#define rotr(x,n)   (((x)>>(n))|((x)<<(32-(n))))
#else
#define rotr(x,n)   _lrotr(x,n)
#endif


static DKC_INLINE uint32 h1(uint32 *Q,uint32 x){
	DKC_4BYTE_UNION_LITTLE_ENDIAN_BASE a;
	a.dword = x;
	return Q[a.w.byte0] +
		Q[256+a.w.byte1] +
		Q[512+a.w.byte2] +
		Q[768+a.w.byte3];
}

static DKC_INLINE uint32 h2(uint32 *P,uint32 x){
	DKC_4BYTE_UNION_LITTLE_ENDIAN_BASE a;
	a.dword = x;
  return P[a.w.byte0] +
		P[256+a.w.byte1] +
		P[512+a.w.byte2] +
		P[768+a.w.byte3];
}

static DKC_INLINE uint32 g1(uint32 *Q,uint32 x,uint32 y){
	uint32 a,b,c;
	a = rotr((x),10);
	b = rotr((y),23);
	c = ((x)^(y))&0x3ff;
	return (a^b) + Q[c];
}
static DKC_INLINE uint32 g2(uint32 *P,uint32 x,uint32 y){
	uint32 a,b,c;
	a = rotr((x),10);
	b = rotr((y),23);
	c = ((x)^(y))&0x3ff;
	return (a^b) + P[c];
}

#define feedback_1(u,v,b,c) {	     \
   uint32 tem0,tem1,tem2;          \
   tem0 = rotr((v),23);             \
   tem1 = rotr((c),10);	           \
   tem2 = ((v) ^ (c)) & 0x3ff;     \
   (u) += (b)+(tem0^tem1)+Q[tem2]; \
}       

#define feedback_2(u,v,b,c) {	     \
   uint32 tem0,tem1,tem2;          \
   tem0 = rotr((v),23);             \
   tem1 = rotr((c),10);	           \
   tem2 = ((v) ^ (c)) & 0x3ff;     \
   (u) += (b)+(tem0^tem1)+P[tem2]; \
}      
static DKC_INLINE uint32 fb1(uint32 *Q,uint32 u,uint32 v,uint32 b,uint32 c){
	 uint32 tem0,tem1,tem2;          
   tem0 = rotr((v),23);             
   tem1 = rotr((c),10);	           
   tem2 = ((v) ^ (c)) & 0x3ff;    
   (u) += (b)+(tem0^tem1)+Q[tem2];
	 return u;
}
static DKC_INLINE uint32 fb2(uint32 *P,uint32 u,uint32 v,uint32 b,uint32 c){
	uint32 tem0,tem1,tem2;          
	tem0 = rotr((v),23);             
	tem1 = rotr((c),10);	           
	tem2 = ((v) ^ (c)) & 0x3ff;     
	(u) += (b)+(tem0^tem1)+P[tem2]; 
	return u;
}
static DKC_INLINE void fb1_(uint32 *Q,uint32 *u,uint32 v,uint32 b,uint32 c){
	 uint32 tem0,tem1,tem2;          
   tem0 = rotr((v),23);             
   tem1 = rotr((c),10);	           
   tem2 = ((v) ^ (c)) & 0x3ff;     
   (*u) += (b)+(tem0^tem1)+Q[tem2]; 
}
static DKC_INLINE void fb2_(uint32 *P,uint32 *u,uint32 v,uint32 b,uint32 c){
	uint32 tem0,tem1,tem2;          
	tem0 = rotr((v),23);             
	tem1 = rotr((c),10);	           
	tem2 = ((v) ^ (c)) & 0x3ff;     
	(*u) += (b)+(tem0^tem1)+P[tem2]; 
}
#define f1(x)  (rotr((x),7) ^ rotr((x),18) ^ ((x) >> 3))
#define f2(x)  (rotr((x),17) ^ rotr((x),19) ^ ((x) >> 10))
#define f(a,b,c,d) (f2((a)) + b + f1((c)) + d) 

void initialization(DKC_HC256 *p,uint32 key[], uint32 iv[]) 
{
	uint32 i,j;
	uint32 *P = p->P;
	uint32 *Q = p->Q;

	//expand the key and iv into P and Q
	for (i = 0; i < 8; i++)     P[i] = key[i];
	for (i = 8; i < 16; i++)    P[i] = iv[i-8]; 

	for (i = 16; i < 528; i++)  
		 P[i] = f(P[i-2],P[i-7],P[i-15],P[i-16])+i;
	for (i = 0; i < 16; i++)    
		 P[i] = P[i+512];
	for (i = 16; i < 1024; i++) 
		 P[i] = f(P[i-2],P[i-7],P[i-15],P[i-16])+512+i;

	for (i = 0;  i < 16;  i++)  
		 Q[i] = P[1024-16+i];
	for (i = 16; i < 32;  i++)  
		 Q[i] = f(Q[i-2],Q[i-7],Q[i-15],Q[i-16])+1520+i;
	for (i = 0;  i < 16;  i++)  
		 Q[i] = Q[i+16];
	for (i = 16; i < 1024;i++)  
		 Q[i] = f(Q[i-2],Q[i-7],Q[i-15],Q[i-16])+1536+i;

	//run the cipher 4096 steps without generating output	
	for (i = 0; i < 2; i++) {
#if 1
		for (j = 0;  j < 10;   j++)  
			 fb1_(Q,&P[j],P[j+1],P[(j-10)&0x3ff],P[(j-3)&0x3ff]);
		for (j = 10; j < 1023; j++)  
			 fb1_(Q,&P[j],P[j+1],P[j-10],P[j-3]);
		fb1_(Q,&P[1023],P[0],P[1013],P[1020]);
		for (j = 0;  j < 10;   j++)  
			 fb2_(P,&Q[j],Q[j+1],Q[(j-10)&0x3ff],Q[(j-3)&0x3ff]);
		for (j = 10; j < 1023; j++)  
			 fb2_(P,&Q[j],Q[j+1],Q[j-10],Q[j-3]);
		fb2_(P,&Q[1023],Q[0],Q[1013],Q[1020]);  
		/*
		for (j = 0;  j < 10;   j++)  
			 P[j] = fb1(P,P[j],P[j+1],P[(j-10)&0x3ff],P[(j-3)&0x3ff]);
		for (j = 10; j < 1023; j++)  
			 P[j] = fb1(P,P[j],P[j+1],P[j-10],P[j-3]);
			 P[1023] = fb1(P,P[1023],P[0],P[1013],P[1020]);
		for (j = 0;  j < 10;   j++)  
			 Q[j] = fb2(Q,Q[j],Q[j+1],Q[(j-10)&0x3ff],Q[(j-3)&0x3ff]);
		for (j = 10; j < 1023; j++)  
			 Q[j] =fb2(Q,Q[j],Q[j+1],Q[j-10],Q[j-3]);
			 Q[1023] =fb2(Q,Q[1023],Q[0],Q[1013],Q[1020]);  
		*/
#else
		for (j = 0;  j < 10;   j++)  
			feedback_1(P[j],P[j+1],P[(j-10)&0x3ff],P[(j-3)&0x3ff]);
		for (j = 10; j < 1023; j++)  
			feedback_1(P[j],P[j+1],P[j-10],P[j-3]);
		feedback_1(P[1023],P[0],P[1013],P[1020]);
		for (j = 0;  j < 10;   j++)  
			feedback_2(Q[j],Q[j+1],Q[(j-10)&0x3ff],Q[(j-3)&0x3ff]);
		for (j = 10; j < 1023; j++)  
			feedback_2(Q[j],Q[j+1],Q[j-10],Q[j-3]);
		feedback_2(Q[1023],Q[0],Q[1013],Q[1020]); 
#endif
	}

	//initialize counter2048, and tables X and Y 
	p->counter2048 = 0;
	for (i = 0; i < 16; i++) p->X[i] = P[1008+i];
	for (i = 0; i < 16; i++) p->Y[i] = Q[1008+i];
}	



DKC_INLINE uint32 dkcHC256Process32(DKC_HC256 *p,uint32 u) 
{
	unsigned long i,i3, i10, i12, i1023;
	unsigned long output;

	i   = p->counter2048 & 0x3ff;
	i3  = (i - 3) & 0x3ff;
	i10 = (i - 10) & 0x3ff;
	i12 = (i - 12) & 0x3ff;
	i1023 = (i - 1023) & 0x3ff;

	if (p->counter2048 < 1024) {
		p->P[i] = p->P[i] + p->P[i10] + g1(p->Q,p->P[i3],p->P[i1023]);
		output = h1(p->Q,p->P[i12]) ^ p->P[i];
	}
	else {
		p->Q[i] = p->Q[i] + p->Q[i10] + g2(p->P,p->Q[i3],p->Q[i1023]);
		output = h2(p->P,p->Q[i12]) ^ p->Q[i];

	}
	p->counter2048 = (p->counter2048+1) & 0x7ff;
	return (output);
}


static DKC_INLINE void step_A(DKC_HC256 *p,uint32 u,uint32 v,
															uint32 a,uint32 b,uint32 c,uint32 d,uint32 *m)
{
	uint32 tem0,tem1,tem2,tem3;
	tem0 = rotr((v),23);
	tem1 = rotr((c),10);
	tem2 = ((v) ^ (c)) & 0x3ff;
	(u) += (b)+(tem0^tem1) + p->Q[tem2];
	(a) = (u);
	tem3 = h1(p->P,d);
	//h1(p->Q,(d),tem3);                    
	//(*m) ^= tem3 ^ (u) ;
	*m = tem3 ^ (u) ;
}
static DKC_INLINE void step_B(DKC_HC256 *p,uint32 u,uint32 v,
															uint32 a,uint32 b,uint32 c,uint32 d,uint32 *m){
	uint32 tem0,tem1,tem2,tem3;      
	tem0 = rotr((v),23);              
	tem1 = rotr((c),10);             
	tem2 = ((v) ^ (c)) & 0x3ff;      
	(u) += (b)+(tem0^tem1)+p->P[tem2];  
	(a) = (u);
	tem3 = h2(p->P,d);
	//h2(p->P,(d),tem3);                    
	//(*m) ^= tem3 ^ (u) ;              
	*m = tem3 ^ (u) ;
}       


static DKC_INLINE void get_encrypt_array(DKC_HC256 *p,uint32 data[])  //each time it encrypts 512-bit data
{
	uint32 cc,dd,counter2048 = p->counter2048;
	uint32 *P = p->P,*Q = p->Q,*X =p->X, *Y = p->Y;

	cc = counter2048 & 0x3ff;
	dd = (cc+16) & 0x3ff;

	if (counter2048 < 1024)	
	{   		
		counter2048 = (counter2048 + 16) & 0x7ff;
		step_A(p,P[cc+0], P[cc+1], X[0], X[6], X[13],X[4], &data[0]);
		step_A(p,P[cc+1], P[cc+2], X[1], X[7], X[14],X[5], &data[1]);
		step_A(p,P[cc+2], P[cc+3], X[2], X[8], X[15],X[6], &data[2]);
		step_A(p,P[cc+3], P[cc+4], X[3], X[9], X[0], X[7], &data[3]);
		step_A(p,P[cc+4], P[cc+5], X[4], X[10],X[1], X[8], &data[4]);
		step_A(p,P[cc+5], P[cc+6], X[5], X[11],X[2], X[9], &data[5]);
		step_A(p,P[cc+6], P[cc+7], X[6], X[12],X[3], X[10],&data[6]);
		step_A(p,P[cc+7], P[cc+8], X[7], X[13],X[4], X[11],&data[7]);
		step_A(p,P[cc+8], P[cc+9], X[8], X[14],X[5], X[12],&data[8]);
		step_A(p,P[cc+9], P[cc+10],X[9], X[15],X[6], X[13],&data[9]);
		step_A(p,P[cc+10],P[cc+11],X[10],X[0], X[7], X[14],&data[10]);
		step_A(p,P[cc+11],P[cc+12],X[11],X[1], X[8], X[15],&data[11]);
		step_A(p,P[cc+12],P[cc+13],X[12],X[2], X[9], X[0], &data[12]);
		step_A(p,P[cc+13],P[cc+14],X[13],X[3], X[10],X[1], &data[13]);
		step_A(p,P[cc+14],P[cc+15],X[14],X[4], X[11],X[2], &data[14]);
		step_A(p,P[cc+15],P[dd+0], X[15],X[5], X[12],X[3], &data[15]);
	}
	else				    
	{
		counter2048 = (counter2048 + 16) & 0x7ff;
		step_B(p,Q[cc+0], Q[cc+1], Y[0], Y[6], Y[13],Y[4], &data[0]);
		step_B(p,Q[cc+1], Q[cc+2], Y[1], Y[7], Y[14],Y[5], &data[1]);
		step_B(p,Q[cc+2], Q[cc+3], Y[2], Y[8], Y[15],Y[6], &data[2]);
		step_B(p,Q[cc+3], Q[cc+4], Y[3], Y[9], Y[0], Y[7], &data[3]);
		step_B(p,Q[cc+4], Q[cc+5], Y[4], Y[10],Y[1], Y[8], &data[4]);
		step_B(p,Q[cc+5], Q[cc+6], Y[5], Y[11],Y[2], Y[9], &data[5]);
		step_B(p,Q[cc+6], Q[cc+7], Y[6], Y[12],Y[3], Y[10],&data[6]);
		step_B(p,Q[cc+7], Q[cc+8], Y[7], Y[13],Y[4], Y[11],&data[7]);
		step_B(p,Q[cc+8], Q[cc+9], Y[8], Y[14],Y[5], Y[12],&data[8]);
		step_B(p,Q[cc+9], Q[cc+10],Y[9], Y[15],Y[6], Y[13],&data[9]);
		step_B(p,Q[cc+10],Q[cc+11],Y[10],Y[0], Y[7], Y[14],&data[10]);
		step_B(p,Q[cc+11],Q[cc+12],Y[11],Y[1], Y[8], Y[15],&data[11]);
		step_B(p,Q[cc+12],Q[cc+13],Y[12],Y[2], Y[9], Y[0], &data[12]);
		step_B(p,Q[cc+13],Q[cc+14],Y[13],Y[3], Y[10],Y[1], &data[13]);
		step_B(p,Q[cc+14],Q[cc+15],Y[14],Y[4], Y[11],Y[2], &data[14]);
		step_B(p,Q[cc+15],Q[dd+0], Y[15],Y[5], Y[12],Y[3], &data[15]);
	}

	p->counter2048 = counter2048;
}

static DKC_INLINE int store(DKC_HC256 *p){
	
	dkcmNOT_ASSERT(0 != p->store_size);
	if(0 != p->store_size) return edk_LogicError;
	get_encrypt_array(p,p->u_store.a32);
	p->store_size = 64;
	return edk_SUCCEEDED;
}

DKC_INLINE DKC_HC256 *WINAPI dkcAllocHC256(BYTE *key,size_t size,uint32 *iv,size_t ivsize){
	DKC_HC256 *p;
	//uint32 iv[8];
	//size_t limit = sizeof(uint32) * 8;
	//if(size != limit || ivsize != limit){
	if(size != dkcdHC256_KEY_SIZE || ivsize != dkcdHC256_IV_SIZE){
		return NULL;
	}
	
	p = dkcAllocate(sizeof(DKC_HC256));
	if(NULL==p){
		return NULL;
	}
	//DKUTIL_MEMZERO(iv,sizeof(iv));
	initialization(p,(uint32 *)key,iv);
	store(p);
	//p->store_size = 64;
	return p;
}

DKC_INLINE DKC_HC256 *dkcAllocHC256Const(const uint8 *key,size_t size,const uint32 *iv,size_t ivsize)
{
	DKC_HC256 *p=NULL;
	uint32 *piv = malloc(ivsize);
	uint8 *pt;
	if(!piv) return NULL;
	pt = malloc(size);
	if(pt){
		memcpy(pt,key,size);
		memcpy(piv,iv,ivsize);
		p = dkcAllocHC256(pt,size,piv,ivsize);
		free(pt);
		free(piv);
	}
	return p;
}

DKC_INLINE int WINAPI dkcFreeHC256(DKC_HC256 **p){
	return dkcFree(p);
}


#define get_offset(p) (64 - p->store_size)

static DKC_INLINE uint8 hc256_byte(DKC_HC256 *p){
	
	size_t offset; 
	//DKC_4BYTE_LITTLE_ENDIAN_BASE a;
	dkcmNOT_ASSERT(0==p->store_size);

	offset = get_offset(p);
	p->store_size--;
	return p->u_store.a8[offset];

}

DKC_INLINE uint8 WINAPI dkcHC256Process(DKC_HC256 *p){
	int r = 0;
	//DKC_4BYTE_LITTLE_ENDIAN_BASE a;
	if(p->store_size <= 0){
		r = store(p);
		dkcmNOT_ASSERT(DKUTIL_FAILED(r));
	}
	return hc256_byte(p);

}

int WINAPI dkcHC256EncryptDOE(DKC_HC256 *p,uint8 *dest,size_t dsize,const uint8 *src,size_t ssize)
{
	if(ssize > dsize){
		return edk_ArgumentException;
	}
	if(dest != src){
		memcpy(dest,src,ssize);
	}
	return dkcHC256EncryptNoDestDOE(p,dest,ssize);

}
static DKC_INLINE int encode_byte(DKC_HC256 *p,uint8 *inout,size_t size,size_t *nextoffset)
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
		inout[i] ^= hc256_byte(p);
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
int WINAPI dkcHC256EncryptNoDestDOE(DKC_HC256 *p,uint8 *inout,size_t size)
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
	return dkcHC256EncryptNoDestDOE(p,inout,rest);

}
/*
int WINAPI dkcHC256EncryptDOE(DKC_HC256 *p,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize)
{
	size_t i;
	int r;
	const BYTE *src_t;
	size_t count,rest;
	if(dsize > 0x7fffffff || ssize > 0x7fffffff || dsize < ssize){
		return edk_ArgumentException;
	}
	//512byte毎
	count = ssize / 512;
	rest = ssize % 512;//最適化に任せよう（笑）
	for(i=0;i<count;i++){
		r = dkcHC256EncryptBlock512DOE(p,&dest[i*512],512,&src[i*512],512);		
		if(DKUTIL_FAILED(r)) return r;
	}
	dest = &dest[i*512];
	src_t = &src[i*512];

	{//4byte毎
		const uint32 *pst = (const uint32 *)src;
		uint32 *pdt = (uint32 *)dest;
		uint8 *pts,*ptd;
		uint32 ts,td;

		count = rest / sizeof(uint32);
		rest = rest % sizeof(uint32);
		

		for(i=0;i<count;i++){
			pdt[i] = dkcHC256Process32(p,pst[i]);
		}
		//1byte毎
		if(rest){
			pts = (uint8 *)&pst[i];
			ptd = (uint8 *)&pdt[i];
			ts = 0;
			for(i=0;i<rest;i++)
			{
				ts |= pts[i] << (8 * i);
			}
			td = dkcHC256Process32(p,ts);
			pts = (uint8*)&td;// Dependent On Endian
			for(i=0;i<rest;i++){
				ptd[i] = pts[i];
			}
		}
	}//end of scope
	return edk_SUCCEEDED;
}


int WINAPI dkcHC256DecryptDOE(DKC_HC256 *p,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize)
{
	size_t i;
	int r;
	const BYTE *src_t;
	if(dsize > 0x7fffffff || ssize > 0x7fffffff || dsize < ssize){
		return edk_ArgumentException;
	}
	//512byte毎
	size_t count = ssize / 512;
	size_t rest = ssize % 512;//最適化に任せよう（笑）
	for(i=0;i<count;i++){
		r = dkcHC256DecryptBlock512DOE(p,&dest[i*512],512,&src[i*512],512);		
		if(DKUTIL_FAILED(r)) return r;
	}
	dest = &dest[i*512];
	src_t = &src[i*512];

	{//4byte毎
		count = rest / sizeof(uint32);
		rest = rest % sizeof(uint32);
		const uint32 *pst = src;
		uint32 *pdt = dest;
		uint8 *pts,*ptd;
		uint32 ts,td

		for(i=0;i<count;i++){
			pdt[i] = dkcHC256Process32(p,pst[i]);
		}
		//1byte毎
		if(rest){
			pts = (uint8 *)&pst[i];
			ptd = (uint8 *)&pdt[i];
			ts = 0;
			for(i=0;i<rest;i++)
			{
				ts |= pts[i] << (8 * i);
			}
			td = dkcHC256Process32(p,ts);
			for(i=0;i<rest;i++){
				ptd[i] = (uint8)(pts[i] >> (8 * i));
			}
		}
	}

}
*/

/*
int WINAPI dkcHC256Encrypt(DKC_HC256 *p,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize)
{


	

}


int WINAPI dkcHC256Decrypt(DKC_HC256 *p,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize)
{


}*/