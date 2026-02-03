/*!
@file dkcCircularMemoryStream.h
@since 2004/3/xx
@brief 環状バッファー boost::circular_streamみたいなもの　かな？
@note
*/
#ifndef DKUTIL_C_CIRCULAR_MEMORYSTREAM_H
#define DKUTIL_C_CIRCULAR_MEMORYSTREAM_H


#include "dkcMemoryStream.h"


enum edkcCirculerMemoryStream{
	///今の位置からシーク
	edkcCirculerMemoryStreamSeekCurrent = edkcSeekCurrent,
	///最後の位置からシーク
	edkcCirculerMemoryStreamSeekEnd = edkcSeekEnd,
	///最初の位置からシーク
	edkcCirculerMemoryStreamSeekSet = edkcSeekSet,


};
/*
enum edkcCircularMemoryStream{
	edkcCircularMemoryStreamInitBuffer = edkcStreamInitBuffer,
	edkcCircularMemoryStreamInitFile = edkcStreamInitFile,
	///無効な領域を含めたすべての内部バッファをダンプする。
	edkcCircularMemoryStreamDumpAll,
	///有効な領域のみの内部バッファをダンプする。
	edkcCircularMemoryStreamDumpEnable,

};
*/
/*!
 円型ストリーム（リングバッファデータ）構造体
 */
typedef struct dkc_CircularMemoryStream{
	/// データ領域のストリーム
	BYTE *mBuffer;
	/// リングバッファの大きさ 
	size_t mSize ;
	/// 有効データの始まりと終り
	size_t mStart,mEnd;		
	/// 有効なデータの長さ
	size_t mEnableLength ;

} DKC_CIRCULAR_MEMORYSTREAM;

DKC_EXTERN DKC_CIRCULAR_MEMORYSTREAM * WINAPI dkcAllocCircularMemoryStream(size_t size);

DKC_EXTERN int WINAPI dkcFreeCircularMemoryStream(DKC_CIRCULAR_MEMORYSTREAM **ptr);

/*!
@param readsize[out] 実際に読んだサイズを返す。NULLを渡してもOK.
@note
・指定したサイズ(size)よりもストリームバッファのサイズが小さかった場合
readsizeに実際に読んだサイズを入れます。
・Readしたデータは取り出したデータとして処理され、
そのバッファ内容は次のデータを保存する領域として再利用されます。
つまり、一度Readしたデータは二度と取り出せません。
*/
///streamをリードする。freadみたいなもの
DKC_EXTERN int WINAPI dkcCircularMemoryStreamRead(DKC_CIRCULAR_MEMORYSTREAM *ptr,void *buffer,size_t size,size_t *readsize);

///中を覗き見るだけ。引数についてはdkcCircularMemoryStreamRead()を見るべし。
DKC_EXTERN int WINAPI dkcCircularMemoryStreamReference(DKC_CIRCULAR_MEMORYSTREAM *ptr,void *buffer,size_t size,size_t *readsize);

/*!
@note
内部ストリームバッファは円型です。いわいるRingBufferって奴です。
dkcCircularMemoryStreamWriteしたらdkcCircularMemoryStreamReadしてデータを出してあげましょう。
そうしなければ、いずれ、円型バッファの有効データが一杯になってこの関数は失敗します。
C言語版 boost::circular_bufferみたいなものですね＾＾；（といいますか、また車輪の再開発してるし･･･。

*/
///streamをライトする。fwriteみたいなもの
DKC_EXTERN int WINAPI dkcCircularMemoryStreamWrite(DKC_CIRCULAR_MEMORYSTREAM *ptr,const void *buffer,size_t size);

///stream内のカウンタを０にして事実上すべてクリアーにする。
DKC_EXTERN int WINAPI dkcCircularMemoryStreamClear(DKC_CIRCULAR_MEMORYSTREAM *ptr);

/*!
DKC_CIRCULAR_MEMORYSTREAMをコピーする。
C++で言うコピーコンストラクタみたいなもの＾＾；
*/
DKC_EXTERN DKC_CIRCULAR_MEMORYSTREAM *WINAPI 
	dkcAllocCircularMemoryStreamCopy(const DKC_CIRCULAR_MEMORYSTREAM *ptr);

DKC_INLINE size_t dkcCirculerMemoryStreamTell(DKC_CIRCULAR_MEMORYSTREAM *ptr)
{
	return ptr->mStart;
}
DKC_INLINE size_t dkcCirculerMemoryStreamSize(DKC_CIRCULAR_MEMORYSTREAM *ptr)
{
	return ptr->mEnableLength;
}
DKC_INLINE size_t dkcCirculerMemoryStreamEOF(DKC_CIRCULAR_MEMORYSTREAM *ptr)
{
	return ptr->mSize == ptr->mEnd;
}

///todo test まだテストして無いよ＾＾；
DKC_INLINE int dkcCirculerMemoryStreamSeek(DKC_CIRCULAR_MEMORYSTREAM *ptr,long offset,long origin)
{
	long point;
	size_t change;
	switch(origin){
	case edkcCirculerMemoryStreamSeekCurrent:
		point = ptr->mStart;
		break;
	case edkcCirculerMemoryStreamSeekEnd:
		point = ptr->mEnd;
		break;
	case edkcCirculerMemoryStreamSeekSet:
		point = 0;
		break;
	default:
		return edk_ArgumentException;
	}
	//有効なリングバッファ内に収まるサイズになるように
	change = offset % ptr->mEnableLength;
	change = (point + change);

	if(TRUE==dkcCheckOverflow32(ptr->mStart ,change)){
		return edk_FAILED;
	}
	/*//if(!(ptr->mSize > change)){
	if(!(ptr->mSize >= change)){
		return edk_FAILED;
	}*/
	if( ptr->mStart + change > ptr->mEnd)
	{
		return edk_FAILED;
	}
	ptr->mSize += change;
	return edk_SUCCEEDED;
}

//stream内のものをバッファにコピーする。
//DKC_EXTERN int WINAPI dkcCircularMemoryStreamToBuffer(
//	const DKC_CIRCULAR_MEMORYSTREAM *ptr,char *buff,size_t size,size_t want_size);

/*
#ifndef DKUTIL_C_CIRCULAR_MEMORYSTREAM_H
#	include "dkcCircularMemoryStream.c"
#endif
*/

#endif