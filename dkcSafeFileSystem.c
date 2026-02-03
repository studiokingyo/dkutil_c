
/*!
@file dkcSafeFileSystem.c
@brief  改竄防止機能付き ファイルシステム
@author d金魚
*/
#define DKUTIL_C_SAFE_FILESYSTEM_C
#include "dkcSafeFileSystem.h"
#include "dkcMath.h"
#include "dkcStdio.h"

static DKC_INLINE int HeaderWrite(DKC_STREAM *p,DKC_FILE_HEADER_FILE_WITH_SIGNATURE  *header)
{
	BYTE isLE = (BYTE)dkcIsLittleEndian();
	if(header->little_endian != isLE)
	{//ココバグっているかも･･･。
		header->sig = dkcReverseEndian32(header->sig);
		header->filesize = dkcReverseEndian64(header->filesize);
	}
	return dkcStreamWrite(p,header,sizeof(DKC_FILE_HEADER_FILE_WITH_SIGNATURE));
}


static DKC_INLINE int GetHeader(
	DKC_FILE_HEADER_FILE_WITH_SIGNATURE *header,
	const char *filename)
{
	size_t readsize = 0;
	size_t mustsize = sizeof(DKC_FILE_HEADER_FILE_WITH_SIGNATURE);
	int r = edk_FAILED;
	FILE *fp = NULL;
	BYTE isLE = (BYTE)dkcIsLittleEndian();
	//BYTE get_flag;
	const char *mode = "rb";

	fp = dkcFOpen(filename,mode);
	if(NULL==fp){
		goto Exit;
	}
	fseek(fp,0,SEEK_SET);
	//readsize = fread(header,1,mustsize,fp);
	readsize = dkcFReadAll(header,mustsize,fp);
	if(mustsize != readsize){
		goto Exit;
	}

	if(header->little_endian != isLE)
	{//エンディアンが違ったらリバース
		header->sig = dkcReverseEndian32(header->sig);
		header->filesize = dkcReverseEndian64(header->filesize);
	}

	
	r = edk_SUCCEEDED;
Exit:
	dkcFClose(&fp);
	return r;
	/*
	DKC_STREAM *fsp;

	//読み込み時
	fsp = dkcAllocStreamFileType(edkcStreamRead,filename,mode);
	if(NULL==fsp){
		return edk_FAILED;
	}
	r = dkcStreamRead(fsp,header,mustsize,&readsize);
	if(DKUTIL_FAILED(r)){
		return r;
	}
	if(readsize != mustsize){
		return edk_FAILED;
	]
	dkcFreeStream(&fsp);
	*/
}


//flagにはエンディアンを
/*
int WINAPI dkcFileWithSignatureCheckCheatFile(UINT flag,const char *filename){
	//チェックオブジェクト
	DKC_STREAM *sp;
	DKC_SHA1 *sha;
	//戻り値
	int r;
	//バッファとか
	BYTE buff[2048];
	size_t i;

	flag |= edkcStreamProcessAsOrdered;
	
	sp = dkcAllocStreamFileType(flag,filename,"rb");
	
	if(NULL==sp){
		return edk_FAILED;
	}
	sha = dkcAllocSHA1();
	if(NULL==sha){
		goto Exit;
	}
	
	dkcStreamRead(sp,buff,sizeof(buff),
Exit:
	dkcFreeSHA1(&sha);
	dkcFreeStream(&sp);
	return r;
}*/

#ifdef _MSC_VER
#	pragma warning(disable:4701)
#endif

DKC_FILE_WITH_SIGNATURE *WINAPI dkcOpenFileWithSignature(
/*
	UINT flag,size_t size,
	const char *filename,const char *mode,
	const void *dustbin,size_t dustbin_size
*/
	UINT stream_flag,
	const char *filename,const char *mode,
	const void *dustbin,size_t dustbin_size,
	UINT signature
)
{
	DKC_STREAM *sp = NULL;
	void *tp = NULL;
	DKC_BUFFER *mbp = NULL;
	DKC_FILE_WITH_SIGNATURE *p = NULL;
	
	DKC_FILE_HEADER_FILE_WITH_SIGNATURE header;

	BOOL isWrite = FALSE;
	BYTE isLE = (BYTE)dkcIsLittleEndian();

	//UINT stream_flag = edkcStreamInitFile;
	
	//フラグの変更
	stream_flag |= edkcStreamInitFile;
	
	DKUTIL_FLAG_DOWN(stream_flag,edkcStreamInitMemory);
	
	DKUTIL_FLAG_DOWN(stream_flag,edkcStreamProcessDefault);

	DKUTIL_FLAG_UP(stream_flag,edkcStreamProcessAsOrdered);

	//size_t stream_size;


	//エラーチェック
	if(NULL==dustbin ||  0 == dustbin_size){
		return NULL;
	}

	if(NULL==mode || NULL==filename){
		return NULL;
	}
	if(NULL != strchr(mode,'a')){//共通モードでは開けない
		return NULL;
	}
	if(NULL != strchr(mode,'t')){//テキストモードでは開けない。
		return NULL;
	}


	//領域取得

	p = (DKC_FILE_WITH_SIGNATURE *)dkcAllocate(sizeof(DKC_FILE_WITH_SIGNATURE));
	if(NULL==p){
		return NULL;
	}
	tp = (DKC_SHA1 *)dkcAllocSHA1();
	if(NULL==tp){
		goto Error;
	}

	//ライトモードかどうか。
	isWrite = (NULL==strchr(mode,'r'));

	if(isWrite){
		//書き込みモードの時
		p->mWriteMode = TRUE;	
	}else{
		if(DKUTIL_FAILED(GetHeader(&header,filename))){
			goto Error;
		}

		if(header.little_endian){
			stream_flag |= edkcStreamLittleEndian;
		}else{
			stream_flag |= edkcStreamBigEndian;
		}

		p->mWriteMode = FALSE;
		//とりあえず、コピーしておかないと支障が出る。
		memcpy(&(p->mFileHeader),&header,sizeof(header));
	}


	sp = dkcAllocStreamFileType( stream_flag,filename,mode);
	if(NULL==sp){
		goto Error;
	}
	//update
	p->mStream = sp;

	if(isWrite){
		//ヘッダ分を書き込んでおく。
		memset(&header,0,sizeof(header));
		dkcStreamWrite(sp,&header,sizeof(header));


		//ヘッダの設定

		//エンディアン設定
		if(isLE){
			if(FALSE==p->mStream->mChangeEndian){
				p->mFileHeader.little_endian = TRUE;
			}else{
				p->mFileHeader.little_endian = FALSE;
			}
		}else{
			if(TRUE==p->mStream->mChangeEndian){
				p->mFileHeader.little_endian = TRUE;
			}else{
				p->mFileHeader.little_endian = FALSE;
			}
		}
		//シグネチャを設定
		p->mFileHeader.sig = signature;
	}

	mbp = dkcAllocBuffer(dustbin,dustbin_size);
	if(NULL==mbp){
		goto Error;
	}



	p->mSHA1 = (DKC_SHA1 *)tp;
	p->mDustbin = mbp;

	//とりあえず、ゴミ値をSHA1にロード
	dkcSHA1Load(p->mSHA1,(const BYTE *)dustbin,dustbin_size);

	if(isWrite==FALSE){
		//ストリームをデータ部にシーク
		dkcStreamSeek(p->mStream,sizeof(DKC_FILE_HEADER_FILE_WITH_SIGNATURE),edkcStreamSeekSet);
	}
		

	return p;
Error:
	dkcFreeStream(&sp);
	dkcFreeSHA1((DKC_SHA1 **)&tp);
	dkcFree((void **)&p);
	return NULL;
}

#ifdef _MSC_VER
#	pragma warning(default:4701)
#endif

///@return SHAがFinalizedしていたらTRUE
static DKC_INLINE BOOL FileWithSignatureIsFinalized(DKC_FILE_WITH_SIGNATURE *ptr){
	return ptr->mSHA1->mFinalized;
}

///SHAをfinalizeしてハッシュ値を取得
static DKC_INLINE int FileWithSignatureGetDigest(DKC_FILE_WITH_SIGNATURE *ptr
																								 //,BYTE *buff,size_t size
																								 )
{
	dkcSHA1Load(
		ptr->mSHA1,
		dkcBufferPointer(ptr->mDustbin),
		dkcBufferSize(ptr->mDustbin)
	);
	return dkcSHA1FinalDigest(
		ptr->mSHA1,
		ptr->mFileHeader.hash_value,
		sizeof(ptr->mFileHeader.hash_value)
	);
}

int WINAPI dkcCloseFileWithSignature(DKC_FILE_WITH_SIGNATURE **p){
	DKC_FILE_WITH_SIGNATURE *ptr;
	//BYTE buff[SHA1_BUFFER_SIZE];

	//size_t offset = SM_OFFSET(DKC_FILE_HEADER_FILE_WITH_SIGNATURE,hash_value);

	if(NULL==p){
		return edk_FAILED;
	}

	ptr = *p;
	
	if(ptr->mWriteMode)
	{//書き込みモード時のみ
		
		//メッセージダイジェストを取得
		//FileWithSignatureGetDigest(ptr,buff,sizeof(buff));
		FileWithSignatureGetDigest(ptr);
		//ファイルサイズ取得
		ptr->mFileHeader.filesize = (ULONGLONG)dkcStreamTell(ptr->mStream);
		//ptr->mFileHeader.datasize = ptr->mFileHeader.filesize - sizeof(DKC_FILE_HEADER_FILE_WITH_SIGNATURE);

		//メッセージダイジェスト領域の位置を取得
		dkcStreamSeek(ptr->mStream,0,edkcStreamSeekSet);
		//メッセージダイジェストを書き込む
		//dkcStreamWrite(ptr->mStream,buff,sizeof(buff));
		HeaderWrite(ptr->mStream,&(ptr->mFileHeader));
	}
	dkcFreeBuffer(&(ptr->mDustbin));

	dkcFreeStream(&(ptr->mStream));
	
	dkcFreeSHA1(&(ptr->mSHA1));

	return dkcFree((void **)p);
}

typedef int (WINAPI *CALLBACK_WRITE_F_T)(DKC_STREAM *,const void *,size_t);

typedef struct FileWithSigCallBackSend{
	DKC_FILE_WITH_SIGNATURE *psig;
	size_t readsize;
	CALLBACK_WRITE_F_T callback;
}FWS_CALLBACK_SEND;

static int WINAPI WriteCallback(DKC_STREAM *ptr,void *buffer,size_t size,void *data){
	int r;
	FWS_CALLBACK_SEND *pfws = (FWS_CALLBACK_SEND *)data;
	CALLBACK_WRITE_F_T callback = pfws->callback;

	r =	callback(ptr,buffer,size);
	
	if(DKUTIL_FAILED(r)){
		return r;
	}
	dkcSHA1Load(pfws->psig->mSHA1,(const BYTE *)buffer,size);
	return r;
}

static int WINAPI ReadCallback(DKC_STREAM *ptr,void *buffer,size_t size,void *data){

	int r;
	FWS_CALLBACK_SEND *pfws = (FWS_CALLBACK_SEND *)data;
	size_t readsize;
	
	r = dkcStreamRead(ptr,buffer,size,&readsize);
	(pfws->readsize) = readsize;

	if(DKUTIL_FAILED(r)){
		return r;
	}
	dkcSHA1Load(pfws->psig->mSHA1,(const BYTE *)buffer,readsize);

	return r;
}


static DKC_INLINE int FileWithSignatureReadLogic(DKC_FILE_WITH_SIGNATURE *p,
																		void *data,size_t size,size_t *readsize){
	FWS_CALLBACK_SEND send;
	int r;
	send.callback = NULL;
	send.psig = p;
	send.readsize = 0;
	r = dkcStreamProcess(p->mStream,data,size,ReadCallback,(void *)&send);																	
	if(readsize){
		*readsize = send.readsize;
	}
	return r;
	/*
	int r;
	size_t readsize_ = 0;
	if(NULL==readsize){
		readsize = &readsize_;
	}
	
	r = dkcStreamRead(p->mStream,data,size,readsize);
	if(DKUTIL_FAILED(r)){
		return r;
	}
	dkcSHA1Load(p->mSHA1,data,*readsize);
	return r;
	*/
}

static DKC_INLINE int FileWithSignatureWriteLogic(DKC_FILE_WITH_SIGNATURE *p,
																		 const void *data,size_t size,CALLBACK_WRITE_F_T callback)
{
	//return WriteCallback(p->mStream,data,size,&send);
	/*int r;
	r =	callback(p->mStream,data,size);
	if(DKUTIL_FAILED(r)){
		return r;
	}
	dkcSHA1Load(p->mSHA1,data,size);
	
	return r;
	*/
}


static DKC_INLINE int ReadEOFCheck(DKC_FILE_WITH_SIGNATURE *p){
	ULONGLONG filesize = p->mFileHeader.filesize;
	ULONGLONG stream_size = dkcStreamTell(p->mStream);
	if(filesize == stream_size || TRUE==p->mAllLoad)
	{
		p->mAllLoad = TRUE;
		return edk_EndProcess;
	}
	return edk_SUCCEEDED;
}

int WINAPI dkcFileWithSignatureRead(DKC_FILE_WITH_SIGNATURE *p,
																		void *data,size_t size,size_t *readsize){
	size_t tsize;
	ULONGLONG filesize = p->mFileHeader.filesize;
	ULONGLONG stream_size = dkcStreamTell(p->mStream);
	int r = edk_FAILED;

	if(readsize){
		//とりあえず、明示的にまだ読み込んでいない事を告げる。
		*readsize = 0;
	}

	if(p->mWriteMode){//書き込みモードじゃねぇかよ。
		return edk_LogicError;
	}

	if(filesize == stream_size || TRUE==p->mAllLoad)
	{
		p->mAllLoad = TRUE;
		return edk_EndProcess;
	}
	//ファイルに冗長なモノが引っ付いている。
	if(filesize < stream_size){
		p->mAllLoad = TRUE;
		return edk_FileCheated_Addition;
	}
	//読み取りサイズをフィルターする
	if(stream_size + size > filesize){
		tsize = (size_t)(ULONGLONG)(filesize - stream_size);
	}else{
		tsize = size;
	}
	
	r = FileWithSignatureReadLogic(
		p,data,tsize,readsize);
	

	if(DKUTIL_FAILED(r)){
		return r;
	}
		
	return ReadEOFCheck(p);
	//return dkcStreamRead(p->mStream,data,tsize,readsize);
}


int WINAPI dkcFileWithSignatureWrite(DKC_FILE_WITH_SIGNATURE *p,
																		 const void *data,size_t size)
{
	FWS_CALLBACK_SEND send;
	send.callback = dkcStreamWrite;
	send.psig = p;
	send.readsize = 0;
	return dkcStreamProcess(p->mStream,(void *)data,size,WriteCallback,&send);
	//return FileWithSignatureWriteLogic(p,data,size,dkcStreamWrite);

}

int WINAPI dkcFileWithSignatureWrite16(DKC_FILE_WITH_SIGNATURE *p,
																			 const void *data,size_t size)
{
	FWS_CALLBACK_SEND send;
	send.callback = dkcStreamWrite16;
	send.psig = p;
	send.readsize = 0;
	return dkcStreamProcess(p->mStream,(void *)data,size,WriteCallback,&send);
	//return FileWithSignatureWriteLogic(p,data,size,dkcStreamWrite16);
}



int WINAPI dkcFileWithSignatureWrite32(DKC_FILE_WITH_SIGNATURE *p,
																								const void *data,size_t size)
{
	FWS_CALLBACK_SEND send;
	send.callback = dkcStreamWrite32;
	send.psig = p;
	send.readsize = 0;
	return dkcStreamProcess(p->mStream,(void *)data,size,WriteCallback,&send);
	//return FileWithSignatureWriteLogic(p,data,size,dkcStreamWrite32);
}

int WINAPI dkcFileWithSignatureWrite64(DKC_FILE_WITH_SIGNATURE *p,
																								const void *data,size_t size)
{
	FWS_CALLBACK_SEND send;
	send.callback = dkcStreamWrite64;
	send.psig = p;
	send.readsize = 0;
	return dkcStreamProcess(p->mStream,(void *)data,size,WriteCallback,&send);
	//return FileWithSignatureWriteLogic(p,data,size,dkcStreamWrite64);
}

int WINAPI dkcFileWithSignatureCheckCheat(DKC_FILE_WITH_SIGNATURE *p){
	ULONGLONG filesize = p->mFileHeader.filesize;
	ULONGLONG stream_size = dkcStreamTell(p->mStream);
	BYTE buff[SHA1_BIN_BUFFER_SIZE];
	int r;

	if(FALSE==p->mAllLoad){//すべて読み込んでいない。
		return edk_LogicError;
	}
	if(TRUE==p->mWriteMode){//書き込みモード時は使用できない。
		return edk_LogicError;
	}

	dkcSHA1Load(p->mSHA1,
		dkcBufferPointer(p->mDustbin),
		dkcBufferSize(p->mDustbin)
	);

	//メッセージダイジェストが欲しい。（二回呼び出されてもOK）
	dkcSHA1FinalDigest(p->mSHA1,buff,sizeof(buff));
	
	r = dkc_memcmp(
		buff,sizeof(buff),
		p->mFileHeader.hash_value,sizeof(p->mFileHeader.hash_value));

	if(DKUTIL_FAILED(r)){//違うみたい～～。
		return edk_SignatureException;
	}

	if(filesize < stream_size){//サイズが違う～～。へんなもの引っ付いている～～。
		return edk_FileCheated_Addition;
	}
	if(filesize == stream_size){
		return edk_SUCCEEDED;
	}
	//誰だよ、チートしたのは･･･。
	return edk_FAILED;

}

/*

BOOL WINAPI dkcFileWithSignatureIsEOF(DKC_FILE_WITH_SIGNATURE *p);

BOOL WINAPI dkcFileWithSignatureIsError(DKC_FILE_WITH_SIGNATURE *p);

//@return 改竄されていたらTRUE
BOOL WINAPI dkcFileWithSignatureIsCheated(DKC_FILE_WITH_SIGNATURE *p){
	if(p->mSHA1->mFinalized){
		dkcmNOT_ASSERT("ERROR");
		return edk_LogicError;
	}
	p->mSHA1

}
																												 
*/