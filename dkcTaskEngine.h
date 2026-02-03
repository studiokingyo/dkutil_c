/*!
@file dkcTaskEngine.h
@brief		いわいるタスク処理
@author		d金魚
@since		2004/08/22
@note
<h3>タスク処理に関する持論</h3>


タスクマネージャー（タスクの生成、削除を管理するもの）（このソースではDKC_TASK_ENGINE構造体）
タスク（一つの処理）（このソースではDKC_TASK構造体）

タスクマネージャーがタスクを操作する形。

ここで、今までのタスク処理に関して多少不満があった。
- タスク処理は沢山タスクを一つのタスクマネージャーに登録するとデバッグしにくい。(例１）
- タスク処理は連結リストを使うのでイテレーションが遅いほうだ。
- 連結リストなのでシリアライズ、デシリアライズの実装が非常に面倒。

と言う事で、これらの解決案としては
- 一つのタスクマネージャーはタスクを管理できるのは255-256個にする。 \
また、一連のプロセスの流れに対してタスクマネージャーを一個割り当てるようにする。 \
これらのタスクマネージャーを断層構造にして処理をする方が分かりやすいと考える。
- 連結リストではなく配列でアクセスを行う。
- 連結リストではなくなったのでシリアライズ、デシリアライズの実装が容易。

また、タスク処理はメモリ使用量を少なく済ませる事が美徳と考える私は
メモリ使用量をかなり少なめにする事にした。
メモリ使用量を少なくするには空き領域情報とかを無くせば良いと思うが、
その情報が無くなるといちいち線形探索である。
ここらへん、どーすれば良いかが良くわからない･･･。
ARRAY_ONEBYONEとか？

　妄想メモ

- 通称、順序無しリスト（std::vectorのようなもの？）を使おうか？。\
http://www.aya.or.jp/%7Esanami/peace/memorial/code41-50.html#CODE44</s>
- タスクワークのプールを外部にする事で、コピーのオーバーヘッドを抑える事が出来るのでは？


と、言う事で、考えてみた結果･･･。

	決定事項

- 関数へのポインタが何もしない関数（ Return() )の時のタスク領域は使用可能

以上。このような思想の元にタスクエンジンをくみ上げる事にした。

例１：
例えばシューティングで弾を沢山出したい時、
弾ひとつをタスクとすると沢山のタスクがタスクマネージャーに登録される。
これにより、主要処理をしているタスクの状態遷移が見にくくなり、デバッグしにくくなる。

なので、この場合はタスクの中にもう一つ、弾用タスクエンジンを使って、
または専用の弾の「生成」「消滅」マネージャーを作ってしまうのが良いと思われる。

タスク処理については以下のページが詳しい
http://homepage3.nifty.com/moha/
http://www.hh.iij4u.or.jp/~peto/Games/games_top.html
*/
#ifndef DKUTIL_C_TASK_ENGINE_H
#define DKUTIL_C_TASK_ENGINE_H


#include <stdio.h>
#include <string.h>
#include "dkcOSIndependent.h"

/*


*/
///タスクの数
#define dkcdTASK_NUM 256
/*!
事実上ULONGLONとかの64bit整数は無理なので指定しないで下さい。
多分USHORTあたりまでがいいところだと思う。
また、unsigned な変数型にしてください。
*/
///カウント変数のタイプ
typedef BYTE TASK_COUNT_T;


///タスクマネージャー
typedef struct dkc_TaskEngine{
	///タスクプール
	BYTE *mTaskPool[dkcdTASK_NUM];
	///タスクワークプール
	BYTE *mTaskWorkPool[dkcdTASK_NUM];
	///タスクワークのサイズ
	size_t mTaskWorkSize;
	///今、イテレーションしているタスクのID
	TASK_COUNT_T mNowPriority;
}DKC_TASK_ENGINE;

///タスク本体
typedef struct dkc_Task{
	///タスクワークへのポインタ
	BYTE *work;
	///処理関数へのポインタ
	void (*proc)(DKC_TASK_ENGINE *,struct dkc_Task *);
}DKC_TASK;


typedef void (*DKC_TASK_PROC)(DKC_TASK_ENGINE *,DKC_TASK *);






DKC_EXTERN DKC_TASK_ENGINE *dkcAllocTaskEngine(size_t taskworksize);

DKC_EXTERN int dkcFreeTaskEngine(DKC_TASK_ENGINE **);
/*!
@param TaskPool[in] タスクdkcdTASK_NUM個分のメモリ空間へのアドレス
@param TaskWorkPool[in] タスクワークdkcdTASK_NUM個分のメモリ空間へのアドレス
@note
内部でmallocを使わない版
なぜこんなのを用意したかと言うと、タスク処理は組み込み系でも使われそうと思うので･･･？
かなり速度等にシビアなものだと考えているからだ･･･。
*/
///自分で確保したメモリ領域を割り当てて初期化する場合。
DKC_EXTERN void dkcTaskEngineInit(DKC_TASK_ENGINE *p,DKC_TASK *TaskPool[dkcdTASK_NUM],BYTE *TaskWorkPool[dkcdTASK_NUM]);

///タスクを設定します。使用済みのプライオリティーの場合等で設定できなかった場合はedk_FAILEDを返します。
DKC_EXTERN int dkcTaskEngineSet(DKC_TASK_ENGINE *p,TASK_COUNT_T prio,DKC_TASK_PROC proc);
///タスクを削除します。
DKC_EXTERN int dkcTaskEngineKill(DKC_TASK_ENGINE *p,TASK_COUNT_T prio);
///タスクエンジンに登録されているタスクをイテレーションします。
DKC_EXTERN int dkcTaskEngineIteration(DKC_TASK_ENGINE *p);

///prioのタスクは空いているかどうか　TRUEなら空いている
DKC_EXTERN BOOL dkcTaskEngineEmpty(DKC_TASK_ENGINE *p,TASK_COUNT_T prio);


#if !defined(   DKUTIL_C_TASK_ENGINE_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcTaskEngine.c"
#endif



#endif //end of include once