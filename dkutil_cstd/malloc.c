/**
@author d金魚
@file malloc.c
@brief malloc.c
@since 2005/01/17
*/

#include "malloc.h"
#include "memory.h"

DKCSTD_MALLOC_STATE dkcstd_malloc_state_obj;

#define STATE dkcstd_malloc_state_obj


DKCSTD_MALLOC_STATE *dkcstd_malloc_init(size_t init_size,size_t expand_size){
	STATE.init_size = init_size;
	STATE.expand_size = expand_size;
	STATE.alloc_count = 0;

	return &STATE;
}

int dkcstd_malloc_end(){
	int r;
	if(STATE.alloc_count != 0){
		r = -1;
	}

	return r;
}

DKCSTD_MALLOC_STATE *dkcstd_get_malloc_state(){
	return &STATE;
}




void *dkcstd_malloc( size_t size){
	if(0==size){
		return NULL;
	}
	return dkcstdMALLOC_BASE(size);
}

void dkcstd_free(void *p){
	dkcstdFREE_BASE(p);
}


void *dkcstd_realloc( void *memblock, size_t size ){
	if(NULL==memblock){
		return dkcstd_malloc(size);
	}
	return dkcstdREALLOC_BASE(memblock,size);
}



static DKC_INLINE void *aligned_malloc_calc(void *a){
	BYTE *p = (BYTE *)a;
	uint32 *op,*base = (uint32 *)p;
	//assert(sizeof(uint32 *) == sizeof(void *));
	
	
	p += sizeof(void *);
	p = dkcstd_pointer_align(p,dkcstdMALLOC_MEMORY_ALIGN);
	
	//最初のアドレスを保存
	op = (uint32 *)( (BYTE *)(p - sizeof(void *)) );
	*op = (uint32)base;
	return p;
}

static DKC_INLINE void *aligned_free_calc(void *a){
	BYTE *p = a;
	uint32 *pb;
	void *f;
	pb = (uint32 *)( (BYTE *)(p - sizeof(void *)) );
	f = (void *)(*pb);//ただのpbになっていた。○|￣|_
	return f;
}

void *dkcstd_aligned_malloc(size_t size){
	size_t ss = size + 	dkcstdMALLOC_MEMORY_ALIGN + sizeof(void *);
	void *p = dkcstd_malloc(ss);

	
	if(NULL==p){
		return NULL;
	}
	p = aligned_malloc_calc(p);

	return p;
}


void dkcstd_aligned_free(void *a){
	a = aligned_free_calc(a);
	dkcstd_free(a);
}

void *dkcstd_aligned_realloc(void *memblock,size_t size){
	void *p;
	//if(memblock){
		memblock = aligned_free_calc(memblock);
		p = dkcstd_realloc(memblock,size);
		p = aligned_malloc_calc(p);
	//}else{
	//	p = aligned_malloc(size);
	//}
	return p;
}


#undef STATE





