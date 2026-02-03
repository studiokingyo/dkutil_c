/*!
@file dkcSafeFileSystem.h
@brief 改竄防止機能付き ファイルシステム
@author d金魚
@note
<h4>ウンチク</h4?
http://studiokingyo.fc2web.com/dxlib/kowaza/k4.html#k19

<h4>セーブ</h4>
-ファイルに保存するすべてのデータをハッシュ関数に食わせる。
-ハッシュ値をいただく。
-ファイルに保存するすべてのデータを暗号化する。
-ハッシュ値を保存
-暗号化したデータを保存

<h4>ロード</h4>
-ファイルに保存した暗号化したデータをメモリ上にロード
-メモリ上にロードしたデータを複合化する
-複合化したデータをハッシュ関数に食わせる
-ハッシュ値をいただく。
-ファイルに保存してあるハッシュ値とさっき取得したハッシュ値をくらべる。
（ここでハッシュ値が違う場合は、セーブデータが改竄されていると判断できる。）

<h4>ファイル構造</h4>
- ヘッダ部（場合によっては暗号化されていたりしていなかったり･･･）
	- ユーザー独自のシグネチャ
	- データ部のサイズ
	- ハッシュ値
- データ部（場合によっては暗号化していたりしていなかったり･･･)
	-データ
*/
#ifndef DKUTIL_C_SAFE_FILESYSTEM_H
#define DKUTIL_C_SAFE_FILESYSTEM_H

#include "dkcCryptograph.h"
#include "dkcSHA1.h"
#include "dkcSJISFileSystem.h"
#include "dkcBuffer.h"
#include "dkcStream.h"

typedef struct dkc_FileHeader_FileWithSignature{
	///エンディアンフラグ little endianだったらTRUE
	BYTE little_endian;
	///このファイル自体のシグネチャ
	UINT sig;
	///ファイルのサイズ
	ULONGLONG filesize;
	//データ部のサイズ指定は何故か大きい
	//ULONGLONG datasize;
	///ハッシュ値の格納庫
	BYTE hash_value[SHA1_BIN_BUFFER_SIZE]; 

}DKC_FILE_HEADER_FILE_WITH_SIGNATURE;

typedef struct dkc_FileWithSignature{
	DKC_STREAM *mStream;
	DKC_SHA1 *mSHA1;
	DKC_BUFFER *mDustbin;
	///書き込みモードならTRUE
	BYTE mWriteMode;
	///すべてロードしたならTRUE
	BYTE mAllLoad;
	///ファイルヘッダ
	DKC_FILE_HEADER_FILE_WITH_SIGNATURE mFileHeader;
	
}DKC_FILE_WITH_SIGNATURE;


/*!
@param stream_flag[in] dkcAllocStream()に使われるflag edkcStreamInitMemoryは使えません。
 //@param stream_size[in] dkcAllocStream()に使われるsize
@param filename[in] ファイル名
@param mode[in] ファイルオープンモード (aを指定するとエラーとして処理を返します。rかwを指定してください。）
@param dustbin[in] ゴミデータ
@param dustbin_size[in] ゴミデータのサイズ
@param signature[in] 自分が識別するファイルのシグネチャ。このread open時 最初のシグネチャが違うとエラーとなる。
 //@param CheckCheat[out] リード Open時にシグネチャをチェックしない時はNULL、
チェックするときはint型変数へのポインタを入れる。
受け取ったポインタ先がedk_SUCCEEDED 以外だったらチートされていた。
- edk_FileSignatureException:ファイルの内容自体を変化させられた。
- edk_FileCheated_Addition:ファイルの内容に問題は無いが、冗長なデータが引っ付いている。

@return DKC_FILE_WITH_SIGNATUREへのポインタ
*/
DKC_EXTERN DKC_FILE_WITH_SIGNATURE *WINAPI dkcOpenFileWithSignature(
	UINT stream_flag,//size_t stream_size,
	const char *filename,const char *mode,
	const void *dustbin,size_t dustbin_size,
	UINT signature
);


DKC_EXTERN int WINAPI dkcFileWithSignatureWrite(DKC_FILE_WITH_SIGNATURE *p,
																								const void *data,size_t size);

DKC_EXTERN int WINAPI dkcFileWithSignatureWrite16(DKC_FILE_WITH_SIGNATURE *p,
																								const void *data,size_t size);


DKC_EXTERN int WINAPI dkcFileWithSignatureWrite32(DKC_FILE_WITH_SIGNATURE *p,
																								const void *data,size_t size);

DKC_EXTERN int WINAPI dkcFileWithSignatureWrite64(DKC_FILE_WITH_SIGNATURE *p,
																								const void *data,size_t size);

/*!
@return すべて読み終わると	edk_EndProcessを返します。それ以外は指定した初期化フラグに依存します。
@note
エンディアンの考慮はありません。
*/
DKC_EXTERN int WINAPI dkcFileWithSignatureRead(DKC_FILE_WITH_SIGNATURE *p,
																								void *data,size_t size,size_t *readsize);

/*!
@return edk_SUCCEEDED以外はチートされている。edk_LogicErrorなら呼び出し時が間違っている。
詳しくはdkcOpenFileWithSignature()にて。
@note
dkcFileWithSignatureRead()でedk_EndProcessが返ってきたらこれを呼び出してチートされているかどうか確かめてください。
*/
DKC_EXTERN int WINAPI dkcFileWithSignatureCheckCheat(DKC_FILE_WITH_SIGNATURE *p);

DKC_EXTERN int WINAPI dkcFileWithSignatureCheckCheatFile(const char *filename);

///SHAの最終処理をする
///@note 別にこれを呼び出さなくてもdkcCloseFIleWithSignature()が行ってくれる
DKC_EXTERN void WINAPI dkcFileWithSignatureFinal(DKC_FILE_WITH_SIGNATURE *p);
																												 

DKC_EXTERN int WINAPI dkcCloseFileWithSignature(DKC_FILE_WITH_SIGNATURE **);


//**********************************************************
//暗号化機能付き ファイルセーブ
//**********************************************************

typedef struct dkc_Arcfour_File{
	///ハッシュ付きファイルセーブオブジェクト
	DKC_FILE_WITH_SIGNATURE *mFile;
	///Arcfour暗号化オブジェクト
	DKC_ARCFOUR_STATE *maf;
}DKC_ARCFOUR_FILE;

/*!
@see 引数のほとんどはdkcOpenFileWithSignature()と同じ。
@param arcfour_key[in] arcfourに使うキー
@param arcfour_size[in] arcfour_keyのサイズ
@return DKC_ARCFOUR_FILE への領域へのポインタ
*/
DKC_EXTERN DKC_ARCFOUR_FILE *dkcOpenArcfourFile(
	UINT stream_flag,size_t stream_size,
	const char *filename,const char *mode,
	const void *dustbin,size_t dustbin_size,
	const void *arcfour_key,size_t arcfour_size,
	UINT signature,BOOL *IsCheated
);

DKC_EXTERN int dkcCloseArcfourFile(DKC_ARCFOUR_FILE **);


DKC_EXTERN int WINAPI dkcArcfourFileWrite(DKC_ARCFOUR_FILE *p,
																								const void *data,size_t size);

DKC_EXTERN int WINAPI dkcArcfourFileWrite16(DKC_ARCFOUR_FILE *p,
																								const void *data,size_t size);


DKC_EXTERN int WINAPI dkcArcfourFileWrite32(DKC_ARCFOUR_FILE *p,
																								const void *data,size_t size);

DKC_EXTERN int WINAPI dkcArcfourFileWrite64(DKC_ARCFOUR_FILE *p,
																								const void *data,size_t size);


DKC_EXTERN int WINAPI dkcArcfourFileRead(DKC_ARCFOUR_FILE *p,
																								void *data,size_t size);




#endif //end of include once