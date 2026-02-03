
/*!
@file dkcDCF01.c
@brief dKingyo Cryption File Version01
@since 2005/11/22
@note
更新履歴：
2005/12/19:テスト終了
2005/12/18:dkcDCF.cからコード片をこのファイルに移動。
2005/11/25:一応完成するが、読み込みがまだ上手く出来ていない。
2005/11/22:製作開始
*/
#define DKUTIL_C_DCF01_C
#include "dkcDCF01.h"
#include "dkcGenericFileSystem.h"
#include "dkcSJISFileSystem.h"
#include "dkcCryptograph.h"




DKC_INLINE DKC_DCF *WINAPI dkcAllocDCF_Version01_SNOW2(
	DKC_SNOW2 *attach_cp,DKC_STREAM *attach_sp)
{
	DKC_DCF *p;
	if(NULL==attach_cp || NULL==attach_sp) return NULL;
	if(!(attach_sp->mMode & edkcStreamInitFile64))
	{
		return NULL;
	}
	if(0!=dkcStreamTell(attach_sp)){
		return NULL;
	}
	p = dkcAllocate(sizeof(DKC_DCF));
	if(NULL==p) return NULL;
	
	p->CryptObj = attach_cp;
	p->StreamObj = attach_sp;
	p->SHAObj = dkcAllocSHA512();
	if(NULL==p->SHAObj){
		goto Error;
	}
	p->BufferObj = dkcAllocBuffer(NULL,1024 * 64);
	if(NULL==p->BufferObj){
		goto Error;
	}
	dkcDCF01Init(p);
	return p;
Error:
	dkcFreeSHA512(&(p->SHAObj));
	dkcFree(&p);
	return NULL;
}



DKC_DCF *WINAPI dkcAllocDCF01(const char *filename,uint32 stream_flags,const void *key,size_t keysize)
{
	DKC_DCF *p;
	DKC_STREAM *ps;
	DKC_SNOW2 *psnow2;

	ps = dkcAllocStreamFile64Type(edkcStreamWin32PromoteFlag,filename,stream_flags);
	if(NULL==ps) return NULL;
	psnow2 = dkcAllocSNOW2NoLimitKeyLength(key,keysize);
	if(NULL==psnow2) goto Error;
	
	p = dkcAllocDCF_Version01_SNOW2(psnow2,ps);
	if(NULL==p) goto Error;
	return p;
Error:
	//dkcFreeDCF01(&p);
	dkcFreeSNOW2(&psnow2);
	dkcFreeStream(&ps);
	return NULL;
}


int WINAPI dkcFreeDCF01(DKC_DCF **pp)
{
	DKC_DCF *p = *pp;
	if(NULL==pp || NULL==*pp) return edk_FAILED;
	dkcFreeBuffer(&(p->BufferObj));
	dkcFreeSHA512(&(p->SHAObj));
	return dkcFree(pp);
}

void WINAPI dkcDCF01Init(DKC_DCF *p)
{
	dkcSHA512Init(p->SHAObj);
	dkcStreamSeek(p->StreamObj,
		SHA512_BIN_BUFFER_SIZE + sizeof(uint64),edkcStreamSeekSet
	);
	p->Count = 0;


	p->fpInit = dkcDCF01Init;
	p->fpWrite = dkcDCF01Write;
	p->fpWriteFinal = dkcDCF01WriteFinal;
	p->fpRead = dkcDCF01Read;
	p->fpReadFinal = dkcDCF01ReadFinal;
	

	p->fpFree = (DKC_DCF_FREE_F_TYPE)dkcFreeDCF01;

	p->fpFileToDCF = dkcFileToDCF01;
	p->fpDCFToFile = dkcDCF01ToFile;
}

DKC_INLINE int WINAPI dkcDCF01Write(DKC_DCF *p,const void *data,size_t size,size_t *write_size)
{
	int r;
	size_t offset,i;
	size_t pSize = p->BufferObj->mSize;
	size_t div =  size / pSize;
	size_t rest = size % pSize;
	uint8 *pBuff = p->BufferObj->mBuff;
	const uint8 *pData = data;

	
	dkcSHA512Load(p->SHAObj,data,size);
	
	offset = 0;
	for(i=0;i<div;i++){
		memcpy(pBuff,pData + offset,pSize);
		r = dkcSNOW2EncryptNoDestDOE(p->CryptObj,pBuff,pSize);
		if(DKUTIL_FAILED(r)) return r;
		r = dkcStreamWriteWithWriteSize(p->StreamObj,pBuff,pSize,write_size);
		if(DKUTIL_FAILED(r)) return r;
		if(pSize != *write_size) return edk_FAILED;
		offset += pSize;
		//p->Count += (*write_size);
	}
	memcpy(pBuff,pData + offset,rest);
	r = dkcSNOW2EncryptNoDestDOE(p->CryptObj,pBuff,rest);
	if(DKUTIL_FAILED(r)) return r;
	r = dkcStreamWriteWithWriteSize(p->StreamObj,pBuff,rest,write_size);
	if(DKUTIL_FAILED(r)) return r;
	if(rest != *write_size) return edk_FAILED;
	
	//p->Count += (*write_size);
	p->Count += size;
	return r;
}


/**
@return edk_SUCCEEDEDでハッシュ書き込み成功
*/
DKC_INLINE int WINAPI dkcDCF01WriteFinal(DKC_DCF *p){
	int r;
	uint8 sha512[SHA512_BIN_BUFFER_SIZE];
	r = dkcSHA512FinalDigest(p->SHAObj,sha512,sizeof(sha512));
	if(DKUTIL_FAILED(r)) return r;
	dkcStreamSeek(p->StreamObj,0,edkcStreamSeekSet);
	r = dkcStreamWrite(p->StreamObj,sha512,sizeof(sha512));
	if(DKUTIL_FAILED(r)) return r;

	r = dkcStreamWrite(p->StreamObj,&(p->Count),sizeof(p->Count));
	return r;
}

DKC_INLINE int WINAPI dkcDCF01Read(DKC_DCF *p,void *buff,size_t size,size_t *readsize)
{
	int r;
	*readsize = 0;
	r = dkcStreamRead(p->StreamObj,buff,size,readsize);
	if(DKUTIL_FAILED(r)) return r;
	r = dkcSNOW2DecryptNoDestDOE(p->CryptObj,buff,*readsize);
	if(DKUTIL_FAILED(r)) return r;
	dkcSHA512Load(p->SHAObj,buff,*readsize);
	p->Count += (*readsize);

	if(dkcStreamEOF(p->StreamObj)/* || 0==*readsize*/){
		return edk_EndProcess;
	}
	return r;

}


/**
@return edk_SUCCEEDEDでハッシュチェック完了
*/
DKC_INLINE int WINAPI dkcDCF01ReadFinal(DKC_DCF *p)
{
	uint64 datasize;
	uint8 sha512[SHA512_BIN_BUFFER_SIZE],fsig[SHA512_BIN_BUFFER_SIZE];
	int r;
	size_t readsize;
	r = dkcStreamSeek(p->StreamObj,0,edkcStreamSeekSet);
	if(DKUTIL_FAILED(r)) return r;
	r = dkcStreamRead(p->StreamObj,fsig,sizeof(fsig),&readsize);
	if(DKUTIL_FAILED(r)) return r;
	if(readsize != sizeof(fsig)) return edk_FAILED;
	r = dkcStreamRead(p->StreamObj,&datasize,sizeof(datasize),&readsize);
	if(DKUTIL_FAILED(r)) return r;
	if(readsize != sizeof(datasize)) return edk_FAILED;
	if(p->Count != datasize){
		return edk_SignatureException;
	}
	r = dkcSHA512FinalDigest(p->SHAObj,sha512,sizeof(sha512));
	if(DKUTIL_FAILED(r)) return r;
	if(0 != memcmp(sha512,fsig,sizeof(sha512))) return edk_SignatureException;
	
	return edk_SUCCEEDED;
}


///セーブロードテスト
int WINAPI dkcFileToDCF01(const char *save_filename,const char *target_filename,
					 const void *key,size_t keysize)
{
	DKC_STREAM *ps = NULL,*reader = NULL;
	DKC_SNOW2 *psnow2 = NULL;
	DKC_DCF *p = NULL;
	//DWORD high,low;
	size_t readsize,writesize;
	uint8 tempbuff[1024];
	int r = edk_FAILED;

	ps = dkcAllocStreamFile64Type(edkcStreamWin32PromoteFlag,save_filename,edkcWriteMode);
	if(NULL==ps) return edk_FAILED;
	psnow2 = dkcAllocSNOW2NoLimitKeyLength(key,keysize);
	if(NULL==psnow2) goto Error;
	
	p = dkcAllocDCF_Version01_SNOW2(psnow2,ps);
	if(NULL==p) goto Error;



	//if(FALSE==dkcFileSize64(target_filename,&high,&low)) goto Error;
	//printf("%d bytes\n",(high << 32) + low)	;

	reader = dkcAllocStreamFile64Type(edkcStreamWin32PromoteFlag,target_filename,edkcReadMode);
	if(NULL==reader) goto Error;

	while(FALSE==dkcStreamEOF(reader))
	{
		dkcStreamRead(reader,tempbuff,sizeof(tempbuff),&readsize);
		r = dkcDCF01Write(p,tempbuff,readsize,&writesize);
		if(DKUTIL_FAILED(r)) goto Error;
	}
	r = dkcDCF01WriteFinal(p);
	if(DKUTIL_FAILED(r)) goto Error;

	dkcDCF01Init(p);

	r = edk_SUCCEEDED;
Error:
	dkcFreeDCF01(&p);
	dkcFreeSNOW2(&psnow2);
	dkcFreeStream(&ps);
	dkcFreeStream(&reader);
	return r;
}

///セーブロードテスト
int WINAPI dkcDCF01ToFile(const char *dest_filename,const char *target_filename,
					 const void *key,size_t keysize)
{
	DKC_STREAM *ps = NULL,*writer = NULL;
	DKC_SNOW2 *psnow2 = NULL;
	DKC_DCF *p = NULL;
	//DWORD high,low;
	size_t readsize;
	uint8 tempbuff[1024];
	int r = edk_FAILED;

	ps = dkcAllocStreamFile64Type(edkcStreamWin32PromoteFlag,target_filename,edkcReadMode);
	if(NULL==ps) return edk_FAILED;
	psnow2 = dkcAllocSNOW2NoLimitKeyLength(key,keysize);
	if(NULL==psnow2) goto Error;
	
	p = dkcAllocDCF_Version01_SNOW2(psnow2,ps);
	if(NULL==p) goto Error;


	
	//if(FALSE==dkcFileSize64(target_filename,&high,&low) ) goto Error;
	
	writer = dkcAllocStreamFile64Type(edkcStreamWin32PromoteFlag,dest_filename,edkcWriteMode);
	if(NULL==writer) goto Error;

	do
	{
		r = dkcDCF01Read(p,tempbuff,sizeof(tempbuff),&readsize);
		
		if(DKUTIL_FAILED(r) && (r != edk_EndProcess))
		{
			goto Error;
		}
		if(dkcStreamWrite(writer,tempbuff,readsize))
			goto Error;

	}while(edk_EndProcess!=r);

	r = dkcDCF01ReadFinal(p);
	if(DKUTIL_FAILED(r)) goto Error;

	dkcDCF01Init(p);

	r = edk_SUCCEEDED;
Error:
	dkcFreeDCF01(&p);
	dkcFreeSNOW2(&psnow2);
	dkcFreeStream(&ps);
	dkcFreeStream(&writer);
	return r;
} 


#if 0

BOOL WINAPI dkcDCF01PasswordCheck(const char *filename,const void *key,size_t keysize)
{
	DKC_STREAM *ps = NULL;
	DKC_SNOW2 *psnow2 = NULL;
	DKC_DCF *p = NULL;
	//DWORD high,low;
	size_t readsize;
	uint8 tempbuff[1024];
	BOOL r = FALSE;


	ps = dkcAllocStreamFile64Type(edkcStreamWin32PromoteFlag,filename,edkcReadMode);
	if(NULL==ps) return FALSE;
	psnow2 = dkcAllocSNOW2NoLimitKeyLength(key,keysize);
	if(NULL==psnow2) goto Error;
	
	p = dkcAllocDCF_Version01_SNOW2(psnow2,ps);
	if(NULL==p) goto Error;

	do
	{
		r = dkcDCFRead(p,tempbuff,sizeof(tempbuff),&readsize);
		
		if(DKUTIL_FAILED(r) && (r != edk_EndProcess))
		{
			goto Error;
		}

	}while(edk_EndProcess!=r);

	r = dkcDCFReadFinal(p);
	if(DKUTIL_FAILED(r)) goto Error;

	r = TRUE;
Error:
	dkcFreeDCF(&p);
	dkcFreeSNOW2(&psnow2);
	dkcFreeStream(&ps);
	return r;
}

#endif