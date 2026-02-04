/**
@author d����
@file define.h
@brief C����̒�`(�Ǝ��g���A�����)
@since 2005/01/17
@note
<h2>stable release����Ă��Ȃ��@�\���g��DLL�Ɋւ���R���p�C���I�v�V����define</h2>
USE_DKC_DLL�@DKUTIL_CDLL_EXPORTS : DLL mode��dkutil_c���R���p�C�������ɕK�v�B
USE_DKC_DLL : DLL mode�Ń����N����Ƃ��ɕK�v�B

*/


#ifndef DKUTIL_CSTD_DEFINE_H
#define DKUTIL_CSTD_DEFINE_H

#include "auto_config.h"

//**********************************************************
//������ւ����32�r�b�g���ˑ�������O�O�G�G�G�G�G���ӂ��ĂˁO�O�G�G�G
//**********************************************************

//�萔���`����
///ULONGLONG�̕ϐ��̃}�b�N�X�l(�ő�l)
#define DKINGYO_ULONGLONG_MAX 18446744073709551615 
///LONGLONG�̕ϐ��̃}�b�N�X�l(�ő�l)
#define DKINGYO_LONGLONG_MAX 9223372036854775807 
///LONGLONG�̕ϐ��̃~���l(�ŏ��l)
#define DKINGYO_LONGLONG_MIN -9223372036854775808 

#ifndef dkcdUINT8_MAX
#	define dkcdUINT8_MAX 255
#endif
#ifndef dkcdUINT16_MAX
#	define dkcdUINT16_MAX 65535
#endif
#ifndef dkcdUINT32_MAX 
#	define dkcdUINT32_MAX 4294967295
#endif
#ifndef dkcdUINT64_MAX 
#	define dkcdUINT64_MAX DKINGYO_ULONGLONG_MAX
#endif

/*

(0xffffffff) unsigned long �̍ő�l 
INT_MAX 2147483647 (signed)  int �̍ő�l 
INT_MIN -2147483647-1 (signed)  int �̍ŏ��l 
LONG_MAX 2147483647 (signed)  long �̍ő�l 
LONG_MIN -2147483647-1 (signed)  long �̍ŏ��l 
*/

//**********************************************************
//�C���q�̒�`
//**********************************************************

#ifdef __cplusplus
#	define DKUTIL_EXTERN extern "C"
#else
///extern...
#	define DKUTIL_EXTERN extern
#endif


#ifndef DKC_INLINE
#define DKC_HAVE_INLINE 1
//inline
/*
#	if _MSC_VER >= 1100 && defined(_inline)
#		define DKC_INLINE _inline
#	elif defined(__inline)
#		define DKC_INLINE __inline
#	elif defined(inline)
#		define DKC_INLINE inline
*/
#	if _MSC_VER >= 1100
#		define DKC_INLINE __inline
#	elif USEINLINE
#		define DKC_INLINE __inline__
#	else
#		define DKC_INLINE
#		undef DKC_HAVE_INLINE
#		define DKC_HAVE_INLINE 0
#	endif

//forceinline
//#	if defined(__forceinline)
#	ifdef FORCEINLINE
#		define DKC_FORCE_INLINE FORCEINLINE
#	elif _MSC_VER >= 1100
#		define DKC_FORCE_INLINE __forceinline
#	else
#		define DKC_FORCE_INLINE DKC_INLINE
#	endif//end of FORCEINLINE
#endif//end of DKC_INLINE

/*
#ifdef _USRDLL
#	define USE_DKC_DLL
#endif
*/

#ifdef WIN32
#	define DKC_EXPORT __declspec(dllexport)
#	define DKC_IMPORT __declspec(dllimport)
#	ifdef USE_DKC_DLL
#		if defined(DKCDLL_EXPORTS) || defined(DKUTIL_CDLL_EXPORTS)
#			define DKC_EXTERN DKUTIL_EXTERN DKC_EXPORT
#		else
#			define DKC_EXTERN DKUTIL_EXTERN DKC_IMPORT
#		endif
#	else
#		ifdef _MSC_VER
#			define DKC_EXTERN DKUTIL_EXTERN DKC_INLINE
#		else
#			define DKC_EXTERN DKUTIL_EXTERN
#		endif
#	endif
#else
#	define DKC_EXPORT
#	define DKC_IMPORT
#	define DKC_EXTERN DKUTIL_EXTERN
#endif

#ifdef _DEBUG
#	ifndef DEBUG
#		define DEBUG
#	endif
#endif
#ifdef _NDEBUG
#	ifndef NDEBUG
#		define NDEBUG
#	endif
#endif

//#error "I don't know multithread define..."
#if defined(_MT) || defined(__MT__)//MultiThread
#	define dkcdMultiThread
#endif


#include "macro.h"




enum edkOS{//OS�̃o�[�W�������擾���邽�߂̗�
	enuWindows3_1 = 1,
	enuWindowsNT3_51,
	enuWindowsNT4_0,
	enuWindows95,
	enuWindows98,
	enuWindows2000,
	enuWindowsMe,
	enuWindowsXP,
	///WindowsXP??�̃T�[�o�[
	enuWindows_NET,

};


enum edkOSExtension{
	///Windows95 OS R2
	edkOSE_OSR2 = 1,
	///Windows98 SE
	edkOSE_edkSE,
	///WindowsXP HomeEdition
	edkOSE_HomeEditon,
	///WindowsXP Professional Edition �܂��́ANT�n��Professional
	edkOSE_Professional,
	///Windows .NET��DataCenter Server
	edkOSE_DataCenter_Server,
	///NT�n��Advanced�@�I
	edkOSE_Advanced_Server,
	edkOSE_Enterprise_Server,
	edkOSE_Web_Server,
	///NT�n�̕��ʂ̃T�[�o�[
	edkOSE_Server,

};

///�ϐ��̃^�C�v�̃C�i���R
enum edkVariableType{
	///�킩��Ȃ��B
	edkVariableUnknown = 0,//������폜���Ȃ����ƁB���̂O�̂܂܂ɂ��Ă������ƁB
	///int
	edkInt,
	///UINT
	edkUInt,
	///LONGLONG
	edkLongLong,
	///ULONGLONG
	edkULongLong,
	///double
	edkDouble,
	///std::string
	edkString,
};



///������̒����̎��ʎq�B
enum{
	///INT�ȓ��̐�
	enuSTRLENGTH_INT_DU = 1,//�����͐�΂ɂP�ł�����ˁI�I����������_���I
	///UINT�ȓ��̐�
	enuSTRLENGTH_UINT_DU,
	///LONGLONG(__int64)�ȓ��̐�
	enuSTRLENGTH_LONGLONG_DU,
	///ULONGLONG( unsigned __int64)�ȓ��̐�
	enuSTRLENGTH_ULONGLONG_DU,
	///������(float�Ȃ�Ă���˂��i��))�ȓ��̐��l�H
	enuSTRLENGTH_DOUBLE,
};

///�܂��A�ς��\��������̂ŁA���܂�g��Ȃ��ŉ������O�O�G
enum edkFileSignature{
	edkfALL = 0,
	//�e�L�X�g�n
	edkfTXT,
	edkfHTML,
	edkfXML,
	//���[�r�[�n
	edkfAVI,
	edkfMPEG,
	edkfMOV,
	//���n
	edkfWAV,
	edkfMIDI,
	edkfMP3,
	edkfOGG,
	//�摜�n
	edkfPNG,
	edkfBMP,
	edkfTIFF,
	edkfJPEG,
	edkfGIF,
	//���k�n
	edkfZIP,
	edkfLZH,
	edkfGZ,
	edkfGCA,
	edkfDGC,
	edkfRAR,
	//�Ō�̈�B
	edkfSENTINEL,
};

/**
���񏈗��̃f�t�H���gID
@attention
����ID��t�������Ă�����
*/
#if 0
//old
enum edk_ProcessSignatures{
	edk_NONE_SIGNATURE = 0,

//���k�n
	edk_LZSS_SIGNATURE = 10,
	edk_ZLIB_SIGNATURE,
	edk_RLE_PACKBITS_SIGNATURE,
	edk_BLOCKSORT_SIGNATURE,

//�Í����n
	edk_BLOWFISH_SIGNATURE = 100,
	edk_ARCFOUR_SIGNATURE,
//���̑�
	//200
	//300
	//edk_ARRAY_PROCESS_ID_END = USHRT_MAX * 2,
};
#else
//new
enum edk_ProcessSignatures{
	edk_NONE_SIGNATURE = 0,

//���k�n
	edk_LZSS_SIGNATURE = 0x100,
	edk_ZLIB_SIGNATURE = 0x200,
	edk_RLE_PACKBITS_SIGNATURE = 0x400,
	edk_BLOCKSORT_SIGNATURE = 0x800,
	edk_LZW_SIGNATURE = 0x1000,

//�Í����n
	edk_BLOWFISH_SIGNATURE =	0x10000,
	edk_ARCFOUR_SIGNATURE =		0x20000,
	edk_HC256_SIGNATURE =			0x40000,
	edk_SNOW20_SIGNATURE =		0x80000,
	edk_SNOW2_SIGNATURE = edk_SNOW20_SIGNATURE,
	edk_VERNAM_SIGNATURE =		0x100000,
	edk_RIJNDAEL_SIGNATURE =	0x200000,
	edk_CAMELLIA_SIGNATURE =	0x400000,

//���̑�
	//200
	//300
	//edk_ARRAY_PROCESS_ID_END = USHRT_MAX * 2,
};

#endif

/*
enum edk_{
	///�������Ȃ�
	edkc_Plane = 0,
//���k�`
	edkc_LZW = 0x1,
	edkc_LZSS =0x2,
	edkc_RLE	=0x4,
	edkc_BLOCKSORTED_RLE = 0x8
//�Í����n
	edkc_RIJNDAEL = 0x10000,
	edkc_Arcfour = 0x20000,
	edkc_Blowfish = 0x40000,
	edkc_HC256 = 0x80000,
	edkc_SNOW20 = 0x100000,
	edkc_VERNAM = 0x200000,
};
*/

/**
dkcSJISFileSystem.h�̃t�@�C������Ɋւ��� enum
*/
enum edk_SJISFileSystem{
	edkcFileRewrite = 1,
};

/**
�Z�L���A�n�b�V���n�̃A���S���Y���񋓒�`
�A���S���Y����unsigned int�^�̑I�������Ɏg��
@see http://d.hatena.ne.jp/studiokingyo/00010023

*/
enum edk_SecureHash{

	edkcSH_MD2 = 0,
	edkcSH_MD4,
	edkcSH_MD5,
	
	edkcSH_SHA0 = 16,
	edkcSH_SHA1 ,
	edkcSH_SHA160 = edkcSH_SHA1,
	edkcSH_SHA256,
	edkcSH_SHA384,
	edkcSH_SHA512,

	edkcSH_SHA3_224 = 48,
	edkcSH_SHA3_256,
	edkcSH_SHA3_384,
	edkcSH_SHA3_512,

	edkcSH_BLAKE224 = 64,
	edkcSH_BLAKE256,
	edkcSH_BLAKE384,
	edkcSH_BLAKE512,

	edkcSH_BLAKE2s = 80,
	edkcSH_BLAKE2b,

	edkcSH_BLAKE3 = 96,

	edkcSH_RIPEMD128 = 32,
	edkcSH_RIPEMD160,
	edkcSH_RIPEMD256,
	edkcSH_RIPEMD320,

};

/*!
�����łőΉ�����ł��낤�G���[ID��(�������A�Ή��͖��襥�
@note
edk��Enum of D Kingyo�̗� (��)
*/
enum edkResult{
	///�Ƃ������A�G���[�炵���B
	edk_FAILED = -1,
	///�����@�����I�I
	edk_SUCCEEDED = 0,
	///������Ȃ��B
	edk_Not_Found,
	///�t�@�C����������Ȃ��B
	edk_FileNotFound,
	///�I������Ȃ������B
	edk_Not_Selected,
	///�����ɓK���Ȃ��l�������Ă����B(�����G���[
	edk_ArgumentException,
	///Parsing�ł��Ȃ������B
	edk_ParsingError,

	/*!
	�����I�ɃG���[���N�������B<br>
	(�܂��A���̊֐����g���������o���Ă��Ȃ��Ƃ�,
	�N���肦�Ȃ��G���[���N�������Ƃ�,
	�Ăяo���Ă͂����Ȃ��̂ɌĂяo�����Ƃ��B
	*/
	edk_LogicError,
	///�o�b�t�@���͂ݏo���Ă��܂��̂ŏ����������ł��Ȃ������B
	edk_BufferOverFlow,
	///�o�̓o�b�t�@�[������Ȃ��Ȃ����B
	edk_OutputBufferWasLost,
	///���̓o�b�t�@�[������Ȃ��Ȃ����B
	edk_InputBufferWasLost,
	//�t�@�C���̃V�O�l�`��������Ȃ�
	//edk_FileSignatureException,
	///�V�O�l�`��������Ȃ�
	edk_SignatureException,
	///�G���[�ł͂Ȃ����A�����ȏ����͂���Ȃ������B�Ӑ}�Ƃ������ʂ͓����Ȃ���������Ȃ��B
	edk_Not_Satisfactory,
	///���̊֐��̏����͒�`����Ă��Ȃ�
	edk_Not_Defined_Function,
	///������������Ȃ�����
	edk_OutOfMemory,
	///���̊֐��ŏ�������̂͂����I���
	edk_EndProcess,
	///�ʂ̌��ʂ������B
	edk_LogicException,
	///�������鉿�l������
	edk_NoValueToProcess,
	//�t�@�C���̓��e���̂�ω�������ꂽ�B
	edk_FileCheated_Change,
	///�t�@�C���̓��e�ɖ��͖������A�璷�ȃf�[�^�������t���Ă���B
	edk_FileCheated_Addition,
	///�ϐ��̃I�[�o�[�t���[
	edk_VariableOverFlow,
	///�ϐ��̃A���_�[�t���[
	edk_VariableUnderFlow,
	///enum edkResult �̍Ō�̒l
	edk_ResultDefinitionEnd = 32767,
	/**
	enum edkResult�̖߂�l�̎d�l�ƈꏏ�Ƀ��[�U�[���Ǝ���`����߂�l���g���Ƃ��Ɉȉ��̂悤�Ɏg�p����
	@code
	enum edkResultEx{
		edk_HackedValue = edk_UserDefinitionBegin,
		edk_UnhackedValue,
		edk_Kracked_Network,
		edk_Unkracked_Network,
	};
	@endcode
	*/
	edk_UserDefinitionBegin,//
};

#define DKUTIL_SUCCEEDED(s) (s==edk_SUCCEEDED)
#define DKUTIL_FAILED(s) (s)//(s != edk_SUCCEEDED)
#define DKUTIL_FAILED_BOOL(s) (s != edk_SUCCEEDED)
#define DKUTIL_SUCCEEDED_BOOL(s) (s==edk_SUCCEEDED)

#define dkcmFAILED_CHECK_RETURN(t,a) \
	t = a;\
	if(DKUTIL_FAILED(t)){ return t;}

#include "os.h"

#endif //end of include once
