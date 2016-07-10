# Microsoft Developer Studio Project File - Name="reaper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=reaper - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "reaper.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "reaper.mak" CFG="reaper - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "reaper - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "reaper - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "reaper - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /Gi /GR /GX /Zi /O2 /I "../../../src" /I "../include" /I "../../win32-support/include" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D _STLP_DEBUG=1 /Fr /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

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
# ADD CPP /nologo /G6 /MDd /W3 /GR /GX /Z7 /Od /Gy /I "../../win32-support/include" /I "../../../src" /I "../include" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /Fr /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /nodefaultlib:libc

!ENDIF 

# Begin Target

# Name "reaper - Win32 Release"
# Name "reaper - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ai"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\ai\astar.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\fsm.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\gvbase.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\gvcontainer.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\gvordinary.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\gvturret.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\ai\navigation_graph.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\player.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\ship_base.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\ship_bomber.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\ship_fighter.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\ai\thinker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\turret.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_ai"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_ai"
# ADD CPP /Gm /Zi /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\ai\waypoints.cpp
# End Source File
# End Group
# Begin Group "gfx"

# PROP Default_Filter ""
# Begin Group "pm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\gfx\pm\badheap.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\pm\pm.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\pm\treap.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# End Group
# Begin Group "shadow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\main.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\silhouette_all.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\silhouette_dynamic.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\silhouette_player.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\simple_dynamic.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\simple_player.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\types.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\utility.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# End Group
# Begin Group "effects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\gfx\effects.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\envmapper.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\hud.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\particles.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\sky.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\terrain.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\water.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# End Group
# Begin Group "geometry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gfx\geometry.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\io.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\mesh.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\mesh_file.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\mesh_file_read.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# End Group
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gfx\billboard.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\camera.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\cubemap.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\dynamic_texture.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\light.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\png.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\shaders.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\static_geom.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\texture.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# End Group
# Begin Group "engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Src\Gfx\exceptions.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\fps_meter.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\instances.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\light_mgr.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\mesh_mgr.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\settings.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\statistics.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\texture_mgr.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\vertex_mgr.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /O2 /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# End Group
# Begin Group "cloud"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gfx\cloud\cloud.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\cloud\impostor.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\cloud\particle.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\cloud\simulate.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\\src\gfx\gfx.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\gfx_init.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\gfx_render.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\gfx_screenshot.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\gfx_simulate.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\misc.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_gfx"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_gfx"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# End Group
# Begin Group "net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\net\game_mgr.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_net"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_net"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\net\nameservice.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_net"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_net"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\net\net.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_net"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_net"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\net\netgame.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_net"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_net"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\net\server.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_net"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_net"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\net\sockstream.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_net"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_net"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\net\sync.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_net"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_net"

!ENDIF 

# End Source File
# End Group
# Begin Group "res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\res\config.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_res"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_res"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\res\res.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_res"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_res"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\res\resource.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_res"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_res"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\res\zstream.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_res"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_res"

!ENDIF 

# End Source File
# End Group
# Begin Group "main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\main\exceptions.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_main"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_main"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\types_io.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_main"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_main"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\types_ops.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_main"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_main"

!ENDIF 

# End Source File
# End Group
# Begin Group "misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\misc\font.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_misc"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_misc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\menu_system.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_misc"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_misc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\parse.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_misc"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_misc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\plugin.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_misc"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_misc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\profile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\ptrstats.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_misc"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_misc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\singleton.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_misc"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_misc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\unique.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_misc"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_misc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\utility.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_misc"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_misc"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\version.cpp
# End Source File
# End Group
# Begin Group "world"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\world\geometry.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_world"
# ADD CPP /Ob2

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_world"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\level.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_world"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_world"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\quadtree.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_world"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_world"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\world.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_world"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_world"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\world\world_impl.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_world"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_world"

!ENDIF 

# End Source File
# End Group
# Begin Group "snd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\snd\sound_system.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "release_snd"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_snd"

!ENDIF 

# End Source File
# End Group
# Begin Group "object"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\object\accessor.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\base.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\collide.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\controls.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\event.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\factory.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\gen_ship.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\gvs.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\hull.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\mkinfo.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\obj_factory.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\object.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\object_impl.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\objectgroup.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\objevent.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\phys.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "reaper_object"
# ADD CPP /Zd

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "reaper_object"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\phys_data.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\projectile.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\renderer.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\sheep.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\ship.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\shot.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\sound.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\sync.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\tree.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\turret.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\weapon.cpp
# PROP Intermediate_Dir "reaper_object"
# End Source File
# End Group
# Begin Group "ext"

# PROP Default_Filter ""
# End Group
# Begin Group "phys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\phys\accessor.cpp
# PROP Intermediate_Dir "reaper_phys"
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\phys\engine.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "reaper_phys"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "reaper_phys"
# ADD CPP /Z7 /Od /Ob0

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\phys\pairs.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "reaper_phys"

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "reaper_phys"
# ADD CPP /Z7 /Od /Ob0

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\phys\treap.cpp
# PROP Intermediate_Dir "reaper_phys"
# End Source File
# End Group
# Begin Group "game"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\game\helpers.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "Release_game"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_game"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\init.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\mainloop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\menus.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "Release_game"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_game"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\mission.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "Release_game"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_game"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\\src\game\object_mgr.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "Release_game"
# ADD CPP /Gi- /Zd /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_game"
# ADD CPP /Gi- /Z7 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\progressbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\game\scenario_mgr.cpp

!IF  "$(CFG)" == "reaper - Win32 Release"

# PROP Intermediate_Dir "Release_game"
# ADD CPP /Gi- /FR

!ELSEIF  "$(CFG)" == "reaper - Win32 Debug"

# PROP Intermediate_Dir "debug_game"
# ADD CPP /MDd /Gi- /vd1 /GR /GX /Z7 /Od /Ob0 /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\scheduler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\startup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\state_base.cpp
# End Source File
# End Group
# Begin Group "os"

# PROP Default_Filter ""
# Begin Group "os_gfx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\os\gfx\gfx_drv.cpp
# End Source File
# End Group
# Begin Group "os_net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\os\net\netcompat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\net\socket.cpp
# End Source File
# End Group
# Begin Group "os_event"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\os\event\event_impl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\event\event_prim.cpp
# End Source File
# End Group
# Begin Group "os_gl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\os\gl\gl_defs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\gl\gl_info.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\gl\gl_state.cpp
# End Source File
# End Group
# Begin Group "os_snd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\os\snd\snd_system.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\os\compat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\conc_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\dynload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\extra.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\osinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\stacktrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\time.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\time_types.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\videomode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\win32_errors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\os\worker.cpp
# End Source File
# End Group
# Begin Group "msg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\msg\event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\msg\mapping.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "ai hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\ai\astar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\exceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\fsm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\ai\msg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ai\navigation_graph.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ai\steering.h
# End Source File
# End Group
# Begin Group "gfx hdr"

# PROP Default_Filter ""
# Begin Group "pm hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\gfx\pm\badheap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\pm\pm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\pm\pm_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\pm\treap.h
# End Source File
# End Group
# Begin Group "shadow hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\main.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\shadow\utility.h
# End Source File
# End Group
# Begin Group "effects hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gfx\hud.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\interfaces.h
# End Source File
# End Group
# Begin Group "geometry hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gfx\geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\io.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\mesh.h
# End Source File
# End Group
# Begin Group "core hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\gfx\camera.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\cexcept.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\displaylist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\fence.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\light.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\shaders.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\static_geom.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\texture.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\vertex_array.h
# End Source File
# End Group
# Begin Group "engine hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\gfx\abstracts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\exceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\fps_meter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\initializers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\instances.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\managers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\settings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\statistics.h
# End Source File
# End Group
# Begin Group "cloud hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gfx\cloud\cloud.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\cloud\impostor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\cloud\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\cloud\particle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gfx\cloud\simulate.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\\src\gfx\gfx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\gfx_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\gfx\renderer.h
# End Source File
# End Group
# Begin Group "hw hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\hw\abstime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\bits.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\compat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\conc_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\concurrent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\dxwrap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\dynload.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\event.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\event_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\event_prim.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\exceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\fdstream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\gfx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\gfx_driver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\gfx_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\gfxplugin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\gl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\gl_helper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\gl_state.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\glext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\interfaces.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\mapping.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\netcompat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\osinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\profile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\reltime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\snd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\snd_mp3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\snd_simple.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\snd_subsystem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\snd_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\snd_wave.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\socket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\stacktrace.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\stat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\time.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\time_types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\video.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\video_helper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\hw\win32_errors.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\windows.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\hw\worker.h
# End Source File
# End Group
# Begin Group "net hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\net\game_mgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\net\nameservice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\net\net.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\net\netgame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\net\server.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\net\sockstream.h
# End Source File
# End Group
# Begin Group "res hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\res\config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\res\res.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SRC\RES\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\res\zstream.h
# End Source File
# End Group
# Begin Group "main hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\main\enums.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\exceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\types4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\types4_io.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\types4_ops.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\main\types_base.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\main\types_forward.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\types_io.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\types_ops.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\main\types_param.h
# End Source File
# End Group
# Begin Group "misc hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\misc\base.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\command.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\creator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\event.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\font.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\free.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\hash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\iostream_helper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\map.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\maybe.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\menu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\menu_system.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\monitor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\parse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\plugin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\refptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\resmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\sequence.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\skipiter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\smartptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\stlhelper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\test_main.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc\tuple.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\unique.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\uniqueptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\misc\utility.h
# End Source File
# End Group
# Begin Group "world hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\world\exceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\geom_helper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\level.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\quadtree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\query_obj.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\search.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\triangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\world.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\world\world_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\world\worldref.h
# End Source File
# End Group
# Begin Group "snd hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\snd\sound_system.h
# End Source File
# End Group
# Begin Group "object hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\\src\object\ai.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\base.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Src\Object\base_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Src\Object\Baseptr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Src\Object\Collide.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\controls.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\current_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Src\Object\Event.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Src\Object\exceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\factory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Src\Object\factory_loaders.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\forward.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\hull.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\mkinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Src\Object\obj_factory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\object.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\object_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\objectgroup.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\phys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\phys_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\player_ai.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\object\projectile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\renderer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\sound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\object\weapon.h
# End Source File
# End Group
# Begin Group "ext hdr"

# PROP Default_Filter ""
# Begin Group "glh hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_array.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_combiners.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_convenience.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_cube_map.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_extensions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_genext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_glut.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_glut2.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_glut_callfunc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_glut_replay.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_glut_text.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_interactors.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_linear.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_mipmaps.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_nveb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_obs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ext\glh\glh_text.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\ext\nvparse.h
# End Source File
# End Group
# Begin Group "phys hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\phys\collisionQueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\phys\engine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\phys\exceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\phys\pairs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\phys\phys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\phys\physConstants.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\phys\treap.h
# End Source File
# End Group
# Begin Group "game hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\game\forked.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\game\helpers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\mainloop.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\menus.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\mission.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\game\object_mgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\progressbar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\game\scenario_mgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\\src\game\scenario_states.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\scheduler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\game\state.h
# End Source File
# End Group
# Begin Group "gl hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\gl\glext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gl\glxext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gl\wglext.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE="..\..\win32-support\lib\libpngz.lib"
# End Source File
# End Target
# End Project
