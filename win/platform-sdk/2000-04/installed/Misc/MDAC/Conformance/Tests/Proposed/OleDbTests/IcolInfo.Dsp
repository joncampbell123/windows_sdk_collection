# Microsoft Developer Studio Project File - Name="ICOLINFO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (ALPHA) Dynamic-Link Library" 0x0602

CFG=ICOLINFO - Win32 x86 Debug MBCS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Icolinfo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Icolinfo.mak" CFG="ICOLINFO - Win32 x86 Debug MBCS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ICOLINFO - Win32 x86 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ICOLINFO - Win32 x86 Debug MBCS" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ICOLINFO - Win32 (ALPHA) axp Debug" (based on\
 "Win32 (ALPHA) Dynamic-Link Library")
!MESSAGE "ICOLINFO - Win32 x86 Debug TT" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ICOLINFO - Win32 x86 Debug MBCS TT" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/OLEDB/TestSuite/Tests", LAAAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\ICOLINFO"
# PROP BASE Intermediate_Dir ".\ICOLINFO"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_Debu"
# PROP Intermediate_Dir ".\x86_Debu"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /c
# SUBTRACT BASE CPP /Fr /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /Yu"modstandard.hpp" /FD /c
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
# ADD BASE LINK32 msad.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 oledb.lib msdasc.lib modulecore.lib privlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\TEMPLAT0"
# PROP BASE Intermediate_Dir ".\TEMPLAT0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_Debu\mbcs"
# PROP Intermediate_Dir ".\x86_Debu\mbcs"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /c
# SUBTRACT BASE CPP /Fr /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /Yu"modstandard.hpp" /FD /c
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
# ADD BASE LINK32 msad.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 oledb.lib msdasc.lib modulecore.lib privlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 (ALPHA) axp Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\ICOLINFO"
# PROP BASE Intermediate_Dir ".\ICOLINFO"
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
# ADD BASE LINK32 modulecore.lib privlib.lib msad.lib /nologo /subsystem:windows /dll /debug /machine:ALPHA
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 oledb.lib msdasc.lib modulecore.lib privlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:ALPHA
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug TT"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\ICOLINFO"
# PROP BASE Intermediate_Dir ".\ICOLINFO"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_Debu"
# PROP Intermediate_Dir ".\x86_Debu"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /Yu"modstandard.hpp" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /D "TEMPTBL" /Yu"modstandard.hpp" /FD /c
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
# ADD BASE LINK32 oledb.lib msdasc.lib modulecore.lib privlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 oledb.lib msdasc.lib modulecore.lib privlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS TT"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\ICOLINF0"
# PROP BASE Intermediate_Dir ".\ICOLINF0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_Debu\mbcs"
# PROP Intermediate_Dir ".\x86_Debu\mbcs"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /Yu"modstandard.hpp" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_AFXEXT" /D "_AFXDLL" /D "TEMPTBL" /Yu"modstandard.hpp" /FD /c
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
# ADD BASE LINK32 oledb.lib msdasc.lib modulecore.lib privlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 oledb.lib msdasc.lib modulecore.lib privlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT LINK32 /pdb:none /map

!ENDIF 

# Begin Target

# Name "ICOLINFO - Win32 x86 Debug"
# Name "ICOLINFO - Win32 x86 Debug MBCS"
# Name "ICOLINFO - Win32 (ALPHA) axp Debug"
# Name "ICOLINFO - Win32 x86 Debug TT"
# Name "ICOLINFO - Win32 x86 Debug MBCS TT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Modsrc\ExtraLib.cpp

!IF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 (ALPHA) axp Debug"

DEP_CPP_EXTRA=\
	{$(INCLUDE)}"ccol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"cmodinfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"cstorage.hpp"\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"ExtraLib.h"\
	{$(INCLUDE)}"list.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modstandard.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	{$(INCLUDE)}"msdadc.h"\
	{$(INCLUDE)}"msdaguid.h"\
	{$(INCLUDE)}"msdasc.h"\
	{$(INCLUDE)}"msdasql.h"\
	{$(INCLUDE)}"msdatt.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"strings.h"\
	{$(INCLUDE)}"transact.h"\
	

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug TT"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS TT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\ICOLINFO.cpp

!IF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 (ALPHA) axp Debug"

DEP_CPP_ICOLI=\
	{$(INCLUDE)}"ccol.hpp"\
	{$(INCLUDE)}"cexterr.hpp"\
	{$(INCLUDE)}"cmodinfo.hpp"\
	{$(INCLUDE)}"coledb.hpp"\
	{$(INCLUDE)}"cparsestrm.hpp"\
	{$(INCLUDE)}"cstorage.hpp"\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"ctable.hpp"\
	{$(INCLUDE)}"ctranstn.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"ExtraLib.h"\
	{$(INCLUDE)}"ICOLINFO.H"\
	{$(INCLUDE)}"list.h"\
	{$(INCLUDE)}"miscfunc.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modstandard.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	{$(INCLUDE)}"msdadc.h"\
	{$(INCLUDE)}"msdaguid.h"\
	{$(INCLUDE)}"msdasc.h"\
	{$(INCLUDE)}"msdasql.h"\
	{$(INCLUDE)}"msdatt.h"\
	{$(INCLUDE)}"oledb.h"\
	{$(INCLUDE)}"oledberr.h"\
	{$(INCLUDE)}"privcnst.h"\
	{$(INCLUDE)}"privlib.h"\
	{$(INCLUDE)}"prvtrace.h"\
	{$(INCLUDE)}"strings.h"\
	{$(INCLUDE)}"transact.h"\
	
# ADD BASE CPP /Gt0 /Yu"modstandard.hpp"
# ADD CPP /Gt0 /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug TT"

# ADD BASE CPP /Yu"modstandard.hpp"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS TT"

# ADD BASE CPP /Yu"modstandard.hpp"
# ADD CPP /Yu"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\Res\ICOLINFO.rc
# ADD BASE RSC /l 0x409 /i "Modsrc\Res"
# ADD RSC /l 0x409 /i "Modsrc\Res" /i ".\Modsrc\Res"
# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODStandard.cpp

!IF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug"

# ADD CPP /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS"

# ADD CPP /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 (ALPHA) axp Debug"

DEP_CPP_MODST=\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modstandard.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	
# ADD BASE CPP /Gt0 /Yc"modstandard.hpp"
# ADD CPP /Gt0 /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug TT"

# ADD BASE CPP /Yc"modstandard.hpp"
# ADD CPP /Yc"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS TT"

# ADD BASE CPP /Yc"modstandard.hpp"
# ADD CPP /Yc"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODSTUB.CPP

!IF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS"

# ADD BASE CPP /Yu"stdafx.h"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 (ALPHA) axp Debug"

DEP_CPP_MODSTU=\
	{$(INCLUDE)}"csuperlog.hpp"\
	{$(INCLUDE)}"dtmcommon.h"\
	{$(INCLUDE)}"modclasses.hpp"\
	{$(INCLUDE)}"modcommon.hpp"\
	{$(INCLUDE)}"moderror.hpp"\
	{$(INCLUDE)}"modmacros.hpp"\
	{$(INCLUDE)}"modstandard.hpp"\
	{$(INCLUDE)}"modulecore.h"\
	
# ADD BASE CPP /Gt0 /Yu"modstandard.hpp"
# ADD CPP /Gt0 /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug TT"

# ADD BASE CPP /Yu"modstandard.hpp"
# ADD CPP /Yu"modstandard.hpp"

!ELSEIF  "$(CFG)" == "ICOLINFO - Win32 x86 Debug MBCS TT"

# ADD BASE CPP /Yu"modstandard.hpp"
# ADD CPP /Yu"modstandard.hpp"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Modsrc\MODSTUB.DEF
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Include\ExtraLib.h
# End Source File
# Begin Source File

SOURCE=.\Include\ICOLINFO.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
