
/*!
@file dkcMD5.c
@brief MD5 hash algorithm
@note
original: http://sourceforge.net/projects/libmd5-rfc/
reconstruct: d金魚

*/
#define DKUTIL_C_MD5_C

#include "md5.h"
#include "dkcMD5.h"
#include "dkcStdio.h"


DKC_MD5 *WINAPI dkcAllocMD5(){
	return dkcAllocMD5Ex(edkcMD5_Default);
}
DKC_MD5 *WINAPI dkcAllocMD5Ex(uint32 initflag)
{
	DKC_MD5 *p = (DKC_MD5 *)dkcAllocate(sizeof(DKC_MD5));
	if(NULL==p){
		return NULL;
	}
	switch(initflag){
		case edkcMD5_Default:
		case edkcMD5_Aladdin:
		default:
		p->mpObj = dkcAllocate(sizeof(md5_state_t));
	};
	if(NULL==p->mpObj)
	{
		dkcFree(&p);
		return NULL;
	}
	p->mObjFlag = initflag;
	dkcMD5Init(p);
	return p;
}

int WINAPI dkcFreeMD5(DKC_MD5 **pp){
	DKC_MD5 *p = *pp;
	if(NULL==pp || NULL==p){
		return edk_FAILED;
	}
	if(!p->mpObj) return edk_FAILED;
	switch(p->mObjFlag){
		case edkcMD5_Default:
		case edkcMD5_Aladdin:
		default:
		dkcFree(&(p->mpObj));
	}
	return dkcFree((void **)p);
}

void WINAPI dkcMD5Init(DKC_MD5 *p){
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		md5_init((md5_state_t *)p->mpObj);
	}
	p->mFinalized = FALSE;
}

int WINAPI dkcMD5Load(DKC_MD5 *p,const BYTE *pBuffer,DWORD dwSize){
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		if(dwSize > INT_MAX){
			return edk_FAILED;
		}
	};

	dkcMD5LoadStandard(p,pBuffer,dwSize);
	return edk_SUCCEEDED;
}

void WINAPI dkcMD5LoadStandard(DKC_MD5 *p,const BYTE *pBuffer,DWORD dwSize){
	if(p->mFinalized){
		return;
	}
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		md5_append((md5_state_t *)p->mpObj,pBuffer,(int)dwSize);
	};
}



void WINAPI dkcMD5Final(DKC_MD5 *p){
	if(p->mFinalized){
		return;
	}
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		md5_finalize((md5_state_t *)p->mpObj);
	}
	p->mFinalized = TRUE;
}

int WINAPI dkcMD5Digest(DKC_MD5 *p,BYTE *buff,size_t size){
	//if(size < 16){
	if(size < MD5_BIN_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		//第二引数のbuffのキャストがバグかもしれない。
		md5_get_digest((md5_state_t *)p->mpObj,(BYTE *)buff);
	}
	return edk_SUCCEEDED;
}

int WINAPI dkcMD5DigestStr(DKC_MD5 *p,char *buff,size_t size){
	//if(size < 32 + 1){
	if(size < MD5_STR_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	switch(p->mObjFlag){
	case edkcMD5_Default:
	case edkcMD5_Aladdin:
	default:
		md5_get_str_digest((md5_state_t *)p->mpObj,(char *)buff);
	}
	return edk_SUCCEEDED;
}

int WINAPI dkcMD5FinalDigestStr(DKC_MD5 *p,char *buff,size_t size){
	dkcMD5Final(p);
	return dkcMD5DigestStr(p,buff,size);
}

int WINAPI dkcMD5FinalDigest(DKC_MD5 *p,BYTE *buff,size_t size){
	dkcMD5Final(p);
	return dkcMD5Digest(p,buff,size);

}


