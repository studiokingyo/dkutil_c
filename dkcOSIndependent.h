/*!
@file dkcOSIndependent.h
@brief �Ɨ��n
@note
�ڂ���������dkc.h�ɏ����Ă��܂��B

*/
#ifndef DKUTIL_C_OS_INDEPENDENT_H
#define DKUTIL_C_OS_INDEPENDENT_H



#include "dkc_misc.h"



///@return FALSE�Ȃ�Native�ȃt�@�C���p�X�ł͂Ȃ��B
DKC_EXTERN BOOL WINAPI dkcIsNativePathString(const char *s,size_t size);



/*
@param OldSize[in] ���̃������̈�̃T�C�Y
@param ExpandSize[in] ����Ȃ��������̈�̃T�C�Y
@return �����A���z�n�̃T�C�Y�H�J�i�H
@note realloc���鎞�̍œK�ȃ������T�C�Y���v�Z����B
(���������͂��傭���傭�ς��܂��B
*/

DKC_EXTERN  size_t dkcReallocateSizeFunction(size_t OldSize,size_t ExpandSize);

///malloc()�n �֐��̌^
typedef void *( WINAPIV *DKC_ALLOC_FUNC_TYPE)(size_t);
///free()�n �֐��̌^
typedef void (WINAPIV *DKC_FREE_FUNC_TYPE)(void *);

DKC_EXTERN void *WINAPIV dkcMallocAdapter(size_t);

DKC_EXTERN void WINAPIV dkcFreeAdapter(void *);

DKC_EXTERN  void* WINAPIV dkcAllocate(size_t size);

DKC_EXTERN  int	WINAPIV dkcFree(void **p);

DKC_EXTERN void *WINAPIV dkcAllocateFast(size_t size);

DKC_EXTERN void *WINAPIV dkcAllocateFill(size_t size,BYTE fill);

DKC_EXTERN  int WINAPI dkcReallocate(
	void **NewPtr,size_t NewSize,void **OldPtr
);

/*!
@param your_realloc[in] realloc����֐�
@param NewPtr[out] �V������������ԃA�h���X
@param NewSize[in] �V������������ԃA�h���X�̃T�C�Y�B
@param OldPtr[in][out] �Â���������ԃA�h���X�B�֐�����������ƁAOldPtr�͖����ɂȂ�
@param OldSize[in] OldPtr�̃T�C�Y
*/
///@return edk_SUCCEEDED�������琬��
DKC_EXTERN int WINAPI dkcReallocateEx(
	DKC_REALLOC_F_TYPE your_realloc,void **NewPtr,size_t NewSize,
	void **OldPtr
);

/*!
@param your_realloc[in] realloc����֐�
@param NewPtr[out] �V������������ԃA�h���X
@param OldPtr[in][out] �Â���������ԃA�h���X�B�֐�����������ƁAOldPtr�͖����ɂȂ�
@param OldSize[in] OldPtr�̃T�C�Y
@param reallocated_size[out] �m�ۂ����T�C�Y
@note
�������T�C�Y�g����p
*/
DKC_EXTERN int WINAPI dkcReallocateAutoExpand(
	DKC_REALLOC_F_TYPE your_realloc,void **NewPtr,
	void **OldPtr,size_t OldSize,size_t *reallocated_size
);

DKC_EXTERN int WINAPI dkcGetMemorySize(uint64 *pTotalMemory,uint64 *pFreeMemory);

/*!
���Z��Swap������BSWAP_NUM()�}�N�����Q�ƁB
*/
DKC_EXTERN void WINAPI 
	dkcSwap(void *p1,void *p2,size_t size);
/*!
�ǂ�ȕ��@�ł���������Ƃ���������Swap������B
@note
size���̃������𓮓I�m�ۂ��ăX���b�v���܂��B
*/

DKC_EXTERN BOOL WINAPI dkcSwapFast(void *p1,void *p2,size_t size);

/*!
@param p1[in] ULONGLONG *�̃o�b�t�@�ւ̃|�C���^
@param p2[in] ULONGLONG *�̃o�b�t�@�ւ̃|�C���^
@param size[in] p1,p2�o�b�t�@�̃T�C�Y
@note
p1,p2����ULONGLONG *�ɋ����^�L���X�g���Ă��������B
*/
DKC_EXTERN void WINAPI dkcSwap64(ULONGLONG *p1,ULONGLONG *p2,size_t size);


//#define dkcSwap(p1_,p2_,size) dkcSwap64(p1_,p2_,size)
/*
#define dkcSwapFast(p1,p2,size) dkcSwap64(p1,p2,size)
*/


///fopen�̎��̑��������L���Ȓl���ǂ������ׂ�B
DKC_EXTERN BOOL dkcIs_foepn_mode(const char *s);
///fopen�̃��b�p�[
DKC_EXTERN FILE * WINAPI dkcFOpen(const char *filename,const char *mode);
///fclose�̃��b�p�[
DKC_EXTERN int WINAPI dkcFClose(FILE **ptr);

///fread�̑S���ӔC�����ēǂݍ��݂܂��o�[�W�����B(�ǂݍ��߂Ȃ��ꍇ��EOF��error)
DKC_EXTERN size_t WINAPI dkcFReadAll(void *,size_t size,FILE *fp);


///@see dkcFReadAll() dkcFReadAll��fwrite��
DKC_EXTERN size_t WINAPI dkcFWriteAll(const void *,size_t size,FILE *fp);

///dkcFileSize()�𕁒i�͂��g�����������B�G���[���̎��͓���͖���`�ł��B
DKC_EXTERN size_t WINAPI dkcFSize(FILE *fp);


///fopen�̃t�@�C���֐����g���ăo�C�i���Z�[�u���s���B@note �ގ�: dkcMemoryToFile()
DKC_EXTERN int WINAPI dkcSaveBinary(const void *data,size_t size,const char *fname);
///�o�C�i���f�[�^���������ɓǂݍ��ށBdkcSaveBinary()�Ƒ� @note �ގ�: dkcFileToMemory()
DKC_EXTERN int WINAPI dkcLoadBinary(void *data,size_t size,const char *fname,size_t *readsize);
///fopen�Ńe�L�X�g�Z�[�u���s��
DKC_EXTERN int WINAPI dkcSaveText(const char *text,size_t length,const char *fname);
///�e�L�X�g�f�[�^���������ɓǂݍ��ށBdkcSaveText()�Ƒ�
DKC_EXTERN int WINAPI dkcLoadText(char *text,size_t length,const char *fname,size_t *readsize);
///��t�@�C�������B
DKC_EXTERN BOOL WINAPI dkcCreateEmptyFile(const char *filename);
///�G���[���O(�t�@�C��)�������ɏ���������A@note ���S�ɖ�������킯�ł͂Ȃ� @param filename[in] �t�@�C���̖��O
DKC_EXTERN int WINAPI dkcInitFileFast( const char *filename );
///���O��f�� @param filename[in] �t�@�C���l�[�� @param str[in] �f�������� @note �o�͑��x���x���̂ő��p�͏��サ�Ȃ�
DKC_EXTERN  int WINAPI dkcAddLogFile( const char *filename,const char *str , ... );

DKC_EXTERN void WINAPI dkcTwoDWORDToULONGLONG(ULONGLONG *dest,DWORD high,DWORD low);

DKC_EXTERN void WINAPI dkcULONGLONGToTwoDWORD(DWORD *dhigh,DWORD *dlow,ULONGLONG src);

DKC_EXTERN void WINAPI dkcLONGLONGToTwoLONG(LONG *high,LONG *low,LONGLONG src);

DKC_EXTERN void WINAPI dkcTwoLONGToLONGLONG(LONGLONG *dest,LONG high,LONG low);


///@note �G���f�B�A�����ɂ���ď�肭�o���邩������Ȃ�
DKC_INLINE void dkcTwoLONGToULONGLONG(ULONGLONG *dest,LONG high,LONG low){
	ULONG *p = (ULONG *)&low,*p2;
	*dest = *p;
	p = (ULONG *)(dest + 1);
	p2 = (ULONG *)&high;
	*p = *p2;
}

DKC_EXTERN int WINAPI dkcOutputDebugString(const char *str,...);
/*!
@return almost TRUE(1)
*/
DKC_EXTERN int WINAPI dkcErrorMessage(const char *expression,
										 const char *filename,size_t line,const char *message,...);



#if defined(DEBUG) || defined(DKUTIL_DEBUG)
#	define dkcmNOT_ASSERT(ex) ( (ex) && dkcErrorMessage(#ex,__FILE__,__LINE__,NULL) )
#	define dkcmASSERT(ex) ( (ex) || dkcErrorMessage(#ex,__FILE__,__LINE__,NULL) )
#	define dkcmNOT_ASSERT_MESSAGE(ex,mes) ( (ex) && dkcErrorMessage(#ex,__FILE__,__LINE__,mes) )
#	define dkcmASSERT_MESSAGE(ex,mes) ( (ex) || dkcErrorMessage(#ex,__FILE__,__LINE__,mes) )
#else
#	define dkcmNOT_ASSERT_MESSAGE(ex,mes) ((void)0)
#	define dkcmASSERT_MESSAGE(ex,mes) ((void)0)
#	define dkcmNOT_ASSERT(ex) ((void)0)
#	define dkcmASSERT(ex) ((void)0)
#endif

#	define dkcmFORCE_NOT_ASSERT(ex) ( (ex) && dkcErrorMessage(#ex,__FILE__,__LINE__,NULL) )
#	define dkcmFORCE_ASSERT(ex) ( (ex) || dkcErrorMessage(#ex,__FILE__,__LINE__,NULL) )
#	define dkcmFORCE_NOT_ASSERT_MESSAGE(ex,mes) ( (ex) && dkcErrorMessage(#ex,__FILE__,__LINE__,mes) )
#	define dkcmFORCE_ASSERT_MESSAGE(ex,mes) ( (ex) || dkcErrorMessage(#ex,__FILE__,__LINE__,mes) )

///@return path separator�̓�����������(1byte����path separator�̕����萔�������Ă���)���Q�b�g										 
DKC_EXTERN const char *WINAPI dkcGetPathSep();

/**
@param isForceExit[in] TRUE���w�肷���dkutil_c�̏�����ƈ�����ꍇ�A�����I������B
@return edk_Result
*/
DKC_EXTERN int dkcDynamicCheckEnvironment(BOOL isForceExit);
///@param flag[in] TRUE�Ń`�F�b�N��L��
DKC_EXTERN void WINAPI dkcCheckMemoryLeak(BOOL flag);
///@return MMX���g����Ȃ�TRUE
DKC_EXTERN BOOL WINAPI dkcIsMMX();



#define dkcmREVERSE_ENDIAN16(x) ( (USHORT)( (x >> 8) | (x << 8) ) )

#define dkcmREVERSE_ENDIAN32(x) \
	( (x << 24) | ( (x & 0x0000ff00) << 8) | ( (x & 0x00ff0000) >> 8 ) | (x >> 24) )

#ifdef WIN32
#	define dkcmREVERSE_ENDIAN64(x) (\
		(ULONGLONG)dkcReverseEndian32( (DWORD) (x & 0x00000000ffffffff) ) << 32 )\
		| \
		dkcReverseEndian32((DWORD)(x >> 32)\
	)
#else
#	define dkcmREVERSE_ENDIAN64(x) dkcReverseEndian64(x)

#endif
///@note  General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.���
DKC_EXTERN ULONG dkcReverseEndian32(ULONG x);
///@note  General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.���
DKC_EXTERN ULONGLONG dkcReverseEndian64(ULONGLONG x);
///endian change 16 bit version
DKC_EXTERN USHORT dkcReverseEndian16(USHORT x);

///@return little endian �Ȃ�TRUE
DKC_EXTERN BOOL dkcIsLittleEndian();
///@return big endian�Ȃ�TRUE
#define dkcIsBigEndian() (!dkcIsLittleEndian())

enum edkcByteOrder{
	///big-endian motorola
	edkcByteOrder_ABCD = 0,
	///little-endian intel
	edkcByteOrder_DCBA,
	///BADC - vax i386?
	edkcByteOrder_BADC,
	edkcByteOrder_Unknown = 255,
};
typedef union dkc_byte_order_check_union{
	uint32 x;
	uint8 abcd[4];
}DKC_BYTE_ORDER_CHECK_UNION;
///@return enum edkcByteOrder
DKC_EXTERN int WINAPI dkcGetByteOrder();

///unsigned long�^�̓�̐������v���X������I�[�o�[�t���[���邩�ǂ������`�F�b�N����B
DKC_EXTERN BOOL dkcCheckOverflowULONG(ULONG a1,ULONG a2);
DKC_EXTERN BOOL dkcCheckOverflowULONGLONG(ULONGLONG a1,ULONGLONG a2);

#define dkcCheckOverflow32(a,b) dkcCheckOverflowULONG(a,b)
#define dkcCheckOverflow64(a,b) dkcCheckOverflowULONGLONG(a,b)


typedef int (WINAPIV *DKC_COMPARE_TYPE)(const void *, const void *, size_t);

/*!
@param dest[out] ���Ԗڂ̃f�[�^�����邽�߂̃o�b�t�@ width�o�C�g�̃o�b�t�@���K�v
@param a_src[in] �������ւ̃|�C���^
@param n[in] ���̗v�f�����邩
@param k[in] ���ڂ̗v�f�̃I�t�Z�b�g��m�肽����
@param width[in] �I�t�Z�b�g
@param less[in] LESS�̃R���y�A�֐��ւ̃|�C���^
@return edk_SUCCEEDED�Ő���
�I�t�Z�b�g�l�� (BYTE *)a_src + width * offset �ł���B
@note
���Ȃ݂�a_src�ɓn�����|�C���^�͓����Ń������m�ۂ���A�R�s�[����܂��B
@code
int greater_comp(const void *v1,const void *v2){
	return *( ( int * ) v2 ) - *( ( int * ) v1 );
}
void test()
{
	int arr[]={0,1,2,3};
	int dest;
	dkcSelect((void *)&dest,(const void *)arr,4,3,sizeof(int),greater_comp);
}
@endcode
*/
///���킢��Z���N�g(�I��)�A���S���Y��
DKC_EXTERN int WINAPI dkcSelect(void *dest,const void *a_src,size_t n,int k,size_t width ,DKC_COMPARE_TYPE comp);

/*!
�ő�l��I������
@see dkcSelect() �� ����k����������
*/
DKC_EXTERN int WINAPI dkcSelectMax(void *dest,const void *a, size_t n,size_t width ,DKC_COMPARE_TYPE comp);
/*!
�ŏ��l��I������ 
@see dkcSelect() �� ����k����������
*/
DKC_EXTERN int WINAPI dkcSelectMin(void *dest,const void *a, size_t n,size_t width ,DKC_COMPARE_TYPE comp);


DKC_INLINE int dkcRotateShiftRightMemoryLogic(void *dest,size_t size,size_t n,void *workbuff,size_t worksize);


DKC_EXTERN int WINAPI dkcRotateShiftRightMemory(void *dest,size_t size,size_t n);

/*
DKC_EXTERN void dkcSetAllocateMode(int flag);

DKC_EXTERN void *dkcDebugAlloc
*/



#if DKC_HAVE_INLINE==1

DKC_FORCE_INLINE void *WINAPI dkcSecureFillMemory (void * Destination,size_t Length,uint8 Fill){
	volatile uint8 *vptr = (volatile uint8 *)Destination;
	while(Length){
		*vptr = Fill;
		vptr++;
		Length--;
	}
	return Destination;
}

DKC_FORCE_INLINE void *dkcSecureZeroMemory(void  *ptr, size_t cnt)
{
	/*volatile char *vptr = (volatile char *)ptr;
	while (cnt) {
		*vptr = 0;
		vptr++;
		cnt--;
	}
	return ptr;*/
	return dkcSecureFillMemory(ptr,cnt,0);
}


///�ǂݍ��߂Ȃ����iFALSE���j��feof��ferror()���g���Ă��������B
DKC_INLINE BOOL dkcFReadAllCheck(void *buff,size_t size,FILE *fp)
{
	if(size != dkcFReadAll(buff,size,fp))
		return FALSE;
	return TRUE;
}
DKC_INLINE BOOL dkcFWriteAllCheck(const void *buff,size_t size,FILE *fp)
{
	if(size != dkcFWriteAll(buff,size,fp))
		return FALSE;
	return TRUE;
}



///�Ó��ȃe���|�����o�b�t�@�̃T�C�Y��Ԃ�
DKC_INLINE size_t dkcGetProperTemporaryBufferSize(){
	uint64 total_size,free_size;
	if(DKUTIL_FAILED(dkcGetMemorySize(&total_size,&free_size)))
	{
		return 1024 * 64;
	}

	/*if(free_size < 1024 * 1024)
	{
		return 1024;
	}
	//freespace��������肠������
	if(free_size > total_size / 2)
	{
		return total_size / 0x4000;//32768
	}*/
	total_size = free_size / 0x1000;//4096
	if(total_size > 1024 * 1024){
		total_size = 1024 * 1024;
	}
	else if(0==total_size) 
	{
		total_size = free_size % 0x1000;
		if(0==total_size){
			return 1;
		}
	}
	return (size_t)total_size;
}
#else

// WINAPI ...

#endif





#ifdef __cplusplus

namespace dkutil{


/*!

@param data[in] �t�@�C���̃f�[�^�ւ̃|�C���^(void *)�ɃL���X�g���ĂԂ�����ł��������B
@param size[in] �t�@�C���̃f�[�^�̃T�C�Y�@(�������ރT�C�Y)
@param fname[in] �t�@�C���̖��O
@param mode[in] �f�t�H���g��="wb" �I�[�v�����[�h �ڂ�����MSDN��fopen�֐�������I
@return true=���� false=���s

*/
///fopen�̃t�@�C���֐����g���ăo�C�i���Z�[�u���s���B
inline bool SaveBinary(const void *data,size_t size,const char *fname,const char *mode="wb"){//="wb"
	FILE *fp;
	fp = fopen( fname , mode ) ;//wb
	if(fp==NULL) return false;
	//fwrite( data , size  , 1 , fp ) ;
	dkcFWriteAll(data,size,fp);
	//fwrite( data , sizeof(size)  , 1 , fp ) ;
	fclose( fp ) ;
	return true;
	//return dkcSaveBinary(data,size,fname);
}

/*!
@param data[out] �t�@�C���̃f�[�^�̃o�b�t�@ (void *)�ɃL���X�g���ĂԂ�����ł��������B
@param size[in] �t�@�C���̃f�[�^�̃o�b�t�@�T�C�Y�@(�ǂݍ��ރT�C�Y)
@param fname[in] �t�@�C���̖��O
@param mode[in] �f�t�H���g��="rb" �I�[�v�����[�h �ڂ�����MSDN��fopen�֐�������I
@return true=���� false=���s
*/
///�o�C�i���f�[�^���������ɓǂݍ��ށBSaveBinary()�Ƒ�
inline bool LoadBinary(void *data,size_t size,const char *fname,const char *mode="rb" ){//="rb"
	FILE *fp ;
	fp = fopen( fname , mode ) ;//rb
	if(fp==NULL)return false;
	dkcFReadAll(data,size,fp);
	//fread( data ,  size , 1 , fp ) ;
	//fread( data , sizeof(size)  , 1 , fp ) ;
	fclose( fp ) ;
	return true;
}

inline bool SaveText(const char *text,size_t length,const char *fname,const char *mode="wt"){
	return SaveBinary(text,length,fname,mode);
}
inline bool LoadText(char *text,size_t length,const char *fname,const char *mode="rt"){
	return LoadBinary(text,length,fname,mode);
}



}//end of dkutil namespace


#else //for c lang




#endif //end of __cplusplus

#endif //end of include once