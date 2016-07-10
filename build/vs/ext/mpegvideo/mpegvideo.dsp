# Microsoft Developer Studio Project File - Name="mpegvideo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mpegvideo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mpegvideo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mpegvideo.mak" CFG="mpegvideo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mpegvideo - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mpegvideo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mpegvideo - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mpegvideo - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../../../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "mpegvideo - Win32 Release"
# Name "mpegvideo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\output\audioData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\output\audioDataArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\output\audioTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\output\avSyncer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\input\bufferInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\colorTable8Bit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\colorTableHighBit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\decoder\command.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\decoder\commandPipe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\copyFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\copyFunctions_asm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\copyFunctions_mmx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\decoderClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\decoder\decoderPlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\decoderTables.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\dither16Bit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\dither32Bit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\dither32mmx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\dither8Bit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\ditherer_mmx16.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\ditherRGB.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\ditherRGB_flipped.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\dither\ditherWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\dxHead.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\dynBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\filter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\filter_2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\globals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\gop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\huffmanlookup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\huffmantable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\input\inputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\jrevdct.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\macroBlock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\mmxidct.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\motionVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\mpegAudioBitWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\mpegAudioHeader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\mpegAudioStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\mpegExtension.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\mpeglayer1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\mpeglayer2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\mpeglayer3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\mpegPlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpgplayer\mpegStreamPlayer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\mpegSystemHeader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\mpegSystemStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\mpegtable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\mpegtoraw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\mpegVideoBitWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\mpegVideoHeader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\mpegVideoLength.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\mpegVideoStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpgplayer\mpgPlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\output\outputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\picture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\pictureArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\output\pluginInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\recon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\input\simpleRingBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\slice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\splay\splayPlugin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\syncClock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\syncClockMPEG.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\timeStampArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\timeWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\mpegplay\videoDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\src\ext\mpegvideo\util\render\yuvPicture.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Source File

SOURCE=..\..\lib\pthread.lib
# End Source File
# End Target
# End Project
