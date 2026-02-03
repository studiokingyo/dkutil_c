
/*!
@file dkcDCF.c
@brief Cryptograph algorithm
@since 2005/11/22
@note
更新履歴：
2005/12/19:実装をdkcDCF01.cに移した。このファイルには汎用的な処理を書くことにした。
2005/11/25:一応完成するが、読み込みがまだ上手く出来ていない。
2005/11/22:製作開始
*/
#define DKUTIL_C_DCF_C
#include "dkcDCF.h"
#include "dkcGenericFileSystem.h"
#include "dkcSJISFileSystem.h"
#include "dkcCryptograph.h"

int WINAPI dkcFreeDCF(DKC_DCF **pp){
	DKC_DCF *p = *pp;
	DKC_DCF_FREE_F_TYPE ff;
	if(NULL==pp || NULL==p) return edk_FAILED;
	ff = p->fpFree;
	return ff(pp);
}

int WINAPI dkcFileToDCF(uint32 option,const char *save_filename,const char *target_filename,
					 const void *key,size_t keysize)
{
	int r = edk_FAILED;
	switch(option){
		case edkcDCF01:
			r = dkcFileToDCF01(save_filename,target_filename,key,keysize);
			break;
	}
	return r;
}

int WINAPI dkcDCFToFile(uint32 option,const char *dest_filename,const char *target_filename,
					 const void *key,size_t keysize)
{
	int r = edk_FAILED;
	switch(option){
		case edkcDCF01:
			r = dkcDCF01ToFile(dest_filename,target_filename,key,keysize);
			break;
	}
	return r;
} 


BOOL WINAPI dkcDCFPasswordCheck(DKC_DCF *p,const char *filename,const void *password,size_t size)
{
	
	DKC_DCF_INIT_F_TYPE init;
	DKC_DCF_READ_F_TYPE read;
	DKC_DCF_READ_FINAL_F_TYPE readfinal;
	BOOL r = FALSE;
	uint8 tempbuff[1024];
	size_t readsize;
	if(NULL==p) goto Error;

	init = p->fpInit;
	read = p->fpRead;
	readfinal = p->fpReadFinal;

	init(p);
	do
	{
		r = read(p,tempbuff,sizeof(tempbuff),&readsize);
		
		if(DKUTIL_FAILED(r) && (r != edk_EndProcess))
		{
			goto Error;
		}

	}while(edk_EndProcess!=r);

	r = readfinal(p);
	if(DKUTIL_FAILED(r)) goto Error;

	r = TRUE;
Error:
	return r;
	
}

int WINAPI dkcDCFAllRead(DKC_DCF *p,void *buff,size_t size)
{

	DKC_DCF_INIT_F_TYPE init;
	DKC_DCF_READ_F_TYPE read;
	DKC_DCF_READ_FINAL_F_TYPE readfinal;
	size_t readsize;
	uint8 *pBuff = (uint8 *)buff;
	int r = edk_FAILED;
	size_t offset = 0;
	
	if(NULL==p) return edk_ArgumentException;
	init = p->fpInit;
	read = p->fpRead;
	readfinal = p->fpReadFinal;
	init(p);

	do
	{
		r = read(p,pBuff + offset,size - offset,&readsize);
		
		if(DKUTIL_FAILED(r) && (r != edk_EndProcess))
		{
			return r;
		}
		offset += readsize;
		if(size < offset){
			return edk_OutputBufferWasLost;
		}else if(size == offset){
			if(edk_EndProcess!=r){
				return edk_OutputBufferWasLost;
			}
			break;
		}
	}while(edk_EndProcess!=r);

	r = readfinal(p);
	if(DKUTIL_FAILED(r)) return r;

	r = edk_SUCCEEDED;

	return r;
}

int WINAPI dkcDCFFileSizeExpect(const char *filename,uint64 *pv){
	int r;
	DKC_FILE64 *p;
	size_t readsize;
	uint64 tv;
	if(dkcFileExist(filename)==FALSE)
		return edk_FileNotFound;

	p = dkcAllocFile64(edkcReadMode,filename);
	if(NULL==p) return edk_FAILED;

	//ファイルの先頭を取得。
	r = dkcFile64Seek(p,SHA512_BIN_BUFFER_SIZE,edkcSeekSet);
	if(DKUTIL_FAILED(r)){goto Error;}
	r = dkcFile64Read(p,pv,sizeof(*pv),&readsize);
	if(DKUTIL_FAILED(r)){goto Error;}
	if(readsize != sizeof(*pv)){
		r = edk_LogicException;
		goto Error;
	}

	//ファイルの最後までのサイズと比較
	r = dkcFile64Seek(p,0,edkcSeekEnd);
	if(DKUTIL_FAILED(r)){goto Error;}
	
	r = dkcFile64Tell(p,&tv);
	if(DKUTIL_FAILED(r)){goto Error;}
	
	if(*pv != tv - (SHA512_BIN_BUFFER_SIZE + readsize)){
		r = edk_LogicException;
		goto Error;
	}
	r = edk_SUCCEEDED;
Error:
	dkcFreeFile64(&p);
	return r;
}

