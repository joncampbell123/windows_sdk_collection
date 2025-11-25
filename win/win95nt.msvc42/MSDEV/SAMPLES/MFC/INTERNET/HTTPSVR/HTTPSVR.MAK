# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Power Macintosh Application" 0x0401

!IF "$(CFG)" == ""
CFG=HttpSvr - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to HttpSvr - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "HttpSvr - Win32 Release" && "$(CFG)" !=\
 "HttpSvr - Win32 Debug" && "$(CFG)" != "HttpSvr - Power Macintosh Release" &&\
 "$(CFG)" != "HttpSvr - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "httpsvr.mak" CFG="HttpSvr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HttpSvr - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "HttpSvr - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "HttpSvr - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "HttpSvr - Power Macintosh Debug" (based on\
 "Power Macintosh Application")
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
# PROP Target_Last_Scanned "HttpSvr - Power Macintosh Debug"

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\httpsvr.exe"

CLEAN : 
	-@erase "$(INTDIR)\GenPage.obj"
	-@erase "$(INTDIR)\Http.obj"
	-@erase "$(INTDIR)\HttpDoc.obj"
	-@erase "$(INTDIR)\HttpSvr.obj"
	-@erase "$(INTDIR)\httpsvr.pch"
	-@erase "$(INTDIR)\HttpSvr.res"
	-@erase "$(INTDIR)\HttpView.obj"
	-@erase "$(INTDIR)\Listen.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\NamePage.obj"
	-@erase "$(INTDIR)\NoRoot.obj"
	-@erase "$(INTDIR)\ReqSock.obj"
	-@erase "$(INTDIR)\Request.obj"
	-@erase "$(INTDIR)\RootDlg.obj"
	-@erase "$(INTDIR)\RootPage.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\httpsvr.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/httpsvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/HttpSvr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/httpsvr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/httpsvr.pdb" /machine:I386 /out:"$(OUTDIR)/httpsvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\GenPage.obj" \
	"$(INTDIR)\Http.obj" \
	"$(INTDIR)\HttpDoc.obj" \
	"$(INTDIR)\HttpSvr.obj" \
	"$(INTDIR)\HttpSvr.res" \
	"$(INTDIR)\HttpView.obj" \
	"$(INTDIR)\Listen.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\NamePage.obj" \
	"$(INTDIR)\NoRoot.obj" \
	"$(INTDIR)\ReqSock.obj" \
	"$(INTDIR)\Request.obj" \
	"$(INTDIR)\RootDlg.obj" \
	"$(INTDIR)\RootPage.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\httpsvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\httpsvr.exe" "$(OUTDIR)\httpsvr.bsc"

CLEAN : 
	-@erase "$(INTDIR)\GenPage.obj"
	-@erase "$(INTDIR)\GenPage.sbr"
	-@erase "$(INTDIR)\Http.obj"
	-@erase "$(INTDIR)\Http.sbr"
	-@erase "$(INTDIR)\HttpDoc.obj"
	-@erase "$(INTDIR)\HttpDoc.sbr"
	-@erase "$(INTDIR)\HttpSvr.obj"
	-@erase "$(INTDIR)\httpsvr.pch"
	-@erase "$(INTDIR)\HttpSvr.res"
	-@erase "$(INTDIR)\HttpSvr.sbr"
	-@erase "$(INTDIR)\HttpView.obj"
	-@erase "$(INTDIR)\HttpView.sbr"
	-@erase "$(INTDIR)\Listen.obj"
	-@erase "$(INTDIR)\Listen.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\NamePage.obj"
	-@erase "$(INTDIR)\NamePage.sbr"
	-@erase "$(INTDIR)\NoRoot.obj"
	-@erase "$(INTDIR)\NoRoot.sbr"
	-@erase "$(INTDIR)\ReqSock.obj"
	-@erase "$(INTDIR)\ReqSock.sbr"
	-@erase "$(INTDIR)\Request.obj"
	-@erase "$(INTDIR)\Request.sbr"
	-@erase "$(INTDIR)\RootDlg.obj"
	-@erase "$(INTDIR)\RootDlg.sbr"
	-@erase "$(INTDIR)\RootPage.obj"
	-@erase "$(INTDIR)\RootPage.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\httpsvr.bsc"
	-@erase "$(OUTDIR)\httpsvr.exe"
	-@erase "$(OUTDIR)\httpsvr.ilk"
	-@erase "$(OUTDIR)\httpsvr.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/httpsvr.pch"\
 /Yu"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/HttpSvr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/httpsvr.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\GenPage.sbr" \
	"$(INTDIR)\Http.sbr" \
	"$(INTDIR)\HttpDoc.sbr" \
	"$(INTDIR)\HttpSvr.sbr" \
	"$(INTDIR)\HttpView.sbr" \
	"$(INTDIR)\Listen.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\NamePage.sbr" \
	"$(INTDIR)\NoRoot.sbr" \
	"$(INTDIR)\ReqSock.sbr" \
	"$(INTDIR)\Request.sbr" \
	"$(INTDIR)\RootDlg.sbr" \
	"$(INTDIR)\RootPage.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\httpsvr.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/httpsvr.pdb" /debug /machine:I386 /out:"$(OUTDIR)/httpsvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\GenPage.obj" \
	"$(INTDIR)\Http.obj" \
	"$(INTDIR)\HttpDoc.obj" \
	"$(INTDIR)\HttpSvr.obj" \
	"$(INTDIR)\HttpSvr.res" \
	"$(INTDIR)\HttpView.obj" \
	"$(INTDIR)\Listen.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\NamePage.obj" \
	"$(INTDIR)\NoRoot.obj" \
	"$(INTDIR)\ReqSock.obj" \
	"$(INTDIR)\Request.obj" \
	"$(INTDIR)\RootDlg.obj" \
	"$(INTDIR)\RootPage.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\httpsvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PMacRel"
# PROP BASE Intermediate_Dir "PMacRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PMacRel"
# PROP Intermediate_Dir "PMacRel"
# PROP Target_Dir ""
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\httpsvr.exe" "$(OUTDIR)\httpsvr.trg"

CLEAN : 
	-@erase "$(INTDIR)\GenPage.obj"
	-@erase "$(INTDIR)\Http.obj"
	-@erase "$(INTDIR)\HttpDoc.obj"
	-@erase "$(INTDIR)\HttpMac.rsc"
	-@erase "$(INTDIR)\HttpSvr.obj"
	-@erase "$(INTDIR)\httpsvr.pch"
	-@erase "$(INTDIR)\HttpSvr.rsc"
	-@erase "$(INTDIR)\HttpView.obj"
	-@erase "$(INTDIR)\Listen.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\NamePage.obj"
	-@erase "$(INTDIR)\NoRoot.obj"
	-@erase "$(INTDIR)\ReqSock.obj"
	-@erase "$(INTDIR)\Request.obj"
	-@erase "$(INTDIR)\RootDlg.obj"
	-@erase "$(INTDIR)\RootPage.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(OUTDIR)\httpsvr.exe"
	-@erase "$(OUTDIR)\httpsvr.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /ppc
# ADD MTL /nologo /D "NDEBUG" /ppc
MTL_PROJ=/nologo /D "NDEBUG" /ppc 
CPP=cl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "c:\projects\mfcsock" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "c:\projects\mfcsock" /D "_MAC" /D "_MPPC_"\
 /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL"\
 /Fp"$(INTDIR)/httpsvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/HttpSvr.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/httpsvr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="HTTP" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="HTTP"\
 /incremental:no /pdb:"$(OUTDIR)/httpsvr.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/httpsvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\GenPage.obj" \
	"$(INTDIR)\Http.obj" \
	"$(INTDIR)\HttpDoc.obj" \
	"$(INTDIR)\HttpMac.rsc" \
	"$(INTDIR)\HttpSvr.obj" \
	"$(INTDIR)\HttpSvr.rsc" \
	"$(INTDIR)\HttpView.obj" \
	"$(INTDIR)\Listen.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\NamePage.obj" \
	"$(INTDIR)\NoRoot.obj" \
	"$(INTDIR)\ReqSock.obj" \
	"$(INTDIR)\Request.obj" \
	"$(INTDIR)\RootDlg.obj" \
	"$(INTDIR)\RootPage.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\httpsvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)\httpsvr.exe"

"$(OUTDIR)\httpsvr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\httpsvr.exe\
 "$(MFILE32_DEST):httpsvr.exe">"$(OUTDIR)\httpsvr.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\httpsvr.exe\
 "$(MFILE32_DEST):httpsvr.exe">"$(OUTDIR)\httpsvr.trg"

!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PMacDbg"
# PROP BASE Intermediate_Dir "PMacDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "PMacDbg"
# PROP Intermediate_Dir "PMacDbg"
# PROP Target_Dir ""
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\httpsvr.exe" "$(OUTDIR)\httpsvr.bsc" "$(OUTDIR)\httpsvr.trg"

CLEAN : 
	-@erase "$(INTDIR)\GenPage.obj"
	-@erase "$(INTDIR)\GenPage.sbr"
	-@erase "$(INTDIR)\Http.obj"
	-@erase "$(INTDIR)\Http.sbr"
	-@erase "$(INTDIR)\HttpDoc.obj"
	-@erase "$(INTDIR)\HttpDoc.sbr"
	-@erase "$(INTDIR)\HttpMac.rsc"
	-@erase "$(INTDIR)\HttpSvr.obj"
	-@erase "$(INTDIR)\httpsvr.pch"
	-@erase "$(INTDIR)\HttpSvr.rsc"
	-@erase "$(INTDIR)\HttpSvr.sbr"
	-@erase "$(INTDIR)\HttpView.obj"
	-@erase "$(INTDIR)\HttpView.sbr"
	-@erase "$(INTDIR)\Listen.obj"
	-@erase "$(INTDIR)\Listen.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\NamePage.obj"
	-@erase "$(INTDIR)\NamePage.sbr"
	-@erase "$(INTDIR)\NoRoot.obj"
	-@erase "$(INTDIR)\NoRoot.sbr"
	-@erase "$(INTDIR)\ReqSock.obj"
	-@erase "$(INTDIR)\ReqSock.sbr"
	-@erase "$(INTDIR)\Request.obj"
	-@erase "$(INTDIR)\Request.sbr"
	-@erase "$(INTDIR)\RootDlg.obj"
	-@erase "$(INTDIR)\RootDlg.sbr"
	-@erase "$(INTDIR)\RootPage.obj"
	-@erase "$(INTDIR)\RootPage.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\httpsvr.bsc"
	-@erase "$(OUTDIR)\httpsvr.exe"
	-@erase "$(OUTDIR)\httpsvr.ilk"
	-@erase "$(OUTDIR)\httpsvr.pdb"
	-@erase "$(OUTDIR)\httpsvr.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /ppc
# ADD MTL /nologo /D "_DEBUG" /ppc
MTL_PROJ=/nologo /D "_DEBUG" /ppc 
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /QPm /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "c:\projects\mfcsock" /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /QPm /c
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /I "c:\projects\mfcsock" /D "_MAC" /D\
 "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/httpsvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /QPm /c 
CPP_OBJS=.\PMacDbg/
CPP_SBRS=.\PMacDbg/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /r /m /fo"$(INTDIR)/HttpSvr.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" /d "_AFXDLL" 
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/httpsvr.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\GenPage.sbr" \
	"$(INTDIR)\Http.sbr" \
	"$(INTDIR)\HttpDoc.sbr" \
	"$(INTDIR)\HttpSvr.sbr" \
	"$(INTDIR)\HttpView.sbr" \
	"$(INTDIR)\Listen.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\NamePage.sbr" \
	"$(INTDIR)\NoRoot.sbr" \
	"$(INTDIR)\ReqSock.sbr" \
	"$(INTDIR)\Request.sbr" \
	"$(INTDIR)\RootDlg.sbr" \
	"$(INTDIR)\RootPage.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\httpsvr.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="HTTP" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="HTTP"\
 /incremental:yes /pdb:"$(OUTDIR)/httpsvr.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/httpsvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\GenPage.obj" \
	"$(INTDIR)\Http.obj" \
	"$(INTDIR)\HttpDoc.obj" \
	"$(INTDIR)\HttpMac.rsc" \
	"$(INTDIR)\HttpSvr.obj" \
	"$(INTDIR)\HttpSvr.rsc" \
	"$(INTDIR)\HttpView.obj" \
	"$(INTDIR)\Listen.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\NamePage.obj" \
	"$(INTDIR)\NoRoot.obj" \
	"$(INTDIR)\ReqSock.obj" \
	"$(INTDIR)\Request.obj" \
	"$(INTDIR)\RootDlg.obj" \
	"$(INTDIR)\RootPage.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\httpsvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)\httpsvr.exe"

"$(OUTDIR)\httpsvr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\httpsvr.exe\
 "$(MFILE32_DEST):httpsvr.exe">"$(OUTDIR)\httpsvr.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\httpsvr.exe\
 "$(MFILE32_DEST):httpsvr.exe">"$(OUTDIR)\httpsvr.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "HttpSvr - Win32 Release"
# Name "HttpSvr - Win32 Debug"
# Name "HttpSvr - Power Macintosh Release"
# Name "HttpSvr - Power Macintosh Debug"

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HttpSvr.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_HTTPS=\
	".\genpage.h"\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\httpview.h"\
	".\listen.h"\
	".\mainfrm.h"\
	".\namepage.h"\
	".\reqsock.h"\
	".\RootPage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpSvr.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_HTTPS=\
	".\genpage.h"\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\httpview.h"\
	".\listen.h"\
	".\mainfrm.h"\
	".\namepage.h"\
	".\reqsock.h"\
	".\RootPage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpSvr.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\HttpSvr.sbr" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_HTTPS=\
	".\genpage.h"\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\httpview.h"\
	".\listen.h"\
	".\mainfrm.h"\
	".\namepage.h"\
	".\reqsock.h"\
	".\RootPage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpSvr.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_HTTPS=\
	".\genpage.h"\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\httpview.h"\
	".\listen.h"\
	".\mainfrm.h"\
	".\namepage.h"\
	".\reqsock.h"\
	".\RootPage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpSvr.obj" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\HttpSvr.sbr" : $(SOURCE) $(DEP_CPP_HTTPS) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/httpsvr.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\httpsvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/httpsvr.pch"\
 /Yc"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\httpsvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /I "c:\projects\mfcsock" /D "_MAC" /D "_MPPC_"\
 /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL"\
 /Fp"$(INTDIR)/httpsvr.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\httpsvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_STDAF=\
	".\stdafx.h"\
	
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /GX /Zi /Od /I "c:\projects\mfcsock" /D "_MAC" /D\
 "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL"\
 /FR"$(INTDIR)/" /Fp"$(INTDIR)/httpsvr.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /QPm /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\httpsvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_MAINF=\
	".\httpsvr.h"\
	".\listen.h"\
	".\mainfrm.h"\
	".\reqsock.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_MAINF=\
	".\httpsvr.h"\
	".\listen.h"\
	".\mainfrm.h"\
	".\reqsock.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_MAINF=\
	".\httpsvr.h"\
	".\listen.h"\
	".\mainfrm.h"\
	".\reqsock.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_MAINF=\
	".\httpsvr.h"\
	".\listen.h"\
	".\mainfrm.h"\
	".\reqsock.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HttpSvr.rc

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_RSC_HTTPSV=\
	".\res\httpdoc.ico"\
	".\res\httpsvr.rc2"\
	".\res\images.bmp"\
	".\res\mainfrm.ico"\
	".\res\smimages.bmp"\
	

"$(INTDIR)\HttpSvr.res" : $(SOURCE) $(DEP_RSC_HTTPSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_RSC_HTTPSV=\
	".\res\httpdoc.ico"\
	".\res\httpsvr.rc2"\
	".\res\images.bmp"\
	".\res\mainfrm.ico"\
	".\res\smimages.bmp"\
	

"$(INTDIR)\HttpSvr.res" : $(SOURCE) $(DEP_RSC_HTTPSV) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_RSC_HTTPSV=\
	".\res\httpdoc.ico"\
	".\res\httpsvr.rc2"\
	".\res\images.bmp"\
	".\res\mainfrm.ico"\
	".\res\smimages.bmp"\
	

"$(INTDIR)\HttpSvr.rsc" : $(SOURCE) $(DEP_RSC_HTTPSV) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/HttpSvr.rsc" /d "_MAC" /d "_MPPC_" /d\
 "NDEBUG" /d "_AFXDLL" $(SOURCE)


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_RSC_HTTPSV=\
	".\res\httpdoc.ico"\
	".\res\httpsvr.rc2"\
	".\res\images.bmp"\
	".\res\mainfrm.ico"\
	".\res\smimages.bmp"\
	

"$(INTDIR)\HttpSvr.rsc" : $(SOURCE) $(DEP_RSC_HTTPSV) "$(INTDIR)"
   $(RSC) /l 0x409 /r /m /fo"$(INTDIR)/HttpSvr.rsc" /d "_MAC" /d "_MPPC_" /d\
 "_DEBUG" /d "_AFXDLL" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Http.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_HTTP_=\
	".\http.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Http.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_HTTP_=\
	".\http.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Http.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\Http.sbr" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_HTTP_=\
	".\http.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Http.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_HTTP_=\
	".\http.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Http.obj" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\Http.sbr" : $(SOURCE) $(DEP_CPP_HTTP_) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RootPage.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_ROOTP=\
	".\httpsvr.h"\
	".\listen.h"\
	".\RootPage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\RootPage.obj" : $(SOURCE) $(DEP_CPP_ROOTP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_ROOTP=\
	".\httpsvr.h"\
	".\listen.h"\
	".\RootPage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\RootPage.obj" : $(SOURCE) $(DEP_CPP_ROOTP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\RootPage.sbr" : $(SOURCE) $(DEP_CPP_ROOTP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_ROOTP=\
	".\httpsvr.h"\
	".\listen.h"\
	".\RootPage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\RootPage.obj" : $(SOURCE) $(DEP_CPP_ROOTP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_ROOTP=\
	".\httpsvr.h"\
	".\listen.h"\
	".\RootPage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\RootPage.obj" : $(SOURCE) $(DEP_CPP_ROOTP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\RootPage.sbr" : $(SOURCE) $(DEP_CPP_ROOTP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Listen.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_LISTE=\
	".\listen.h"\
	".\reqsock.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Listen.obj" : $(SOURCE) $(DEP_CPP_LISTE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_LISTE=\
	".\listen.h"\
	".\reqsock.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Listen.obj" : $(SOURCE) $(DEP_CPP_LISTE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\Listen.sbr" : $(SOURCE) $(DEP_CPP_LISTE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_LISTE=\
	".\listen.h"\
	".\reqsock.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Listen.obj" : $(SOURCE) $(DEP_CPP_LISTE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_LISTE=\
	".\listen.h"\
	".\reqsock.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Listen.obj" : $(SOURCE) $(DEP_CPP_LISTE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\Listen.sbr" : $(SOURCE) $(DEP_CPP_LISTE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ReqSock.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_REQSO=\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\reqsock.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ReqSock.obj" : $(SOURCE) $(DEP_CPP_REQSO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_REQSO=\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\reqsock.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ReqSock.obj" : $(SOURCE) $(DEP_CPP_REQSO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\ReqSock.sbr" : $(SOURCE) $(DEP_CPP_REQSO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_REQSO=\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\reqsock.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ReqSock.obj" : $(SOURCE) $(DEP_CPP_REQSO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_REQSO=\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\reqsock.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ReqSock.obj" : $(SOURCE) $(DEP_CPP_REQSO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\ReqSock.sbr" : $(SOURCE) $(DEP_CPP_REQSO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Request.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_REQUE=\
	".\httpsvr.h"\
	".\listen.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Request.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_REQUE=\
	".\httpsvr.h"\
	".\listen.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Request.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\Request.sbr" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_REQUE=\
	".\httpsvr.h"\
	".\listen.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Request.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_REQUE=\
	".\httpsvr.h"\
	".\listen.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Request.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\Request.sbr" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GenPage.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_GENPA=\
	".\genpage.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\stdafx.h"\
	

"$(INTDIR)\GenPage.obj" : $(SOURCE) $(DEP_CPP_GENPA) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_GENPA=\
	".\genpage.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\stdafx.h"\
	

"$(INTDIR)\GenPage.obj" : $(SOURCE) $(DEP_CPP_GENPA) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\GenPage.sbr" : $(SOURCE) $(DEP_CPP_GENPA) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_GENPA=\
	".\genpage.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\stdafx.h"\
	

"$(INTDIR)\GenPage.obj" : $(SOURCE) $(DEP_CPP_GENPA) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_GENPA=\
	".\genpage.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\stdafx.h"\
	

"$(INTDIR)\GenPage.obj" : $(SOURCE) $(DEP_CPP_GENPA) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\GenPage.sbr" : $(SOURCE) $(DEP_CPP_GENPA) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RootDlg.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_ROOTD=\
	".\httpsvr.h"\
	".\listen.h"\
	".\rootdlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\RootDlg.obj" : $(SOURCE) $(DEP_CPP_ROOTD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_ROOTD=\
	".\httpsvr.h"\
	".\listen.h"\
	".\rootdlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\RootDlg.obj" : $(SOURCE) $(DEP_CPP_ROOTD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\RootDlg.sbr" : $(SOURCE) $(DEP_CPP_ROOTD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_ROOTD=\
	".\httpsvr.h"\
	".\listen.h"\
	".\rootdlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\RootDlg.obj" : $(SOURCE) $(DEP_CPP_ROOTD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_ROOTD=\
	".\httpsvr.h"\
	".\listen.h"\
	".\rootdlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\RootDlg.obj" : $(SOURCE) $(DEP_CPP_ROOTD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\RootDlg.sbr" : $(SOURCE) $(DEP_CPP_ROOTD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HttpDoc.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_HTTPD=\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpDoc.obj" : $(SOURCE) $(DEP_CPP_HTTPD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_HTTPD=\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpDoc.obj" : $(SOURCE) $(DEP_CPP_HTTPD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\HttpDoc.sbr" : $(SOURCE) $(DEP_CPP_HTTPD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_HTTPD=\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpDoc.obj" : $(SOURCE) $(DEP_CPP_HTTPD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_HTTPD=\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\listen.h"\
	".\request.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpDoc.obj" : $(SOURCE) $(DEP_CPP_HTTPD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\HttpDoc.sbr" : $(SOURCE) $(DEP_CPP_HTTPD) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HttpMac.r

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"


"$(INTDIR)\HttpMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/HttpMac.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"


"$(INTDIR)\HttpMac.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/HttpMac.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\HttpView.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_HTTPV=\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\httpview.h"\
	".\listen.h"\
	".\NoRoot.h"\
	".\request.h"\
	".\rootdlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpView.obj" : $(SOURCE) $(DEP_CPP_HTTPV) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_HTTPV=\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\httpview.h"\
	".\listen.h"\
	".\NoRoot.h"\
	".\request.h"\
	".\rootdlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpView.obj" : $(SOURCE) $(DEP_CPP_HTTPV) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\HttpView.sbr" : $(SOURCE) $(DEP_CPP_HTTPV) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_HTTPV=\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\httpview.h"\
	".\listen.h"\
	".\NoRoot.h"\
	".\request.h"\
	".\rootdlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpView.obj" : $(SOURCE) $(DEP_CPP_HTTPV) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_HTTPV=\
	".\http.h"\
	".\httpdoc.h"\
	".\httpsvr.h"\
	".\httpview.h"\
	".\listen.h"\
	".\NoRoot.h"\
	".\request.h"\
	".\rootdlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\HttpView.obj" : $(SOURCE) $(DEP_CPP_HTTPV) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\HttpView.sbr" : $(SOURCE) $(DEP_CPP_HTTPV) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NamePage.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_NAMEP=\
	".\httpsvr.h"\
	".\listen.h"\
	".\namepage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\NamePage.obj" : $(SOURCE) $(DEP_CPP_NAMEP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_NAMEP=\
	".\httpsvr.h"\
	".\listen.h"\
	".\namepage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\NamePage.obj" : $(SOURCE) $(DEP_CPP_NAMEP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\NamePage.sbr" : $(SOURCE) $(DEP_CPP_NAMEP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_NAMEP=\
	".\httpsvr.h"\
	".\listen.h"\
	".\namepage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\NamePage.obj" : $(SOURCE) $(DEP_CPP_NAMEP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_NAMEP=\
	".\httpsvr.h"\
	".\listen.h"\
	".\namepage.h"\
	".\stdafx.h"\
	

"$(INTDIR)\NamePage.obj" : $(SOURCE) $(DEP_CPP_NAMEP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\NamePage.sbr" : $(SOURCE) $(DEP_CPP_NAMEP) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NoRoot.cpp

!IF  "$(CFG)" == "HttpSvr - Win32 Release"

DEP_CPP_NOROO=\
	".\httpsvr.h"\
	".\listen.h"\
	".\NoRoot.h"\
	".\stdafx.h"\
	

"$(INTDIR)\NoRoot.obj" : $(SOURCE) $(DEP_CPP_NOROO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Win32 Debug"

DEP_CPP_NOROO=\
	".\httpsvr.h"\
	".\listen.h"\
	".\NoRoot.h"\
	".\stdafx.h"\
	

"$(INTDIR)\NoRoot.obj" : $(SOURCE) $(DEP_CPP_NOROO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\NoRoot.sbr" : $(SOURCE) $(DEP_CPP_NOROO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Release"

DEP_CPP_NOROO=\
	".\httpsvr.h"\
	".\listen.h"\
	".\NoRoot.h"\
	".\stdafx.h"\
	

"$(INTDIR)\NoRoot.obj" : $(SOURCE) $(DEP_CPP_NOROO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ELSEIF  "$(CFG)" == "HttpSvr - Power Macintosh Debug"

DEP_CPP_NOROO=\
	".\httpsvr.h"\
	".\listen.h"\
	".\NoRoot.h"\
	".\stdafx.h"\
	

"$(INTDIR)\NoRoot.obj" : $(SOURCE) $(DEP_CPP_NOROO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"

"$(INTDIR)\NoRoot.sbr" : $(SOURCE) $(DEP_CPP_NOROO) "$(INTDIR)"\
 "$(INTDIR)\httpsvr.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
