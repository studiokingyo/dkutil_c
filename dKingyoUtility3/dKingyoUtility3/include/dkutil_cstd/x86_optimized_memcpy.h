/**
@author d金魚
@file x86_optimized_memcpy.h
@brief x86_optimized_memcpy.h
@since 2005/03/05
@see memory.hのnote
@note
memory.hの覚書を読んでください。
今の所 MSVC Onlyです。
*/


#ifndef DKUTIL_CSTD_X86_OPTIMIZED_MEMORY_H
#define DKUTIL_CSTD_X86_OPTIMIZED_MEMORY_H

#include "define.h"
#include "malloc.h"


#if 1==DKUTIL_CSTD_MSVC_IASM

/*
void mem2( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,2;
lx:
		mov eax,[esi];
		add esi,4;
		mov [edi],eax;
		add edi,4;
		dec ecx;
		jnz lx;
	}
}

void mem3( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,3;
lx:
		mov eax,[esi];
		mov eax,[esi+4];
		add esi,8;
		mov [edi],eax;
		mov [edi+4],eax;
		add edi,8;
		dec ecx;
		jnz lx;
	}
}
*/

DKC_INLINE void optimized_memcpy_12bytes(void *d, const void* s, size_t _size ) {
  //if(0==_size) return d;
  size_t t = _size / 12;
	_asm {
    mov edi,d			;
    mov esi,s			;
		mov ecx,t			;
    //mov eax,_size	;
		//mov ecx,0Ch		;12
    //div ecx;
		//mov ecx,eax;
lx:
    mov eax,[esi];
    mov ebx,[esi+4];
		mov edx,[esi+8];
    add esi,12;
    mov [edi],eax;
    mov [edi+4],ebx;
		mov [edi+8],edx;
    add edi,12;
    dec ecx;
    jnz lx;
  }
}



DKC_INLINE void optimized_memcpy_mmx_8bytes( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,3;
lx:
		movq mm0,[esi];
		add esi,8;
		movq [edi],mm0;
		add edi,8;
		dec ecx;
		jnz lx;
	}
}

DKC_INLINE void optimized_memcpy_mmx_16bytes( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
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
	}
}

DKC_INLINE void optimized_memcpy_mmx_movntq_8bytes( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,3;
lx:
		movq mm0,[esi];
		lea esi,[esi+8];
		movntq [edi],mm0;
		lea edi,[edi+8];
		dec ecx;
		jnz lx;
	}
}

DKC_INLINE void optimized_memcpy_mmx_movntq_16bytes( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,4;
lx:
		movq mm0,[esi];
		movq mm1,[esi+8];
		lea esi,[esi+16];
		movntq [edi],mm0;
		movntq [edi+8],mm1;
		lea edi,[edi+16];
		dec ecx;
		jnz lx;
	}
}

DKC_INLINE void optimized_memcpy_MMX_movntq_prefetch_16bytes( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,4;
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
	}
}

DKC_INLINE void memcpy_sse_16byte( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,4;
lx:
		movaps xmm0,[esi];
		lea esi,[esi+16];
		movaps [edi],xmm0;
		lea edi,[edi+16];
		dec ecx;
		jnz lx;
	}
}

DKC_INLINE void memcpy_sse_movntps_16bytes( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,4;
lx:
		movaps xmm0,[esi];
		lea esi,[esi+16];
		movntps [edi],xmm0;
		lea edi,[edi+16];
		dec ecx;
		jnz lx;
	}
}

DKC_INLINE void optimized_memcpy_sse_movntps_prefetch_32bytes( void * d, const void* s, size_t _size )
 {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,5;
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
	}
}



///なんかエラーします。使わんといてください。
DKC_INLINE void optimized_memcpy_fpu_8bytes(
	void * d, const void* s, size_t _size )
{	//unsigned char a[256],b[256];
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,3;
lx:
		fld double ptr [esi];
		lea esi,[esi+8];
		fstp double ptr [edi];
		lea edi,[edi+8];
		dec ecx;
		jnz lx;
	}

	//memcpy(a,d,_size);
	//memcpy(b,s,_size);
}


DKC_INLINE void optimized_memcpy_rep( void * d, const void* s, size_t _size ) {
	_asm {
		mov edi,d;
		mov esi,s;
		mov ecx,_size;
		shr ecx,2;
		rep movsd;
	}
}


#endif


#endif //end of include once
