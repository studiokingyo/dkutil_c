
/*!
@file dkcSJISFileSystem.c
@author d金魚
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
 文字列 str の str[nPos] について、<br>
 @return
   ０ …… １バイト文字<br>
   １ …… ２バイト文字の一部（第１バイト）<br>
   ２ …… ２バイト文字の一部（第２バイト）<br>
<br>

シフトＪＩＳ文字列の判定をします。
*/ 
DKC_EXTERN int WINAPI dkcIsShiftJIS( const char *str, int nPos )
{
	int i;
	int state; // { 0, 1, 2 } = { １バイト文字, ２バイト文字の第１バイト, ２バイト文字の第２バイト }

	state = 0;
	for( i = 0; str[i] != '\0'; i++ )
	{
		if      ( ( state == 0 ) && ( jms1( str[i] ) ) ) state = 1; // 0 -> 1
		else if ( ( state == 1 ) && ( jms2( str[i] ) ) ) state = 2; // 1 -> 2
		else if ( ( state == 2 ) && ( jms1( str[i] ) ) ) state = 1; // 2 -> 1
		else                                             state = 0; // 2 -> 0, その他

		// str[nPos] での状態を返す。
		if ( i == nPos ) return state;
	}
	return 0;
}
static DKC_INLINE int isJMS(const char *str, int nPos ){
	return dkcIsShiftJIS(str,nPos);
}
/// 文字列末尾の文字 '\0' の位置を返す
static DKC_INLINE char *strtail( const char *stringg )
{
	return strchr( stringg, '\0' );
}//strtail

/// パス名から、拡張子の開始位置を取得する
/// ファイル名の末尾からファイル名の最後の '.' までを、拡張子と判断する
///@return NULLだと拡張子は判別不能。また、与えられた文字列の最初のアドレスが返された場合も判別不能
///@note SJIS対策済み？
const char * WINAPI dkcGetFileExtension( const char *PathName )
{

	char *p;
	char *get_tail;

	get_tail = strtail( PathName );
	for( p = get_tail; p >= PathName; p-- ) // 文字列の最後から最初まで
	{
		if ( ('\\'==*p) && !isJMS(PathName,p-PathName) )
			return get_tail; // ファイル名はここまで

		if ( '.' == *p )
			return p+1; // '.' を発見
	}
	return get_tail; // 拡張子なし
}//GetFileExtension


BOOL WINAPI dkcIsEffectivePath(const char *path,size_t size){
	char dest[dkcdMAXPATH_BUFFER];
	const size_t dsize = dkcdMAXPATH_BUFFER;

	//相対パス？絶対パス？でもそのファイルが存在するか？
	if(FALSE==dkcFileExist(path)){
		return FALSE;
	}
	//pathを正規化
	if(DKUTIL_FAILED(dkcToAbsolutelyPath(dest,dsize,path,size))){
		return FALSE;
	}
	//正規化して、ファイルに使用して良い文字列を使っているか？
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

///文字列の後ろがPathSeparatorだったらTRUE
BOOL WINAPI dkcIsTailPathSep(const char *src,size_t dlen){
	int point;
	point = dkcSJIS_SearchPathSepLast(src);
	//if(point != -1 && (size_t)point == dlen - 1/*&& (size_t)point != dlen*/)
	if((size_t)point == dlen - 1)
	{	//みつかった。
		return TRUE;
	}
	//みつからない。
	return FALSE;
}
///@return path separatorを挿入する必要があり、挿入出来たらTRUE
int WINAPI dkcPushBackPathSep(char *dest,size_t dlen,size_t size){
	if(FALSE==dkcIsTailPathSep(dest,dlen)){
		if(size < dlen + 2){
			return edk_OutputBufferWasLost;//バッファが足らんよ！
		}
		dest[dlen ] = dkcdPATH_SEP;
		dest[dlen + 1] = '\0';//こいつを忘れてはいけない。
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
	{//path separatorが見つかり、それがdestの最期に無い時
		if(!dkcmIS_PATH_SEP(src[0]) )
		{//path separatorで無い時
			//path 区切り文字を付加してやる。
			dest[dlen ] = dkcdPATH_SEP;
			dest[dlen + 1] = '\0';//こいつを忘れてはいけない。
		}
	}*/
	dkcPushBackPathSep(dest,dlen,dsize);
	return dkc_strcat_amap(dest,dsize,dlen,src,strlen(src));
}

int WINAPI dkcCurrentDirectoryConcatenate(char *dest,size_t dsize,const char *src)
{
	//size_t len;
	//int point;
	
	dkcmNOT_ASSERT(dsize <= dkcdMAXPATH_LEN);//<=はNULL文字も含むため。
	
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
	/*if(dsize < dkcdMAXPATH_BUFFER){//こんなバッファじゃどうなるか分からないよ＾＾；
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
//パスを正規化する関数
int WINAPI dkcToAbsolutelyPath(char *dest,size_t dsize,const char *src,size_t ssize)
{
	char *tp = NULL;
	int r;
	MAX_PATH_CHECK(dsize);

	
	if(NULL==dest) return edk_FAILED;

	if(dkcIsRelativityPath(src)==TRUE)
	{//カレントディレクトリを付加する。

		tp = (char *)malloc(dkcdMAXPATH_BUFFER);
		if(NULL==tp) return edk_OutOfMemory;

		dkcCurrentDirectoryConcatenate(tp,dkcdMAXPATH_BUFFER,src);
		r =  ToAbsolutelyLogic(dest,dsize,tp);
		free(tp);
		return r;

	}
	return ToAbsolutelyLogic(dest,dsize,src);


	
	/*
	状態繊維
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
	//list確保
	plo = dkcAllocSingleListObject(NULL,20);
	if(NULL==plo) return edk_FAILED;

	//directoryをパース
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
			==plo->push_back(plo,&src[i],point - 1)//見つかったところまでだから-1
			)
		{
			return edk_FAILED;
		}

		i += point;
	}

	for(i=0;i<ssize;){


		if(dkcmIsSJIS1(src[i]) && dkcmIsSJIS2(src[i + 1]))
		{//SJIS抜かし
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

	p->mString = dkcAllocString(dkcdMAXPATH_BUFFER + 1);//このサイズじゃないとダメ
	if(NULL==p->mString) goto Error;

	//パス名をコピー
	if(path){
		len = strlen(path);
		//ネイティブなパスを入れる。
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

//パスのデータを得る。
size_t WINAPI dkcPathStringSize(const DKC_PATHSTRING *p)
{
	return dkcStringSize(p->mString);
}

const char *WINAPI dkcPathStringPointer(const DKC_PATHSTRING *p)
{
	return dkcStringPointer(p->mString);
}


//パスを区切る。

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

	for(i=0;i<point;i++){//drive指定のものがあるかどうか。
		if(':'==p[i]){
			if(DKUTIL_FAILED(dkc_strcpy(
				buff,size,p,(size_t)i//-1
				)))
			{
				return edk_BufferOverFlow;
			}
			point = dkcSJIS_StrChrSearch(&p[i],'\\');//次の物を見つける。
			//（まぁ:の次は\\と言う事が決まっているけど、いちおう。
			*count = (size_t)i + point + 1;//次に始めるのは
			return edk_SUCCEEDED;
		}
	}
	if(DKUTIL_FAILED(dkc_strcpy(
		buff,size,p,(size_t)point-1
		)))
	{
		return edk_FAILED;
	}
	*count = (size_t)point + 1;//次に始めるのは
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
	{//もうこの関数を呼び出すのは終わりだっちゅうに！！
		return edk_EndProcess;
	}
	point = dkcSJIS_StrChrSearch(&p[*count],'\\');
	if(-1==point)
	{//最後まで来ちまった。

	
		len -= *count;
		if(DKUTIL_FAILED(dkc_strcpy(//残り物を全部コピー
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
	*count += (size_t)point + 1;//次に始めるのは
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
	if(FALSE==dkcIsTailPathSep(src,len))//後ろにPathSeparatorが無い！！
	{
		p = (char *)malloc(size);
	
		if(!p) return edk_OutOfMemory;
		strcpy(p,src);//あまり使いたくないけどすでにバッファは保証されているのでstrcpy
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
		//endlen;//マイナスしておく \\をReplaceしないため
	}
	return dkcStringErase(ptr->mString,count - 1,(size_t)endlen + 1);//count -1は前の\\を消すためendlen + 1は-1した為
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
	endlen = dkcSJIS_SearchPathSep(&p[count]);//countから検索している
	if(-1==endlen){
		endlen = dkcPathStringSize(ptr);
		endlen = endlen - count;
	}else{
		if(0 != endlen)
			endlen--;//マイナスしておく \\をReplaceしないため
	}
	return dkcStringReplace(ptr->mString,count,count + endlen,src,len);
	/*
	size_t size = len + 5;
	int r;
//	int point;
	const char *pointer;
	char *p ;
	if(FALSE==dkcIsTailPathSep(src,len))//後ろにPathSeparatorが無い！！
	{
		//文字列をちと変更
		p = malloc(size);
		if(!p) return edk_OutOfMemory;
		strcpy(p,src);
		r = dkcPushBackPathSep(p,len,size);
	}else{
		p = (char *)src;
	}
	pointer = dkcPathStringPointer(ptr);
	//countから調べる。
	dkcSJIS_SearchPathSep(&pointer[count]);
	*/
	


	

}





//パスを正規化して入れる。
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

#	ifdef DEBUG //ありえないよ～エラーチェック
	dkcmNOT_ASSERT(DKUTIL_FAILED(result));	
	dkcmNOT_ASSERT(len >= bsize);// >=にするのはNULL文字が潰されているかどうか。
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
//パスを正規化して入れる。
int WINAPI dkcPathStringCopy(DKC_PATHSTRING *ptr,const char *buff,size_t size)
{
	
	dkcmNOT_ASSERT(ptr->mString->mByteSize + size >= dkcdMAXPATH);
	if(ptr->mString->mByteSize + size >= dkcdMAXPATH)
	{//これ以上は入れられない。
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

//パスを正規化して繋げる。
int WINAPI dkcPathStringConcatenate(DKC_PATHSTRING *ptr,const char *buff,size_t size)
{
	int result;

	dkcmNOT_ASSERT(ptr->mString->mByteSize + size >= dkcdMAXPATH);
	if(ptr->mString->mByteSize + size >= dkcdMAXPATH)
	{//これ以上は入れられない。
		return edk_FAILED;
	}

	if(ptr->mString->mByteSize)
	{//すでに入っているものがある。
		result = dkcPathStringNormalizeConcatenateLogic(ptr,buff,size);
	}
	else
	{//何も無い時はこちらで
		result =  dkcPathStringCopy(ptr,buff,size);
	}
	//return dkcPathStringNormalizeInsertLogic(ptr,buff,size,dkcStringConcatenate);	
	return result;
}



int WINAPI dkcPathStringGetDrive(DKC_PATHSTRING *ptr,char *buff,size_t size){
	const char *p = dkcStringPointer(ptr->mString);
	int point = dkcSJIS_StrChrSearch(p,':');
	if(-1 == point) return edk_Not_Found;//見つからない。
	//return dkc_memcpy(buff,size,p,(size_t)point - 1);
	return dkc_strcpy(buff,size,p,(size_t)1);//driveなら1文字で十分だよね＾＾；
}

int WINAPI dkcPathStringGetFileExtension(DKC_PATHSTRING *ptr,char *buff,size_t size)
{
	int point2;
	size_t len;
	const char *p = dkcStringPointer(ptr->mString);
	int point = dkcSJIS_StrChrSearchLast(p,'.');

	if(point < 0) return edk_Not_Found;//見つからない。
	
	point2 = dkcSJIS_SearchPathSep(&p[point]);
	if(point < point2){//C:\ok.OK\folderのような状況になっている。
		return edk_Not_Found;
	}
	len = dkcStringSize(ptr->mString);
	//if(point - 1 <= 0) return edk_FAILED;

	if((size_t)(point + 1) > len) return edk_FAILED;//Logicalなエラー？？
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
	if(point < 0) return edk_Not_Found;//見つからない。
	if((size_t)(point + 1) > len) return edk_FAILED;
	if((size_t)point == len) return edk_FAILED;//ファイル名が無い。

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
	//if((size_t)point == len) return edk_FAILED;//ファイル名が無い。
	return dkc_strcpy(buff,size,p,point);//\\も含める。

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
	// ファイルサイズ取得
	if((hFind = FindFirstFile(str,&findData)) == INVALID_HANDLE_VALUE){
		return FALSE;
	}

	//ファイルのサイズをヴちこむ
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
	/* どういう実装になっているか分からないのでとりあえず+1 */
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
		dkcmNOT_ASSERT_MESSAGE("pathが長すぎる。",pPath);
		return edk_FAILED;
	}
	if(0==len ){
		return edk_ArgumentException;
	}

	//まずは一つ目を撃破
	if ( dkcmIsSJIS1(pPath[n]) || ! dkcmIS_INVALID_FOLDERNAME_CHAR(pPath[n]) )
	{//SJISの１文字目かINVALIDな値では無かったら。
		work[n] = pPath[n];
		if(1==len){
			//フォルダ作成
			attr.nLength = sizeof(SECURITY_ATTRIBUTES);
			attr.lpSecurityDescriptor = NULL;
			attr.bInheritHandle = FALSE;

			result = dkcCreateDirectoryLogic( work, &attr );
			
			dkcmNOT_ASSERT(FALSE==result && "directoryを作れなかった" );
			return edk_SUCCEEDED;
		}
	}
	n++;
	//二つ目から開始
	while ( n < len )
	{
		 //フォルダ名取得
		while ( n < len )
		{
			
			if(! dkcmIsSJIS1(pPath[n - 1]) && ! dkcmIsSJIS2(pPath[n]) )
			{//SJISではない！！
				if ( ( dkcmIS_PATH_SEP(pPath[n]) ) /*&& pPath[n] != '\0'*//*(n != '\0')*/ )
				{
					if ( work[n-1] != ':' )
					{//driveを読み込ませたくないらしい。
						break;
					}
				}
				else if(dkcmIS_INVALID_FOLDERNAME_CHAR(pPath[n]))
				{//SJIS問題をクリアしたのに間違いだった
					return edk_FAILED;
				}
			}
			
			work[n] = pPath[n];
			n++;
		}
		work[n] = '\0';

		//フォルダ作成
		attr.nLength = sizeof(SECURITY_ATTRIBUTES);
		attr.lpSecurityDescriptor = NULL;
		attr.bInheritHandle = FALSE;



		result = dkcCreateDirectoryLogic( work, &attr );
		
		//dkcmNOT_ASSERT("directoryを作れなかった" && FALSE==result);
		if(FALSE==result){
			return edk_FAILED;
		}
		work[n++] = dkcdPATH_SEP;
	}
#else //no support sjis
	NULL_CHAR_ARRAY(work);
	
		//error check
	if(dkcdMAXPATH_LEN < len){
		dkcmNOT_ASSERT_MESSAGE("pathが長すぎる。",pPath);
		return edk_FAILED;
	}
	if(0==len ){
		return edk_ArgumentException;
	}

	while ( n < len )
	{
		 //フォルダ名取得
		while ( n < len )
		{
			if ( ( dkcmIS_PATH_SEP(pPath[n]) ) && (n != '\0') )
			{
				if ( work[n-1] != ':' )//Linuxのドライブのパスはコレデいいのかな？
				{
					break;
				}
			}
			work[n] = pPath[n];
			n++;
		}
		work[n] = '\0';

		result = dkcCreateDirectoryLogic( work,NULL );

		//dkcmNOT_ASSERT("directoryを作れなかった" && FALSE==result);
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
	size_t filesize;//ファイルサイズ
	size_t readed;//読み込んだデータ
	size_t count;//count
	size_t i;
	size_t rest;//残り
	int result = FALSE;//戻り値
	DKC_THREAD_LOCK *lock = NULL;
	DKC_FILECOPY_CALLBACK_STRUCT cbdata;


	if(NULL==dest || NULL==src){
		return FALSE;
	}
	if(inner_buffer_size <= 1024){
		inner_buffer_size = 1024;
	}

	//バッファの準備
	buff = malloc(inner_buffer_size);
	if(NULL==buff){
		inner_buffer_size = 1024 * 256;
		buff = malloc(inner_buffer_size);
		if(NULL==buff)
			return FALSE;
	}
	//thread lock
	if(bThreadLock){
		lock = dkcAllocThreadLock();
		if(NULL==lock){
			goto Error;
		}
		dkcThreadLock_Lock(lock);
	}
	//ちっちゃい場合
	filesize = dkcFileSize(src);
	//destが存在する
	if(FALSE==bReWrite && TRUE==dkcFileExist(dest)){
		goto Error;
	}
	cbdata.filesize = filesize;
	cbdata.count = 0;
	for(;;)
	{//non loop 
		if(0 == filesize)
		{//ファイルサイズが０の場合
			if(TRUE==dkcCreateEmptyFile(dest))
			{
				result = TRUE;
			}
			break;
		}
		if(filesize < inner_buffer_size)
		{//ファイルサイズがバッファに収まってしまう場合。
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
		//バッファを使わなければコピーできない場合。

		srcf = dkcFOpen(src,"rb");
		if(NULL==srcf) goto Error;
		destf = dkcFOpen(dest,"wb");
		if(NULL==destf) goto Close;

		//ループする回数を計算
		count = filesize / inner_buffer_size;

		for(i=0;i<count;i++){
			dkcmFORCE_NOT_ASSERT(1 != fread(buff,inner_buffer_size,1,srcf));
			dkcmFORCE_NOT_ASSERT(1 != fwrite(buff,inner_buffer_size,1,destf));
			
			cbdata.count += inner_buffer_size;
			if(pfcallback){//コールバックで相手方に伝える。
				if(FALSE==pfcallback(&cbdata,parg)){
					goto Close;
				}
			}
		}

		rest = filesize - (count * inner_buffer_size);
		
		//残りを出力
		dkcmFORCE_NOT_ASSERT(rest != fread(buff,1,rest,srcf));
		dkcmFORCE_NOT_ASSERT(rest != fwrite(buff,1,rest,destf));
		
		
		cbdata.count += rest;
		dkcmNOT_ASSERT(cbdata.count != cbdata.filesize);
		if(pfcallback){//コールバックで相手方に伝える。
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
	if(bThreadLock){
		if(lock){
			dkcThreadLock_Unlock(lock);
			dkcFreeThreadLock(&lock);
		}
	}
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
	
	//ファイルの存在を確かめる
	r = dkcFileExist(filename1);
	if(r==FALSE){
		return edk_ArgumentException;
	}
	r = dkcFileExist(filename2);
	if(r==FALSE){
		return edk_ArgumentException;
	}

	//ファイルのサイズを確かめる
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
		if(FALSE==r){//ファイルサイズが違った
			return edk_FAILED;
		}

	}


	{
		DKC_STREAM *s1 = NULL,*s2=NULL;
		BYTE *buffer1 = NULL,*buffer2 = NULL;
		size_t buffsize = 1024 * 52;


		//initialize

		//ファイルをオープン
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

		//バッファを用意
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

		//ファイルの存在を確かめる
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
	{//DWORD_MAX以上のサイズのファイルをメモリには読みこめないですよ^^;
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

	//ディレクトリ
	p->mDir = dkcAllocPathString(dir);//allocate and copy 
	if(NULL==p->mDir){
		goto Error;
	}
	//再帰用スタック
	p->mStack = dkcAllocStack(10,sizeof(DKC_PATHSTRING *));
	if(NULL==p->mStack){
		goto Error;
	}
	//target文字列
	p->mTarget = dkcAllocString(128);
	if(NULL==p->mTarget){
		goto Error;
	}
	//target文字列をコピー
	dkcStringCopy(p->mTarget,target,strlen(target));

	p->mState = edkcFileFinderEmpty;//何もしていない。
	p->mbSubDir = bSubDir;//サブdirectoryも検索するかどうか

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
	{/*こんなのじゃ文字列はいただけないなぁ。*/\
		p->mState = edkcFileFinderSearching;\
		return WithFolderSearch(p,path,bCopySucceeded);\
	}\
	if(TRUE==isFolder(p))\
	{/*おや？フォルダーだ。*/\
		dkcFileFinderReferenceFileName(p,path);\
		p->mState = edkcFileFinderSearching;\
		FFPushStack(p);/*folderをスタックに詰める。*/\
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
	{//呼び出して最初だったら、
		r = dkcFindFirstFile(p);
		if(DKUTIL_FAILED(r)) return edk_FAILED;
#	ifdef WIN32 //windowsの場合は内部に格納している･･･。
		if(TRUE==isDot(p))
		{//こんなのじゃ文字列はいただけないなぁ。
			p->mState = edkcFileFinderSearching;
			return WithFolderSearch(p,path,bCopySucceeded);
		}
		if(TRUE==isFolder(p))
		{//おや？フォルダーだ。
			dkcFileFinderReferenceFileName(p,path);
			p->mState = edkcFileFinderSearching;
			FFPushStack(p);//folderをスタックに詰める。
			return WithFolderSearch(p,path,bCopySucceeded);
		}
		//FFCHECKING(p,path,bCopySucceeded)

		r = dkcFileFinderReferenceFileName(p,path);
			
		if(DKUTIL_SUCCEEDED(r)){//コピーも成功
			*bCopySucceeded = TRUE;
		}
#	endif
		p->mState = edkcFileFinderSearching;
		return r;
	}else if(edkcFileFinderSearching == p->mState)
	{//検索中だったら。
		r = dkcFindNextFile(p);
		if(edk_SUCCEEDED == r)
		{//成功だ。
			if(TRUE==isDot(p))
			{//こんなのじゃ文字列はいただけないなぁ。
				p->mState = edkcFileFinderSearching;
				return WithFolderSearch(p,path,bCopySucceeded);
			}
			if(TRUE==isFolder(p))
			{//おや？フォルダーだ。
				dkcFileFinderReferenceFileName(p,path);
				p->mState = edkcFileFinderSearching;
				FFPushStack(p);//folderをスタックに詰める。
				return WithFolderSearch(p,path,bCopySucceeded);
			}

			r = dkcFileFinderReferenceFileName(p,path);
			
			if(DKUTIL_SUCCEEDED(r)){//コピーも成功
				*bCopySucceeded = TRUE;
			}/*else{//コピー失敗！？？
				*bCopySucceeded = FALSE;
			}*/
		}else if(edk_EndProcess == r)
		{//検索が終わった。
			if(FALSE==FFIsStackEmpty(p))
			{//空ではない。
				FFReSearch(p);
				return WithFolderSearch(p,path,bCopySucceeded);
			}else{
				dkcFindClose(p);//終了ナリ。（内部でp->mState = edkcFileFinderFinish;をやっている。
			}
		}else{
			dkcmNOT_ASSERT("そんなばかな～");
		}
		return r;
	}else if(edkcFileFinderFinish == p->mState)
	{
		return edk_EndProcess;
	}
	dkcmNOT_ASSERT("dkcFileFinderNextのプログラムがおかしい。チートされているかも！？");
	return edk_FAILED;

}


static int NormalSearch(DKC_FILE_FINDER *p,DKC_PATHSTRING *path,BOOL *bCopySucceeded)
{
	int r;
	r = 0;
	*bCopySucceeded = FALSE;

	if(edkcFileFinderEmpty == p->mState)
	{//呼び出して最初だったら、
		r = dkcFindFirstFile(p);
		if(DKUTIL_FAILED(r)) return edk_FAILED;
#	ifdef WIN32 //windowsの場合は内部に格納している･･･。
		if(TRUE==isDot(p) || TRUE==isFolder(p))
		{//こんなのじゃ文字列はいただけないなぁ。
			p->mState = edkcFileFinderSearching;
			return NormalSearch(p,path,bCopySucceeded);
		}
		r = dkcFileFinderReferenceFileName(p,path);
			
		if(DKUTIL_SUCCEEDED(r)){//コピーも成功
			*bCopySucceeded = TRUE;
		}
#	endif
		p->mState = edkcFileFinderSearching;
		return r;
	}else if(edkcFileFinderSearching == p->mState)
	{//検索中だったら。
		r = dkcFindNextFile(p);
		if(edk_SUCCEEDED == r)
		{//成功だ。
			if(TRUE==isDot(p) || TRUE==isFolder(p))
			{//こんなのじゃ文字列はいただけないなぁ。



				//p->mState = edkcFileFinderSearching;
				return NormalSearch(p,path,bCopySucceeded);
			}
			r = dkcFileFinderReferenceFileName(p,path);
			
			if(DKUTIL_SUCCEEDED(r)){//コピーも成功
				*bCopySucceeded = TRUE;
			}/*else{//コピー失敗！？？
				*bCopySucceeded = FALSE;
			}*/
		}else if(edk_EndProcess == r)
		{//検索が終わった。
			dkcFindClose(p);//終了ナリ。（内部でp->mState = edkcFileFinderFinish;をやっている。
		}else{
			dkcmNOT_ASSERT("そんなばかな～");
		}
		return r;
	}else if(edkcFileFinderFinish == p->mState)
	{
		return edk_EndProcess;
	}/*else{
		return edk_FAILED;
	}*/
	dkcmNOT_ASSERT("dkcFileFinderNextのプログラムがおかしい。チートされているかも！？");
	return edk_FAILED;

}

/*!
@param ptr[in] dkcAllocFileFinder()で取得したポインタ
@return edk_SUCCEEDEDならdkcFileFinderGetResult()を呼び出して良い。
それ以外ならdkcFreeFileFinder()で終了させなければならない。
*/
int WINAPI dkcFileFinderNext(DKC_FILE_FINDER *p,DKC_PATHSTRING *path,BOOL *bCopySucceeded)
{
	if(FALSE==p->mbSubDir){
		return NormalSearch(p,path,bCopySucceeded);
	}else{//sub directoryも検索
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
		{//なんだ、もう無いじゃん。
			return edk_EndProcess;
		}
		else//最悪のパターン
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
    {//もう無いよ＾＾；
			return edk_EndProcess;
    }
		else//最悪のパターン
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
	ptr->mHandle = INVALID_HANDLE_VALUE;//しっかりリセット
#else
	if(0 == ptr->mHandle){
		return edk_FAILED;
	}
	closedir(ptr->mHandle);
	ptr->mHandle = 0;//しっかりリセット
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
	{//呼び出しが間違っているよ＾＾；
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
		return edk_LogicError;//呼び出しが間違っているナリ！
	}
#	ifdef WIN32
	if ( 0 == FindNextFileA( ptr->mHandle, &(ptr->mFindData) ))
	{
		if ( GetLastError() == ERROR_NO_MORE_FILES )
		{//なんだ、もう無いじゃん。
			return edk_EndProcess;
		}
		else//最悪のパターン
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
    {//もう無いよ＾＾；
			return edk_EndProcess;
    }
		else//最悪のパターン
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
	ptr->mHandle = INVALID_HANDLE_VALUE;//しっかりリセット
#else
	if(0 == ptr->mHandle){
		return edk_FAILED;
	}
	closedir(ptr->mHandle);
	ptr->mHandle = 0;//しっかりリセット
	ptr->mDirent = NULL;
#endif
	//終了！！
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
	{//呼び出しが間違っているよ＾＾；
		return edk_LogicError;
	}
	len = strlen(ptr->mDirent.d_name);
	if(0 == len) return edk_FAILED;
	r = dkcPathStringCopy(path,ptr->mDirent.d_name,len);
#endif
	return r;
}
*/