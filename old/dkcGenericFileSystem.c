
/**
@file dkcGenericFileSystem.c
@note
sjis_ 系のファイル操作ライブラリについて
thread lockおよびthread unlockはGetLastError()がスレッド固有で内部にエラー情報を蓄えている場合は必要ない

*/

#include "dkcGenericFileSystem.h"

#ifdef WIN32

#ifdef _MSC_VER
//関数へのﾎﾟｲﾝﾀがﾃﾞｰﾀへのﾎﾟｲﾝﾀに変換されました。
#	pragma warning(disable:4055 4152)
#endif

enum{
	edkcWin32SJIS = 0,
	edkcWin32Unicode = 1,
};


typedef HANDLE (WINAPI *CreateFileA_FT)(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );


typedef HANDLE (WINAPI *CreateFileW_FT)(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );


typedef BOOL (WINAPI *SetFileAttributesA_FT)(
    LPCSTR lpFileName,
    DWORD dwFileAttributes
    );


typedef BOOL (WINAPI *SetFileAttributesW_FT)(
    LPCWSTR lpFileName,
    DWORD dwFileAttributes
    );



typedef DWORD (WINAPI *GetFileAttributesA_FT)(
    LPCSTR lpFileName
    );

typedef DWORD (WINAPI *GetFileAttributesW_FT)(
    LPCWSTR lpFileName
    );


typedef BOOL (WINAPI *DeleteFileA_FT)(
    LPCSTR lpFileName
    );

typedef BOOL (WINAPI *DeleteFileW_FT)(
    LPCWSTR lpFileName
    );




DKC_INLINE void dkc_to_CreateFile_arg_data(DKC_OPEN_ARG_DATA *data,uint32 flag)
{
	BOOL wm = (flag & edkcWriteMode) != 0;
	BOOL rm = (flag & edkcReadMode) != 0;
	BOOL pm = (flag & edkcPostScriptMode) != 0;
	memset(data,0,sizeof(*data));

	if(pm){	//追記モード
		/*if(wm && rm){
			data->CreationDisposition |= OPEN_EXISTING;
		}else if(wm){
			data->CreationDisposition |= OPEN_EXISTING;
		}else if(rm){
			data->CreationDisposition |= OPEN_EXISTING;
		}*/
		data->CreationDisposition |= OPEN_EXISTING;
	}else{//追記じゃないモード
		if(wm && rm){
			//もし、存在したらオープンする指定
			data->CreationDisposition |= OPEN_EXISTING;
		}else if(wm){
			//上書きし指定
			data->CreationDisposition |= CREATE_ALWAYS;
		}else if(rm){
			//存在したらオープンする指定
			data->CreationDisposition |= OPEN_EXISTING;
		}

	}


	if(wm){
		//書き込み中は読み込みだけ許可
		data->ShareMode |= FILE_SHARE_READ;
		
		data->DesiredAccess |= GENERIC_WRITE;
	}
	if(rm){
		//同じく読み込み中も
		data->ShareMode |= FILE_SHARE_READ;
		
		data->DesiredAccess |= GENERIC_READ;
	}



	//普通ファイル指定
	data->FlagsAndAttributes |= FILE_ATTRIBUTE_NORMAL;
	//data->TemplateFile = NULL;
	
}


#define WIN32_FS_CALL(type,obj,function_address)\
	(type)(function_address)


static void *win32_open(void *pobj,uint32 flags,const void *filename){
	DKC_WIN32_FS *pfs = pobj;
	HANDLE fp;
	DKC_OPEN_ARG_DATA Arg;
	CreateFileA_FT af = pfs->CreateFile;
	CreateFileW_FT wf = pfs->CreateFile;


	dkc_to_CreateFile_arg_data(&Arg,flags);
	
	
	if(edkcWin32SJIS == pfs->mFlag){ 
		fp = af(
				filename,
				Arg.DesiredAccess, // access (read-write) mode
				Arg.ShareMode, // share mode
				Arg.lpSecurityAttributes,// pointer to security attributes
				Arg.CreationDisposition, // how to create
				Arg.FlagsAndAttributes,// file attributes
				Arg.TemplateFile // handle to file with 
			//GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
		);
	}else{
		fp = wf(
				filename,
				Arg.DesiredAccess, // access (read-write) mode
				Arg.ShareMode, // share mode
				Arg.lpSecurityAttributes,// pointer to security attributes
				Arg.CreationDisposition, // how to create
				Arg.FlagsAndAttributes,// file attributes
				Arg.TemplateFile // handle to file with 
			//GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
		);

	}

	if(fp==INVALID_HANDLE_VALUE){
		//visual_lasterror();
		return NULL;
	}
	pfs->mHandle = fp;
	memcpy(&(pfs->mArg),&Arg,sizeof(pfs->mArg));

	return fp;
}


static int win32_read(void *pobj,void *pb,size_t size,size_t *readsize){
	DKC_WIN32_FS *pfs = pobj;
	HANDLE h = pfs->mHandle;
	//thread lock
	BOOL r = ReadFile(
			h, // handle of file to read
			pb, // pointer to buffer that receives data
			size, // number of bytes to read
			(LPDWORD)&readsize, // pointer to number of bytes read
			NULL//&mOL // pointer to structure for data
		);

	if(0==r){
		pfs->mLastError = GetLastError();
		
		r = edk_FAILED;
		goto End;
	}
	if (r && readsize== 0)
	{// ファイルの終わりに達した。
		//mIsEOF = true;
	}
	r = edk_SUCCEEDED;
End:
	//thread unlock
	return r;
}

static int win32_write(void *pobj,const void *pb,size_t size,size_t *writesize){
	DKC_WIN32_FS *pfs = pobj;
	

	//thread lock
	BOOL r = WriteFile(pfs->mHandle, pb, size , (LPDWORD)&writesize, NULL);
	
	if(r==FALSE){
		
		pfs->mLastError = GetLastError();
		r = edk_FAILED;
		goto End;
	}
	r = edk_SUCCEEDED;
End:
	//thread unlock
	return r;
	
}
static int win32_close(void *pobj){
	DKC_WIN32_FS *pfs = pobj;
	int r;
	if(pfs->mHandle != INVALID_HANDLE_VALUE)
	{
		if(FALSE==CloseHandle(pfs->mHandle)){
			r = edk_FAILED;
			goto End;
		}
	}
	pfs->mHandle = INVALID_HANDLE_VALUE;
	r = edk_SUCCEEDED;
End:
	return r;
}

static int win32_seek(void *pobj,const DKC_GFS_SEEK_ARG *parg){
	DKC_WIN32_FS *pfs = pobj;
	DWORD origin = 0xFFFFFFFF;
	DWORD dw;
	DWORD LastError;
	int r;

	switch(parg->origin){
	case edkcSeekSet:
		origin = FILE_BEGIN;
		break;
	case edkcSeekCurrent:
		origin = FILE_CURRENT;
		break;
	case edkcSeekEnd:
		origin = FILE_END;
		break;
	}
	dkcmNOT_ASSERT(0xFFFFFFFF==origin);
	
	//hFile ハンドルが FILE_FLAG_NO_BUFFERING を持つ場合は、
	//セクタサイズの整数倍の位置にしかファイルポインタを移動できません。
	// 事例 2 : lpDistanceToMoveHigh != NULL の場合
	dw = SetFilePointer(pfs->mHandle,
							parg->Point.u.LowPart,
							(PLONG)&(parg->Point.u.HighPart),
							origin//FILE_BEGIN
		);

	//multithread lock
	//これではダメ
	//if (dw == 0xFFFFFFFF && error())
	
	LastError = GetLastError();
	pfs->mLastError = LastError;
	if(LastError != NOERROR)
	{
		//visual_lasterror();
		//< エラー処理 >
		//return false;

		r = edk_FAILED;
		goto End;
	}
	//シーク成功したらとりあえずEOF解除･･･　ヽ(`Д´)ﾉｳﾜｧﾝ
	//mIsEOF = false;
	r = edk_SUCCEEDED;
End:
	//thread unlock
	return r;
}


static int win32_tell(void *pobj,DKC_UINT64_STRUCT *dest){
	DKC_WIN32_FS *pfs = pobj;
	LONG high = 0, low = 0;
	DKC_UINT64_STRUCT ull;
	DWORD LastError;
	int r;

	low = SetFilePointer(pfs->mHandle,
						low,
						&high, FILE_CURRENT
	);

	//multithread lock
	
	//lowと high を一緒にゲットしているので･･･　これは間違い
	//if (low == 0xFFFFFFFF && error())

	LastError = GetLastError();
	pfs->mLastError = LastError;
	if(LastError != NOERROR)
	{
		//visual_lasterror();
		//< エラー処理 >
		//return false;

		r = edk_FAILED;
		goto End;
	}
	//シーク成功したらとりあえずEOF解除･･･　ヽ(`Д´)ﾉｳﾜｧﾝ
	//mIsEOF = false;
	r = edk_SUCCEEDED;
	ull.u.LowPart = low;
	ull.u.HighPart = high;
	memcpy(dest,&ull,sizeof(*dest));
End:
	//thread unlock
	return r;

}

static int win32_delete_file(void *pobj,const void *filename_){
	DKC_WIN32_FS *pfs = pobj;
	BOOL r;
	DeleteFileA_FT af = pfs->DeleteFile;
	DeleteFileW_FT wf = pfs->DeleteFile;
	if(edkcWin32SJIS == pfs->mFlag){
		r = af(filename_);
	}else{
		r = wf(filename_);
	}
	if(FALSE==r){
		return edk_FAILED;
	}
	return edk_SUCCEEDED;
}




#define LoadFunctionGFS(funcname,dest) \
	tp = dkcGetProcAddress(dllp,funcname);\
	if(NULL==tp){\
		return FALSE;\
	}\
	dest = tp

static BOOL load_function_win32_sjis(DKC_DLL *dllp,DKC_WIN32_FS *p){
	void *tp;
	LoadFunctionGFS("CreateFileA",p->CreateFile);
	LoadFunctionGFS("SetFileAttributesA",p->SetFileAttributes);
	LoadFunctionGFS("GetFileAttributesA",p->GetFileAttributes);
	LoadFunctionGFS("DeleteFileA",p->DeleteFile);
	p->mFlag = edkcWin32SJIS;
	return TRUE;
}

static BOOL load_function_win32_unicode(DKC_DLL *dllp,DKC_WIN32_FS *p){
	void *tp;
	LoadFunctionGFS("CreateFileW",p->CreateFile);
	LoadFunctionGFS("SetFileAttributesW",p->SetFileAttributes);
	LoadFunctionGFS("GetFileAttributesW",p->GetFileAttributes);
	LoadFunctionGFS("DeleteFileW",p->DeleteFile);
	p->mFlag = edkcWin32Unicode;
	return TRUE;
}
typedef BOOL (*load_fs_func_win32_t)(DKC_DLL *,DKC_WIN32_FS *);

static DKC_WIN32_FS *alloc_win32_fs(load_fs_func_win32_t ploadfunc){
	//void *tp;
	DKC_WIN32_FS *p;
	DKC_DLL *dllp;
	
	p = dkcAllocate(sizeof(DKC_WIN32_FS));
	if(NULL==p){
		return NULL;
	}

	dllp = dkcLoadLibrary("kernel32.dll");
	if(NULL==dllp){
		goto Error;
	}

	/*
	tp = dkcGetProcAddress(dllp,"CreateFileA");
	if(NULL==tp){
		goto Error;
	}
	p->CreateFile = (CreateFileA_FT)tp;
	*/
	if(FALSE==ploadfunc(dllp,p)){
		goto Error;
	}

	p->mdllobj = dllp;
	return p;

Error:
	dkcFree(&p);
	return NULL;
}



static int free_win32_fs(void *pa){
	DKC_WIN32_FS *p = pa;

	win32_close(p);

	if(DKUTIL_FAILED(dkcUnloadLibrary(&(p->mdllobj))))
	{
		return edk_FAILED;
	}
	memset(p,0,sizeof(*p));
	return dkcFree(&p);
	//return edk_SUCCEEDED;
}

static DKC_GENERIC_FILESYSTEM *WINAPI dkcAlloc_Win32_FileSystem(BOOL isSJIS){
	void *fp;
	
	DKC_GENERIC_FILESYSTEM *pfs = dkcAllocate(sizeof(DKC_GENERIC_FILESYSTEM));
	if(NULL==pfs){
		return NULL;
	}

	if(isSJIS){
		fp = load_function_win32_sjis;
	}else{
		fp = load_function_win32_unicode;
	}

	pfs->mfp = alloc_win32_fs(fp);
	
	if(NULL==pfs->mfp){
		goto Error;
	}
	pfs->mfClose = win32_close;
	pfs->mfOpen = win32_open;
	pfs->mfRead = win32_read;
	pfs->mfSeek = win32_seek;
	pfs->mfTell = win32_tell;
	pfs->mfWrite = win32_write;
	pfs->mfDeleteFile = win32_delete_file;

	pfs->mfObjFree = free_win32_fs;
	return pfs;

Error:
	//free_sjis
	dkcFree(&pfs);
	return NULL;
}

/*
DKC_GENERIC_FILESYSTEM *WINAPI dkcAllocGenericFileSystem(){
#ifdef WIN32
	if(dkcIsOSNT()){
		return dkcAlloc_UNICODE_FileSystem();
	}
	return dkcAlloc_SJIS_FileSystem();
#else

#endif
}
*/
DKC_GENERIC_FILESYSTEM *WINAPI dkcAlloc_SJIS_FileSystem()
{
	return dkcAlloc_Win32_FileSystem(TRUE);
}



DKC_GENERIC_FILESYSTEM *WINAPI dkcAlloc_UNICODE_FileSystem()
{
	return dkcAlloc_Win32_FileSystem(FALSE);
}


#endif

int WINAPI dkcFreeGenericFileSystem(DKC_GENERIC_FILESYSTEM **pp){
	DKC_GENERIC_FILESYSTEM *p = *pp;
	DKC_GFS_FREE_F_TYPE fff;
	int r;
	
	if(NULL==pp || NULL==p){
		return edk_FAILED;
	}

	
	fff = p->mfObjFree;
	r = fff(p->mfp);
	if(DKUTIL_FAILED(r)){
		return edk_FAILED;
	}

	return dkcFree(pp);
}

int WINAPI dkcGenericFileSystemOpen(DKC_GENERIC_FILESYSTEM *ptr,uint32 flags,const void *filename){
	void *p = ptr->mfOpen(ptr->mfp,flags,filename);
	if(NULL==p){
		return edk_FAILED;
	}
	return edk_SUCCEEDED;
}

int WINAPI dkcGenericFileSystemClose(DKC_GENERIC_FILESYSTEM *ptr){
	return ptr->mfClose(ptr->mfp);
}

int WINAPI dkcGenericFileSystemRead(DKC_GENERIC_FILESYSTEM *ptr,void *pbuff,size_t size,size_t *read_size)
{
	return ptr->mfRead(ptr->mfp,pbuff,size,read_size);
}

int WINAPI dkcGenericFileSystemWrite(DKC_GENERIC_FILESYSTEM *ptr,const void *pbuff,size_t size,size_t *write_size)
{
	return ptr->mfWrite(ptr->mfp,pbuff,size,write_size);
}

int WINAPI dkcGenericFileSystemSeek(DKC_GENERIC_FILESYSTEM *ptr,const DKC_GFS_SEEK_ARG *parg)
{
	return ptr->mfSeek(ptr->mfp,parg);
}

int WINAPI dkcGenericFileSystemTell(DKC_GENERIC_FILESYSTEM *ptr,DKC_UINT64_STRUCT *offset)
{
	return ptr->mfTell(ptr->mfp,offset);
}

#ifdef _MSC_VER
#	pragma warning(default:4055 4152)
#endif
