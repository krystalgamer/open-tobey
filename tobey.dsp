# Microsoft Developer Studio Project File - Name="tobey" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tobey - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tobey.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tobey - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TOBEY_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "SpideyTM\SRC\sgistl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TOBEY_EXPORTS" /D "BUILD_BOOTABLE" /D "REGIONCULL" /D "TARGET_PC" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "tobey - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TOBEY_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TOBEY_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tobey - Win32 Release"
# Name "tobey - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SpideyTM\SRC\app.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\debugutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dllmain.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\errorcontext.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\file_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\filespec.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\global.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\ini_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\mustash.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\my_assertions.cpp
# End Source File
# Begin Source File

SOURCE=.\NGL\PC\ngl_pc.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\pc_timer.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\pstring.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\region.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\script_object.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\semaphores.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\signal.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\singleton.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\so_data_block.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\stringx.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\textfile.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vm_executable.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vm_stack.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vm_symbol.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vm_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\w32_archalloc.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\w32_errmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\w32_file.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\wds.cpp
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\x86_math.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "sgistl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\algo.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\algobase.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\alloc.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\bvector.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\char_traits.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\concept_checks.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\container_concepts.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\defalloc.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\deque.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\function.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\hash_map.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\hash_set.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\hashtable.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\heap.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\iterator.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\list.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\map.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\multimap.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\multiset.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\pair.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\pthread_alloc.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\rope.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\ropeimpl.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\sequence_concepts.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\set.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\slist.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stack.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_algo.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_algobase.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_alloc.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_bvector.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_config.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_construct.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_ctraits_fns.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_deque.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_exception.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_function.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_hash_fun.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_hash_map.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_hash_set.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_hashtable.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_heap.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_iterator.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_iterator_base.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_list.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_map.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_multimap.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_multiset.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_numeric.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_pair.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_queue.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_range_errors.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_raw_storage_iter.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_relops.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_rope.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_set.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_slist.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_stack.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_string_fwd.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_tempbuf.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_threads.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_tree.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_uninitialized.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\stl_vector.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\tempbuf.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\tree.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\type_traits.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sgistl\vector.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\SpideyTM\SRC\aggvertbuf.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\algebra.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\anim.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\anim_flavor.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\anim_flavors.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\anim_ids.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\app.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\archalloc.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\avltree.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\beam.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\beam_signals.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\bone.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\bound.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\box_trigger_interface.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\camera.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\capsule.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\cface.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\chunkfile.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\colgeom.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\colmesh.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\color.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\conglom.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\constants.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\convex_box.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\d3d_rasterize.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\d3d_texturemgr.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\debug.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\devoptflags.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\devoptints.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\devoptstrs.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\entflavor.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\entity.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\entity_anim.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\entity_interface.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\entity_maker.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\entity_signals.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\entityflags.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\entityid.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\errorcontext.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\face.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\faceflags.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\fast_vector.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\file.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\file_finder.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\file_manager.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\filespec.h
# End Source File
# Begin Source File

SOURCE=.\forwards.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\frame_info.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\game.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\game_info_vars.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\generator.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\global.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\global_signals.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\graph.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\hull.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\hwmath.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\hwrasterize.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\hyperplane.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\ini_parser.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\inputmgr.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\instance.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\item.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\item_signals.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\ksheaps.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\light.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\linear_anim.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\link_interface.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\map_e.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\marker.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\material.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\matfac.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\maxskinbones.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\meshrefs.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\mic.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\mobject.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\mustash.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\my_assertions.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\my_export.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\my_patch.h
# End Source File
# Begin Source File

SOURCE=.\my_validations.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\ngl.h
# End Source File
# Begin Source File

SOURCE=.\NGL\PC\ngl_pc.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\ode.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\opcodes.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\osalloc.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\osdevopts.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\oserrmsg.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\osfile.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\ostimer.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\particle.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\path.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\pc_algebra.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\pc_port.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\pcglobals.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\physics.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\plane.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\platform_defines.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\pmesh.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\po.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\po_anim.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\portal.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\profiler.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\project.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\pstring.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\random.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\rect.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\refptr.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\region.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\region_graph.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\render_data.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\renderflav.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\scene_anim.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\script_lib_mfg.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\script_library_class.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\script_mfg_signals.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\script_object.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\semaphores.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\signal_anim.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\signals.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\singleton.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sky.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sl_debugger.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\so_data_block.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\sphere.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\stash_support.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\stashes.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\staticmem.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\stringx.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\terrain.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\textfile.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\timer.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\txtcoord.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\types.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\usefulmath.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\users.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vert.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\visrep.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vm_executable.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vm_stack.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vm_symbol.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vm_symbol_list.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\vm_thread.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\w32_alloc.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\w32_errmsg.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\w32_file.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\w32_timer.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\warnlvl.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\wds.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\wedge.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\widget.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\widget_entity.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\HWOSPC\x86_math.h
# End Source File
# Begin Source File

SOURCE=.\SpideyTM\SRC\zip_filter.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
