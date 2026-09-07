# Microsoft Developer Studio Project File - Name="IACCESSR" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (ALPHA) Dynamic-Link Library" 0x0602

CFG=IACCESSR - Win32 x86 Debug MBCS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iaccessr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iaccessr.mak" CFG="IACCESSR - Win32 x86 Debug MBCS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IACCESSR - Win32 x86 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IACCESSR - Win32 x86 Debug MBCS" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IACCESSR - Win32 (ALPHA) axp Debug" (based on\
 "Win32 (ALPHA) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "IACCESSR - Win32 x86 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\iaccess0"
# PROP BASE Intermediate_Dir ".\iaccess0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_Debu"
# PROP Intermediate_Dir ".\x86_Debu"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /c
# SUBTRACT BASE CPP /Fr /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /D "_UNICODE" /Yu"modstandard.hpp" /FD /c
# SUBTRACT CPP /Fr
MTL=midl.exe
# ADD MTL /mktyplib203
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 modulecore.lib privlib.lib dbuuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 modulecore.lib privlib.lib oledb.lib msdasc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "IACCESSR - Win32 x86 Debug MBCS"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\iaccess1"
# PROP BASE Intermediate_Dir ".\iaccess1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_Debu\mbcs"
# PROP Intermediate_Dir ".\x86_Debu\mbcs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /c
# SUBTRACT BASE CPP /Fr /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /Yu"modstandard.hpp" /Fd".\x86_Debu\mbcs\iaccessr.pdb" /FD /c
MTL=midl.exe
# ADD MTL /mktyplib203
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 modulecore.lib privlib.lib dbuuid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 modulecore.lib privlib.lib oledb.lib msdasc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "IACCESSR - Win32 (ALPHA) axp Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\IACCESS0"
# PROP BASE Intermediate_Dir ".\IACCESS0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\axp_debu\mbcs"
# PROP Intermediate_Dir ".\axp_debu\mbcs"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /alpha
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /alpha
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Yu"modstandard.hpp" /MDd /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /Yu"modstandard.hpp" /FD /MDd /c
# SUBTRACT CPP /Fr
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 modulecore.lib privlib.lib dbuuid.lib /nologo /subsystem:windows /dll /debug /machine:ALPHA
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 modulecore.lib privlib.lib oledb.lib msdasc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:ALPHA
# SUBTRACT LINK32 /pdb:none /map

!ENDIF 

# Begin Target

# Name "IACCESSR - Win32 x86 Debug"
# Name "IACCESSR - Win32 x86 Debug MBCS"
# Name "IACCESSR - Win32 (ALPHA) axp Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Modsrc\IACCESSR.cpp

!IF  "$(CFG)" == "IACCESSR - Win32 x86 Debug"

# ADD BASE CPP /Yu"MODStandard.hpp"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IACCESSR - Win32 x86 Debug MBCS"

# ADD BASE CPP /Yu"MODStandard.hpp"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IACCESSR - Win32 (ALPHA) axp Debug"

NODEP_CPP_IACCE=\
	".\accessr.h"\
	".\Col.hpp"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ODClasses.hpp"\
	".\ODCommon.hpp"\
	".\ODError.hpp"\
	".\ODMacros.hpp"\
	".\ODStandard.hpp"\
	".\ODuleCore2.h"\
	".\oledb.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivlib.h"\
	".\rvtrace.h"\
	".\sdasql.h"\
	".\SuperLog.hpp"\
	".\table.hpp"\
	".\TMCommon.h"\
	".\TMSink.h"\
	".\transtn.hpp"\
	
# ADD BASE CPP /Gt0 /Yu"modstandard.hpp"
# ADD CPP /Gt0 /Yu"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\res\IACCESSR.rc
# ADD BASE RSC /l 0x409 /i "Modsrc\res"
# ADD RSC /l 0x409 /i "Modsrc\res" /i ".\Modsrc\res"
# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODStandard.cpp

!IF  "$(CFG)" == "IACCESSR - Win32 x86 Debug"

# ADD BASE CPP /Yc"MODStandard.hpp"
# ADD CPP /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IACCESSR - Win32 x86 Debug MBCS"

# ADD BASE CPP /Yc"MODStandard.hpp"
# ADD CPP /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IACCESSR - Win32 (ALPHA) axp Debug"

NODEP_CPP_MODST=\
	".\ODClasses.hpp"\
	".\ODCommon.hpp"\
	".\ODError.hpp"\
	".\ODMacros.hpp"\
	".\ODStandard.hpp"\
	".\ODuleCore2.h"\
	".\SuperLog.hpp"\
	".\TMCommon.h"\
	".\TMSink.h"\
	
# ADD BASE CPP /Gt0 /Yc"modstandard.hpp"
# ADD CPP /Gt0 /Yc"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODSTUB.CPP

!IF  "$(CFG)" == "IACCESSR - Win32 x86 Debug"

# ADD BASE CPP /Yu"MODStandard.hpp"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IACCESSR - Win32 x86 Debug MBCS"

# ADD BASE CPP /Yu"MODStandard.hpp"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "IACCESSR - Win32 (ALPHA) axp Debug"

NODEP_CPP_MODSTU=\
	".\ODClasses.hpp"\
	".\ODCommon.hpp"\
	".\ODError.hpp"\
	".\ODMacros.hpp"\
	".\ODStandard.hpp"\
	".\ODuleCore2.h"\
	".\SuperLog.hpp"\
	".\TMCommon.h"\
	".\TMSink.h"\
	
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

SOURCE=.\Include\iaccessr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
