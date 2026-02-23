
/*!
@file dkcSJISFileSystem.c
@author d����
@
*/

#define DKUTIL_C_SJIS_FILESYSTEM_C

#ifdef WIN32
#include <SYS/STAT.H>
#endif
#include "dkcOSIndependent.h"
#include "dkcSJISFileSystem.h"
#include "dkcStdio.h"
#include "dkcString.h"
#include "dkcThreadLock.h"
#include "dkcSingleList.h"
#include "dkcStream.h"



static DKC_INLINE BOOL jms1(int c){
	return (((((unsigned char)(c))>=0x81)&&(((unsigned char)(c))<=0x9F))||((((unsigned char)(c))>=0xE0)&&(((unsigned char)(c))<=0xFC)));
}
static DKC_INLINE BOOL jms2(int c){
	return ((((unsigned char)(c))!=0x7F)&&(((unsigned char)(c))>=0x40)&&(((unsigned char)(c))<=0xFC));
}

/*!
 ������ str �� str[nPos] �ɂ��āA<br>
 @return
   �O �c�c �P�o�C�g����<br>
   �P �c�c �Q�o�C�g�����̈ꕔ�i��P�o�C�g�j<br>
   �Q �c�c �Q�o�C�g�����̈ꕔ�i��Q�o�C�g�j<br>
<br>

�V�t�g�i�h�r������̔�������܂��B
*/ 
DKC_EXTERN int WINAPI dkcIsShiftJIS( const char *str, int nPos )
{
	int i;
	int state; // { 0, 1, 2 } = { �P�o�C�g����, �Q�o�C�g�����̑�P�o�C�g, �Q�o�C�g�����̑�Q�o�C�g }

	state = 0;
	for( i = 0; str[i] != '\0'; i++ )
	{
		if      ( ( state == 0 ) && ( jms1( str[i] ) ) ) state = 1; // 0 -> 1
		else if ( ( state == 1 ) && ( jms2( str[i] ) ) ) state = 2; // 1 -> 2
		else if ( ( state == 2 ) && ( jms1( str[i] ) ) ) state = 1; // 2 -> 1
		else                                             state = 0; // 2 -> 0, ���̑�

		// str[nPos] �ł̏�Ԃ�Ԃ��B
		if ( i == nPos ) return state;
	}
	return 0;
}
static DKC_INLINE int isJMS(const char *str, int nPos ){
	return dkcIsShiftJIS(str,nPos);
}
/// �����񖖔��̕��� '\0' �̈ʒu��Ԃ�
static DKC_INLINE char *strtail( const char *stringg )
{
	return strchr( stringg, '\0' );
}//strtail

/// �p�X������A�g���q�̊J�n�ʒu���擾����
/// �t�@�C�����̖�������t�@�C�����̍Ō�� '.' �܂ł��A�g���q�Ɣ��f����
///@return NULL���Ɗg���q�͔��ʕs�\�B�܂��A�^����ꂽ������̍ŏ��̃A�h���X���Ԃ��ꂽ�ꍇ�����ʕs�\
///@note SJIS�΍�ς݁H
const char * WINAPI dkcGetFileExtension( const char *PathName )
{

	char *p;
	char *get_tail;

	get_tail = strtail( PathName );
	for( p = get_tail; p >= PathName; p-- ) // ������̍Ōォ��ŏ��܂�
	{
		if ( ('\\'==*p) && !isJMS(PathName,p-PathName) )
			return get_tail; // �t�@�C�����͂����܂�

		if ( '.' == *p )
			return p+1; // '.' �𔭌�
	}
	return get_tail; // �g���q�Ȃ�
}//GetFileExtension


BOOL WINAPI dkcIsEffectivePath(const char *path,size_t size){
	char dest[dkcdMAXPATH_BUFFER];
	const size_t dsize = dkcdMAXPATH_BUFFER;

	//���΃p�X�H��΃p�X�H�ł����̃t�@�C�������݂��邩�H
	if(FALSE==dkcFileExist(path)){
		return FALSE;
	}
	//path�𐳋K��
	if(DKUTIL_FAILED(dkcToAbsolutelyPath(dest,dsize,path,size))){
		return FALSE;
	}
	//���K�����āA�t�@�C���Ɏg�p���ėǂ���������g���Ă��邩�H
	if(FALSE==dkcIsNativePathString(dest,strlen(dest))){
		return FALSE;
	}

	return TRUE;
}
BOOL WINAPI dkcIsRelativityPath(const char *path)
{
	int point;
	dkcmNOT_ASSERT(NULL==path);
	point = dkcSJIS_StrChrSearch(path,':');
	if(point == -1) return TRUE;
	return FALSE;
}
BOOL WINAPI dkcIsAbsolutelyPath(const char *path)
{
	return !dkcIsRelativityPath(path);
}

///������̌�낪PathSeparator��������TRUE
BOOL WINAPI dkcIsTailPathSep(const char *src,size_t dlen){
	int point;
	point = dkcSJIS_SearchPathSepLast(src);
	//if(point != -1 && (size_t)point == dlen - 1/*&& (size_t)point != dlen*/)
	if((size_t)point == dlen - 1)
	{	//�݂������B
		return TRUE;
	}
	//�݂���Ȃ��B
	return FALSE;
}
///@return path separator��}������K�v������A�}���o������TRUE
int WINAPI dkcPushBackPathSep(char *dest,size_t dlen,size_t size){
	if(FALSE==dkcIsTailPathSep(dest,dlen)){
		if(size < dlen + 2){
			return edk_OutputBufferWasLost;//�o�b�t�@��������I
		}
		dest[dlen ] = dkcdPATH_SEP;
		dest[dlen + 1] = '\0';//������Y��Ă͂����Ȃ��B
		return edk_SUCCEEDED;
	}
	return edk_EndProcess;
}

int WINAPI dkcDirectoryConcatenate(char *dest,size_t dlen,size_t dsize,const char *src){
	//int point;
	//error check
	dkcmNOT_ASSERT(dlen + 2 > dsize);
	if(dlen + 2 > dsize){
		return edk_FAILED;
	}
	
	/*point = dkcSJIS_StrChrSearchTail(dest,dlen,dkcdPATH_SEP);
	if(point != -1 && (size_t)point != dlen)
	{//path separator��������A���ꂪdest�̍Ŋ��ɖ�����
		if(!dkcmIS_PATH_SEP(src[0]) )
		{//path separator�Ŗ�����
			//path ��؂蕶����t�����Ă��B
			dest[dlen ] = dkcdPATH_SEP;
			dest[dlen + 1] = '\0';//������Y��Ă͂����Ȃ��B
		}
	}*/
	dkcPushBackPathSep(dest,dlen,dsize);
	return dkc_strcat_amap(dest,dsize,dlen,src,strlen(src));
}

int WINAPI dkcCurrentDirectoryConcatenate(char *dest,size_t dsize,const char *src)
{
	//size_t len;
	//int point;
	
	dkcmNOT_ASSERT(dsize <= dkcdMAXPATH_LEN);//<=��NULL�������܂ނ��߁B
	
	dkcGetCurrentDirectory(dest,dsize);
	
	
	return dkcDirectoryConcatenate(dest,strlen(dest),dsize,src);
}
#define MAX_PATH_CHECK(dsize) \
{\
	dkcmNOT_ASSERT(dsize < dkcdMAXPATH_BUFFER);\
	if(dsize < dkcdMAXPATH_BUFFER){\
		return edk_BufferOverFlow;\
	}\
}

static int ToAbsolutelyLogic(char *dest,size_t dsize,const char *src)
{
	/*if(dsize < dkcdMAXPATH_BUFFER){//����ȃo�b�t�@����ǂ��Ȃ邩������Ȃ���O�O�G
		return edk_BufferOverFlow;
	}*/
	MAX_PATH_CHECK(dsize);
#	ifdef WIN32

	if(NULL==_fullpath(dest,src,dsize)){
		return edk_FAILED;
	}
#	else//unix or linux ??
	if(NULL==__realpath(src,dest)){
		return edk_FAILED;
	}
#	endif
	return edk_SUCCEEDED;	
}
//�p�X�𐳋K������֐�
int WINAPI dkcToAbsolutelyPath(char *dest,size_t dsize,const char *src,size_t ssize)
{
	char *tp = NULL;
	int r;
	MAX_PATH_CHECK(dsize);

	
	if(NULL==dest) return edk_FAILED;

	if(dkcIsRelativityPath(src)==TRUE)
	{//�J�����g�f�B���N�g����t������B

		tp = (char *)malloc(dkcdMAXPATH_BUFFER);
		if(NULL==tp) return edk_OutOfMemory;

		dkcCurrentDirectoryConcatenate(tp,dkcdMAXPATH_BUFFER,src);
		r =  ToAbsolutelyLogic(dest,dsize,tp);
		free(tp);
		return r;

	}
	return ToAbsolutelyLogic(dest,dsize,src);


	
	/*
	��ԑ@��
	PATH_SEP
	.
	sjis

	/////
	.....
	../../
	/././

	*/
		/*
	size_t i,di;
//	size_t j;
	int point;
	//char prev;
	const char *pathsep="\\/";
	DKC_SINGLELIST_OBJECT *plo = NULL;
	DKC_SINGLELIST *lit = NULL;
	
	
	di = 0;

	if(dsize < ssize){
		return edk_ArgumentException;
	}
	//list�m��
	plo = dkcAllocSingleListObject(NULL,20);
	if(NULL==plo) return edk_FAILED;

	//directory���p�[�X
	for(i=0;;){
		point = dkcSJIS_StrChrSearchInStr(&src[i],pathsep);
		if(point < 0){
			if(i != ssize){
				if(FALSE==plo->push_back(plo,&src[i],size - i)){
					return edk_FAILED;
				}
			}
			break;
		}
		
		if(FALSE
			==plo->push_back(plo,&src[i],point - 1)//���������Ƃ���܂ł�����-1
			)
		{
			return edk_FAILED;
		}

		i += point;
	}

	for(i=0;i<ssize;){


		if(dkcmIsSJIS1(src[i]) && dkcmIsSJIS2(src[i + 1]))
		{//SJIS������
			i++;
			i++;
			continue;
		}
		// src == //
		if(prev==dkcdPATH_SEP && src[i] == dkcdPATH_SEP)
		{
			i++;
			continue;
		}
		// src == ..
		if(prev=='.' && src[i] == '.')
		{
			while(src[i] != '.') i++;
			if(dkcdPATH_SEP==src[i]) i++;
			point = dkcSJIS_StrChrSearch(&src[i],dkcdPATH_SEP);

			if(point == -1){
				break;
			}
			i += point;

			continue;
		}

		dest[di] = src[i];
		di++;
		i++;
		prev = src[i];
	}

	
	return edk_SUCCEEDED;	*/
}


DKC_PATHSTRING * WINAPI dkcAllocPathString(const char *path)
{
	DKC_PATHSTRING *p;
	size_t len;
	/*if(NULL==path){
		return NULL;
	}*/

	p = (DKC_PATHSTRING *)dkcAllocate(sizeof(DKC_PATHSTRING));
	if(NULL==p) return NULL;

	p->mString = dkcAllocString(dkcdMAXPATH_BUFFER + 1);//���̃T�C�Y����Ȃ��ƃ_��
	if(NULL==p->mString) goto Error;

	//�p�X�����R�s�[
	if(path){
		len = strlen(path);
		//�l�C�e�B�u�ȃp�X������B
		if(FALSE==dkcIsNativePathString(path,len)){
			goto Error;
		}
		if(DKUTIL_FAILED(dkcPathStringCopy(p,path,len))){
			goto Error;
		}
	}
	p->mIterateCount = 0;

	return p;
Error:
	if(p){
		dkcFreeString(&p->mString);
	}
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreePathString(DKC_PATHSTRING **ptr)
{
	if(NULL==ptr || NULL==*ptr){
		return edk_ArgumentException;
	}
	dkcFreeString(&((*ptr)->mString));
	return dkcFree((void **)ptr);
}

//�p�X�̃f�[�^�𓾂�B
size_t WINAPI dkcPathStringSize(const DKC_PATHSTRING *p)
{
	return dkcStringSize(p->mString);
}

const char *WINAPI dkcPathStringPointer(const DKC_PATHSTRING *p)
{
	return dkcStringPointer(p->mString);
}


//�p�X����؂�B

int WINAPI dkcPathStringDevideBegin(DKC_PATHSTRING *ptr,char *buff,size_t size)
{
	return dkcPathStringDevideBegin_Logic(ptr,&ptr->mIterateCount,buff,size);
}

int WINAPI dkcPathStringDevideNext(DKC_PATHSTRING *ptr,char *buff,size_t size)
{
	return dkcPathStringDevideNext_Logic(ptr,&ptr->mIterateCount,buff,size);

}

void WINAPI dkcPathStringDevideEnd(DKC_PATHSTRING *ptr){
	dkcPathStringDevideEnd_Logic(&ptr->mIterateCount);
}


int WINAPI dkcPathStringDevideBegin_Logic(DKC_PATHSTRING *ptr,size_t *count,char *buff,size_t size)
{
	int i,point;
	const char *p;
	dkcmNOT_ASSERT(NULL==ptr || NULL==buff || 0==size);
	
	//len = dkcPathStringSize(ptr);
	p = dkcPathStringPointer(ptr);

	point = dkcSJIS_StrChrSearch(p,'\\');
	if(-1==point){return edk_EndProcess;}

	for(i=0;i<point;i++){//drive�w��̂��̂����邩�ǂ����B
		if(':'==p[i]){
			if(DKUTIL_FAILED(dkc_strcpy(
				buff,size,p,(size_t)i//-1
				)))
			{
				return edk_BufferOverFlow;
			}
			point = dkcSJIS_StrChrSearch(&p[i],'\\');//���̕���������B
			//�i�܂�:�̎���\\�ƌ����������܂��Ă��邯�ǁA���������B
			*count = (size_t)i + point + 1;//���Ɏn�߂�̂�
			return edk_SUCCEEDED;
		}
	}
	if(DKUTIL_FAILED(dkc_strcpy(
		buff,size,p,(size_t)point-1
		)))
	{
		return edk_FAILED;
	}
	*count = (size_t)point + 1;//���Ɏn�߂�̂�
	return edk_SUCCEEDED;
}

int WINAPI dkcPathStringDevideNext_Logic(DKC_PATHSTRING *ptr,size_t *count,char *buff,size_t size)
{
	int point;
	const char *p;
	size_t len;
	//len = dkcPathStringSize(ptr);
	p = dkcPathStringPointer(ptr);
	len = dkcStringSize(ptr->mString);
	if(len <= *count)
	{//�������̊֐����Ăяo���̂͏I��肾�����イ�ɁI�I
		return edk_EndProcess;
	}
	point = dkcSJIS_StrChrSearch(&p[*count],'\\');
	if(-1==point)
	{//�Ō�܂ŗ����܂����B

	
		len -= *count;
		if(DKUTIL_FAILED(dkc_strcpy(//�c�蕨��S���R�s�[
			buff,size,&p[*count],len
		)))
		{
			return edk_FAILED;
		}
		*count += len;
		return edk_SUCCEEDED;//out
	}
	if(DKUTIL_FAILED(dkc_strcpy(
		buff,size,&p[*count],(size_t)point
		)))
	{
		return edk_FAILED;
	}
	*count += (size_t)point + 1;//���Ɏn�߂�̂�
	return edk_SUCCEEDED;//out
}

void WINAPI dkcPathStringDevideEnd_Logic(size_t *count){
	*count = 0;
}

int WINAPI dkcPathStringElementInsert_Logic(DKC_PATHSTRING *ptr,size_t count,
					const char *src,size_t len)
{
	int r;
	size_t size = len + 5;
	
	char *p;
	if(len==0 || FALSE==dkcIsNativePathString(src,len))
	{
		return edk_FAILED;
	}
	if(FALSE==dkcIsTailPathSep(src,len))//����PathSeparator�������I�I
	{
		p = (char *)malloc(size);
	
		if(!p) return edk_OutOfMemory;
		strcpy(p,src);//���܂�g�������Ȃ����ǂ��łɃo�b�t�@�͕ۏ؂���Ă���̂�strcpy
		dkcPushBackPathSep(p,len,size);

		r = dkcStringInsert(ptr->mString,count,p,strlen(p));
		free(p);
	}else{
		r = dkcStringInsert(ptr->mString,count,src,len);
	}
	return r;
}

int WINAPI dkcPathStringElementErase_Logic(
	DKC_PATHSTRING *ptr,size_t count)
{
	const char *p = dkcPathStringPointer(ptr);
	int endlen = dkcSJIS_SearchPathSep(&p[count]);

	if(-1==endlen){
		endlen = dkcPathStringSize(ptr);
		endlen = endlen - count;
	}else{
		//endlen;//�}�C�i�X���Ă��� \\��Replace���Ȃ�����
	}
	return dkcStringErase(ptr->mString,count - 1,(size_t)endlen + 1);//count -1�͑O��\\����������endlen + 1��-1������
}



int WINAPI dkcPathStringElementReplace_Logic(DKC_PATHSTRING *ptr,size_t count,
																			const char *src,size_t len)
{
	const char *p = dkcPathStringPointer(ptr);
	int endlen;
	if(len==0 || FALSE==dkcIsNativePathString(src,len))
	{
		return edk_FAILED;
	}
	endlen = dkcSJIS_SearchPathSep(&p[count]);//count���猟�����Ă���
	if(-1==endlen){
		endlen = dkcPathStringSize(ptr);
		endlen = endlen - count;
	}else{
		if(0 != endlen)
			endlen--;//�}�C�i�X���Ă��� \\��Replace���Ȃ�����
	}
	return dkcStringReplace(ptr->mString,count,count + endlen,src,len);
	/*
	size_t size = len + 5;
	int r;
//	int point;
	const char *pointer;
	char *p ;
	if(FALSE==dkcIsTailPathSep(src,len))//����PathSeparator�������I�I
	{
		//����������ƕύX
		p = malloc(size);
		if(!p) return edk_OutOfMemory;
		strcpy(p,src);
		r = dkcPushBackPathSep(p,len,size);
	}else{
		p = (char *)src;
	}
	pointer = dkcPathStringPointer(ptr);
	//count���璲�ׂ�B
	dkcSJIS_SearchPathSep(&pointer[count]);
	*/
	


	

}





//�p�X�𐳋K�����ē����B
static int dkcPathStringNormalizeCopyLogic(DKC_PATHSTRING *ptr,const char *buff,size_t size,
																						 int (WINAPI *function__)(DKC_STRING *,const char *,size_t))
{
	//const size_t bsize = dkcdMAXPATH_BUFFER + 1;
	size_t len;
	int result;
	//char *pb = malloc(bsize);
	//if(NULL== pb) return edk_OutOfMemory;
	char pb[dkcdMAXPATH_BUFFER];
	size_t bsize = sizeof(pb);

	result = 	dkcToAbsolutelyPath(pb,bsize,buff,size);
	

	len = strlen(pb);

#	ifdef DEBUG //���肦�Ȃ���`�G���[�`�F�b�N
	dkcmNOT_ASSERT(DKUTIL_FAILED(result));	
	dkcmNOT_ASSERT(len >= bsize);// >=�ɂ���̂�NULL�������ׂ���Ă��邩�ǂ����B
	dkcmNOT_ASSERT(NULL==function__);
#	endif
	if(DKUTIL_FAILED(result)){
		goto Error;
	}
	result = function__(ptr->mString,pb,len);

Error:
	//if(pb){free(pb);pb=NULL;}
	return result;
}
//�p�X�𐳋K�����ē����B
int WINAPI dkcPathStringCopy(DKC_PATHSTRING *ptr,const char *buff,size_t size)
{
	
	dkcmNOT_ASSERT(ptr->mString->mByteSize + size >= dkcdMAXPATH);
	if(ptr->mString->mByteSize + size >= dkcdMAXPATH)
	{//����ȏ�͓�����Ȃ��B
		return edk_FAILED;
	}
	return dkcPathStringNormalizeCopyLogic(ptr,buff,size,dkcStringCopy);
}

int WINAPI dkcPathStringNormalizeConcatenateLogic(
	DKC_PATHSTRING *ptr,const char *buff,size_t size)
{
	char dest[dkcdMAXPATH_BUFFER];


	if(FALSE==dkcIsTailPathSep(dkcPathStringPointer(ptr),dkcPathStringSize(ptr)))
	{
		dkcStringConcatenate(ptr->mString,dkcdPATH_SEP_STR,1);
	}
	dkcStringConcatenate(ptr->mString,buff,size);
	
	size = dkcPathStringSize(ptr) + 1;

	//p = malloc(dkcPathStringSize(ptr));
	//if(!p) return edk_OutOfMemory;


	if(DKUTIL_FAILED(
		ToAbsolutelyLogic(dest,sizeof(dest),dkcPathStringPointer(ptr))
		)){
		return edk_FAILED;
	}

	return dkcPathStringCopy(ptr,dest,strlen(dest));
}

//�p�X�𐳋K�����Čq����B
int WINAPI dkcPathStringConcatenate(DKC_PATHSTRING *ptr,const char *buff,size_t size)
{
	int result;

	dkcmNOT_ASSERT(ptr->mString->mByteSize + size >= dkcdMAXPATH);
	if(ptr->mString->mByteSize + size >= dkcdMAXPATH)
	{//����ȏ�͓�����Ȃ��B
		return edk_FAILED;
	}

	if(ptr->mString->mByteSize)
	{//���łɓ����Ă�����̂�����B
		result = dkcPathStringNormalizeConcatenateLogic(ptr,buff,size);
	}
	else
	{//�����������͂������
		result =  dkcPathStringCopy(ptr,buff,size);
	}
	//return dkcPathStringNormalizeInsertLogic(ptr,buff,size,dkcStringConcatenate);	
	return result;
}



int WINAPI dkcPathStringGetDrive(DKC_PATHSTRING *ptr,char *buff,size_t size){
	const char *p = dkcStringPointer(ptr->mString);
	int point = dkcSJIS_StrChrSearch(p,':');
	if(-1 == point) return edk_Not_Found;//������Ȃ��B
	//return dkc_memcpy(buff,size,p,(size_t)point - 1);
	return dkc_strcpy(buff,size,p,(size_t)1);//drive�Ȃ�1�����ŏ\������ˁO�O�G
}

int WINAPI dkcPathStringGetFileExtension(DKC_PATHSTRING *ptr,char *buff,size_t size)
{
	int point2;
	size_t len;
	const char *p = dkcStringPointer(ptr->mString);
	int point = dkcSJIS_StrChrSearchLast(p,'.');

	if(point < 0) return edk_Not_Found;//������Ȃ��B
	
	point2 = dkcSJIS_SearchPathSep(&p[point]);
	if(point < point2){//C:\ok.OK\folder�̂悤�ȏ󋵂ɂȂ��Ă���B
		return edk_Not_Found;
	}
	len = dkcStringSize(ptr->mString);
	//if(point - 1 <= 0) return edk_FAILED;

	if((size_t)(point + 1) > len) return edk_FAILED;//Logical�ȃG���[�H�H
	return dkc_strcpy(buff,size,&p[point + 1],(size_t)len - (size_t)point );

}

int WINAPI dkcPathStringGetFileName(DKC_PATHSTRING *ptr,char *buff,size_t size)
{
	const char *p = dkcStringPointer(ptr->mString);
	//int point = dkcSJIS_StrChrSearchLast(p,dkcdPATH_SEP);
	int point = dkcSJIS_SearchPathSepLast(p);
	size_t len = dkcStringSize(ptr->mString);
	//if(point - 1 <= 0) return edk_FAILED;
#if 0
	if(point < 0) return edk_Not_Found;//������Ȃ��B
	if((size_t)(point + 1) > len) return edk_FAILED;
	if((size_t)point == len) return edk_FAILED;//�t�@�C�����������B

#else
	printf("%d",point);

	dkcmFORCE_NOT_ASSERT(NULL==p);
	dkcmFORCE_NOT_ASSERT(point < 0);
	dkcmFORCE_NOT_ASSERT((size_t)(point + 1) > len);
	dkcmFORCE_NOT_ASSERT((size_t)point == len);

#endif
	return dkc_strcpy(buff,size,&p[point + 1],(size_t)len - (size_t)point );
}

int WINAPI dkcPathStringGetDirectory(DKC_PATHSTRING *ptr,char *buff,size_t size)
{
	const char *p = dkcStringPointer(ptr->mString);
	int point = dkcSJIS_StrChrSearchTail(p,strlen(p),dkcdPATH_SEP);
	size_t len = dkcStringSize(ptr->mString);
	//if(point - 1 <= 0) return edk_FAILED;
	if(point < 0) return edk_FAILED;
	if((size_t)(point + 1) > len) return edk_FAILED;
	//if((size_t)point == len) return edk_FAILED;//�t�@�C�����������B
	return dkc_strcpy(buff,size,p,point);//\\���܂߂�B

}


/*
BOOL WINAPI dkcFileAndFolderExist(const char *filename)
{
	struct stat s;
	if(!filename) return FALSE;
	return (stat(filename,&s)==0);
}
*/
static DKC_INLINE file_exist_base(const char *filename,BOOL with_file,BOOL with_folder){
	struct stat s;
	if(!filename) return FALSE;
	if(!(stat(filename,&s)==0)) return FALSE;
	//from MSDN //folder
	if(s.st_mode & S_IFDIR){
		return with_folder;
	}
	return with_file;
}

BOOL WINAPI dkcFileExist(const char *filename)
{
	return file_exist_base(filename,TRUE,FALSE);
}

BOOL WINAPI dkcFileAndFolderExist(const char *filename)
{
	return file_exist_base(filename,TRUE,TRUE);
}

BOOL WINAPI dkcFolderExist(const char *filename)
{
	return file_exist_base(filename,FALSE,TRUE);
}



ULONG WINAPI dkcFileSize(const char *filename){
	struct stat s;//s
	if(!filename) return 0;
  return (stat(filename,&s)==0) ? (ULONG)s.st_size : 0;
}
BOOL WINAPI dkcFileSize64(const char *str,DWORD *high,DWORD *low){
#ifdef WIN32
	WIN32_FIND_DATA findData;
	HANDLE hFind=NULL;
	// �t�@�C���T�C�Y�擾
	if((hFind = FindFirstFile(str,&findData)) == INVALID_HANDLE_VALUE){
		return FALSE;
	}

	//�t�@�C���̃T�C�Y����������
	*high = findData.nFileSizeHigh;
	*low = findData.nFileSizeLow;
	FindClose(hFind);
	return TRUE;
#else

#endif
}


BOOL WINAPI dkcSetCurrentDirectory(const char *filename){
#ifdef DEBUG
	size_t len = strlen(filename);
	dkcmNOT_ASSERT(0==len || FALSE==dkcIsEffectivePath(filename,len));
#endif
#	ifdef WIN32
	return(0 != SetCurrentDirectory(filename));
	//return (_chdir(filename)==0);
#	else
	return (chdir(filename)==0);
#	endif
}


BOOL WINAPI dkcGetCurrentDirectory(char *buff,size_t size){
#	ifdef WIN32
	if(0==GetCurrentDirectory(size,buff)){
		return FALSE;
	}
	/*if(NULL==_getcwd(path,dkcdMAXPATH_BUFFER)){
		return FALSE;
	}*/
#else
  if(NULL==getcwd(buff,size))
		return FALSE;
#endif
	return TRUE;

#if 0
	/* �ǂ����������ɂȂ��Ă��邩������Ȃ��̂łƂ肠����+1 */
	char path[dkcdMAXPATH_BUFFER + 1];
	size_t len;
#	ifdef WIN32
	if(0==GetCurrentDirectory(size,path)){
		return FALSE;
	}
	/*if(NULL==_getcwd(path,dkcdMAXPATH_BUFFER)){
		return FALSE;
	}*/
#else
  if(NULL==getcwd(path,dkcdMAXPATH_BUFFER))
		return FALSE;
#endif
	len = strlen(path);
	return DKUTIL_SUCCEEDED(dkc_strcpy(buff,size,path,len));
#endif //end of if 0
}

static BOOL WINAPI dkcCreateDirectoryLogic(const char *dir,const void *ptr){
#ifdef WIN32
	 //return (0 == _mkdir( dir ));
	 return (0 != CreateDirectory(dir,(SECURITY_ATTRIBUTES *)ptr));
#else
	 return (0 == mkdir( dir ));
#endif
}



int WINAPI dkcCreateDirectory(const char *pPath)
{
	BOOL result;
	char work[dkcdMAXPATH_BUFFER];
	unsigned long n = 0;
	unsigned long len = strlen(pPath);

#ifdef WIN32//sjis support
	SECURITY_ATTRIBUTES attr;
	
	DKUTIL_STRUCTURE_INIT(attr);
	NULL_CHAR_ARRAY(work);

	//error check
	if(dkcdMAXPATH_LEN < len){
		dkcmNOT_ASSERT_MESSAGE("path����������B",pPath);
		return edk_FAILED;
	}
	if(0==len ){
		return edk_ArgumentException;
	}

	//�܂��͈�ڂ����j
	if ( dkcmIsSJIS1(pPath[n]) || ! dkcmIS_INVALID_FOLDERNAME_CHAR(pPath[n]) )
	{//SJIS�̂P�����ڂ�INVALID�Ȓl�ł͖���������B
		work[n] = pPath[n];
		if(1==len){
			//�t�H���_�쐬
			attr.nLength = sizeof(SECURITY_ATTRIBUTES);
			attr.lpSecurityDescriptor = NULL;
			attr.bInheritHandle = FALSE;

			result = dkcCreateDirectoryLogic( work, &attr );
			
			dkcmNOT_ASSERT(FALSE==result && "directory�����Ȃ�����" );
			return edk_SUCCEEDED;
		}
	}
	n++;
	//��ڂ���J�n
	while ( n < len )
	{
		 //�t�H���_���擾
		while ( n < len )
		{
			
			if(! dkcmIsSJIS1(pPath[n - 1]) && ! dkcmIsSJIS2(pPath[n]) )
			{//SJIS�ł͂Ȃ��I�I
				if ( ( dkcmIS_PATH_SEP(pPath[n]) ) /*&& pPath[n] != '\0'*//*(n != '\0')*/ )
				{
					if ( work[n-1] != ':' )
					{//drive��ǂݍ��܂������Ȃ��炵���B
						break;
					}
				}
				else if(dkcmIS_INVALID_FOLDERNAME_CHAR(pPath[n]))
				{//SJIS�����N���A�����̂ɊԈႢ������
					return edk_FAILED;
				}
			}
			
			work[n] = pPath[n];
			n++;
		}
		work[n] = '\0';

		//�t�H���_�쐬
		attr.nLength = sizeof(SECURITY_ATTRIBUTES);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle = FALSE;



		result = dkcCreateDirectoryLogic( work, &attr );
		
		//dkcmNOT_ASSERT("directory�����Ȃ�����" && FALSE==result);
		if(FALSE==result){
			return edk_FAILED;
		}
		work[n++] = dkcdPATH_SEP;
	}
#else //no support sjis
	NULL_CHAR_ARRAY(work);
	
		//error check
	if(dkcdMAXPATH_LEN < len){
		dkcmNOT_ASSERT_MESSAGE("path����������B",pPath);
		return edk_FAILED;
	}
	if(0==len ){
		return edk_ArgumentException;
	}

	while ( n < len )
	{
		 //�t�H���_���擾
		while ( n < len )
		{
			if ( ( dkcmIS_PATH_SEP(pPath[n]) ) && (n != '\0') )
			{
				if ( work[n-1] != ':' )//Linux�̃h���C�u�̃p�X�̓R���f�����̂��ȁH
				{
					break;
				}
			}
			work[n] = pPath[n];
			n++;
		}
		work[n] = '\0';

		result = dkcCreateDirectoryLogic( work,NULL );

		//dkcmNOT_ASSERT("directory�����Ȃ�����" && FALSE==result);
		if(FALSE==result){
			return edk_FAILED;
		}
		work[n++] = dkcdPATH_SEP;
	}

#endif

	return edk_SUCCEEDED;
}

BOOL WINAPI dkcFileCopy(const char *dest,const char *src,BOOL bReWrite){
	return dkcFileCopyEx(dest,src,1024 * 64,bReWrite,FALSE,NULL,NULL);
}

BOOL WINAPI dkcFileCopyEx(const char *dest,const char *src,
																		size_t inner_buffer_size,BOOL bReWrite,
																		BOOL bThreadLock,
																		DKC_STD_CALLBACK pfcallback,void *parg)
{
	void *buff;
	FILE *srcf,*destf;
	size_t filesize;//�t�@�C���T�C�Y
	size_t readed;//�ǂݍ��񂾃f�[�^
	size_t count;//count
	size_t i;
	size_t rest;//�c��
	int result = FALSE;//�߂�l
	DKC_THREAD_LOCK *lock = NULL;
	DKC_FILECOPY_CALLBACK_STRUCT cbdata;


	if(NULL==dest || NULL==src){
		return FALSE;
	}
	if(inner_buffer_size <= 1024){
		inner_buffer_size = 1024;
	}

	//�o�b�t�@�̏���
	buff = malloc(inner_buffer_size);
	if(NULL==buff){
		inner_buffer_size = 1024 * 256;
		buff = malloc(inner_buffer_size);
		if(NULL==buff)
			return FALSE;
	}
	//thread lock
#ifdef _MT
	if(bThreadLock){
		lock = dkcAllocThreadLock();
		if(NULL==lock){
			goto Error;
		}
		dkcThreadLock_Lock(lock);
	}
#endif
	//�������Ⴂ�ꍇ
	filesize = dkcFileSize(src);
	//dest�����݂���
	if(FALSE==bReWrite && TRUE==dkcFileExist(dest)){
		goto Error;
	}
	cbdata.filesize = filesize;
	cbdata.count = 0;
	for(;;)
	{//non loop 
		if(0 == filesize)
		{//�t�@�C���T�C�Y���O�̏ꍇ
			if(TRUE==dkcCreateEmptyFile(dest))
			{
				result = TRUE;
			}
			break;
		}
		if(filesize < inner_buffer_size)
		{//�t�@�C���T�C�Y���o�b�t�@�Ɏ��܂��Ă��܂��ꍇ�B
			if(DKUTIL_FAILED(dkcLoadBinary(buff,filesize,src,&readed)))
			{
				goto Error;
			}
#	ifdef DEBUG
			if(readed != filesize){
				ODS("readed != filesize why?\n");
			}
#	endif
			if(DKUTIL_SUCCEEDED(dkcSaveBinary(buff,filesize,dest)))
			{
				result = TRUE;
			}
			break;
		}
		//�o�b�t�@���g��Ȃ���΃R�s�[�ł��Ȃ��ꍇ�B

		srcf = dkcFOpen(src,"rb");
		if(NULL==srcf) goto Error;
		destf = dkcFOpen(dest,"wb");
		if(NULL==destf) goto Close;

		//���[�v����񐔂��v�Z
		count = filesize / inner_buffer_size;

		for(i=0;i<count;i++){
			dkcmFORCE_NOT_ASSERT(1 != fread(buff,inner_buffer_size,1,srcf));
			dkcmFORCE_NOT_ASSERT(1 != fwrite(buff,inner_buffer_size,1,destf));
			
			cbdata.count += inner_buffer_size;
			if(pfcallback){//�R�[���o�b�N�ő�����ɓ`����B
				if(FALSE==pfcallback(&cbdata,parg)){
					goto Close;
				}
			}
		}

		rest = filesize - (count * inner_buffer_size);
		
		//�c����o��
		dkcmFORCE_NOT_ASSERT(rest != fread(buff,1,rest,srcf));
		dkcmFORCE_NOT_ASSERT(rest != fwrite(buff,1,rest,destf));
		
		
		cbdata.count += rest;
		dkcmNOT_ASSERT(cbdata.count != cbdata.filesize);
		if(pfcallback){//�R�[���o�b�N�ő�����ɓ`����B
			if(FALSE==pfcallback(&cbdata,parg)){
				goto Close;
			}
		}
		result = TRUE;
Close:
		//close
		dkcFClose(&srcf);
		dkcFClose(&destf);

		break;
	}//end of for




Error:
#ifdef _MT
	if(bThreadLock){
		if(lock){
			dkcThreadLock_Unlock(lock);
			dkcFreeThreadLock(&lock);
		}
	}
#endif
	if(buff){
		free(buff);buff=NULL;
	}
	return result;
}

BOOL WINAPI dkcFileRemove(const char *filename)
{
#ifdef WIN32
	return (0 != DeleteFile(filename));
	//return (0==remove(filename));
#else
	return (0==remove(filename));
#endif
}

BOOL WINAPI dkcFileRename(const char *oldname,const char *newname){
#ifdef WIN32
	return (0==rename(oldname,newname));
#else
	return (0==rename(oldname,newname));
#endif
}

BOOL WINAPI dkcCreateZeroByteFile(const char *filename,BOOL rewrite)
{
	FILE *fp;
	int r = FALSE;
	if(FALSE==dkcFileExist(filename) || TRUE==rewrite){
		fp = fopen(filename,"wb");
		if(!fp){
			return r;
		}
		fclose(fp);
		r = TRUE;
	}
	return r;
}


int WINAPI dkcFileBinaryCompare(const char *filename1,const char *filename2)
{
	BOOL r = FALSE;
	
	//�t�@�C���̑��݂��m���߂�
	r = dkcFileExist(filename1);
	if(r==FALSE){
		return edk_ArgumentException;
	}
	r = dkcFileExist(filename2);
	if(r==FALSE){
		return edk_ArgumentException;
	}

	//�t�@�C���̃T�C�Y���m���߂�
	{
		DWORD high = 0,low = 0,high2 = 0,low2 = 0;
		r = dkcFileSize64(filename1,&high,&low);
		if(r==FALSE){
			return edk_ArgumentException;
		}
		r = dkcFileSize64(filename2,&high2,&low2);
		if(r==FALSE){
			return edk_ArgumentException;
		}
		r = (high==high2 && low==low2);
		if(FALSE==r){//�t�@�C���T�C�Y�������
			return edk_FAILED;
		}

	}


	{
		DKC_STREAM *s1 = NULL,*s2=NULL;
		BYTE *buffer1 = NULL,*buffer2 = NULL;
		size_t buffsize = 1024 * 52;


		//initialize

		//�t�@�C�����I�[�v��
		r = edk_LogicError;

		s1 = dkcAllocStreamFileType( 
			edkcStreamDefaultEndian | edkcStreamProcessAsOrdered,
			filename1,"rb");
		if(NULL==s1){
			return edk_LogicError;
		}

		s2 = dkcAllocStreamFileType( 
			edkcStreamDefaultEndian | edkcStreamProcessAsOrdered,
			filename2,"rb");

		if(NULL==s2){
			goto Error;
		}

		//�o�b�t�@��p��
		r = edk_OutOfMemory;

		buffer1 = malloc(buffsize);
		if(NULL==buffer1){
			
			goto Error;
		}
		buffer2 = malloc(buffsize);
		if(NULL==buffer2){
			goto Error;
		}

		//process

		for(;;){
			size_t readsize1,readsize2;
			BOOL re1,re2;

			dkcStreamRead(s1,buffer1,buffsize,&readsize1);
			dkcStreamRead(s2,buffer2,buffsize,&readsize2);
			
			re1 = dkcStreamError(s1);
			re2 = dkcStreamError(s2);
			if(re1 || re2){
				r = edk_LogicError;
				goto Error;
			}


			dkcmNOT_ASSERT(readsize1 != readsize2);
			

			r = dkc_memcmp(buffer1,buffsize,buffer2,buffsize);
			if(DKUTIL_FAILED(r)){
				r = edk_FAILED;
				break;
			}
			re1 = dkcStreamEOF(s1);
			re2 = dkcStreamEOF(s2);
			if(re1 && re2){
				r = edk_SUCCEEDED;
				break;
			}
			else if(FALSE==re1 && FALSE==re2)
			{
				continue;
			}
			else
			{
				r = edk_LogicError;
				goto Error;
			}
		}
Error:
		if(buffer2){
			free(buffer2);
		}
		if(buffer1){
			free(buffer1);
		}
		dkcFreeStream(&s2);
		dkcFreeStream(&s1);
	}

	return r;
}

int WINAPI dkcMemoryToFile(const char *filename,const void *buff,size_t size,UINT flag)
{
	BOOL r;
	DKC_STREAM *p;
	int re = edk_FAILED;
	if(!(edkcFileRewrite & flag))
	{

		//�t�@�C���̑��݂��m���߂�
		r = dkcFileExist(filename);
		if(r==TRUE){
			return edk_ArgumentException;
		}
	}
	p = dkcAllocStreamFileType( 
			edkcStreamDefaultEndian | edkcStreamProcessAsOrdered,
			filename,"wb");

	if(NULL==p){
		return edk_FAILED;
	}
	if(DKUTIL_FAILED(dkcStreamWrite(p,buff,size))){
		goto End;
	}
	re = edk_SUCCEEDED;
End:
	dkcFreeStream(&p);

	return re;

}


int WINAPI dkcFileToMemory(const char *filename,void *buff,size_t size)
{
	DWORD h = 0,l = 0;
	DKC_STREAM *p;
	int r = edk_FAILED;
	if(FALSE==dkcFileSize64(filename,&h,&l)){
		return edk_FileNotFound;
	}

	if(h != 0 || ( (size_t)(l) > size ))
	{//DWORD_MAX�ȏ�̃T�C�Y�̃t�@�C�����������ɂ͓ǂ݂��߂Ȃ��ł���^^;
		return edk_BufferOverFlow;
	}

	p = dkcAllocStreamFileType( 
			edkcStreamDefaultEndian | edkcStreamProcessAsOrdered,
			filename,"rb");

	if(NULL==p){
		return edk_FAILED;
	}
	if(DKUTIL_FAILED(dkcStreamRead(p,buff,l,(size_t *)&h))){
		goto End;
	}
	if(h != l){
		goto End;
	}
	r = edk_SUCCEEDED;
End:
	dkcFreeStream(&p);
	return r;
}

/*
int WINAPI dkcMemoryToTextFile(const char *filename,const void *buff,size_t size)
{
	
	

}

size_t WINAPI dkcTextFileToMemorySize(const char *filename)
{


}

int WINAPI dkcTextFileToMemory(const char *filename,void *buff,size_t size)
{


}
*/

//**********************************************************


#if 0

DKC_FILE_FINDER * WINAPI dkcAllocFileFinder(
			const char *target,const char *dir,BOOL bSubDir
			){

	DKC_FILE_FINDER *p;
	p = dkcAllocate(sizeof(DKC_FILE_FINDER));
	if(NULL==p) return NULL;

	//�f�B���N�g��
	p->mDir = dkcAllocPathString(dir);//allocate and copy 
	if(NULL==p->mDir){
		goto Error;
	}
	//�ċA�p�X�^�b�N
	p->mStack = dkcAllocStack(10,sizeof(DKC_PATHSTRING *));
	if(NULL==p->mStack){
		goto Error;
	}
	//target������
	p->mTarget = dkcAllocString(128);
	if(NULL==p->mTarget){
		goto Error;
	}
	//target��������R�s�[
	dkcStringCopy(p->mTarget,target,strlen(target));

	p->mState = edkcFileFinderEmpty;//�������Ă��Ȃ��B
	p->mbSubDir = bSubDir;//�T�udirectory���������邩�ǂ���

	return p;
Error:
	if(p){
		dkcFreeString(&(p->mTarget));
		dkcFreeStack(&(p->mStack));
		dkcFreePathString(&(p->mDir));
	}
	dkcFree((void **)&p);
	return NULL;
}


int WINAPI dkcFreeFileFinder(DKC_FILE_FINDER **p){
	if(NULL==p || NULL==(*p)){
		return edk_ArgumentException;
	}
	dkcFreeString(&(*p)->mTarget);
	dkcFreeStack(&(*p)->mStack);
	dkcFreePathString(&(*p)->mDir);
	return dkcFree((void **)p);
}

static BOOL isDot(DKC_FILE_FINDER *ptr){

#ifdef WIN32
	return (
		strcmp(ptr->mFindData.cFileName,"..") == 0 || 
		strcmp(ptr->mFindData.cFileName,".") == 0
	);
#else
	

#endif

}
static BOOL isFolder(DKC_FILE_FINDER *ptr){
#ifdef WIN32
	return (
			ptr->mFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
			&& strcmp(ptr->mFindData.cFileName,"..")!=0 
			&& strcmp(ptr->mFindData.cFileName,".")!=0
			);
#else


#endif
}
static int FFPushStack(DKC_FILE_FINDER *p){
	DKC_PATHSTRING *tp;
	int r;
	char buff[dkcdMAXPATH_BUFFER];

#ifdef WIN32
	
	dkcCurrentDirectoryConcatenate(buff,sizeof(buff),p->mFindData.cFileName);
#else
	dkcCurrentDirectoryConcatenate(buff,sizeof(buff),ptr->mDirent.d_name);
#endif

	tp = dkcAllocPathString(buff);
	if(NULL==tp){
		return edk_FAILED;
	}
	r = dkcStackDynamicPush(p->mStack,tp);
	return r;
}

static BOOL FFIsStackEmpty(DKC_FILE_FINDER *p){
	return dkcStackIsEmpty(p->mStack);
}


static int FFPopStack(DKC_FILE_FINDER *p){
	int r;
	DKC_PATHSTRING *tp;
	r = dkcStackTop(p->mStack,&tp);
	if(DKUTIL_FAILED(r)){return r;}

	dkcFreePathString( &(p->mDir));
	dkcStackPop(p->mStack);
	
	p->mDir = tp;
	return r;
}

static void FFReSearch(DKC_FILE_FINDER *p){
	dkcFindClose(p);
	FFPopStack(p);
	p->mState = edkcFileFinderEmpty;
	DKUTIL_STRUCTURE_INIT(p->mFindData);
}
/*
static int FFEndSearch(DKC_FILE_FINDER *p){

}
*/

///http://www.ne.jp/asahi/oh/landd/prog_html/prog23.html
static int ReflexiveSearch(DKC_FILE_FINDER *p,DKC_PATHSTRING *path,BOOL *bCopySucceeded)
{

}
/*static int FFCheckingLogic(DKC_FILE_FINDER *p,DKC_PATHSTRING *path,BOOL *bCopySucceeded)
{*/
#define FFCHECKING(p,path,bCopySucceeded)\
	if(TRUE==isDot(p))\
	{/*����Ȃ̂��ᕶ����͂��������Ȃ��Ȃ��B*/\
		p->mState = edkcFileFinderSearching;\
		return WithFolderSearch(p,path,bCopySucceeded);\
	}\
	if(TRUE==isFolder(p))\
	{/*����H�t�H���_�[���B*/\
		dkcFileFinderReferenceFileName(p,path);\
		p->mState = edkcFileFinderSearching;\
		FFPushStack(p);/*folder���X�^�b�N�ɋl�߂�B*/\
		return WithFolderSearch(p,path,bCopySucceeded);\
	}
/*}*/

static int WithFolderSearch(DKC_FILE_FINDER *p,DKC_PATHSTRING *path,BOOL *bCopySucceeded)
{
	int r;
//	DKC_PATHSTRING *pps = NULL;
//	DKC_FILE_FINDER *

	r = 0;
	*bCopySucceeded = FALSE;

	if(edkcFileFinderEmpty == p->mState)
	{//�Ăяo���čŏ���������A
		r = dkcFindFirstFile(p);
		if(DKUTIL_FAILED(r)) return edk_FAILED;
#	ifdef WIN32 //windows�̏ꍇ�͓����Ɋi�[���Ă��饥��B
		if(TRUE==isDot(p))
		{//����Ȃ̂��ᕶ����͂��������Ȃ��Ȃ��B
			p->mState = edkcFileFinderSearching;
			return WithFolderSearch(p,path,bCopySucceeded);
		}
		if(TRUE==isFolder(p))
		{//����H�t�H���_�[���B
			dkcFileFinderReferenceFileName(p,path);
			p->mState = edkcFileFinderSearching;
			FFPushStack(p);//folder���X�^�b�N�ɋl�߂�B
			return WithFolderSearch(p,path,bCopySucceeded);
		}
		//FFCHECKING(p,path,bCopySucceeded)

		r = dkcFileFinderReferenceFileName(p,path);
			
		if(DKUTIL_SUCCEEDED(r)){//�R�s�[������
			*bCopySucceeded = TRUE;
		}
#	endif
		p->mState = edkcFileFinderSearching;
		return r;
	}else if(edkcFileFinderSearching == p->mState)
	{//��������������B
		r = dkcFindNextFile(p);
		if(edk_SUCCEEDED == r)
		{//�������B
			if(TRUE==isDot(p))
			{//����Ȃ̂��ᕶ����͂��������Ȃ��Ȃ��B
				p->mState = edkcFileFinderSearching;
				return WithFolderSearch(p,path,bCopySucceeded);
			}
			if(TRUE==isFolder(p))
			{//����H�t�H���_�[���B
				dkcFileFinderReferenceFileName(p,path);
				p->mState = edkcFileFinderSearching;
				FFPushStack(p);//folder���X�^�b�N�ɋl�߂�B
				return WithFolderSearch(p,path,bCopySucceeded);
			}

			r = dkcFileFinderReferenceFileName(p,path);
			
			if(DKUTIL_SUCCEEDED(r)){//�R�s�[������
				*bCopySucceeded = TRUE;
			}/*else{//�R�s�[���s�I�H�H
				*bCopySucceeded = FALSE;
			}*/
		}else if(edk_EndProcess == r)
		{//�������I������B
			if(FALSE==FFIsStackEmpty(p))
			{//��ł͂Ȃ��B
				FFReSearch(p);
				return WithFolderSearch(p,path,bCopySucceeded);
			}else{
				dkcFindClose(p);//�I���i���B�i������p->mState = edkcFileFinderFinish;������Ă���B
			}
		}else{
			dkcmNOT_ASSERT("����Ȃ΂��ȁ`");
		}
		return r;
	}else if(edkcFileFinderFinish == p->mState)
	{
		return edk_EndProcess;
	}
	dkcmNOT_ASSERT("dkcFileFinderNext�̃v���O���������������B�`�[�g����Ă��邩���I�H");
	return edk_FAILED;

}


static int NormalSearch(DKC_FILE_FINDER *p,DKC_PATHSTRING *path,BOOL *bCopySucceeded)
{
	int r;
	r = 0;
	*bCopySucceeded = FALSE;

	if(edkcFileFinderEmpty == p->mState)
	{//�Ăяo���čŏ���������A
		r = dkcFindFirstFile(p);
		if(DKUTIL_FAILED(r)) return edk_FAILED;
#	ifdef WIN32 //windows�̏ꍇ�͓����Ɋi�[���Ă��饥��B
		if(TRUE==isDot(p) || TRUE==isFolder(p))
		{//����Ȃ̂��ᕶ����͂��������Ȃ��Ȃ��B
			p->mState = edkcFileFinderSearching;
			return NormalSearch(p,path,bCopySucceeded);
		}
		r = dkcFileFinderReferenceFileName(p,path);
			
		if(DKUTIL_SUCCEEDED(r)){//�R�s�[������
			*bCopySucceeded = TRUE;
		}
#	endif
		p->mState = edkcFileFinderSearching;
		return r;
	}else if(edkcFileFinderSearching == p->mState)
	{//��������������B
		r = dkcFindNextFile(p);
		if(edk_SUCCEEDED == r)
		{//�������B
			if(TRUE==isDot(p) || TRUE==isFolder(p))
			{//����Ȃ̂��ᕶ����͂��������Ȃ��Ȃ��B



				//p->mState = edkcFileFinderSearching;
				return NormalSearch(p,path,bCopySucceeded);
			}
			r = dkcFileFinderReferenceFileName(p,path);
			
			if(DKUTIL_SUCCEEDED(r)){//�R�s�[������
				*bCopySucceeded = TRUE;
			}/*else{//�R�s�[���s�I�H�H
				*bCopySucceeded = FALSE;
			}*/
		}else if(edk_EndProcess == r)
		{//�������I������B
			dkcFindClose(p);//�I���i���B�i������p->mState = edkcFileFinderFinish;������Ă���B
		}else{
			dkcmNOT_ASSERT("����Ȃ΂��ȁ`");
		}
		return r;
	}else if(edkcFileFinderFinish == p->mState)
	{
		return edk_EndProcess;
	}/*else{
		return edk_FAILED;
	}*/
	dkcmNOT_ASSERT("dkcFileFinderNext�̃v���O���������������B�`�[�g����Ă��邩���I�H");
	return edk_FAILED;

}

/*!
@param ptr[in] dkcAllocFileFinder()�Ŏ擾�����|�C���^
@return edk_SUCCEEDED�Ȃ�dkcFileFinderGetResult()���Ăяo���ėǂ��B
����ȊO�Ȃ�dkcFreeFileFinder()�ŏI�������Ȃ���΂Ȃ�Ȃ��B
*/
int WINAPI dkcFileFinderNext(DKC_FILE_FINDER *p,DKC_PATHSTRING *path,BOOL *bCopySucceeded)
{
	if(FALSE==p->mbSubDir){
		return NormalSearch(p,path,bCopySucceeded);
	}else{//sub directory������
		return WithFolderSearch(p,path,bCopySucceeded);
	}
	//return ReflexiveSearch(p,path,bCopySucceeded);
}
#endif

DKC_FINDFILE *WINAPI dkcAllocFindFile()
{
	DKC_FINDFILE *p;
	p = (DKC_FINDFILE *)dkcAllocate(sizeof(DKC_FINDFILE));
	return p;
}
int WINAPI dkcFreeFindFile(DKC_FINDFILE **ptr){
	if(NULL==ptr /*|| *ptr==NULL*/){
		return edk_FAILED;
	}
	return dkcFree((void **)ptr);
}


int WINAPI dkcFindFirstFile(DKC_FINDFILE *ptr,const char *target){
#ifdef WIN32
	ptr->mHandle = 
		FindFirstFileA( target, &(ptr->mFindData) );
	if(ptr->mHandle == INVALID_HANDLE_VALUE){
		return edk_FAILED;
	}
#else
	ptr->mHandle = opendir( target );
	if(NULL==ptr->mHandle){
		return edk_FAILED;
	}

#endif
	return edk_SUCCEEDED;
}

int WINAPI dkcFindNextFile(DKC_FINDFILE *ptr){
#	ifdef WIN32
	if ( 0 == FindNextFileA( ptr->mHandle, &(ptr->mFindData) ))
	{
		if ( GetLastError() == ERROR_NO_MORE_FILES )
		{//�Ȃ񂾁A�������������B
			return edk_EndProcess;
		}
		else//�ň��̃p�^�[��
		{
			return edk_FAILED;
		}
	}
#	else
  errno = 0;
	ptr->mDirent = readdir( ptr->mHandle );
  if ( ptr->mDirent == 0 )
  {
    if ( errno == 0 )
    {//����������O�O�G
			return edk_EndProcess;
    }
		else//�ň��̃p�^�[��
		{
			return edk_FAILED;
		}
  }
#	endif
	return edk_SUCCEEDED;
}

int WINAPI dkcFindClose(DKC_FINDFILE *ptr)
{
#ifdef WIN32
	if(INVALID_HANDLE_VALUE == ptr->mHandle){
		return edk_FAILED;
	}
	FindClose(ptr->mHandle);
	ptr->mHandle = INVALID_HANDLE_VALUE;//�������胊�Z�b�g
#else
	if(0 == ptr->mHandle){
		return edk_FAILED;
	}
	closedir(ptr->mHandle);
	ptr->mHandle = 0;//�������胊�Z�b�g
	ptr->mDirent = NULL;
#endif

	return edk_SUCCEEDED;

}

int WINAPI dkcFindFileGetFileName(DKC_FINDFILE *ptr,char *buff,size_t buffsize)
{
	int r;
	size_t len;
#ifdef WIN32
	len = strlen(ptr->mFindData.cFileName);
	if(0 == len) return edk_FAILED;
	r = dkc_strcpy(buff,buffsize,ptr->mFindData.cFileName,len);
#else
	if(NULL==ptr->mDirent)
	{//�Ăяo�����Ԉ���Ă����O�O�G
		return edk_LogicError;
	}
	len = strlen(ptr->mDirent.d_name);
	if(0 == len) return edk_FAILED;
	r = dkc_strcpy(buff,buffsize,ptr->mDirent.d_name,len);
#endif
	return r;
}

BOOL WINAPI dkcFindFileIsFolder(DKC_FINDFILE *ptr){


#ifdef WIN32
	return (
			ptr->mFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
			&& strcmp(ptr->mFindData.cFileName,"..")!=0 
			&& strcmp(ptr->mFindData.cFileName,".")!=0
			);
#else


#endif
}


BOOL WINAPI dkcFindFileIsDot(DKC_FINDFILE *ptr){
#ifdef WIN32
	return (
		strcmp(ptr->mFindData.cFileName,"..") == 0 || 
		strcmp(ptr->mFindData.cFileName,".") == 0
	);
#else
	

#endif
}

BOOL WINAPI dkcFindFileIsNormalFile(DKC_FINDFILE *ptr)
{
#ifdef WIN32
	return (ptr->mFindData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL);
#else

#endif
}

BOOL WINAPI dkcFindFileIsReadOnly(DKC_FINDFILE *ptr){
#ifdef WIN32
	return (ptr->mFindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY);
#else

#endif
}
void WINAPI dkcFindFileSize(DKC_FINDFILE *ptr,ULONG *High,ULONG *Low){
#ifdef WIN32
	*High = ptr->mFindData.nFileSizeHigh;
	*Low = ptr->mFindData.nFileSizeLow;
#else

#endif

}
/*
int WINAPI dkcFindFirstFile(DKC_FILE_FINDER *ptr){

#ifdef WIN32
	ptr->mHandle = 
		FindFirstFileA( dkcPathStringPointer(ptr->mDir), &(ptr->mFindData) );
	if(ptr->mHandle == INVALID_HANDLE_VALUE){
		return edk_FAILED;
	}
#else
	ptr->mHandle = opendir( dkcPathStringPointer(ptr->mDir) );
	if(NULL==ptr->mHandle){
		return edk_FAILED;
	}

#endif
	return edk_SUCCEEDED;
}

int WINAPI dkcFindNextFile(DKC_FILE_FINDER *ptr)
{
	if(edkcFileFinderFinish == ptr->mState){
		return edk_LogicError;//�Ăяo�����Ԉ���Ă���i���I
	}
#	ifdef WIN32
	if ( 0 == FindNextFileA( ptr->mHandle, &(ptr->mFindData) ))
	{
		if ( GetLastError() == ERROR_NO_MORE_FILES )
		{//�Ȃ񂾁A�������������B
			return edk_EndProcess;
		}
		else//�ň��̃p�^�[��
		{
			return edk_FAILED;
		}
	}
#	else
  errno = 0;
	ptr->mDirent = readdir( ptr->mHandle );
  if ( ptr->mDirent == 0 )
  {
    if ( errno == 0 )
    {//����������O�O�G
			return edk_EndProcess;
    }
		else//�ň��̃p�^�[��
		{
			return edk_FAILED;
		}
  }
#	endif
	return edk_SUCCEEDED;
}

int WINAPI dkcFindClose(DKC_FILE_FINDER *ptr){
#ifdef WIN32
	if(INVALID_HANDLE_VALUE == ptr->mHandle){
		return edk_FAILED;
	}
	FindClose(ptr->mHandle);
	ptr->mHandle = INVALID_HANDLE_VALUE;//�������胊�Z�b�g
#else
	if(0 == ptr->mHandle){
		return edk_FAILED;
	}
	closedir(ptr->mHandle);
	ptr->mHandle = 0;//�������胊�Z�b�g
	ptr->mDirent = NULL;
#endif
	//�I���I�I
	ptr->mState = edkcFileFinderFinish;

	return edk_SUCCEEDED;
}


int WINAPI dkcFileFinderReferenceFileName(DKC_FILE_FINDER *ptr,DKC_PATHSTRING *path)
{
	int r;
	size_t len;
#ifdef WIN32
	len = strlen(ptr->mFindData.cFileName);
	if(0 == len) return edk_FAILED;
	r = dkcPathStringCopy(path,ptr->mFindData.cFileName,len);
#else
	if(NULL==ptr->mDirent)
	{//�Ăяo�����Ԉ���Ă����O�O�G
		return edk_LogicError;
	}
	len = strlen(ptr->mDirent.d_name);
	if(0 == len) return edk_FAILED;
	r = dkcPathStringCopy(path,ptr->mDirent.d_name,len);
#endif
	return r;
}
*/