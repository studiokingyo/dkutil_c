
/*!
@file dkcSemiRealRandom.c
@brief		ほぼ真性な乱数を生成
@author		d金魚
@since		2005/02/20
@see dkcSemiRealRandom.h
*/

#include "dkcSemiRealRandom.h"
#include "dkcOS.h"
#include "dkcStdio.h"

#ifdef _MSC_VER
#	pragma warning(disable:4055)
#endif



#define dkcdSRR_PATH_SIZE (sizeof(wchar_t) * dkcdMAXPATH_BUFFER)

typedef 
DWORD (WINAPI *GetTempPathA_FT)(
    DWORD nBufferLength,
    LPSTR lpBuffer
    );

typedef 
DWORD (WINAPI *GetTempPathW_FT)(
    DWORD nBufferLength,
    LPWSTR lpBuffer
    );


typedef 
UINT (WINAPI *GetTempFileNameA_FT)(
    LPCSTR lpPathName,
    LPCSTR lpPrefixString,
    UINT uUnique,
    LPSTR lpTempFileName
    );

typedef 
UINT (WINAPI *GetTempFileNameW_FT)(
    LPCWSTR lpPathName,
    LPCWSTR lpPrefixString,
    UINT uUnique,
    LPWSTR lpTempFileName
    );


static DKC_INLINE int load_dll(DKC_SEMI_REAL_RANDOM *p,BOOL isNT){
	DKC_DLL *pdll = dkcLoadLibrary("kernel32.dll");
	if(NULL==pdll)
		return edk_FAILED;

	if(isNT){
		p->fpGetTempFileName =	dkcGetProcAddress(pdll,"GetTempFileNameW");
		p->fpGetTempPath =			dkcGetProcAddress(pdll,"GetTempPathW");
	}else{
		p->fpGetTempFileName = dkcGetProcAddress(pdll,"GetTempFileNameA");
		p->fpGetTempPath =			dkcGetProcAddress(pdll,"GetTempPathA");
	}
	//p->fpQueryPerformanceCounter = dkcGetProcAddress(pdll,"QueryPerformanceCounter");
	dkcmFORCE_NOT_ASSERT(
		p->fpGetTempFileName == NULL ||
		p->fpGetTempPath == NULL
	);

	p->pDLL = pdll;
	return edk_SUCCEEDED;
}

DKC_SEMI_REAL_RANDOM *WINAPI dkcAllocSemiRealRandomWin32()
{
	DKC_SEMI_REAL_RANDOM *ptr;
	DKC_GENERIC_FILESYSTEM *pfs;
	BOOL isNT = dkcIsOSNT();
	if(isNT)
	{//Windows NT
		pfs = dkcAlloc_UNICODE_FileSystem();
	}else{
		pfs = dkcAlloc_SJIS_FileSystem();
	}
	
	ptr = dkcAllocSemiRealRandomFromGFS(pfs);
	if(NULL==ptr){
		dkcFreeGenericFileSystem(&pfs);
		return NULL;
	}
	//余分に確保しておく･･･
	ptr->pPath = dkcAllocate(dkcdSRR_PATH_SIZE);
	if(NULL==ptr->pPath){
		goto End;
	}

	if(DKUTIL_FAILED(load_dll(ptr,isNT))){
		goto End;
	}
	
	ptr->mIsNT = isNT;
	//successful
	return ptr;
End:
	dkcFreeSemiRealRandom(&ptr);
	return NULL;

	/*
	DKC_SEMI_REAL_RANDOM *ptr;
	pfs = dkcAlloc_SJIS_FileSystem();
	
	ptr = dkcAllocSemiRealRandomFromGFS(pfs);
	if(NULL==ptr){
		dkcFreeGenericFileSystem(&pfs);
		return NULL;
	}
	//successful
	return ptr;*/
}


DKC_SEMI_REAL_RANDOM *WINAPI dkcAllocSemiRealRandomFromGFS(DKC_GENERIC_FILESYSTEM *pa)
{
	DKC_SEMI_REAL_RANDOM *p;
	if(NULL==pa){
		return NULL;
	}
	p = dkcAllocate(sizeof(DKC_SEMI_REAL_RANDOM));
	if(NULL==p){
		return NULL;
	}
	p->mpObj = pa;
	return p;
}


int WINAPI dkcFreeSemiRealRandom(DKC_SEMI_REAL_RANDOM **pp)
{
	DKC_SEMI_REAL_RANDOM *p = *pp;
	if(NULL==pp || NULL==p)
		return edk_FAILED;
	
	dkcFreeGenericFileSystem(&(p->mpObj));
	return dkcFree(pp);
}

static DKC_INLINE int process(DKC_GENERIC_FILESYSTEM *gfs,const void *filename){
	size_t ws;
	char buff[1];
	int r = dkcGenericFileSystemOpen(gfs,edkcBinaryMode | edkcWriteMode,filename); 
	if(DKUTIL_FAILED(r))	return r;

	dkcGenericFileSystemWrite(gfs,buff,sizeof(buff),&ws);
	
	dkcGenericFileSystemClose(gfs);

	dkcGenericFileSystemDeleteFile(gfs,filename);
	return edk_SUCCEEDED;
}

static BOOL generate(DKC_SEMI_REAL_RANDOM *ptr,uint32 *value32){
#ifdef WIN32
	DKC_INT64_STRUCT hd;
	uint32 temp;
	QueryPerformanceCounter(&hd); 
	temp = hd.u.LowPart;

	if(DKUTIL_FAILED(process(ptr->mpObj,ptr->pPath))){
		return FALSE;
	}

	QueryPerformanceCounter(&hd);
	*value32 = (hd.LowPart ^ temp);
	return TRUE;
#else


#endif
}

///@return エラーで0xFFFFFFFF
static uint32 test(DKC_SEMI_REAL_RANDOM *ptr,size_t test_count,size_t min_,size_t max_)
{
	uint8 bits[dkcdSEMI_REAL_RANDOM_BITS];
	uint8 prior_bits[dkcdSEMI_REAL_RANDOM_BITS];
	uint32 various[dkcdSEMI_REAL_RANDOM_BITS];
	uint32 val;
	int randRshift,randLshift;
	int i;
	size_t j;

	memset(various,0,sizeof(uint32) * dkcdSEMI_REAL_RANDOM_BITS);

	for(j=0;j<test_count;j++)
	{
		if(FALSE==generate(ptr,&val))
			return 0xFFFFFFFF;

		for(i=0;i<dkcdSEMI_REAL_RANDOM_BITS;i++)
		{
			bits[i]=(uint8)(val & 1);
			val = val >> 1;
			various[i] += ( bits[i] != prior_bits[i] );
			prior_bits[i]=bits[i];
		}
	}

	randRshift=255;
	randLshift=-1;
	for(i=0;i<dkcdSEMI_REAL_RANDOM_BITS;i++)
	{
		if((various[i]>=min_)&&(various[i]<=max_))
		{
			if(i < randRshift)
				randRshift=i;
			
			if(i >= randLshift)
				randLshift=i-randRshift+1;
		}
		else break;
	}
	if(randRshift==255)	return 0xFFFFFFFF;
	if(randLshift<=0)		return 0xFFFFFFFF;
	
	ptr->mLeftShift = randLshift;
	ptr->mRightShift = randRshift;

	return (uint32)randLshift;
}

int WINAPI dkcSemiRealRandomInit(DKC_SEMI_REAL_RANDOM *ptr)
{
#ifdef WIN32

	GetTempPathA_FT pGTPA;
	//char *pstr;

	GetTempPathW_FT pGTPW;
	//wchar_t *pwstr;
	
	if(ptr->mIsNT){
		pGTPW = (GetTempPathW_FT)ptr->fpGetTempPath;
		dkcmFORCE_NOT_ASSERT(dkcdMAXPATH < pGTPW(0,ptr->pPath));

		if(pGTPW(dkcdMAXPATH,ptr->pPath)==0)
			 return edk_FAILED; //テンポラリファイルフォルダを取得

		//仮のテンポラリファイル。ちゃんとやるならGetTempFileNameで。
		if(DKUTIL_FAILED(dkc_wcscat2(ptr->pPath,dkcdSRR_PATH_SIZE,dkcdSRR_TEMP_FILE_NAME_W)))
		{ 
			return edk_FAILED;
		}

	}else{//Win9x
		pGTPA = (GetTempPathA_FT)ptr->fpGetTempPath;
		dkcmFORCE_NOT_ASSERT(dkcdMAXPATH < pGTPA(0,ptr->pPath));

		if(pGTPA(dkcdMAXPATH,ptr->pPath)==0)
			 return edk_FAILED; //テンポラリファイルフォルダを取得

		//仮のテンポラリファイル。ちゃんとやるならGetTempFileNameで。
		if(DKUTIL_FAILED(dkc_strcat2(ptr->pPath,dkcdSRR_PATH_SIZE,dkcdSRR_TEMP_FILE_NAME)))
		{ 
			return edk_FAILED;
		}
	}
	{
		int i;
		for(i=0;i<dkcdSEMI_REAL_RANDOM_TRIAL_AND_ERROR_LIMIT;)//64回も行って成功しないは無いだろう･･･
		{
			if(0xFFFFFFFF==test(ptr,128,40,60)){
				i++;
			}else{
				break;
			}
		}
		if(!(i<64))
		{
			return edk_FAILED;
		}
	}

	return edk_SUCCEEDED;
#else


#endif

}



BOOL WINAPI dkcSemiRealRandomGet32(DKC_SEMI_REAL_RANDOM *ptr,uint32 *value32)
{
	uint32 tmp=0;
	uint32 val;
	int totalbit=0;
	size_t i,j;
	for(j=0;j<dkcdSEMI_REAL_RANDOM_TRIAL_AND_ERROR_LIMIT;j++)
	{
		if(FALSE==generate(ptr,&val))
			return FALSE;

		val = val >> ptr->mRightShift;
		for(i=0;i<ptr->mLeftShift;i++)
		{
			tmp+=val & 1;
			val=val >> 1;
			totalbit++;
			if(totalbit>=32){
				*value32 = tmp;
				goto End;
			}
			tmp=tmp<<1;
		}
	}

End:
	if(!(j<dkcdSEMI_REAL_RANDOM_TRIAL_AND_ERROR_LIMIT))
	{
		return FALSE;
	}
	return TRUE;
}

uint8 WINAPI dkcSemiRealRandomGet1(DKC_SEMI_REAL_RANDOM *ptr)
{
#ifdef WIN32
	uint32 val;
	generate(ptr,&val);
	return (uint8) ( (val >> ptr->mRightShift ) & 1) ;

#else


#endif



}



#ifdef _MSC_VER
#	pragma warning(default:4055)
#endif

