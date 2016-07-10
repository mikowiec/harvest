# Microsoft Developer Studio Project File - Name="GLUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GLUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GLUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GLUI.mak" CFG="GLUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GLUI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GLUI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GLUI - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../../include" /I "../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x41d /d "NDEBUG"
# ADD RSC /l 0x41d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "GLUI - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm- /Gi- /GR /GX /Zi /Od /I "../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /GZ /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x41d /d "_DEBUG"
# ADD RSC /l 0x41d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "GLUI - Win32 Release"
# Name "GLUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\algebra3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\arcball.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_add_controls.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_bitmap_img_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_bitmaps.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_button.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_checkbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_column.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_control.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_edittext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_checkbox_0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_checkbox_0_dis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_checkbox_1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_checkbox_1_dis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_downarrow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_leftarrow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_listbox_down.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_listbox_up.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_listbox_up_dis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_radiobutton_0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_radiobutton_0_dis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_radiobutton_1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_radiobutton_1_dis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_rightarrow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_spindown_0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_spindown_1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_spindown_dis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_spinup_0.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_spinup_1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_spinup_dis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_img_uparrow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_mouse_iaction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_node.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_panel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_radio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_rollout.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_rotation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_separator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_spinner.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_statictext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui_translation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\quaternion.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\algebra3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\arcball.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\glui.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\quaternion.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\stdinc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Reaper\src\ext\glui\viewmodel.h
# End Source File
# End Group
# End Target
# End Project
