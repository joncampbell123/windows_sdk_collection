# Microsoft Developer Studio Project File - Name="PRIVLIB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (ALPHA) Static Library" 0x0604
# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=PRIVLIB - Win32 x86 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Privlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Privlib.mak" CFG="PRIVLIB - Win32 x86 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PRIVLIB - Win32 x86 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "PRIVLIB - Win32 (ALPHA) axp Debug" (based on\
 "Win32 (ALPHA) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PrivlibS"
# PROP BASE Intermediate_Dir "PrivlibS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\x86_debu"
# PROP Intermediate_Dir ".\x86_debu"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fd".\x86_debu/privlib.pdb" /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PrivlibS"
# PROP BASE Intermediate_Dir "PrivlibS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\axp_debu"
# PROP Intermediate_Dir ".\axp_debu"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fd".\axp_debu/privlib.pdb" /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "PRIVLIB - Win32 x86 Debug"
# Name "PRIVLIB - Win32 (ALPHA) axp Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SRC\CCOL.CPP

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CCOL_=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\ccommand.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CCOMM=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\oledb.hpp"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\cdso.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CDSO_=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\oledb.hpp"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\cexterr.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CEXTE=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CModInfo.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CMODI=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\sdasc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\trings.h"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\coledb.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_COLED=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\oledb.hpp"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CParseStrm.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CPARS=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\trings.h"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CRow.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CROW_=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\oledb.hpp"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\crowset.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CROWS=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\oledb.hpp"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\csession.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CSESS=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\oledb.hpp"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CStorage.CPP

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CSTOR=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\Storage.hpp"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CTABLE.CPP

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CTABL=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\ctranstn.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CTRAN=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\oledb.hpp"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\transtn.hpp"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\CTree.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_CTREE=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\exterr.hpp"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\oledb.hpp"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\init.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_INIT_=\
	".\ledb.h"\
	".\ledberr.h"\
	".\ransact.h"\
	".\sdadc.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\miscfunc.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_MISCF=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\Storage.hpp"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\trings.h"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\privstd.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yc"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_PRIVS=\
	".\clcommon.hpp"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\tmcommon.h"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yc"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\PRVTRACE.CPP

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_PRVTR=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SRC\Strings.cpp

!IF  "$(CFG)" == "PRIVLIB - Win32 x86 Debug"

# ADD CPP /Yu"privstd.h"

!ELSEIF  "$(CFG)" == "PRIVLIB - Win32 (ALPHA) axp Debug"

NODEP_CPP_STRIN=\
	".\clcommon.hpp"\
	".\COL.HPP"\
	".\iscfunc.h"\
	".\ist.h"\
	".\ledb.h"\
	".\ledberr.h"\
	".\ModInfo.hpp"\
	".\odclasses.hpp"\
	".\odcommon.hpp"\
	".\oderror.hpp"\
	".\odmacros.hpp"\
	".\odulecore.h"\
	".\ParseStrm.hpp"\
	".\ransact.h"\
	".\rivcnst.h"\
	".\rivstd.h"\
	".\RVTRACE.H"\
	".\sdadc.h"\
	".\superlog.hpp"\
	".\TABLE.HPP"\
	".\tmcommon.h"\
	".\Tree.hpp"\
	".\trings.h"\
	".\vcprov.h"\
	
# ADD BASE CPP /Gt0
# ADD CPP /Gt0 /Yu"privstd.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Include\CCOL.HPP
# End Source File
# Begin Source File

SOURCE=..\..\Include\cexterr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\CModInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\coledb.hpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\CParseStrm.hpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\CStorage.hpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\CTABLE.HPP
# End Source File
# Begin Source File

SOURCE=..\..\Include\ctranstn.hpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\CTree.hpp
# End Source File
# Begin Source File

SOURCE=..\..\Include\List.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\miscfunc.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\privcnst.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\PRIVLIB.H
# End Source File
# Begin Source File

SOURCE=..\..\Include\privstd.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\PRVTRACE.H
# End Source File
# Begin Source File

SOURCE=..\..\Include\Strings.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\svcprov.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\VERSION.H
# End Source File
# End Group
# End Target
# End Project
