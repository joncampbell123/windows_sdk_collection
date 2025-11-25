# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

!IF "$(CFG)" == ""
CFG=scope - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to scope - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "scope - Win32 Release" && "$(CFG)" != "scope - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "scope.mak" CFG="scope - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scope - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "scope - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "scope - Win32 Debug"

!IF  "$(CFG)" == "scope - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f makefile"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "makefile.exe"
# PROP BASE Bsc_Name "makefile.bsc"
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE WIN95=1 CPU=i386 PERF=1 /f makefile"
# PROP Rebuild_Opt "/a"
# PROP Target_File "d:\qsysuk1\sdk\bin\debug\scope.ax"
# PROP Bsc_Name "scope.bsc"
OUTDIR=.\Release
INTDIR=.\Release

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f makefile"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "makefile.exe"
# PROP BASE Bsc_Name "makefile.bsc"
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE WIN95=1 CPU=i386 PERF=1 /f makefile"
# PROP Rebuild_Opt "/a"
# PROP Target_File "d:\qsysuk1\sdk\bin\debug\scope.ax"
# PROP Bsc_Name "scope.bsc"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ENDIF 

################################################################################
# Begin Target

# Name "scope - Win32 Release"
# Name "scope - Win32 Debug"

!IF  "$(CFG)" == "scope - Win32 Release"

"$(OUTDIR)\scope.ax" : 
   CD D:\qsysuk1\sdk\samples\scope
   NMAKE WIN95=1 CPU=i386 PERF=1 /f makefile

!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

"$(OUTDIR)\scope.ax" : 
   CD D:\qsysuk1\sdk\samples\scope
   NMAKE WIN95=1 CPU=i386 PERF=1 /f makefile

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\makefile

!IF  "$(CFG)" == "scope - Win32 Release"

!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scope.cpp

!IF  "$(CFG)" == "scope - Win32 Release"

!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scope.rc

!IF  "$(CFG)" == "scope - Win32 Release"

!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\icon1.ico

!IF  "$(CFG)" == "scope - Win32 Release"

!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\scope.h

!IF  "$(CFG)" == "scope - Win32 Release"

!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.h

!IF  "$(CFG)" == "scope - Win32 Release"

!ELSEIF  "$(CFG)" == "scope - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
