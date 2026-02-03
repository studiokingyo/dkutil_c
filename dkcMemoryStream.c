/*!
@author d金魚
@note
licence:BSD Licence
*/
#define DKUTIL_C_MEMORYSTREAM_C
#include <limits.h>
#include "dkcMemoryStream.h"
#include "dkcSJISFileSystem.h"
#include "dkcStdio.h"





DKC_MEMORYSTREAM * WINAPI dkcAllocMemoryStream(size_t size){
	DKC_MEMORYSTREAM *p;
	if(0==size) return NULL;

	p = (DKC_MEMORYSTREAM *)dkcAllocate(sizeof(DKC_MEMORYSTREAM));

	if(NULL==p) return NULL;
	p->mBuffer = (BYTE *)dkcAllocate(size);
	if(NULL==p->mBuffer) goto Error;

	p->mSize = size;
	p->mNowOffset = 0;
	return p;
Error:
	dkcFree((void **)&p);
	return NULL;

}


int WINAPI dkcFreeMemoryStream(DKC_MEMORYSTREAM **ptr){
	if(NULL==ptr || NULL==*ptr) return edk_ArgumentException;
	if(NULL==(*ptr)->mBuffer) return edk_FAILED;

	dkcFree((void **)&((*ptr)->mBuffer));
	return dkcFree((void **)ptr);
}




static int WINAPI dkcMemoryStreamSeekLogic(DKC_MEMORYSTREAM *ptr,int offset,int point,int origin){
	/*
	状態繊維：
	offset + succeeded
	offset - succeeded
	offset + length error
	offset - length error
	*/
	size_t change = (size_t)(point + offset);

	if(TRUE==dkcCheckOverflow32(point,offset)){
		return edk_FAILED;
	}
	//if(!(ptr->mSize > change)){
	if(!(ptr->mSize >= change)){
		return edk_FAILED;
	}

	//common...

	ptr->mNowOffset = change;
		

	return edk_SUCCEEDED;
}

int WINAPI dkcMemoryStreamSeek(DKC_MEMORYSTREAM *ptr,int offset,int origin){
	int result = edk_FAILED;
	if(NULL==ptr) return edk_ArgumentException;
	switch(origin)
	{
	case edkcMemoryStreamSeekCurrent://今の位置からシーク
		result = dkcMemoryStreamSeekLogic(ptr,offset,ptr->mNowOffset,origin);
		break;
	case edkcMemoryStreamSeekEnd://最後の位置からシーク
		//result = dkcMemoryStreamSeekLogic(ptr,offset,0,origin);
		result = dkcMemoryStreamSeekLogic(ptr,offset,ptr->mSize /*- 1*/,origin);
		break;
	case edkcMemoryStreamSeekSet://最初の位置からシーク
		//result = dkcMemoryStreamSeekLogic(ptr,offset,ptr->mSize /*- 1*/,origin);
		result = dkcMemoryStreamSeekLogic(ptr,offset,0,origin);
		break;
	default:
		//変な引数入れるな！
		return edk_ArgumentException;
	}
	return result;
}

size_t WINAPI dkcMemoryStreamGetSeekPoint(DKC_MEMORYSTREAM *ptr)
{
	return ptr->mNowOffset;
}

DKC_INLINE int WINAPI dkcMemoryStreamRead(DKC_MEMORYSTREAM *ptr,void *buffer,size_t size,size_t *readsize){

	
	size_t f_read = 0;
	//error check

	if(NULL==ptr || NULL==readsize) return edk_ArgumentException;

	//Read = (int)(ptr->mNowOffset + size); 
	//Read = (int)(ptr->mSize - Read); 
	//if(Read < 0) return edk_BufferOverFlow;
	
	//process...

	f_read = ptr->mNowOffset + size;

	if(ptr->mSize <= f_read){
		f_read = ptr->mSize - ptr->mNowOffset;
		if(0==f_read) return edk_BufferOverFlow;
	}else{
		f_read = size;
	}


	//酷バグ：まずい、酷いバグだ。
	/*dkcmNOT_ASSERT(edk_FAILED==dkc_memcpy(buffer,size,
		ptr->mBuffer + ptr->mNowOffset,f_read
	));*/
	if(DKUTIL_FAILED(dkc_memcpy(buffer,size,ptr->mBuffer + ptr->mNowOffset,f_read)))
	{
		return edk_FAILED;
	}

	//update...
	ptr->mNowOffset += f_read;
	*readsize = f_read;
	
	return edk_SUCCEEDED;
}


int WINAPI dkcMemoryStreamResize(DKC_MEMORYSTREAM *ptr,size_t want_size)
{
	int ra_r = 0;
	void *NewPtr = NULL;
	void *OldPtr = NULL;
	
	//error check
	if(NULL==ptr) return edk_ArgumentException;


	//process
	OldPtr = ptr->mBuffer;

	ra_r = dkcReallocate(&NewPtr,want_size,&OldPtr);
	if(DKUTIL_FAILED(ra_r)){
		return edk_FAILED;
	}

	//update...
	ptr->mBuffer = (BYTE *)NewPtr;
	ptr->mSize = want_size;


	return edk_SUCCEEDED;
}

/*!
@param ptr[in] 所貴下済みのDKC_MEMORYSTREAMへのポインタ
@param expand_size[in] 希望する拡張サイズ
*/
static int WINAPI dkcMemoryStreamAutoExpandResize(DKC_MEMORYSTREAM *ptr,size_t expand_size){
	int ra_r = 0;
	void *NewPtr = NULL;
	void *OldPtr = NULL;
	size_t ReallocatedSize = 0;
	size_t want_size;
	//size_t want_size = ptr->mSize + expand_size + 1;//なんとなく＋１
	
	if(NULL==ptr) return edk_ArgumentException;

	OldPtr = ptr->mBuffer;
	/*
	ra_r = dkcReallocateAutoExpand(realloc,&NewPtr,&OldPtr,ptr->mSize,&ReallocatedSize);
	if(DKUTIL_FAILED(ra_r)){
		return edk_FAILED;
	}
	if(want_size > ReallocatedSize){
		OldPtr = NewPtr;
		ra_r = dkcReallocate(&NewPtr,want_size,&OldPtr);
		if(DKUTIL_FAILED(ra_r)){
			return edk_FAILED;
		}
		ReallocatedSize = want_size;
	}
	*/
	want_size = dkcReallocateSizeFunction(
		ptr->mSize + ptr->mNowOffset,
		(expand_size <= 256) ? 256 : expand_size
	);
	ra_r = dkcReallocate(&NewPtr,want_size,&OldPtr);
	if(DKUTIL_FAILED(ra_r)){
		return edk_FAILED;
	}
	ReallocatedSize = want_size;



	ptr->mBuffer = (BYTE *)NewPtr;
	ptr->mSize = ReallocatedSize;


	return edk_SUCCEEDED;

}

DKC_INLINE int WINAPI dkcMemoryStreamWrite(DKC_MEMORYSTREAM *ptr,const void *buffer,size_t size){
	size_t rest;

	if(NULL==ptr) return edk_ArgumentException;
	

	if(ptr->mSize < ptr->mNowOffset + size){return edk_FAILED;}
	rest = ptr->mSize - ptr->mNowOffset;
	
	//酷バグ：dkcmNOT_ASSERTはDEBUGモード時にしか展開されない。
	/*dkcmNOT_ASSERT(
	));*/
	if(DKUTIL_FAILED(dkc_memcpy(
		(void *)((BYTE *)ptr->mBuffer + ptr->mNowOffset),
		rest,
		buffer,size))){
		return edk_FAILED;
	}
	
	ptr->mNowOffset += size;
	
	return edk_SUCCEEDED;
}


int WINAPI dkcMemoryStreamDynamicWrite(DKC_MEMORYSTREAM *ptr,const void *buffer,size_t size){
	int result;
	size_t want_size;

	if(NULL==ptr) return edk_ArgumentException;

	want_size =  ptr->mNowOffset + size;

	if(ptr->mSize < want_size)
	{
		result = dkcMemoryStreamAutoExpandResize(ptr,size);
		//result = dkcMemoryStreamResize(ptr,want_size);
		if(DKUTIL_FAILED(result)) return edk_FAILED;
	}

	return dkcMemoryStreamWrite(ptr,buffer,size);
}








DKC_INLINE int WINAPI dkcMemoryStreamClear(DKC_MEMORYSTREAM *ptr){
	dkcmNOT_ASSERT(NULL==ptr);
	//if(NULL==ptr) return edk_ArgumentException;
	memset(ptr->mBuffer,0,ptr->mSize);
	ptr->mNowOffset = 0;
	return edk_SUCCEEDED;
}

DKC_INLINE size_t WINAPI dkcMemoryStreamNowOffset(DKC_MEMORYSTREAM *p){
	return p->mNowOffset;
}

DKC_INLINE BYTE *WINAPI dkcMemoryStreamPointer(DKC_MEMORYSTREAM *p){
	return p->mBuffer;

}

DKC_MEMORYSTREAM * WINAPI dkcAllocMemoryStreamCopy(const DKC_MEMORYSTREAM *ptr)
{
	DKC_MEMORYSTREAM *tp = NULL;

	dkcmNOT_ASSERT(NULL==ptr);

	tp = dkcAllocMemoryStream(ptr->mSize);
	if(NULL==tp) return NULL;

	dkcMemoryStreamWrite(tp,ptr->mBuffer,ptr->mSize);

	//内部オフセット変更									/* 先頭から */
	dkcMemoryStreamSeek(tp,ptr->mNowOffset,edkcMemoryStreamSeekSet);
	return tp;
}

/*
DKC_INLINE int WINAPI dkcMemoryStreamPushBack(DKC_MEMORYSTREAM_ADAPTER *p,const BYTE *buff,size_t size)
{
	return msa_rw_proc(p,(BYTE *)buff,size,FALSE);
}
*/
DKC_INLINE int WINAPI dkcMemoryStreamPopBack(DKC_MEMORYSTREAM_ADAPTER *p,size_t size)
{
	/*if(size > p->mNowOffset){
		return edk_FAILED;
	}
	p->mNowOffset -= size;
	return edk_SUCCEEDED;*/
	int t;
	if(size > (size_t)(-(INT_MIN))){//sizeがでかい
		return edk_ArgumentException;
	}
	t = (int)size;
	t = -t;
	return dkcMemoryStreamSeek(p,t,SEEK_CUR);
}

//**********************************************************
//memory stream adapter
/*
///read or write process
static DKC_FORCE_INLINE int msa_rw_proc(DKC_MEMORYSTREAM_ADAPTER *p,BYTE *buff,size_t size,BOOL is_read)
{
	size_t offset = p->mNowOffset;
	BYTE *tb = p->mBuffer;
	if(dkcCheckOverflowULONG(offset,size)){
		return edk_FAILED;
	}
	if(p->mNowOffset >= p->mSize){
		return edk_FAILED;
	}
	if(is_read){
		size_t rs = size;//only for debug
		if(offset + size > p->mSize){
			rs = offset + size - p->mSize;
		}
		memcpy(buff,&tb[offset],rs);
		p->mNowOffset += rs;
	}else{
		//if(offset + size >= p->mSize){
		if(offset + size > p->mSize){
			return edk_BufferOverFlow;
		}
		memcpy(&tb[offset],buff,size);
		p->mNowOffset += size;
	}

	return edk_SUCCEEDED;
}
*/
DKC_INLINE void WINAPI dkcMemoryStreamAdapterInit(DKC_MEMORYSTREAM_ADAPTER *p,BYTE *buff,size_t size)
{
	p->mBuffer = buff;
	p->mSize = size;
	p->mNowOffset = 0;

}
DKC_MEMORYSTREAM_ADAPTER *WINAPI dkcAllocMemoryStreamAdapter(BYTE *buff,size_t size){
	DKC_MEMORYSTREAM_ADAPTER *p = dkcAllocate(sizeof(DKC_MEMORYSTREAM_ADAPTER));
	if(NULL==p) return NULL;
	dkcMemoryStreamAdapterInit(p,buff,size);
	return p;
}

int WINAPI dkcFreeMemoryStreamAdapter(DKC_MEMORYSTREAM_ADAPTER **p)
{
	return dkcFree(p);
}
DKC_MEMORYSTREAM_ADAPTER *dkcAllocMemoryStreamAdapterCopy(DKC_MEMORYSTREAM_ADAPTER *p)
{
	DKC_MEMORYSTREAM_ADAPTER *tp = dkcAllocMemoryStreamAdapter(p->mBuffer,p->mSize);
	if(!tp) return NULL;
	tp->mNowOffset = p->mNowOffset;
	return tp;
}


#if 0
DKC_INLINE int WINAPI dkcMemoryStreamAdapterSetOffset(DKC_MEMORYSTREAM_ADAPTER *p,size_t offset){
	if(offset > p->mSize){
		return edk_FAILED;
	}
	p->mNowOffset = offset;
	return edk_SUCCEEDED;
}
DKC_INLINE size_t WINAPI dkcMemoryStreamAdapterGetOffset(DKC_MEMORYSTREAM_ADAPTER *p){
	return p->mNowOffset;
}

DKC_INLINE int WINAPI dkcMemoryStreamAdapterRead(DKC_MEMORYSTREAM_ADAPTER *p,void *buff,size_t buffsize,size_t *readsize)
{
	return msa_rw_proc(p,(BYTE *)buff,buffsize,TRUE);
}
DKC_INLINE int WINAPI dkcMemoryStreamAdapterGetChar(DKC_MEMORYSTREAM_ADAPTER *p,BYTE *t)
{
	/*size_t offset = p->mNowOffset;
	if(dkcCheckOverflowULONG(offset,1)){
		return edk_FAILED;
	}
	if(offset + 1 > p->mSize){
		return edk_BufferOverFlow;
	}
	//p->mNowOffset += 1;
	*t = p->mBuffer[p->mNowOffset++];
	return edk_SUCCEEDED;*/
	size_t rs;
	return dkcMemoryStreamAdapterRead(p,t,1,&rs);
}
#endif

int WINAPI dkcMemoryStreamDump(DKC_MEMORYSTREAM *ptr,const char *filename,UINT flag){
	if(flag == edkcDumpRegistBufferAll){

		return dkcSaveBinary(ptr->mBuffer,ptr->mSize,filename);
	}else{
		
	}
	return dkcSaveBinary(ptr->mBuffer,ptr->mNowOffset,filename);
}

int WINAPI dkcMemoryStreamWriteToMemory(const DKC_MEMORYSTREAM *ptr,void *buffer,size_t buffsize,UINT flag)
{
	int r=edk_FAILED;
	/*if(buffsize < ptr->mSize){
		return edk_BufferOverFlow;	
	}	*/
	size_t size_temp = 0;
	switch(flag){
	case edkcStreamBufferAll:
		r = dkc_memcpy(buffer,buffsize,ptr->mBuffer,ptr->mSize);
		break;
	case edkcStreamBufferToNowOffset:
		r = dkc_memcpy(buffer,buffsize,ptr->mBuffer,ptr->mNowOffset);
		break;
	case edkcStreamBufferFirst:
		if(buffsize >= ptr->mSize){//ここの条件間違って要る可能性あり
			size_temp = ptr->mSize;
		}else{
			size_temp = buffsize;
		}
		r = dkc_memcpy(buffer,buffsize,ptr->mBuffer,size_temp);
	default://todo:強制終了させてもOK
		return edk_FAILED;
	}
	return r;
}

int WINAPI dkcMemoryStreamGet8(DKC_MEMORYSTREAM *ptr,BYTE *t){
	size_t rs;
	return dkcMemoryStreamRead(ptr,t,1,&rs);

}
int WINAPI dkcMemoryStreamPut8(DKC_MEMORYSTREAM *ptr,BYTE t){
	return dkcMemoryStreamWrite(ptr,&t,1);
}
//#error メモリストリーム系全般をテスト
/*
todo:
put get 8bit系
write readバッファオーバーフローチェック
 状態を元に戻すのをチェック
seek tell の統一


*/



/*
@param flag[in] edkcStreamBufferAll または edkcStreamBufferFirst
*/
DKC_INLINE int WINAPI dkcMemoryStreamLoadFromFile(DKC_MEMORYSTREAM *ptr,const char *filename,size_t permit_size,UINT flag)
{
	size_t size;
	//許容サイズよりファイルサイズが大きい条件の結果をc1に入れる
	int c1 ;
	int r = edk_FAILED;
	size_t readsize = 0;
	size_t size_temp = 0;

	if(FALSE==dkcFileExist(filename))
		return edk_FileNotFound;

	
	size  = dkcFileSize(filename);
	c1 = (permit_size < size);
	switch(flag){
	case edkcStreamBufferAll:
		if(c1){
			return edk_FAILED;
		}
		size_temp = size;
		break;
	case edkcStreamBufferFirst:

		if(c1){
			size_temp = permit_size;
		}else{
			size_temp = size;
		}
		break;
	default:
		return edk_ArgumentException;
	}


	r = dkcMemoryStreamResize(ptr,size_temp);
	if(DKUTIL_FAILED(r)) goto End;
	
	r = dkcLoadBinary(ptr->mBuffer,size_temp,filename,&readsize);

	if(DKUTIL_FAILED(r)) goto End;			
End:
	return r;
}
DKC_INLINE int WINAPI dkcMemoryStreamLoadFromMemory(DKC_MEMORYSTREAM *ptr,
																										const void *buffer,size_t buffsize)
{//#error checkLoadFromMeory and LoadFromFile
	//return load(ptr,NULL,srcsize,edkcStreamBufferAll,buffer,buffsize,FALSE);
	int r;
	//size_t read_size;
	r = dkcMemoryStreamResize(ptr,buffsize);
	if(DKUTIL_FAILED(r)) goto End;
	
	r = dkc_memcpy(ptr->mBuffer,ptr->mSize,buffer,buffsize);

	//if(DKUTIL_FAILED(r)) goto End;			
End:
	return r;
}