/*!
@author d金魚
@note
licence:BSD Licence
*/

#define DKUTIL_C_STREAM_C
#include "dkcOSIndependent.h"
#include "dkcStream.h"
#include "dkcStdio.h"
#include "dkcMemoryStream.h"
#include "dkcGenericFileSystem.h"
#include <limits.h>


static DKC_INLINE int SetStreamInfo(DKC_STREAM *p,UINT flag){
	UINT stream_mode = 0;
	BYTE endian_mode = 0;
	UINT proc_mode = 0;

	BOOL isLittle = dkcIsLittleEndian();


	///ストリームの種類をきめる
	if(flag & edkcStreamInitMemory){
		stream_mode = edkcStreamInitMemory;
	}else if(flag & edkcStreamInitFile){
		stream_mode = edkcStreamInitFile;
	}else if(flag & edkcStreamInitMemoryAdapter){
		stream_mode = edkcStreamInitMemoryAdapter;
	}else if(flag & edkcStreamInitFile64){
		stream_mode = edkcStreamInitFile64;
	}else{
		goto Error;
	}


	endian_mode = FALSE;
	///エンディアンをきめる
	if(flag & edkcStreamBigEndian){
		//endianmode = edkcStreamBigEndian;
		if(isLittle){
			endian_mode = TRUE;
		}
	}else if(flag & edkcStreamLittleEndian){
		//endianmode = edkcStreamLittleEndian;
		if(!isLittle){
			endian_mode = TRUE;
		}
	}else /*if(flag & edkcStreamInitDefaultEndian)*/{
		if(isLittle){
			//endianmode = edkcStreamLittleEndian;
		}else{
			//endianmode = edkcStreamBigEndian;
		}
	}

	//プロセスの詳細 設定
	proc_mode |= (flag & edkcStreamProcessDefault);
	if(0==proc_mode){
		proc_mode |= (flag & 	edkcStreamProcessAsOrdered);
		
		if(0==proc_mode){
			proc_mode |= 	edkcStreamProcessAsOrdered;
		}
	}
	proc_mode |= (flag & 	edkcStreamWriteErrorWhenEndianChange );

	//update
	p->mMode = stream_mode;

	dkcmNOT_ASSERT(proc_mode > UCHAR_MAX);
	p->mProcessMode = (BYTE)proc_mode;

	p->mChangeEndian = endian_mode;

	return edk_SUCCEEDED;
Error:
	return edk_FAILED;

}




DKC_STREAM *WINAPI dkcAllocStreamMemoryType(UINT flag,const void *default_data,size_t size){
	DKUTIL_FLAG_UP(flag,edkcStreamInitMemory);
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitFile);
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitMemoryAdapter);
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitFile64);
	return dkcAllocStream(flag,default_data,size,NULL,NULL);

}

DKC_STREAM *WINAPI dkcAllocStreamFileType(UINT flag,const char *filename,const char *mode){
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitMemory);
	DKUTIL_FLAG_UP(flag,edkcStreamInitFile);
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitMemoryAdapter);
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitFile64);
	return dkcAllocStream(flag,NULL,0,filename,mode);
}

DKC_STREAM *WINAPI dkcAllocStreamMemoryAdapterType(UINT flag,void *adapt_buffer,size_t size){
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitMemory);
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitFile);
	DKUTIL_FLAG_UP(flag,edkcStreamInitMemoryAdapter);
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitFile64);
	return dkcAllocStream(flag,adapt_buffer,size,NULL,NULL);
}

DKC_STREAM *WINAPI dkcAllocStreamFile64Type(UINT flag,const char *filename,uint32 mode){
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitMemory);
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitFile);
	DKUTIL_FLAG_DOWN(flag,edkcStreamInitMemoryAdapter);
	DKUTIL_FLAG_UP(flag,edkcStreamInitFile64);
	return dkcAllocStream(flag,NULL,0,filename,(const char *)&mode);

}

DKC_STREAM *WINAPI dkcAllocStreamEncouragementFileType(const char *filename,const char *mode,BOOL OrderFlag)
{
	UINT flag = 0;
	DKUTIL_FLAG_UP(flag,edkcStreamInitFile);
	DKUTIL_FLAG_UP(flag,edkcStreamLittleEndian);
	DKUTIL_FLAG_DOWN(flag,edkcStreamBigEndian);
	if(OrderFlag){
		DKUTIL_FLAG_UP(flag,edkcStreamProcessAsOrdered );
	}else{
		DKUTIL_FLAG_UP(flag,edkcStreamProcessDefault);
	}
	return dkcAllocStream(flag,NULL,0,filename,mode);
}

/*
static DKC_INLINE int getFile64OpenMode(const char*mode,uint32 *get_mode){
	size_t len = strlen(mode),i;
	uint32 tm = 0;
	for(i=0;i<len;i++){
		switch(mode[i]){
		case 'r':
			tm |= edkcReadMode;
			break;
		case 'w':
			tm |= edkcWriteMode:
			break;
		case 'b':
			tm |= edkcBinaryMode;
			break;
		case 't':
			tm |= edkcTextMode;
			break;
		case 'a':
			tm |= edkcPostScriptMode;
			break;
		default:

			break;
		}
	}

}*/

DKC_STREAM * WINAPI dkcAllocStream(UINT flag,
																	 const void *default_data,size_t size,
																	 const char *filename,const char *mode)
{
	
	DKC_STREAM *p;
	void *psig;
	const uint32 *use_flag = (const uint32 *)mode;

	p = (DKC_STREAM *)dkcAllocate(sizeof(DKC_STREAM));
	if(NULL==p){
		return NULL;
	}

	if(DKUTIL_FAILED(SetStreamInfo(p,flag))){
		goto Error;
	}

	switch(p->mMode){
	case edkcStreamInitMemory:
		psig = (void *)dkcAllocMemoryStream(size);
		if(NULL==psig){
			goto Error;
		}
		//update	
		p->mSig = psig;
		//write default data
		dkcStreamWrite(p,default_data,size);
		dkcStreamSeek(p,0,edkcStreamSeekSet);
		break;
	case edkcStreamInitFile:
		psig = (void *)dkcFOpen(filename,mode);
		if(NULL==psig){
			goto Error;
		}
		//update
		p->mSig = psig;
		
		break;
	case edkcStreamInitMemoryAdapter:
		psig = dkcAllocMemoryStreamAdapter((void *)default_data,size);
		if(NULL==psig){
			goto Error;
		}
		p->mSig = psig;
	case edkcStreamInitFile64:
		psig = dkcAllocFile64(*use_flag,filename);
		if(NULL==psig){
			goto Error;
		}
		p->mSig = psig;
		break;
	default:
		goto Error;
	}

	
	//p->mEndianMode = endianmode

	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}


int WINAPI dkcFreeStream(DKC_STREAM **p){
	DKC_STREAM *t;
	if(NULL==p){
		return edk_FAILED;
	}
	t = *p;
	if(NULL==t){
		return edk_FAILED;
	}

	switch(t->mMode){
	case edkcStreamInitMemory:
		dkcFreeMemoryStream((DKC_MEMORYSTREAM **)&(t->mSig));
		break;
	case edkcStreamInitFile:
		dkcFClose((FILE **)&(t->mSig));
		break;
	case edkcStreamInitMemoryAdapter:
		dkcFreeMemoryStreamAdapter((DKC_MEMORYSTREAM_ADAPTER **)&(t->mSig));
		break;
	case edkcStreamInitFile64:
		dkcFreeFile64((DKC_FILE64 **)&(t->mSig));
		break;
#ifdef DEBUG
		default:
			dkcmNOT_ASSERT("dkcFreeStream FAILED");
#endif
	}
	return dkcFree((void **)p);
}


//**********************************************************

int WINAPI dkcStreamSeek(DKC_STREAM *ptr,int offset,int origin){
	int r = edk_FAILED;
	switch(ptr->mMode){
	case edkcStreamInitMemory:
		r = dkcMemoryStreamSeek((DKC_MEMORYSTREAM *)ptr->mSig,offset,origin);
		break;
	case edkcStreamInitFile:
		//成功したら０ == edk_SUCCEEDED;
		r = fseek((FILE *)ptr->mSig,offset,origin);
		break;	
	case edkcStreamInitMemoryAdapter:
		r = dkcMemroyStreamAdapterSeek((DKC_MEMORYSTREAM_ADAPTER *)ptr->mSig,offset,origin);
		break;
	case edkcStreamInitFile64:
		r = dkcFile64Seek((DKC_FILE64 *)ptr->mSig,offset,origin);
		break;
	}
	return r;

}


int WINAPI dkcStreamTell64(DKC_STREAM *ptr,uint64 *pv){
	int r = edk_FAILED;
	switch(ptr->mMode){
	case edkcStreamInitMemory:
		r =  dkcMemoryStreamTell((DKC_MEMORYSTREAM *)ptr->mSig);
		*pv = r;
		break;
	case edkcStreamInitFile:
		r = ftell((FILE *)ptr->mSig);
		if(r==-1){
			return edk_FAILED;
		}
		*pv = r;
		break;
	case edkcStreamInitFile64:
		r = dkcFile64Tell((DKC_FILE64 *)ptr->mSig,pv);
		if(DKUTIL_FAILED(r)) return r;
		break;
	case edkcStreamInitMemoryAdapter:
		r = dkcMemoryStreamAdapterTell((DKC_MEMORYSTREAM_ADAPTER *)ptr->mSig);
		*pv = r;
		break;
	}
	
	return edk_SUCCEEDED;
}
///streamの位置を取得する。
long WINAPI dkcStreamTell(DKC_STREAM *ptr){
	int r = -1;
	uint64 t;
	switch(ptr->mMode){
	case edkcStreamInitMemory:
		r =  dkcMemoryStreamTell((DKC_MEMORYSTREAM *)ptr->mSig);
		break;
	case edkcStreamInitFile:
		r = ftell((FILE *)ptr->mSig);
		if(r==-1){
			return edk_FAILED;
		}
		break;
	case edkcStreamInitFile64:
		dkcFile64Tell((DKC_FILE64 *)ptr->mSig,&t);
		if(t > LONG_MAX){
			return -1;
		}
		r = (long)t;
		break;
	case edkcStreamInitMemoryAdapter:
		r = dkcMemoryStreamAdapterTell((DKC_MEMORYSTREAM_ADAPTER *)ptr->mSig);
		break;
	}
	return r;
	//dkcmNOT_ASSERT("dkcStreamTell failed");
}

int WINAPI dkcStreamFlush(DKC_STREAM *ptr){
	int r = edk_SUCCEEDED;
	//filter
	if(edkcStreamInitFile64==ptr->mMode){
		_flushall();
		return edk_SUCCEEDED;
	}
	if(edkcStreamInitFile!=ptr->mMode){
		return edk_FAILED;
	}

	r = fflush((FILE *)ptr->mSig);

	if(r==EOF){
		return edk_FAILED;
	}
	return edk_SUCCEEDED;
}

BOOL WINAPI dkcStreamEOF(DKC_STREAM *ptr){
	
	//filter
	if(edkcStreamInitFile64==ptr->mMode){
		return dkcFile64EOF((DKC_FILE64 *)ptr->mSig);	
	}
	if(edkcStreamInitFile != ptr->mMode){
		goto Exit;
	}
	if(0 != feof((FILE *)ptr->mSig)){
		return TRUE;
	}
Exit:
	return FALSE;
}

BOOL WINAPI dkcStreamError(DKC_STREAM *ptr){
	//filter
	if(edkcStreamInitFile != ptr->mMode){
		goto Exit;
	}
	if(0 != ferror((FILE *)ptr->mSig)){
		return TRUE;
	}
Exit:
	return FALSE;
}
//**********************************************************

static DKC_INLINE int dkcStreamReadObayAnOrder(
	DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize)
{
	int r = edk_FAILED;
	size_t readsize_ = 0;
	size_t count;
	size_t tc,tsize;
	BYTE *tbuffer;
	FILE *fp = (FILE *)ptr->mSig;

	if(NULL==readsize){
		readsize = &readsize_;
	}

	//1,sizeの引数を入れ替えるとどうか？
	//1 = fread(buffer,size,1,ptr->mfp);
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
			//*readsize = count;
			r = edk_FAILED;
			break;
		}
		if(feof(fp)){
			//*readsize = count;
			r = edk_BufferOverFlow;
			break;
		}
		//read
		tc = fread(&tbuffer[count],1,tsize,fp);
		//update
		tsize -= tc;
		count += tc;

		if(count == size){
			r = edk_SUCCEEDED;
			break;
		}
#	ifdef DEBUG
		//ありえないエラーチェック
		dkcmNOT_ASSERT(count > size);
#	else
		if(count > size){
			break;
		}
#	endif
	}
	*readsize = count;
	return r;

}

DKC_INLINE int WINAPI dkcStreamRead(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize){
	int r = edk_FAILED;
	FILE *fp = (FILE *)ptr->mSig;
	size_t redsize = 0;

	//readsize filter
	if(NULL==readsize){
		readsize = &redsize;
	}

	//エンディアンチェンジ指定されているよ～～。
	/*if(flag & edkcStreamReadErrorWhenEndianChange){
		if(ptr->mChangeEndian){
			return edk_Not_Satisfactory;
		}
	}*/

	switch(ptr->mMode){
	case edkcStreamInitMemory:
		r = dkcMemoryStreamRead((DKC_MEMORYSTREAM *)ptr->mSig,
			buffer,size,readsize);
		break;
	case edkcStreamInitFile:

		if(ptr->mProcessMode & edkcStreamProcessDefault){
			
			*readsize = fread(buffer,1,size,fp);
			
			//set result 
			if(feof(fp)){
				r = edk_BufferOverFlow;
			}else if(!ferror(fp)){
				r = edk_SUCCEEDED;
			}
			
		}else{
			r = dkcStreamReadObayAnOrder(ptr,buffer,size,readsize);
		}


		break;
	case edkcStreamInitMemoryAdapter:
		r = dkcMemoryStreamAdapterRead((DKC_MEMORYSTREAM_ADAPTER *)
			ptr->mSig,buffer,size,readsize);
		break;
	case edkcStreamInitFile64:
		r = dkcFile64Read((DKC_FILE64*)ptr->mSig,buffer,size,readsize);
		
		break;
	}
	return r;
}

static int WINAPI StreamRead(DKC_STREAM *ptr, void *buffer,size_t size,void *hoge){
	return dkcStreamRead(ptr,buffer,size,(size_t *)hoge);	
}

///@todo 仕様を満たしていないバグがあるかも　または仕様、実装変更
int WINAPI dkcStreamRead8(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize){
	return dkcStreamRead(ptr,buffer,size,readsize);
}

int WINAPI dkcStreamRead16(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize)
{
	return dkcStreamProcess16(ptr,buffer,size,StreamRead,(void *)readsize);
	/*
	size_t sentinel = size / 2;
	size_t i = 0;
	size_t t_rs = 0;

	USHORT *pus;

	size_t real_readsize = 0;
	int result;

	//filter
	if(size_t % 2 != 0){
		return edk_ArgumentException;
	}

	pus = (USHORT *)buffer;
	
	for(;i<sentinel;i++){
		//process
		result = dkcStreamRead(ptr,&pus[i],2,&t_rs);
		

		//update data
		real_readsize += t_rs;


		//process state update check
		switch(result){
		case edkSUCCEEDED:
			continue;
		case edk_FAILED:
		case edk_BufferOverFlow:
			goto End;
		default:
			dkcmNOT_ASSERT("dkcStreamSwapRead16 Error");
			goto End;
		}
	}//end of for
End:
	if(readsize){//set readsize
		*readsize = real_readsize;
	}
	//process result
	return result;
	*/
}

int WINAPI dkcStreamRead32(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize){
	return dkcStreamProcess32(ptr,buffer,size,StreamRead,(void *)readsize);
}

int WINAPI dkcStreamRead64(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize){
	return dkcStreamProcess64(ptr,buffer,size,StreamRead,(void *)readsize);
}


int WINAPI dkcStreamRef(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize)
{
	//位置を記憶
	long t = dkcStreamTell(ptr);
	//読みこむ（内部で位置がsize分（readsize分）ずらされる）
	int r = dkcStreamRead(ptr,buffer,size,readsize);
	
	
	/*
	if(readsize > INT_MAX){
		dkcmNOT_ASSEERT("INT_MAX byteも一気に読みこむな！ヽ(`Д´)ノムキィ");
		return edk_FAILED;
	}
	t = (*readsize) - t;
	
	//この条件はありえない。
	dkcmNOT_ASSERT(t < 0);*/


	///tとして保存していた物を指定して　元の位置に戻す
	dkcStreamSeek(ptr,t,edkcStreamSeekSet);
	return r;
}

int WINAPI dkcStreamRef16(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize){
	long t = dkcStreamTell(ptr);
	int r = dkcStreamRead16(ptr,buffer,size,readsize);
	dkcStreamSeek(ptr,t,edkcStreamSeekSet);
	return r;
}
/**
@note
sizeは4byte単位でないといけない
*/

int WINAPI dkcStreamRef32(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize){
	long t = dkcStreamTell(ptr);
	int r = dkcStreamRead32(ptr,buffer,size,readsize);
	dkcStreamSeek(ptr,t,edkcStreamSeekSet);
	return r;
}
/**
@note
sizeは8byte単位でないといけない
*/
int WINAPI dkcStreamRef64(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize){
	long t = dkcStreamTell(ptr);
	int r = dkcStreamRead64(ptr,buffer,size,readsize);
	dkcStreamSeek(ptr,t,edkcStreamSeekSet);
	return r;

}














static DKC_INLINE int dkcStreamWriteObayAnOrder(DKC_STREAM *ptr,const void *buffer,size_t size){
	int r = edk_FAILED;
	size_t tc,tsize;
	size_t count;
	const BYTE *tbuffer = (const BYTE *)buffer;
	FILE *fp;

	tc = 0;
	count = 0;
	tsize = size;
	fp =  (FILE *)ptr->mSig;

	for(;;){
		//error check
		if(ferror(fp)){
			return edk_FAILED;
		}
		if(feof(fp)){
			return edk_BufferOverFlow;
		}
		//write
		tc = fwrite(&tbuffer[count],1,tsize,fp);
		//update
		tsize -= tc;
		count += tc;

		if(count == size){
			r = edk_SUCCEEDED;
			break;
		}
#	ifdef DEBUG
		//ありえないエラーチェック
		dkcmNOT_ASSERT(count > size);
#	else
		if(count > size){

			break;
		}
#	endif

	}


	return r;
}


int WINAPI dkcStreamWriteWithWriteSize(DKC_STREAM *ptr,const void *buffer,size_t size,size_t *ws)
{
	int r = edk_FAILED;
	UINT proc_flag = ptr->mProcessMode;
	///書き込み指定がこうなっているので･･･。
	if(proc_flag & edkcStreamWriteErrorWhenEndianChange){
		if(ptr->mChangeEndian){
			return edk_Not_Satisfactory;
		}
	}

	switch(ptr->mMode){
	case edkcStreamInitMemory:
		r =  dkcMemoryStreamWrite((DKC_MEMORYSTREAM *)ptr->mSig,buffer,size);
		break;
	case edkcStreamInitFile:
		r = dkcStreamWriteObayAnOrder(ptr,buffer,size);
		break;
	case edkcStreamInitMemoryAdapter:
		r = dkcMemoryStreamAdapterWrite((DKC_MEMORYSTREAM_ADAPTER *)ptr->mSig,buffer,size);		
		break;
	case edkcStreamInitFile64:
		r = dkcFile64Write((DKC_FILE64*)ptr->mSig,buffer,size,ws);
		
		break;
	}

	return r;

}

static int WINAPI StreamWrite(DKC_STREAM *ptr, void *buffer,size_t size,void *hoge){
	return dkcStreamWrite(ptr,buffer,size);	
}
/*int WINAPI dkcStreamRef8(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize){
	//dkcStream
	
}*/

/*static DKC_INLINE dkcStreamWriteChangeEndian(DKC_STREAM *ptr,const void *buffer,size_t size){
	USHORT *pb;
	size_t elem_num;
	size_t i;

	pb = (USHORT)malloc(size);
	if(NULL==pb){
		return edk_FAILED;
	}

	memcpy(pb,buffer,size);
	elem_num = size / 16;

	//エンディアンをチェンジさせる。
	for(i=0;i<elem_num;i++){
		pb[i] = dkcReverseEndian16(pb[i]);
	}
	r = dkcStreamWrite(ptr,pb,size);

	free(pb);
}*/


int WINAPI dkcStreamWrite8(DKC_STREAM *ptr,void *buffer,size_t size){
	return dkcStreamWrite(ptr,buffer,size);
}
int WINAPI dkcStreamWrite16(DKC_STREAM *ptr, const void *buffer,size_t size){
	return dkcStreamProcess16(ptr,(void *)buffer,size,StreamWrite,NULL);
}

int WINAPI dkcStreamWrite32(DKC_STREAM *ptr, const void *buffer,size_t size){
	return dkcStreamProcess32(ptr,(void *)buffer,size,StreamWrite,NULL);

}



int WINAPI dkcStreamWrite64(DKC_STREAM *ptr, const void *buffer,size_t size){
	return dkcStreamProcess64(ptr,(void *)buffer,size,StreamWrite,NULL);		
}



///@todo 各ストリームプロセスのmalloc部を高速化するにはMemoryPoolを使う。
int WINAPI dkcStreamProcess(DKC_STREAM *ptr,void *buffer,size_t size,
														DKC_STREAM_PROCESS_TYPE write_t,void *data)
{
	return write_t(ptr,buffer,size,data);
}



int WINAPI dkcStreamProcess16(DKC_STREAM *ptr,void *buffer,size_t size,
																			 DKC_STREAM_PROCESS_TYPE write_t,void *data){
	int r;
	USHORT *pb;
	size_t elem_num,pause = 16;
	size_t i;

	if(size % pause != 0){
		return edk_ArgumentException;
	}
	if(ptr->mChangeEndian)
	{//エンディアンチェンジが必要
		pb = (USHORT *)malloc(size);
		if(NULL==pb){
			//return edk_FAILED;
			return edk_OutOfMemory;
		}
		
		//メモリコピー
		memcpy(pb,buffer,size);
		elem_num = size / pause;

		//エンディアンをチェンジさせる。
		for(i=0;i<elem_num;i++){
			pb[i] = dkcReverseEndian16(pb[i]);
		}
		//書き込む
		r = write_t(ptr,pb,size,data);

		free(pb);
	}else{
		r = write_t(ptr,buffer,size,data);
	}
	return r;
}


int WINAPI dkcStreamProcess32(DKC_STREAM *ptr,void *buffer,size_t size,
																			 DKC_STREAM_PROCESS_TYPE write_t,void *data){
	int r;
	ULONG *pb;
	size_t elem_num,pause = 32;
	size_t i;

	if(size % pause != 0){
		return edk_ArgumentException;
	}
	if(ptr->mChangeEndian)
	{//エンディアンチェンジが必要
		pb = (ULONG *)malloc(size);
		if(NULL==pb){
			return edk_OutOfMemory;
		}
		
		//メモリコピー
		memcpy(pb,buffer,size);
		elem_num = size / pause;

		//エンディアンをチェンジさせる。
		for(i=0;i<elem_num;i++){
			pb[i] = dkcReverseEndian32(pb[i]);
		}
		//書き込む
		r = write_t(ptr,pb,size,data);

		free(pb);
	}else{
		r = write_t(ptr,buffer,size,data);
	}
	return r;

}










int WINAPI dkcStreamProcess64(DKC_STREAM *ptr,void *buffer,size_t size,
																			 DKC_STREAM_PROCESS_TYPE write_t,void *data){
	int r;
	ULONGLONG *pb;
	size_t elem_num,pause = 64;
	size_t i;

	if(size % pause != 0){
		return edk_ArgumentException;
	}
	if(ptr->mChangeEndian)
	{//エンディアンチェンジが必要
		pb = (ULONGLONG *)malloc(size);
		if(NULL==pb){
			return edk_OutOfMemory;
		}
		
		//メモリコピー
		memcpy(pb,buffer,size);
		elem_num = size / pause;

		//エンディアンをチェンジさせる。
		for(i=0;i<elem_num;i++){
			pb[i] = dkcReverseEndian64(pb[i]);
		}
		//書き込む
		r = write_t(ptr,pb,size,data);

		free(pb);
	}else{
		r = write_t(ptr,buffer,size,data);
	}
	return r;
}



//stream内をmemset(0)する。(seek位置も最初に戻される。
int WINAPI dkcStreamClear(DKC_STREAM *ptr){
	//file size
	size_t fsize;
	//temp size
	size_t tsize;
	size_t write_size;
	FILE *fp;
	int r = edk_FAILED;
	char null_array[1024];


	switch(ptr->mMode){
	case edkcStreamInitMemory:
		r = dkcMemoryStreamClear( (DKC_MEMORYSTREAM *)ptr->mSig );
		break;
	case edkcStreamInitFile:
		
		memset(null_array,0,sizeof(null_array));
		fp =(FILE *) ptr->mSig;


		fseek( fp, 0, SEEK_END ) ;
		fsize = ftell( fp ) ;
		fseek( fp, 0, SEEK_SET ) ;
		
		if(fsize > sizeof(null_array))
		{//ファイルのサイズがNULL配列より大きい場合
			
			tsize = 0;
			write_size = sizeof(null_array);
			for(;;){
				
				r = dkcStreamWrite(ptr,null_array,write_size);
				
				if(DKUTIL_FAILED(r)){
					return r;
				}
				if(tsize >= fsize){
					r = edk_SUCCEEDED;
					break;
				}

				if(tsize + write_size > fsize){
					write_size = fsize - tsize;
				}else{
					tsize += write_size;

					//こいつはいらないけど一応･･･
					write_size = sizeof(null_array);
				}
			}
		}else{//ちっこいなら上書き上書き･･･
			r = dkcStreamWrite(ptr,null_array,fsize);
		}

		break;
	case edkcStreamInitMemoryAdapter:
		r = dkcMemoryStreamAdapterClear((DKC_MEMORYSTREAM_ADAPTER *)ptr->mSig);
		break;


	}
	return r;
}

