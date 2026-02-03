/*!
@file dkcStdio.h
@brief		buffer over flowしないよ　関数^^;
@author		d金魚
@since		transplanted from dkutilStdio.h:2004/2/28
@todo 
前までstdioというのはC言語で標準的な意味を持つのか？と思ってきたが、
最近STanDard Input Outputの略と言うことが分かった。
なのでこのファイル名はSTanDard LIBraryの方がイイと思ったので dkcStdlibにしようかと考えて見た。
が、面倒なのでいちおうこのままにしておこう＾＾；
@note
ここを見よ！<br>
http://www-6.ibm.com/jp/developerworks/security/000616/j-buffer-defend.html
*/
#ifndef DKUTIL_C_STDIO_H
#define DKUTIL_C_STDIO_H


#include "dkcOSIndependent.h"

#ifdef __cplusplus
namespace dkutil{
	/*!
	@param dest[out] コピー先
	@param size1[in] destのサイズ
	@param src[in] コピー元
	@param size2[in] srcのサイズ
	@return size1 < size2 だったらedk_FAILEDを返す。
	*/
	///memcpyの安全版？
	inline int dkmemcpy(void *dest,size_t size1,const void *src,size_t size2){
		if(size1 < size2) return edk_FAILED;
		::memcpy(dest,src,size2);
		return edk_SUCCEEDED;
	}
	/*!
	@param dest[out] コピー先
	@param size1[in] destのサイズ
	@param src[in] コピー元
	@param size2[in] srcのサイズ
	@return size1 < size2 だったらedk_FAILEDを返す。
	*/
	///memmoveの安全版？
	inline int dkmemmove( void *dest,size_t size1,const void *src, size_t size2 ){
		if(size1 < size2) return edk_FAILED;
		::memmove(dest,src,size2);
		return edk_SUCCEEDED;
	}
	/*!
	@todo 
	sprintf見たいに書式つきなんたらかんたらが上手く出来ない %xとか%fとか？
	それを直す。
	つーかboost::format使ったほうがイイナ＾＾；；；廃止にしチャおっかな。この関数。
	*/
#	if 0
	inline int dksprintf(char *buff,size_t bufsize,char *format,...){
		BOOST_STATIC_ASSERT(!"つかうなー！");
		//char buff[2048]="";
		size_t buffnum=bufsize;
		char *origin = format;
		{
			va_list VaList;
			va_start( VaList , origin ) ;
			int r;
			r = _vsnprintf( buff ,buffnum - 1, origin , VaList );
			if(0 > r){
				return edk_FAILED;
			}else{
				buff[r+1] = '\0';
			}
			va_end( VaList ); 
		}

		return edk_SUCCEEDED;
	}
#	endif //end of if 0
	/*!
	@note
	<b>strncpyの危険性</b><br>
	string2 の最初の count 文字数を string1 にコピーしてから string1 を返します。
	count がstring2 の長さ以下の場合は、
	<font color=red>コピーした文字列に自動的に NULL 文字 (\0) を付けません。</font><br>
	<br>
	なので、srclenのサイズがsize1より大きい場合はedk_FAILEDを返します。
	@param dest[out] コピー先
	@param size1[in] destのサイズ
	@oaram src[in] コピー元
	@param srclen[in] srcからコピーする文字数
	@bug
	bug:		if(size1 < srclen) correct:if(size1 <= srclen)
	*/
	inline int dkstrcpy(char *dest,size_t size1,const char *src,size_t srclen){
		if(size1 <= srclen) return edk_FAILED;
		::strncpy(dest,src,srclen);
		//if(dest[srclen] != '\0') return edk_FAILED;
		dest[srclen] = '\0';//0を入れるからsrclen + 1ではない。
		return edk_SUCCEEDED;
	}
	///@see dkstrcpy(char *dest,size_t size1,const char *src,size_t srclen);
	inline int dkstrcpy(BYTE *dest,size_t size1,const BYTE *src,size_t srclen)
	{
		return dkstrcpy((char *)dest,size1,(const char *)src,srclen);
	}
	/*!
	@bug
	bug:if(size1 <  srclen) correct:if(size1 <= srclen)
	@todo
	size1が0の時危ない･･･さぁ、チェックするべきか？
	いや、大丈夫だな。srclenも０じゃないと成り立たない。
	
	*/
	///dkstrcpyと違い、バッファに入るだけコピーする。@note amap = as much as possible.
	inline int dkstrcpy_amap(char *dest,size_t size1,const char *src,size_t srclen){
#	ifndef NDEBUG
		dkcmFORCE_NOT_ASSERT((0 == size1 && 0 == srclen));
#	endif
		if(size1 <= srclen){//ここは<=か！？
			::strncpy(dest,src,size1);
			dest[size1 - 1] = '\0';
			return edk_Not_Satisfactory;
		}else{
			::strncpy(dest,src,srclen);
			dest[srclen] = '\0';
			return edk_SUCCEEDED;
		}
	}
	inline int dkstrcpy_amap2(char *dest,size_t size1,const char *src){
		return dkstrcpy_amap(dest,size1,src,strlen(src));
	}
	/*!
	@param dest[out] コピー先
	@param destsize[in] destのサイズ(byte)
	@param destlen[in] destに入っている文字列の長さ *1
	@param src[in] コピー元
	@param srclen[in] コピー元に入っている文字列の長さ *1
	@note
	<b>*1:</b> NULL文字を含まない。<br>
	<b>strncatの危険性？</b><br>
	内部実装がどうなっているかは分からないが、
	いちおうdestのサイズをチェックしておいた方が安全だと思う。<br>
	*/
	inline int dkstrcat(char *dest,size_t destsize,size_t destlen,
		const char *src,size_t srclen)
	{
		//destlen + srclen + NULL文字
		if(destsize < destlen + srclen + 1)	return edk_FAILED;
		::strncat(dest,src,srclen);
		return edk_SUCCEEDED;
	}
	///dkstrcat()のHelper 関数
	inline int dkstrcat2(char *dest,size_t destsize,const char *src){
		return dkstrcat(dest,destsize,strlen(dest),src,strlen(src));
	}
	///dkstrcat()の出来るだけコピーする版 
	inline int dkstrcat_amap(char *dest,size_t destsize,size_t destlen,
		const char *src,size_t srclen)
	{
		//destlen + srclen + NULL文字
		int result = edk_SUCCEEDED;
		size_t lsize = destlen + srclen + 1;
		if(destsize < lsize)
		{//オーバーしてたら･･･
			srclen = lsize - destsize - 1;
			result = edk_Not_Satisfactory;
		}
		::strncat(dest,src,srclen);
		//dest[destlen + srclen - 1] = '\0';
		return result;
	}
	///dkstrcat3のHelper
	inline int dkstrcat_amap2(char *dest,size_t destsize,const char *src){
		return dkstrcat_amap(dest,destsize,strlen(dest),src,strlen(src));
	}

	/*!
	@param buf1[in] 比較バッファその１
	@param buf1PermitSize[in] buf1のサイズ
	@param buf2[in] 比較バッファその２
	@param count[in] 何バイト比較するかどうかのサイズ
	@return  buf1PermitSize < count の時、edk_LogicErrorを返す。
	@note
	*/	
	inline int dkmemcmp( const void *buf1,size_t buf1PermitSize,
		const void *buf2, size_t count )
	{
		if(buf1PermitSize < count)return edk_LogicError;
		if(0 !=::memcmp(buf1,buf2,count))return edk_FAILED;
		return edk_SUCCEEDED;
	}

	/*!
	@param b1[in] 検索されるデータ
	@param b1size[in] b1の数
	@param b2[in] 検索するデータ
	@param b2size[in] b2の数
	@return b1size < b2sizeの時、edk_LogicErrorを返す。
	b1とb2が両方とも同じ時はedk_SUCCEEDED 違う時はedk_FAILED
	*/
	///コンペアのベース
	template<typename TYPE_>
	inline int WINAPI dkcmp_base(const TYPE_ *b1,size_t b1size,
		const TYPE_ *b2,size_t b2size)
	{
		if(b1size < b2size) return edk_LogicError;
		int result = edk_SUCCEEDED;
		//とりあえずマッチするか確かめる。
		for(size_t i=0;i<b2size;i++)
		{
			if(b1[i] != b2[i])
			{
				result = edk_FAILED;
				break;
			}
		}
		return result;
	}

	/*!
	@param b1[in] 検索される文字列
	@param b1size[in] b1の文字数
	@param b2[in] 検索する文字列
	@param b2size[in] b2の文字数
	@return b1size < b2sizeの時、edk_LogicErrorを返す。

	*/
	inline int dkstrcmp(const char *b1,size_t b1size,const char *b2,size_t b2size)
	{
		//検索する文字列が多いとおかしいのでエラー
		if(b1size < b2size) return edk_LogicError;
		
		int result = edk_SUCCEEDED;
		if(b2size % 4 == 0)
		{//無理やりintで検索
			size_t fb1size = b1size / 4;
			size_t fb2size = b2size / 4;
			const int *fb1 = (const int *)b1;
			const int *fb2 = (const int *)b2;
			return dkcmp_base(fb1,fb1size,fb2,fb2size);
		}
		//else
		//{
			//basic な検索･･･。
			return dkcmp_base(b1,b1size,b2,b2size);
		//}
	}
	/*!
	@param num[in] 桁数を数えたい変数
	@return 整数の桁数
	@todo
	ビット演算でどうにか出来そうだけど＾＾；；ウーム
	*/
	///整数の桁数を数える
	template<typename INTEGER_>
	inline size_t beam(INTEGER_ num){	
		
		size_t b = 0 ;
		for(INTEGER_ i = 10 ; num >= i ; i *= 10 ){
			b ++ ;
		}
		return b;
	}

	/// va から size バイトの内容と、vb から size バイトの内容を交換する。
	inline void memswap( void *va, void *vb, size_t size )
	{
		char *ca;
		char *cb;
		char  cTmp;
		for( ca = (char *)va, cb = (char *)vb; size > 0; size--, ca++, cb++ )
		{
			cTmp = *ca;
			*ca  = *cb;
			*cb  = cTmp;
		}
	}//memswap



	/// 大きさ size の要素 num 個からなる配列 array の要素を
	/// 逆順に並べ替える。
	inline void memreverse( void *array, size_t num, size_t size )
	{
		char *l;
		char *r;

		l = (char *)array;
		r = (char *)array + ((num-1) * size);
		for( ; l < r; l = l+size, r = r-size )
			memswap( l, r, size );
	}//memreverse


	/// 大きさ size の要素 num 個からなる配列 array の要素を
	/// 右方向に要素 shift 個分だけ回転移動する。
	inline void memrrotate( void *array, size_t shift, size_t num, size_t size )
	{
		memreverse( array,                      num,       size ); // 012345678 -> 876543210
		memreverse( array,                      shift,     size ); // 876543210 -> 678543210
		memreverse( (char *)array+(size*shift), num-shift, size ); // 678543210 -> 678012345
	}//memrrotate


	/// 大きさ size の要素 num 個からなる配列 array の要素を
	/// 左方向に要素 shift 個分だけ回転移動する。
	inline void memlrotate( void *array, size_t shift, size_t num, size_t size )
	{
		memreverse( array,                      shift,     size ); // 012345678 -> 210345678
		memreverse( (char *)array+(size*shift), num-shift, size ); // 210345678 -> 210876543
		memreverse( array,                      num,       size ); // 210876543 -> 345678012
	}//memlrotate
}//end of dkutil namespace
#if 0
inline int WINAPI dkc_memcpy(void *dest,size_t size1,const void *src,size_t size2){
	return dkutil::dkmemcpy(dest,size1,src,size2);
}
inline int WINAPI dkc_strcpy(char *dest,size_t size1,const char *src,size_t srclen){
	return dkutil::dkstrcpy(dest,size1,src,srclen);
}
inline int WINAPI dkc_strcpy(BYTE *dest,size_t size1,const BYTE *src,size_t srclen){
	return dkc_strcpy((char *)dest,size1,(const char *)src,srclen);
}
inline int WINAPI dkc_strcpy_amap(char *dest,size_t size1,const char *src,size_t srclen){
	return dkutil::dkstrcpy_amap(dest,size1,src,srclen);
}
///@see dkutil::dkstrcpy_amap2
inline int WINAPI dkc_strcpy_amap2(char *dest,size_t size1,const char *src){
	return dkutil::dkstrcpy_amap2(dest,size1,src);
}
///@see dkutil::dkstrcat
inline int WINAPI dkc_strcat(char *dest,size_t destsize,size_t destlen,
		const char *src,size_t srclen)
{
		return dkutil::dkstrcat(dest,destsize,destlen,src,srclen);
}
///@see dkutil::dkstrcat2
inline int WINAPI dkc_strcat2(char *dest,size_t destsize,const char *src){
	return dkutil::dkstrcat2(dest,destsize,src);
}
///@see dkutil::dkstrcat_amap
inline int WINAPI dkc_strcat_amap(char *dest,size_t destsize,size_t destlen,
		const char *src,size_t srclen)
{
	return dkutil::dkstrcat_amap(dest,destsize,destlen,src,srclen);
}
///@see dkutil::dkstrcat_amap2
inline int WINAPI dkc_strcat_amap2(char *dest,size_t destsize,const char *src){
	return dkutil::dkstrcat_amap2(dest,destsize,src);
}
///@see dkutil::dkmemcmp
inline int WINAPI dkc_memcmp( const void *buf1,size_t buf1PermitSize,
	const void *buf2, size_t count )
{
	return dkutil::dkmemcmp(buf1,buf1PermitSize,buf2,count);
}
inline int WINAPI dkc_strcmp(const char *b1,size_t b1size,const char *b2,size_t b2size)
{
	return dkutil::dkstrcmp(b1,b1size,b2,b2size);
}
inline size_t dkc_beam(int num){	
	return dkutil::beam(num);
}

///@see dkutil::memswap
inline void dkc_memswap( void *va, void *vb, size_t size )
{
	dkutil::memswap(va,vb,size);
}

///@see dkutil::memreverse
inline void dkc_memreverse( void *array, size_t num, size_t size )
{
	dkutil::memreverse(array,num,size);
}
///@see dkutil::memrrotate
inline void dkc_memrrotate( void *array, size_t shift, size_t num, size_t size )
{
	dkutil::memrrotate(array,shift,num,size);
}

///@see dkutil::memlrotate
inline void dkc_memlrotate( void *array, size_t shift, size_t num, size_t size )
{
	dkutil::memlrotate(array,shift,num,size);
}
#endif//end of if 0

#else //for C lang

#endif //end of __cplusplus

//mempry用

DKC_EXTERN int WINAPI dkc_memcpy(void *dest,size_t size1,const void *src,size_t size2);

///zero checked memcpy
DKC_EXTERN int WINAPI dkc_memcpy_zc(void *dest,size_t size1,const void *src,size_t size2);


DKC_EXTERN int WINAPI dkc_memmove(void *dest,size_t size1,const void *src, size_t size2 );


///@see dkutil::dkmemcmp
DKC_EXTERN int WINAPI dkc_memcmp( const void *buf1,size_t buf1PermitSize,
	const void *buf2, size_t count );


//char 文字列用

DKC_EXTERN int WINAPI dkc_strcpy(char *dest,size_t size1,const char *src,size_t srclen);

DKC_EXTERN int WINAPI dkc_strcpy_amap(char *dest,size_t size1,const char *src,size_t srclen);
///@see dkutil::dkstrcpy_amap2
DKC_EXTERN int WINAPI dkc_strcpy_amap2(char *dest,size_t size1,const char *src);
///@see dkutil::dkstrcat
DKC_EXTERN int WINAPI dkc_strcat(char *dest,size_t destsize,size_t destlen,
		const char *src,size_t srclen);
///@see dkutil::dkstrcat2
DKC_EXTERN int WINAPI dkc_strcat2(char *dest,size_t destsize,const char *src);
///@see dkutil::dkstrcat_amap
DKC_EXTERN int WINAPI dkc_strcat_amap(char *dest,size_t destsize,size_t destlen,
		const char *src,size_t srclen);
///@see dkutil::dkstrcat_amap2
DKC_EXTERN int WINAPI dkc_strcat_amap2(char *dest,size_t destsize,const char *src);

DKC_EXTERN int WINAPI dkc_strcmp(const char *b1,size_t b1size,const char *b2,size_t b2size);

//wchar文字列用
/*
なんか、バッファのバイト単位と文字単位が引数にごちゃごちゃになりそうなので保留

DKC_EXTERN int WINAPI dkc_wcsncpy( wchar_t *dest,size_t destsize, const wchar_t *src, size_t srcsize );
///destsizeはバイト単位･･･
DKC_EXTERN int WINAPI dkc_wcsncat( wchar_t *dest,size_t destsize,size_t destlen,
		const char *wchar_t,size_t srclen );
*/
///destsizeはバイト単位･･･
DKC_EXTERN int WINAPI dkc_wcscat2( wchar_t *dest,size_t destsize,
		const wchar_t *src);



///@return 10進法での桁数を返す
DKC_EXTERN size_t dkc_beam(int num);

///@see dkutil::memswap
DKC_EXTERN void dkc_memswap( void *va, void *vb, size_t size );

///@see dkutil::memreverse
DKC_EXTERN void dkc_memreverse( void *array, size_t num, size_t size );
///@see dkutil::memrrotate
DKC_EXTERN void dkc_memrrotate( void *array, size_t shift, size_t num, size_t size );

///@see dkutil::memlrotate
DKC_EXTERN void dkc_memlrotate( void *array, size_t shift, size_t num, size_t size );

/*!
@param memory[in][out] バッファへのポインタ
@param size[in] memoryのサイズ
@param point[in] 何処を基点(何バイト目から)にSwapするか？
@return edk_SUCCEEDED以外ならエラー
@note
char *buff="buffer";みたいに初期化したバッファをSwapさせるとエラーする。
*/
///一つのメモリ領域をpointを基準に入れ替える。
DKC_EXTERN int dkc_stream_swap(char *memory,size_t size,size_t point);



#if 0
/*!
@note
	出典：二反田 雅彦 氏のKittyエンジンより

  strnicmp - ２つの文字列中の文字を、大文字小文字を区別せずに
              比較します。

  ※この関数は、Visual C++にはあるが、ボーランドコンパイラには
  なかった。このため、ボーランド用に作成した。
  　ここでは、あまり役に立たない文字列の大小の判定は行わないこ
  ととする。

　引数：
    const char *string1　比較する文字１
    const char *string2　比較する文字２
    size_t     count　　 比較する文字数

  戻り値：
    int ２つの文字が等しければ０を、等しくなければ１を返す
        ※比較する文字数に達する前に、文字列の終端に達した場合は、
        等しくないとみなす。

@param string1　比較する文字１
@param string2　比較する文字２
@param 比較する文字数
@return ２つの文字が等しければ０を、等しくなければ１を返す
※比較する文字数に達する前に、文字列の終端に達した場合は、等しくないとみなす。
*/

DKC_EXTERN int WINAPI strnicmp(
    const char *string1, const char *string2, size_t count);

#endif




#endif //end of include once