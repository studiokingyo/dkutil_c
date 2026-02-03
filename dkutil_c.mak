# Microsoft Developer Studio Generated NMAKE File, Based on dkutil_c.dsp
!IF "$(CFG)" == ""
CFG=dkutil_c - Win32 DebugMT
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの dkutil_c - Win32 DebugMT を設定します。
!ENDIF 

!IF "$(CFG)" != "dkutil_c - Win32 Release" && "$(CFG)" != "dkutil_c - Win32 Debug" && "$(CFG)" != "dkutil_c - Win32 DebugMT" && "$(CFG)" != "dkutil_c - Win32 ReleaseMT"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "dkutil_c.mak" CFG="dkutil_c - Win32 DebugMT"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "dkutil_c - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkutil_c - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkutil_c - Win32 DebugMT" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkutil_c - Win32 ReleaseMT" ("Win32 (x86) Static Library" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dkutil_c - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dkutil_c.lib" "$(OUTDIR)\dkutil_c.bsc"

!ELSE 

ALL : "dkutil_cstd - Win32 Release" "$(OUTDIR)\dkutil_c.lib" "$(OUTDIR)\dkutil_c.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dkutil_cstd - Win32 ReleaseCLEAN" 
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
	-@erase "$(OUTDIR)\dkutil_c.bsc"
	-@erase "$(OUTDIR)\dkutil_c.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dkutil_c.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_c.bsc" 
BSC32_SBRS= \
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
	"$(INTDIR)\dkcUniqueID.sbr" \
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
	"$(INTDIR)\dkcUnityArchivers.sbr" \
	"$(INTDIR)\dkcBlockSort.sbr" \
	"$(INTDIR)\dkcLZSS.sbr" \
	"$(INTDIR)\dkcLZW.sbr" \
	"$(INTDIR)\dkcRLE.sbr" \
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
	"$(INTDIR)\dkc2Tree.sbr" \
	"$(INTDIR)\dkcArrayOneByOne.sbr" \
	"$(INTDIR)\dkcBuffer.sbr" \
	"$(INTDIR)\dkcCircularMemoryStream.sbr" \
	"$(INTDIR)\dkcDeque.sbr" \
	"$(INTDIR)\dkcMemoryStream.sbr" \
	"$(INTDIR)\dkcQueue.sbr" \
	"$(INTDIR)\dkcRedBlackTree.sbr" \
	"$(INTDIR)\dkcSingleList.sbr" \
	"$(INTDIR)\dkcStack.sbr" \
	"$(INTDIR)\dkcString.sbr" \
	"$(INTDIR)\dkcDCF.sbr" \
	"$(INTDIR)\dkcDCF01.sbr" \
	"$(INTDIR)\dkcTelecomConfirmFile.sbr" \
	"$(INTDIR)\dkcTelecomConfirmPacket.sbr"

"$(OUTDIR)\dkutil_c.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dkutil_c.lib" 
LIB32_OBJS= \
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
	"$(INTDIR)\dkcUnityArchivers.obj" \
	"$(INTDIR)\dkcBlockSort.obj" \
	"$(INTDIR)\dkcLZSS.obj" \
	"$(INTDIR)\dkcLZW.obj" \
	"$(INTDIR)\dkcRLE.obj" \
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
	"$(INTDIR)\dkc2Tree.obj" \
	"$(INTDIR)\dkcArrayOneByOne.obj" \
	"$(INTDIR)\dkcBuffer.obj" \
	"$(INTDIR)\dkcCircularMemoryStream.obj" \
	"$(INTDIR)\dkcDeque.obj" \
	"$(INTDIR)\dkcMemoryStream.obj" \
	"$(INTDIR)\dkcQueue.obj" \
	"$(INTDIR)\dkcRedBlackTree.obj" \
	"$(INTDIR)\dkcSingleList.obj" \
	"$(INTDIR)\dkcStack.obj" \
	"$(INTDIR)\dkcString.obj" \
	"$(INTDIR)\dkcDCF.obj" \
	"$(INTDIR)\dkcDCF01.obj" \
	"$(INTDIR)\dkcTelecomConfirmFile.obj" \
	"$(INTDIR)\dkcTelecomConfirmPacket.obj" \
	".\dkutil_cstd\Release\dkutil_cstd.lib"

"$(OUTDIR)\dkutil_c.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "dkutil_cstd - Win32 Release" "$(OUTDIR)\dkutil_c.lib" "$(OUTDIR)\dkutil_c.bsc"
   copylib.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dkutil_cd.lib" "$(OUTDIR)\dkutil_c.bsc"

!ELSE 

ALL : "dkutil_cstd - Win32 Debug" "$(OUTDIR)\dkutil_cd.lib" "$(OUTDIR)\dkutil_c.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dkutil_cstd - Win32 DebugCLEAN" 
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
	-@erase "$(OUTDIR)\dkutil_c.bsc"
	-@erase "$(OUTDIR)\dkutil_cd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W4 /WX /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dkutil_c.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_c.bsc" 
BSC32_SBRS= \
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
	"$(INTDIR)\dkcUniqueID.sbr" \
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
	"$(INTDIR)\dkcUnityArchivers.sbr" \
	"$(INTDIR)\dkcBlockSort.sbr" \
	"$(INTDIR)\dkcLZSS.sbr" \
	"$(INTDIR)\dkcLZW.sbr" \
	"$(INTDIR)\dkcRLE.sbr" \
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
	"$(INTDIR)\dkcDCF.sbr" \
	"$(INTDIR)\dkcDCF01.sbr" \
	"$(INTDIR)\dkcTelecomConfirmFile.sbr" \
	"$(INTDIR)\dkcTelecomConfirmPacket.sbr"

"$(OUTDIR)\dkutil_c.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dkutil_cd.lib" 
LIB32_OBJS= \
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
	"$(INTDIR)\dkcUnityArchivers.obj" \
	"$(INTDIR)\dkcBlockSort.obj" \
	"$(INTDIR)\dkcLZSS.obj" \
	"$(INTDIR)\dkcLZW.obj" \
	"$(INTDIR)\dkcRLE.obj" \
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
	"$(INTDIR)\dkcDCF.obj" \
	"$(INTDIR)\dkcDCF01.obj" \
	"$(INTDIR)\dkcTelecomConfirmFile.obj" \
	"$(INTDIR)\dkcTelecomConfirmPacket.obj" \
	".\dkutil_cstd\Debug\dkutil_cstdd.lib"

"$(OUTDIR)\dkutil_cd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=ライブラリをコピーします。
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "dkutil_cstd - Win32 Debug" "$(OUTDIR)\dkutil_cd.lib" "$(OUTDIR)\dkutil_c.bsc"
   copylib.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 DebugMT"

OUTDIR=.\DebugMT
INTDIR=.\DebugMT
# Begin Custom Macros
OutDir=.\DebugMT
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dkutil_cMTd.lib" "$(OUTDIR)\dkutil_c.bsc"

!ELSE 

ALL : "dkutil_cstd - Win32 DebugMT" "$(OUTDIR)\dkutil_cMTd.lib" "$(OUTDIR)\dkutil_c.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dkutil_cstd - Win32 DebugMTCLEAN" 
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
	-@erase "$(OUTDIR)\dkutil_c.bsc"
	-@erase "$(OUTDIR)\dkutil_cMTd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W4 /WX /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dkutil_c.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_c.bsc" 
BSC32_SBRS= \
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
	"$(INTDIR)\dkcUniqueID.sbr" \
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
	"$(INTDIR)\dkcUnityArchivers.sbr" \
	"$(INTDIR)\dkcBlockSort.sbr" \
	"$(INTDIR)\dkcLZSS.sbr" \
	"$(INTDIR)\dkcLZW.sbr" \
	"$(INTDIR)\dkcRLE.sbr" \
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
	"$(INTDIR)\dkcDCF.sbr" \
	"$(INTDIR)\dkcDCF01.sbr" \
	"$(INTDIR)\dkcTelecomConfirmFile.sbr" \
	"$(INTDIR)\dkcTelecomConfirmPacket.sbr"

"$(OUTDIR)\dkutil_c.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dkutil_cMTd.lib" 
LIB32_OBJS= \
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
	"$(INTDIR)\dkcUnityArchivers.obj" \
	"$(INTDIR)\dkcBlockSort.obj" \
	"$(INTDIR)\dkcLZSS.obj" \
	"$(INTDIR)\dkcLZW.obj" \
	"$(INTDIR)\dkcRLE.obj" \
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
	"$(INTDIR)\dkcDCF.obj" \
	"$(INTDIR)\dkcDCF01.obj" \
	"$(INTDIR)\dkcTelecomConfirmFile.obj" \
	"$(INTDIR)\dkcTelecomConfirmPacket.obj" \
	".\dkutil_cstd\Debug\dkutil_cstdMTd.lib"

"$(OUTDIR)\dkutil_cMTd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=ライブラリをコピーします。
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\DebugMT
# End Custom Macros

$(DS_POSTBUILD_DEP) : "dkutil_cstd - Win32 DebugMT" "$(OUTDIR)\dkutil_cMTd.lib" "$(OUTDIR)\dkutil_c.bsc"
   copylib.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 ReleaseMT"

OUTDIR=.\ReleaseMT
INTDIR=.\ReleaseMT
# Begin Custom Macros
OutDir=.\ReleaseMT
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dkutil_cMT.lib" "$(OUTDIR)\dkutil_c.bsc"

!ELSE 

ALL : "dkutil_cstd - Win32 ReleaseMT" "$(OUTDIR)\dkutil_cMT.lib" "$(OUTDIR)\dkutil_c.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dkutil_cstd - Win32 ReleaseMTCLEAN" 
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
	-@erase "$(OUTDIR)\dkutil_c.bsc"
	-@erase "$(OUTDIR)\dkutil_cMT.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W4 /WX /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dkutil_c.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_c.bsc" 
BSC32_SBRS= \
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
	"$(INTDIR)\dkcUniqueID.sbr" \
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
	"$(INTDIR)\dkcUnityArchivers.sbr" \
	"$(INTDIR)\dkcBlockSort.sbr" \
	"$(INTDIR)\dkcLZSS.sbr" \
	"$(INTDIR)\dkcLZW.sbr" \
	"$(INTDIR)\dkcRLE.sbr" \
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
	"$(INTDIR)\dkc2Tree.sbr" \
	"$(INTDIR)\dkcArrayOneByOne.sbr" \
	"$(INTDIR)\dkcBuffer.sbr" \
	"$(INTDIR)\dkcCircularMemoryStream.sbr" \
	"$(INTDIR)\dkcDeque.sbr" \
	"$(INTDIR)\dkcMemoryStream.sbr" \
	"$(INTDIR)\dkcQueue.sbr" \
	"$(INTDIR)\dkcRedBlackTree.sbr" \
	"$(INTDIR)\dkcSingleList.sbr" \
	"$(INTDIR)\dkcStack.sbr" \
	"$(INTDIR)\dkcString.sbr" \
	"$(INTDIR)\dkcDCF.sbr" \
	"$(INTDIR)\dkcDCF01.sbr" \
	"$(INTDIR)\dkcTelecomConfirmFile.sbr" \
	"$(INTDIR)\dkcTelecomConfirmPacket.sbr"

"$(OUTDIR)\dkutil_c.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dkutil_cMT.lib" 
LIB32_OBJS= \
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
	"$(INTDIR)\dkcUnityArchivers.obj" \
	"$(INTDIR)\dkcBlockSort.obj" \
	"$(INTDIR)\dkcLZSS.obj" \
	"$(INTDIR)\dkcLZW.obj" \
	"$(INTDIR)\dkcRLE.obj" \
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
	"$(INTDIR)\dkc2Tree.obj" \
	"$(INTDIR)\dkcArrayOneByOne.obj" \
	"$(INTDIR)\dkcBuffer.obj" \
	"$(INTDIR)\dkcCircularMemoryStream.obj" \
	"$(INTDIR)\dkcDeque.obj" \
	"$(INTDIR)\dkcMemoryStream.obj" \
	"$(INTDIR)\dkcQueue.obj" \
	"$(INTDIR)\dkcRedBlackTree.obj" \
	"$(INTDIR)\dkcSingleList.obj" \
	"$(INTDIR)\dkcStack.obj" \
	"$(INTDIR)\dkcString.obj" \
	"$(INTDIR)\dkcDCF.obj" \
	"$(INTDIR)\dkcDCF01.obj" \
	"$(INTDIR)\dkcTelecomConfirmFile.obj" \
	"$(INTDIR)\dkcTelecomConfirmPacket.obj" \
	".\dkutil_cstd\Release\dkutil_cstdMT.lib"

"$(OUTDIR)\dkutil_cMT.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=ライブラリをコピーします。
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\ReleaseMT
# End Custom Macros

$(DS_POSTBUILD_DEP) : "dkutil_cstd - Win32 ReleaseMT" "$(OUTDIR)\dkutil_cMT.lib" "$(OUTDIR)\dkutil_c.bsc"
   copylib.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dkutil_c.dep")
!INCLUDE "dkutil_c.dep"
!ELSE 
!MESSAGE Warning: cannot find "dkutil_c.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dkutil_c - Win32 Release" || "$(CFG)" == "dkutil_c - Win32 Debug" || "$(CFG)" == "dkutil_c - Win32 DebugMT" || "$(CFG)" == "dkutil_c - Win32 ReleaseMT"
SOURCE=.\dkcGenericFileSystem.c

"$(INTDIR)\dkcGenericFileSystem.obj"	"$(INTDIR)\dkcGenericFileSystem.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSafeFileSystem.c

"$(INTDIR)\dkcSafeFileSystem.obj"	"$(INTDIR)\dkcSafeFileSystem.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSJISFileSystem.c

"$(INTDIR)\dkcSJISFileSystem.obj"	"$(INTDIR)\dkcSJISFileSystem.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcStream.c

"$(INTDIR)\dkcStream.obj"	"$(INTDIR)\dkcStream.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcBit.c

"$(INTDIR)\dkcBit.obj"	"$(INTDIR)\dkcBit.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcDLL.c

"$(INTDIR)\dkcDLL.obj"	"$(INTDIR)\dkcDLL.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcLua.c

"$(INTDIR)\dkcLua.obj"	"$(INTDIR)\dkcLua.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcMath.c

"$(INTDIR)\dkcMath.obj"	"$(INTDIR)\dkcMath.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcMemoryPool.c

"$(INTDIR)\dkcMemoryPool.obj"	"$(INTDIR)\dkcMemoryPool.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcOS.c

"$(INTDIR)\dkcOS.obj"	"$(INTDIR)\dkcOS.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcOSIndependent.c

"$(INTDIR)\dkcOSIndependent.obj"	"$(INTDIR)\dkcOSIndependent.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcRegex.c

"$(INTDIR)\dkcRegex.obj"	"$(INTDIR)\dkcRegex.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSort.c

"$(INTDIR)\dkcSort.obj"	"$(INTDIR)\dkcSort.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcStdio.c

"$(INTDIR)\dkcStdio.obj"	"$(INTDIR)\dkcStdio.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcTaskEngine.c

"$(INTDIR)\dkcTaskEngine.obj"	"$(INTDIR)\dkcTaskEngine.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcThread.c

"$(INTDIR)\dkcThread.obj"	"$(INTDIR)\dkcThread.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcThreadLock.c

"$(INTDIR)\dkcThreadLock.obj"	"$(INTDIR)\dkcThreadLock.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcUniqueID.c

"$(INTDIR)\dkcUniqueID.obj"	"$(INTDIR)\dkcUniqueID.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcCRC.c

"$(INTDIR)\dkcCRC.obj"	"$(INTDIR)\dkcCRC.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcHash.c

"$(INTDIR)\dkcHash.obj"	"$(INTDIR)\dkcHash.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcMD2.c

"$(INTDIR)\dkcMD2.obj"	"$(INTDIR)\dkcMD2.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcMD4.c

"$(INTDIR)\dkcMD4.obj"	"$(INTDIR)\dkcMD4.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcMD5.c

"$(INTDIR)\dkcMD5.obj"	"$(INTDIR)\dkcMD5.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSHA.c

"$(INTDIR)\dkcSHA.obj"	"$(INTDIR)\dkcSHA.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSHA1.c

"$(INTDIR)\dkcSHA1.obj"	"$(INTDIR)\dkcSHA1.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSHA256.c

"$(INTDIR)\dkcSHA256.obj"	"$(INTDIR)\dkcSHA256.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSHA384.c

"$(INTDIR)\dkcSHA384.obj"	"$(INTDIR)\dkcSHA384.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSHA512.c

"$(INTDIR)\dkcSHA512.obj"	"$(INTDIR)\dkcSHA512.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\md5.c

"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\st.c

"$(INTDIR)\st.obj"	"$(INTDIR)\st.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcUnityArchivers.c

"$(INTDIR)\dkcUnityArchivers.obj"	"$(INTDIR)\dkcUnityArchivers.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcBlockSort.c

"$(INTDIR)\dkcBlockSort.obj"	"$(INTDIR)\dkcBlockSort.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcLZSS.c

"$(INTDIR)\dkcLZSS.obj"	"$(INTDIR)\dkcLZSS.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcLZW.c

"$(INTDIR)\dkcLZW.obj"	"$(INTDIR)\dkcLZW.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcRLE.c

"$(INTDIR)\dkcRLE.obj"	"$(INTDIR)\dkcRLE.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcRijndael.c

"$(INTDIR)\dkcRijndael.obj"	"$(INTDIR)\dkcRijndael.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=".\rijndael-alg-fst.c"

"$(INTDIR)\rijndael-alg-fst.obj"	"$(INTDIR)\rijndael-alg-fst.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=".\rijndael-api-fst.c"

"$(INTDIR)\rijndael-api-fst.obj"	"$(INTDIR)\rijndael-api-fst.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcArcfour.c

"$(INTDIR)\dkcArcfour.obj"	"$(INTDIR)\dkcArcfour.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcBlowfish.c

"$(INTDIR)\dkcBlowfish.obj"	"$(INTDIR)\dkcBlowfish.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcCryptograph.c

"$(INTDIR)\dkcCryptograph.obj"	"$(INTDIR)\dkcCryptograph.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcHC256.c

"$(INTDIR)\dkcHC256.obj"	"$(INTDIR)\dkcHC256.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcHMAC.c

"$(INTDIR)\dkcHMAC.obj"	"$(INTDIR)\dkcHMAC.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSemiRealRandom.c

"$(INTDIR)\dkcSemiRealRandom.obj"	"$(INTDIR)\dkcSemiRealRandom.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSNOW20.c

"$(INTDIR)\dkcSNOW20.obj"	"$(INTDIR)\dkcSNOW20.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcVernam.c

"$(INTDIR)\dkcVernam.obj"	"$(INTDIR)\dkcVernam.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkc2Tree.c

"$(INTDIR)\dkc2Tree.obj"	"$(INTDIR)\dkc2Tree.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcArrayOneByOne.c

"$(INTDIR)\dkcArrayOneByOne.obj"	"$(INTDIR)\dkcArrayOneByOne.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcBuffer.c

"$(INTDIR)\dkcBuffer.obj"	"$(INTDIR)\dkcBuffer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcCircularMemoryStream.c

"$(INTDIR)\dkcCircularMemoryStream.obj"	"$(INTDIR)\dkcCircularMemoryStream.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcCircularStream.c

!IF  "$(CFG)" == "dkutil_c - Win32 Release"

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 Debug"


"$(INTDIR)\dkcCircularStream.obj"	"$(INTDIR)\dkcCircularStream.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_c - Win32 DebugMT"


"$(INTDIR)\dkcCircularStream.obj"	"$(INTDIR)\dkcCircularStream.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_c - Win32 ReleaseMT"

!ENDIF 

SOURCE=.\dkcDeque.c

"$(INTDIR)\dkcDeque.obj"	"$(INTDIR)\dkcDeque.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcMemoryStream.c

"$(INTDIR)\dkcMemoryStream.obj"	"$(INTDIR)\dkcMemoryStream.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcQueue.c

"$(INTDIR)\dkcQueue.obj"	"$(INTDIR)\dkcQueue.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcRedBlackTree.c

"$(INTDIR)\dkcRedBlackTree.obj"	"$(INTDIR)\dkcRedBlackTree.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcSingleList.c

"$(INTDIR)\dkcSingleList.obj"	"$(INTDIR)\dkcSingleList.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcStack.c

"$(INTDIR)\dkcStack.obj"	"$(INTDIR)\dkcStack.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcString.c

"$(INTDIR)\dkcString.obj"	"$(INTDIR)\dkcString.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcDCF.c

"$(INTDIR)\dkcDCF.obj"	"$(INTDIR)\dkcDCF.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcDCF01.c

"$(INTDIR)\dkcDCF01.obj"	"$(INTDIR)\dkcDCF01.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcTelecomConfirmFile.c

"$(INTDIR)\dkcTelecomConfirmFile.obj"	"$(INTDIR)\dkcTelecomConfirmFile.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dkcTelecomConfirmPacket.c

"$(INTDIR)\dkcTelecomConfirmPacket.obj"	"$(INTDIR)\dkcTelecomConfirmPacket.sbr" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "dkutil_c - Win32 Release"

"dkutil_cstd - Win32 Release" : 
   cd ".\dkutil_cstd"
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_cstd.mak CFG="dkutil_cstd - Win32 Release" 
   cd ".."

"dkutil_cstd - Win32 ReleaseCLEAN" : 
   cd ".\dkutil_cstd"
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_cstd.mak CFG="dkutil_cstd - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 Debug"

"dkutil_cstd - Win32 Debug" : 
   cd ".\dkutil_cstd"
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_cstd.mak CFG="dkutil_cstd - Win32 Debug" 
   cd ".."

"dkutil_cstd - Win32 DebugCLEAN" : 
   cd ".\dkutil_cstd"
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_cstd.mak CFG="dkutil_cstd - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 DebugMT"

"dkutil_cstd - Win32 DebugMT" : 
   cd ".\dkutil_cstd"
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_cstd.mak CFG="dkutil_cstd - Win32 DebugMT" 
   cd ".."

"dkutil_cstd - Win32 DebugMTCLEAN" : 
   cd ".\dkutil_cstd"
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_cstd.mak CFG="dkutil_cstd - Win32 DebugMT" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 ReleaseMT"

"dkutil_cstd - Win32 ReleaseMT" : 
   cd ".\dkutil_cstd"
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_cstd.mak CFG="dkutil_cstd - Win32 ReleaseMT" 
   cd ".."

"dkutil_cstd - Win32 ReleaseMTCLEAN" : 
   cd ".\dkutil_cstd"
   $(MAKE) /$(MAKEFLAGS) /F .\dkutil_cstd.mak CFG="dkutil_cstd - Win32 ReleaseMT" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 


!ENDIF 

