/*!
@file dkcCircularMemoryStream.c
@since 2004/3/xx
@note
*/
#define DKUTIL_C_CIRCULAR_MEMORYSTREAM_C
#include "dkcCircularMemoryStream.h"
#include "dkcStdio.h"


DKC_CIRCULAR_MEMORYSTREAM * WINAPI dkcAllocCircularMemoryStream(size_t size)
{
	DKC_CIRCULAR_MEMORYSTREAM *p = NULL;

	p = (DKC_CIRCULAR_MEMORYSTREAM *)dkcAllocate(sizeof(DKC_CIRCULAR_MEMORYSTREAM));
	if(NULL==p) return NULL;

	p->mBuffer = dkcAllocate(size);
	if(NULL==p->mBuffer) goto Error;

	p->mStart = 0;
	p->mEnd = 0;
	p->mEnableLength = 0;
	p->mSize = size;
	
	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreeCircularMemoryStream(DKC_CIRCULAR_MEMORYSTREAM **ptr){
	if(NULL==ptr || NULL==*ptr){return edk_ArgumentException;}
	dkcFree((void **)&(*ptr)->mBuffer);
	return dkcFree((void **)ptr);
}


static DKC_INLINE BOOL isDoubleProcess(size_t start,size_t data_length,size_t border_size)
{
	return (start + data_length > border_size);
}



static int WINAPI dkcCircularMemoryStreamReadLogic(
	DKC_CIRCULAR_MEMORYSTREAM *RingBuf,void *Buf , size_t Len,size_t *readsize,BOOL PeekFlag)
{
	if(NULL==RingBuf || NULL==RingBuf->mBuffer || NULL==readsize)
		return edk_ArgumentException;

	if( RingBuf->mEnableLength==0) return edk_FAILED;

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
		memcpy( Buf, 
			(char *)RingBuf->mBuffer + RingBuf->mStart,
			RingBuf->mSize - RingBuf->mStart ) ;
		memcpy( (char *)Buf + ( RingBuf->mSize - RingBuf->mStart ),
			RingBuf->mBuffer,
			Len - ( RingBuf->mSize - RingBuf->mStart ) ) ;
		
		//最初からシーク
		/*dkcStreamToBuffer(RingBuf->mBuffer,edkcStreamSeekSet,
			RingBuf->mStart, RingBuf->mSize - RingBuf->mStart,Buf,Len);

		dkcStreamToBuffer(RingBuf->mBuffer,edkcStreamSeekSet,
			0 ,Len - ( RingBuf->mSize - RingBuf->mStart ),
			(char *)Buf + ( RingBuf->mSize - RingBuf->mStart ),Len);
		*/
		if( PeekFlag == FALSE ) RingBuf->mStart = Len - ( RingBuf->mSize - RingBuf->mStart ) ;
	}
	else
	{
		// １回で格納する場合の処理
		memcpy( Buf, (char *)RingBuf->mBuffer + RingBuf->mStart, Len ) ;
		
		/*dkcStreamToBuffer(RingBuf->mBuffer,edkcStreamSeekSet,
			RingBuf->mStart,Len,Buf,Len);
		*/
		if( PeekFlag == FALSE )  RingBuf->mStart += Len ;
	}

	// データの量を減らす
	if( PeekFlag == FALSE )  RingBuf->mEnableLength -= Len ;
	return edk_SUCCEEDED;
}

//streamをリードする。freadみたいなもの
int WINAPI dkcCircularMemoryStreamRead(DKC_CIRCULAR_MEMORYSTREAM *ptr,void *buffer,size_t size,size_t *readsize)
{
	return dkcCircularMemoryStreamReadLogic(ptr,buffer,size,readsize,FALSE);
}
//中を覗き見るだけ。
int WINAPI dkcCircularMemoryStreamReference(DKC_CIRCULAR_MEMORYSTREAM *ptr,void *buffer,size_t size,size_t *readsize)
{
	return dkcCircularMemoryStreamReadLogic(ptr,buffer,size,readsize,TRUE);
}


//streamをライトする。fwriteみたいなもの
int WINAPI dkcCircularMemoryStreamWrite(DKC_CIRCULAR_MEMORYSTREAM *ptr,const void *buffer,size_t size){
	if(NULL==ptr || NULL==buffer || 0==size ) return edk_ArgumentException;
	//DxLibを参考にしました。この場をお借りしてお礼申し上げます。m(_ _)m
	
	//error check
	if(ptr->mSize - ptr->mEnableLength < size) return edk_FAILED;

	// ２回に分けて格納しなければならないかどうかで処理を分岐
	if(isDoubleProcess(ptr->mEnd,size,ptr->mSize))
	{
		// ２回に別けて格納する場合の処理
		memcpy( (char *)ptr->mBuffer + ptr->mEnd, buffer, ptr->mSize - ptr->mEnd ) ;
		memcpy( ptr->mBuffer, (char *)buffer + ( ptr->mSize - ptr->mEnd ), size - ( ptr->mSize - ptr->mEnd ) ) ;
		//dkcStreamWrite(ptr->mBuffer,buffer,ptr->mSize - ptr->mEnd);
		//dkcStreamSeek(ptr->mBuffer,0,edkcStreamSeekSet);//オフセットを最初にする。
		//dkcStreamWrite(ptr->mBuffer,(char *)buffer + ( ptr->mSize - ptr->mEnd ),size - ( ptr->mSize - ptr->mEnd ));
		ptr->mEnd = size - ( ptr->mSize - ptr->mEnd ) ;
	}else{
		// １回で格納する場合の処理
		memcpy( (char *)ptr->mBuffer + ptr->mEnd, buffer, size ) ;
		//dkcStreamWrite(ptr->mBuffer,buffer,size);
		ptr->mEnd += size ;
	}


	// 格納しているデータの量を増やす
	ptr->mEnableLength += size;
	return edk_SUCCEEDED;
}


int WINAPI dkcCircularMemoryStreamClear(DKC_CIRCULAR_MEMORYSTREAM *ptr){

	if(NULL==ptr || NULL==ptr->mBuffer){return edk_ArgumentException;}

	ptr->mEnableLength = 0;
	ptr->mEnd = 0;

	ptr->mStart = 0;


	return edk_SUCCEEDED;
}

//stream内のものをバッファにコピーする。
int WINAPI dkcCircularMemoryStreamToBuffer(
	const DKC_CIRCULAR_MEMORYSTREAM *ptr,char *buff,size_t size,size_t want_size)
{
	
	return edk_SUCCEEDED;
}

DKC_CIRCULAR_MEMORYSTREAM *WINAPI 
	dkcAllocCircularMemoryStreamCopy(const DKC_CIRCULAR_MEMORYSTREAM *ptr)
{
	DKC_CIRCULAR_MEMORYSTREAM *p;
	
	dkcmNOT_ASSERT(NULL==ptr);
	
	p = dkcAllocCircularMemoryStream(ptr->mSize);
	
	
	if(NULL==p) return NULL;//強制終了したい気分！！

	dkcmNOT_ASSERT(DKUTIL_FAILED(
		dkc_memcpy(p->mBuffer,p->mSize,ptr->mBuffer,ptr->mSize)
		));

	p->mEnableLength = ptr->mEnableLength;
	p->mEnd = p->mEnd;
	p->mStart = p->mStart;


	return p;
}



