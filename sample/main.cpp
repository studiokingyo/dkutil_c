/**
@filename sample.c
@brief サンプル集
@note

一部にM.Hiroi's Home Page 
http://www.geocities.jp/m_hiroi/
よりプログラムを利用しております。
<PRE>
権利・免責事項など
『続・サルでも書けるＣプログラム講座』、『番外編』、
『Ｃ++編』の著作権は筆者「広井誠 (Makoto Hiroi) 」が保持します。
無断使用や無断転載は禁止いたします。

『続・サルでも書けるＣプログラム講座』、『番外編』、
『Ｃ++編』で作成したプログラムはフリーソフトウェアとします。
ご自由にお使いください。プログラムの改造や配布もご自由にどうぞ。
ただし、これらのプログラムは無保証であり、
使用したことにより生じた損害について、
作者「広井誠 (Makoto Hiroi) 」は一切の責任を負いません。
また、これらのプログラムを販売することで利益を得るといった
商行為は禁止いたします。 

Copyright (C) 1995-2005 Makoto Hiroi
All rights reserved.
</PRE>
http://www.geocities.jp/m_hiroi/zsaru.htmlより引用（2005/12/31）

*/

///dkc.hが上のディレクトリにあるため
#include "../dkc.h"
//#include "sha1.h"
//#include "sha2.h"

#include <boost/progress.hpp>

#define FOR if(FALSE){}else for

#define dODS(s) printf("%s\n",s)
#define if_free(s) if(s){free(s);s=NULL;}



extern void Test_dkcSJIS_StrSearch();

extern void Test_dkcPathString();

extern void Test_dkcStackAndQueue();

extern void Test_dkcString();

extern void Test_dkcCircularMemoryStream();

extern void Test_dkcSingleList();

extern void Test_dkcLZSS();

extern void Test_dkcSort();

extern void Test_dkcHashDigest();

extern void Test_ArrayOneByOne();

extern void Test_CRC();

extern void Test_BMStrStr();

extern void Test_RLE();


///シフトSJISをスキップして特定の文字を検索するもの
void Test_dkcSJIS_StrSearch(){
	const char *sjis="ЫⅨ噂浬欺圭構蚕十申曾箪貼能表暴予禄兔喀媾彌拿杤歃濬畚秉綵臀藹觸軆鐔饅鷭\\";
	size_t len,found;
	found = dkcSJIS_StrChrSearch(sjis,'\\');
	len = strlen(sjis);
	dkcmFORCE_NOT_ASSERT(found != len - 1);
}


///パス文字列格納用コンテナ
void Test_dkcPathString()
{

	const char *dir="Z:\\ok\\―ソЫⅨ噂浬欺圭構蚕十申曾箪貼能表暴予禄兔喀媾彌拿杤歃濬畚秉綵臀藹觸軆鐔饅鷭\\偆砡.ok";
	const char *relative_dir="I:\\python\\..\\ruby\\..\\tcl\\..\\.\\\\\\.\\.\\.\\―ソЫⅨ噂浬欺圭";
	const char *relative_dir2="\\python\\..\\ruby\\..\\tcl\\";
	{
		DKC_PATHSTRING *ps = dkcAllocPathString(dir);
		char buff[dkcdMAXPATH_BUFFER]="";

		//ファイルパスの要素の表示テスト
		dkcPathStringGetDrive(ps,buff,sizeof(buff));
		printf("Drive = %s ",buff);

		dkcPathStringGetFileName(ps,buff,sizeof(buff));
		printf("FileName = %s ",buff);

		dkcPathStringGetFileExtension(ps,buff,sizeof(buff));
		printf("FileExtension = %s ",buff);

		dODS("");
		//ファイルパスの要素の分割テスト
		dkcPathStringDevideBegin(ps,buff,sizeof(buff));
		FOR(int i=0;;i++){
			int r;
			printf("%s ",buff);
			r = dkcPathStringDevideNext(ps,buff,sizeof(buff));
			if(edk_EndProcess==r){
				break;
			}
			dkcmNOT_ASSERT(DKUTIL_FAILED(r));
		}
		dkcPathStringDevideEnd(ps);
		dODS("");

		//パスの正規化のテスト
		dkcPathStringCopy(ps,relative_dir,strlen(relative_dir));
		
		printf("%s -> ",relative_dir);
		dODS(dkcPathStringPointer(ps));

		dkcPathStringCopy(ps,relative_dir2,strlen(relative_dir2));
		
		printf("%s -> ",relative_dir2);
		dODS(dkcPathStringPointer(ps));//ここで、今、実行しているファイルがあるドライブが付加されている。
		//printf("%s ",buff);

		dkcFreePathString(&ps);
		dODS("");
	}
}

///。。。。将来削除予定
void Test_dkcSingleListObject(){
	{
		BYTE data = 'a';
		DKC_SINGLELIST_OBJECT *p = dkcAllocSingleListObject(&data,1);//push a
		data++;//b
		p->push_back(p,&data,1);//push b1
		p->push_back(p,&data,1);//push b2
		p->erase(p,p->begin(p)->mNext,p->begin(p)->mNext->mNext);//delete b1
		data++;//c
		p->push_back(p,&data,1);//push c;
		p->erase(p,p->begin(p),p->begin(p)->mNext);//delete b2

		FOR(DKC_SINGLELIST *it = p->begin(p);it != NULL;it = p->next(it))
		{
			BYTE buffer;
			p->getbuffer(it,&buffer,1);
			printf("%c ",buffer);//output:a c
		}
		dkcFreeSingleListObject(&p);
	}

}
///文字列格納用コンテナのサンプル
void Test_dkcString(){
	const char *astr="abvb";
	const char *bstr="ほげほげ～！";
	const char *cstr="ｷﾀ━━━━(ﾟ∀ﾟ)━━━━!!";
	{
		size_t alen = strlen(astr);
		size_t blen = strlen(bstr);
		size_t clen = strlen(cstr);
		DKC_STRING *p = dkcAllocString(alen);
		
		//dkcStringCopy(p,astr,alen);
		dkcStringConcatenate(p,astr,alen);
		dkcStringConcatenate(p,bstr,blen);
		dkcStringConcatenate(p,cstr,clen);

		dkcStringInsert(p,2,"ok",2);//abokvb
		dkcStringErase(p,0,2);//okvb
		dkcStringReplace(p,2,4,"OK",2);//okOK


		printf("DKC_STRING *p == %s",dkcStringPointer(p));
		printf("\n");

		dkcFreeString(&p);
	}
}
///円方バッファ（ストリーム）のサンプル
void Test_dkcCircularMemoryStream(){
	const int count = 5;
	const size_t bytes = (size_t)sizeof(int) * count;
	const char *astr="abcde";
	{
		DKC_CIRCULAR_MEMORYSTREAM *p = 
			dkcAllocCircularMemoryStream(bytes);

		//基本動作確認
		FOR(int i=0;i<100;i++){
			if(DKUTIL_FAILED(dkcCircularMemoryStreamWrite(p,&i,sizeof(i)))){
				printf("count of %d | ",i);
				break;
			}
		}
		printf("DKC_CIRCULAR_MEMORYSTREAM(%d bytes): ",bytes);
		FOR(int i=0;i<count;i++){
			int r;size_t read;
			dkcCircularMemoryStreamRead(p,&r,sizeof(i),&read);
			dkcmNOT_ASSERT(sizeof(i) != read);
			printf("%d ",r);
		}
		dODS("");
		
		dkcCircularMemoryStreamClear(p);

		//可変長配列を入れた時の動作確認
		FOR(int i=0;i<100;i++){//a ab abc abcd abcde
			if(DKUTIL_FAILED(dkcCircularMemoryStreamWrite(p,astr,i+1))){
				printf("count of %d | ", i );//i = 1なので
				break;
			}
		}
		
		printf("DKC_CIRCULAR_MEMORYSTREAM(%d bytes): ",bytes);
		
		FOR(int i=0;i<100;i++){
			char r[bytes]="";
			size_t read;
			if(DKUTIL_FAILED(
				dkcCircularMemoryStreamRead(p,&r,count - i,&read)
				))
			{
				break;
			}
			dkcmNOT_ASSERT(size_t(count - i) != read);
			printf("%s ",(const char *)r);//aabab cabc dab cd e
		}
		dODS("");

		dkcFreeCircularMemoryStream(&p);
	}
		
}



void Test_BMStrStr(){
	const char *dest="hellow vaold hellw";
	const char *src="lw";
	
	int f = dkcBMStrStr(dest,src);
	printf("%dで%sが見つかった",f,&dest[f]);

}

///単方向リストのサンプル
void Test_dkcSingleList()
{
	{
		const char *begin_signature="begin--\n";
		DKC_SINGLELIST *p = dkcAllocSingleList(begin_signature,
			strlen(begin_signature) + 1);// + 1はNULL文字分

		if(!p) return;//エラーかよ！
		

		//基本的な使い方
		DKC_SINGLELIST *begin = p;

		{for(int i=0;i<5;i++){
			char st[2];
			st[0]='A';
			st[1]='\0';//NULL文字を入れる事によって文字列とする。
			dkcSingleListInsert(p,st,sizeof(st));
		}}
		
		{
			DKC_SINGLELIST *it = begin;
			printf("DKC_SINGLELIST : ");
			//for(int i=0;i<30;i++){
			for(;it != NULL;){
				printf((char *)it->mBuff);
				it = it->mNext;
			}
			dODS("");
		}
/*
		//シリアライズテスト
		{
			DKC_SERIALIZE *pse1 = dkcAllocSerialize(
				edkcSerializeBufferMode,"slist_serialize01.bin");
			DKC_SERIALIZE *pse2 = dkcAllocSerialize(
				edkcSerializeFileMode,"slist_serialize02.bin");

			dkcSingleListSerialize(begin,pse1);
			dkcSingleListSerialize(begin,pse2);

			dkcFreeSerialize(&pse1);//ここでファイルに保存
			dkcFreeSerialize(&pse2);//もともとfopenなのでfclose...

		}
		//デシリアライズテスト
		{
			//ファイルに保存したシリアライズデータを開く
			DKC_DESERIALIZE *pse = dkcAllocDeserialize(
				edkcSerializeBufferMode,"slist_serialize02.bin");

			//デシリアライズする。
			DKC_SINGLELIST *sp = dkcAllocSingleListDeserialize(pse);
			//先頭を保存しておかないとメモリリークしてしまう。
			DKC_SINGLELIST *begin_tmp = sp;
			//これはシリアライズ前のデータ
			DKC_SINGLELIST *tp = begin;

			//比較する...
			while(true){
				dkcmNOT_ASSERT(DKUTIL_FAILED(
					dkc_memcmp(sp->mBuff,sp->mSize,tp->mBuff,tp->mSize)
					));
				if(NULL==sp->mNext) break;
				sp = sp->mNext;
				tp = tp->mNext;
			}

			//デシリアライザを解放
			dkcFreeDeserialize(&pse);
			//シングルリスト領域を解放
			dkcFreeSingleList(&begin_tmp);
		}*/

		dkcFreeSingleList(&begin);
	}

}

///スタックとキューのサンプル
void Test_dkcStackAndQueue(){
	const size_t nums = 5;
	{
	
		DKC_STACK *p = dkcAllocStack(nums,sizeof(int));
		FOR(int i=0;i<100;i++){
			if(DKUTIL_FAILED(dkcStackPush(p,&i)))
				break;
		}
		printf("DKC_STACK(%d num): ",nums);
		FOR(int i=0;i<100;i++){
			int r;
			if(DKUTIL_FAILED(dkcStackTop(p,&r)))
				break;
			dkcStackPop(p);
			printf("%d ",r);
		}
		dODS("");
		dkcFreeStack(&p);
	}


	{
		DKC_QUEUE *p = dkcAllocQueue(nums,sizeof(int));
		FOR(int i=0;i<100;i++){
			if(DKUTIL_FAILED(dkcQueuePush(p,&i)))
				break;
		}
		FOR(int i=0;i<100;i++){
			int r;
			if(DKUTIL_FAILED(dkcQueueTop(p,&r)))
				break;
			dkcQueuePop(p);
		}
		FOR(int i=0;i<10;i++){
			dkcQueuePush(p,&i);
		}
		printf("DKC_QUEUE(%d num): ",nums);
		FOR(int i=0;i<10;i++){
			int r;
			if(DKUTIL_FAILED(dkcQueueTop(p,&r)))
				break;
			dkcQueuePop(p);
			printf("%d ",r);
		}
		dODS("");

		dkcQueueClear(p);

		//キューは絶対にnums個入るかをテスト
		FOR(int i=0;i<nums;i++){
			if(DKUTIL_FAILED(dkcQueuePush(p,&i)))
				break;
		}
		FOR(int i=0;i<nums - 3;i++){
			int r;
			if(DKUTIL_FAILED(dkcQueueTop(p,&r)))
				break;
			dkcQueuePop(p);
		}
		FOR(int i=0;i<nums;i++){
			if(DKUTIL_FAILED(dkcQueuePush(p,&i)))
				break;
		}
		printf("DKC_QUEUE(%d num): ",nums);
		FOR(int i=0;i<10;i++){
			int r;
			if(DKUTIL_FAILED(dkcQueueTop(p,&r)))
				break;
			dkcQueuePop(p);
			//23401
			printf("%d ",r);
		}
		dODS("");

		dkcFreeQueue(&p);
	}




}
///LZSSサンプル
void Test_dkcLZSS(){
	//LZSSの準備
	DKC_LZSS *p = dkcAllocLZSS();//LZSSオブジェクト？へのポインタ
	DKC_LZSS_HEADER header={0};//ヘッダー。
	const char *target="target.ico";
	BYTE *buffer;//バッファへのポインタ
	size_t readsize;//十差に読み込んだサイズを取得
	int result;//戻り値

	size_t size = dkcFileSize(target);

	BYTE *dest;//圧縮先
	BYTE *check;//チェック用バッファ

	if(0==size){
		printf("%sが存在しなかったみたい。",target);
		return;
		}
	
	buffer = (BYTE *)malloc(size);
	dest = (BYTE *)malloc(size);
	check = (BYTE *)malloc(size);
	
	if(NULL==buffer || NULL==dest || NULL==check){
		printf("%dbyteを確保できなかったみたい。",size);
		goto END;
		}
	if(DKUTIL_FAILED(dkcLoadBinary(buffer,size,target,&readsize))){
		printf("%sがオープンできなかったみたい。",target);
		goto END;
		}
	if(size != readsize){
		printf("なんかファイルサイズが違うみたい・・・。普通はありえないんだけどなぁ");
		goto END;
		}
	
	printf("TestLZSSCompress()");
	printf("圧縮前:%d byte ",size);
	//圧縮前のデータをコピー
	memcpy(check,buffer,size);

	//圧縮
	result = dkcLZSSEncode(p,&header,(BYTE *)dest,size,(const BYTE *)buffer,size,size,dkcdLZSS_SIGNATURE);

	if(DKUTIL_FAILED(result)){
		if(edk_NoValueToProcess==result){
			printf("このファイルは圧縮する価値がありません。");
		}else if(edk_BufferOverFlow==result){
			printf("バッファがあふれました。");
		}else{
			printf("なんかエラーです。");
		}
		goto END;
	}

	printf("圧縮後:%d byte ",header.mCompressedSize);
	//展開
	dkcLZSSDecode(p,&header,(BYTE *)buffer,size,(const BYTE *)dest,size,dkcdLZSS_SIGNATURE);

	//チェック
	dkcmNOT_ASSERT(DKUTIL_FAILED(
		dkc_memcmp(
		buffer,size,check,size
		)
	 )
	);

END:
	if_free(buffer);
	if_free(dest);
	if_free(check);
	dkcFreeLZSS(&p);

}



void print_mem(const char *name,const BYTE *p,size_t num){
	size_t i=0;
	printf("%s : ",name);
	for(;i<num;i++){
		printf("%02x",p[i]);
	}
	printf("\n");
}

void print_str(const char *name,const char *str){
	printf("%s = %s\n",name,str);
}

void Test_ArrayOneByOne(){
	const int num = 10;
	int i=0;
	int id[num];
	DKC_ARRAY_ONEBYONE *p = dkcAllocArrayOneByOneStatic(sizeof(int),num);
	
	{
		for(i=0;i<num;i++){
			id[i] = dkcArrayOneByOnePush(p,&i);
		}
		for(i=0;i<num;i++){
			int temp;
			dkcArrayOneByOneReference(p,id[i],(void *)&temp);
			printf("id[%d]==%d -> %d,\n",i,id[i],temp);
		}
		printf("\n");
	}

	{
		//開放
		for(i=0;i<num;i+=2){
			dkcArrayOneByOnePop(p,id[i]);
		}
		//挿入
		for(i=0;i<num/2;i++){
			id[i*2] = dkcArrayOneByOnePush(p,&i);
		}
		//次はもう入らないので-1を返すはず
		dkcmNOT_ASSERT(-1 != dkcArrayOneByOnePush(p,&i));
		for(i=0;i<num;i++){
			int temp;
			dkcArrayOneByOneReference(p,id[i],(void *)&temp);
			printf("id[%d]==%d -> %d,\n",i,id[i],temp);
		}
		printf("\n");
	}

	dkcFreeArrayOneByOne(&p);







}

void *LoadTarget(size_t *psize){
	static BYTE str[1024 * 6]="";
	static bool Flag = false;
	long size;
	FILE *fp;

	if(Flag){
		goto End;
	}

	fp = fopen("target.ico","rb");
	if(NULL==fp){
		return NULL;
	}
	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	fread(str,1,size,fp);
	fclose(fp);
	Flag = true;
End:
	*psize = (size_t)size;
	return str;
}
#if 0
#if 1



template < size_t Bits, ULONG TruncPoly,
           ULONG InitRem, ULONG FinalXor,
           bool ReflectIn, bool ReflectRem >
class Test_CRC_Helper{
	const BYTE *str;
	size_t size;
public:
	Test_CRC_Helper(){
	
		str = (const BYTE *)LoadTarget(&size);
		if(NULL==str){
			return;
		}
		test();

	}
	bool test(){
		using namespace dkutil;

		if(NULL==str){
			return false;
		}

		CCRC ccrc(Bits,TruncPoly,InitRem,FinalXor,ReflectIn,ReflectRem);
		//typedef boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, FALSE, FALSE> ct;
		typedef boost::crc_optimal<Bits,TruncPoly,InitRem,FinalXor, ReflectIn,ReflectRem> ct;
		ct bcrc;

		ccrc.load(str,size);
		bcrc.process_bytes(str,size);

		USHORT pttt[0x100];
		printf("table variable type size = %u\n",sizeof(ct::crc_table_type::value_type));
		memcpy(pttt,ct::crc_table_type::table_,sizeof(ct::crc_table_type::value_type) * 0x100);
		
		//dkcmNOT_ASSERT(typeid(hoge) != typeid(hoge2));


			
		dkcmNOT_ASSERT(
			DKUTIL_FAILED(
				dkc_memcmp(
					pttt,sizeof(ct::crc_table_type::value_type) * 0x100,
					ccrc.getTable(),dkcCRCGetUseTableSize(st.mBits)
				)
			)
		);
		ULONG cc;
		ccrc.getSignature()
		
			printf("boost mask_bit : %08lX \n",ct::masking_type::sig_bits_fast);
		
		const char *ss = "dkcCRC:%08lX boost::crc:%08lX";
		printf(ss,,crc());

		return true;
	}
};



void Test_CRC(){

#if 1
	typedef Test_CRC_Helper<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, FALSE, FALSE> crc32left;
	typedef Test_CRC_Helper<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, TRUE> crc32right;
	typedef Test_CRC_Helper<16, 0x8005, 0, 0, TRUE,TRUE> crc_ansi16left;
	typedef Test_CRC_Helper<16, 0xa001, 0, 0, FALSE,FALSE> crc_ansi16right;
	typedef Test_CRC_Helper<16, 0x1021, 0xFFFF, 0, FALSE,FALSE> crc_ccitt_left;
	typedef Test_CRC_Helper<16, 0x8005, 0xFFFF, 0, TRUE,TRUE> crc_ccitt_right;
	typedef Test_CRC_Helper<16, 0x51, 0xFFFF, 0, FALSE,FALSE> crc7left;
	typedef Test_CRC_Helper<16, 0x8a, 0xFFFF, 0, TRUE,TRUE> crc7right;
	typedef Test_CRC_Helper<16, 0x8408, 0, 0, TRUE,TRUE> crc_xmodem;

	crc32left c32left;
	crc32right c32right;
	crc_ansi16left c16left;
	crc_ansi16right c16right;
	crc_ccitt_left ccitt_left;
	crc_ccitt_right ccitt_right;
	crc7left c7left;
	crc7right c7right;
	crc_xmodem x;


#else
	size_t size;
	const BYTE *str = NULL;
	str = (const BYTE *)LoadTarget(&size);
	if(NULL==str){
		return;
	}
	USHORT table[0x100];
	DKC_CRC st;

	/* 構造体へのポインタ,CRCテーブルへのポインタ,生成多項式,初期値,CRC計算の最後にXORする値,最後に反対にするかどうかとか　どうかとか･･･。*/
	//dkcCRCInit(&st,table,32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, FALSE, FALSE);
	dkcCRCInit(&st,table,16, 0x8005, 0, 0, true, true);


	dkcCRCLoad(&st,str,size);
	
	//typedef boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, FALSE, FALSE> ct;
	typedef boost::crc_optimal<16, 0x8005, 0, 0, true, true> ct;
	ct crc;
	crc32_left c32l;
	crc.process_bytes(str,size);
	USHORT pttt[0x100];
	printf("table variable type size = %u\n",sizeof(ct::crc_table_type::value_type));
	memcpy(pttt,ct::crc_table_type::table_,sizeof(ct::crc_table_type::value_type) * 0x100);
	
	//dkcmNOT_ASSERT(typeid(hoge) != typeid(hoge2));


		
	dkcmNOT_ASSERT(
		DKUTIL_FAILED(
			dkc_memcmp(
				pttt,sizeof(ct::crc_table_type::value_type) * 0x100,
				st.mTable,dkcCRCGetUseTableSize(st.mBits)
			)
		)
	);

	
	const char *ss = "dkcCRC:%08lX boost::crc:%08lX crc32_left:%08lX\n";
	printf("%08lX",ct::masking_type::sig_bits_fast);
	printf(ss,
		dkcCRCFinal(&st),crc(),c32l.signature(str,size)
	);
	Sleep(500);
#endif

}
#else

void Test_CRC(){

	const char *ss = "dkcCRC:%08lX\n";
	size_t size;
	const BYTE *str = NULL;
	str = (const BYTE *)LoadTarget(&size);
	if(NULL==str){
		return;
	}
	USHORT table[0x100];
	DKC_CRC st;
	ULONG Result;
	/* 構造体へのポインタ,CRCテーブルへのポインタ,生成多項式,初期値,CRC計算の最後にXORする値,最後に反対にするかどうかとか　どうかとか･･･。*/
	//dkcCRCInit(&st,table,32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, FALSE, FALSE);
	dkcCRCInit(&st,table,16, 0x8005, 0, 0, true, true);


	dkcCRCLoad(&st,str,size);
	Result = dkcCRCFinal(&st);
	
	

	printf(ss,Result	);
	Sleep(500);

}


#endif
#endif


void Test_dkcHashDigest(){
	size_t size;
	const char *str = NULL;
	str = (const char *)LoadTarget(&size);
	if(NULL==str){
		return;
	}
	//char buff[640]="";
	///手順
	{
		/*
		///まず、メモリ領域をもらう
		DKC_SHA1 *p = dkcAllocSHA1();
		///strの内容をLoadする。
		dkcSHA1Load(p,(const BYTE *)str,size);
		///Finalize処理をしてDigestをbuffに納める
		dkcSHA1FinalDigestStr(p,buff,sizeof(buff));
		///SHA1のメモリ領域を開放
		dkcFreeSHA1(&p);
		///buffの中身を表示してみる。
		printf("sha1 = %s\n",buff);
		*/
		char bs[SHA384_STR_BUFFER_SIZE]="";
		{
			BYTE b1[SHA1_BIN_BUFFER_SIZE]="";

			DKC_SHA1 *p = dkcAllocSHA1();
			dkcSHA1Load(p,(const BYTE *)str,size);
			dkcSHA1FinalDigest(p,b1,sizeof(b1));
			dkcSHA1DigestStr(p,bs,sizeof(bs));
			dkcFreeSHA1(&p);
			print_mem("sha1",b1,sizeof(b1));
			print_str("sha1",bs);


			/*BYTE b2[SHA1_BIN_BUFFER_SIZE]="";
			sha1(b2,(const BYTE *)str,size);
			print_mem("sha1",b2,sizeof(b2));
			
			//SHA_VISUAL((DWORD *)b2);
			
			
			dkcmNOT_ASSERT(DKUTIL_FAILED(dkc_memcmp(b1,sizeof(b1),b2,sizeof(b2))));*/
		}
		{
			BYTE b1[SHA256_BIN_BUFFER_SIZE]="";

			DKC_SHA256 *p = dkcAllocSHA256();
			dkcSHA256Load(p,(const BYTE *)str,size);
			dkcSHA256FinalDigest(p,b1,sizeof(b1));
			dkcSHA256DigestStr(p,bs,sizeof(bs));
			dkcFreeSHA256(&p);
			print_mem("sha256",b1,sizeof(b1));
			print_str("sha256",bs);

			/*
			BYTE b2[SHA256_BIN_BUFFER_SIZE]="";
			sha256(b2,(const BYTE *)str,size);
			print_mem("sha256",b2,sizeof(b2));
			
			dkcmNOT_ASSERT(DKUTIL_FAILED(dkc_memcmp(b1,sizeof(b1),b2,sizeof(b2))));*/
		}
		{
			BYTE b1[SHA512_BIN_BUFFER_SIZE]="";

			DKC_SHA512 *p = dkcAllocSHA512();
			dkcSHA512Load(p,(const BYTE *)str,size);
			dkcSHA512FinalDigest(p,b1,sizeof(b1));
			dkcSHA512DigestStr(p,bs,sizeof(bs));
			dkcFreeSHA512(&p);
			print_mem("sha512",b1,sizeof(b1));
			print_str("sha512",bs);

			/*
			BYTE b2[SHA512_BIN_BUFFER_SIZE]="";
			sha512(b2,(const BYTE *)str,size);
			print_mem("sha512",b2,sizeof(b2));

			dkcmNOT_ASSERT(DKUTIL_FAILED(dkc_memcmp(b1,sizeof(b1),b2,sizeof(b2))));*/
		}
		{
			BYTE b1[SHA384_BIN_BUFFER_SIZE]="";


			DKC_SHA384 *p = dkcAllocSHA384();
			dkcSHA384Load(p,(const BYTE *)str,size);
			dkcSHA384FinalDigest(p,b1,sizeof(b1));
			
			print_mem("sha384",b1,sizeof(b1));
			dkcSHA384DigestStr(p,bs,sizeof(bs));
			print_str("sha384",bs);
			dkcFreeSHA384(&p);

			/*BYTE b2[SHA384_BIN_BUFFER_SIZE]="";
			sha384(b2,(const BYTE *)str,size);
			print_mem("sha384",b2,sizeof(b2));

			dkcmNOT_ASSERT(DKUTIL_FAILED(dkc_memcmp(b1,sizeof(b1),b2,sizeof(b2))));*/
		}



		


		//その他のHash系 ( SHA256,MD5等 ) も同じ感じです。
	}

	{
		uint64 ad = 0xffffffffffffffff,bd=0x00000001ffffffff;
		__asm{
			movq mm0,ad
			movq mm1,bd
			paddd mm0,mm1
			emms
		}
	}

	{
		BYTE data[2]={'a','\0'};
		BYTE data2[64];
		char digest[MD5_STR_BUFFER_SIZE * 4];
		int i;
		DKC_MD5 *p = dkcAllocMD5();
		DKC_MD5 *p2= dkcAllocMD5();

		for(i=0;i<64;i++){
			data2[i] = i;
		}
		dkcMD5LoadDouble(p,data2,sizeof(data2),
			p2,data2,sizeof(data2));

		dkcMD5FinalDigestStr(p,digest,sizeof(digest));
		printf("md5 data2 mmx    : %s\n",digest);
		//dkcMD5FinalDigestStr(p2,d.get(),d.size());
		dkcFreeMD5(&p);
		dkcFreeMD5(&p2);
		
		dkcSecureHashCalculateStringDigest(edkcSH_MD5,digest,sizeof(digest),
			data2,sizeof(data2));
		printf("md5 data2 result : %s\n",digest);

		dkcSecureHashCalculateStringDigest(edkcSH_MD5,digest,sizeof(digest),
			data,1);
		printf("md5 a result : %s\n",digest);
		
		dkcSecureHashCalculateStringDigest(edkcSH_MD4,digest,sizeof(digest),
			data,1);
		printf("md4 a result : %s\n",digest);

		dkcSecureHashCalculateStringDigest(edkcSH_MD2,digest,sizeof(digest),
			data,1);
		//32ec01ec4a6dac72c0ab96fb34c0b5d1
		printf("md2 a result : %s\n",digest);

	}



}

#if 0
void Test_dkcRijndael(){
	const BYTE key[]="keykeykey";
	const BYTE data[RIJNDAEL_BLOCK_SIZE]={
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
		1,2,3,4,5,6,7,8,
	};
	BYTE dest[RIJNDAEL_BLOCK_SIZE],dest2[RIJNDAEL_BLOCK_SIZE];
	int i;

	size_t len = strlen((const char *)key);
	printf("original data:"); 
	for(i=0;i<RIJNDAEL_BLOCK_SIZE;i++){
		printf("%d,",data[i]);
	}
	printf("\n");

	{
		DKC_RIJNDAEL *p = dkcAllocRijndaelEncrypt(key,len);
	
		dkcRijndaelEncrypt(p,dest,sizeof(dest),data,sizeof(data));
	
		printf("rijndael encrypt:"); 
		for(i=0;i<RIJNDAEL_BLOCK_SIZE;i++){
			printf("%d,",dest[i]);
		}
		printf("\n");

		dkcFreeRijndael(&p);
	}

	{
		DKC_RIJNDAEL *p = dkcAllocRijndaelDecrypt(key,len);
	
		dkcRijndaelDecrypt(p,dest2,sizeof(dest2),dest,sizeof(dest));
	
		printf("rijndael decrypt:"); 
		for(i=0;i<RIJNDAEL_BLOCK_SIZE;i++){
			printf("%d,",dest2[i]);
		}
		printf("\n");

		dkcFreeRijndael(&p);
	}
	for(i=0;i<RIJNDAEL_BLOCK_SIZE;i++){
		dkcmNOT_ASSERT(data[i] != dest2[i]);
	}
	Sleep(4000);


}

#endif

//**********************************************************
//ソート系サンプル

int less_comp(const void *v1,const void *v2){
	/*int *i1,*i2;
	i1 = (int *)v1;
	i2 = (int *)v2;
	*/
	//return (*i1 < *i2);
	//return (*i1 < *i2) ? 1 : (*i1) < (*i2) ?  1 : 0;

	//return ((*i1) < (*i2));
	//return (*i1 - *i2) ? 1 : 0;
	return *( ( int * ) v1 ) - *( ( int * ) v2 );
}
int select_comp(const void *v1,const void *v2){
	int *i1,*i2;
	i1 = (int *)v1;
	i2 = (int *)v2;
	//return *( ( int * ) v1 ) - *( ( int * ) v2 );
	return ((*i1) < (*i2));
	//return *(i2) - *(i1);
}

void init_rand_array(int *arr,size_t num){
	//dkcThreadLock_Lock();
	srand(1);
	for(size_t i=0;i<num;i++){
		arr[i] = rand();
	}
	//dkcThreadLock_Unlock();
}

void print_rand(int *arr,size_t num){
	for(size_t i=0;i<num;i++){
		printf("%d,",arr[i]);
	}
	printf("\n");
}


void comb_sort(size_t num,int *base){
	
	int *b = base;
  int gap = num;
  if ( gap < 1 ) {

     return;
  }
  size_t first2 = num;
  BOOL swapped = FALSE;
  do {
    int newgap = (gap*10+3)/13;
    if ( newgap < 1 ) newgap = 1;
    //std::advance(first2, newgap-gap);
    first2 += newgap - gap;
		gap = newgap;
    swapped = FALSE;
    for ( size_t i = 0, j = first2;
          j != num;
          ++i, ++j) {
      if ( b[j] < b[i] ) {
        //std::iter_swap(i, j);
				SWAP_NUM(b[j],b[i]);
        swapped = TRUE;
      }
    }
  } while ( (gap > 1) || swapped );
}

#if 0

struct gdc{
	size_t count;
	void *data;
};

void calc_count(void *p , int min){

	//a[i];
}

static void *get_offset(void *p,size_t offset){
	BYTE *oo = (BYTE *)p;
	
	return (void *)&(oo[offset]);
}

void WINAPI dkcDistCountSortGeneric(size_t num, const void *a, void *b,
																		size_t buffsize,size_t width
																		int Min_,int Max_)
{

	int i, x;

	void *count = (int *)dkcAllocate(width * (Max_));

	//for (i = 0; i <= Max_ - Min_; i++) count[i] = 0;
	
	/*for (i = 0; i < num; i++){
		count[a[i] - Min_]++;
		}*/
	for(i = 0;i < num;i++){
		size_t byte_offset = calc_offset(a[i],Min_);
		BYTE *p = count;
		p[byte_offset];
		

	for (i = 1; i <= Max_ - Min_; i++){
		count[i] += count[i - 1];
		}
	for (i = num - 1; i >= 0; i--) {
		x = a[i] - Min_; b[--count[x]] = a[i]; //debuged
		//x = a[i] - Min_;  b[--count[x]] = x;//origin
	}
	dkcFree((void **)&count);

}

#endif

typedef int keytype;

keytype select(keytype a[], size_t n, int k,DKC_COMPARE_TYPE comp)
{
	int i, j, left, right;
	keytype x, t;

	left = 0;  right = n - 1;
	while (left < right) {
		x = a[k];  i = left;  j = right;
		for ( ; ; ) {
			//while (a[i] < x) i++;
			while(comp((const void *)&a[i] ,(const void *)&x)) i++;
			//while (x < a[j]) j--;
			while(comp((const void *)&x,(const void *)&a[j])) j--;
			if (i > j) break;
			t = a[i];  a[i] = a[j];  a[j] = t;
			i++;  j--;
		}
		if (j < k) left  = i;
		if (k < i) right = j;
	}
	return a[k];
}

keytype select_min(keytype a[],size_t n,DKC_COMPARE_TYPE comp){
	return select(a,n,0,comp);
}

keytype select_max(keytype a[],size_t n,DKC_COMPARE_TYPE comp){
	return select(a,n,n-1,comp);
}

void Test_dkcSort()
{
	int arr[10];
	int out[10];

	//dkcThreadLockInit();


	init_rand_array(arr,10);
	qsort(arr,DKUTIL_ARRAY_NUMOF(arr),sizeof(arr[0]),less_comp);
	print_rand(arr,10);
	
	init_rand_array(arr,10);
	int min,max;
#	if 1
	int dmin,dmax;
	dmin = select_min(arr,10,select_comp);
	dmax = select_max(arr,10,select_comp);
	//dmin = min,dmax = max;
#endif
	//int pmax,pmin;
	dkcSelectMax(&max,arr,10,sizeof(int),select_comp);
	dkcSelectMin(&min,arr,10,sizeof(int),select_comp);

	//max = 
	//min = arr[os2];

	//distsort(10,arr,out,min,max);
	dkcDistCountSortInt(10,arr,out,min,max);
	print_rand(out,10);

	/*init_rand_array(arr,10);
	dkcShellSort(arr,DKUTIL_ARRAY_NUMOF(arr),sizeof(arr[0]),less_comp);
	print_rand(arr,10);

	init_rand_array(arr,10);
	dkcBubbleSort(arr,DKUTIL_ARRAY_NUMOF(arr),sizeof(arr[0]),less_comp);
	print_rand(arr,10);

	init_rand_array(arr,10);
	dkcCombSort(arr,DKUTIL_ARRAY_NUMOF(arr),sizeof(arr[0]),less_comp);
	print_rand(arr,10);
	*/
	//dkcThreadLockEnd();
	Sleep(1000);
}

//**********************************************************


void Test_dkcRandom(){
	int i,t;
	ULONG seed = 10;
	for(i=0;i<USHRT_MAX * 10;i++){
		t = dkcRandom(&seed,UCHAR_MAX);
		dkcmNOT_ASSERT(t < 0 || t > UCHAR_MAX);
		/*if(t == UCHAR_MAX){
			dODS("*");
		}
		*/
	}
}
/*
///ビットを文字列にしてビジュアル化する。
template<typename T>
inline bool BitIsVisualized( char *buff,size_t buffsize,T dt ,bool left=true)
{

	size_t len = sizeof(dt) * CHAR_BIT;;
	char *p=buff;
	if(len+1 > buffsize){//NULL文字分
		return false;
	}
 // int a=0;
	if(left==false){
		for (size_t i=len-1; i >= 0; i-- ){
				// ビットをぶち込む
				if(0 > _snprintf(p,buffsize,"%u",( dt>>i ) & 0x0001))
					return false;
				p++;
		}
	}else{
		for (size_t i=0; i < len; i++ ){
			// ビットをぶち込む
			if(0 > _snprintf(p,buffsize,"%u",( dt>>i ) & 0x0001))
				return false;
			p++;
		}
	}
	return true;
}
*/

/*
void bit_printf(UINT bit){
	char buff[32 + 1]="";
	BitIsVisualized(buff,sizeof(buff),bit);
	printf("%s\n",buff);
}
*/
void Test_dkcStream(){
	


}

void Test_dkcOpenFileWithSignature(){
	//ゴミデータ
	const char *dustbin="dust";
	//ゴミデータの長さ
	size_t dustbin_size = strlen(dustbin);
	//ファイル名
	const char *filename = "fws.bin";
	//シグネチャ
	UINT signature = 'FWSS';
	//書き込むデータ
	const char *arr="dummydata dummydata";
	//書き込むサイズ
	size_t write_size = strlen(arr);

	{
		DKC_FILE_WITH_SIGNATURE *p = dkcOpenFileWithSignature (
			edkcStreamLittleEndian,
			filename,"wb",
			dustbin,dustbin_size,
			signature
		);
	
		if(NULL==p){
			printf("ERROR\n ");
		}

		
		dkcFileWithSignatureWrite(p,arr,write_size);
		
		dkcCloseFileWithSignature(&p);
	}

	{
		char buff[512];
		size_t readsize = 0;

		DKC_FILE_WITH_SIGNATURE *p = dkcOpenFileWithSignature(
			edkcStreamLittleEndian,
			filename,"rb",
			dustbin,dustbin_size,
			signature
		);

		
		dkcFileWithSignatureRead(p,buff,sizeof(buff),
			&readsize);

		//読み込んだサイズをチェック
		dkcmNOT_ASSERT(readsize != write_size);

		//チートされているかどうかチェック。
		dkcmNOT_ASSERT(DKUTIL_FAILED(dkcFileWithSignatureCheckCheat(p)));

		dkcCloseFileWithSignature(&p);

	}



}

void print_array(const char *p,size_t size){
	size_t i=0;
	for(;i<size;i++){
		if(isalpha(p[i])){
			printf("%c",p[i]);
		}else{
			printf("%d,",p[i]);
		}
	}
	printf("\n");
}

typedef BYTE Uchar;
#define MAX_LEN 256
int rl_encode( Uchar *out, const Uchar *in_, int size, int n )
{	Uchar *in = (Uchar *)in_;
  Uchar *limit = in + size, *wp = out;
  do {
    int c = *in++;
    int k = 1;
    for( ; in < limit && k < n + MAX_LEN; in++ ){
      if( *in != c ) break;
      k++;
    }
    if( k >= n ){
      /* ランレングス */
      int m;
      for( m = 0; m < n; m++ ) *wp++ = c;
      k -= n;
      if( k <= 127 ){
        *wp++ = k;
      } else {
        k -= 128;
        *wp++ = (k >> 8) + 0x80;
        *wp++ = k & 0xff;
      }
    } else {
      while( k-- > 0 ) *wp++ = c;
    }
  } while( in < limit );
  return wp - out;
}

/* 復号 */
int rl_decode( Uchar *out, const Uchar *in_, int size, int n )
{
	Uchar *in = (Uchar *)in_;
  Uchar *limit = in + size, *wp = out;
  do {
    int c = *in++;
    int k = 1;
    for( ; in < limit && k < n; in++ ){
      if( *in != c ) break;
      k++;
    }
    if( k == n ){
      /* c を展開 */
      int m = *in++;
      if( m & 0x80 ){
        m &= 0x7f;
        k += (m << 8) + 128 + *in++;
      } else {
        k += m;
      }
    }
    while( k-- > 0 ) *wp++ = c;
  } while( in < limit );
  return wp - out;
}


LARGE_INTEGER cycles;

inline void cpu_clock_begin(){
	__asm {
		cpuid
		rdtsc

		mov cycles.LowPart,  eax 
		mov cycles.HighPart, edx
	}
}

inline ULONGLONG cpu_clock_get()
{
	__asm {
		cpuid
		rdtsc

		sub eax, cycles.LowPart  
		sbb edx, cycles.HighPart

		mov cycles.LowPart,  eax 
		mov cycles.HighPart, edx
	}
	return cycles.QuadPart;
}


void Test_RLE(){
	//const BYTE data[]={0,0,0,0,0,0,0,1,2,3,4,0,0,0,1};
	const char data[]="aaaaaaabcdeaaab";
	BYTE tdata[100],ddata[100];
	DKC_RLE_PACKBITS_HEADER h;
	int r;

	printf("origin:");
	print_array((const char *)data,sizeof(data));
	cpu_clock_begin();

	dkcRLEPackBitsEncode(&h,tdata,sizeof(tdata),(const BYTE *)data,sizeof(data),3);
	printf("time = %d\n",cpu_clock_get());

	print_array((const char *)tdata,h.mCompressedSize);
	dkcRLEPackBitsDecode(&h,ddata,sizeof(ddata),tdata,h.mCompressedSize);
	print_array((const char *)ddata,h.mOriginSize);
	
	cpu_clock_begin();
	r = rl_encode(tdata,(BYTE *)data,sizeof(data),2);
	printf("time = %d\n",cpu_clock_get());

	print_array((const char *)tdata,r);

	memcpy(tdata,data,sizeof(data));
	dkcRotateShiftRightMemory(tdata,sizeof(data),1);
	print_array((const char *)tdata,sizeof(data));


	/*
	DKC_RLE *p = dkcAllocRLE();
	DKC_MEMORYSTREAM *pms = dkcAllocMemoryStream(100);
	DKC_RLE_HEADER h;
	const BYTE data[]={0,0,0,0,0,0,0,1,2,3,4,0,0,0,1};
	char tdata[100];
	size_t len = sizeof(data);

	dkcRLEEncode(p,pms,&h,(BYTE *)&data,len,len,
		dkcdRLE_SIGNATURE,dkcdRLE_EOF_ID,dkcdRLE_NOCOMPRESS_ID
	);

	print_array(dkcMemoryStreamPointer(pms),h.mCompressedSize);

	size_t os = dkcMemoryStreamNowOffset(pms);
	memcpy(tdata,dkcMemoryStreamPointer(pms),os);
	dkcRLEDecode(p,pms,&h,
		(BYTE *)&tdata,os,
		dkcdRLE_SIGNATURE
	);

	print_array(dkcMemoryStreamPointer(pms),dkcMemoryStreamNowOffset(pms));


	dkcFreeMemoryStream(&pms);
	dkcFreeRLE(&p);
	*/

}
#define BUF_SIZE 0x256

typedef unsigned char   Uchar;

/* バッファ定義 */
Uchar buffer[BUF_SIZE * 2];
Uchar output[BUF_SIZE];
Uchar *table[BUF_SIZE];

/* 単純挿入ソート */
void insert_sort( Uchar *buff, int size, int len )
{
  int i, j;
  /* table の初期化 */
  for( i = 0; i < size; i++ ) table[i] = buff + i;
  /* ソート */
  for( i = 1; i < size; i++ ){
    /* char を Uchar に訂正 2002/10/28 */
    Uchar *tmp = table[i];
    for( j = i - 1; j >= 0 && memcmp( tmp, table[j], len ) < 0; j-- ){
      table[j + 1] = table[j];
    }
    table[j + 1] = tmp;
  }
}

/* 符号化 */
int blocksort_encode( Uchar *in, Uchar *out, int size )
{
  int i, top = -1;
  memcpy( in + size, in, size );  /* コピー */
  insert_sort( in, size, size );  /* ソート */
  /* 出力 */
  for( i = 0; i < size; i++ ){
    /* char を Uchar に訂正 2002/10/28 */
    Uchar *ptr = table[i];
    if( ptr == buffer )
			top = i;
    out[i] = *(ptr + size - 1);
  }
  return top;
}

/* 復号 */
void blocksort_decode( Uchar *out,Uchar *buff, int top, int size )
{
  int i;
  Uchar *ptr;  /* char を Uchar に訂正 2002/10/28 */
  insert_sort( buff, size, 1 );    /* ソート */
  /* 出力 */
  ptr = table[top]; 
  for( i = 0; i < size; i++ ){
		*out++ = *ptr;
    //fputc( *ptr, stdout );
    ptr = table[ptr - buff];
  }
}

void Test_BlockSort(){
	const char data[]="aeadacab";

	DKC_BLOCKSORT_INFO info;

	size_t len = strlen(data);
	memcpy(buffer,data,len);
	print_array(data,len);
	
	int r = blocksort_encode(buffer,output,(int)len);
	print_array((const char *)output,len);
	blocksort_decode((BYTE *)buffer,(BYTE *)output,r,(int)len);
	print_array((const char *)buffer,len);

	BYTE mybuffer[256];
	memcpy(mybuffer,data,len);


	
	dkcmNOT_ASSERT(DKUTIL_FAILED(dkcBlockSortEncode(mybuffer,len,&info)));
	//print_array((const char *)mybuffer,len);

	dkcmNOT_ASSERT(DKUTIL_FAILED(dkcBlockSortDecode(mybuffer,len,&info)));
	//print_array((const char *)mybuffer,len);


}

typedef struct dkc_Histogram{
	size_t count[0xFFFF];
	size_t limit;
}DKC_HISTOGRAM;

void dkcHistogramInit(DKC_HISTOGRAM *p,size_t limit){
	memset(p,0,sizeof(DKC_HISTOGRAM));
	if(limit > 0xFFFF)
		limit = 0xFFFF;
	p->limit = limit;
}

///@todo overflowでDebugBreak()とか
#define dkcHistogramInc(obj,i) ((obj)->count[(i)]++)
//#define dkcHistogramDec(obj,i)

void dkcHistogramPrintf(DKC_HISTOGRAM *p){
	size_t i=0,s = p->limit,*cp = &(p->count[0]);
	//DKC_HISTOGRAM t;
	//dkcHistogramInit(&t,p->limit);
	
	for(;i<s;i++){
		
	//	dkcHistogramInc(&t,cp[i]);
		
	//	printf("%d,",i);
		printf("i = %d / count = %d\n",i,cp[i]);
		
	}

}

#define TEST_HASH_CSV "hash_hist.csv"
void dkcHistogramOutputCSV(DKC_HISTOGRAM *p){
	size_t i=0,s = p->limit,*cp = &(p->count[0]);
	//DKC_HISTOGRAM t;
	//dkcHistogramInit(&t,p->limit);
	FILE *fp = fopen(TEST_HASH_CSV,"wt");
	dkcmASSERT(fp);
	if(NULL==fp){
		return;
	}
	for(;i<s;i++){
		fprintf(fp,"%d,%d\n",i,cp[i]);
	}
	fclose(fp);

}

void dkcHistogramCountLimitCheck(DKC_HISTOGRAM *p,size_t count){
	size_t i=0,s = p->limit,*cp = &(p->count[0]);


	for(;i<s;i++){
		if(count < cp[i])
			printf("%d / %d length error\n",i,cp[i]); 
	}


}
	//SHA256で使われている初期価値
	/*const static uint32 H[8] = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19, 
	};*/
/*
int dkcHash16_(unsigned char *pd,size_t cycle){
  unsigned int v = 0;
  size_t i;
	BYTE cc = 
  
	v = pd[0] | (pd[1] << 8) | (pd[2]<< 16) | (pd[3] << 24);
  
  return (int)((v % 31991) & 0x7FFF);
}


int dkcHash16_(unsigned char *pd,size_t cycle){
  unsigned int v = 0;
  size_t i;
  
  //SHA1で使われている初期価値
  const static unsigned long H[3]={0x67452301,0xefcdab89,0x98badcfe};

  for(i=0;i<cycle;i++){
    v = (v << 6) + pd[i] ^ H[i & 2];
  }
  return (int)((v % 31991) & 0x7FFF);
}
*/
static DKC_INLINE DWORD Ch(DWORD x, DWORD y, DWORD z) {return (x & (y ^ z)) ^ z;}
//Ch(v,pd[i],H[j & 2])


DKC_EXTERN uint8 dkcHashOutput8_16(const void *);
DKC_EXTERN uint16 dkcHashOutput16_32(const void *);
DKC_EXTERN uint32 dkcHashOutput32_64(const void *);
DKC_EXTERN uint64 dkcHashOutput64_128(const void *);


uint8 dkcHashOutput8_16(const void *a){
	unsigned int v;
  uint16 *p = (uint16 *)a;
	v = (*p) % dkcdUNSIGNED_8BIT_PRIME_NUM;

	return v & 0xff;
}

uint16 dkcHashOutput16_32(const void *a){
	unsigned int v;
  uint32 *p = (uint32 *)a;
	v = (*p) % dkcdUNSIGNED_16BIT_PRIME_NUM;
	return v & 0xffff;
}

#define TEST_HASH_LOOP 0xFFFF

void Test_Hash2(){

	
	DKC_HISTOGRAM obj;
	uint64 v,t;
	printf("凄く長い時間行うけどガマンしてね\n");
	FILE *fp = fopen(TEST_HASH_CSV,"wt");
	dkcmASSERT(fp);

	FOR(UINT i=0;i<TEST_HASH_LOOP;i++){
		UINT a = i;
		v = dkcHashOutput16_32((void *)(&i));
		//v = dkcHashOutput8_16(&a);
		if(t==v)
			fprintf(fp,"前のと同じ %04x / before %d / now %d\n",v,i-1,i);
		else
			fprintf(fp,"%d / %04X\n",i,v );

		t = v;
	}
	fclose(fp);
	
}
#undef TEST_HASH_LOOP

int dkcHash16_(unsigned char *pd,size_t cycle){
	unsigned int v = 0;
	size_t i,j;
	size_t shift_ = 8;
	//SHA1で使われている初期価値
	const static DWORD H[3]={0x67452301,0xefcdab89,0x98badcfe};

	for(i=0;i<cycle;i++){
		v =  ((v << shift_) + pd[i]) % 31991;
	}
	v = (v & 0x7FFF);
	return (int)v;
	//return (int)((v % 31991) & 0x7FFF);
}
uint8 dkcHash(uint8 *pd,size_t cycle){
	uint32 v = 0;
	size_t i;
	for(i=0;i<cycle;i++){
		v = (pd[i] << 3) + v;
	}
	return (uint8)((v % dkcdUNSIGNED_8BIT_PRIME_NUM) & 0xFF);
}

#define TEST_HASH dkcHash16_

#define TEST_HASH_VAL_MAX 31991
#define TEST_HASH_LOOP 0xFFFFFF
#define TEST_HASH_BYTE (TEST_HASH_LOOP >> \
	(( dkcNumOfBits( TEST_HASH_LOOP ) - dkcNumOfBits( TEST_HASH_LOOP ) / 8)))



void Test_Hash(){

	//UINT *data = (UINT *)malloc(sizeof(UINT) * 0xFFFFFF);
	DKC_HISTOGRAM obj;
	uint16 v,t;
	printf("凄く長い時間行うけどガマンしてね\n");

	dkcHistogramInit(&obj,TEST_HASH_VAL_MAX);
	boost::progress_display show_progress( TEST_HASH_LOOP );
	/*FOR( unsigned int i = 0 ; i < 0x100; i++ ){
		printf("%2X hash:%d\n", i, dkcHash16_((unsigned char*)&i, 1) );
	}*/
	FOR(UINT i=0;i<TEST_HASH_LOOP;i++){
		//data[i] = i;
		//v = TEST_HASH((uint8 *)(&data[i]),3);
		v = TEST_HASH((uint8 *)(&i),TEST_HASH_BYTE);
		if(t==v)
			printf("前のと同じ / %d / %d",i-1,i);

		//printf("%2X hash:%d\n", i, dkcHash16_((unsigned char*)&i, 1) );
		dkcHistogramInc(&obj,v);
		++show_progress;
		t = v;
	}
	dkcHistogramOutputCSV(&obj);
	dkcHistogramCountLimitCheck(&obj,TEST_HASH_LOOP / TEST_HASH_BYTE);
	//free(data);
}


void Test_Regex(){
	const DKC_UNICODE *pat=L"[0-9]";
	const DKC_UNICODE *str=L"12";
	ULONG a,b;
	BOOL r = dkcRegularExpressionSearch(pat,TRUE,FALSE,str,dkcstd_wcslen(str),0,&a,&b);
	printf("dkcRegularExpressionSearch() = %d\n",r);
	r = dkcRegularExpressionMatch(pat,str,TRUE);
	printf("dkcRegularExpressionMatch() = %d\n",r);
	//dkcRegularExpressionMatch(

}

void Test_LZW(){
	//in C
	{
		//ranking_timer timer;
		char *filename="target.ico";
		DKC_LZW_HEADER h;
		FILE *fp;
		BYTE dest[1024 * 8];
		BYTE src[1024 * 8];
		BYTE dest2[1024 * 8];
		BYTE temp[1024 * 8];
		size_t filesize;

		int tr;


		NULL_CHAR_ARRAY(dest);
		NULL_CHAR_ARRAY(src);
		NULL_CHAR_ARRAY(dest2);
		NULL_CHAR_ARRAY(temp);

		DKC_LZW *p = dkcAllocLZW(1024 * 8);
		if(NULL==p) return;

		fp = dkcFOpen(filename,"rb");
		if(NULL==fp) goto FREELZW;
		
		filesize = dkcFileSize(filename);
		 
		if(filesize != dkcFReadAll(src,filesize,fp))
			goto FREEFILE;

		{
			int ttt = dkcdLZW_NUM;
			
		}
#if 1
		{


			{

				dkcmFORCE_NOT_ASSERT(DKUTIL_FAILED(
					dkcLZWEncode(p,&h,dest,sizeof(dest),src,filesize,1024 * 8,'LZW',edkcLZW_TST)
				));
			}
#if 1
			memcpy(temp,dest,h.mCompressedSize);
			h.option = edkcLZW_HASH;
			dkcLZWDecode(p,&h,dest2,sizeof(dest2),dest,h.mCompressedSize,'LZW');
			//dkcLZWDecode(p,&h,dest2,sizeof(dest2),dest,h.mCompressedSize,'LZW');
			dkcmFORCE_NOT_ASSERT(0 != memcmp(src,dest2,filesize));
#endif	
			{
				NULL_CHAR_ARRAY(dest);
				
				dkcmFORCE_NOT_ASSERT(DKUTIL_FAILED(
					dkcLZWEncode(p,&h,dest,sizeof(dest),src,filesize,1024 * 8,'LZW',edkcLZW_HASH)
				));
			}
			dkcLZWDump(p,"lzw_compressed.bin");
			//dkcLZWToFile(p,"lzw_compressed.bin");
			dkcSaveBinary(dest,h.mCompressedSize,"lzwed0.bin");
			dkcSaveBinary(temp,h.mCompressedSize,"lzwed1.bin");
			//dkcmFORCE_NOT_ASSERT(0 != memcmp(temp,dest,sizeof(temp)));
			dkcmFORCE_NOT_ASSERT(0 != memcmp(temp,dest,h.mCompressedSize));
			//h.option = edkcLZW_TST;
			NULL_CHAR_ARRAY(dest2);
			dkcLZWDecode(p,&h,dest2,sizeof(dest2),dest,h.mCompressedSize,'LZW');
			
			dkcmFORCE_NOT_ASSERT(0 != memcmp(src,dest2,filesize));
			
		}
#endif

		
FREEFILE:
		dkcFClose(&fp);
FREELZW:
		dkcFreeLZW(&p);
	}

}

///NULLチェックなどのエラーハンドリングは各自で＾＾；
void Test_RedBlackTree(){
	int a, maxnum = 3, ct;
	DKC_RED_BLACK_ROOT *p = dkcAllocRedBlackTreeRoot(128,maxnum,free);
	DKC_RB_TREE_NODE *t;

	

	for (ct = maxnum; ct; ct--)
	{
		rb_tree_data_type data;
		a = rand() % 9 + 1;
		t = dkcRedBlackTree_findNode(p,a);
		if (t) {
			dkcRedBlackTree_deleteNode(
				p,t,&data
			);
			free(data);
		} else {
			//データ領域を用意してやる
		 data = malloc(sizeof(a));
			//*((int*)data) = a;
			dkcRedBlackTree_insertNode(
				p,a,data
			);

			//使用後のdkcBufferUninit()は呼ばなくて良い。すべてRedBlackTree側が開放してくれる。
		}
	}

	
	
	dkcFreeRedBlackTreeRoot(&p);


}

int main(int argc,char *argv[]){
	dkcCheckMemoryLeak(TRUE);
	dkcIsMMX();
	Test_RedBlackTree();

	Test_Regex();
	Test_LZW();
	//Test_BlockSort();
	//Test_Hash2();
	//Test_Hash();
#if 1
	Test_RLE();
	Test_BMStrStr();

	//Test_CRC();

	Test_ArrayOneByOne();
	
	//Test_dkcRijndael();

	//searchtest();
	//loadtest();

	//return EXIT_SUCCESS;

	dODS("Test_dkcSort()");
	Test_dkcSort();
	dODS("");

	/*
	{

		UINT flag = 0;
		DKUTIL_FLAG_UP(flag,edkcStreamInitMemory);
		DKUTIL_FLAG_DOWN(flag,edkcStreamInitMemory);
		DKUTIL_FLAG_DOWN(flag,edkcStreamInitMemory);
		bit_printf(edkcStreamInitMemory);
		bit_printf(~edkcStreamInitMemory);
		
		bit_printf(flag);
		DKUTIL_FLAG_UP(flag,edkcStreamInitFile);
		//flag |= edkcStreamInitFile;
		//DKUTIL_FLAG_DOWN(flag,edkcStreamInitFile);
		//DKUTIL_FLAG_DOWN(flag,edkcStreamInitFile);
		bit_printf(flag);
	}
	*/
	dODS("Test_dkcOpenFileWithSignature()");
	Test_dkcOpenFileWithSignature();
	dODS("");

	dODS("Test_dkcRandom()");
	Test_dkcRandom();
	dODS("");


	dODS("Test_dkcSJIS_StrSearch()");
	Test_dkcSJIS_StrSearch();
	dODS("");

	dODS("Test_dkcPathString()");
	Test_dkcPathString();
	dODS("");

	dODS("Test_dkcStackAndQueue()");
	Test_dkcStackAndQueue();
	dODS("");

	dODS("Test_dkcString()");
	Test_dkcString();
	dODS("");

	dODS("Test_dkcCircularMemoryStream()");
	Test_dkcCircularMemoryStream();
	dODS("");

	dODS("Test_dkcSingleList()");
	Test_dkcSingleList();
	dODS("");

	dODS("Test_dkcLZSS()");
	Test_dkcLZSS();
	dODS("");

	dODS("Test_dkcHashDigest()");
	Test_dkcHashDigest();
	dODS("");

#endif
	
	system("PAUSE");
	return 0;
}
