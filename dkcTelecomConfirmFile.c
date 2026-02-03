
/**
@file dkcTelecomConfirmFile.c
@brief 通信のときに使うファイルフォーマット
@author d金魚
@since 2005/11/17
*/
#define  DKUTIL_C_TELECOM_CONFIRM_FILE_C
#include "dkcTelecomConfirmFile.h"
#include "dkcMath.h"


#if 0


DKC_INLINE DKC_TELECOM_CONFIRM_FILE* WINAPI dkcAllocTelecomConfirmFileGenerate(uint32 version,size_t lzw_output_block_size)
{
	DKC_TELECOM_CONFIRM_FILE *p;
	if(dkcmTelecomConfirmFileVersionError(version)){return NULL;}
	p = dkcAllocate(sizeof(DKC_TELECOM_CONFIRM_FILE));
	if(NULL==p){return NULL;}


	p->mLZW = dkcAllocLZW(lzw_output_block_size);
	//if(NULL==p->mLZW ) goto Error;
	p->mMD5 = dkcAllocMD5();
	//if(NULL==p->mMD5) goto Error;

	p->mSHA512 = dkcAllocSHA512();
	p->mBuff = dkcAllocBuffer(NULL,dkcdTELECOM_CONFIRM_FILE_BEGIN_BUFFER_SIZE);
	if(NULL==p->mSHA512 || NULL==p->mMD5 || NULL==p->mLZW || NULL==p->mBuff){
		goto Error;
	}
	p->version = version;
	p->block_num = 0;
	p->lzw_output_block_size = lzw_output_block_size;
	return p;
Error:
	dkcFreeTelecomConfirmFile(&p);
	return NULL;
}

DKC_INLINE DKC_TELECOM_CONFIRM_FILE *WINAPI dkcAllocTelecomConfirmFileExtract(DKC_STREAM *input)
{


}

DKC_INLINE int WINAPI dkcFreeTelecomConfirmFile(DKC_TELECOM_CONFIRM_FILE **pp){
	
	DKC_TELECOM_CONFIRM_FILE *p = *pp;
	if(NULL==pp || NULL==p) return edk_FAILED;
	if(p->mBuff){dkcFreeBuffer(&(p->mBuff));}
	if(p->mLZW){dkcFreeLZW(&(p->mLZW));}
	if(p->mMD5){dkcFreeMD5(&(p->mMD5));}
	if(p->mSHA512){dkcFreeSHA512(&(p->mSHA512));}
	return dkcFree(&p);
}

DKC_INLINE int WINAPI dkcTelecomConfirmFileInit(DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *pstream)
{
	int r;
	dkcMD5Init(p->mMD5);
	dkcSHA512Init(p->mSHA512);
	r = dkcBufferResize(p->mBuff,dkcdTELECOM_CONFIRM_FILE_BEGIN_BUFFER_SIZE);
	if(DKUTIL_FAILED(r)) return r;
	if(pstream){
		dkcmFAILED_CHECK_RETURN(r,dkcStreamSeek(pstream,0,edkcStreamSeekSet));
	}
	p->block_num = 0;
	return edk_SUCCEEDED;
}

DKC_INLINE int WINAPI dkcTelecomConfirmFileGenerateBegin(DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *output)
{
	
	if(0 != dkcStreamTell(output)){
		return edk_LogicError;
	}
	r = dkcStreamSeek(output,sizeof(DKC_TELECOM_CONFIRM_FILE_HEADER01),edkcStreamCurrent);
	if(DKUTIL_FAILED(r)) return r;
	return r;
}

static DKC_INLINE int tcpf_generate_output_block_version01(DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *output,const uint8 *src,size_t ssize)
{
	int r;
	DKC_LZW_HEADER lzw_header;
	DKC_TELECOM_CONFIRM_FILE_BLOCK01 block;
	
	if(p->mBuff->mSize < ssize){
		r = dkcBufferResize(p->mBuff,ssize);
		if(DKUTIL_FAILED(r)){
			return r;
		}
	}

	dkcSHA512Load(p->mSHA512,src,ssize);

	{

		dkcMD5Init(p->mMD5);
		dkcmFAILED_CHECK_RETURN(r,dkcMD5Load(p->mMD5,src,ssize));
		dkcmFAILED_CHECK_RETURN(r,dkcMD5FinalDigest(p->mMD5,block.origin_md5,sizeof(block.origin_md5)));


	}

	r = dkcLZWEncode(p->mLZW,&lzw_header,p->mBuff->mBuff,p->mBuff->mSize,
		src,ssize,UINT_MAX,'LZW',edkcLZW_Default);

	if(DKUTIL_FAILED(r)) return r;
		

	{

		dkcMD5Init(p->mMD5);
		dkcmFAILED_CHECK_RETURN(r,dkcMD5Load(p->mMD5,p->mBuff->mBuff,lzw_header.mCompressedSize));
		dkcmFAILED_CHECK_RETURN(r,dkcMD5FinalDigest(p->mMD5,block.compress_md5,sizeof(block.compress_md5)));
		
	}
	memcpy(&(block.lzw_header),&lzw_header,sizeof(block.lzw_header));
	dkcmFAILED_CHECK_RETURN(r,dkcStreamWrite(output,&block,sizeof(block)));
	dkcmFAILED_CHECK_RETURN(r,dkcStreamWrite(output,p->mBuff->mBuff,lzw_header.mCompressedSize);

	p->block_num++;
	return edk_SUCCEEDED;
}

DKC_INLINE int WINAPI dkcTelecomConfirmFileGenerateOutputBlock(DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *output,const uint8 *src,size_t ssize)
{
	int r;
	switch(p->version){
	case edkcTelecomConfirmFileVersion01:
		r = tcpf_generate_output_block_version01(p,output,src,ssize);
		break;
	default:
		return edk_LogicError;
	}
	return r;
}
static DKC_INLINE int tcpf_generate_final(DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *output)
{
	int r;
	DKC_TELECOM_CONFIRM_FILE_HEADER01 header;
	uint64 length = dkcStreamTell64(output);
	dkcmFAILED_CHECK_RETURN(r,dkcStreamSeek(output,0,edkcStreamSeekSet));
	dkcSHA512FinalDigest(p->mSHA512,header.origin_sig,sizeof(header.origin_sig));
	header.lzw_output_block_size = p->lzw_output_block_size;
	header.length

}

DKC_INLINE int WINAPI dkcTelecomConfirmFileGenerateFinal(DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *output)
{
	int r;
	switch(p->version){
	case edkcTelecomConfirmFileVersion01:
		r = tcpf_generate_final(p,output);
		break;
	default:
		return edk_LogicError;
	}
	return r;

}


DKC_INLINE int WINAPI dkcTelecomConfirmFileGetHeader(DKC_STREAM *input,DKC_TELECOM_CONFIRM_FILE_HEADER *header);

DKC_INLINE int WINAPI dkcTelecomConfirmFileGetBlock(DKC_STREAM *input,DKC_TELECOM_CONFIRM_FILE_BLOCK *block);


DKC_INLINE int WINAPI dkcTelecomConfirmFileExtractOutputBlock(
	DKC_TELECOM_CONFIRM_FILE *p,DKC_STREAM *input,const DKC_TELECOM_CONFIRM_FILE_BLOCK *block,
	uint8 *buff,size_t buffsize
);


DKC_INLINE int WINAPI dkcTelecomConfirmFileEncode(const char *dest_filename,const char *src_filename);


DKC_INLINE int WINAPI dkcTelecomConfirmFileDecode(const char *dest_filename,const char *src_filename);

																				 
DKC_INLINE size_t WINAPI dkcTelecomConfirmFileGetDecodeSize(const char *filename);

#endif