# Microsoft Developer Studio Generated NMAKE File, Format Version 42001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

!IF "$(CFG)" == ""
CFG=ps1 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ps1 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ps1 - Win32 Release" && "$(CFG)" != "ps1 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ps1.mak" CFG="ps1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ps1 - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "ps1 - Win32 Debug" (based on "Win32 (x86) External Target")
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
# PROP Target_Last_Scanned "ps1 - Win32 Release"

!IF  "$(CFG)" == "ps1 - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f ps.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ps.exe"
# PROP BASE Bsc_Name "ps.bsc"
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE /f ps.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "ps1.exe"
# PROP Bsc_Name "ps1.bsc"
OUTDIR=.\Release
INTDIR=.\Release

ALL : 

CLEAN : 
        -@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "ps1 - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f ps.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ps.exe"
# PROP BASE Bsc_Name "ps.bsc"
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE   /f ps.mak "
# PROP Rebuild_Opt "/a"
# PROP Target_File "ps1.exe"
# PROP Bsc_Name "ps1.bsc"
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

# Name "ps1 - Win32 Release"
# Name "ps1 - Win32 Debug"

!IF  "$(CFG)" == "ps1 - Win32 Release"

".\ps1.exe" : 
   CD D:\VCEnterpriseProjects\settime
   NMAKE /f ps.mak

!ELSEIF  "$(CFG)" == "ps1 - Win32 Debug"

".\ps1.exe" : 
   CD D:\VCEnterpriseProjects\settime
   NMAKE   /f ps.mak 

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ps.mak

!IF  "$(CFG)" == "ps1 - Win32 Release"

!ELSEIF  "$(CFG)" == "ps1 - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
