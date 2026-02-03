#ifndef DKUTIL_C_CIRCULAR_STREAM_H
#define DKUTIL_C_CIRCULAR_STREAM_H


#include "dkcStream.h"

#if 0
enum edkcCircularStream{
	edkcCircularStreamInitMemory = edkcStreamInitMemory,
	edkcCircularStreamInitFile = edkcStreamInitFile,
	///無効な領域を含めたすべての内部バッファをダンプする。
	edkcCircularStreamDumpAll,
	///有効な領域のみの内部バッファをダンプする。
	edkcCircularStreamDumpEnable,

};

/// 円型ストリーム（リングバッファデータ）構造体
typedef struct dkc_CircularStream{
	/// データ領域のストリーム
	DKC_STREAM *mStream;
	/// 有効データの始まりと終り
	size_t mStart,mEnd;		
	/// 有効なデータの長さ
	size_t mEnableLength ;
	/// リングバッファの大きさ 
	size_t mSize ;
	///モード
	short mMode;
} DKC_CIRCULARSTREAM;

DKC_EXTERN DKC_CIRCULARSTREAM * WINAPI dkcAllocNewCircularStream(size_t size,short mode,const char *filename,const char *openmode);

DKC_EXTERN int WINAPI dkcFreeDeleteCircularStream(DKC_CIRCULARSTREAM **ptr);

DKC_EXTERN int WINAPI dkcNewCircularStream(DKC_CIRCULARSTREAM *ptr,size_t size);

DKC_EXTERN int WINAPI dkcNewCircularStreamForFile(DKC_CIRCULARSTREAM *ptr,size_t size,const char *filename,const char *openmode);

///circular streamをデリート fcloseみたいなもの　@note 必ずストリームを使用したあとはこれを呼んでください。
DKC_EXTERN int WINAPI dkcDeleteCircularStream(DKC_CIRCULARSTREAM *ptr);


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
DKC_EXTERN int WINAPI dkcCircularStreamRead(DKC_CIRCULARSTREAM *ptr,void *buffer,size_t size,size_t *readsize);

///中を覗き見るだけ。引数についてはdkcCircularStreamRead()を見るべし。
DKC_EXTERN int WINAPI dkcCircularStreamReference(DKC_CIRCULARSTREAM *ptr,void *buffer,size_t size,size_t *readsize);

/*!
@note
内部ストリームバッファは円型です。いわいるRingBufferって奴です。
dkcCircularStreamWriteしたらdkcCircularStreamReadしてデータを出してあげましょう。
そうしなければ、いずれ、円型バッファの有効データが一杯になってこの関数は失敗します。
C言語版 boost::circular_bufferみたいなものですね＾＾；（といいますか、また車輪の再開発してるし･･･。

*/
///streamをライトする。fwriteみたいなもの
DKC_EXTERN int WINAPI dkcCircularStreamWrite(DKC_CIRCULARSTREAM *ptr,const void *buffer,size_t size);


/*!
@note
ファイルとしてストリームを初期化したのならばこの関数は失敗する
*/
///streamをファイルにダンプする。
DKC_EXTERN int WINAPI dkcCircularStreamDump(DKC_CIRCULARSTREAM *ptr,const char *filename,int flag);
///stream内をmemset(0)する。
DKC_EXTERN int WINAPI dkcCircularStreamZero(DKC_CIRCULARSTREAM *ptr);
///stream内のものをバッファにコピーする。
DKC_EXTERN int WINAPI dkcCircularStreamToBuffer(const DKC_STREAM *ptr,char *buff,size_t size,size_t want_size);

#ifndef DKUTIL_C_CIRCULAR_MEMORYSTREAM_C
#	include "dkcCircularStream.h"
#endif

#endif //eo0

#endif