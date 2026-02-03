# Microsoft Developer Studio Generated NMAKE File, Based on dkutil_cDLL.dsp
!IF "$(CFG)" == ""
CFG=dkutil_cDLL - Win32 DebugMT
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの dkutil_cDLL - Win32 DebugMT を設定します。
!ENDIF 

!IF "$(CFG)" != "dkutil_cDLL - Win32 Release" && "$(CFG)" != "dkutil_cDLL - Win32 Debug" && "$(CFG)" != "dkutil_cDLL - Win32 ReleaseMT" && "$(CFG)" != "dkutil_cDLL - Win32 DebugMT"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "dkutil_cDLL.mak" CFG="dkutil_cDLL - Win32 DebugMT"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "dkutil_cDLL - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "dkutil_cDLL - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "dkutil_cDLL - Win32 ReleaseMT" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "dkutil_cDLL - Win32 DebugMT" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"

OUTDIR=.\ReleaseDLL
INTDIR=.\ReleaseDLL
# Begin Custom Macros
OutDir=.\ReleaseDLL
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dkutil_cDLL.dll"

!ELSE 

ALL : "dkutil_c - Win32 Release" "$(OUTDIR)\dkutil_cDLL.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dkutil_c - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dkc2Tree.obj"
	-@erase "$(INTDIR)\dkcArcfour.obj"
	-@erase "$(INTDIR)\dkcArrayOneByOne.obj"
	-@erase "$(INTDIR)\dkcBit.obj"
	-@erase "$(INTDIR)\dkcBlockSort.obj"
	-@erase "$(INTDIR)\dkcBlowfish.obj"
	-@erase "$(INTDIR)\dkcBuffer.obj"
	-@erase "$(INTDIR)\dkcCircularMemoryStream.obj"
	-@erase "$(INTDIR)\dkcCircularStream.obj"
	-@erase "$(INTDIR)\dkcCRC.obj"
	-@erase "$(INTDIR)\dkcCryptograph.obj"
	-@erase "$(INTDIR)\dkcDCF.obj"
	-@erase "$(INTDIR)\dkcDCF01.obj"
	-@erase "$(INTDIR)\dkcDeque.obj"
	-@erase "$(INTDIR)\dkcDLL.obj"
	-@erase "$(INTDIR)\dkcGenericFileSystem.obj"
	-@erase "$(INTDIR)\dkcHash.obj"
	-@erase "$(INTDIR)\dkcHC256.obj"
	-@erase "$(INTDIR)\dkcHMAC.obj"
	-@erase "$(INTDIR)\dkcLua.obj"
	-@erase "$(INTDIR)\dkcLZSS.obj"
	-@erase "$(INTDIR)\dkcLZW.obj"
	-@erase "$(INTDIR)\dkcMath.obj"
	-@erase "$(INTDIR)\dkcMD2.obj"
	-@erase "$(INTDIR)\dkcMD4.obj"
	-@erase "$(INTDIR)\dkcMD5.obj"
	-@erase "$(INTDIR)\dkcMemoryPool.obj"
	-@erase "$(INTDIR)\dkcMemoryStream.obj"
	-@erase "$(INTDIR)\dkcOS.obj"
	-@erase "$(INTDIR)\dkcOSIndependent.obj"
	-@erase "$(INTDIR)\dkcQueue.obj"
	-@erase "$(INTDIR)\dkcRedBlackTree.obj"
	-@erase "$(INTDIR)\dkcRegex.obj"
	-@erase "$(INTDIR)\dkcRijndael.obj"
	-@erase "$(INTDIR)\dkcRLE.obj"
	-@erase "$(INTDIR)\dkcSafeFileSystem.obj"
	-@erase "$(INTDIR)\dkcSemiRealRandom.obj"
	-@erase "$(INTDIR)\dkcSHA.obj"
	-@erase "$(INTDIR)\dkcSHA1.obj"
	-@erase "$(INTDIR)\dkcSHA256.obj"
	-@erase "$(INTDIR)\dkcSHA384.obj"
	-@erase "$(INTDIR)\dkcSHA512.obj"
	-@erase "$(INTDIR)\dkcSingleList.obj"
	-@erase "$(INTDIR)\dkcSJISFileSystem.obj"
	-@erase "$(INTDIR)\dkcSNOW20.obj"
	-@erase "$(INTDIR)\dkcSort.obj"
	-@erase "$(INTDIR)\dkcStack.obj"
	-@erase "$(INTDIR)\dkcStdio.obj"
	-@erase "$(INTDIR)\dkcStream.obj"
	-@erase "$(INTDIR)\dkcString.obj"
	-@erase "$(INTDIR)\dkcTaskEngine.obj"
	-@erase "$(INTDIR)\dkcTelecomConfirmFile.obj"
	-@erase "$(INTDIR)\dkcTelecomConfirmPacket.obj"
	-@erase "$(INTDIR)\dkcThread.obj"
	-@erase "$(INTDIR)\dkcThreadLock.obj"
	-@erase "$(INTDIR)\dkcUniqueID.obj"
	-@erase "$(INTDIR)\dkcUnityArchivers.obj"
	-@erase "$(INTDIR)\dkcVernam.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\rijndael-alg-fst.obj"
	-@erase "$(INTDIR)\rijndael-api-fst.obj"
	-@erase "$(INTDIR)\st.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dkutil_cDLL.dll"
	-@erase "$(OUTDIR)\dkutil_cDLL.exp"
	-@erase "$(OUTDIR)\dkutil_cDLL.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USE_DKC_DLL" /D "DKUTIL_CDLL_EXPORTS" /Fp"$(INTDIR)\dkutil_cDLL.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_cDLL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\dkutil_cDLL.pdb" /machine:I386 /out:"$(OUTDIR)\dkutil_cDLL.dll" /implib:"$(OUTDIR)\dkutil_cDLL.lib" 
LINK32_OBJS= \
	"$(INTDIR)\dkcDCF.obj" \
	"$(INTDIR)\dkcDCF01.obj" \
	"$(INTDIR)\dkcTelecomConfirmFile.obj" \
	"$(INTDIR)\dkcTelecomConfirmPacket.obj" \
	"$(INTDIR)\dkc2Tree.obj" \
	"$(INTDIR)\dkcArrayOneByOne.obj" \
	"$(INTDIR)\dkcBuffer.obj" \
	"$(INTDIR)\dkcCircularMemoryStream.obj" \
	"$(INTDIR)\dkcCircularStream.obj" \
	"$(INTDIR)\dkcDeque.obj" \
	"$(INTDIR)\dkcMemoryStream.obj" \
	"$(INTDIR)\dkcQueue.obj" \
	"$(INTDIR)\dkcRedBlackTree.obj" \
	"$(INTDIR)\dkcSingleList.obj" \
	"$(INTDIR)\dkcStack.obj" \
	"$(INTDIR)\dkcString.obj" \
	"$(INTDIR)\dkcRijndael.obj" \
	"$(INTDIR)\rijndael-alg-fst.obj" \
	"$(INTDIR)\rijndael-api-fst.obj" \
	"$(INTDIR)\dkcArcfour.obj" \
	"$(INTDIR)\dkcBlowfish.obj" \
	"$(INTDIR)\dkcCryptograph.obj" \
	"$(INTDIR)\dkcHC256.obj" \
	"$(INTDIR)\dkcHMAC.obj" \
	"$(INTDIR)\dkcSemiRealRandom.obj" \
	"$(INTDIR)\dkcSNOW20.obj" \
	"$(INTDIR)\dkcVernam.obj" \
	"$(INTDIR)\dkcUnityArchivers.obj" \
	"$(INTDIR)\dkcBlockSort.obj" \
	"$(INTDIR)\dkcLZSS.obj" \
	"$(INTDIR)\dkcLZW.obj" \
	"$(INTDIR)\dkcRLE.obj" \
	"$(INTDIR)\dkcCRC.obj" \
	"$(INTDIR)\dkcHash.obj" \
	"$(INTDIR)\dkcMD2.obj" \
	"$(INTDIR)\dkcMD4.obj" \
	"$(INTDIR)\dkcMD5.obj" \
	"$(INTDIR)\dkcSHA.obj" \
	"$(INTDIR)\dkcSHA1.obj" \
	"$(INTDIR)\dkcSHA256.obj" \
	"$(INTDIR)\dkcSHA384.obj" \
	"$(INTDIR)\dkcSHA512.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\st.obj" \
	"$(INTDIR)\dkcGenericFileSystem.obj" \
	"$(INTDIR)\dkcSafeFileSystem.obj" \
	"$(INTDIR)\dkcSJISFileSystem.obj" \
	"$(INTDIR)\dkcStream.obj" \
	"$(INTDIR)\dkcBit.obj" \
	"$(INTDIR)\dkcDLL.obj" \
	"$(INTDIR)\dkcLua.obj" \
	"$(INTDIR)\dkcMath.obj" \
	"$(INTDIR)\dkcMemoryPool.obj" \
	"$(INTDIR)\dkcOS.obj" \
	"$(INTDIR)\dkcOSIndependent.obj" \
	"$(INTDIR)\dkcRegex.obj" \
	"$(INTDIR)\dkcSort.obj" \
	"$(INTDIR)\dkcStdio.obj" \
	"$(INTDIR)\dkcTaskEngine.obj" \
	"$(INTDIR)\dkcThread.obj" \
	"$(INTDIR)\dkcThreadLock.obj" \
	"$(INTDIR)\dkcUniqueID.obj" \
	".\Release\dkutil_c.lib"

"$(OUTDIR)\dkutil_cDLL.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"

OUTDIR=.\DebugDLL
INTDIR=.\DebugDLL
# Begin Custom Macros
OutDir=.\DebugDLL
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dkutil_cDLLd.dll" "$(OUTDIR)\dkutil_cDLL.bsc"

!ELSE 

ALL : "dkutil_c - Win32 Debug" "$(OUTDIR)\dkutil_cDLLd.dll" "$(OUTDIR)\dkutil_cDLL.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dkutil_c - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dkc2Tree.obj"
	-@erase "$(INTDIR)\dkc2Tree.sbr"
	-@erase "$(INTDIR)\dkcArcfour.obj"
	-@erase "$(INTDIR)\dkcArcfour.sbr"
	-@erase "$(INTDIR)\dkcArrayOneByOne.obj"
	-@erase "$(INTDIR)\dkcArrayOneByOne.sbr"
	-@erase "$(INTDIR)\dkcBit.obj"
	-@erase "$(INTDIR)\dkcBit.sbr"
	-@erase "$(INTDIR)\dkcBlockSort.obj"
	-@erase "$(INTDIR)\dkcBlockSort.sbr"
	-@erase "$(INTDIR)\dkcBlowfish.obj"
	-@erase "$(INTDIR)\dkcBlowfish.sbr"
	-@erase "$(INTDIR)\dkcBuffer.obj"
	-@erase "$(INTDIR)\dkcBuffer.sbr"
	-@erase "$(INTDIR)\dkcCircularMemoryStream.obj"
	-@erase "$(INTDIR)\dkcCircularMemoryStream.sbr"
	-@erase "$(INTDIR)\dkcCircularStream.obj"
	-@erase "$(INTDIR)\dkcCircularStream.sbr"
	-@erase "$(INTDIR)\dkcCRC.obj"
	-@erase "$(INTDIR)\dkcCRC.sbr"
	-@erase "$(INTDIR)\dkcCryptograph.obj"
	-@erase "$(INTDIR)\dkcCryptograph.sbr"
	-@erase "$(INTDIR)\dkcDCF.obj"
	-@erase "$(INTDIR)\dkcDCF.sbr"
	-@erase "$(INTDIR)\dkcDCF01.obj"
	-@erase "$(INTDIR)\dkcDCF01.sbr"
	-@erase "$(INTDIR)\dkcDeque.obj"
	-@erase "$(INTDIR)\dkcDeque.sbr"
	-@erase "$(INTDIR)\dkcDLL.obj"
	-@erase "$(INTDIR)\dkcDLL.sbr"
	-@erase "$(INTDIR)\dkcGenericFileSystem.obj"
	-@erase "$(INTDIR)\dkcGenericFileSystem.sbr"
	-@erase "$(INTDIR)\dkcHash.obj"
	-@erase "$(INTDIR)\dkcHash.sbr"
	-@erase "$(INTDIR)\dkcHC256.obj"
	-@erase "$(INTDIR)\dkcHC256.sbr"
	-@erase "$(INTDIR)\dkcHMAC.obj"
	-@erase "$(INTDIR)\dkcHMAC.sbr"
	-@erase "$(INTDIR)\dkcLua.obj"
	-@erase "$(INTDIR)\dkcLua.sbr"
	-@erase "$(INTDIR)\dkcLZSS.obj"
	-@erase "$(INTDIR)\dkcLZSS.sbr"
	-@erase "$(INTDIR)\dkcLZW.obj"
	-@erase "$(INTDIR)\dkcLZW.sbr"
	-@erase "$(INTDIR)\dkcMath.obj"
	-@erase "$(INTDIR)\dkcMath.sbr"
	-@erase "$(INTDIR)\dkcMD2.obj"
	-@erase "$(INTDIR)\dkcMD2.sbr"
	-@erase "$(INTDIR)\dkcMD4.obj"
	-@erase "$(INTDIR)\dkcMD4.sbr"
	-@erase "$(INTDIR)\dkcMD5.obj"
	-@erase "$(INTDIR)\dkcMD5.sbr"
	-@erase "$(INTDIR)\dkcMemoryPool.obj"
	-@erase "$(INTDIR)\dkcMemoryPool.sbr"
	-@erase "$(INTDIR)\dkcMemoryStream.obj"
	-@erase "$(INTDIR)\dkcMemoryStream.sbr"
	-@erase "$(INTDIR)\dkcOS.obj"
	-@erase "$(INTDIR)\dkcOS.sbr"
	-@erase "$(INTDIR)\dkcOSIndependent.obj"
	-@erase "$(INTDIR)\dkcOSIndependent.sbr"
	-@erase "$(INTDIR)\dkcQueue.obj"
	-@erase "$(INTDIR)\dkcQueue.sbr"
	-@erase "$(INTDIR)\dkcRedBlackTree.obj"
	-@erase "$(INTDIR)\dkcRedBlackTree.sbr"
	-@erase "$(INTDIR)\dkcRegex.obj"
	-@erase "$(INTDIR)\dkcRegex.sbr"
	-@erase "$(INTDIR)\dkcRijndael.obj"
	-@erase "$(INTDIR)\dkcRijndael.sbr"
	-@erase "$(INTDIR)\dkcRLE.obj"
	-@erase "$(INTDIR)\dkcRLE.sbr"
	-@erase "$(INTDIR)\dkcSafeFileSystem.obj"
	-@erase "$(INTDIR)\dkcSafeFileSystem.sbr"
	-@erase "$(INTDIR)\dkcSemiRealRandom.obj"
	-@erase "$(INTDIR)\dkcSemiRealRandom.sbr"
	-@erase "$(INTDIR)\dkcSHA.obj"
	-@erase "$(INTDIR)\dkcSHA.sbr"
	-@erase "$(INTDIR)\dkcSHA1.obj"
	-@erase "$(INTDIR)\dkcSHA1.sbr"
	-@erase "$(INTDIR)\dkcSHA256.obj"
	-@erase "$(INTDIR)\dkcSHA256.sbr"
	-@erase "$(INTDIR)\dkcSHA384.obj"
	-@erase "$(INTDIR)\dkcSHA384.sbr"
	-@erase "$(INTDIR)\dkcSHA512.obj"
	-@erase "$(INTDIR)\dkcSHA512.sbr"
	-@erase "$(INTDIR)\dkcSingleList.obj"
	-@erase "$(INTDIR)\dkcSingleList.sbr"
	-@erase "$(INTDIR)\dkcSJISFileSystem.obj"
	-@erase "$(INTDIR)\dkcSJISFileSystem.sbr"
	-@erase "$(INTDIR)\dkcSNOW20.obj"
	-@erase "$(INTDIR)\dkcSNOW20.sbr"
	-@erase "$(INTDIR)\dkcSort.obj"
	-@erase "$(INTDIR)\dkcSort.sbr"
	-@erase "$(INTDIR)\dkcStack.obj"
	-@erase "$(INTDIR)\dkcStack.sbr"
	-@erase "$(INTDIR)\dkcStdio.obj"
	-@erase "$(INTDIR)\dkcStdio.sbr"
	-@erase "$(INTDIR)\dkcStream.obj"
	-@erase "$(INTDIR)\dkcStream.sbr"
	-@erase "$(INTDIR)\dkcString.obj"
	-@erase "$(INTDIR)\dkcString.sbr"
	-@erase "$(INTDIR)\dkcTaskEngine.obj"
	-@erase "$(INTDIR)\dkcTaskEngine.sbr"
	-@erase "$(INTDIR)\dkcTelecomConfirmFile.obj"
	-@erase "$(INTDIR)\dkcTelecomConfirmFile.sbr"
	-@erase "$(INTDIR)\dkcTelecomConfirmPacket.obj"
	-@erase "$(INTDIR)\dkcTelecomConfirmPacket.sbr"
	-@erase "$(INTDIR)\dkcThread.obj"
	-@erase "$(INTDIR)\dkcThread.sbr"
	-@erase "$(INTDIR)\dkcThreadLock.obj"
	-@erase "$(INTDIR)\dkcThreadLock.sbr"
	-@erase "$(INTDIR)\dkcUniqueID.obj"
	-@erase "$(INTDIR)\dkcUniqueID.sbr"
	-@erase "$(INTDIR)\dkcUnityArchivers.obj"
	-@erase "$(INTDIR)\dkcUnityArchivers.sbr"
	-@erase "$(INTDIR)\dkcVernam.obj"
	-@erase "$(INTDIR)\dkcVernam.sbr"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\md5.sbr"
	-@erase "$(INTDIR)\rijndael-alg-fst.obj"
	-@erase "$(INTDIR)\rijndael-alg-fst.sbr"
	-@erase "$(INTDIR)\rijndael-api-fst.obj"
	-@erase "$(INTDIR)\rijndael-api-fst.sbr"
	-@erase "$(INTDIR)\st.obj"
	-@erase "$(INTDIR)\st.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dkutil_cDLL.bsc"
	-@erase "$(OUTDIR)\dkutil_cDLLd.dll"
	-@erase "$(OUTDIR)\dkutil_cDLLd.exp"
	-@erase "$(OUTDIR)\dkutil_cDLLd.ilk"
	-@erase "$(OUTDIR)\dkutil_cDLLd.lib"
	-@erase "$(OUTDIR)\dkutil_cDLLd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USE_DKC_DLL" /D "DKUTIL_CDLL_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dkutil_cDLL.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_cDLL.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dkcDCF.sbr" \
	"$(INTDIR)\dkcDCF01.sbr" \
	"$(INTDIR)\dkcTelecomConfirmFile.sbr" \
	"$(INTDIR)\dkcTelecomConfirmPacket.sbr" \
	"$(INTDIR)\dkc2Tree.sbr" \
	"$(INTDIR)\dkcArrayOneByOne.sbr" \
	"$(INTDIR)\dkcBuffer.sbr" \
	"$(INTDIR)\dkcCircularMemoryStream.sbr" \
	"$(INTDIR)\dkcCircularStream.sbr" \
	"$(INTDIR)\dkcDeque.sbr" \
	"$(INTDIR)\dkcMemoryStream.sbr" \
	"$(INTDIR)\dkcQueue.sbr" \
	"$(INTDIR)\dkcRedBlackTree.sbr" \
	"$(INTDIR)\dkcSingleList.sbr" \
	"$(INTDIR)\dkcStack.sbr" \
	"$(INTDIR)\dkcString.sbr" \
	"$(INTDIR)\dkcRijndael.sbr" \
	"$(INTDIR)\rijndael-alg-fst.sbr" \
	"$(INTDIR)\rijndael-api-fst.sbr" \
	"$(INTDIR)\dkcArcfour.sbr" \
	"$(INTDIR)\dkcBlowfish.sbr" \
	"$(INTDIR)\dkcCryptograph.sbr" \
	"$(INTDIR)\dkcHC256.sbr" \
	"$(INTDIR)\dkcHMAC.sbr" \
	"$(INTDIR)\dkcSemiRealRandom.sbr" \
	"$(INTDIR)\dkcSNOW20.sbr" \
	"$(INTDIR)\dkcVernam.sbr" \
	"$(INTDIR)\dkcUnityArchivers.sbr" \
	"$(INTDIR)\dkcBlockSort.sbr" \
	"$(INTDIR)\dkcLZSS.sbr" \
	"$(INTDIR)\dkcLZW.sbr" \
	"$(INTDIR)\dkcRLE.sbr" \
	"$(INTDIR)\dkcCRC.sbr" \
	"$(INTDIR)\dkcHash.sbr" \
	"$(INTDIR)\dkcMD2.sbr" \
	"$(INTDIR)\dkcMD4.sbr" \
	"$(INTDIR)\dkcMD5.sbr" \
	"$(INTDIR)\dkcSHA.sbr" \
	"$(INTDIR)\dkcSHA1.sbr" \
	"$(INTDIR)\dkcSHA256.sbr" \
	"$(INTDIR)\dkcSHA384.sbr" \
	"$(INTDIR)\dkcSHA512.sbr" \
	"$(INTDIR)\md5.sbr" \
	"$(INTDIR)\st.sbr" \
	"$(INTDIR)\dkcGenericFileSystem.sbr" \
	"$(INTDIR)\dkcSafeFileSystem.sbr" \
	"$(INTDIR)\dkcSJISFileSystem.sbr" \
	"$(INTDIR)\dkcStream.sbr" \
	"$(INTDIR)\dkcBit.sbr" \
	"$(INTDIR)\dkcDLL.sbr" \
	"$(INTDIR)\dkcLua.sbr" \
	"$(INTDIR)\dkcMath.sbr" \
	"$(INTDIR)\dkcMemoryPool.sbr" \
	"$(INTDIR)\dkcOS.sbr" \
	"$(INTDIR)\dkcOSIndependent.sbr" \
	"$(INTDIR)\dkcRegex.sbr" \
	"$(INTDIR)\dkcSort.sbr" \
	"$(INTDIR)\dkcStdio.sbr" \
	"$(INTDIR)\dkcTaskEngine.sbr" \
	"$(INTDIR)\dkcThread.sbr" \
	"$(INTDIR)\dkcThreadLock.sbr" \
	"$(INTDIR)\dkcUniqueID.sbr"

"$(OUTDIR)\dkutil_cDLL.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\dkutil_cDLLd.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dkutil_cDLLd.dll" /implib:"$(OUTDIR)\dkutil_cDLLd.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dkcDCF.obj" \
	"$(INTDIR)\dkcDCF01.obj" \
	"$(INTDIR)\dkcTelecomConfirmFile.obj" \
	"$(INTDIR)\dkcTelecomConfirmPacket.obj" \
	"$(INTDIR)\dkc2Tree.obj" \
	"$(INTDIR)\dkcArrayOneByOne.obj" \
	"$(INTDIR)\dkcBuffer.obj" \
	"$(INTDIR)\dkcCircularMemoryStream.obj" \
	"$(INTDIR)\dkcCircularStream.obj" \
	"$(INTDIR)\dkcDeque.obj" \
	"$(INTDIR)\dkcMemoryStream.obj" \
	"$(INTDIR)\dkcQueue.obj" \
	"$(INTDIR)\dkcRedBlackTree.obj" \
	"$(INTDIR)\dkcSingleList.obj" \
	"$(INTDIR)\dkcStack.obj" \
	"$(INTDIR)\dkcString.obj" \
	"$(INTDIR)\dkcRijndael.obj" \
	"$(INTDIR)\rijndael-alg-fst.obj" \
	"$(INTDIR)\rijndael-api-fst.obj" \
	"$(INTDIR)\dkcArcfour.obj" \
	"$(INTDIR)\dkcBlowfish.obj" \
	"$(INTDIR)\dkcCryptograph.obj" \
	"$(INTDIR)\dkcHC256.obj" \
	"$(INTDIR)\dkcHMAC.obj" \
	"$(INTDIR)\dkcSemiRealRandom.obj" \
	"$(INTDIR)\dkcSNOW20.obj" \
	"$(INTDIR)\dkcVernam.obj" \
	"$(INTDIR)\dkcUnityArchivers.obj" \
	"$(INTDIR)\dkcBlockSort.obj" \
	"$(INTDIR)\dkcLZSS.obj" \
	"$(INTDIR)\dkcLZW.obj" \
	"$(INTDIR)\dkcRLE.obj" \
	"$(INTDIR)\dkcCRC.obj" \
	"$(INTDIR)\dkcHash.obj" \
	"$(INTDIR)\dkcMD2.obj" \
	"$(INTDIR)\dkcMD4.obj" \
	"$(INTDIR)\dkcMD5.obj" \
	"$(INTDIR)\dkcSHA.obj" \
	"$(INTDIR)\dkcSHA1.obj" \
	"$(INTDIR)\dkcSHA256.obj" \
	"$(INTDIR)\dkcSHA384.obj" \
	"$(INTDIR)\dkcSHA512.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\st.obj" \
	"$(INTDIR)\dkcGenericFileSystem.obj" \
	"$(INTDIR)\dkcSafeFileSystem.obj" \
	"$(INTDIR)\dkcSJISFileSystem.obj" \
	"$(INTDIR)\dkcStream.obj" \
	"$(INTDIR)\dkcBit.obj" \
	"$(INTDIR)\dkcDLL.obj" \
	"$(INTDIR)\dkcLua.obj" \
	"$(INTDIR)\dkcMath.obj" \
	"$(INTDIR)\dkcMemoryPool.obj" \
	"$(INTDIR)\dkcOS.obj" \
	"$(INTDIR)\dkcOSIndependent.obj" \
	"$(INTDIR)\dkcRegex.obj" \
	"$(INTDIR)\dkcSort.obj" \
	"$(INTDIR)\dkcStdio.obj" \
	"$(INTDIR)\dkcTaskEngine.obj" \
	"$(INTDIR)\dkcThread.obj" \
	"$(INTDIR)\dkcThreadLock.obj" \
	"$(INTDIR)\dkcUniqueID.obj" \
	".\Debug\dkutil_cd.lib"

"$(OUTDIR)\dkutil_cDLLd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"

OUTDIR=.\ReleaseMTDLL
INTDIR=.\ReleaseMTDLL
# Begin Custom Macros
OutDir=.\ReleaseMTDLL
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dkutil_cDLLMT.dll"

!ELSE 

ALL : "dkutil_c - Win32 ReleaseMT" "$(OUTDIR)\dkutil_cDLLMT.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dkutil_c - Win32 ReleaseMTCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dkc2Tree.obj"
	-@erase "$(INTDIR)\dkcArcfour.obj"
	-@erase "$(INTDIR)\dkcArrayOneByOne.obj"
	-@erase "$(INTDIR)\dkcBit.obj"
	-@erase "$(INTDIR)\dkcBlockSort.obj"
	-@erase "$(INTDIR)\dkcBlowfish.obj"
	-@erase "$(INTDIR)\dkcBuffer.obj"
	-@erase "$(INTDIR)\dkcCircularMemoryStream.obj"
	-@erase "$(INTDIR)\dkcCircularStream.obj"
	-@erase "$(INTDIR)\dkcCRC.obj"
	-@erase "$(INTDIR)\dkcCryptograph.obj"
	-@erase "$(INTDIR)\dkcDCF.obj"
	-@erase "$(INTDIR)\dkcDCF01.obj"
	-@erase "$(INTDIR)\dkcDeque.obj"
	-@erase "$(INTDIR)\dkcDLL.obj"
	-@erase "$(INTDIR)\dkcGenericFileSystem.obj"
	-@erase "$(INTDIR)\dkcHash.obj"
	-@erase "$(INTDIR)\dkcHC256.obj"
	-@erase "$(INTDIR)\dkcHMAC.obj"
	-@erase "$(INTDIR)\dkcLua.obj"
	-@erase "$(INTDIR)\dkcLZSS.obj"
	-@erase "$(INTDIR)\dkcLZW.obj"
	-@erase "$(INTDIR)\dkcMath.obj"
	-@erase "$(INTDIR)\dkcMD2.obj"
	-@erase "$(INTDIR)\dkcMD4.obj"
	-@erase "$(INTDIR)\dkcMD5.obj"
	-@erase "$(INTDIR)\dkcMemoryPool.obj"
	-@erase "$(INTDIR)\dkcMemoryStream.obj"
	-@erase "$(INTDIR)\dkcOS.obj"
	-@erase "$(INTDIR)\dkcOSIndependent.obj"
	-@erase "$(INTDIR)\dkcQueue.obj"
	-@erase "$(INTDIR)\dkcRedBlackTree.obj"
	-@erase "$(INTDIR)\dkcRegex.obj"
	-@erase "$(INTDIR)\dkcRijndael.obj"
	-@erase "$(INTDIR)\dkcRLE.obj"
	-@erase "$(INTDIR)\dkcSafeFileSystem.obj"
	-@erase "$(INTDIR)\dkcSemiRealRandom.obj"
	-@erase "$(INTDIR)\dkcSHA.obj"
	-@erase "$(INTDIR)\dkcSHA1.obj"
	-@erase "$(INTDIR)\dkcSHA256.obj"
	-@erase "$(INTDIR)\dkcSHA384.obj"
	-@erase "$(INTDIR)\dkcSHA512.obj"
	-@erase "$(INTDIR)\dkcSingleList.obj"
	-@erase "$(INTDIR)\dkcSJISFileSystem.obj"
	-@erase "$(INTDIR)\dkcSNOW20.obj"
	-@erase "$(INTDIR)\dkcSort.obj"
	-@erase "$(INTDIR)\dkcStack.obj"
	-@erase "$(INTDIR)\dkcStdio.obj"
	-@erase "$(INTDIR)\dkcStream.obj"
	-@erase "$(INTDIR)\dkcString.obj"
	-@erase "$(INTDIR)\dkcTaskEngine.obj"
	-@erase "$(INTDIR)\dkcTelecomConfirmFile.obj"
	-@erase "$(INTDIR)\dkcTelecomConfirmPacket.obj"
	-@erase "$(INTDIR)\dkcThread.obj"
	-@erase "$(INTDIR)\dkcThreadLock.obj"
	-@erase "$(INTDIR)\dkcUniqueID.obj"
	-@erase "$(INTDIR)\dkcUnityArchivers.obj"
	-@erase "$(INTDIR)\dkcVernam.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\rijndael-alg-fst.obj"
	-@erase "$(INTDIR)\rijndael-api-fst.obj"
	-@erase "$(INTDIR)\st.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dkutil_cDLLMT.dll"
	-@erase "$(OUTDIR)\dkutil_cDLLMT.exp"
	-@erase "$(OUTDIR)\dkutil_cDLLMT.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USE_DKC_DLL" /D "DKUTIL_CDLL_EXPORTS" /Fp"$(INTDIR)\dkutil_cDLL.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_cDLL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\dkutil_cDLLMT.pdb" /machine:I386 /out:"$(OUTDIR)\dkutil_cDLLMT.dll" /implib:"$(OUTDIR)\dkutil_cDLLMT.lib" 
LINK32_OBJS= \
	"$(INTDIR)\dkcDCF.obj" \
	"$(INTDIR)\dkcDCF01.obj" \
	"$(INTDIR)\dkcTelecomConfirmFile.obj" \
	"$(INTDIR)\dkcTelecomConfirmPacket.obj" \
	"$(INTDIR)\dkc2Tree.obj" \
	"$(INTDIR)\dkcArrayOneByOne.obj" \
	"$(INTDIR)\dkcBuffer.obj" \
	"$(INTDIR)\dkcCircularMemoryStream.obj" \
	"$(INTDIR)\dkcCircularStream.obj" \
	"$(INTDIR)\dkcDeque.obj" \
	"$(INTDIR)\dkcMemoryStream.obj" \
	"$(INTDIR)\dkcQueue.obj" \
	"$(INTDIR)\dkcRedBlackTree.obj" \
	"$(INTDIR)\dkcSingleList.obj" \
	"$(INTDIR)\dkcStack.obj" \
	"$(INTDIR)\dkcString.obj" \
	"$(INTDIR)\dkcRijndael.obj" \
	"$(INTDIR)\rijndael-alg-fst.obj" \
	"$(INTDIR)\rijndael-api-fst.obj" \
	"$(INTDIR)\dkcArcfour.obj" \
	"$(INTDIR)\dkcBlowfish.obj" \
	"$(INTDIR)\dkcCryptograph.obj" \
	"$(INTDIR)\dkcHC256.obj" \
	"$(INTDIR)\dkcHMAC.obj" \
	"$(INTDIR)\dkcSemiRealRandom.obj" \
	"$(INTDIR)\dkcSNOW20.obj" \
	"$(INTDIR)\dkcVernam.obj" \
	"$(INTDIR)\dkcUnityArchivers.obj" \
	"$(INTDIR)\dkcBlockSort.obj" \
	"$(INTDIR)\dkcLZSS.obj" \
	"$(INTDIR)\dkcLZW.obj" \
	"$(INTDIR)\dkcRLE.obj" \
	"$(INTDIR)\dkcCRC.obj" \
	"$(INTDIR)\dkcHash.obj" \
	"$(INTDIR)\dkcMD2.obj" \
	"$(INTDIR)\dkcMD4.obj" \
	"$(INTDIR)\dkcMD5.obj" \
	"$(INTDIR)\dkcSHA.obj" \
	"$(INTDIR)\dkcSHA1.obj" \
	"$(INTDIR)\dkcSHA256.obj" \
	"$(INTDIR)\dkcSHA384.obj" \
	"$(INTDIR)\dkcSHA512.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\st.obj" \
	"$(INTDIR)\dkcGenericFileSystem.obj" \
	"$(INTDIR)\dkcSafeFileSystem.obj" \
	"$(INTDIR)\dkcSJISFileSystem.obj" \
	"$(INTDIR)\dkcStream.obj" \
	"$(INTDIR)\dkcBit.obj" \
	"$(INTDIR)\dkcDLL.obj" \
	"$(INTDIR)\dkcLua.obj" \
	"$(INTDIR)\dkcMath.obj" \
	"$(INTDIR)\dkcMemoryPool.obj" \
	"$(INTDIR)\dkcOS.obj" \
	"$(INTDIR)\dkcOSIndependent.obj" \
	"$(INTDIR)\dkcRegex.obj" \
	"$(INTDIR)\dkcSort.obj" \
	"$(INTDIR)\dkcStdio.obj" \
	"$(INTDIR)\dkcTaskEngine.obj" \
	"$(INTDIR)\dkcThread.obj" \
	"$(INTDIR)\dkcThreadLock.obj" \
	"$(INTDIR)\dkcUniqueID.obj" \
	".\ReleaseMT\dkutil_cMT.lib"

"$(OUTDIR)\dkutil_cDLLMT.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"

OUTDIR=.\DebugMTDLL
INTDIR=.\DebugMTDLL

!IF "$(RECURSE)" == "0" 

ALL : ".\DebugDLL\dkutil_cDLLMTd.dll"

!ELSE 

ALL : "dkutil_c - Win32 DebugMT" ".\DebugDLL\dkutil_cDLLMTd.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dkutil_c - Win32 DebugMTCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dkc2Tree.obj"
	-@erase "$(INTDIR)\dkcArcfour.obj"
	-@erase "$(INTDIR)\dkcArrayOneByOne.obj"
	-@erase "$(INTDIR)\dkcBit.obj"
	-@erase "$(INTDIR)\dkcBlockSort.obj"
	-@erase "$(INTDIR)\dkcBlowfish.obj"
	-@erase "$(INTDIR)\dkcBuffer.obj"
	-@erase "$(INTDIR)\dkcCircularMemoryStream.obj"
	-@erase "$(INTDIR)\dkcCircularStream.obj"
	-@erase "$(INTDIR)\dkcCRC.obj"
	-@erase "$(INTDIR)\dkcCryptograph.obj"
	-@erase "$(INTDIR)\dkcDCF.obj"
	-@erase "$(INTDIR)\dkcDCF01.obj"
	-@erase "$(INTDIR)\dkcDeque.obj"
	-@erase "$(INTDIR)\dkcDLL.obj"
	-@erase "$(INTDIR)\dkcGenericFileSystem.obj"
	-@erase "$(INTDIR)\dkcHash.obj"
	-@erase "$(INTDIR)\dkcHC256.obj"
	-@erase "$(INTDIR)\dkcHMAC.obj"
	-@erase "$(INTDIR)\dkcLua.obj"
	-@erase "$(INTDIR)\dkcLZSS.obj"
	-@erase "$(INTDIR)\dkcLZW.obj"
	-@erase "$(INTDIR)\dkcMath.obj"
	-@erase "$(INTDIR)\dkcMD2.obj"
	-@erase "$(INTDIR)\dkcMD4.obj"
	-@erase "$(INTDIR)\dkcMD5.obj"
	-@erase "$(INTDIR)\dkcMemoryPool.obj"
	-@erase "$(INTDIR)\dkcMemoryStream.obj"
	-@erase "$(INTDIR)\dkcOS.obj"
	-@erase "$(INTDIR)\dkcOSIndependent.obj"
	-@erase "$(INTDIR)\dkcQueue.obj"
	-@erase "$(INTDIR)\dkcRedBlackTree.obj"
	-@erase "$(INTDIR)\dkcRegex.obj"
	-@erase "$(INTDIR)\dkcRijndael.obj"
	-@erase "$(INTDIR)\dkcRLE.obj"
	-@erase "$(INTDIR)\dkcSafeFileSystem.obj"
	-@erase "$(INTDIR)\dkcSemiRealRandom.obj"
	-@erase "$(INTDIR)\dkcSHA.obj"
	-@erase "$(INTDIR)\dkcSHA1.obj"
	-@erase "$(INTDIR)\dkcSHA256.obj"
	-@erase "$(INTDIR)\dkcSHA384.obj"
	-@erase "$(INTDIR)\dkcSHA512.obj"
	-@erase "$(INTDIR)\dkcSingleList.obj"
	-@erase "$(INTDIR)\dkcSJISFileSystem.obj"
	-@erase "$(INTDIR)\dkcSNOW20.obj"
	-@erase "$(INTDIR)\dkcSort.obj"
	-@erase "$(INTDIR)\dkcStack.obj"
	-@erase "$(INTDIR)\dkcStdio.obj"
	-@erase "$(INTDIR)\dkcStream.obj"
	-@erase "$(INTDIR)\dkcString.obj"
	-@erase "$(INTDIR)\dkcTaskEngine.obj"
	-@erase "$(INTDIR)\dkcTelecomConfirmFile.obj"
	-@erase "$(INTDIR)\dkcTelecomConfirmPacket.obj"
	-@erase "$(INTDIR)\dkcThread.obj"
	-@erase "$(INTDIR)\dkcThreadLock.obj"
	-@erase "$(INTDIR)\dkcUniqueID.obj"
	-@erase "$(INTDIR)\dkcUnityArchivers.obj"
	-@erase "$(INTDIR)\dkcVernam.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\rijndael-alg-fst.obj"
	-@erase "$(INTDIR)\rijndael-api-fst.obj"
	-@erase "$(INTDIR)\st.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dkutil_cDLLMTd.exp"
	-@erase "$(OUTDIR)\dkutil_cDLLMTd.lib"
	-@erase "$(OUTDIR)\dkutil_cDLLMTd.pdb"
	-@erase ".\DebugDLL\dkutil_cDLLMTd.dll"
	-@erase ".\DebugDLL\dkutil_cDLLMTd.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USE_DKC_DLL" /D "DKUTIL_CDLL_EXPORTS" /Fp"$(INTDIR)\dkutil_cDLL.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_cDLL.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\dkutil_cDLLMTd.pdb" /debug /machine:I386 /out:"DebugDLL/dkutil_cDLLMTd.dll" /implib:"$(OUTDIR)\dkutil_cDLLMTd.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dkcDCF.obj" \
	"$(INTDIR)\dkcDCF01.obj" \
	"$(INTDIR)\dkcTelecomConfirmFile.obj" \
	"$(INTDIR)\dkcTelecomConfirmPacket.obj" \
	"$(INTDIR)\dkc2Tree.obj" \
	"$(INTDIR)\dkcArrayOneByOne.obj" \
	"$(INTDIR)\dkcBuffer.obj" \
	"$(INTDIR)\dkcCircularMemoryStream.obj" \
	"$(INTDIR)\dkcCircularStream.obj" \
	"$(INTDIR)\dkcDeque.obj" \
	"$(INTDIR)\dkcMemoryStream.obj" \
	"$(INTDIR)\dkcQueue.obj" \
	"$(INTDIR)\dkcRedBlackTree.obj" \
	"$(INTDIR)\dkcSingleList.obj" \
	"$(INTDIR)\dkcStack.obj" \
	"$(INTDIR)\dkcString.obj" \
	"$(INTDIR)\dkcRijndael.obj" \
	"$(INTDIR)\rijndael-alg-fst.obj" \
	"$(INTDIR)\rijndael-api-fst.obj" \
	"$(INTDIR)\dkcArcfour.obj" \
	"$(INTDIR)\dkcBlowfish.obj" \
	"$(INTDIR)\dkcCryptograph.obj" \
	"$(INTDIR)\dkcHC256.obj" \
	"$(INTDIR)\dkcHMAC.obj" \
	"$(INTDIR)\dkcSemiRealRandom.obj" \
	"$(INTDIR)\dkcSNOW20.obj" \
	"$(INTDIR)\dkcVernam.obj" \
	"$(INTDIR)\dkcUnityArchivers.obj" \
	"$(INTDIR)\dkcBlockSort.obj" \
	"$(INTDIR)\dkcLZSS.obj" \
	"$(INTDIR)\dkcLZW.obj" \
	"$(INTDIR)\dkcRLE.obj" \
	"$(INTDIR)\dkcCRC.obj" \
	"$(INTDIR)\dkcHash.obj" \
	"$(INTDIR)\dkcMD2.obj" \
	"$(INTDIR)\dkcMD4.obj" \
	"$(INTDIR)\dkcMD5.obj" \
	"$(INTDIR)\dkcSHA.obj" \
	"$(INTDIR)\dkcSHA1.obj" \
	"$(INTDIR)\dkcSHA256.obj" \
	"$(INTDIR)\dkcSHA384.obj" \
	"$(INTDIR)\dkcSHA512.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\st.obj" \
	"$(INTDIR)\dkcGenericFileSystem.obj" \
	"$(INTDIR)\dkcSafeFileSystem.obj" \
	"$(INTDIR)\dkcSJISFileSystem.obj" \
	"$(INTDIR)\dkcStream.obj" \
	"$(INTDIR)\dkcBit.obj" \
	"$(INTDIR)\dkcDLL.obj" \
	"$(INTDIR)\dkcLua.obj" \
	"$(INTDIR)\dkcMath.obj" \
	"$(INTDIR)\dkcMemoryPool.obj" \
	"$(INTDIR)\dkcOS.obj" \
	"$(INTDIR)\dkcOSIndependent.obj" \
	"$(INTDIR)\dkcRegex.obj" \
	"$(INTDIR)\dkcSort.obj" \
	"$(INTDIR)\dkcStdio.obj" \
	"$(INTDIR)\dkcTaskEngine.obj" \
	"$(INTDIR)\dkcThread.obj" \
	"$(INTDIR)\dkcThreadLock.obj" \
	"$(INTDIR)\dkcUniqueID.obj" \
	".\DebugMT\dkutil_cMTd.lib"

".\DebugDLL\dkutil_cDLLMTd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dkutil_cDLL.dep")
!INCLUDE "dkutil_cDLL.dep"
!ELSE 
!MESSAGE Warning: cannot find "dkutil_cDLL.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dkutil_cDLL - Win32 Release" || "$(CFG)" == "dkutil_cDLL - Win32 Debug" || "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT" || "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"
SOURCE=.\dkcDCF.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcDCF.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcDCF.obj"	"$(INTDIR)\dkcDCF.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcDCF.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcDCF.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcDCF01.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcDCF01.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcDCF01.obj"	"$(INTDIR)\dkcDCF01.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcDCF01.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcDCF01.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcTelecomConfirmFile.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcTelecomConfirmFile.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcTelecomConfirmFile.obj"	"$(INTDIR)\dkcTelecomConfirmFile.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcTelecomConfirmFile.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcTelecomConfirmFile.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcTelecomConfirmPacket.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcTelecomConfirmPacket.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcTelecomConfirmPacket.obj"	"$(INTDIR)\dkcTelecomConfirmPacket.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcTelecomConfirmPacket.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcTelecomConfirmPacket.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkc2Tree.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkc2Tree.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkc2Tree.obj"	"$(INTDIR)\dkc2Tree.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkc2Tree.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkc2Tree.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcArrayOneByOne.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcArrayOneByOne.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcArrayOneByOne.obj"	"$(INTDIR)\dkcArrayOneByOne.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcArrayOneByOne.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcArrayOneByOne.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcBuffer.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcBuffer.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcBuffer.obj"	"$(INTDIR)\dkcBuffer.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcBuffer.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcBuffer.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcCircularMemoryStream.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcCircularMemoryStream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcCircularMemoryStream.obj"	"$(INTDIR)\dkcCircularMemoryStream.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcCircularMemoryStream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcCircularMemoryStream.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcCircularStream.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcCircularStream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcCircularStream.obj"	"$(INTDIR)\dkcCircularStream.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcCircularStream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcCircularStream.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcDeque.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcDeque.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcDeque.obj"	"$(INTDIR)\dkcDeque.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcDeque.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcDeque.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcMemoryStream.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcMemoryStream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcMemoryStream.obj"	"$(INTDIR)\dkcMemoryStream.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcMemoryStream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcMemoryStream.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcQueue.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcQueue.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcQueue.obj"	"$(INTDIR)\dkcQueue.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcQueue.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcQueue.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcRedBlackTree.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcRedBlackTree.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcRedBlackTree.obj"	"$(INTDIR)\dkcRedBlackTree.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcRedBlackTree.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcRedBlackTree.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSingleList.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSingleList.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSingleList.obj"	"$(INTDIR)\dkcSingleList.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSingleList.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSingleList.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcStack.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcStack.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcStack.obj"	"$(INTDIR)\dkcStack.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcStack.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcStack.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcString.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcString.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcString.obj"	"$(INTDIR)\dkcString.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcString.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcString.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcRijndael.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcRijndael.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcRijndael.obj"	"$(INTDIR)\dkcRijndael.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcRijndael.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcRijndael.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=".\rijndael-alg-fst.c"

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\rijndael-alg-fst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\rijndael-alg-fst.obj"	"$(INTDIR)\rijndael-alg-fst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\rijndael-alg-fst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\rijndael-alg-fst.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=".\rijndael-api-fst.c"

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\rijndael-api-fst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\rijndael-api-fst.obj"	"$(INTDIR)\rijndael-api-fst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\rijndael-api-fst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\rijndael-api-fst.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcArcfour.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcArcfour.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcArcfour.obj"	"$(INTDIR)\dkcArcfour.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcArcfour.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcArcfour.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcBlowfish.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcBlowfish.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcBlowfish.obj"	"$(INTDIR)\dkcBlowfish.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcBlowfish.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcBlowfish.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcCryptograph.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcCryptograph.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcCryptograph.obj"	"$(INTDIR)\dkcCryptograph.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcCryptograph.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcCryptograph.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcHC256.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcHC256.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcHC256.obj"	"$(INTDIR)\dkcHC256.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcHC256.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcHC256.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcHMAC.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcHMAC.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcHMAC.obj"	"$(INTDIR)\dkcHMAC.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcHMAC.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcHMAC.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSemiRealRandom.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSemiRealRandom.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSemiRealRandom.obj"	"$(INTDIR)\dkcSemiRealRandom.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSemiRealRandom.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSemiRealRandom.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSNOW20.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSNOW20.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSNOW20.obj"	"$(INTDIR)\dkcSNOW20.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSNOW20.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSNOW20.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcVernam.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcVernam.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcVernam.obj"	"$(INTDIR)\dkcVernam.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcVernam.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcVernam.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcUnityArchivers.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcUnityArchivers.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcUnityArchivers.obj"	"$(INTDIR)\dkcUnityArchivers.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcUnityArchivers.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcUnityArchivers.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcBlockSort.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcBlockSort.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcBlockSort.obj"	"$(INTDIR)\dkcBlockSort.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcBlockSort.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcBlockSort.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcLZSS.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcLZSS.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcLZSS.obj"	"$(INTDIR)\dkcLZSS.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcLZSS.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcLZSS.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcLZW.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcLZW.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcLZW.obj"	"$(INTDIR)\dkcLZW.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcLZW.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcLZW.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcRLE.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcRLE.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcRLE.obj"	"$(INTDIR)\dkcRLE.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcRLE.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcRLE.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcCRC.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcCRC.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcCRC.obj"	"$(INTDIR)\dkcCRC.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcCRC.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcCRC.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcHash.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcHash.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcHash.obj"	"$(INTDIR)\dkcHash.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcHash.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcHash.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcMD2.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcMD2.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcMD2.obj"	"$(INTDIR)\dkcMD2.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcMD2.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcMD2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcMD4.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcMD4.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcMD4.obj"	"$(INTDIR)\dkcMD4.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcMD4.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcMD4.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcMD5.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcMD5.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcMD5.obj"	"$(INTDIR)\dkcMD5.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcMD5.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcMD5.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSHA.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSHA.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSHA.obj"	"$(INTDIR)\dkcSHA.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSHA.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSHA.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSHA1.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSHA1.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSHA1.obj"	"$(INTDIR)\dkcSHA1.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSHA1.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSHA1.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSHA256.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSHA256.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSHA256.obj"	"$(INTDIR)\dkcSHA256.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSHA256.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSHA256.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSHA384.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSHA384.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSHA384.obj"	"$(INTDIR)\dkcSHA384.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSHA384.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSHA384.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSHA512.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSHA512.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSHA512.obj"	"$(INTDIR)\dkcSHA512.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSHA512.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSHA512.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\md5.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\st.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\st.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\st.obj"	"$(INTDIR)\st.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\st.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\st.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcGenericFileSystem.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcGenericFileSystem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcGenericFileSystem.obj"	"$(INTDIR)\dkcGenericFileSystem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcGenericFileSystem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcGenericFileSystem.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSafeFileSystem.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSafeFileSystem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSafeFileSystem.obj"	"$(INTDIR)\dkcSafeFileSystem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSafeFileSystem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSafeFileSystem.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSJISFileSystem.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSJISFileSystem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSJISFileSystem.obj"	"$(INTDIR)\dkcSJISFileSystem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSJISFileSystem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSJISFileSystem.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcStream.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcStream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcStream.obj"	"$(INTDIR)\dkcStream.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcStream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcStream.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcBit.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcBit.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcBit.obj"	"$(INTDIR)\dkcBit.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcBit.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcBit.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcDLL.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcDLL.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcDLL.obj"	"$(INTDIR)\dkcDLL.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcDLL.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcDLL.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcLua.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcLua.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcLua.obj"	"$(INTDIR)\dkcLua.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcLua.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcLua.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcMath.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcMath.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcMath.obj"	"$(INTDIR)\dkcMath.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcMath.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcMath.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcMemoryPool.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcMemoryPool.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcMemoryPool.obj"	"$(INTDIR)\dkcMemoryPool.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcMemoryPool.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcMemoryPool.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcOS.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcOS.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcOS.obj"	"$(INTDIR)\dkcOS.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcOS.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcOS.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcOSIndependent.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcOSIndependent.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcOSIndependent.obj"	"$(INTDIR)\dkcOSIndependent.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcOSIndependent.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcOSIndependent.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcRegex.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcRegex.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcRegex.obj"	"$(INTDIR)\dkcRegex.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcRegex.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcRegex.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcSort.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcSort.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcSort.obj"	"$(INTDIR)\dkcSort.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcSort.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcSort.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcStdio.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcStdio.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcStdio.obj"	"$(INTDIR)\dkcStdio.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcStdio.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcStdio.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcTaskEngine.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcTaskEngine.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcTaskEngine.obj"	"$(INTDIR)\dkcTaskEngine.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcTaskEngine.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcTaskEngine.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcThread.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcThread.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcThread.obj"	"$(INTDIR)\dkcThread.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcThread.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcThread.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcThreadLock.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcThreadLock.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcThreadLock.obj"	"$(INTDIR)\dkcThreadLock.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcThreadLock.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcThreadLock.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dkcUniqueID.c

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"


"$(INTDIR)\dkcUniqueID.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"


"$(INTDIR)\dkcUniqueID.obj"	"$(INTDIR)\dkcUniqueID.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"


"$(INTDIR)\dkcUniqueID.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"


"$(INTDIR)\dkcUniqueID.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

!IF  "$(CFG)" == "dkutil_cDLL - Win32 Release"

"dkutil_c - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_c.mak CFG="dkutil_c - Win32 Release" 
   cd "."

"dkutil_c - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_c.mak CFG="dkutil_c - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 Debug"

"dkutil_c - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_c.mak CFG="dkutil_c - Win32 Debug" 
   cd "."

"dkutil_c - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_c.mak CFG="dkutil_c - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 ReleaseMT"

"dkutil_c - Win32 ReleaseMT" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_c.mak CFG="dkutil_c - Win32 ReleaseMT" 
   cd "."

"dkutil_c - Win32 ReleaseMTCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_c.mak CFG="dkutil_c - Win32 ReleaseMT" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "dkutil_cDLL - Win32 DebugMT"

"dkutil_c - Win32 DebugMT" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_c.mak CFG="dkutil_c - Win32 DebugMT" 
   cd "."

"dkutil_c - Win32 DebugMTCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_c.mak CFG="dkutil_c - Win32 DebugMT" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

