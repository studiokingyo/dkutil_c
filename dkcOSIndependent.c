
/*!
@file dkcOSIndependent.c
@author d����
@
*/
#define DKUTIL_C_OS_INDEPENDENT_C

#include "dkcOSIndependent.h"
#include "dkcDefined.h"

#include "dkcQueue.h"


#include <assert.h>
#include <math.h>


#ifdef WIN32

#include <shellapi.h>

#pragma warning(disable:4127)

int dkcDynamicCheckEnvironment(BOOL isForceExit){
	int r = edk_FAILED;
	if((sizeof(int) != sizeof(long))) goto Error;
	if(sizeof(int) != 4) goto Error;
	
	if((sizeof(UINT) != sizeof(ULONG))) goto Error;
	if(sizeof(UINT) != 4) goto Error;

	if((sizeof(BYTE) != sizeof(char))) goto Error;
	if((sizeof(BYTE) != 1)) goto Error;
	if(sizeof(double) != 8) goto Error;
	if(sizeof(void *) != sizeof(unsigned)) goto Error;
	if(sizeof(void *) != sizeof(int)) goto Error;
	if(sizeof(short) != 2) goto Error;
	r = edk_SUCCEEDED;
Error:
	if(DKUTIL_FAILED(r) && isForceExit){
		exit(-1);
	}
	return r;
}

#pragma warning(default:4127)
#else
int dkcDynamicCheckEnvironment(BOOL isForceExit){

}

#endif


int WINAPI dkcIsNativePathString(const char *s,size_t size){

	size_t i=0;
	if(s==NULL) return edk_FAILED;
	for(;i<size;i++){
		if(dkcmIS_INVALID_PATH_CHAR(s[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}

DKC_INLINE int WINAPI dkcOutputDebugString(const char *str,...){
	char s[1024 * 2];

	
	int va_list_result;
	va_list Valist;

	
	
	SET_VA_LIST_C(Valist,va_list_result,s,sizeof(s),str);
	if(0 > va_list_result){
		return edk_FAILED;
	}
	OutputDebugString(s);
	return edk_SUCCEEDED;
	//fprintf(stderr,s);
}

int WINAPI dkcErrorMessage(const char *expression,
										 const char *filename,size_t line,const char *message,...)
{
	char s[1024];
	char ss[1024 * 2];
	
	int va_list_result;
	va_list Valist;

	NULL_CHAR_ARRAY(s);
	NULL_CHAR_ARRAY(ss);
	
	if(message){
		SET_VA_LIST_C(Valist,va_list_result,s,sizeof(s),message);
	}else{
		//SET_VA_LIST(s,sizeof(s),"No message");
		_snprintf(s,sizeof(s),"No message\n");
	}

	_snprintf(ss,sizeof(ss),
		"DKUTIL_ASSERT(%s):\nfile:%s / \n line:%d / \n message: %s",
		expression,filename,line,s);



#ifdef NDEBUG//release
#	ifdef WIN32
		ODS(ss);
	//::MessageBox(NULL,ss,"ERROR ASSERTION !!",MB_OK);
	{
		LPCTSTR name="ERROR_TEMP.txt";
		{
			//�t�@�C�����I�[�v��
			FILE *fp;
			if(NULL != (fp = fopen( name , "at" ))){
			
				// �G���[���O�t�@�C���ɏ����o��
				fputs( ss , fp ) ;
				fclose( fp ) ;
				Sleep(800);
			}
		}
		ShellExecute(NULL, "open",name, NULL, NULL, SW_SHOWNORMAL);
		//Sleep(1000);
		//DeleteFile(name);
	}
	MB("���̃e�L�X�g�����̃\�t�g�̊J���҂ɓn���΁A�o�O�����̋ߓ��Ȃ�܂��B\n \
		�J���҂ɒm�点��Ƃ��͂��̃e�L�X�g��]�L���Ă����܂��傤�B");
	
	/*if(IDYES==MessageBox(NULL,
		"���̂܂܂��̃\�t�g���N�����Â���ƁA�X�Ȃ�G���[����������\��������܂��B\n�I�����܂����H",
		"�����Ă͂����Ȃ� �G���[!!!",
		MB_YESNO))
	{
		exit(edk_FAILED);
		//terminate();
	}else{
	}*/
	exit(edk_FAILED);
#	else	//else of win32
	
	fprintf(stderr,ss);
	exit(edk_FAILED);

#	endif //end of win32

#else //debug

#	ifdef WIN32
		ODS(ss);
	fprintf(stderr,ss);
	//_CrtDebugBreak();
	DebugBreak();
#	else //else of win32

	fprintf(stderr,ss);
	Debugger();

#	endif //end of win32
		
	return TRUE;
#endif//end of NDEBUG
	//assert(ss);

}

void *WINAPIV dkcMallocAdapter(size_t s){
	return malloc(s);
}

void WINAPIV dkcFreeAdapter(void *p){
	if(p){
		free(p);
	}
}

DKC_INLINE void *dkcAllocateFill(size_t size,BYTE fill){
	return dkcAllocateFill_INL(size,fill);
}
DKC_INLINE void *dkcAllocateFast(size_t size){
	return dkcAllocateFast_INL(size);
}
DKC_INLINE void* dkcAllocate(size_t size){
	return dkcAllocate_INL(size);
}

DKC_INLINE int	dkcFree(void **p){
	return dkcFree_INL(p);
}
BOOL dkcIs_foepn_mode(const char *s){
	/* wrtb+ */
	const char *ms= "wrtb+";
	size_t slen = strlen(s);
	size_t mslen = strlen(ms);
	size_t i,j;
	int flag = FALSE;
	for(i=0;i<slen;i++){
		for(j=0;j<mslen;j++){
			if(ms[j]==s[i]){
				flag = TRUE;
				break;
			}
		}
		if(TRUE!=flag){
			return FALSE;
		}else{
			flag = FALSE;
		}
	}
	return TRUE;
}
/*
struct dkcDoublePoint{
	double x,y;
}DKC_DOUBLE_POINT;

int calc(DKC_DOUBLE_POINT *pa,size_t nums){
	pa->x	

}*/
/*
double calc(double StartX,double StartY,double EndX,double EndY)
{
	return	StartY - ((EndY - StartY) / (EndX - StartX)) * StartX;
}
*/
size_t dkcReallocateSizeFunction(size_t OldSize,size_t ExpandSize){
	//y=-25.43035 * pow(x,2) + 116.7214
	//y = -0.00000018 * pow(x,2) + 0.00019
	//y = -99.9999 * pow(OldSize,2) + 104857600;
	//y = 0.105263 * x * x + 10.52631
	//double x;
//	size_t size;
	if(OldSize <= 1024 * 5){//5KB�ȓ��Ȃ�2�{
		return OldSize * 2;
	}else if(OldSize >= 1024 * 1024 * 10 || 0==OldSize)
	{//10MB�ȏ�Ȃ� + ExpandSize
		return OldSize + ExpandSize;
	}
	//����ȊO�Ȃ�ExpandSize * 3
	return OldSize + ExpandSize * 3; 
	//return OldSize * 2;//�Ђł��֐����B
}

int WINAPI dkcReallocate(void **NewPtr,size_t NewSize,
	void **OldPtr)
{
	return dkcReallocateEx(realloc,NewPtr,NewSize,OldPtr);
}

int WINAPI dkcReallocateEx(
	DKC_REALLOC_F_TYPE your_realloc,void **NewPtr,size_t NewSize,
	void **OldPtr
)
{
	void *ptr = NULL;

	if(NULL== your_realloc  /*|| NULL == *OldPtr ||
		0==OldSize*/
	){
		return edk_ArgumentException;//�A�z���������ȁI�I
	}

	ptr = your_realloc(*OldPtr,NewSize);
	if(NULL==ptr) return edk_FAILED;

	//�X�V
	*OldPtr = NULL;
	*NewPtr = ptr;
	
	return edk_SUCCEEDED;
}



int WINAPI dkcReallocateAutoExpand(
	DKC_REALLOC_F_TYPE your_realloc,void **NewPtr,
	void **OldPtr,size_t OldSize,size_t *reallocated_size
	)
{
	void *ptr = NULL;
	size_t want_size = OldSize / 8;
	size_t ra_size = dkcReallocateSizeFunction(OldSize,
		 (want_size <= 20) ? 20 : want_size
	);
	size_t div = 8;
	size_t tmp_ra = ra_size / div;
	size_t i;
	if(NULL== your_realloc /*|| NULL==*NewPtr*/ || NULL == *OldPtr ||
		0==OldSize  || NULL==reallocated_size	
	){
		return edk_ArgumentException;//�A�z���������ȁI�I
	}

	for(i=0;i<div;i++){
		ptr = your_realloc(*OldPtr,ra_size);
		if(NULL==ptr){
			if(OldSize > (ra_size -= tmp_ra)){//�Ƃ肠����tmp_ra���������B
				break;//����]�݂Ȃ�����
			}
			continue;
		}else{
			break;
		}
	}
	if(NULL==ptr) return edk_FAILED;

	//�X�V
	*NewPtr = ptr;	
	*OldPtr = NULL;

	*reallocated_size = ra_size;

	return edk_SUCCEEDED;
}

int WINAPI dkcGetMemorySize(uint64 *pTotalMemory,uint64 *pFreeMemory)
{
#ifdef WIN32
	MEMORYSTATUS ms;
	memset(&ms,0,sizeof(ms));
	ms.dwLength =  sizeof( ms );
	GlobalMemoryStatus( &ms );

	if ( pTotalMemory == NULL || pFreeMemory  == NULL)
		return edk_FAILED;

	*pTotalMemory = (uint64)ms.dwTotalPhys;
	*pFreeMemory  = (uint64)ms.dwAvailPhys;
	return edk_SUCCEEDED;
#else


#endif
}


static int WINAPI dkcLoadFile(void *data,size_t size,const char *fname,const char *mode,size_t *readsize){//="rb"
	FILE *fp ;
	if(NULL==readsize || NULL==data || NULL==fname || NULL==mode){
		return edk_FAILED;
	}
	fp = fopen( fname , mode ) ;//rb
	if(fp==NULL)return edk_FAILED;
	*readsize = dkcFReadAll(data,size,fp);
	//*readsize = fread( data , 1 , size , fp ) ;
	//fread( data , sizeof(size)  , 1 , fp ) ;
	fclose( fp ) ;

	return edk_SUCCEEDED;

}
static int WINAPI dkcSaveFile(const void *data,size_t size,const char *fname,const char *mode)
{
	FILE *fp;
	if(NULL==fname || NULL==mode){
		return edk_FAILED;
	}
	fp = fopen( fname , mode ) ;//wb
	if(fp==NULL) return edk_FAILED;
	dkcFWriteAll(data,size,fp);
	//fwrite( data , size  , 1 , fp ) ;
	//fwrite( data , sizeof(size)  , 1 , fp ) ;
	fclose( fp ) ;
	return edk_SUCCEEDED;
}


DKC_EXTERN FILE * WINAPI dkcFOpen(const char *filename,const char *mode){
	return fopen(filename,mode);
}

DKC_EXTERN int WINAPI dkcFClose(FILE **ptr){
	if(NULL==ptr || NULL==*ptr) return edk_ArgumentException;
	fclose(*ptr);
	*ptr = NULL;
	return edk_SUCCEEDED;
}
DKC_EXTERN size_t WINAPI dkcFReadAll(void *buffer,size_t size,FILE *fp)
{
	size_t count;
	size_t tc,tsize;
	BYTE *tbuffer;

	//read size
	tsize = size;
	//temp variable
	tc = 0;
	//offset counter
	count = 0;
	//byte type pointer
	tbuffer = (BYTE *)buffer;
	for(;;)
	{
		//error check
		if(ferror(fp) ){
			break;
		}
		if(feof(fp)){
			break;
		}
		//read
		tc = fread(&tbuffer[count],1,tsize,fp);
		//update
		tsize -= tc;
		count += tc;

		if(count == size){
			break;
		}
#	ifdef DEBUG
		//���肦�Ȃ��G���[�`�F�b�N
		dkcmNOT_ASSERT(count > size);
#	else
		if(count > size){
			break;
		}
#	endif
	}
	return count;
}

DKC_EXTERN size_t WINAPI dkcFWriteAll(const void *buffer,size_t size,FILE *fp){
	size_t count;
	size_t tc,tsize;
	BYTE *tbuffer;

	//read size
	tsize = size;
	//temp variable
	tc = 0;
	//offset counter
	count = 0;
	//byte type pointer
	tbuffer = (BYTE *)buffer;
	for(;;)
	{
		//error check
		if(ferror(fp) ){
			break;
		}
		if(feof(fp)){
			break;
		}
		//read
		tc = fwrite(&tbuffer[count],1,tsize,fp);
		//update
		tsize -= tc;
		count += tc;

		if(count == size){
			break;
		}
#	ifdef DEBUG
		//���肦�Ȃ��G���[�`�F�b�N
		dkcmNOT_ASSERT(count > size);
#	else
		if(count > size){
			break;
		}
#	endif
	}
	return count;

}


DKC_INLINE size_t WINAPI dkcFSize(FILE *fp)
{
	long t;
	size_t len;
	//temporary
	t = ftell(fp);
	fseek(fp,0,SEEK_END);
	len = ftell(fp);
	fseek(fp,t,SEEK_SET);
	return len;
}

//fopen�̃t�@�C���֐����g���ăo�C�i���Z�[�u���s���B
int WINAPI dkcSaveBinary(const void *data,size_t size,const char *fname){//="wb"
	return dkcSaveFile(data,size,fname,"wb");
}

DKC_INLINE BOOL WINAPI dkcCreateEmptyFile(const char *filename){
	FILE *fp;
	fp = fopen( filename , "wb" ) ;//wb
	if(fp==NULL) return FALSE;
	fclose( fp ) ;
	return TRUE;
}


DKC_INLINE int WINAPI dkcLoadBinary(void *data,size_t size,const char *fname,size_t *readsize){//="rb"
	return dkcLoadFile(data,size,fname,"rb",readsize);
}

DKC_INLINE int WINAPI dkcSaveText(const char *text,size_t length,const char *fname){
	return dkcSaveFile(text,length,fname,"wt");
}
DKC_INLINE int WINAPI dkcLoadText(char *text,size_t length,const char *fname,size_t *readsize){
	return dkcLoadFile(text,length,fname,"rt",readsize);
}


DKC_INLINE int WINAPI dkcInitFileFast( const char *filename )
{
	FILE *fp;
	// �G���[���O�t�@�C�����J��(������)
	if(NULL==(fp = fopen( filename , "wt" ))){
		return edk_FAILED;
	}
	fclose( fp ) ;
	
	return edk_SUCCEEDED;
}

DKC_INLINE  int WINAPI dkcAddLogFile( const char *filename,const char *str , ... ){
	FILE *fp=NULL;
	char s[2048]="";
	va_list VaList;
	int r;
	//SET_VA_LIST_INSERT_YEN_N(s,2048,str);
	SET_VA_LIST_C(VaList,r,s,sizeof(s),str);

	//�t�@�C�����I�[�v��
	if(NULL == (fp = fopen( filename , "at" )))
		return edk_FAILED;
	// �G���[���O�t�@�C���ɏ����o��
	fputs( s , fp ) ;
	fclose( fp ) ;
	return edk_SUCCEEDED;
}


BOOL WINAPI dkcSwapFast(void *p1,void *p2,size_t size)
{
	void *p;
#if 0//stack overflow ��O����������̂��������狓���͋��ʂŁB
	int state = 0;
	p = _alloca( size );
	if(NULL==p){
		state = 1;
		p = malloc(size);
		if(NULL==p)
			return FALSE;
	}
#else
	p = malloc(size);
	if(NULL==p)
		return FALSE;
#endif
	memcpy(p,p1,size);
	memcpy(p1,p2,size);
	memcpy(p2,p,size);
#if 0
	if(1==state){
		free(p);
	}

#else
	free(p);
#endif
	return TRUE;
}
/*
template<typename TYPE_T>
inline void dkcSwapT(TYPE_T *p1,TYPE_T *p2,size_t size){
	size_t i;
	char buf;
	size_t mod_ = size % sizeof(TYPE_T);
	size_t cnt = size / sizeof(TYPE_T);
	for(i=0;i<cnt;i++){
		SWAP_NUM(p1[i],p2[i]);
	}
	for(i=0;i<mod_;i++){
		buf = (((BYTE *)p1)[size - mod_ + i]);
		(((BYTE *)p1)[size - mod_ + i]) = 
			(((BYTE *)p2)[size - mod_ + i]) ;
		(((BYTE *)p2)[size - mod_ + i]) = buf;
	}

}


void WINAPI	dkcSwapFast(int *p1,int *p2,size_t size)
{
	size_t i;
	char buf;
	size_t mod_ = size % sizeof(int);
	size_t cnt = size / sizeof(int);
	for(i=0;i<cnt;i++){
		SWAP_NUM(p1[i],p2[i]);
	}
	for(i=0;i<mod_;i++){
		buf = (((BYTE *)p1)[size - mod_ + i]);
		(((BYTE *)p1)[size - mod_ + i]) = 
			(((BYTE *)p2)[size - mod_ + i]) ;
		(((BYTE *)p2)[size - mod_ + i]) = buf;
	}
}*/

#ifdef _MSC_VER
#	pragma warning(disable:4244)
#endif

void WINAPI dkcSwap64(ULONGLONG *p1,ULONGLONG *p2,size_t size){
	size_t i;
	BYTE buf;
	size_t mod_ = size % sizeof(ULONGLONG);
	size_t cnt = size / sizeof(ULONGLONG);
	//char *tp1 = (char *)p1,*tp2 = (char *)p2;
	for(i=0;i<cnt;i++){
		SWAP_TEMP(register ULONGLONG,p1[i],p2[i]);
		//SWAP_NUM(p1[i],p2[i]);
	}
	for(i=0;i<mod_;i++){
		
		//SWAP_NUM((char)tp1[size - mod_ + 1],(char)tp2[size - mod_ + i]);
		/*a = b - a ;\
    b -= a ;\
    a += b
		*/
		/*( ( ((char *) p2)[size - mod_ + i]))
			= (char)( ( ((char *) p2)[size - mod_ + i])) - 
			(char)( ( ((char *) p1)[size - mod_ + i]));
		
		( ( ((char *) p2)[size - mod_ + i])) -= ( ( ((char *) p1)[size - mod_ + i]));

		( ( ((char *) p1)[size - mod_ + i])) += ( ( ((char *) p2)[size - mod_ + i]));
			*/
		
		
		buf = (((BYTE *)p1)[size - mod_ + i]);
		(((BYTE *)p1)[size - mod_ + i]) = 
			(((BYTE *)p2)[size - mod_ + i]) ;
		(((BYTE *)p2)[size - mod_ + i]) = buf;
		
	}

}

void WINAPI dkcSwap(void *p1,void *p2,size_t size)
{
	dkcSwap64((ULONGLONG *)p1,(ULONGLONG *)p2,size);
}



DKC_INLINE void WINAPI dkcTwoDWORDToULONGLONG(ULONGLONG *dest,DWORD high,DWORD low){
	//*dest = (ULONGLONG)((high << 32) + low);
	*dest = (ULONGLONG)high << 32;
	*dest += low;
		//( high * MAXDWORD ) + low;
}
///@todo �G���f�B�A�������ǂ��ɂ����悤�I(����H
DKC_INLINE void WINAPI dkcULONGLONGToTwoDWORD(DWORD *dhigh,DWORD *dlow,ULONGLONG src){
	DWORD *o = (DWORD *)&src;
	dkcmNOT_ASSERT(sizeof(DWORD) * 2 != sizeof(ULONGLONG));


	//*dhigh = o[0];*dlow = o[1];
	*dhigh = o[1];*dlow = o[0];
}

DKC_INLINE void WINAPI dkcLONGLONGToTwoLONG(LONG *high,LONG *low,LONGLONG src){
	/*LONG *o = (LONG *)&src;
	dkcmNOT_ASSERT(sizeof(LONG) * 2 != sizeof(LONGLONG));
	*high = o[0];
	*low = o[1];*/
#ifdef WIN32
	DKC_TWOINT32_TO_INT64 a;
	a.QuadPart = src;
	*low = a.u.LowPart;
	*high = a.u.HighPart;
#else

#endif
}

DKC_INLINE void WINAPI dkcTwoLONGToLONGLONG(LONGLONG *dest,LONG high,LONG low){
	/*//*dest = (ULONGLONG)((high << 32) + low);
	*dest = (LONGLONG)high << 32;
	//*dest += low;
	(*dest) &= low;
	*/
#ifdef WIN32
	DKC_TWOINT32_TO_INT64 a;

	a.u.HighPart = high;
	a.u.LowPart = low;
	*dest = a.QuadPart;

#else

#endif
}

/*
//�܂����A����͓���B
DKC_INLINE int WINAPI dkcDataReplace(BYTE *dest,size_t destsize,
																		 const BYTE *src,size_t srcsize,
																		 const BYTE *target_data,size_t target_data_size,
																		 const BYTE *replace_data,size_t replace_data_size)
{
	
	size_t i;

	//�Ƃ肠�����A20�@�Ł@offset��size_t��
	DKC_QUEUE *pq = dkcAllocQueue(20,sizeof(size_t));
	if(NULL==pq){
		return edk_OutOfMemory;
	}

	//�܂��͒u���ꏊ��T��
	for(i=0;i<srcsize;i++){
		memcmp(&src[i],





}*/



#ifdef _MSC_VER
#	pragma warning(default:4244)



void WINAPI dkcCheckMemoryLeak(BOOL flag){
	if(flag){
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}else{
		_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);
	}
}

#else

void WINAPI dkcCheckMemoryLeak(BOOL flag){

}
#endif

const char *WINAPI dkcGetPathSep(){
#ifdef WIN32
	static char target[3]={dkcdPATH_SEP,'/','\0'};
#else
	static char target[3]={dkcdPATH_SEP,'\0'};
#endif
	return target;
}


#ifdef _MSC_VER
BOOL WINAPI dkcIsMMX(void)
{
	BOOL flag = FALSE;

	__asm{
		push	edx
		push	ecx
		pushfd
		pop		eax
		xor		eax, 00200000h
		push	eax
		popfd
		pushfd
		pop		ebx
		cmp		eax, ebx
		jnz		non_mmx
		mov		eax, 0
		cpuid
		cmp		eax, 0
		jz		non_mmx
		mov		eax, 1
		cpuid
		and		edx, 00800000h
		jz		non_mmx
		mov		flag, TRUE
	non_mmx:
		pop		ecx
		pop		edx
	}
	return flag;
}
#else
BOOL WINAPI dkcIsMMX(void){
  return FALSE;
}
#endif


DKC_INLINE USHORT dkcReverseEndian16(USHORT x){
	return (USHORT)((x >> 8) | (x << 8));
}

DKC_INLINE DWORD dkcReverseEndian32(DWORD x){
	return (x << 24) | ((x & 0x0000ff00) << 8) | ((x & 0x00ff0000) >> 8) | (x >> 24);
}

DKC_INLINE ULONGLONG dkcReverseEndian64(ULONGLONG x) {

	return (
		(ULONGLONG)dkcReverseEndian32((DWORD)(x & 0x00000000ffffffff)) << 32)
		| 
		dkcReverseEndian32((DWORD)(x >> 32)
	);

}
DKC_INLINE  BOOL dkcIsLittleEndian(){
	int x=1;
	return (BOOL)((*(char*)&x));
}



int WINAPI dkcGetByteOrder()
{
	DKC_BYTE_ORDER_CHECK_UNION a;
	a.x = 0x04030201;
	switch(a.abcd[0]){
	case 0x01:
		return edkcByteOrder_DCBA;
	case 0x02:
		return edkcByteOrder_Unknown; 
	case 0x03:
		return edkcByteOrder_BADC;
	case 0x04:
		return edkcByteOrder_ABCD;
	}
	return edkcByteOrder_Unknown;
}

DKC_INLINE BOOL dkcCheckOverflowULONG(ULONG a1,ULONG a2){
	ULONG maxv = ULONG_MAX;
  ULONG halfv = maxv / 2 ;
  ULONG x = a1,y = a2;
  if(x < y){
    SWAP_NUM(x,y);
  }
  if(x > halfv + 1){
    if(y >= halfv){
      return TRUE;
    }
  }
  return FALSE;
}

DKC_INLINE BOOL dkcCheckOverflowULONGLONG(ULONGLONG a1,ULONGLONG a2)
{
	ULONGLONG maxv = ULONGLONG_MAX;
  ULONGLONG halfv = maxv / 2 ;
  ULONGLONG x = a1,y = a2;
  if(x < y){
    SWAP_NUM(x,y);
  }
  if(x > halfv + 1){
    if(y >= halfv){
      return TRUE;
    }
  }
  return FALSE;
}


static void *get_offset(void *p,size_t offset){
	BYTE *oo = (BYTE *)p;
	
	return (void *)&(oo[offset]);
}

/*!
n:�z��̍đ䐔
k:���ׂ����ԍ��i��������������j
@return offset number ,Error : UINT_MAX
*/
int WINAPI dkcSelect(void *dest,const void *a_src,size_t n,int k,size_t width ,DKC_COMPARE_TYPE less)
{

	int i,j,left, right;

	void *x = malloc(width);
	void *a = malloc(n * width);
	if(NULL==x || NULL==a){
		return edk_ArgumentException;
	}
	if(n > INT_MAX){
		return edk_FAILED;
	}
	

	memcpy(a,a_src,width * n);


	left = 0;  right = n - 1;

	while (left < right) 
	{

		memcpy(x,	(const void *)get_offset(a,k * width),width);
		i = left;
		j = right;

		for ( ; ; ) {

			while( less(get_offset(a,i * width),x, width) > 0){
			//while( *(int *)get_offset(a,i * width) < *(int *)x){
				i++;
			}

			while( less(x,get_offset(a,j * width), width) > 0){
			//while( *(int *)x < *(int *)get_offset(a,j * width) ){
				j--;
			}

			if (i > j){
				break;
			}
			

			dkcSwapFast(
				get_offset(a,i * width),
				get_offset(a,j * width),
				width
				);
			
			i++;
			j--;
		}
		if (j < k){
			left  = i;
		}
		if (k < i){
			right = j;
		}
	}

	//�Q�b�g����
	{
		//printf("%d",*(int *)get_offset(a,k * width));

		memcpy(dest,(const void *)get_offset(a,k * width),width);

	}
	free(x);
	free(a);

	return edk_SUCCEEDED;

}


DKC_INLINE int WINAPI dkcSelectMax(void *a,const void *a_src, size_t n,size_t width ,DKC_COMPARE_TYPE less){
	if(n-1 > INT_MAX){
		return edk_FAILED;
	}
	return dkcSelect(a,a_src,n,(int)n-1,width,less);
}
DKC_INLINE int WINAPI dkcSelectMin(void *a,const void *a_src, size_t n,size_t width ,DKC_COMPARE_TYPE less){
	return dkcSelect(a,a_src,n,(int)0,width,less);
}
#if 0
int WINAPI dkcSelect(void *dest,const void *a_src,size_t n,size_t k,size_t width ,DKC_COMPARE_TYPE less)
{
	//int i, j, left, right;
	size_t i,j,left, right;
	//keytype x, t;
	//size_t x,t;
	void *x = malloc(width);
	void *a = malloc(n * width);
	if(NULL==x || NULL==a){
		return edk_ArgumentException;
	}
	
	//void *t = malloc(width);
	memcpy(a,a_src,width * n);


	left = 0;  right = n - 1;
	/*
	x = a[k];  i = left;  j = right;
		for ( ; ; ) {
			while (a[i] < x) i++;
			while (x < a[j]) j--;
			if (i > j) break;
			t = a[i];  a[i] = a[j];  a[j] = t;
			i++;  j--;
		}
		if (j < k) left  = i;
		if (k < i) right = j;
	*/
	while (left < right) 
	{
		//x = a[k];  i = left;  j = right;
		//offset_temp = (BYTE *)a;
	
		//memcpy(x,	(const void *)&offset_temp[k * width],width);
		memcpy(x,	(const void *)get_offset(a,k * width),width);
		i = left;
		j = right;

		for ( ; ; ) {
			//while (a[i] < x) i++;
			//while(less(a[i],x)) i++;
//#error less ����Ȃ��� greater �ł��Aqsort�Ɠ����d�l�ɂ��邽�ߥ���B�X�}��
			//while( less(get_offset(a,i * width),x, width) < 0){
			while( less(get_offset(a,i * width),x, width) ){
				i++;
			}

			/*void *p = get_offset(a,i * width);
			BYTE *ma = (BYTE *)get_offset(a,n * width);
			dkcmNOT_ASSERT(p < a);
			dkcmNOT_ASSERT(p > ((BYTE *)a) + n * width);
			dkcmNOT_ASSERT((ma - (BYTE *)p) % width != 0);
			while(less(p,x) > 0){
				i++;
				p = get_offset(a,i * width);
				ma = (BYTE *)get_offset(a,n * width);
				dkcmNOT_ASSERT(p < a);
				dkcmNOT_ASSERT(p > ma - width);
				dkcmNOT_ASSERT((ma - p) % width != 0);
			}*/

			//while (x < a[j]) j--;
			//while(less(x,a[j]) j--;
			//while( less(x,get_offset(a,j * width), width) < 0){
			while( less(x,get_offset(a,j * width), width) ){
				j--;
			}

			if (i > j){
				break;
			}
			
			//t = a[i];  a[i] = a[j];  a[j] = t;
			//dkcSwap(&a[i],&a[j],width);
			dkcSwap(
				get_offset(a,i * width),
				get_offset(a,j * width),
				width
				);
			
			i++;
			j--;
		}
		if (j < k){
			left  = i;
		}
		if (k < i){
			right = j;
		}
	}

	//�Q�b�g����
	{
		printf("%d",*(int *)get_offset(a,k * width));
		//memcpy(,	(const void *)get_offset(a,k * width),width);
		//memcpy(dest,(const void *)get_offset(a,k * width),width);
		memcpy(dest,x,width);
	}
	free(x);
	free(a);
	//free(t);
	return edk_SUCCEEDED;
	//return a[k];
}
#endif

DKC_INLINE int dkcRotateShiftRightMemoryLogic(void *d,size_t size,size_t n,void *workbuff,size_t worksize)
{
	BYTE *dest = (BYTE *)d;
	size_t temp;

	if(n > worksize){
		return edk_FAILED;
	}
	for(;;){
		if(n < size){
			break;
		}
		n -= size;
	}
	temp = size - n;
	memcpy(workbuff,dest + temp,n);
	memmove(dest + n,dest,temp);
	memcpy(dest,workbuff,n);
	return edk_SUCCEEDED;

}

int WINAPI dkcRotateShiftRightMemory(void *d,size_t size,size_t num){
	//size_t i = 0;
	void *pd = NULL;
	int r = edk_FAILED;

	pd = malloc(num);
	if(NULL==pd){
		return edk_OutOfMemory;
	}
	if(DKUTIL_SUCCEEDED(dkcRotateShiftRightMemoryLogic(d,size,num,pd,num))){
		r = edk_SUCCEEDED;
	}

	free(pd);
	
	return r;

}