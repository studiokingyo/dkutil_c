/*!
@file dkcSJISFileSystem.h
@author d金魚
@note
Licence is "BSD Licence"<br>
dkc_PathStringはパスを相対パスでも必ず絶対パスにして格納します。<br>
dkc_FileFinderはSJIS問題に対応しています。(多分＾＾；；）<br>
@brief ShiftJISを考慮したファイルシステム
*/
#ifndef DKUTIL_C_SJIS_FILESYSTEM_H
#define DKUTIL_C_SJIS_FILESYSTEM_H

#include "dkcOSIndependent.h"
#include "dkcMemoryStream.h"
#include "dkcString.h"
#include "dkcStack.h"

#ifdef WIN32

#else


# include "dirent.h"
# include "unistd.h"
# include "fcntl.h"
# include "utime.h"

#endif

DKC_EXTERN const char * WINAPI dkcGetFileExtension( const char *PathName );

DKC_EXTERN int WINAPI dkcIsShiftJIS( const char *str, int nPos );

/* Generic */
/*! @return pathが存在して、有効なパス名だったらTRUE */
DKC_EXTERN BOOL WINAPI dkcIsEffectivePath(const char *path,size_t size);

/*! @return パスが相対パスだったらTRUE */
DKC_EXTERN BOOL WINAPI dkcIsRelativityPath(const char *path);
/*! @return パスが絶対パスだったらTRUE */
DKC_EXTERN BOOL WINAPI dkcIsAbsolutelyPath(const char *path);
/*!
@param dest[out] バッファへのポインタ
@param dsize[in] destのサイズ
@param src[in] ファイルの文字列
無条件でsrcに入れた文字列をカレントディレクトリ+srcとしてdestに出力する。
@note
dsizeがdkcdMAXPATH_LEN以下だと失敗する。
@return edk_SUCCEEDEDで成功
*/
DKC_EXTERN int WINAPI dkcCurrentDirectoryConcatenate(char *dest,size_t dsize,const char *src);
/*!
@param dest[in] <b>カレントディレクトリの文字列が入っている</b>バッファへのポインタ
@param dlen[in] destの文字数
@param dsize[in] destのサイズ
@param src[in] 元文字列
@return dlen + 2 > dsizeだとバッファオーバーフローする可能性があるので失敗する。
*/
DKC_EXTERN int WINAPI dkcDirectoryConcatenate(char *dest,size_t dlen,size_t dsize,const char *src);

///@return srcの一番後ろにに\\がついていたらTRUE
DKC_EXTERN BOOL WINAPI dkcIsTailPathSep(const char *src,size_t dlen);
/*!
@param dest[in] バッファ
@param dlen[in] destの文字数
@param size[in] destのサイズ
@return 
edk_SUCCEEDEDなら成功 <br>
edk_OutputBufferWasLostはバッファが足りなかった<br>
その他は入れる必要が無かった。<br>
*/
DKC_EXTERN BOOL WINAPI dkcPushBackPathSep(char *dest,size_t dlen,size_t size);

///パスを正規化(相対パスから絶対パス)する関数
DKC_EXTERN int WINAPI dkcToAbsolutelyPath(char *dest,size_t dsize,const char *src,size_t ssize);

/*!
//**********************************************************
ファイルパス　ユーティリティ
//**********************************************************
*/
/*!
@note
仕様：
mStringにディレクトリを入れる。
内部で必ず絶対パスに変換してからmStringに入れる。
@todo
Releaseモード時にバグってしまう問題をどうにかする。
*/
typedef struct dkc_PathString{
	DKC_STRING *mString;
	///イテレートカウント
	size_t mIterateCount;
}DKC_PATHSTRING;

//PATHSTRINGの割り当て、解放
/*!
@param path[in] ファイル、ディレクトリへのパスを入れる。
@return 無効な文字列が使われていたり、上手く割り当て出来なかったらNULL
@note
パスが相対パスだった場合、内部で勝手に絶対パスに変換される。
*/
DKC_EXTERN DKC_PATHSTRING * WINAPI dkcAllocPathString(const char *path);

DKC_EXTERN int WINAPI dkcFreePathString(DKC_PATHSTRING **);

//パスのデータを得る。

DKC_EXTERN size_t WINAPI dkcPathStringSize(const DKC_PATHSTRING *);

DKC_EXTERN const char *WINAPI dkcPathStringPointer(const DKC_PATHSTRING *);


//パスを区切る。

DKC_EXTERN int WINAPI dkcPathStringDevideBegin(DKC_PATHSTRING *,char *buff,size_t size);
/*! @return  edk_EndProcessが返ってきたらdkcPathStringDevideEnd()を呼び出して終了する */
DKC_EXTERN int WINAPI dkcPathStringDevideNext(DKC_PATHSTRING *,char *buff,size_t size);

DKC_EXTERN void WINAPI dkcPathStringDevideEnd(DKC_PATHSTRING *);




///パスを正規化して入れる。
DKC_EXTERN int WINAPI dkcPathStringCopy(DKC_PATHSTRING *,const char *buff,size_t size);
///パスを正規化して繋げる。
DKC_EXTERN int WINAPI dkcPathStringConcatenate(DKC_PATHSTRING *,const char *buff,size_t size);

DKC_EXTERN int WINAPI dkcPathStringNormalizeConcatenateLogic(
	DKC_PATHSTRING *ptr,const char *buff,size_t size);
/*
DKC_EXTERN int WINAPI dkcPathStringElementInsert(DKC_PATHSTRING *,const char *buff,size_t size);

DKC_EXTERN int WINAPI dkcPathStringElementReplace(DKC_PATHSTRING *,const char *buff,size_t size);
*/

DKC_EXTERN int WINAPI dkcPathStringGetDrive(DKC_PATHSTRING *,char *buff,size_t size);

DKC_EXTERN int WINAPI dkcPathStringGetFileExtension(DKC_PATHSTRING *,char *buff,size_t size);

DKC_EXTERN int WINAPI dkcPathStringGetFileName(DKC_PATHSTRING *,char *buff,size_t size);
/*!
@note
取得した文字列の最後尾に\\を付加させる。
*/
DKC_EXTERN int WINAPI dkcPathStringGetDirectory(DKC_PATHSTRING *,char *buff,size_t size);


//**********************************************************
//関数の後ろにLogicという名前がついているものは
//内部を理解していない場合は使わな方が無難です。

///@see dkcPathStringDevideBegin()
DKC_EXTERN int WINAPI dkcPathStringDevideBegin_Logic(DKC_PATHSTRING *,size_t *count,char *buff,size_t size);
///@see dkcPathStringDevideNext()
DKC_EXTERN int WINAPI dkcPathStringDevideNext_Logic(DKC_PATHSTRING *,size_t *count,char *buff,size_t size);
///@see dkcPathStringDevideEnd()
DKC_EXTERN void WINAPI dkcPathStringDevideEnd_Logic(size_t *count);

///パスの要素を挿入する。(正規化はしない lenは文字数
DKC_EXTERN int WINAPI dkcPathStringElementInsert_Logic(DKC_PATHSTRING *,size_t count,
																								const char *src,size_t len);

DKC_EXTERN int WINAPI dkcPathStringElementErase_Logic(DKC_PATHSTRING *,size_t count);

///パスの要素を置換する。(正規化はしない lenは文字数
DKC_EXTERN int WINAPI dkcPathStringElementReplace_Logic(DKC_PATHSTRING *,size_t count,
																								 const char *,size_t len);


/*!
//**********************************************************
ファイル操作ユーティリティ
//**********************************************************
*/

/*! @return TRUEで成功 */
DKC_EXTERN BOOL WINAPI dkcSetCurrentDirectory(const char *filename);

/*! @return TRUEで成功 */
DKC_EXTERN BOOL WINAPI dkcGetCurrentDirectory(char *buff,size_t size);

/*!
@note
Thread safe ではありません。(多分safeでなくても大丈夫だと思いますが。
*/
DKC_EXTERN int WINAPI dkcCreateDirectory(const char *pPath);
/*!
@note
thread safe ではありません。（気になる方はdkcFileCopyExをお使いください。)
何故ならコピーするファイルがデカいと処理が止まったままになってしまうからです。
*/
DKC_EXTERN BOOL WINAPI dkcFileCopy(const char *dest,const char *src,BOOL bReWrite);



typedef struct dkc_FileCopyExCallbackData{
	size_t filesize;
	size_t count;
}DKC_FILECOPY_CALLBACK_STRUCT;
/**
@param inner_buffer_size[in] 内部のコピーで使うバッファのサイズを設定する。
@param bRewrite[in] 上書きOKな場合はこれを指定する。
@param bThreadLock[in] TRUEだとスレッドをロックする。(大きいファイルの場合は危険)
@param pfCallback[in] DKC_STD_CALLBACK型の関数 Callback関数を指定する NULLでもかまわない
@param parg[in][out] Callbackに渡すvoid *型ポインタ
@return TRUEだと成功
@todo
HDのサイズ等を取得してコピーするのが安全かどうかを調べなければならない。
スレッド生成してすぐに処理を返すとか？
スレッドをロックするオプションは廃止する。
*/
DKC_EXTERN BOOL WINAPI dkcFileCopyEx(const char *dest,const char *src,
																		size_t inner_buffer_size,
																		BOOL bReWrite,BOOL bThreadLock,
																		DKC_STD_CALLBACK pcallback,void *parg);

/*! ファイルを削除する。 @return 削除できたらTRUE @param filename[in] ファイル名*/
DKC_EXTERN BOOL WINAPI dkcFileRemove(const char *filename);
/*!
@param oldname[in] 古い名前へのポインタ
@param newname[in] 新しい名前へのポインタ
*/
DKC_EXTERN BOOL WINAPI dkcFileRename(const char *oldname,const char *newname);

/*!
@return TRUEだとファイルが存在する なお、Directory(folder)は存在しないことになる。
*/
DKC_EXTERN BOOL WINAPI dkcFileExist(const char *filename);

DKC_EXTERN BOOL WINAPI dkcFileAndFolderExist(const char *filename);

DKC_EXTERN BOOL WINAPI dkcFolderExist(const char *filename);
/*!
@return 0だとファイルサイズが０　または　内部エラー また　はファイルが存在しない
*/
DKC_EXTERN ULONG WINAPI dkcFileSize(const char *filename);
/*!
@return FALSEだと失敗
*/
DKC_EXTERN BOOL WINAPI dkcFileSize64(const char *filename,DWORD *high,DWORD *low);

DKC_INLINE BOOL dkcFileSize64V(const char *filename,uint64 *pv){
	DWORD high,low;
	if(FALSE==dkcFileSize64(filename,&high,&low)){
		return FALSE;	
	}
	dkcTwoDWORDToULONGLONG(pv,high,low);
	return TRUE;
}

/*!
@param filename[in] ファイル名
@param rewrite[in] 上書き保存を許可するかどうか？
zerobyteのファイルを作る
*/
DKC_EXTERN BOOL WINAPI dkcCreateZeroByteFile(const char *filename,BOOL rewrite);
/**
@param filename1[in] 比較したいファイル名（フルパスが望ましい）
@param filename2[in] 比較したいファイル名（フルパスが望ましい）
@retunr バイナリレベルで一致したらedk_SUCCEEDED edk_FAILEDで一致しない　その他は　引数などの呼び出し側のエラー
*/
DKC_EXTERN int WINAPI dkcFileBinaryCompare(const char *filename1,const char *filename2);

/**
buffの内容をfilenameファイルに書きこむ
@param filename[in]保存したいファイル名
@param buff[in] メモリへのアドレス
@param size[in] buffのサイズ
@param flag[in] edkcFileRewriteの組み合わせ edkcFileRewriteとか
@return edkResultのどれか
*/
DKC_EXTERN int WINAPI dkcMemoryToFile(const char *filename,const void *buff,size_t size,UINT flag);

/**
filenameの内容をbuffに読みこむ
@param filename[in] 読みこみたいファイル名
@param buff[in] バッファへのアドレス
@param size[in] buffのサイズ
@return edkResultのどれか
*/
DKC_EXTERN int WINAPI dkcFileToMemory(const char *filename,void *buff,size_t size);

/* 64bitファイルシステムができるまでお預け
DKC_EXTERN int WINAPI dkcMemoryToTextFile(const char *filename,const void *buff,size_t size);

DKC_EXTERN size_t WINAPI dkcTextFileToMemorySize(const char *filename);

DKC_EXTERN int WINAPI dkcTextFileToMemory(const char *filename,void *buff,size_t size);
*/

/**
Fileを検索するための機構の状態を保存する構造体
*/
typedef struct dkc_FileFinder{
	///Directory
	DKC_PATHSTRING *mDir;
	///target (wildcardもOK)
	DKC_STRING *mTarget;
	///サブディレクトリも検索するか？フラグ
	BOOL mbSubDir;
	///状態
	int mState;
	///スタック。
	DKC_STACK *mStack;
#	ifdef WIN32
	/// 検索結果 
	HANDLE mHandle;
	WIN32_FIND_DATAA mFindData;
#	else //linux or unix...
	struct dirent *mDirent;
	DIR *mHandle;
#	endif
}DKC_FILE_FINDER;

typedef struct dkc_FindFile{
#	ifdef WIN32
	/// 検索結果 
	HANDLE mHandle;
	WIN32_FIND_DATAA mFindData;
#	else //linux or unix...
	struct dirent *mDirent;
	DIR *mHandle;
#endif
}DKC_FINDFILE;

DKC_EXTERN DKC_FINDFILE *WINAPI dkcAllocFindFile();

DKC_EXTERN int WINAPI dkcFreeFindFile(DKC_FINDFILE **);

/*! Windowsで言うFindFirstFile()のラッパー UNIXやLINUXでも動くかも？ */
DKC_EXTERN int WINAPI dkcFindFirstFile(DKC_FINDFILE *ptr,const char *target);
/*! FindNextFile() @see dkcFindFirstFile() */
DKC_EXTERN int WINAPI dkcFindNextFile(DKC_FINDFILE *ptr);
/*! FindClose() @see dkcFindFirstFile() */
DKC_EXTERN int WINAPI dkcFindClose(DKC_FINDFILE *ptr);

DKC_EXTERN int WINAPI dkcFindFileGetFileName(DKC_FINDFILE *ptr,
																						 char *buff,size_t buffsize);

DKC_EXTERN BOOL WINAPI dkcFindFileIsFolder(DKC_FINDFILE *ptr);

DKC_EXTERN BOOL WINAPI dkcFindFileIsDot(DKC_FINDFILE *ptr);

DKC_EXTERN BOOL WINAPI dkcFindFileIsNormalFile(DKC_FINDFILE *ptr);

DKC_EXTERN BOOL WINAPI dkcFindFileIsReadOnly(DKC_FINDFILE *ptr);

DKC_EXTERN void WINAPI dkcFindFileSize(DKC_FINDFILE *ptr,ULONG *,ULONG *);




#if 0
enum edkcFileFinder{
	///何も行っていない。
	edkcFileFinderEmpty = 0,
	///ただ今検索中
	edkcFileFinderSearching,
	///検索終了。
	edkcFileFinderFinish,
};


/*!
@param target[in] ターゲットファイル。(wild cardもOK）
@param dir[in] ディレクトリ(誤作動の無いようにフルパスが望ましい。）
@param bSubDir[in] TRUEだとサブディレクトリも検索する。
@return NULLだと引数のどれかが例外だったとかいろいろ ともかく失敗。
*/
DKC_EXTERN DKC_FILE_FINDER * WINAPI dkcAllocFileFinder(
			const char *target,const char *dir,BOOL bSubDir
			);

DKC_EXTERN int WINAPI dkcFreeFileFinder(DKC_FILE_FINDER **);


/*!
@param ptr[in] dkcAllocFileFinder()で取得したポインタ
@param path[in][out] pathはdkcAllocPathString()で確保した領域を使用する事。
@param bCopySucceeded[out] pathへコピーが完了したらTRUEを得れる。
@return edk_SUCCEEDEDならdkcFileFinderNext()を また、呼び出して良い。
それ以外ならdkcFreeFileFinder()で終了させなければならない。
*/
DKC_EXTERN int WINAPI dkcFileFinderNext(
	DKC_FILE_FINDER *ptr,DKC_PATHSTRING *path,BOOL *bCopySucceeded
);



/*! ファイルネームを取得 
@return edk_LogicExceptionの時は検索した結果がファイルの名前ではなかった。
なのでdkcFindNextFile()を呼び出す必要がある。
*/
DKC_EXTERN int WINAPI dkcFileFinderReferenceFileName(DKC_FILE_FINDER *ptr,DKC_PATHSTRING *path);

#ifdef __cplusplus
namespace dkutil{


/*!
@param STL_T std::stringをテンプレート引数に持ったpush_backが使えるコンテナ（例：std::list<std::string>
@param target[in] ターゲットファイル。(wild cardもOK）
@param dir[in] ディレクトリ(誤作動の無いようにフルパスが望ましい。）
@param bSubDir[in] TRUEだとサブディレクトリも検索する。
@param c[in][out] STL_T型のコンテナ
@return trueだと成功
*/
template<class STL_T>
inline bool file_listup(const char *target,const char *dir,bool bSubDir,
												STL_T &c)
{
	//定義
	//typedef STL_T::iterator iterator;
	
	bool result = false;
	BOOL bCopySucceeded = FALSE;
	DKC_PATHSTRING *pPath = NULL;
	DKC_FILE_FINDER *pFinder = NULL;

	//初期化
	pPath = dkcAllocPathString(dir);
	if(NULL==pPath){	return false;}
	pFinder = dkcAllocFileFinder(target,dir,(BOOL)bSubDir);
	if(NULL==pFinder){ goto Error;}
	
	//std::string str;
	//処理
	for(;;){
		int r = dkcFileFinderNext(pFinder,pPath,&bCopySucceeded);
		if(edk_EndProcess == r){
			break;
		}else if(DKUTIL_FAILED(r)){
			goto Error;
		}
		if(TRUE==bCopySucceeded){//コピーできている。
			c.push_back(dkcPathStringPointer(pPath));
		}
	}
	
	result = true;

Error://エラー∩後始末
	dkcFreePathString(&pPath);
	dkcFreeFileFinder(&pFinder);
	return result;
}


}
#endif //end of __cplusplus


#endif //end of if 0



#endif //end of include once