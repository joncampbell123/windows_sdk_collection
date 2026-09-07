# Microsoft Developer Studio Project File - Name="TableDump" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103
# TARGTYPE "Win32 (ALPHA) Console Application" 0x0603

CFG=TableDump - Win32 x86 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TableDump.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TableDump.mak" CFG="TableDump - Win32 x86 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TableDump - Win32 x86 Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "TableDump - Win32 x86 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "TableDump - Win32 (ALPHA) axp Debug" (based on\
 "Win32 (ALPHA) Console Application")
!MESSAGE "TableDump - Win32 (ALPHA) axp Release" (based on\
 "Win32 (ALPHA) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/OLEDB/TestSuite/Tools/TableDump", MAFAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "TableDump - Win32 x86 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TableDum"
# PROP BASE Intermediate_Dir "TableDum"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "x86_Debu"
# PROP Intermediate_Dir "x86_Debu"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_AFXDLL" /D "NO_LTMSPY" /FR /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_AFXDLL" /FR /YX /Fd"x86_Debu/TableDump.pdb" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 privlib.lib oledb.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib modulecore.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 odbc32.lib odbccp32.lib privlib.lib msdasc.lib oledb.lib modulecore.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "TableDump - Win32 x86 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TableDu0"
# PROP BASE Intermediate_Dir "TableDu0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "x86_Rele"
# PROP Intermediate_Dir "x86_Rele"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_AFXDLL" /D "NO_LTMSPY" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_AFXDLL" /FR /YX /Fd"x86_Rele/TableDump.pdb" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 privlib.lib oledb.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib modulecore.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 odbc32.lib odbccp32.lib privlib.lib msdasc.lib oledb.lib modulecore.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "TableDump - Win32 (ALPHA) axp Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TableDum"
# PROP BASE Intermediate_Dir "TableDum"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "axp_Debu"
# PROP Intermediate_Dir "axp_Debu"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /Fd"x86_Debu/TableDump.pdb" /FD /c
# ADD CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_AFXDLL" /FR /YX /Fd"axp_Debu/TableDump.pdb" /FD /MDd /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 privlib.lib oledb.lib modulecore.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:ALPHA
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 privlib.lib msdasc.lib oledb.lib modulecore.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:ALPHA
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "TableDump - Win32 (ALPHA) axp Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TableDu0"
# PROP BASE Intermediate_Dir "TableDu0"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "axp_Rele"
# PROP Intermediate_Dir "axp_Rele"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /Fd"x86_Rele/TableDump.pdb" /FD /c
# ADD CPP /nologo /MD /Gt0 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_AFXDLL" /FR /YX /Fd"axp_Rele/TableDump.pdb" /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 privlib.lib oledb.lib modulecore.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:ALPHA
# ADD LINK32 privlib.lib msdasc.lib oledb.lib modulecore.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:ALPHA

!ENDIF 

# Begin Target

# Name "TableDump - Win32 x86 Debug"
# Name "TableDump - Win32 x86 Release"
# Name "TableDump - Win32 (ALPHA) axp Debug"
# Name "TableDump - Win32 (ALPHA) axp Release"
# Begin Group "Source"

# PROP Default_Filter "cpp c"
# Begin Source File

SOURCE=.\SRC\Common.cpp

!IF  "$(CFG)" == "TableDump - Win32 x86 Debug"

!ELSEIF  "$(CFG)" == "TableDump - Win32 x86 Release"

!ELSEIF  "$(CFG)" == "TableDump - Win32 (ALPHA) axp Debug"

DEP_CPP_COMMO=\
	".\SRC\COMMON.H"\
	".\SRC\TableDump.h"\
	".\SRC\TableDumpVer.h"\
	{$(INCLUDE)}"ccol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"cmodinfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"list.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"strings.h"\
	{$(INCLUDE)}"transact.h"\
	

!ELSEIF  "$(CFG)" == "TableDump - Win32 (ALPHA) axp Release"

DEP_CPP_COMMO=\
	".\SRC\COMMON.H"\
	".\SRC\TableDump.h"\
	".\SRC\TableDumpVer.h"\
	{$(INCLUDE)}"ccol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"cmodinfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"list.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"strings.h"\
	{$(INCLUDE)}"transact.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\Error.cpp

!IF  "$(CFG)" == "TableDump - Win32 x86 Debug"

!ELSEIF  "$(CFG)" == "TableDump - Win32 x86 Release"

!ELSEIF  "$(CFG)" == "TableDump - Win32 (ALPHA) axp Debug"

DEP_CPP_ERROR=\
	".\SRC\COMMON.H"\
	".\SRC\TableDump.h"\
	{$(INCLUDE)}"ccol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"cmodinfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"list.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"strings.h"\
	{$(INCLUDE)}"transact.h"\
	

!ELSEIF  "$(CFG)" == "TableDump - Win32 (ALPHA) axp Release"

DEP_CPP_ERROR=\
	".\SRC\COMMON.H"\
	".\SRC\TableDump.h"\
	{$(INCLUDE)}"ccol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"cmodinfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"list.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"strings.h"\
	{$(INCLUDE)}"transact.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\TableDump.cpp

!IF  "$(CFG)" == "TableDump - Win32 x86 Debug"

!ELSEIF  "$(CFG)" == "TableDump - Win32 x86 Release"

!ELSEIF  "$(CFG)" == "TableDump - Win32 (ALPHA) axp Debug"

DEP_CPP_TABLE=\
	".\SRC\COMMON.H"\
	".\SRC\TableDump.h"\
	{$(INCLUDE)}"ccol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"cmodinfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"list.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"strings.h"\
	{$(INCLUDE)}"transact.h"\
	

!ELSEIF  "$(CFG)" == "TableDump - Win32 (ALPHA) axp Release"

DEP_CPP_TABLE=\
	".\SRC\COMMON.H"\
	".\SRC\TableDump.h"\
	{$(INCLUDE)}"ccol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"cmodinfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"list.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"strings.h"\
	{$(INCLUDE)}"transact.h"\
	

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\TableDump.rc
# End Source File
# Begin Source File

SOURCE=.\SRC\TableDump.rc2
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "h hpp"
# Begin Source File

SOURCE=.\SRC\COMMON.H
# End Source File
# Begin Source File

SOURCE=.\SRC\Error.h
# End Source File
# Begin Source File

SOURCE=.\SRC\resource.h
# End Source File
# Begin Source File

SOURCE=.\SRC\TableDump.h
# End Source File
# Begin Source File

SOURCE=.\SRC\TableDumpVer.h
# End Source File
# End Group
# End Target
# End Project
