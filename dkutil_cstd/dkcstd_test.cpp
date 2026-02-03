

#define DKUTIL_CSTD_NOT_DEFINE_STDLIB_MACRO
#include <dkutil_cstd/stdlib.h>

#include <dkutil_c/dkc.h>

#include <dkutil_unit_test/unit_test_macro.hpp>

void *memfpu( void* d, const void* s, int _size ) {
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
	return d;
}

void dkcstd_memcpy_test(void *dest1,void *dest2,const void *src,size_t count)
{


#	define memcpy_test_base( funcname,dest1,dest2,src,count ) \
	memset(dest1,0x00,count);\
	memset(dest2,0xFF,count);\
	memcpy(dest1,src,count);\
	funcname((void *)dest2,(const void *)src,count);\
	dkcmASSERT(memcmp(dest1,dest2,count) == 0);\
	dkcmASSERT(	memcpy(dest1,src,count) == funcname((void *)dest1,(const void *)src,count) )

#	define memcpy_test( funcname) memcpy_test_base(funcname, ,dest1,dest2,src,count)

//C language
	memcpy_test( dkcstd_memcpy );
	memcpy_test( dkcstd_memcpy8 );
	memcpy_test( dkcstd_memcpy32 );
	memcpy_test( dkcstd_memcpy64 );
	memcpy_test( dkcstd_memcpy64f );
//asm


	UINT f = dkcstd_getX86CPUType();

	if(f & edkcCPU_MMX){
		memcpy_test( dkcstd_memcpy_mmx128 );
		memcpy_test( dkcstd_memcpy_mmx64 );
	}
	//memcpy_test( 			memfpu);
	//memcpy_test( dkcstd_memcpy_FPU64 );


	memcpy_test( dkcstd_memcpy_asm64 );
	memcpy_test( dkcstd_memcpy_asm32 );
	memcpy_test( dkcstd_memcpy_rep_movsd );



	if(f & edkcCPU_SSE ){


		memcpy_test( dkcstd_memcpy_movups_sse_prefetch256 );
		DKCSTD_ALIGNED_MEM m;
		dkcstd_get_memory_align(&m,src,count,16);
		BYTE *srct =(BYTE *)m.mem;
		dkcstd_get_memory_align(&m,dest1,count,16);
		BYTE *d1t = (BYTE *)m.mem;
		dkcstd_get_memory_align(&m,dest2,count,16);
		BYTE *d2t = (BYTE *)m.mem;

		size_t c = m.access_permit_from_mem;
#	define memcpy_sse_test(funcname) memcpy_test_base(funcname,d1t,d2t,srct,c)

		memcpy_sse_test( dkcstd_memcpy_movntps_sse_prefetch256 );
		memcpy_sse_test( dkcstd_memcpy_movntps_sse128 );
		
		memcpy_sse_test( dkcstd_memcpy_movntq_mmx_prefetch128 );
		memcpy_sse_test( dkcstd_memcpy_movntq_mmx128 );
		memcpy_sse_test( dkcstd_memcpy_movntq_mmx64 );
		memcpy_sse_test( dkcstd_memcpy_sse128 );
	}
}

void dkcstd_test(){
	const int size = 256;
	//memory.h
	{
		BYTE a[size],b[size],data[size];
			
		//memcpy
		dkcstd_memcpy_test(a,b,data,sizeof(a));
	}

	{
		BYTE a[size],b[size],data[size];
		memset(data,0xF2,sizeof(data));
		
		//memset
		memset(a,1,sizeof(a));
		dkcstd_memset(b,1,sizeof(b));

			dkcmASSERT(memcmp(a,b,sizeof(a)) == 0);
		




		//memmove
		memmove(a,a + 3,sizeof(a) - 3);
		dkcstd_memcpy(b,b + 3,sizeof(b) - 3);

			dkcmASSERT(memcmp(a,b,sizeof(a)) == 0);

		//memcmp
			dkcmASSERT(
				dkcstd_memcmp(a,b,sizeof(a)) == 
				memcmp(a,b,sizeof(a))
			);

		//memchr
		FOR(int i=0;i<size;i++){
			a[i] = b[i] = i;
		}
			dkcmASSERT(
				memchr(a,size - 1,sizeof(a)) == 
				dkcstd_memchr(a,size - 1,sizeof(a))
			);
			
	
	//strings.h
/*
	//bcopy
	dkcstd_bcopy(data,a,sizeof(a));
	memcpy(b,data,sizeof(b));
	dkcmASSERT(memcmp(a,b,sizeof(a)) == 0);

	dkcstd_bzero
#define bzero(a,b) dkcstd_bzero(a,b)
#define bcmp(a,b,c) dkcstd_bcmp(a,b,c)
	*/
	}

	//string.h
	{
		char a[size],b[size],data[size];
		NULL_CHAR_ARRAY(a);
		NULL_CHAR_ARRAY(b);
		NULL_CHAR_ARRAY(data);
		FOR(int i=0;i<'z' - 'a';i++){
			data[i] = 'a' + i;
		}

		//strcpy
		strcpy(a,data);
		dkcstd_strcpy(b,data);

		dkcmASSERT(strcmp(a,b) == 0);
		//strlen
		dkcmASSERT(strlen(a)==dkcstd_strlen(a));

		//strcmp
		dkcmASSERT(strcmp(a,b) == dkcstd_strcmp(a,b));

		//strncmp
		dkcmASSERT(strncmp(a,b,sizeof(a)) == dkcstd_strncmp(a,b,sizeof(a)));

		//strstr
		dkcmASSERT(strstr(a,"yz") == dkcstd_strstr(a,"yz"));
	}
	{
		const char *numstr = "12345";
		const char *numstr16 = "3039";
		const char *d10 = "4210818301";
		const char *d16 = "FAFBFCFD";
		dkcmASSERT(atoi(numstr) == dkcstd_atoi(numstr));
		dkcmASSERT(atoi(numstr) == dkcstd_atox(numstr16));
		dkcmASSERT(atoi(d10) == dkcstd_atox(d16));
	}


}

int main(){

	dkcstd_test();
	return edk_SUCCEEDED;
}

//stdlib.h

