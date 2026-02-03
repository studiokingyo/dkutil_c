/*!
@file dkcMemoryStream.h
@note
dkcMemoryStreamSerializeのシリアライズ済みファイルデータ構造

DKC_MEMORYSTREAM構造体
BYTE DKC_MEMORYSTREAM::mSize個分。
@brief 可変長な配列をぶち込める。std::vectorみたいなもの。


*/

#ifndef DKUTIL_C_MEMORYSTREAM_H
#define DKUTIL_C_MEMORYSTREAM_H


#include "dkcOSIndependent.h"

#include "dkcDefined.h"
#include "dkcBuffer.h"

/*!
メモリストリーム構造体。fopen fwrite freadをメモリ城で行うようなストリーム。
C言語でstd::vectorみたいな感じで使う。
*/
typedef struct dkc_MemoryStream{
	///バッファ
	BYTE *mBuffer;
	///バッファのサイズ
	size_t mSize;
	///今のオフセット
	size_t mNowOffset;
	
}DKC_MEMORYSTREAM;





enum edkcMemoryStream{
	///今の位置からシーク
	edkcMemoryStreamSeekCurrent = edkcSeekCurrent,
	///最後の位置からシーク
	edkcMemoryStreamSeekEnd = edkcSeekEnd,
	///最初の位置からシーク
	edkcMemoryStreamSeekSet = edkcSeekSet,


};



/**
@param flag[in] edkcMemoryStreamにある、StreamInit系のものを渡す
@param size[in] ストリームの初期化時のサイズ
@param filename[in] edkcMemoryStreamInitFileをflagに渡した場合、その保存するファイルの名前
flagにedkcMemoryStreamInitFile以外の値を渡した場合はNULLでOK
@param mode[in] fopenのモード
flagにedkcMemoryStreamInitFile以外の値を渡した場合はNULLでOK
@return DKC_MEMORYSTREAM *のストリームデータ。
*/
///ストリーム領域を得る。
DKC_EXTERN DKC_MEMORYSTREAM * WINAPI dkcAllocMemoryStream(size_t size);


/*!
@param ptr[in][out] 削除するストリームデータ。入れたデータはNULLになる。
*/
///dkcAllocNewStream()で確保したストリーム領域を開放する
DKC_EXTERN int WINAPI dkcFreeMemoryStream(DKC_MEMORYSTREAM **ptr);


///streamをシークする。fseekみたいなもの
DKC_EXTERN int WINAPI dkcMemoryStreamSeek(DKC_MEMORYSTREAM *ptr,int offset,int origin);

///@return streamのシークポイントを返す。
DKC_EXTERN size_t WINAPI dkcMemoryStreamGetSeekPoint(DKC_MEMORYSTREAM *ptr);

#define dkcMemoryStreamTell(tt) dkcMemoryStreamGetSeekPoint(tt)

/*!
@param readsize[out] 実際に読んだサイズを返す。NULLを渡してもOK.
@note
指定したサイズ(size)よりもストリームバッファのサイズが小さかった場合
readsizeに実際に読んだサイズを入れます。
*/
///streamをリードする。freadみたいなもの
DKC_EXTERN int WINAPI dkcMemoryStreamRead(DKC_MEMORYSTREAM *ptr,void *buffer,size_t size,size_t *readsize);
/*!
@note
内部ストリームバッファは動的拡張です。気にせずどんどんぶち込みましょう。
C言語版のSTLのstd::vectorみたいなものですね。（といいますか、車輪の再開発してるし･･･。

*/
///streamをライトする。fwriteみたいなもの
DKC_EXTERN int WINAPI dkcMemoryStreamWrite(DKC_MEMORYSTREAM *ptr,const void *buffer,size_t size);

///streamをライトする。ライト出来なかったら内部で動的拡張する。
DKC_EXTERN int WINAPI dkcMemoryStreamDynamicWrite(DKC_MEMORYSTREAM *ptr,const void *buffer,size_t size);

///streamのバッファを拡張する。
DKC_EXTERN int WINAPI dkcMemoryStreamResize(DKC_MEMORYSTREAM *ptr,size_t want_size);
///@return 今、どこまで書きこんでいるかのサイズを取得
DKC_EXTERN size_t WINAPI dkcMemoryStreamNowOffset(DKC_MEMORYSTREAM *p);
///@return 内部バッファへのポインタを取得
DKC_EXTERN BYTE *WINAPI dkcMemoryStreamPointer(DKC_MEMORYSTREAM *p);



///stream内をmemset(0)する。(seek位置も最初に戻される。
DKC_EXTERN int WINAPI dkcMemoryStreamClear(DKC_MEMORYSTREAM *ptr);


/*!
@return 成功したらそのオブジェクトへのポインタが返る。失敗したらNULLが返る。
@note
C++で言うコピーコンストラクタ。
*/

DKC_EXTERN DKC_MEMORYSTREAM * WINAPI dkcAllocMemoryStreamCopy(const DKC_MEMORYSTREAM *ptr);


//DKC_EXTERN int WINAPI dkcMemoryStreamPushBack(DKC_MEMORYSTREAM *p,const BYTE *buff,size_t size);

///バッファオーバーフローする可能性があります。内部構造を知らないと危険です。
#define dkcMemoryStreamPushBackMacro(type,p,x) {\
	*((type *)&( (p)->mBuff[(p)->mNowOffset])) = (type)x;(p)->mNowOffset += sizeof(type);\
}

#define dkcMemoryStreamIsEnd(p) ( (p)->mNowOffset >= (p)->mSize )

DKC_EXTERN int WINAPI dkcMemoryStreamPopBack(DKC_MEMORYSTREAM *p,size_t size);

//**********************************************************
//memory stream adapter

#if 0
typedef struct dkc_MemoryStreamAdapter{
	BYTE *mBuffer;
	size_t mSize;
	size_t mNowOffset;
}DKC_MEMORYSTREAM_ADAPTER;
#else
///同じ構造だったのでtypedefで十分
typedef DKC_MEMORYSTREAM DKC_MEMORYSTREAM_ADAPTER;
#endif

DKC_EXTERN void WINAPI dkcMemoryStreamAdapterInit(DKC_MEMORYSTREAM_ADAPTER *p,BYTE *buff,size_t size);

DKC_EXTERN DKC_MEMORYSTREAM_ADAPTER *WINAPI dkcAllocMemoryStreamAdapter(BYTE *buff,size_t size);

DKC_EXTERN int WINAPI dkcFreeMemoryStreamAdapter(DKC_MEMORYSTREAM_ADAPTER **);

DKC_EXTERN DKC_MEMORYSTREAM_ADAPTER *dkcAllocMemoryStreamAdapterCopy(DKC_MEMORYSTREAM_ADAPTER *);


#define dkcMemoryStreamAdapterWrite(a,b,c) dkcMemoryStreamWrite(a,b,c)
#define dkcMemoryStreamAdapterRead(a,b,c,d) dkcMemoryStreamRead(a,b,c,d)
#define dkcMemroyStreamAdapterSeek(a,b,c) dkcMemoryStreamSeek(a,b,c)
#define dkcMemoryStreamAdapterTell(a) dkcMemoryStreamTell(a)
#define dkcMemoryStreamAdapterClear(a) dkcMemoryStreamClear(a)
/*
DKC_EXTERN BYTE *WINAPI dkcMemoryStreamAdapterPointer(DKC_MEMORYSTREAM_ADAPTER *p);

DKC_EXTERN size_t WINAPI dkcMemoryStreamAdapterSize(DKC_MEMORYSTREAM_ADAPTER *p);

DKC_EXTERN int WINAPI dkcMemoryStreamAdapterSetOffset(DKC_MEMORYSTREAM_ADAPTER *ptr,size_t offset);

DKC_EXTERN size_t WINAPI dkcMemoryStreamAdapterGetOffset(DKC_MEMORYSTREAM_ADAPTER *p);

DKC_EXTERN int WINAPI dkcMemoryStreamAdapterRead(DKC_MEMORYSTREAM_ADAPTER *p,void *buff,size_t buffsize,size_t *readsize);


///@see dkcMemoryStreamAdapterPushBack()
#define dkcMemoryStreamAdapterWrite(p,buff,write_size) dkcMemoryStreamAdapterPushBack(p,buff,write_size)

DKC_EXTERN int WINAPI dkcMemoryStreamAdapterGetChar(DKC_MEMORYSTREAM_ADAPTER *p,BYTE *t);
*/
//**********************************************************
//stream generic macro
/*
///メモリストリーム系のバッファをファイルをダンプします。
#define dkcmMemoryStreamDump(ptr,filename,buffer_all) {\
	if(buffer_all){\
		dkcSaveBinary(ptr->mBuffer,ptr->mSize,filename);\
	}else{\
		dkcSaveBinary(ptr->mBuffer,ptr->mNowOffset,filename);\
	}\
}
*/
enum{
	edkcDumpRegistBufferAll = 0,
	edkcDumpNowOffset = 1,
};
DKC_EXTERN int WINAPI dkcMemoryStreamDump(DKC_MEMORYSTREAM *ptr,const char *filename,UINT flag);

///メモリストリーム内のバッファをbufferにコピーする @param flag[in] 
DKC_EXTERN int WINAPI dkcMemoryStreamWriteToMemory(const DKC_MEMORYSTREAM *ptr,void *buffer,size_t buffsize,UINT flag);
///bufferからmemorystreamに読みこむ
DKC_EXTERN int WINAPI dkcMemoryStreamLoadFromMemory( DKC_MEMORYSTREAM *ptr,const void *buffer,size_t buffsize);
 ///ファイルからmemorystreamに読みこむ
DKC_EXTERN int WINAPI dkcMemoryStreamLoadFromFile(DKC_MEMORYSTREAM *ptr,const char *filename,size_t permit_size,UINT flag);

DKC_EXTERN int WINAPI dkcMemoryStreamGet8(DKC_MEMORYSTREAM *ptr,BYTE *t);
DKC_EXTERN int WINAPI dkcMemoryStreamPut8(DKC_MEMORYSTREAM *Ptr,BYTE t);


#endif
