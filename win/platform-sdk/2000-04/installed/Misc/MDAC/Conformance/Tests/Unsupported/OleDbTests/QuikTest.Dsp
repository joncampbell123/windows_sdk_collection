# Microsoft Developer Studio Project File - Name="QUIKTEST" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (ALPHA) Dynamic-Link Library" 0x0602

CFG=QUIKTEST - Win32 x86 Debug MBCS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Quiktest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Quiktest.mak" CFG="QUIKTEST - Win32 x86 Debug MBCS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QUIKTEST - Win32 x86 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "QUIKTEST - Win32 x86 Debug MBCS" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "QUIKTEST - Win32 (ALPHA) axp Debug" (based on\
 "Win32 (ALPHA) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/OLEDB/TestSuite/Tests", LAAAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\QUIKTEST"
# PROP BASE Intermediate_Dir ".\QUIKTEST"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_Debu"
# PROP Intermediate_Dir ".\x86_Debu"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /c
# SUBTRACT BASE CPP /Fr /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /FD /c
MTL=midl.exe
# ADD MTL /mktyplib203
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msad.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 modulecore.lib privlib.lib oledb.lib msdasc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msdasc.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug MBCS"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\TEMPLAT0"
# PROP BASE Intermediate_Dir ".\TEMPLAT0"
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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /Yu"modstandard.hpp" /FD /c
MTL=midl.exe
# ADD MTL /mktyplib203
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msad.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 modulecore.lib privlib.lib oledb.lib msdasc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msdasc.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 (ALPHA) axp Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\QUIKTEST_"
# PROP BASE Intermediate_Dir ".\QUIKTEST_"
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
# ADD BASE CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /Fr /YX /MDd /c
# ADD CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /YX /FD /MDd /c
# SUBTRACT CPP /Fr
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 modulecore.lib privlib.lib msad.lib /nologo /subsystem:windows /dll /profile /map /debug /machine:ALPHA
# ADD LINK32 modulecore.lib privlib.lib oledb.lib msdasc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib msdasc.lib /nologo /subsystem:windows /dll /debug /machine:ALPHA
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "QUIKTEST - Win32 x86 Debug"
# Name "QUIKTEST - Win32 x86 Debug MBCS"
# Name "QUIKTEST - Win32 (ALPHA) axp Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Modsrc\EXTRALIB.CPP

!IF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug MBCS"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 (ALPHA) axp Debug"

DEP_CPP_EXTRA=\
	{$(INCLUDE)}"CCol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"CModInfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"CSuperLog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"DTMCommon.h"\
	{$(INCLUDE)}"EXTRALIB.h"\
	{$(INCLUDE)}"List.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"MODClasses.hpp"\
	{$(INCLUDE)}"MODCommon.hpp"\
	{$(INCLUDE)}"MODError.hpp"\
	{$(INCLUDE)}"MODMacros.hpp"\
	{$(INCLUDE)}"MODStandard.hpp"\
	{$(INCLUDE)}"MODuleCore.h"\
	{$(INCLUDE)}"msdaguid.h"\
	{$(INCLUDE)}"msdasql.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"Strings.h"\
	{$(INCLUDE)}"transact.h"\
	
# ADD BASE CPP /Gt0 /Yu"modstandard.hpp"
# ADD CPP /Gt0 /Yu"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODStandard.cpp

!IF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug"

# ADD CPP /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug MBCS"

# ADD CPP /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 (ALPHA) axp Debug"

DEP_CPP_MODST=\
	{$(INCLUDE)}"CSuperLog.hpp"\
	{$(INCLUDE)}"DTMCommon.h"\
	{$(INCLUDE)}"MODClasses.hpp"\
	{$(INCLUDE)}"MODCommon.hpp"\
	{$(INCLUDE)}"MODError.hpp"\
	{$(INCLUDE)}"MODMacros.hpp"\
	{$(INCLUDE)}"MODStandard.hpp"\
	{$(INCLUDE)}"MODuleCore.h"\
	
# ADD BASE CPP /Gt0 /Yc"modstandard.hpp"
# ADD CPP /Gt0 /Yc"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODSTUB.CPP

!IF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug MBCS"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 (ALPHA) axp Debug"

DEP_CPP_MODSTU=\
	{$(INCLUDE)}"CSuperLog.hpp"\
	{$(INCLUDE)}"DTMCommon.h"\
	{$(INCLUDE)}"MODClasses.hpp"\
	{$(INCLUDE)}"MODCommon.hpp"\
	{$(INCLUDE)}"MODError.hpp"\
	{$(INCLUDE)}"MODMacros.hpp"\
	{$(INCLUDE)}"MODStandard.hpp"\
	{$(INCLUDE)}"MODuleCore.h"\
	
# ADD BASE CPP /Gt0 /Yu"modstandard.hpp"
# ADD CPP /Gt0 /Yu"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODSTUB.DEF
# End Source File
# Begin Source File

SOURCE=.\Modsrc\QUIKTEST.CPP

!IF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 x86 Debug MBCS"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "QUIKTEST - Win32 (ALPHA) axp Debug"

DEP_CPP_QUIKT=\
	{$(INCLUDE)}"CCol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"CModInfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"CSuperLog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"DTMCommon.h"\
	{$(INCLUDE)}"EXTRALIB.h"\
	{$(INCLUDE)}"List.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"MODClasses.hpp"\
	{$(INCLUDE)}"MODCommon.hpp"\
	{$(INCLUDE)}"MODError.hpp"\
	{$(INCLUDE)}"MODMacros.hpp"\
	{$(INCLUDE)}"MODStandard.hpp"\
	{$(INCLUDE)}"MODuleCore.h"\
	{$(INCLUDE)}"msdadc.h"\
	{$(INCLUDE)}"msdaguid.h"\
	{$(INCLUDE)}"msdasc.h"\
	{$(INCLUDE)}"msdasql.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"QuikTest.h"\
	{$(INCLUDE)}"Strings.h"\
	{$(INCLUDE)}"transact.h"\
	
# ADD BASE CPP /Gt0 /Yu"modstandard.hpp"
# ADD CPP /Gt0 /Yu"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\res\QUIKTEST.rc
# ADD BASE RSC /l 0x409 /i "Modsrc\res"
# ADD RSC /l 0x409 /i "Modsrc\res" /i ".\Modsrc\res"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Include\EXTRALIB.h
# End Source File
# Begin Source File

SOURCE=.\Include\QuikTest.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
