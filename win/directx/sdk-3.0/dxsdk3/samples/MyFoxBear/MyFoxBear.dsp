# Microsoft Developer Studio Project File - Name="MyFoxBear" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MyFoxBear - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MyFoxBear.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MyFoxBear.mak" CFG="MyFoxBear - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MyFoxBear - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MyFoxBear - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MyFoxBear - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "D:\code\dxsdk3\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib fastfile.lib ddraw.lib dsound.lib dxguid.lib dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /libpath:"D:\code\dxsdk3\sdk\lib"

!ELSEIF  "$(CFG)" == "MyFoxBear - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "D:\code\dxsdk3\sdk\inc" /I "D:\code\dxsdk3\sdk\samples\misc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib fastfile.lib ddraw.lib dsound.lib dxguid.lib dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"D:\code\dxsdk3\sdk\lib"

!ENDIF 

# Begin Target

# Name "MyFoxBear - Win32 Release"
# Name "MyFoxBear - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\bmp.c
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\ddraw.c
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\misc\dsutil.c
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\fbsound.c
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\foxbear.c
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\foxbear.rc
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\gameproc.c
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\gfx.c
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\plane.c
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\sprite.c
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\tile.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\sdk\samples\misc\dsutil.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\fbsound.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\foxbear.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\gameproc.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\gfx.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\plane.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\rcids.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\sprite.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\samples\foxbear\tile.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
