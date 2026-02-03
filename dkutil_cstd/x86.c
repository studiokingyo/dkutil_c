/**
@author d金魚
@file x86.c
@brief x86系？のCPUの判別や専用処理　等
@since 2005/01/23
@note
一部のソースに
<ul>
<li>「memory copy code benchmark」のソースが使われています。
以下の文章が原文です。
<PRE>
// memory copy code benchmark
// source code for Visual C++ 6.0 + Service Pack 4 + Processor Pack
// copyright(C) 2001 XELF. All rights reserved.
// http://www.cyborg.ne.jp/~xelf/

// You can use this source code for any purpose without permission.
// Notes that this source code is not supported the processing of fraction bytes.
</PRE>

<li>「DirectX9用汎用ライブラリ Luna」のLuna.cppのソースコードが使われています。
以下が利用条件の原文です。
<PRE>
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
【タイトル】 DirectX9用汎用ライブラリ Luna
【ファイル】 Luna.zip
【作成月日】 2003/07/01 - 2004/01/01
【カテゴリ】 ライブラリ
【制 作 者】 葉迩倭
【動作環境】 DirectX9のSDKを用いたアプリケーションがコンパイルできる環境
【配布形態】 フリーウェア
【 E-Mail 】 haniwa@twin-tail.jp
【HomePage】 http://www.twin-tail.jp/
【転　　載】 自由
【著 作 権】 いらんので、好き勝手使ってください。
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

●説明●
　タイトルのまんまです。
　著作権とかはどうでもいいんで好き勝手使ってください。
　一応使用条件として"諸注意"参照。

...略


●諸注意●
　このプログラムを用いることによって何らかの被害、損失が貴方にあったとしても、
　当方は一切責任を負いませんのでご了承ください。
</PRE>

</ul>
*/
#include "x86.h"
#include "memory.h"


#if 1!=DKUTIL_CSTD_MSVC_IASM
//#	error "unsupport msvc inline assembler"
#else

BOOL WINAPI dkcstd_getX86CPUInfo(DKCSTD_CPU_INFO *dest,int CPUClockCalcTime)
{

	char CPUName[256]	= "";
	char CPUType[128]	= "";
	long bFPU			= FALSE;
	long bTSC			= FALSE;
	long bCMOV			= FALSE;
	long bFCMOV			= FALSE;
	long bCPUID			= FALSE;
	long bMMX			= FALSE;
	long bMMX2			= FALSE;
	long bSSE			= FALSE;
	long bSSE2			= FALSE;
	long b3DNOW			= FALSE;
	long bE3DNOW		= FALSE;
	long TypeID			= 0;
	long FamilyID		= 0;
	long ModelID		= 0;
	long SteppingID		= 0;
	long CPUClock		= 0;

	__asm
	{
		//-----------------------------------------------------------
		// CPUID命令の存在チェック
		//-----------------------------------------------------------
		PUSHFD
		POP		EAX
		MOV		EBX,		EAX
		XOR		EAX,		1<<21
		PUSH	EAX
		POPFD
		PUSHFD
		POP		EAX
		CMP		EAX,		EBX
		JE		EXIT				// ない
		MOV		bCPUID,		1

		//-----------------------------------------------------------
		// CPUID 0
		//-----------------------------------------------------------
		MOV		EAX,		0
		CPUID

		CMP		EAX,		0
		JE		EXIT				// 0では話にならん

		MOV DWORD PTR [CPUType+0],	EBX
		MOV DWORD PTR [CPUType+8],	ECX
		MOV DWORD PTR [CPUType+4],	EDX

		//-----------------------------------------------------------
		// CPUID 1
		//-----------------------------------------------------------
		MOV		EAX,		1
		CPUID

		//----------------------------------------------
		// EAXレジスタの中身検証
		//----------------------------------------------
		// ステッピングID
		MOV		ESI,		EAX
		AND		ESI,		0x0F;		// 下位4バイトにステッピングID
		MOV		[SteppingID],ESI

		// モデルID
		SHR		EAX,		4			// 右ステッピングID分に詰めて･･･
		MOV		ESI,		EAX
		AND		ESI,		0x0F		// さらに4バイトに
		MOV		[ModelID],	ESI

		// ファミリーID
		SHR		EAX,		4			// また詰めて･･･
		MOV		ESI,		EAX
		AND		ESI,		0x0F		// おなじみ4バイト
		MOV		[FamilyID],	ESI

		// タイプID
		SHR		EAX,		4			// もうええがな
		MOV		ESI,		EAX
		AND		ESI,		0x03		// 今度は2バイト
		MOV		[TypeID],	ESI

		//----------------------------------------------
		// EDXレジスタの中身検証
		//----------------------------------------------
		// FPU（なかったらヤバイと思うけどね＾＾；
		XOR		EAX,		EAX			// ゼロクリア
		TEST	EDX,		1<<0		// 1バイト目チェック
		SETNZ	AL						// 0でないならALレジスタへ転送
		MOV		[bFPU],		EAX			// 結果やいかに

		// TSC
		TEST	EDX,		1<<4
		SETNZ	AL
		MOV		[bTSC],		EAX

		// 条件付転送命令および比較
		XOR		EAX,		EAX
		TEST	EDX,		1<<15
		SETNZ	AL
		MOV		[bCMOV],	EAX

		// MMX命令
		XOR		EAX,		EAX
		TEST	EDX,		1<<23
		SETNZ	AL
		MOV		[bMMX],		EAX

		// MMX2 & SSE 命令
		XOR		EAX,		EAX
		TEST	EDX,		1<<25
		SETNZ	AL
		MOV		[bMMX2],	EAX
		MOV		[bSSE],		EAX

//////////////////////////////////
//		以下はAMDのCPU情報		//
//////////////////////////////////

		//-----------------------------------------------------------
		// CPUID 0x80000000
		//-----------------------------------------------------------
		MOV		EAX,		0x80000000
		CPUID

		CMP		EAX,		0x80000001	// 次のCPUID命令のサポートチェック
		JB		EXIT

		//-----------------------------------------------------------
		// CPUID 0x80000001
		//-----------------------------------------------------------
		MOV		EAX,		0x80000001
		CPUID

		// MMX2
		XOR		EAX,		EAX
		TEST	EDX,		1<<22
		SETNZ	AL
		MOV		[bMMX2],	EAX

		// Enhansed 3DNow!
		XOR		EAX,		EAX
		TEST	EDX,		1<<30
		SETNZ	AL
		MOV		[bE3DNOW],	EAX

		// Enhansed 3DNow!
		XOR		EAX,		EAX
		TEST	EDX,		1<<31
		SETNZ	AL
		MOV		[b3DNOW],	EAX

		//-----------------------------------------------------------
		// CPUID 0x80000002 - 0x80000004
		//-----------------------------------------------------------
		// サポートしてるか？
		MOV		EAX,		0x80000000
		CPUID
		CMP		EAX,		0x80000004
		JB		EXIT

		// 0x80000002
		MOV EAX, 0x80000002
		CPUID

		MOV DWORD PTR [CPUName+ 0],		EAX
		MOV DWORD PTR [CPUName+ 4],		EBX
		MOV DWORD PTR [CPUName+ 8],		ECX
		MOV DWORD PTR [CPUName+12],		EDX

		// 0x80000003
		MOV EAX, 0x80000003
		CPUID

		MOV DWORD PTR [CPUName+16],		EAX
		MOV DWORD PTR [CPUName+20],		EBX
		MOV DWORD PTR [CPUName+24],		ECX
		MOV DWORD PTR [CPUName+28],		EDX

		// 0x80000004
		MOV EAX, 0x80000004
		CPUID

		MOV DWORD PTR [CPUName+32],		EAX
		MOV DWORD PTR [CPUName+36],		EBX
		MOV DWORD PTR [CPUName+40],		ECX
		MOV DWORD PTR [CPUName+44],		EDX
		
	// 終了
	EXIT:
	}

	

	//------------------------------------------------------------------------
	// CPUクロック取得
	//------------------------------------------------------------------------
	if ( bTSC && 0 != CPUClockCalcTime)
	{

		/*__asm
		{
			RDTSC
			MOV		[CPUClock],		EAX
		}

		Sleep( 1000 );

		__asm
		{
			RDTSC
			SUB		EAX,			[CPUClock]
			MOV		[CPUClock],		EAX
		}

		CPUClock /= 1000000;
		*/

		__asm
		{
			RDTSC
			MOV		[CPUClock],		EAX
		}

		Sleep( CPUClockCalcTime );

		__asm
		{
			RDTSC
			SUB		EAX,			[CPUClock]
			MOV		[CPUClock],		EAX
		}

		CPUClock /= (CPUClockCalcTime * 1000);
	}

	//--------------------------------------------------
	// チェック完了～
	//--------------------------------------------------


	{//埋め込み
		size_t cns = sizeof(dest->CPUName);
		size_t cts = sizeof(dest->CPUType);
	
		dkcstd_memcpy(dest->CPUName,CPUName,cns);
		dest->CPUName[cns - 1] = '\0';

		dkcstd_memcpy(dest->CPUType,CPUType,cts);
		dest->CPUType[cts - 1] = '\0';
		
		dest->bFPU		= (unsigned char)bFPU;
		dest->bTSC		= (unsigned char)bTSC;
		dest->bCMOV		= (unsigned char)bCMOV;
		dest->bCPUID	= (unsigned char)bCPUID;
		dest->bMMX		= (unsigned char)bMMX;
		dest->bMMX2		= (unsigned char)bMMX2;
		dest->bSSE		= (unsigned char)bSSE;
		dest->bSSE2		= (unsigned char)bSSE2;
		dest->b3DNOW	= (unsigned char)b3DNOW;
		dest->bE3DNOW	= (unsigned char)bE3DNOW;
		dest->TypeID	= TypeID;
		dest->FamilyID= FamilyID;
		dest->ModelID	= ModelID;
		dest->SteppingID	= SteppingID;
		dest->CPUClock		= CPUClock;
	}
	return TRUE;
}

int WINAPI dkcstd_IsCPUID(){
	int flag = 0;

	__asm{
		pushfd
		pop eax
		mov ebx,eax
		xor eax,00200000h
		push eax
		popfd
		pushfd
		pop eax
		cmp eax,ebx //CPUIDの有無を比較
		je end 
		mov flag,1
end:
	}
	return flag;
}

int WINAPI dkcstd_getCPUID(){
	int flags=0;
	__asm {
		xor eax,eax;
		cpuid;
		or eax,eax;
		jz quit;
		mov eax,1;
		cpuid;
		mov flags,edx;
	quit:
	}
	return flags;
}

uint32 WINAPI dkcstd_getX86CPUType() {
	int flags;
	uint32 type;

	if(FALSE==dkcstd_IsCPUID()){
		return 0;
	}
	flags=dkcstd_getCPUID();


	//AMD系
	if (flags&(1<<31))
	{
		type|=edkcCPU_3DNOW;
		if (flags&(1<<30))
		{
			type|=edkcCPU_E3DNOW;
		}
	}
	//Intel系
	if (flags&(1<<23))
	{
		type|=edkcCPU_MMX;
		if (flags&(1<<25))
		{
			type|=edkcCPU_SSE;
			//多分使えるらしい･･･
			type|=edkcCPU_MMX2;
			if (flags&(1<<26)) 
			{
				type|=edkcCPU_SSE2;
			}
		}
	}
	return type;
}

uint16 WINAPI dkcstd_getFPUMode(){
	uint16 wSave;
	__asm fstcw wSave

	return wSave;
}

int WINAPI dkcstd_setFPUMode(uint16 data){
	if(data > 0x300){
		return edk_FAILED;
	}
	__asm fldcw data
	return edk_SUCCEEDED;
}


void WINAPI dkcstd_getRDTSC(DKC_UINT64_STRUCT *p)
{
	*p = dkcstd_getRDTSC_Fast();
}
#endif