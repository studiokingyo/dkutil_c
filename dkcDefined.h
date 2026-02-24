
/*!
@author d����
@since 2003/9/13
@brief ��`�S
@file dkcDefined.h
@note
C language only

C++�ɂ͂���Ȋ����̒�`��������̂� ios_base ���Ă��̂��ŋ߂��鎖��m��B
*/


#ifndef DKUTIL_C_DEFINED_H
#define DKUTIL_C_DEFINED_H


#include <stdio.h>
#include <stddef.h>
#include "dkutil_cstd/define.h"

//**********************************************************
//���ʂ̒�`
//**********************************************************

///8bit�Ɏ��܂�f�� (unsigned)
#define dkcdUNSIGNED_8BIT_PRIME_NUM 251 
///8bit�Ɏ��܂�f�� (signed)
#define dkcdSIGNED_8BIT_PRIME_NUM 127

///16bit�Ɏ��܂�f�� (unsigned)
#define dkcdUNSIGNED_16BIT_PRIME_NUM 65521

#define dkcdSIGNED_16BIT_PRIME_NUM 32749

///24bit�Ɏ��܂�f�� (unsigned)
#define dkcdUNSIGNED_24BIT_PRIME_NUM 16777213

///64bit�Ɏ��܂�ő�l (unsigned)
#define dkcd64BIT_MAX DKINGYO_ULONGLONG_MAX
///64bit�Ɏ��܂�ŏ��l (signed)
#define dkcd64BIT_MIN DKINGYO_LONGLONG_MIN

///�����o�֐�����`����Ă��Ȃ����ɕԂ��ϐ�
#define DKINGYO_NOT_MOUNTED -100
///DKINGYO_NOT_MOUNTED�Ɠ���
#define DKINGYO_NO_DEFINED_FUNCTION DKINGYO_NOT_MOUNTED





//**********************************************************
//���ʂ��邽�߂̒�`
//**********************************************************




///�摜�t�@�C������͂��鎞�Ɏg���O�O
enum{
	enuDKINGYO_BMPFILEHEADER_ADR = 1,
	enuDKINGYO_BMPCOREHEADER_ADR,
	enuDKINGYO_BMPINFOHEADER_ADR,
	enuDKINGYO_BMPPALLETE_ADR,
	enuDKINGYO_BMPGRAPH_ADR,
	///isBmpFile()�ł�flag�ɂԂ����ގ��ʎq Bitmap�t�@�C�����ǂ������m�ȃ`�F�b�N���s���B
	enuDKINGYO_ISBMPFILE_EXACT_CHECK,
	///isBmpFile()�ł�flag�ɂԂ����ގ��ʎq �U���r�b�g�}�b�v���ǂ������ׂ�B
	enuDKINGYO_ISBMPFILE_CAMOUFLAGE_CHECK,
};


//**********************************************************
//�f�t�H���g���̐ݒ�p�萔�̒�`
//**********************************************************
enum{
	///�g���^�����̃o�b�t�@�̒ǉ��T�C�Y���w�肷��
	enuExtendVaListLength = 256,
	///�R���e�i�n�̃e���v���[�g�N���X�łǂ̂��炢�̃T�C�Y�i�܂��͐��j���m�ۂ��邩��ݒ肷��
	enuDefaultExpandSize = 100,
};

//**********************************************************
//�^��`
//**********************************************************

//���ʌ^
typedef void (*VOIDFUNC)(void); 
typedef void (*VOIDARGFUNC)(void *);
typedef void (*VOIDARGFUNCFLAG)(void *,int);


	
typedef struct dkc_Allocator{
	void (*malloc__)(size_t size);
	void (*free__)(void *);
	void (*realloc__)(void *,size_t);
	void (*zero_memory)(void *,size_t);
}DKC_ALLOCATOR;	
	

//**********************************************************
//dKingyoUtility2�Ɠ����@�}�N���Ƃ�
//**********************************************************


/// MessageBox��\��
#define MB(str) MessageBox(NULL,str,"Message",MB_OK)//MessageBox
/// MessageBox��\���i�^�C�g���w��\�o�[�W�����j
#define MB2(s,title) MessageBox(NULL,s,title,MB_OK);
///OutputDebugString���g�p
#define ODS(str) OutputDebugString(str)//OutputDebugString

/**
@see dkcMemoryStreamDump() dkcMemoryStreamWriteToMemory() dkcBitMemoryStreamLoadFromFile()

*/
enum{
	/**
	@warning ���ӁF�f�[�^�������o����Ă��Ȃ��̈���w�肷��̂Œ��ӁB
	�L���f�[�^�݂̂��w�肷��ꍇ��edkcStreamBufferToNowOffset���w�肷�邱�ƁB
	*/
	///�X�g���[���o�b�t�@�̂��ׂĂ��w��
	edkcStreamBufferAll = 0,
	///�X�g���[���o�b�t�@�̍ŏ��������w��
	edkcStreamBufferFirst = 1,
	///�X�g���[���o�b�t�@�̍Ōゾ�����w��
	//edkcStreamBufferLast = 2,
	///���w�肳��Ă���I�t�Z�b�g�܂ł��w��
	edkcStreamBufferToNowOffset = 3,

	edkcSeekCurrent = SEEK_CUR,
	///�Ō�̈ʒu����V�[�N
	edkcSeekEnd = SEEK_END,
	///�ŏ��̈ʒu����V�[�N
	edkcSeekSet = SEEK_SET,
};




#ifdef WIN32
#ifndef int32
#	define int32 int  		
#endif
#ifndef int16
#	define int16 short
#endif
#ifndef int8
#	define int8 char 		
#endif
#ifndef uint32
#	define uint32 unsigned int 		
#endif
#ifndef uint16 
#	define uint16 unsigned short 
#endif
#ifndef uint8
#	define uint8 unsigned char
#endif
#else
/*
 * Non-WIN32: Linux, macOS, WIN64 (when WIN32 is not defined)
 * Use <stdint.h> for exact-width types (C99 / GCC / Clang / MSVC 2010+).
 */
#if defined(__GNUC__) || defined(__clang__) \
 || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
 || defined(_MSC_VER)
#  include <stdint.h>
#  ifndef int8
#    define int8   int8_t
#  endif
#  ifndef int16
#    define int16  int16_t
#  endif
#  ifndef int32
#    define int32  int32_t
#  endif
#  ifndef uint8
#    define uint8  uint8_t
#  endif
#  ifndef uint16
#    define uint16 uint16_t
#  endif
#  ifndef uint32
#    define uint32 uint32_t
#  endif
#else
/* Last-resort fallback for unknown platforms (LP64 / ILP32 assumed) */
#  ifndef int8
#    define int8   signed char
#  endif
#  ifndef int16
#    define int16  short
#  endif
#  ifndef int32
#    define int32  int
#  endif
#  ifndef uint8
#    define uint8  unsigned char
#  endif
#  ifndef uint16
#    define uint16 unsigned short
#  endif
#  ifndef uint32
#    define uint32 unsigned int
#  endif
#endif /* compiler check */

#endif /* WIN32 */

#endif // end of include once
