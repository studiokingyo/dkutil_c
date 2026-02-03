/*!
@file dkcUnityArchivers.h
@brief	統合アーカイバプロジェクト関係
@author		d金魚
@since	2005/01/02
@todo 
@note
ここを見よ！<br>
http://www.csdinc.co.jp/archiver/
*/
#ifndef DKUTIL_C_UNITY_ARCHIVERS_H
#define DKUTIL_C_UNITY_ARCHIVERS_H


#include "dkcOSIndependent.h"
#include "dkcDLL.h"

#define dkcdUNITY_ARCHIVERS_FILENAME_MAX	512
#ifndef WIN32 //posix... ??

typedef void *HWND;
typedef int		(WINAPI* DKC_UA_ARC_F_TYPE)(const HWND, const char *, char*, const DWORD);

#else //win32



#include <time.h>


typedef BOOL (CALLBACK *DKC_UA_ARCHIVER_WND_PROC_F_TYPE)(HWND _hwnd, UINT _uMsg,
						UINT _nState, LPVOID _lpEis);
//typedef ARCHIVERPROC *LPARCHIVERPROC;


typedef struct {
	DWORD	dwOriginalSize;		/* ファイルのサイズ。		*/
	DWORD	dwCompressedSize;	/* 圧縮後のサイズ。		*/
	DWORD	dwCRC;			/* 格納ファイルのチェックサム/CRC */
	UINT	uFlag;			/* 解凍やテストの処理結果	*/
	UINT	uOSType;		/* このファイルの作成に使われたＯＳ。*/
	WORD	wRatio;			/* 圧縮率（パーミル)		*/
	WORD	wDate;			/* 格納ファイルの日付。		*/
	WORD	wTime;			/* 格納ファイルの時刻。		*/
	char	szFileName[dkcdUNITY_ARCHIVERS_FILENAME_MAX + 1];/* アーカイブファイル名*/
	char	dummy1[3];
	char	szAttribute[8];		/* 格納ファイルの属性。		*/
	char	szMode[8];		/* 格納ファイルの格納モード。	*/
} INDIVIDUALINFO, FAR *LPINDIVIDUALINFO;

typedef HGLOBAL HARC;
//  LHA.DLL 互換 
typedef WORD	(WINAPI* DKC_UA_GETVERSION_F_TYPE)();
typedef BOOL	(WINAPI* DKC_UA_GETRUNNING_F_TYPE)();
typedef BOOL	(WINAPI* DKC_UA_GETBACKGROUDMODE_F_TYPE)();
typedef BOOL	(WINAPI* DKC_UA_SETBACKGROUDMODE_F_TYPE)(const BOOL);
typedef BOOL	(WINAPI* DKC_UA_GETCURSORMODE_F_TYPE)();
typedef BOOL	(WINAPI* DKC_UA_SETCURSORMODE_F_TYPE)(const BOOL);
typedef WORD	(WINAPI* DKC_UA_GETCURSORINTERVAL_F_TYPE)();
typedef BOOL	(WINAPI* DKC_UA_SETCURSORINTERVAL_F_TYPE)(const WORD);
typedef int		(WINAPI* DKC_UA_ARC_F_TYPE)(const HWND, LPCSTR, LPSTR, const DWORD);
typedef BOOL	(WINAPI* DKC_UA_CHECKARCHIVE_F_TYPE)(LPCSTR , const int);
typedef int		(WINAPI* DKC_UA_GETFILECOUNT_F_TYPE)(LPCSTR);
typedef BOOL	(WINAPI* DKC_UA_QUERYFUNCTIONLIST_F_TYPE)(const int);
typedef BOOL	(WINAPI* DKC_UA_CONFIGDIALOG_F_TYPE)(const HWND, LPSTR, const int);
// OpenArchive 系
typedef HARC    (WINAPI* DKC_UA_OPENARCHIVE_F_TYPE)(const HWND, LPCSTR, const DWORD);
typedef int     (WINAPI* DKC_UA_CLOSEARCHIVE_F_TYPE)(HARC);
typedef int     (WINAPI* DKC_UA_FINDFIRST_F_TYPE)(HARC , LPCSTR, INDIVIDUALINFO*);
typedef int     (WINAPI* DKC_UA_FINDNEXT_F_TYPE)(HARC, INDIVIDUALINFO*);
typedef int     (WINAPI* DKC_UA_GETARCFILENAME_F_TYPE)(HARC , LPSTR, const int);
typedef DWORD   (WINAPI* DKC_UA_GETARCFILESIZE_F_TYPE)(HARC);
typedef DWORD   (WINAPI* DKC_UA_GETARCORIGINALSIZE_F_TYPE)(HARC);
typedef DWORD   (WINAPI* DKC_UA_GETARCCOMPRESSEDSIZE_F_TYPE)(HARC);
typedef WORD    (WINAPI* DKC_UA_GETARCRATIO_F_TYPE)(HARC);
typedef WORD    (WINAPI* DKC_UA_GETARCDATE_F_TYPE)(HARC);
typedef WORD    (WINAPI* DKC_UA_GETARCTIME_F_TYPE)(HARC);
typedef UINT    (WINAPI* DKC_UA_GETARCOSTYPE_F_TYPE)(HARC);
typedef int     (WINAPI* DKC_UA_ISSFXFILE_F_TYPE)(HARC);
typedef int     (WINAPI* DKC_UA_GETFILENAME_F_TYPE)(HARC, LPSTR, const int);
typedef int     (WINAPI* DKC_UA_GETMETHOD_F_TYPE)(HARC, LPSTR, const int);
typedef DWORD   (WINAPI* DKC_UA_GETORIGINALSIZE_F_TYPE)(HARC);
typedef DWORD   (WINAPI* DKC_UA_GETCOMPRESSEDSIZE_F_TYPE)(HARC);
typedef WORD    (WINAPI* DKC_UA_GETRATIO_F_TYPE)(HARC);
typedef WORD    (WINAPI* DKC_UA_GETDATE_F_TYPE)(HARC);
typedef WORD    (WINAPI* DKC_UA_GETTIME_F_TYPE)(HARC);
typedef DWORD   (WINAPI* DKC_UA_GETWRITETIME_F_TYPE)(HARC);
typedef DWORD   (WINAPI* DKC_UA_GETCREATETIME_F_TYPE)(HARC);
typedef DWORD   (WINAPI* DKC_UA_GETACCESSTIME_F_TYPE)(HARC);
typedef DWORD   (WINAPI* DKC_UA_GETCRC_F_TYPE)(HARC);
typedef int     (WINAPI* DKC_UA_GETATTRIBUTE_F_TYPE)(HARC);
typedef UINT    (WINAPI* DKC_UA_GETOSTYPE_F_TYPE)(HARC);
typedef DWORD   (WINAPI* DKC_UA_GETWRITETIMEEX_F_TYPE)(HARC, LPFILETIME);
typedef DWORD   (WINAPI* DKC_UA_GETCREATETIMEEX_F_TYPE)(HARC, LPFILETIME);
typedef DWORD   (WINAPI* DKC_UA_GETACCESSTIMEEX_F_TYPE)(HARC, LPFILETIME);
typedef BOOL    (WINAPI* DKC_UA_GETARCCREATETIMEEX_F_TYPE)(HARC, LPFILETIME);
typedef BOOL    (WINAPI* DKC_UA_GETARCACCESSTIMEEX_F_TYPE)(HARC, LPFILETIME);
typedef BOOL    (WINAPI* DKC_UA_GETARCWRITETIMEEX_F_TYPE)(HARC, LPFILETIME);
// メッセージ処理
typedef BOOL	(WINAPI* DKC_UA_SETOWNERWINDOW_F_TYPE)(const HWND);
typedef BOOL	(WINAPI* DKC_UA_CLEAROWNERWINDOW_F_TYPE)();
typedef BOOL	(WINAPI* DKC_UA_SETOWNERWINDOWEX_F_TYPE)(HWND, DKC_UA_ARCHIVER_WND_PROC_F_TYPE);
typedef BOOL	(WINAPI* DKC_UA_KILLOWNERWINDOWEX_F_TYPE)(HWND);
// その他のもの 
///Window列挙用
typedef BOOL (CALLBACK *dkctWND_ENUM_PROC_F_TYPE)(LPVOID);
typedef WORD    (WINAPI* DKC_UA_GETSUBVERSION_F_TYPE)();
typedef int     (WINAPI* DKC_UA_EXTRACTMEM_F_TYPE)(const HWND, LPCSTR, LPBYTE, const DWORD, time_t*, LPWORD, LPDWORD);
typedef int     (WINAPI* DKC_UA_COMPRESSMEM_F_TYPE)(const HWND, LPCSTR, const BYTE*, const DWORD, const time_t*, const WORD*, LPDWORD);
typedef BOOL    (WINAPI* DKC_UA_SETENUMMEMBERSPROC_F_TYPE)(dkctWND_ENUM_PROC);
typedef BOOL    (WINAPI* DKC_UA_CLEARENUMMEMBERSPROC_F_TYPE)();

//ZIP用

typedef BOOL (WINAPI *DKC_UA_QUERYENCRYPTION_F_TYPE)(VOID);

//エクスポート関数の ID の最大値
//#define ISARC_FUNCTION_END    81

//エラーコード
//#define ERROR_CANT_LOAD_DLL    (ERROR_END + 1)
//#define ERROR_CANT_FREE_DLL    (ERROR_END + 2)

/**
@note
UA : Unity Archivers または Unified Archivers
*/
typedef struct dkc_UnityArchivers{
	DKC_UA_GETVERSION_F_TYPE UAGetVersion;
	DKC_UA_GETRUNNING_F_TYPE UAGetRunning;

	DKC_UA_ARC_F_TYPE UA;
	DKC_UA_CONFIGDIALOG_F_TYPE UAConfigDialogv;
	DKC_UA_QUERYFUNCTIONLIST_F_TYPE UAQueryFunctionList;
	DKC_UA_QUERYENCRYPTION_F_TYPE UAQueryEncryption;

	DKC_UA_ARC_F_TYPE UnUA;
	DKC_UA_GETVERSION_F_TYPE UnUAGetVersion;
	DKC_UA_OPENARCHIVE_F_TYPE UnUAOpenArchive;
	DKC_UA_FINDFIRST_F_TYPE UnUAFindFirst;
	DKC_UA_CLOSEARCHIVE_F_TYPE UnUACloseArchive;
	DKC_UA_FINDNEXT_F_TYPE UnUAFindNext;
	DKC_UA_QUERYFUNCTIONLIST_F_TYPE UnUAQueryFunctionList;

	DKC_UA_ARC_F_TYPE UAUnUA;
	DKC_UA_GETVERSION_F_TYPE UAUnUAGetVersion;
	DKC_UA_OPENARCHIVE_F_TYPE UAUnUAOpenArchive;
	DKC_UA_FINDFIRST_F_TYPE UAUnUAFindFirst;
	DKC_UA_CLOSEARCHIVE_F_TYPE UAUnUACloseArchive;
	DKC_UA_FINDNEXT_F_TYPE UAUnUAFindNext;
	DKC_UA_QUERYFUNCTIONLIST_F_TYPE UAUnUAQueryFunctionList;
	DKC_DLL *mDLL;
}DKC_UNITY_ARCHIVERS,DKC_UNIFIED_ARCHIVERS;//普通、二つ名前をもつのは邪道なのだが･･･

DKC_UNITY_ARCHIVERS *WINAPI dkcAllocUnityArchivers(const char *dllname,const char *function_prefix);

int WINAPI dkcFreeUnityArchivers(DKC_UNITY_ARCHIVERS **);

int WINAPI dkcUnityArchiversDirectoryToArchive(DKC_UNITY_ARCHIVERS *,const char *dir_or_file,const char *filename);

int WINAPI dkcUnityArchiversArchiveToDirectory(DKC_UNITY_ARCHIVERS *,const char *filename,const char *dir_or_file);

#endif //end of WIN32

/**
@param archiver_result[out] 統合アーカイバ使用の関数の処理情報 0で成功
*/
///@return edk_Resultのどれか
int WINAPI 	dkcUnityArchiversCommand(
		const char *dllname,const char *command_function_name,const char *command,
		char *StringResultOutputBuffer,size_t output_size,int *archiver_result,HWND wnd
	);

#endif //end of include once