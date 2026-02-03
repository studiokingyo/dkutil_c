# Microsoft Developer Studio Generated NMAKE File, Based on dkutil_cstd.dsp
!IF "$(CFG)" == ""
CFG=dkutil_cstd - Win32 DebugMT
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの dkutil_cstd - Win32 DebugMT を設定します。
!ENDIF 

!IF "$(CFG)" != "dkutil_cstd - Win32 Release" && "$(CFG)" != "dkutil_cstd - Win32 Debug" && "$(CFG)" != "dkutil_cstd - Win32 DebugMT" && "$(CFG)" != "dkutil_cstd - Win32 ReleaseMT"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "dkutil_cstd.mak" CFG="dkutil_cstd - Win32 DebugMT"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "dkutil_cstd - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkutil_cstd - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkutil_cstd - Win32 DebugMT" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkutil_cstd - Win32 ReleaseMT" ("Win32 (x86) Static Library" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dkutil_cstd - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\dkutil_cstd.lib"


CLEAN :
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\stdlib.obj"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\strings.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\x86.obj"
	-@erase "$(OUTDIR)\dkutil_cstd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STRICT" /D "_WIN32" /Fp"$(INTDIR)\dkutil_cstd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_cstd.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dkutil_cstd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\stdlib.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\strings.obj" \
	"$(INTDIR)\x86.obj"

"$(OUTDIR)\dkutil_cstd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\dkutil_cstd.lib"
   copylib.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\dkutil_cstdd.lib" "$(OUTDIR)\dkutil_cstd.bsc"


CLEAN :
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\malloc.sbr"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\math.sbr"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\memory.sbr"
	-@erase "$(INTDIR)\stdlib.obj"
	-@erase "$(INTDIR)\stdlib.sbr"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\string.sbr"
	-@erase "$(INTDIR)\strings.obj"
	-@erase "$(INTDIR)\strings.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\x86.obj"
	-@erase "$(INTDIR)\x86.sbr"
	-@erase "$(OUTDIR)\dkutil_cstd.bsc"
	-@erase "$(OUTDIR)\dkutil_cstdd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STRICT" /D "_WIN32" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dkutil_cstd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_cstd.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\malloc.sbr" \
	"$(INTDIR)\math.sbr" \
	"$(INTDIR)\memory.sbr" \
	"$(INTDIR)\stdlib.sbr" \
	"$(INTDIR)\string.sbr" \
	"$(INTDIR)\strings.sbr" \
	"$(INTDIR)\x86.sbr"

"$(OUTDIR)\dkutil_cstd.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dkutil_cstdd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\stdlib.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\strings.obj" \
	"$(INTDIR)\x86.obj"

"$(OUTDIR)\dkutil_cstdd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\dkutil_cstdd.lib" "$(OUTDIR)\dkutil_cstd.bsc"
   copylib.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 DebugMT"

OUTDIR=.\dkutil_cstd___Win32_DebugMT
INTDIR=.\dkutil_cstd___Win32_DebugMT
# Begin Custom Macros
OutDir=.\dkutil_cstd___Win32_DebugMT
# End Custom Macros

ALL : ".\Debug\dkutil_cstdMTd.lib" "$(OUTDIR)\dkutil_cstd.bsc"


CLEAN :
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\malloc.sbr"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\math.sbr"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\memory.sbr"
	-@erase "$(INTDIR)\stdlib.obj"
	-@erase "$(INTDIR)\stdlib.sbr"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\string.sbr"
	-@erase "$(INTDIR)\strings.obj"
	-@erase "$(INTDIR)\strings.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\x86.obj"
	-@erase "$(INTDIR)\x86.sbr"
	-@erase "$(OUTDIR)\dkutil_cstd.bsc"
	-@erase ".\Debug\dkutil_cstdMTd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STRICT" /D "_WIN32" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dkutil_cstd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_cstd.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\malloc.sbr" \
	"$(INTDIR)\math.sbr" \
	"$(INTDIR)\memory.sbr" \
	"$(INTDIR)\stdlib.sbr" \
	"$(INTDIR)\string.sbr" \
	"$(INTDIR)\strings.sbr" \
	"$(INTDIR)\x86.sbr"

"$(OUTDIR)\dkutil_cstd.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Debug\dkutil_cstdMTd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\stdlib.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\strings.obj" \
	"$(INTDIR)\x86.obj"

".\Debug\dkutil_cstdMTd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\dkutil_cstd___Win32_DebugMT
# End Custom Macros

$(DS_POSTBUILD_DEP) : ".\Debug\dkutil_cstdMTd.lib" "$(OUTDIR)\dkutil_cstd.bsc"
   copylib.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 ReleaseMT"

OUTDIR=.\dkutil_cstd___Win32_ReleaseMT
INTDIR=.\dkutil_cstd___Win32_ReleaseMT
# Begin Custom Macros
OutDir=.\dkutil_cstd___Win32_ReleaseMT
# End Custom Macros

ALL : ".\Release\dkutil_cstdMT.lib" "$(OUTDIR)\dkutil_cstd.bsc"


CLEAN :
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\malloc.sbr"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\math.sbr"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\memory.sbr"
	-@erase "$(INTDIR)\stdlib.obj"
	-@erase "$(INTDIR)\stdlib.sbr"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\string.sbr"
	-@erase "$(INTDIR)\strings.obj"
	-@erase "$(INTDIR)\strings.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\x86.obj"
	-@erase "$(INTDIR)\x86.sbr"
	-@erase "$(OUTDIR)\dkutil_cstd.bsc"
	-@erase ".\Release\dkutil_cstdMT.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STRICT" /D "_WIN32" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dkutil_cstd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dkutil_cstd.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\malloc.sbr" \
	"$(INTDIR)\math.sbr" \
	"$(INTDIR)\memory.sbr" \
	"$(INTDIR)\stdlib.sbr" \
	"$(INTDIR)\string.sbr" \
	"$(INTDIR)\strings.sbr" \
	"$(INTDIR)\x86.sbr"

"$(OUTDIR)\dkutil_cstd.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Release\dkutil_cstdMT.lib" 
LIB32_OBJS= \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\stdlib.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\strings.obj" \
	"$(INTDIR)\x86.obj"

".\Release\dkutil_cstdMT.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\dkutil_cstd___Win32_ReleaseMT
# End Custom Macros

$(DS_POSTBUILD_DEP) : ".\Release\dkutil_cstdMT.lib" "$(OUTDIR)\dkutil_cstd.bsc"
   copylib.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dkutil_cstd.dep")
!INCLUDE "dkutil_cstd.dep"
!ELSE 
!MESSAGE Warning: cannot find "dkutil_cstd.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dkutil_cstd - Win32 Release" || "$(CFG)" == "dkutil_cstd - Win32 Debug" || "$(CFG)" == "dkutil_cstd - Win32 DebugMT" || "$(CFG)" == "dkutil_cstd - Win32 ReleaseMT"
SOURCE=.\malloc.c

!IF  "$(CFG)" == "dkutil_cstd - Win32 Release"


"$(INTDIR)\malloc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 Debug"


"$(INTDIR)\malloc.obj"	"$(INTDIR)\malloc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 DebugMT"


"$(INTDIR)\malloc.obj"	"$(INTDIR)\malloc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 ReleaseMT"


"$(INTDIR)\malloc.obj"	"$(INTDIR)\malloc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\math.c

!IF  "$(CFG)" == "dkutil_cstd - Win32 Release"


"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 Debug"


"$(INTDIR)\math.obj"	"$(INTDIR)\math.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 DebugMT"


"$(INTDIR)\math.obj"	"$(INTDIR)\math.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 ReleaseMT"


"$(INTDIR)\math.obj"	"$(INTDIR)\math.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\memory.c

!IF  "$(CFG)" == "dkutil_cstd - Win32 Release"


"$(INTDIR)\memory.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 Debug"


"$(INTDIR)\memory.obj"	"$(INTDIR)\memory.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 DebugMT"


"$(INTDIR)\memory.obj"	"$(INTDIR)\memory.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 ReleaseMT"


"$(INTDIR)\memory.obj"	"$(INTDIR)\memory.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stdlib.c

!IF  "$(CFG)" == "dkutil_cstd - Win32 Release"


"$(INTDIR)\stdlib.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 Debug"


"$(INTDIR)\stdlib.obj"	"$(INTDIR)\stdlib.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 DebugMT"


"$(INTDIR)\stdlib.obj"	"$(INTDIR)\stdlib.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 ReleaseMT"


"$(INTDIR)\stdlib.obj"	"$(INTDIR)\stdlib.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\string.c

!IF  "$(CFG)" == "dkutil_cstd - Win32 Release"


"$(INTDIR)\string.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 Debug"


"$(INTDIR)\string.obj"	"$(INTDIR)\string.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 DebugMT"


"$(INTDIR)\string.obj"	"$(INTDIR)\string.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 ReleaseMT"


"$(INTDIR)\string.obj"	"$(INTDIR)\string.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\strings.c

!IF  "$(CFG)" == "dkutil_cstd - Win32 Release"


"$(INTDIR)\strings.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 Debug"


"$(INTDIR)\strings.obj"	"$(INTDIR)\strings.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 DebugMT"


"$(INTDIR)\strings.obj"	"$(INTDIR)\strings.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 ReleaseMT"


"$(INTDIR)\strings.obj"	"$(INTDIR)\strings.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\x86.c

!IF  "$(CFG)" == "dkutil_cstd - Win32 Release"


"$(INTDIR)\x86.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 Debug"


"$(INTDIR)\x86.obj"	"$(INTDIR)\x86.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 DebugMT"


"$(INTDIR)\x86.obj"	"$(INTDIR)\x86.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dkutil_cstd - Win32 ReleaseMT"


"$(INTDIR)\x86.obj"	"$(INTDIR)\x86.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

