/**
@author d金魚
@file memory.c
@brief memory.c
@since 2005/01/17
*/
#include "memory.h"
#include "malloc.h"

DKC_INLINE size_t dkcstd_size_align(size_t size,size_t align_offset){
	size_t t = size;
	while((t % align_offset )!= 0){
		t--;
		if(0==t){
			break;
		}
	}
	return t;
}


void dkcstd_get_memory_align(DKCSTD_ALIGNED_MEM *r,const void *src,size_t srcsize,size_t align_offset)
{
	size_t o = dkcstd_offset_align(src,align_offset);
	BYTE *t = (BYTE *)src;
	size_t m = srcsize - o;
	r->access_permit_from_mem = dkcstd_size_align(m,align_offset);
	
	r->mem = t + o;
	r->offset_from_original = o;
}



DKC_INLINE void *dkcstd_pointer_align( const void *src,size_t align_offset){
	return ((uint8 *)src) + dkcstd_offset_align(src,align_offset);
}

DKC_INLINE size_t dkcstd_offset_align(const void *src,size_t align_offset){
	size_t force = (size_t)src;
	size_t save = force;
	while((force % align_offset )!= 0){
		force++;
#ifdef WIN32
		//(確かWin32の場合、0x7FFFFFFFより上の領域は返さないはずだが･･･
		//if(0x7FFFFFFF<=force){
		if(0x80000000 <= force){
			break;
		}
#else
		if(0==force){//安全策 
			break;
		}
#endif
	}
	return force - save;
}
BOOL dkcstd_IsAligned(const void *src,size_t bytes)
{
	size_t force = (size_t)src;
	return (force % bytes == 0);
}

void *dkcstd_memcpy8( void *dest, const void *src, size_t count ){
	size_t i;
	uint8 *d;
	const uint8 *s;

	d = dest;
	s = src;

	for(i=0;i<count;i++){
		d[i] = s[i];
	}
	return dest;
}



void *dkcstd_memset8( void *dest, int c, size_t count ){
	uint8 cc = (uint8)c;
	size_t i;
	for(i=0;i<count;i++){
		((uint8 *)dest)[i] = cc;
	}
	return dest;
}

int dkcstd_memcmp8(const void *buf1, const void *buf2, size_t count ){
	const uint8 *a = buf1,*b = buf2;
	size_t i;
	for(i=0;i<count;i++)
	{
		if(a[i] != b[i])
		{
			/*if(a[i] > b[i]){
				return a[i] - b[i];
			}else{
				return b[i] - a[i];
			}*/
			const int *aa,*bb;
			aa = (const int *)&a[i];
			bb = (const int *)&b[i];
			return *aa - *bb;
			//return ((int *)&a[i]) - ((int *)&b[i]);
		}
	}
	return 0;
}


void *dkcstd_memchr8( const void *buf, int c, size_t count )
{
	size_t i;
	const uint8 *t = buf,cc = (const uint8)c;
	for(i=0;i<count;i++){
		if(t[i] == cc){
			return (void *)&(t[i]);
		}
	}
	return NULL;
}

#if 1==DKUTIL_CSTD_MSVC_IASM
#pragma warning( disable : 4035 ) // 警告：返値がないよん


///memcpy郡 用のフィルターマクロ 邪道だけど許して m(_ _)m　@parma limit bit単位
#define DKCSTD_MEMCPY_FILTER(limit) \
	if(count <= DKUTIL_BIT_TO_BYTE(limit) ) return dkcstd_memcpy(dest,src,count);

///alignされていない場合は強制的にデフォルトにするマクロ @param limit byte単位
#define DKCSTD_MEMCPY_ALIGN_FILTER(limit) \
	if(\
		!(dkcstd_IsAligned(src,DKUTIL_BIT_TO_BYTE(limit)) \
		&& dkcstd_IsAligned(dest,DKUTIL_BIT_TO_BYTE(limit)))\
		|| !(count % DKUTIL_BIT_TO_BYTE(limit) == 0)			\
	)\
		 return dkcstd_memcpy(dest,src,count);

void *dkcstd_memcpy_rep_movsd( void *dest, const void *src, size_t count ){
	_asm {
		mov edi,dest	;edi = dest
		mov esi,src		;esi = src
		mov ecx,count	;ecx = count
		;mov eax,03h			; %4に使うbitmask
		mov edx,ecx			;edx = ecx
		shr ecx,2				;ecx / 4
		rep movsd				;ecxが無くなるまでmemcpy見たいナもの
;MiniCopy:
		and  edx, 03h		;edx & eax ( count % 4 )
		mov  ecx, edx		;
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dst
	}
	//return dest;
}


void *dkcstd_memcpy_FPU64( void *dest, const void *src, size_t count )
{

	DKCSTD_MEMCPY_FILTER(64);
	DKCSTD_MEMCPY_ALIGN_FILTER(64);
	optimized_memcpy_fpu_8bytes(dest,src,count);
	return dest;
/*
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov edx,ecx;//eax = count
		shr ecx,3;// count / 8 sizeof(double)
lx:	
		fld double ptr [esi];		
		fstp double ptr [edi];
		lea esi,[esi+8];
		lea edi,[edi+8];
		dec ecx;
		jnz lx;
	
;MiniCopy:
		//lea esi,[esi - 8]
		//lea edi,[edi - 8]
		mov ecx,edx			;
		and  ecx, 7		;ecx & 07h ( count % 8 )

		rep  movsb			;N  BYTE MOVE
		cld
		//mov  eax,[dest]	;V  return dst


	}
	return dest;
*/
}

void *dkcstd_memcpy_asm64( void *dest, const void *src, size_t count ){
	//if(count < 8)	return dkcstd_memcpy(dest,src,count);
	DKCSTD_MEMCPY_FILTER(64);

	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov edx,ecx;
		shr ecx,3;
lx:
		mov eax,[esi];
		mov ebx,[esi+4];
		add esi,8;
		mov [edi],eax;
		mov [edi+4],ebx;
		add edi,8;
		dec ecx;
		jnz lx;
;MiniCopy:
		mov ecx,edx			;
		and  ecx, 7		;edx & 7 ( count % 8 )
		rep  movsb			;N  BYTE MOVE
		cld
	//	mov  eax,[dest]	;V  return dest
	}
	return dest;

}


void *dkcstd_memcpy_asm32( void *dest, const void *src, size_t count ){
	/*if(count < 4){
		return dkcstd_memcpy(dest,src,count);
	}*/
	DKCSTD_MEMCPY_FILTER(32);
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov ebx,ecx;
		shr ecx,2;
lx:
		mov eax,[esi];
		add esi,4;
		mov [edi],eax;
		add edi,4;
		dec ecx;
		jnz lx;
;MiniCopy:
		mov ecx,ebx			;
		and  ecx, 3		; ( count % 4 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dest
	}


}


DKC_EXTERN void *dkcstd_memcpy_movntq_mmx_prefetch128( void *dest, const void *src, size_t count )
{
	//if(count < DKUTIL_BIT_TO_BYTE(128))	return dkcstd_memcpy(dest,src,count);
	DKCSTD_MEMCPY_FILTER(128);
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov eax,ecx;//count
		shr ecx,4;// count / 16
lx:
		movq mm0,[esi];
		movq mm1,[esi+8];
		lea esi,[esi+16];
		movntq [edi],mm0;
		prefetcht0 [esi+768];
		movntq [edi+8],mm1;
		lea edi,[edi+16];
		dec ecx;
		jnz lx;
;MiniCopy:
		mov ecx,eax			;
		and  ecx, 15		;edx & 15 ( count % 16 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dst
	}


}
void *dkcstd_memcpy_movntq_mmx128( void *dest, const void *src, size_t count ){
		DKCSTD_MEMCPY_FILTER(128);
	
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov eax,ecx
		shr ecx,4;//count / 16
lx:
		movq mm0,[esi];
		movq mm1,[esi+8];
		lea esi,[esi+16];
		movntq [edi],mm0;
		movntq [edi+8],mm1;
		lea edi,[edi+16];
		dec ecx;
		jnz lx;
;MiniCopy:
		mov ecx,eax			;
		and  ecx, 15		;edx & 15 ( count % 16 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dst
	}

}

void *dkcstd_memcpy_mmx128( void *dest, const void *src, size_t count ){
	DKCSTD_MEMCPY_FILTER(128);
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov eax,ecx;
		//add eax,16
		//cmp eax,16 //if(count < 16) goto MiniCopy; 10h == 16 ??
		//jae MiniCopy;
		shr ecx,4;
lx:
		movq mm0,[esi];
		movq mm1,[esi+8];
		lea esi,[esi+16];
		movq [edi],mm0;
		movq [edi+8],mm1;
		lea edi,[edi+16];
		dec ecx;
		jnz lx;
MiniCopy:
		sub eax,10h
		mov ecx,eax			;
		and  ecx, 15		;edx & 15 ( count % 16 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dest
	}


}

void *dkcstd_memcpy_movntps_sse128( void *dest, const void *src, size_t count ){
	DKCSTD_MEMCPY_FILTER(128);
	_asm {
		mov edi,[dest];
		mov esi,[src];
		mov ecx,count;
		mov eax,ecx;
		shr ecx,4;
lx:
		movaps xmm0,[esi];
		lea esi,[esi+16];
		movntps [edi],xmm0;
		lea edi,[edi+16];
		dec ecx;
		jnz lx;
;MiniCopy:
		mov ecx,eax			;
		and  ecx, 15		;edx & 15 ( count % 16 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dest
	}

}

void *dkcstd_memcpy_sse128( void *dest, const void *src, size_t count ){
	DKCSTD_MEMCPY_FILTER(128);
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov eax,ecx;
		shr ecx,4;
lx:
		movaps xmm0,[esi];
		lea esi,[esi+16];
		movaps [edi],xmm0;
		lea edi,[edi+16];
		dec ecx;
		jnz lx;
;MiniCopy:
		mov ecx,eax			;
		and  ecx, 15		;edx & 15 ( count % 16 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dest
	}
}
void *dkcstd_memcpy_movntq_mmx64( void *dest, const void *src, size_t count ){
	DKCSTD_MEMCPY_FILTER(64);
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov eax,ecx;
		shr ecx,3;// / 8
lx:
		movq mm0,[esi];
		lea esi,[esi+8];
		movntq [edi],mm0;
		lea edi,[edi+8];
		dec ecx;
		jnz lx;
;MiniCopy:
		mov ecx,eax			;
		and  ecx, 7		;( count % 16 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dst
	}

}


void *dkcstd_memcpy_mmx64( void *dest, const void *src, size_t count )
{	
		DKCSTD_MEMCPY_FILTER(64);
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov edx,ecx;
		//add eax,16
		//cmp eax,16 //if(count < 16) goto MiniCopy; 10h == 16 ??
		//jae MiniCopy;
		shr ecx,3;		//count / 8
lx:
		movq mm0,[esi];
		add esi,8;
		movq [edi],mm0;
		add edi,8;
		dec ecx;
		jnz lx;
MiniCopy:
		mov ecx,edx			;
		and  ecx, 7			;( count % 8 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dest
	}
}

void *dkcstd_memcpy_movups_sse_prefetch256( void *dest, const void *src, size_t count )
{
		DKCSTD_MEMCPY_FILTER(256);
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov eax,ecx;
		shr ecx,5;// count / 32
lx:
		movups xmm0,[esi];
		movups xmm1,[esi+16];
		lea esi,[esi+32];
		movups [edi],xmm0;
		prefetcht0 [esi+1024];
		movups [edi+16],xmm1;
		lea edi,[edi+32];
		dec ecx;
		jnz lx;
;MiniCopy:
		mov ecx,eax			;
		and  ecx, 31		;( count % 32 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dest
	}


}


void *dkcstd_memcpy_movntps_sse_prefetch256( void *dest, const void *src, size_t count )
{
		DKCSTD_MEMCPY_FILTER(256);
	_asm {
		mov edi,dest;
		mov esi,src;
		mov ecx,count;
		mov eax,ecx;
		shr ecx,5;// count / 32
lx:
		movaps xmm0,[esi];
		movaps xmm1,[esi+16];
		lea esi,[esi+32];
		movntps [edi],xmm0;
		prefetcht0 [esi+1024];
		movntps [edi+16],xmm1;
		lea edi,[edi+32];
		dec ecx;
		jnz lx;
;MiniCopy:
		mov ecx,eax			;
		and  ecx, 31		;( count % 32 )
		rep  movsb			;N  BYTE MOVE
		cld
		mov  eax,[dest]	;V  return dest
	}


}




///@see NYSLで配布されているGreenPadより
void* /*__cdecl*/ dkcstd_memmove( void* dst, const void* src, size_t cnt )
{
	__asm {
		mov  esi, [src]    ;U  esi = const void* src
		mov  edx, [cnt]    ;V  edx =       void* cnt
		mov  edi, [dst]    ;U  edi =       ulong dst
		mov  ebx, edx      ;V
		mov  eax, 03h      ;U  eax = const ulong  3 (for masking)
		add  ebx, esi      ;V  ebx = const void* src+cnt

		cmp  edi, esi      ;
		jbe  CopyUp        ;
		cmp  edi, ebx      ;   if( src < dst < src+cnt )
		jb   CopyDown      ;     downward copy

	CopyUp:
		cmp  edx, eax      ;   if( cnt<=3 )
		jbe  MiniCopy      ;     byte by byte copy

		mov  ebx, edi      ;U
		mov  ecx, eax      ;V
		and  ebx, eax      ;U  ebx = (dst&3)
		inc  ecx           ;V
		sub  ecx, ebx      ;   ecx = (4-(dst&3))
		and  ecx, eax      ;   ecx = {dst%4 0->0 1->3 2->2 3->1}
		sub  edx, ecx      ;
		rep  movsb         ;N  BYTE MOVE (align dst)

		mov  ecx, edx      ;
		shr  ecx, 2        ;   ecx = [rest bytes]/4
		and  edx, eax      ;   edx = [rest bytes]%4
		rep  movsd         ;N  DWORD MOVE
		jmp  MiniCopy      ;

	CopyDown:
		std                  ;
		lea  esi,[esi+edx-1] ;
		lea  edi,[edi+edx-1] ;

		cmp  edx, 4        ;   if( cnt<=4 )
		jbe  MiniCopy      ;     byte by byte copy

		mov  ecx, edi      ;
		and  ecx, eax      ;
		inc  ecx           ;   ecx = {dst%4 0->1 1->2 2->3 3->4}
		sub  edx, ecx      ;
		rep  movsb         ;N  BYTE MOVE (align dst @ dword)

		sub  edi, eax      ;U
		mov  ecx, edx      ;V
		sub  esi, eax      ;U
		shr  ecx, 2        ;V  ecx = [rest bytes]/4
		and  edx, eax      ;   edx = [rest bytes]%4
		rep  movsd         ;N  DWORD MOVE
		add  edi, eax      ;U
		add  esi, eax      ;V

	MiniCopy:
		mov  ecx, edx      ;
		rep  movsb         ;N  BYTE MOVE

		cld                ;U
		mov  eax, [dst]    ;V  return dst
	}
}



#pragma warning( default : 4035 )

#else


///@todo filter部のオーバーフローをチェックする事


///@todo filter部のオーバーフローをチェックする事
void *dkcstd_memmove( void *dest, const void *src, size_t count ){
	uint8 *d = (uint8 *)dest;
  const uint8 *s = (const uint8 *)src;
	size_t i;
	
	uint8 *pb;
	//filter
	size_t alloc_offset;


	pb = (uint8*)s + count;//ここ、オーバーフロー？
	if((void *)pb > dest){
		alloc_offset = (size_t)(uint8*)(pb - d);
		pb = (uint8*)malloc(alloc_offset);
		//dest は srcの最後の部分と重なっているから･･･。
		memcpy(pb,dest,alloc_offset);
	}else{
		alloc_offset = 0;
		pb = NULL;
	}

	
	//process
	{
		size_t c = count - alloc_offset;
		for(i=0;i<c;i++){
			d[i] = s[i];
		}
		d += i;
		for(i=0;i<alloc_offset;i++){
			d[i] = pb[i];
		}
	}


	if(pb){
		free(pb);
	}
	return dest;
}
		
#endif

void *dkcstd_memcpy32( void *dest, const void *src, size_t count ){

	size_t i;
	size_t *d,*s;
	size_t cnt = count / sizeof(size_t);
	//size_t mod_ = count - cnt * sizeof(size_t);
	size_t mod_ = count % sizeof(size_t);//多分、上記よりはイイコードを吐く & 3みたいな？
	d = (size_t *)dest;
	s = (size_t *)src;
	for(i=0;i<cnt;i++){
		d[i] = s[i];
	}

	cnt = count - mod_;
	for(i=0;i<mod_;i++){
		(((uint8 *)dest)[cnt + i]) = (((const uint8 *)src)[cnt + i]);
	}
	return dest;
}

void *dkcstd_memcpy64f( void *dest, const void *src, size_t count ){
#define dkcd64BIT_T double						
	size_t i;
	dkcd64BIT_T *d,*s;
	size_t cnt = count / sizeof(dkcd64BIT_T);
	//size_t mod_ = count - cnt * sizeof(dkcd64BIT_T);
	size_t mod_ = count %  sizeof(dkcd64BIT_T);
	d = (dkcd64BIT_T *)dest;
	s = (dkcd64BIT_T *)src;
	for(i=0;i<cnt;i++){
		d[i] = s[i];
	}

	cnt = count - mod_;
	for(i=0;i<mod_;i++){
		(((uint8 *)dest)[cnt + i]) = (((const uint8 *)src)[cnt + i]);
	}
	return dest;
}

void *dkcstd_memcpy64( void *dest, const void *src, size_t count ){
#ifndef WIN32
	return dkcstd_memcpy64f(dest,src,count);
#else

#undef dkcd64BIT_T
#define dkcd64BIT_T ULONGLONG						
	size_t i;
	dkcd64BIT_T *d,*s;
	size_t cnt = count / sizeof(dkcd64BIT_T);
	//size_t mod_ = count - cnt * sizeof(dkcd64BIT_T);
	size_t mod_ = count %  sizeof(dkcd64BIT_T);
	d = (dkcd64BIT_T *)dest;
	s = (dkcd64BIT_T *)src;
	for(i=0;i<cnt;i++){
		d[i] = s[i];
	}

	cnt = count - mod_;
	for(i=0;i<mod_;i++){
		(((uint8 *)dest)[cnt + i]) = (((const uint8 *)src)[cnt + i]);
	}
	return dest;
#endif
}

void *dkcstd_memcpy( void *dest, const void *src, size_t count ){
	return dkcstd_memcpy64(dest,src,count);
}
	
	
void *dkcstd_memset( void *dest, int c, size_t count ){
	uint8 cc = (uint8)c;
	uint32 ccc = cc | (cc << 8) | (cc << 16) | (cc << 24);
	size_t i;
	size_t cnt = count / sizeof(uint32);// >> 2か？まぁ、最適化頼みって事で
	size_t mod = count % sizeof(uint32);
	size_t *d = dest;
	for(i=0;i<cnt;i++){
		d[i] = ccc;
	}
	dkcstd_memset8(&d[i],c,mod);
	return dest;
}

int dkcstd_memcmp( const void *buf1, const void *buf2, size_t count ){
	size_t i;
	const size_t *d,*s;
	size_t cnt = count / sizeof(size_t);
	//size_t mod_ = count - cnt * sizeof(size_t);
	size_t mod_ = count %  sizeof(size_t);
	d = (const size_t *)buf1;
	s = (const size_t *)buf2;
	for(i=0;i<cnt;i++){
		if(d[i] != s[i]){
			/*if(d[i] > s[i]){
				return d[i] - s[i];
			}else{
				s[i] - d[i];
			}*/
			
			const int *a,*b;
			a = &d[i];
			b = &s[i];
			return *a - *b;
			//return ((int *)&d[i]) - ((int *)&s[i]);
			
		}
	}
	cnt = count - mod_;
	return dkcstd_memcmp8(buf1,buf2,count);
}



void *dkcstd_memchr( const void *buf, int c, size_t count ){
	return dkcstd_memchr8( buf,  c,  count );
}




