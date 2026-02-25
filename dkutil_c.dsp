# Microsoft Developer Studio Project File - Name="dkutil_c" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** �ҏW���Ȃ��ł������� **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dkutil_c - Win32 DebugMT
!MESSAGE ����͗L����Ҳ�̧�قł͂���܂���B ������ۼު�Ă�����ނ��邽�߂ɂ� NMAKE ���g�p���Ă��������B
!MESSAGE [Ҳ�̧�ق̴���߰�] ����ނ��g�p���Ď��s���Ă�������
!MESSAGE 
!MESSAGE NMAKE /f "dkutil_c.mak".
!MESSAGE 
!MESSAGE NMAKE �̎��s���ɍ\�����w��ł��܂�
!MESSAGE ����� ײݏ��ϸۂ̐ݒ���`���܂��B��:
!MESSAGE 
!MESSAGE NMAKE /f "dkutil_c.mak" CFG="dkutil_c - Win32 DebugMT"
!MESSAGE 
!MESSAGE �I���\������� Ӱ��:
!MESSAGE 
!MESSAGE "dkutil_c - Win32 Release" ("Win32 (x86) Static Library" �p)
!MESSAGE "dkutil_c - Win32 Debug" ("Win32 (x86) Static Library" �p)
!MESSAGE "dkutil_c - Win32 DebugMT" ("Win32 (x86) Static Library" �p)
!MESSAGE "dkutil_c - Win32 ReleaseMT" ("Win32 (x86) Static Library" �p)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dkutil_c - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copylib.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\dkutil_cd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copylib.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 DebugMT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dkutil_c___Win32_DebugMT"
# PROP BASE Intermediate_Dir "dkutil_c___Win32_DebugMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMT"
# PROP Intermediate_Dir "DebugMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Debug\dkutil_cd.lib"
# ADD LIB32 /nologo /out:"DebugMT\dkutil_cMTd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=���C�u�������R�s�[���܂��B
PostBuild_Cmds=copylib.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 ReleaseMT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dkutil_c___Win32_ReleaseMT"
# PROP BASE Intermediate_Dir "dkutil_c___Win32_ReleaseMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMT"
# PROP Intermediate_Dir "ReleaseMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ReleaseMT\dkutil_cMT.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=���C�u�������R�s�[���܂��B
PostBuild_Cmds=copylib.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "dkutil_c - Win32 Release"
# Name "dkutil_c - Win32 Debug"
# Name "dkutil_c - Win32 DebugMT"
# Name "dkutil_c - Win32 ReleaseMT"
# Begin Group "common"

# PROP Default_Filter "c;h;"
# Begin Group "filesystem"

# PROP Default_Filter "c;h;"
# Begin Source File

SOURCE=.\dkcFileSystems.h
# End Source File
# Begin Source File

SOURCE=.\dkcGenericFileSystem.c
# End Source File
# Begin Source File

SOURCE=.\dkcGenericFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcSafeFileSystem.c
# End Source File
# Begin Source File

SOURCE=.\dkcSafeFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcSJISFileSystem.c
# End Source File
# Begin Source File

SOURCE=.\dkcSJISFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcStream.c
# End Source File
# Begin Source File

SOURCE=.\dkcStream.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\dkc_misc.h
# End Source File
# Begin Source File

SOURCE=.\dkcBit.c
# End Source File
# Begin Source File

SOURCE=.\dkcBit.h
# End Source File
# Begin Source File

SOURCE=.\dkcDefined.h
# End Source File
# Begin Source File

SOURCE=.\dkcDLL.c
# End Source File
# Begin Source File

SOURCE=.\dkcDLL.h
# End Source File
# Begin Source File

SOURCE=.\dkcLua.c
# End Source File
# Begin Source File

SOURCE=.\dkcLua.h
# End Source File
# Begin Source File

SOURCE=.\dkcMath.c
# End Source File
# Begin Source File

SOURCE=.\dkcMath.h
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryPool.c
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryPool.h
# End Source File
# Begin Source File

SOURCE=.\dkcOS.c
# End Source File
# Begin Source File

SOURCE=.\dkcOS.h
# End Source File
# Begin Source File

SOURCE=.\dkcOSIndependent.c
# End Source File
# Begin Source File

SOURCE=.\dkcOSIndependent.h
# End Source File
# Begin Source File

SOURCE=.\dkcRegex.c
# End Source File
# Begin Source File

SOURCE=.\dkcRegex.h
# End Source File
# Begin Source File

SOURCE=.\dkcSort.c
# End Source File
# Begin Source File

SOURCE=.\dkcSort.h
# End Source File
# Begin Source File

SOURCE=.\dkcStdio.c
# End Source File
# Begin Source File

SOURCE=.\dkcStdio.h
# End Source File
# Begin Source File

SOURCE=.\dkcTaskEngine.c
# End Source File
# Begin Source File

SOURCE=.\dkcTaskEngine.h
# End Source File
# Begin Source File

SOURCE=.\dkcThread.c
# End Source File
# Begin Source File

SOURCE=.\dkcThread.h
# End Source File
# Begin Source File

SOURCE=.\dkcThreadLock.c
# End Source File
# Begin Source File

SOURCE=.\dkcThreadLock.h
# End Source File
# Begin Source File

SOURCE=.\dkcUniqueID.c
# End Source File
# Begin Source File

SOURCE=.\dkcUniqueID.h
# End Source File
# Begin Source File

SOURCE=.\vc_asm_misc.h
# End Source File
# End Group
# Begin Group "hash"

# PROP Default_Filter "c;h;"
# Begin Source File

SOURCE=.\dkcBLAKE2.c
# End Source File
# Begin Source File

SOURCE=.\dkcBLAKE2.h
# End Source File
# Begin Source File

SOURCE=.\dkcBLAKE256.c
# End Source File
# Begin Source File

SOURCE=.\dkcBLAKE256.h
# End Source File
# Begin Source File

SOURCE=.\dkcBLAKE3.c
# End Source File
# Begin Source File

SOURCE=.\dkcBLAKE3.h
# End Source File
# Begin Source File

SOURCE=.\dkcBLAKE512.c
# End Source File
# Begin Source File

SOURCE=.\dkcBLAKE512.h
# End Source File
# Begin Source File

SOURCE=.\dkcCRC.c
# End Source File
# Begin Source File

SOURCE=.\dkcCRC.h
# End Source File
# Begin Source File

SOURCE=.\dkcHash.c
# End Source File
# Begin Source File

SOURCE=.\dkcHash.h
# End Source File
# Begin Source File

SOURCE=.\dkcKeccak.c
# End Source File
# Begin Source File

SOURCE=.\dkcKeccak.h
# End Source File
# Begin Source File

SOURCE=.\dkcLightHash.c
# End Source File
# Begin Source File

SOURCE=.\dkcLightHash.h
# End Source File
# Begin Source File

SOURCE=.\dkcMD2.c
# End Source File
# Begin Source File

SOURCE=.\dkcMD2.h
# End Source File
# Begin Source File

SOURCE=.\dkcMD4.c
# End Source File
# Begin Source File

SOURCE=.\dkcMD4.h
# End Source File
# Begin Source File

SOURCE=.\dkcMD5.c
# End Source File
# Begin Source File

SOURCE=.\dkcMD5.h
# End Source File
# Begin Source File

SOURCE=.\dkcMessageDigest.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA1.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA1.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA256.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA256.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA3.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA3.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA384.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA384.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA512.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA512.h
# End Source File
# Begin Source File

SOURCE=.\md5.c
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\md5_vc_mmx.h
# End Source File
# Begin Source File

SOURCE=.\md_misc.h
# End Source File
# Begin Source File

SOURCE=.\st.c
# End Source File
# Begin Source File

SOURCE=.\st.h
# End Source File
# End Group
# Begin Group "compression"

# PROP Default_Filter "c;h;"
# Begin Group "win32"

# PROP Default_Filter "c;h;"
# Begin Source File

SOURCE=.\dkcUnityArchivers.c
# End Source File
# Begin Source File

SOURCE=.\dkcUnityArchivers.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\dkcBlockSort.c
# End Source File
# Begin Source File

SOURCE=.\dkcBlockSort.h
# End Source File
# Begin Source File

SOURCE=.\dkcLZSS.c
# End Source File
# Begin Source File

SOURCE=.\dkcLZSS.h
# End Source File
# Begin Source File

SOURCE=.\dkcLZW.c
# End Source File
# Begin Source File

SOURCE=.\dkcLZW.h
# End Source File
# Begin Source File

SOURCE=.\dkcRLE.c
# End Source File
# Begin Source File

SOURCE=.\dkcRLE.h
# End Source File
# Begin Source File

SOURCE=.\dkcHuffman.c
# End Source File
# Begin Source File

SOURCE=.\dkcHuffman.h
# End Source File
# Begin Source File

SOURCE=.\dkcRangeCoder.c
# End Source File
# Begin Source File

SOURCE=.\dkcRangeCoder.h
# End Source File
# End Group
# Begin Group "cryptograph"

# PROP Default_Filter "c;h;"
# Begin Group "rijndael"

# PROP Default_Filter "c;h;"
# Begin Source File

SOURCE=.\dkcRijndael.c
# End Source File
# Begin Source File

SOURCE=.\dkcRijndael.h
# End Source File
# Begin Source File

SOURCE=".\rijndael-alg-fst.c"
# End Source File
# Begin Source File

SOURCE=".\rijndael-alg-fst.h"
# End Source File
# Begin Source File

SOURCE=".\rijndael-api-fst.c"
# End Source File
# Begin Source File

SOURCE=".\rijndael-api-fst.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\dkcArcfour.c
# End Source File
# Begin Source File

SOURCE=.\dkcArcfour.h
# End Source File
# Begin Source File

SOURCE=.\dkcArgon2.c
# End Source File
# Begin Source File

SOURCE=.\dkcArgon2.h
# End Source File
# Begin Source File

SOURCE=.\dkcBcrypt.c
# End Source File
# Begin Source File

SOURCE=.\dkcBcrypt.h
# End Source File
# Begin Source File

SOURCE=.\dkcBlowfish.c
# End Source File
# Begin Source File

SOURCE=.\dkcBlowfish.h
# End Source File
# Begin Source File

SOURCE=.\dkcCamellia.c
# End Source File
# Begin Source File

SOURCE=.\dkcCamellia.h
# End Source File
# Begin Source File

SOURCE=.\dkcChaCha20.c
# End Source File
# Begin Source File

SOURCE=.\dkcChaCha20.h
# End Source File
# Begin Source File

SOURCE=.\dkcCryptograph.c
# End Source File
# Begin Source File

SOURCE=.\dkcCryptograph.h
# End Source File
# Begin Source File

SOURCE=.\dkcHC256.c
# End Source File
# Begin Source File

SOURCE=.\dkcHC256.h
# End Source File
# Begin Source File

SOURCE=.\dkcHMAC.c
# End Source File
# Begin Source File

SOURCE=.\dkcHMAC.h
# End Source File
# Begin Source File

SOURCE=.\dkcKCipher2.c
# End Source File
# Begin Source File

SOURCE=.\dkcKCipher2.h
# End Source File
# Begin Source File

SOURCE=.\dkcPoly1305.c
# End Source File
# Begin Source File

SOURCE=.\dkcPoly1305.h
# End Source File
# Begin Source File

SOURCE=.\dkcSemiRealRandom.c
# End Source File
# Begin Source File

SOURCE=.\dkcSemiRealRandom.h
# End Source File
# Begin Source File

SOURCE=.\dkcSNOW20.c
# End Source File
# Begin Source File

SOURCE=.\dkcSNOW20.h
# End Source File
# Begin Source File

SOURCE=.\dkcThreefish.c
# End Source File
# Begin Source File

SOURCE=.\dkcThreefish.h
# End Source File
# Begin Source File

SOURCE=.\dkcTwofish.c
# End Source File
# Begin Source File

SOURCE=.\dkcTwofish.h
# End Source File
# Begin Source File

SOURCE=.\dkcVernam.c
# End Source File
# Begin Source File

SOURCE=.\dkcVernam.h
# End Source File
# End Group
# Begin Group "container"

# PROP Default_Filter "c;h;"
# Begin Source File

SOURCE=.\dkc2Tree.c
# End Source File
# Begin Source File

SOURCE=.\dkc2Tree.h
# End Source File
# Begin Source File

SOURCE=.\dkcAESGCM.c
# End Source File
# Begin Source File

SOURCE=.\dkcAESGCM.h
# End Source File
# Begin Source File

SOURCE=.\dkcArrayOneByOne.c
# End Source File
# Begin Source File

SOURCE=.\dkcArrayOneByOne.h
# End Source File
# Begin Source File

SOURCE=.\dkcAVLTree.c
# End Source File
# Begin Source File

SOURCE=.\dkcAVLTree.h
# End Source File
# Begin Source File

SOURCE=.\dkcBase64.c
# End Source File
# Begin Source File

SOURCE=.\dkcBase64.h
# End Source File
# Begin Source File

SOURCE=.\dkcBigInteger.c
# End Source File
# Begin Source File

SOURCE=.\dkcBigInteger.h
# End Source File
# Begin Source File

SOURCE=.\dkcBloomFilter.c
# End Source File
# Begin Source File

SOURCE=.\dkcBloomFilter.h
# End Source File
# Begin Source File

SOURCE=.\dkcBPlusTree.c
# End Source File
# Begin Source File

SOURCE=.\dkcBPlusTree.h
# End Source File
# Begin Source File

SOURCE=.\dkcBTree.c
# End Source File
# Begin Source File

SOURCE=.\dkcBTree.h
# End Source File
# Begin Source File

SOURCE=.\dkcBuffer.c
# End Source File
# Begin Source File

SOURCE=.\dkcBuffer.h
# End Source File
# Begin Source File

SOURCE=.\dkcCircularMemoryStream.c
# End Source File
# Begin Source File

SOURCE=.\dkcCircularMemoryStream.h
# End Source File
# Begin Source File

SOURCE=.\dkcCircularStream.c

!IF  "$(CFG)" == "dkutil_c - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 Debug"

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 DebugMT"

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 ReleaseMT"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dkcCircularStream.h

!IF  "$(CFG)" == "dkutil_c - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 Debug"

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 DebugMT"

!ELSEIF  "$(CFG)" == "dkutil_c - Win32 ReleaseMT"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dkcCuckooHash.c
# End Source File
# Begin Source File

SOURCE=.\dkcCuckooHash.h
# End Source File
# Begin Source File

SOURCE=.\dkcDeque.c
# End Source File
# Begin Source File

SOURCE=.\dkcDeque.h
# End Source File
# Begin Source File

SOURCE=.\dkcDoubleList.c
# End Source File
# Begin Source File

SOURCE=.\dkcDoubleList.h
# End Source File
# Begin Source File

SOURCE=.\dkcFenwickTree.c
# End Source File
# Begin Source File

SOURCE=.\dkcFenwickTree.h
# End Source File
# Begin Source File

SOURCE=.\dkcFibonacciHeap.c
# End Source File
# Begin Source File

SOURCE=.\dkcFibonacciHeap.h
# End Source File
# Begin Source File

SOURCE=.\dkcHashMap.c
# End Source File
# Begin Source File

SOURCE=.\dkcHashMap.h
# End Source File
# Begin Source File

SOURCE=.\dkcHashMultiMap.c
# End Source File
# Begin Source File

SOURCE=.\dkcHashMultiMap.h
# End Source File
# Begin Source File

SOURCE=.\dkcHashMultiSet.c
# End Source File
# Begin Source File

SOURCE=.\dkcHashMultiSet.h
# End Source File
# Begin Source File

SOURCE=.\dkcHashSet.c
# End Source File
# Begin Source File

SOURCE=.\dkcHashSet.h
# End Source File
# Begin Source File

SOURCE=.\dkcHeap.c
# End Source File
# Begin Source File

SOURCE=.\dkcHeap.h
# End Source File
# Begin Source File

SOURCE=.\dkcLRUCache.c
# End Source File
# Begin Source File

SOURCE=.\dkcLRUCache.h
# End Source File
# Begin Source File

SOURCE=.\dkcLRUCache2.c
# End Source File
# Begin Source File

SOURCE=.\dkcLRUCache2.h
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryStream.c
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryStream.h
# End Source File
# Begin Source File

SOURCE=.\dkcMPH.c
# End Source File
# Begin Source File

SOURCE=.\dkcMPH.h
# End Source File
# Begin Source File

SOURCE=.\dkcNaryTree.c
# End Source File
# Begin Source File

SOURCE=.\dkcNaryTree.h
# End Source File
# Begin Source File

SOURCE=.\dkcPatricia.c
# End Source File
# Begin Source File

SOURCE=.\dkcPatricia.h
# End Source File
# Begin Source File

SOURCE=.\dkcQueue.c
# End Source File
# Begin Source File

SOURCE=.\dkcQueue.h
# End Source File
# Begin Source File

SOURCE=.\dkcRedBlackTree.c
# End Source File
# Begin Source File

SOURCE=.\dkcRedBlackTree.h
# End Source File
# Begin Source File

SOURCE=.\dkcRope.c
# End Source File
# Begin Source File

SOURCE=.\dkcRope.h
# End Source File
# Begin Source File

SOURCE=.\dkcScrypt.c
# End Source File
# Begin Source File

SOURCE=.\dkcScrypt.h
# End Source File
# Begin Source File

SOURCE=.\dkcSegmentTree.c
# End Source File
# Begin Source File

SOURCE=.\dkcSegmentTree.h
# End Source File
# Begin Source File

SOURCE=.\dkcSingleList.c
# End Source File
# Begin Source File

SOURCE=.\dkcSingleList.h
# End Source File
# Begin Source File

SOURCE=.\dkcSkipList.c
# End Source File
# Begin Source File

SOURCE=.\dkcSkipList.h
# End Source File
# Begin Source File

SOURCE=.\dkcSparseSet.c
# End Source File
# Begin Source File

SOURCE=.\dkcSparseSet.h
# End Source File
# Begin Source File

SOURCE=.\dkcSplayTree.c
# End Source File
# Begin Source File

SOURCE=.\dkcSplayTree.h
# End Source File
# Begin Source File

SOURCE=.\dkcStack.c
# End Source File
# Begin Source File

SOURCE=.\dkcStack.h
# End Source File
# Begin Source File

SOURCE=.\dkcString.c
# End Source File
# Begin Source File

SOURCE=.\dkcString.h
# End Source File
# Begin Source File

SOURCE=.\dkcSuffixArray.c
# End Source File
# Begin Source File

SOURCE=.\dkcSuffixArray.h
# End Source File
# Begin Source File

SOURCE=.\dkcTreap.c
# End Source File
# Begin Source File

SOURCE=.\dkcTreap.h
# End Source File
# Begin Source File

SOURCE=.\dkcUnionFind.c
# End Source File
# Begin Source File

SOURCE=.\dkcUnionFind.h
# End Source File
# End Group
# Begin Group "routine"

# PROP Default_Filter "c;h;"
# Begin Source File

SOURCE=.\dcf_misc.h
# End Source File
# Begin Source File

SOURCE=.\dkcDCF.c
# End Source File
# Begin Source File

SOURCE=.\dkcDCF.h
# End Source File
# Begin Source File

SOURCE=.\dkcDCF01.c
# End Source File
# Begin Source File

SOURCE=.\dkcDCF01.h
# End Source File
# Begin Source File

SOURCE=.\dkcTelecomConfirmFile.c
# End Source File
# Begin Source File

SOURCE=.\dkcTelecomConfirmFile.h
# End Source File
# Begin Source File

SOURCE=.\dkcTelecomConfirmPacket.c
# End Source File
# Begin Source File

SOURCE=.\dkcTelecomConfirmPacket.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\dkc.h
# End Source File
# End Target
# End Project
