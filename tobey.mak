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

!IF  "$(CFG)" == "tobey - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\tobey.dll"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\ai_interface.obj"
	-@erase "$(INTDIR)\ai_locomotion.obj"
	-@erase "$(INTDIR)\ai_polypath.obj"
	-@erase "$(INTDIR)\anim_flavor.obj"
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\billboard.obj"
	-@erase "$(INTDIR)\bone.obj"
	-@erase "$(INTDIR)\chunkfile.obj"
	-@erase "$(INTDIR)\colgeom.obj"
	-@erase "$(INTDIR)\collide.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\controller.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\damage_interface.obj"
	-@erase "$(INTDIR)\debugutil.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\entity.obj"
	-@erase "$(INTDIR)\entity_anim.obj"
	-@erase "$(INTDIR)\errorcontext.obj"
	-@erase "$(INTDIR)\file_manager.obj"
	-@erase "$(INTDIR)\filespec.obj"
	-@erase "$(INTDIR)\frame_info.obj"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\ini_parser.obj"
	-@erase "$(INTDIR)\light.obj"
	-@erase "$(INTDIR)\lightmgr.obj"
	-@erase "$(INTDIR)\link_interface.obj"
	-@erase "$(INTDIR)\maketree.obj"
	-@erase "$(INTDIR)\mustash.obj"
	-@erase "$(INTDIR)\my_assertions.obj"
	-@erase "$(INTDIR)\ngl_pc.obj"
	-@erase "$(INTDIR)\nsl_pc_emitter.obj"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\pc_algebra.obj"
	-@erase "$(INTDIR)\pc_audio.obj"
	-@erase "$(INTDIR)\pc_timer.obj"
	-@erase "$(INTDIR)\pmesh.obj"
	-@erase "$(INTDIR)\po.obj"
	-@erase "$(INTDIR)\po_anim.obj"
	-@erase "$(INTDIR)\pstring.obj"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\script_lib.obj"
	-@erase "$(INTDIR)\script_library_class.obj"
	-@erase "$(INTDIR)\script_object.obj"
	-@erase "$(INTDIR)\semaphores.obj"
	-@erase "$(INTDIR)\signal.obj"
	-@erase "$(INTDIR)\singleton.obj"
	-@erase "$(INTDIR)\so_data_block.obj"
	-@erase "$(INTDIR)\sound_group.obj"
	-@erase "$(INTDIR)\sound_interface.obj"
	-@erase "$(INTDIR)\stringx.obj"
	-@erase "$(INTDIR)\textfile.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\visrep.obj"
	-@erase "$(INTDIR)\vm_executable.obj"
	-@erase "$(INTDIR)\vm_stack.obj"
	-@erase "$(INTDIR)\vm_symbol.obj"
	-@erase "$(INTDIR)\vm_thread.obj"
	-@erase "$(INTDIR)\w32_archalloc.obj"
	-@erase "$(INTDIR)\w32_errmsg.obj"
	-@erase "$(INTDIR)\w32_file.obj"
	-@erase "$(INTDIR)\wds.obj"
	-@erase "$(INTDIR)\x86_math.obj"
	-@erase "$(INTDIR)\zip_filter.obj"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\tobey.dll"
	-@erase "$(OUTDIR)\tobey.exp"
	-@erase "$(OUTDIR)\tobey.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /Zi /O2 /I "SpideyTM\SRC\sgistl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TOBEY_EXPORTS" /D "BUILD_BOOTABLE" /D "REGIONCULL" /D "TARGET_PC" /Fp"$(INTDIR)\tobey.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tobey.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\tobey.pdb" /machine:I386 /out:"$(OUTDIR)\tobey.dll" /implib:"$(OUTDIR)\tobey.lib" 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\maketree.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\pc_audio.obj" \
	"$(INTDIR)\nsl_pc_emitter.obj" \
	"$(INTDIR)\ai_interface.obj" \
	"$(INTDIR)\ai_locomotion.obj" \
	"$(INTDIR)\ai_polypath.obj" \
	"$(INTDIR)\anim_flavor.obj" \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\billboard.obj" \
	"$(INTDIR)\bone.obj" \
	"$(INTDIR)\chunkfile.obj" \
	"$(INTDIR)\colgeom.obj" \
	"$(INTDIR)\collide.obj" \
	"$(INTDIR)\controller.obj" \
	"$(INTDIR)\damage_interface.obj" \
	"$(INTDIR)\debugutil.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\entity.obj" \
	"$(INTDIR)\entity_anim.obj" \
	"$(INTDIR)\errorcontext.obj" \
	"$(INTDIR)\file_manager.obj" \
	"$(INTDIR)\filespec.obj" \
	"$(INTDIR)\frame_info.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\ini_parser.obj" \
	"$(INTDIR)\light.obj" \
	"$(INTDIR)\lightmgr.obj" \
	"$(INTDIR)\link_interface.obj" \
	"$(INTDIR)\mustash.obj" \
	"$(INTDIR)\my_assertions.obj" \
	"$(INTDIR)\ngl_pc.obj" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\pc_algebra.obj" \
	"$(INTDIR)\pc_timer.obj" \
	"$(INTDIR)\pmesh.obj" \
	"$(INTDIR)\po.obj" \
	"$(INTDIR)\po_anim.obj" \
	"$(INTDIR)\pstring.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\script_lib.obj" \
	"$(INTDIR)\script_library_class.obj" \
	"$(INTDIR)\script_object.obj" \
	"$(INTDIR)\semaphores.obj" \
	"$(INTDIR)\signal.obj" \
	"$(INTDIR)\singleton.obj" \
	"$(INTDIR)\so_data_block.obj" \
	"$(INTDIR)\sound_group.obj" \
	"$(INTDIR)\sound_interface.obj" \
	"$(INTDIR)\stringx.obj" \
	"$(INTDIR)\textfile.obj" \
	"$(INTDIR)\visrep.obj" \
	"$(INTDIR)\vm_executable.obj" \
	"$(INTDIR)\vm_stack.obj" \
	"$(INTDIR)\vm_symbol.obj" \
	"$(INTDIR)\vm_thread.obj" \
	"$(INTDIR)\w32_archalloc.obj" \
	"$(INTDIR)\w32_errmsg.obj" \
	"$(INTDIR)\w32_file.obj" \
	"$(INTDIR)\wds.obj" \
	"$(INTDIR)\x86_math.obj" \
	"$(INTDIR)\zip_filter.obj"

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
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\ai_interface.obj"
	-@erase "$(INTDIR)\ai_locomotion.obj"
	-@erase "$(INTDIR)\ai_polypath.obj"
	-@erase "$(INTDIR)\anim_flavor.obj"
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\billboard.obj"
	-@erase "$(INTDIR)\bone.obj"
	-@erase "$(INTDIR)\chunkfile.obj"
	-@erase "$(INTDIR)\colgeom.obj"
	-@erase "$(INTDIR)\collide.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\controller.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\damage_interface.obj"
	-@erase "$(INTDIR)\debugutil.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\dllmain.obj"
	-@erase "$(INTDIR)\entity.obj"
	-@erase "$(INTDIR)\entity_anim.obj"
	-@erase "$(INTDIR)\errorcontext.obj"
	-@erase "$(INTDIR)\file_manager.obj"
	-@erase "$(INTDIR)\filespec.obj"
	-@erase "$(INTDIR)\frame_info.obj"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\gzio.obj"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\ini_parser.obj"
	-@erase "$(INTDIR)\light.obj"
	-@erase "$(INTDIR)\lightmgr.obj"
	-@erase "$(INTDIR)\link_interface.obj"
	-@erase "$(INTDIR)\maketree.obj"
	-@erase "$(INTDIR)\mustash.obj"
	-@erase "$(INTDIR)\my_assertions.obj"
	-@erase "$(INTDIR)\ngl_pc.obj"
	-@erase "$(INTDIR)\nsl_pc_emitter.obj"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\pc_algebra.obj"
	-@erase "$(INTDIR)\pc_audio.obj"
	-@erase "$(INTDIR)\pc_timer.obj"
	-@erase "$(INTDIR)\pmesh.obj"
	-@erase "$(INTDIR)\po.obj"
	-@erase "$(INTDIR)\po_anim.obj"
	-@erase "$(INTDIR)\pstring.obj"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\script_lib.obj"
	-@erase "$(INTDIR)\script_library_class.obj"
	-@erase "$(INTDIR)\script_object.obj"
	-@erase "$(INTDIR)\semaphores.obj"
	-@erase "$(INTDIR)\signal.obj"
	-@erase "$(INTDIR)\singleton.obj"
	-@erase "$(INTDIR)\so_data_block.obj"
	-@erase "$(INTDIR)\sound_group.obj"
	-@erase "$(INTDIR)\sound_interface.obj"
	-@erase "$(INTDIR)\stringx.obj"
	-@erase "$(INTDIR)\textfile.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\visrep.obj"
	-@erase "$(INTDIR)\vm_executable.obj"
	-@erase "$(INTDIR)\vm_stack.obj"
	-@erase "$(INTDIR)\vm_symbol.obj"
	-@erase "$(INTDIR)\vm_thread.obj"
	-@erase "$(INTDIR)\w32_archalloc.obj"
	-@erase "$(INTDIR)\w32_errmsg.obj"
	-@erase "$(INTDIR)\w32_file.obj"
	-@erase "$(INTDIR)\wds.obj"
	-@erase "$(INTDIR)\x86_math.obj"
	-@erase "$(INTDIR)\zip_filter.obj"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\tobey.dll"
	-@erase "$(OUTDIR)\tobey.exp"
	-@erase "$(OUTDIR)\tobey.ilk"
	-@erase "$(OUTDIR)\tobey.lib"
	-@erase "$(OUTDIR)\tobey.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TOBEY_EXPORTS" /Fp"$(INTDIR)\tobey.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tobey.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\tobey.pdb" /debug /machine:I386 /out:"$(OUTDIR)\tobey.dll" /implib:"$(OUTDIR)\tobey.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\gzio.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\maketree.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\uncompr.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\pc_audio.obj" \
	"$(INTDIR)\nsl_pc_emitter.obj" \
	"$(INTDIR)\ai_interface.obj" \
	"$(INTDIR)\ai_locomotion.obj" \
	"$(INTDIR)\ai_polypath.obj" \
	"$(INTDIR)\anim_flavor.obj" \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\billboard.obj" \
	"$(INTDIR)\bone.obj" \
	"$(INTDIR)\chunkfile.obj" \
	"$(INTDIR)\colgeom.obj" \
	"$(INTDIR)\collide.obj" \
	"$(INTDIR)\controller.obj" \
	"$(INTDIR)\damage_interface.obj" \
	"$(INTDIR)\debugutil.obj" \
	"$(INTDIR)\dllmain.obj" \
	"$(INTDIR)\entity.obj" \
	"$(INTDIR)\entity_anim.obj" \
	"$(INTDIR)\errorcontext.obj" \
	"$(INTDIR)\file_manager.obj" \
	"$(INTDIR)\filespec.obj" \
	"$(INTDIR)\frame_info.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\ini_parser.obj" \
	"$(INTDIR)\light.obj" \
	"$(INTDIR)\lightmgr.obj" \
	"$(INTDIR)\link_interface.obj" \
	"$(INTDIR)\mustash.obj" \
	"$(INTDIR)\my_assertions.obj" \
	"$(INTDIR)\ngl_pc.obj" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\pc_algebra.obj" \
	"$(INTDIR)\pc_timer.obj" \
	"$(INTDIR)\pmesh.obj" \
	"$(INTDIR)\po.obj" \
	"$(INTDIR)\po_anim.obj" \
	"$(INTDIR)\pstring.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\script_lib.obj" \
	"$(INTDIR)\script_library_class.obj" \
	"$(INTDIR)\script_object.obj" \
	"$(INTDIR)\semaphores.obj" \
	"$(INTDIR)\signal.obj" \
	"$(INTDIR)\singleton.obj" \
	"$(INTDIR)\so_data_block.obj" \
	"$(INTDIR)\sound_group.obj" \
	"$(INTDIR)\sound_interface.obj" \
	"$(INTDIR)\stringx.obj" \
	"$(INTDIR)\textfile.obj" \
	"$(INTDIR)\visrep.obj" \
	"$(INTDIR)\vm_executable.obj" \
	"$(INTDIR)\vm_stack.obj" \
	"$(INTDIR)\vm_symbol.obj" \
	"$(INTDIR)\vm_thread.obj" \
	"$(INTDIR)\w32_archalloc.obj" \
	"$(INTDIR)\w32_errmsg.obj" \
	"$(INTDIR)\w32_file.obj" \
	"$(INTDIR)\wds.obj" \
	"$(INTDIR)\x86_math.obj" \
	"$(INTDIR)\zip_filter.obj"

"$(OUTDIR)\tobey.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("tobey.dep")
!INCLUDE "tobey.dep"
!ELSE 
!MESSAGE Warning: cannot find "tobey.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tobey - Win32 Release" || "$(CFG)" == "tobey - Win32 Debug"
SOURCE=.\SpideyTM\SRC\zlib\adler32.c

"$(INTDIR)\adler32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\compress.c

"$(INTDIR)\compress.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\crc32.c

"$(INTDIR)\crc32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\deflate.c

"$(INTDIR)\deflate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\gzio.c

"$(INTDIR)\gzio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\infblock.c

"$(INTDIR)\infblock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\infcodes.c

"$(INTDIR)\infcodes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\inffast.c

"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\inflate.c

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\inftrees.c

"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\infutil.c

"$(INTDIR)\infutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\maketree.c

"$(INTDIR)\maketree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\trees.c

"$(INTDIR)\trees.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\uncompr.c

"$(INTDIR)\uncompr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\zlib\zutil.c

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\HWOSPC\pc_audio.cpp

"$(INTDIR)\pc_audio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\NSL\PC\nsl_pc_emitter.cpp

"$(INTDIR)\nsl_pc_emitter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\ai_interface.cpp

"$(INTDIR)\ai_interface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\ai_locomotion.cpp

"$(INTDIR)\ai_locomotion.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\ai_polypath.cpp

"$(INTDIR)\ai_polypath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\anim_flavor.cpp

"$(INTDIR)\anim_flavor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\app.cpp

"$(INTDIR)\app.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\billboard.cpp

"$(INTDIR)\billboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\bone.cpp

"$(INTDIR)\bone.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\chunkfile.cpp

"$(INTDIR)\chunkfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\colgeom.cpp

"$(INTDIR)\colgeom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\collide.cpp

"$(INTDIR)\collide.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\controller.cpp

"$(INTDIR)\controller.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\damage_interface.cpp

"$(INTDIR)\damage_interface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\debugutil.cpp

"$(INTDIR)\debugutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dllmain.cpp

"$(INTDIR)\dllmain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SpideyTM\SRC\entity.cpp

"$(INTDIR)\entity.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\entity_anim.cpp

"$(INTDIR)\entity_anim.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\errorcontext.cpp

"$(INTDIR)\errorcontext.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\file_manager.cpp

"$(INTDIR)\file_manager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\filespec.cpp

"$(INTDIR)\filespec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\frame_info.cpp

"$(INTDIR)\frame_info.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\game.cpp

"$(INTDIR)\game.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\global.cpp

"$(INTDIR)\global.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\ini_parser.cpp

"$(INTDIR)\ini_parser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\light.cpp

"$(INTDIR)\light.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\lightmgr.cpp

"$(INTDIR)\lightmgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\link_interface.cpp

"$(INTDIR)\link_interface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\mustash.cpp

"$(INTDIR)\mustash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\my_assertions.cpp

"$(INTDIR)\my_assertions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\NGL\PC\ngl_pc.cpp

"$(INTDIR)\ngl_pc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\path.cpp

"$(INTDIR)\path.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\HWOSPC\pc_algebra.cpp

"$(INTDIR)\pc_algebra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\HWOSPC\pc_timer.cpp

"$(INTDIR)\pc_timer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\pmesh.cpp

"$(INTDIR)\pmesh.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\po.cpp

"$(INTDIR)\po.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\po_anim.cpp

"$(INTDIR)\po_anim.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\pstring.cpp

"$(INTDIR)\pstring.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\region.cpp

"$(INTDIR)\region.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\script_lib.cpp

"$(INTDIR)\script_lib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\script_library_class.cpp

"$(INTDIR)\script_library_class.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\script_object.cpp

"$(INTDIR)\script_object.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\semaphores.cpp

"$(INTDIR)\semaphores.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=.\SpideyTM\SRC\sound_group.cpp

"$(INTDIR)\sound_group.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\sound_interface.cpp

"$(INTDIR)\sound_interface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\stringx.cpp

"$(INTDIR)\stringx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\textfile.cpp

"$(INTDIR)\textfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\visrep.cpp

"$(INTDIR)\visrep.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\vm_executable.cpp

"$(INTDIR)\vm_executable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\vm_stack.cpp

"$(INTDIR)\vm_stack.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\SpideyTM\SRC\vm_symbol.cpp

"$(INTDIR)\vm_symbol.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=.\SpideyTM\SRC\zip_filter.cpp

"$(INTDIR)\zip_filter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

