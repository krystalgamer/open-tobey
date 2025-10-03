# Microsoft Developer Studio Generated NMAKE File, Based on tobey.dsp
!IF "$(CFG)" == ""
CFG=tobey - Win32 Debug
!MESSAGE No configuration specified. Defaulting to tobey - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "tobey - Win32 Release" && "$(CFG)" != "tobey - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tobey.mak" CFG="tobey - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tobey - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tobey - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tobey - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\tobey.dll"


CLEAN :
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\my_assertions.obj"
	-@erase "$(INTDIR)\pc_timer.obj"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\script_object.obj"
	-@erase "$(INTDIR)\signal.obj"
	-@erase "$(INTDIR)\singleton.obj"
	-@erase "$(INTDIR)\so_data_block.obj"
	-@erase "$(INTDIR)\stringx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vm_stack.obj"
	-@erase "$(INTDIR)\vm_thread.obj"
	-@erase "$(INTDIR)\w32_archalloc.obj"
	-@erase "$(INTDIR)\w32_errmsg.obj"
	-@erase "$(INTDIR)\w32_file.obj"
	-@erase "$(INTDIR)\wds.obj"
	-@erase "$(INTDIR)\x86_math.obj"
	-@erase "$(OUTDIR)\tobey.dll"
	-@erase "$(OUTDIR)\tobey.exp"
	-@erase "$(OUTDIR)\tobey.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TOBEY_EXPORTS" /D "BUILD_BOOTABLE" /D "REGIONCULL" /D "TARGET_PC" /Fp"$(INTDIR)\tobey.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tobey.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\tobey.pdb" /machine:I386 /out:"$(OUTDIR)\tobey.dll" /implib:"$(OUTDIR)\tobey.lib" 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\my_assertions.obj" \
	"$(INTDIR)\pc_timer.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\script_object.obj" \
	"$(INTDIR)\signal.obj" \
	"$(INTDIR)\singleton.obj" \
	"$(INTDIR)\stringx.obj" \
	"$(INTDIR)\vm_stack.obj" \
	"$(INTDIR)\vm_thread.obj" \
	"$(INTDIR)\w32_errmsg.obj" \
	"$(INTDIR)\w32_file.obj" \
	"$(INTDIR)\wds.obj" \
	"$(INTDIR)\x86_math.obj" \
	"$(INTDIR)\so_data_block.obj" \
	"$(INTDIR)\w32_archalloc.obj"

"$(OUTDIR)\tobey.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tobey - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\tobey.dll"


CLEAN :
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\my_assertions.obj"
	-@erase "$(INTDIR)\pc_timer.obj"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\script_object.obj"
	-@erase "$(INTDIR)\signal.obj"
	-@erase "$(INTDIR)\singleton.obj"
	-@erase "$(INTDIR)\so_data_block.obj"
	-@erase "$(INTDIR)\stringx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vm_stack.obj"
	-@erase "$(INTDIR)\vm_thread.obj"
	-@erase "$(INTDIR)\w32_archalloc.obj"
	-@erase "$(INTDIR)\w32_errmsg.obj"
	-@erase "$(INTDIR)\w32_file.obj"
	-@erase "$(INTDIR)\wds.obj"
	-@erase "$(INTDIR)\x86_math.obj"
	-@erase "$(OUTDIR)\tobey.dll"
	-@erase "$(OUTDIR)\tobey.exp"
	-@erase "$(OUTDIR)\tobey.ilk"
	-@erase "$(OUTDIR)\tobey.lib"
	-@erase "$(OUTDIR)\tobey.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TOBEY_EXPORTS" /Fp"$(INTDIR)\tobey.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tobey.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\tobey.pdb" /debug /machine:I386 /out:"$(OUTDIR)\tobey.dll" /implib:"$(OUTDIR)\tobey.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\my_assertions.obj" \
	"$(INTDIR)\pc_timer.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\script_object.obj" \
	"$(INTDIR)\signal.obj" \
	"$(INTDIR)\singleton.obj" \
	"$(INTDIR)\stringx.obj" \
	"$(INTDIR)\vm_stack.obj" \
	"$(INTDIR)\vm_thread.obj" \
	"$(INTDIR)\w32_errmsg.obj" \
	"$(INTDIR)\w32_file.obj" \
	"$(INTDIR)\wds.obj" \
	"$(INTDIR)\x86_math.obj" \
	"$(INTDIR)\so_data_block.obj" \
	"$(INTDIR)\w32_archalloc.obj"

"$(OUTDIR)\tobey.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("tobey.dep")
!INCLUDE "tobey.dep"
!ELSE 
!MESSAGE Warning: cannot find "tobey.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tobey - Win32 Release" || "$(CFG)" == "tobey - Win32 Debug"
SOURCE=.\SpideyTM\SRC\app.cpp

"$(INTDIR)\app.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dllmain.cpp

"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SpideyTM\SRC\global.cpp

"$(INTDIR)\global.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\my_assertions.cpp

"$(INTDIR)\my_assertions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\HWOSPC\pc_timer.cpp

"$(INTDIR)\pc_timer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\region.cpp

"$(INTDIR)\region.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\script_object.cpp

"$(INTDIR)\script_object.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\signal.cpp

"$(INTDIR)\signal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\singleton.cpp

"$(INTDIR)\singleton.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\so_data_block.cpp

"$(INTDIR)\so_data_block.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\stringx.cpp

"$(INTDIR)\stringx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\vm_stack.cpp

"$(INTDIR)\vm_stack.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\vm_thread.cpp

"$(INTDIR)\vm_thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\HWOSPC\w32_archalloc.cpp

"$(INTDIR)\w32_archalloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\HWOSPC\w32_errmsg.cpp

"$(INTDIR)\w32_errmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\HWOSPC\w32_file.cpp

"$(INTDIR)\w32_file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\wds.cpp

"$(INTDIR)\wds.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\HWOSPC\x86_math.cpp

"$(INTDIR)\x86_math.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

