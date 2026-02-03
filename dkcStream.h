/**
@file dkcStream.h
@note
ファイルストリームとメモリストリームのアダプターシステム
エンディアン系を考慮したメモリシステム

DKC_STREAM構造体
BYTE DKC_STREAM::mSize個分。



*/

#ifndef DKUTIL_C_STREAM_H
#define DKUTIL_C_STREAM_H


#include "dkcOSIndependent.h"
#include "dkcMemoryStream.h"


///汎用型ストリーム
typedef struct dkc_Stream{
	///edkcStreamのInit指定モード入れ
	UINT mMode;
	///endian を チェンジするならばTRUE
	BYTE mChangeEndian;
	///プロセスの詳細
	BYTE mProcessMode;
	/*union{
		DKC_MEMORYSTREAM *mmem;
		FILE *mfp;
	}uni;*/
	void *mSig;
}DKC_STREAM;

//#error streamのSEEKにRead用とWrite用をつける。


enum edkcStream{
	///今の位置からシーク
	edkcStreamSeekCurrent = SEEK_CUR,
	///最後の位置からシーク
	edkcStreamSeekEnd = SEEK_END,
	///最初の位置からシーク
	edkcStreamSeekSet = SEEK_SET,
	///バッファとして初期化
	edkcStreamInitMemory = 1,
	///バッファはfopenで初期化
	edkcStreamInitFile = 2,
	///エンディアンデフォルト
	edkcStreamDefaultEndian = 4,
	///ビッグエンディアンモード
	edkcStreamBigEndian = 8,
	///リトルエンディアンモード
	edkcStreamLittleEndian = 16,
	///dkcStreamRead() dkcStreamWrite()の挙動 指定したサイズ分読み込めない場合は返す
	edkcStreamProcessDefault = 32,
	///dkcStreamRead() dkcStreamWrite()の挙動 指定したサイズ分 処理を実行する （エラーの時だけ返す)
	edkcStreamProcessAsOrdered = 64,
	///dkcStreamRead() dkcStreamWrite()の挙動
	///明示的なエンディアンチェンジが出来ない書き込み、系関数にて
	///エンディアンチェンジが必要だった時の場合はedk_Not_Satisfactoryを返すように指定
	edkcStreamWriteErrorWhenEndianChange = 128,
	
	///バッファ指定のストリームを外部のメモリ領域をセットしてアダプターとして使用するときに指定。
	///dkcMemoryStreamAdapter系列みたいな～。
	edkcStreamInitMemoryAdapter = 256,

	///64bit対応ファイルシステム
	edkcStreamInitFile64 = 512,
	
	edkcStreamWin32PromoteFlag = edkcStreamLittleEndian | edkcStreamProcessAsOrdered,
	edkcStreamMacPromoteFlag = edkcStreamBigEndian | edkcStreamProcessAsOrdered,

};

//**********************************************************


/**
@param flag[in] edkcStreamにある、初期化指定系のフラグを渡す(ストリームの種類とストリームのエンディアン)
@param default_data[in] ストリームに詰め込むバッファ別にNULLでもOK
@param size[in] ストリームの初期化時のサイズ
@param filename[in] edkcStreamInitFileをflagに渡した場合、その保存するファイルの名前
flagにedkcStreamInitFile以外の値を渡した場合はNULLでOK
@param mode[in] fopenのモード
flagにedkcStreamInitFile以外の値を渡した場合はNULLでOK
@return DKC_STREAM *のストリームデータ。
*/
///ストリーム領域を得る。
DKC_EXTERN DKC_STREAM * WINAPI dkcAllocStream(UINT flag,const void *default_data,size_t size,const char *filename,const char *mode);

DKC_EXTERN DKC_STREAM *WINAPI dkcAllocStreamMemoryType(UINT flag,const void *default_data,size_t size);

DKC_EXTERN DKC_STREAM *WINAPI dkcAllocStreamFileType(UINT flag,const char *filename,const char *mode);
/**
@param mode[in] edkc_FileModeのどれかを入れる。
*/
DKC_EXTERN DKC_STREAM *WINAPI dkcAllocStreamFile64Type(UINT flag,const char *filename,uint32 mode);

/**

@note
adapt_bufferに入れたメモリ領域は自動的に開放されませんので各自で開放してください。
*/
DKC_EXTERN DKC_STREAM *WINAPI dkcAllocStreamMemoryAdapterType(UINT flag,void *adapt_buffer,size_t size);
/**
@param OrderFlag[in] 指定した通りに読みこむタイプならTRUE (edkcStreamProcessAsOrdered)
@note
仕様：強制的に吐き出すファイルのエンディアンはリトルエンディアン
*/
///ファイル用ストリームの領域を得る時の奨励版の関数
DKC_EXTERN DKC_STREAM *WINAPI dkcAllocStreamEncouragementFileType(const char *filename,const char *mode,BOOL OrderFlag);

/*!
@param ptr[in][out] 削除するストリームデータ。入れたデータはNULLになる。
@note

*/
///dkcAllocNewStream()で確保したストリーム領域を開放する
DKC_EXTERN int WINAPI dkcFreeStream(DKC_STREAM **ptr);

//**********************************************************
///streamをシークする。fseekみたいなもの @return edk_SUCCEEDEDで成功　それ以外はエラー
DKC_EXTERN int WINAPI dkcStreamSeek(DKC_STREAM *ptr,int offset,int origin);

///streamの位置を取得する。 @return edkcStreamSeekSetからのオフセット値。edk_FAILEDだとエラー
DKC_EXTERN long WINAPI dkcStreamTell(DKC_STREAM *ptr);

///stream内をmemset(0)する。(seek位置も最初に戻される。
DKC_EXTERN int WINAPI dkcStreamClear(DKC_STREAM *ptr);


///ストリーム内をフラッシュ（完全に書きこむ処理）する。
///@return edk_Resultのどれか
DKC_EXTERN int WINAPI dkcStreamFlush(DKC_STREAM *ptr);

///@return EOFだとTRUE
DKC_EXTERN BOOL WINAPI dkcStreamEOF(DKC_STREAM *ptr);
///@return エラーだとTRUE
DKC_EXTERN BOOL WINAPI dkcStreamError(DKC_STREAM *ptr);

//**********************************************************
/*!
@param readsize[out] 実際に読んだサイズを返す。NULLを渡してもOK.
@note
指定したサイズ(size)よりもストリームバッファのサイズが小さかった場合
readsizeに実際に読んだサイズを入れます。


@return 
edkcStreamInitMemoryが指定されていた場合、
 dkcMemoryStreamRead()を見てください。
edkcStreamInitFileが指定されていた場合、
 ferror()だったらedk_FAILED , feof()だったらedk_BufferOverFlow,
edkcStreamWriteErrorWhenEndianChangeが指定されていた場合、
 edk_Not_Satisfactoryを返す。この場合はdkcStreamRead8()を使用する事
*/
///streamをリードする。freadみたいなもの
DKC_EXTERN int WINAPI dkcStreamRead(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);


/**
sizeは1byte単位･･･　ぶっちゃけdkcStreamRead()と同じ動作。
エンディアン問題に積極的に取り組む為に
edkcStreamWriteErrorWhenEndianChangeが指定されている場合は明示的にこちらを使用する。
*/
DKC_EXTERN int WINAPI dkcStreamRead8(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);


/**
@note
sizeは2byte単位でないといけない
@param flag[in] edkcStreamProcessAsOrdered 指定したサイズで読みこめない時は
	edkcStreamProcessDefault
@todo test
*/
DKC_EXTERN int WINAPI dkcStreamRead16(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);
/**
@note
sizeは4byte単位でないといけない
*/

DKC_EXTERN int WINAPI dkcStreamRead32(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);
/**
@note
sizeは8byte単位でないといけない
*/
DKC_EXTERN int WINAPI dkcStreamRead64(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);


//**********************************************************

///streamの中身を見る。
DKC_EXTERN int WINAPI dkcStreamRef(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);

/**

*/
DKC_EXTERN int WINAPI dkcStreamRef8(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);

DKC_EXTERN int WINAPI dkcStreamRef16(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);
/**
@note
sizeは4byte単位でないといけない
*/

DKC_EXTERN int WINAPI dkcStreamRef32(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);
/**
@note
sizeは8byte単位でないといけない
*/
DKC_EXTERN int WINAPI dkcStreamRef64(DKC_STREAM *ptr,void *buffer,size_t size,size_t *readsize);


//**********************************************************


/*!
@note
内部ストリームバッファは動的拡張です。気にせずどんどんぶち込みましょう。
C言語版のSTLのstd::vectorみたいなものですね。（といいますか、車輪の再開発してるし･･･。

*/
///streamをライトする。fwriteみたいなもの
DKC_EXTERN int WINAPI dkcStreamWriteWithWriteSize(DKC_STREAM *ptr,const void *buffer,size_t size,size_t *write_size);

//DKC_EXTERN int WINAPI dkcStreamWrite(DKC_STREAM *ptr,const void *buffer,size_t size);

DKC_INLINE int WINAPI dkcStreamWrite(DKC_STREAM *ptr,const void *buffer,size_t size)
{
	size_t write_size;
	return dkcStreamWriteWithWriteSize(ptr,buffer,size,&write_size);
	//if(write_size != size)		return edk_FAILED;
}

DKC_EXTERN int WINAPI dkcStreamWrite8(DKC_STREAM *ptr,void *buffer,size_t size);

DKC_EXTERN int WINAPI dkcStreamWrite16(DKC_STREAM *ptr,const void *buffer,size_t size);

DKC_EXTERN int WINAPI dkcStreamWrite32(DKC_STREAM *ptr,const void *buffer,size_t size);

DKC_EXTERN int WINAPI dkcStreamWrite64(DKC_STREAM *ptr,const void *buffer,size_t size);


//**********************************************************

/*!
@param p[in] DKC_STREAM *が入る
@param buffer[in][out] 場合によってin、outになる。バッファへのポインタ
@param size[in] bufferのサイズ
@param data[in] 独自データへのポインタを受け取る
*/
typedef int (WINAPI *DKC_STREAM_PROCESS_TYPE)(DKC_STREAM *p,void *buffer,size_t size,void *data);


DKC_EXTERN int WINAPI dkcStreamProcess(DKC_STREAM *ptr,void *buffer,size_t size,
																			 DKC_STREAM_PROCESS_TYPE write_t,void *data);

DKC_EXTERN int WINAPI dkcStreamProcess16(DKC_STREAM *ptr,void *buffer,size_t size,
																			 DKC_STREAM_PROCESS_TYPE write_t,void *data);

DKC_EXTERN int WINAPI dkcStreamProcess32(DKC_STREAM *ptr,void *buffer,size_t size,
																			 DKC_STREAM_PROCESS_TYPE write_t,void *data);

DKC_EXTERN int WINAPI dkcStreamProcess64(DKC_STREAM *ptr,void *buffer,size_t size,
																			 DKC_STREAM_PROCESS_TYPE write_t,void *data);






#endif
