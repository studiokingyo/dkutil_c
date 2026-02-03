/*!
@file dkcGenericFileSystem.h
@brief	汎用的なファイル入出力インターフェイス
@author		d金魚
@since	2005/02/19
@todo 
@note

*/
#ifndef DKUTIL_C_GENERIC_FILE_SYSTEM_H
#define DKUTIL_C_GENERIC_FILE_SYSTEM_H


#include "dkcOSIndependent.h"
#include "dkcDLL.h"

enum edkc_FileMode{
	///読み込みモード
	edkcReadMode = 1,
	///書き込みモード(ファイルが既に存在する場合は上書きされる）
	edkcWriteMode = 2,
	///追記モード
	edkcPostScriptMode = 4,

	///バイナリモード
	edkcBinaryMode = 64,
	///テキストモード
	edkcTextMode = 128,

};



typedef struct dkc_GenericFileSystemSeekArg{
	///何処に移動するか
	DKC_UINT64_STRUCT Point;
	///edkcSeekCurrentとか入れる
	int origin;
}DKC_GFS_SEEK_ARG;

//Genericなファイル操作関数の型を定義する
typedef void *(*DKC_FILE_OPEN_F_TYPE)(void *,uint32,const void *);
typedef int (*DKC_FILE_READ_F_TYPE)(void *,void *,size_t,size_t *);
typedef int (*DKC_FILE_WRITE_F_TYPE)(void *,const void *,size_t,size_t *);
typedef int (*DKC_FILE_CLOSE_F_TYPE)(void *);
typedef int (*DKC_FILE_SEEK_F_TYPE)(void *,const DKC_GFS_SEEK_ARG *);
typedef int (*DKC_FILE_TELL_F_TYPE)(void *,DKC_UINT64_STRUCT *);
typedef int (*DKC_GFS_FREE_F_TYPE)(void *);

typedef int (*DKC_FILE_DELETE_F_TYPE)(void *,const void *);
#ifdef WIN32

typedef struct dkc_open_arg_data{
	DWORD DesiredAccess; // access (read-write) mode

	DWORD ShareMode; // share mode

	LPSECURITY_ATTRIBUTES lpSecurityAttributes;// pointer to security attributes

	DWORD CreationDisposition; // how to create

	DWORD FlagsAndAttributes; // file attributes

	HANDLE TemplateFile; // handle to file with 
}DKC_OPEN_ARG_DATA;

DKC_EXTERN void dkc_to_CreateFile_arg_data(DKC_OPEN_ARG_DATA *data,uint32 flag);

typedef struct dkc_win32_fs{
	HANDLE mHandle;
	void* CreateFile;
	void* SetFileAttributes;
	void* GetFileAttributes;
	void* DeleteFile;
	DKC_OPEN_ARG_DATA mArg;
	DKC_DLL *mdllobj;

	DWORD mLastError;
	///UNICODEかSJISか
	int mFlag;

}DKC_WIN32_FS;

#endif



typedef struct dkc_GenericFileSystem{
	//DKC_UINT64_STRUCT now_point;

	///ファイルオープン系オブジェクト
#ifdef WIN32
	DKC_WIN32_FS *mfp;
#else
	void *mfp;
#endif
	size_t char_size;

	//関数
	DKC_FILE_OPEN_F_TYPE mfOpen;
	DKC_FILE_CLOSE_F_TYPE mfClose;
	DKC_FILE_READ_F_TYPE mfRead;
	DKC_FILE_WRITE_F_TYPE mfWrite;
	DKC_FILE_SEEK_F_TYPE mfSeek;
	DKC_FILE_TELL_F_TYPE mfTell;

	DKC_GFS_FREE_F_TYPE mfObjFree;

	//ファイル操作系関数
	DKC_FILE_DELETE_F_TYPE mfDeleteFile;

}DKC_GENERIC_FILESYSTEM;


/*

typedef struct dkc_GFS_Win32
{

	//pPathに使うパスはGetTempPath()とかで得てください。
	//GetTempFileName
}DKC_GFS_WIN32;
*/


//その時に応じた差異的なファイルシステムを構築する。
//DKC_GENERIC_FILESYSTEM *WINAPI dkcAllocGenericFileSystem();

DKC_GENERIC_FILESYSTEM *WINAPI dkcAlloc_SJIS_FileSystem();

DKC_GENERIC_FILESYSTEM *WINAPI dkcAlloc_UNICODE_FileSystem();

DKC_EXTERN int WINAPI dkcFreeGenericFileSystem(DKC_GENERIC_FILESYSTEM **);

DKC_EXTERN int WINAPI dkcGenericFileSystemOpen(DKC_GENERIC_FILESYSTEM *,uint32 flags,const void *filename);

DKC_EXTERN int WINAPI dkcGenericFileSystemClose(DKC_GENERIC_FILESYSTEM *);

DKC_EXTERN int WINAPI dkcGenericFileSystemRead(DKC_GENERIC_FILESYSTEM *,void *pbuff,size_t size,size_t *read_size);

DKC_EXTERN int WINAPI dkcGenericFileSystemWrite(DKC_GENERIC_FILESYSTEM *,const void *pbuff,size_t size,size_t *write_size);

DKC_EXTERN int WINAPI dkcGenericFileSystemSeek(DKC_GENERIC_FILESYSTEM *,const DKC_GFS_SEEK_ARG *);

DKC_EXTERN int WINAPI dkcGenericFileSystemTell(DKC_GENERIC_FILESYSTEM *,DKC_UINT64_STRUCT *);

DKC_EXTERN int WINAPI dkcGenericFileSystemDeleteFile(DKC_GENERIC_FILESYSTEM *,const void *filename);




typedef struct dkc_File64{
#ifdef WIN32
	int handle;
#else

#endif
}DKC_FILE64;


/**
@param flags[in] edkc_FileModeを使用
@param filename[in] ファイル名

*/
DKC_FILE64 *WINAPI dkcAllocFile64(uint32 flags,const void *filename);

DKC_EXTERN int WINAPI dkcFreeFile64(DKC_FILE64 **);

DKC_EXTERN int WINAPI dkcFile64Open(DKC_FILE64 *p,uint32 flags,const void *filename);

DKC_EXTERN int WINAPI dkcFile64Close(DKC_FILE64 *p);

DKC_EXTERN int WINAPI dkcFile64Read(DKC_FILE64 *p,void *pbuff,size_t size,size_t *read_size);

DKC_EXTERN int WINAPI dkcFile64Write(DKC_FILE64 *p,const void *pbuff,size_t size,size_t *write_size);
/**
@param origin[in] edkcSeekSet edkcSeekCurrent edkcSeekEndのどれか。
*/
DKC_EXTERN int WINAPI dkcFile64Seek(DKC_FILE64 *p,int64 offset,int origin);

DKC_EXTERN int WINAPI dkcFile64Tell(DKC_FILE64 *p,uint64 *);
///@return -1だとエラー
DKC_EXTERN BOOL WINAPI dkcFile64EOF(DKC_FILE64 *p);

#endif //end of include once


