
#include "dkcCircularStream.h"


#if 0

DKC_CIRCULARSTREAM * WINAPI dkcAllocNewCircularStream(size_t size,
					short mode,const char *filename,const char *openmode)
{
	DKC_CIRCULARSTREAM *p = NULL;
	int r=edk_FAILED;

	p = (DKC_CIRCULARSTREAM *)dkcAllocate(sizeof(DKC_CIRCULARSTREAM));
	if(NULL==p) return NULL;
	switch(mode){
	case edkcCircularStreamInitBuffer:
		r = dkcNewCircularStream(p,size);
		break;
	case edkcCircularStreamInitFile:
		r = dkcNewCircularStreamForFile(p,size,filename,openmode);
		break;
	default:
		goto Error;
	}
	
	if(DKUTIL_FAILED(r))
	{
		goto Error;
	}
	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreeDeleteCircularStream(DKC_CIRCULARSTREAM **ptr){
	if(NULL==ptr){return edk_ArgumentException;}
	if(DKUTIL_FAILED(dkcDeleteCircularStream(*ptr))){
		return edk_FAILED;
	}
	return dkcFree((void **)ptr);
}

int WINAPI dkcNewCircularStream(DKC_CIRCULARSTREAM *ptr,size_t size){
	if(NULL==ptr) return edk_ArgumentException;
	if(NULL != ptr->mStream){
		return edk_FAILED;
	}


	ptr->mStream = dkcAllocStream(edkcStreamInitBuffer,size,NULL,NULL);
	
	if(!ptr->mStream){
		return edk_OutOfMemory;
	}


	ptr->mStart = 0;
	ptr->mEnd = 0;
	ptr->mEnableLength = 0;
	ptr->mMode = edkcCircularStreamInitBuffer;
	ptr->mSize = size;

	return edk_SUCCEEDED;
}

int WINAPI dkcNewCircularStreamForFile(DKC_CIRCULARSTREAM *ptr,size_t size,const char *filename,const char *openmode)
{
	void *p;
	if(NULL==ptr || NULL==filename || NULL==openmode) return edk_ArgumentException;
	
	p = dkcFOpen(filename,openmode);
	if(NULL==p) return edk_FAILED;
	ptr->mStream = p;
	ptr->mEnableLength = 0;
	ptr->mStart = 0;
	ptr->mEnd = 0;
	ptr->mMode = edkcCircularStreamInitFile;
	ptr->mSize = size;
	return edk_SUCCEEDED;

}
static DKC_INLINE BOOL isDoubleProcess(size_t start,size_t data_length,size_t border_size)
{
	return (start + data_length > border_size);
}



static int WINAPI dkcCircularStreamReadLogic(
	DKC_CIRCULARSTREAM *RingBuf,void *Buf , size_t Len,size_t *readsize,BOOL PeekFlag)
{
	if(NULL==RingBuf || NULL==RingBuf->mStream || NULL==readsize)
		return edk_ArgumentException;


	if( RingBuf->mEnableLength < Len )
	{	// 存在するデータ量より多かったら
		//return edk_FAILED ;
		//とりあえず、ありったけのデータをもらおう。
		Len = RingBuf->mEnableLength;

	}else{


	}
	//読み込むデータはこのくらいあるのだ！
	*readsize = Len;

	// ２回に別けなければいけないかどうかで処理を分岐
	if( isDoubleProcess(RingBuf->mStart, Len, RingBuf->mSize) /*RingBuf->mStart + Len > RingBuf->mSize*/ )
	{
		// ２回に別ける場合の処理
		//memcpy( Buf, 
		//	(char *)RingBuf->mBuffer + RingBuf->mStart,
		//	RingBuf->mSize - RingBuf->mStart ) ;
		//memcpy( (char *)Buf + ( RingBuf->mSize - RingBuf->mStart ),
		//	RingBuf->mBuffer,
		//	Len - ( RingBuf->mSize - RingBuf->mStart ) ) ;
		
		//最初からシーク
		dkcStreamToBuffer(RingBuf->mStream,edkcStreamSeekSet,
			RingBuf->mStart, RingBuf->mSize - RingBuf->mStart,Buf,Len);

		dkcStreamToBuffer(RingBuf->mStream,edkcStreamSeekSet,
			0 ,Len - ( RingBuf->mSize - RingBuf->mStart ),
			(char *)Buf + ( RingBuf->mSize - RingBuf->mStart ),Len);
		
		if( PeekFlag == FALSE ) RingBuf->mStart = Len - ( RingBuf->mSize - RingBuf->mStart ) ;
	}
	else
	{
		// １回で格納する場合の処理
		//memcpy( Buf, (char *)RingBuf->mBuffer + RingBuf->mStart, Len ) ;
		
		dkcStreamToBuffer(RingBuf->mStream,edkcStreamSeekSet,
			RingBuf->mStart,Len,Buf,Len);

		if( PeekFlag == FALSE )  RingBuf->mStart += Len ;
	}

	// データの量を減らす
	if( PeekFlag == FALSE )  RingBuf->mEnableLength -= Len ;
	return edk_SUCCEEDED;
}

//streamをリードする。freadみたいなもの
int WINAPI dkcCircularStreamRead(DKC_CIRCULARSTREAM *ptr,void *buffer,size_t size,size_t *readsize)
{
	return dkcCircularStreamReadLogic(ptr,buffer,size,readsize,FALSE);
}
//中を覗き見るだけ。
int WINAPI dkcCircularStreamReference(DKC_CIRCULARSTREAM *ptr,void *buffer,size_t size,size_t *readsize)
{
	return dkcCircularStreamReadLogic(ptr,buffer,size,readsize,TRUE);
}


//streamをライトする。fwriteみたいなもの
int WINAPI dkcCircularStreamWrite(DKC_CIRCULARSTREAM *ptr,const void *buffer,size_t size){
	if(NULL==ptr || NULL==buffer || 0==size ) return edk_ArgumentException;
	//DxLibを参考にしました。この場をお借りしてお礼申し上げます。m(_ _)m
	switch(ptr->mMode){
	case edkcCircularStreamInitBuffer:
		// ２回に分けて格納しなければならないかどうかで処理を分岐
		if(isDoubleProcess(ptr->mEnd,size,ptr->mSize))
		{
			// ２回に別けて格納する場合の処理
			//memcpy( (char *)ptr->mStream + ptr->mEnd, buffer, ptr->mSize - ptr->mEnd ) ;
			//memcpy( ptr->mStream, (char *)buffer + ( ptr->mSize - ptr->mEnd ), size - ( ptr->mSize - ptr->mEnd ) ) ;
			dkcStreamWrite(ptr->mStream,buffer,ptr->mSize - ptr->mEnd);
			dkcStreamSeek(ptr->mStream,0,edkcStreamSeekSet);//オフセットを最初にする。
			dkcStreamWrite(ptr->mStream,(char *)buffer + ( ptr->mSize - ptr->mEnd ),size - ( ptr->mSize - ptr->mEnd ));
			ptr->mEnd = size - ( ptr->mSize - ptr->mEnd ) ;
		}else{
			// １回で格納する場合の処理
			//memcpy( (char *)ptr->mStream + ptr->mEnd, buffer, size ) ;
			dkcStreamWrite(ptr->mStream,buffer,size);
			ptr->mEnd += size ;
		}
		break;
	case edkcCircularStreamInitFile:
		break;
	default://やっぱり老婆心
		return edk_FAILED;
	}
	// 格納しているデータの量を増やす
	ptr->mEnableLength += size;
	return edk_SUCCEEDED;
}
int WINAPI dkcCircularStreamZero(DKC_CIRCULARSTREAM *ptr){
	int r;
	if(NULL==ptr || NULL==ptr->mStream){return edk_ArgumentException;}
	r = dkcStreamClear(ptr->mStream);
	if(DKUTIL_FAILED(r)) return r;
	ptr->mEnableLength = 0;
	ptr->mEnd = 0;
	//ptr->mSize = 0;
	ptr->mStart = 0;

	/*//switch(ptr->mMode){
	//case edkcCircularStreamInitBuffer:
	
		//break;
	//case edkcCircularStreamInitBuffer
		//dkcStreamZeroInit(ptr->mStream);
	//}*/
	return edk_SUCCEEDED;
}

//stream内のものをバッファにコピーする。
int WINAPI dkcCircularStreamToBuffer(const DKC_STREAM *ptr,char *buff,size_t size,size_t want_size)
{
	
	return edk_SUCCEEDED;
}




///streamをデリート fcloseみたいなもの　@note 必ずストリームを使用したあとはこれを呼んでください。
int WINAPI dkcDeleteCircularStream(DKC_CIRCULARSTREAM *ptr){
	if(NULL==ptr) return edk_ArgumentException;
	if(NULL==ptr->mStream) return edk_FAILED;
	switch(ptr->mMode){
	case edkcCircularStreamInitBuffer:
		dkcFree((void **)&ptr->mStream);
		break;
	case edkcCircularStreamInitFile:
		dkcFClose((FILE **)&ptr->mStream);
		break;
	default:
		return edk_FAILED;
	}
	DKUTIL_MEMZERO(ptr,sizeof(DKC_CIRCULARSTREAM));
	return edk_SUCCEEDED;
}





int WINAPI dkcCircularStreamDump(DKC_CIRCULARSTREAM *ptr,const char *filename,int flag){
	int result = edk_FAILED;
	char *buffer = NULL;
	size_t buffsize = 0;

	if(NULL==ptr || NULL==filename ) return edk_ArgumentException;
	switch(flag){
	case edkcCircularStreamDumpAll:
		result = dkcStreamDump(ptr->mStream,filename);
		break;
	case edkcCircularStreamDumpEnable:
		buffer = dkcAllocate(ptr->mEnableLength);
		if(NULL==buffer) return edk_OutOfMemory;
		buffsize = ptr->mEnableLength;


		// * : 使用中バッファ - : 無使用バッファ。
		if(ptr->mStart > ptr->mEnd)
		{//*****------***** こんな感じ。
			result = dkcStreamToBuffer(ptr->mStream,edkcStreamSeekSet,
				ptr->mStart,ptr->mSize - ptr->mStart,
				buffer,ptr->mSize - ptr->mStart);

#		ifdef DEBUG			
			if(DKUTIL_FAILED(result)) return edk_FAILED;
#		endif
			result = dkcStreamToBuffer(ptr->mStream,edkcStreamSeekSet,
				0,ptr->mEnd,
				(char *)buffer + (ptr->mSize - ptr->mStart),ptr->mEnd);

		}else{
			//---*****---- こんな感じ
			result = dkcStreamToBuffer(ptr->mStream,edkcStreamSeekSet,
				ptr->mStart,ptr->mEnd,buffer,buffsize);
		}
		if(DKUTIL_FAILED(result)) return edk_FAILED;


		result = dkcSaveBinary(buffer,buffsize,filename);
		
		
		break;
	default:
		return edk_ArgumentException;
	}
	return result;
}

#endif