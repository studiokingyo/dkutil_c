/*!
@file dcf_misc.h
@brief dKingyo Cryption File code chip miscellany
*/
#ifndef DKUTIL_C_DCF_MISC_H
#define DKUTIL_C_DCF_MISC_H

#include "dkcRijndael.h"
#include "dkcBlowfish.h"
#include "dkcVernam.h"

#include "dkcArcfour.h"
#include "dkcHC256.h"
#include "dkcSNOW20.h"

#include "dkcHMAC.h"
#include "dkcStream.h"


enum edkcDCF{

	edkcDCF01 = 1,
};

typedef void (WINAPI *DKC_DCF_INIT_F_TYPE)(void *p);

typedef int (WINAPI *DKC_DCF_WRITE_F_TYPE)(void *p,const void *data,size_t size,size_t *write_size);
typedef int (WINAPI *DKC_DCF_WRITE_FINAL_F_TYPE)(void *p);
typedef int (WINAPI *DKC_DCF_READ_F_TYPE)(void *p,void *buff,size_t size,size_t *readsize);
typedef int (WINAPI *DKC_DCF_READ_FINAL_F_TYPE)(void *p);

typedef int (WINAPI *DKC_DCF_FREE_F_TYPE)(void **pp);

typedef int (WINAPI *DKC_DCF_FILE_TO_DCF_F_TYPE)(const char *save_filename,const char *target_filename,
					 const void *key,size_t keysize);
typedef int (WINAPI *DKC_DCF_DCF_TO_FILE_F_TYPE)(const char *dest_filename,const char *target_filename,
					 const void *key,size_t keysize);


typedef struct dkc_dKingyoCryptionFile{
	void *CryptObj;
	DKC_STREAM *StreamObj;
	DKC_SHA512 *SHAObj;
	DKC_BUFFER *BufferObj;
	uint32 Version;
	uint64 Count;

	DKC_DCF_INIT_F_TYPE fpInit;

	DKC_DCF_WRITE_F_TYPE fpWrite;
	DKC_DCF_WRITE_FINAL_F_TYPE fpWriteFinal;
	DKC_DCF_READ_F_TYPE fpRead;
	DKC_DCF_READ_FINAL_F_TYPE fpReadFinal;

	DKC_DCF_FREE_F_TYPE fpFree;

	DKC_DCF_FILE_TO_DCF_F_TYPE fpFileToDCF;
	DKC_DCF_DCF_TO_FILE_F_TYPE fpDCFToFile;
	
}DKC_DCF,DKC_DKINGYO_CRYPTION_FILE;
/*
typedef void (WINAPI *DKC_DCF_INIT_F_TYPE)(DKC_DCF *p);

typedef int (WINAPI *DKC_DCF_WRITE_F_TYPE)(DKC_DCF *p,const void *data,size_t size,size_t *write_size);
typedef int (WINAPI *DKC_DCF_WRITE_FINAL_F_TYPE)(DKC_DCF *p);
typedef int (WINAPI *DKC_DCF_READ_F_TYPE)(DKC_DCF *p,void *buff,size_t size,size_t *readsize);
typedef int (WINAPI *DKC_DCF_READ_FINAL_F_TYPE)(DKC_DCF *p);

typedef int (WINAPI *DKC_DCF_FREE_F_TYPE)(DKC_DCF **pp);

typedef int (WINAPI *DKC_DCF_FILE_TO_DCF_F_TYPE)(const char *save_filename,const char *target_filename,
					 const void *key,size_t keysize);
typedef int (WINAPI *DKC_DCF_DCF_TO_FILE_F_TYPE)(const char *dest_filename,const char *target_filename,
					 const void *key,size_t keysize);
*/
#endif //end of include once