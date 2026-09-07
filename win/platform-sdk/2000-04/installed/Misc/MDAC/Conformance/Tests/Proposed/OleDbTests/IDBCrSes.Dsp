# Microsoft Developer Studio Project File - Name="IDBCRSES" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (ALPHA) Dynamic-Link Library" 0x0602

CFG=IDBCRSES - Win32 x86 Debug MBCS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IDBCrSes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IDBCrSes.mak" CFG="IDBCRSES - Win32 x86 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IDBCRSES - Win32 x86 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IDBCRSES - Win32 x86 Debug MBCS" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IDBCRSES - Win32 (ALPHA) axp Debug" (based on\
 "Win32 (ALPHA) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/OLEDB/TestSuite/Tests", LAAAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "IDBCRSES - Win32 x86 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_debu"
# PROP Intermediate_Dir ".\x86_debu"
# PROP Ignore_Export_Lib 0
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_UNICODE" /D "_DEBUG" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /Yu
MTL=midl.exe
# ADD MTL /mktyplib203
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 modulecore.lib privlib.lib oledb.lib msdasc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "IDBCRSES - Win32 x86 Debug MBCS"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Win32_D0"
# PROP BASE Intermediate_Dir ".\Win32_D0"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_debu\mbcs"
# PROP Intermediate_Dir ".\x86_debu\mbcs"
# PROP Ignore_Export_Lib 0
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /FR /c
# SUBTRACT BASE CPP /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /Yu
MTL=midl.exe
# ADD MTL /mktyplib203
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 dbuuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 modulecore.lib privlib.lib oledb.lib msdasc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "IDBCRSES - Win32 (ALPHA) axp Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\IDBCRSES"
# PROP BASE Intermediate_Dir ".\IDBCRSES"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\axp_debu\mbcs"
# PROP Intermediate_Dir ".\axp_debu\mbcs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /alpha
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /alpha
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /YX /MDd /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /D "_MBCS" /YX /FD /MDd /c
# SUBTRACT CPP /Fr
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 modulecore.lib dbuuid.lib privlib.lib /nologo /subsystem:windows /dll /debug /machine:ALPHA
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 modulecore.lib privlib.lib oledb.lib msdasc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:ALPHA
# SUBTRACT LINK32 /pdb:none /map

!ENDIF 

# Begin Target

# Name "IDBCRSES - Win32 x86 Debug"
# Name "IDBCRSES - Win32 x86 Debug MBCS"
# Name "IDBCRSES - Win32 (ALPHA) axp Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Modsrc\IDBCRSES.CPP

!IF  "$(CFG)" == "IDBCRSES - Win32 x86 Debug"

# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IDBCRSES - Win32 x86 Debug MBCS"

# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IDBCRSES - Win32 (ALPHA) axp Debug"

DEP_CPP_IDBCR=\
	{$(INCLUDE)}"ccol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"IDBCrSes.h"\
	{$(INCLUDE)}"ltmsink.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modstandard.hpp"\
	{$(INCLUDE)}"modulecore2.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"transact.h"\
	
# ADD BASE CPP /Gt0 /Yu"modstandard.hpp"
# ADD CPP /Gt0 /Yu"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\res\IDBCRSES.RC
# ADD BASE RSC /l 0x409 /i "Modsrc\res"
# ADD RSC /l 0x409 /i "Modsrc\res" /i ".\Modsrc\res"
# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODStandard.cpp

!IF  "$(CFG)" == "IDBCRSES - Win32 x86 Debug"

# ADD CPP /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IDBCRSES - Win32 x86 Debug MBCS"

# ADD CPP /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IDBCRSES - Win32 (ALPHA) axp Debug"

DEP_CPP_MODST=\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"ltmsink.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modstandard.hpp"\
	{$(INCLUDE)}"modulecore2.h"\
	
# ADD BASE CPP /Gt0 /Yc"modstandard.hpp"
# ADD CPP /Gt0 /Yc"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODSTUB.CPP

!IF  "$(CFG)" == "IDBCRSES - Win32 x86 Debug"

# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IDBCRSES - Win32 x86 Debug MBCS"

# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IDBCRSES - Win32 (ALPHA) axp Debug"

DEP_CPP_MODSTU=\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"ltmsink.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"modcoreguids.c"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modstandard.hpp"\
	{$(INCLUDE)}"modulecore2.h"\
	
# ADD BASE CPP /Gt0 /Yu"modstandard.hpp"
# ADD CPP /Gt0 /Yu"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODSTUB.DEF
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Include\IDBCrSes.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
