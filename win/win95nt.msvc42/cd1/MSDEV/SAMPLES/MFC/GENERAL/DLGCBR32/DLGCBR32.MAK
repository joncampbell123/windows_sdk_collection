# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=DLGCBAR - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DLGCBAR - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DLGCBAR - Win32 Release" && "$(CFG)" !=\
 "DLGCBAR - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Dlgcbr32.mak" CFG="DLGCBAR - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DLGCBAR - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DLGCBAR - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "DLGCBAR - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DLGCBAR - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Dlgcbr32.exe"

CLEAN : 
	-@erase "$(INTDIR)\ABOUTDLG.OBJ"
	-@erase "$(INTDIR)\Dlgbars.obj"
	-@erase "$(INTDIR)\DLGCBAR.OBJ"
	-@erase "$(INTDIR)\DLGCBAR.res"
	-@erase "$(INTDIR)\Dlgcbr32.pch"
	-@erase "$(INTDIR)\MDLSMAIN.OBJ"
	-@erase "$(INTDIR)\MODELESS.OBJ"
	-@erase "$(INTDIR)\RATEDLG.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\WNDLIST.OBJ"
	-@erase "$(OUTDIR)\Dlgcbr32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /WX /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"STDAFX.H" /c
# ADD CPP /nologo /MD /W3 /WX /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MD /W3 /WX /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Dlgcbr32.pch" /Yu"STDAFX.H" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DLGCBAR.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Dlgcbr32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /machine:IX86
# ADD LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /machine:IX86
# SUBTRACT LINK32 /map
LINK32_FLAGS=oldnames.lib /nologo /stack:0x2800 /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/Dlgcbr32.pdb" /machine:IX86\
 /out:"$(OUTDIR)/Dlgcbr32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ABOUTDLG.OBJ" \
	"$(INTDIR)\Dlgbars.obj" \
	"$(INTDIR)\DLGCBAR.OBJ" \
	"$(INTDIR)\DLGCBAR.res" \
	"$(INTDIR)\MDLSMAIN.OBJ" \
	"$(INTDIR)\MODELESS.OBJ" \
	"$(INTDIR)\RATEDLG.OBJ" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\WNDLIST.OBJ"

"$(OUTDIR)\Dlgcbr32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DLGCBAR - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Dlgcbr32.exe"

CLEAN : 
	-@erase "$(INTDIR)\ABOUTDLG.OBJ"
	-@erase "$(INTDIR)\Dlgbars.obj"
	-@erase "$(INTDIR)\DLGCBAR.OBJ"
	-@erase "$(INTDIR)\DLGCBAR.res"
	-@erase "$(INTDIR)\Dlgcbr32.pch"
	-@erase "$(INTDIR)\MDLSMAIN.OBJ"
	-@erase "$(INTDIR)\MODELESS.OBJ"
	-@erase "$(INTDIR)\RATEDLG.OBJ"
	-@erase "$(INTDIR)\STDAFX.OBJ"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\WNDLIST.OBJ"
	-@erase "$(OUTDIR)\Dlgcbr32.exe"
	-@erase "$(OUTDIR)\Dlgcbr32.ilk"
	-@erase "$(OUTDIR)\Dlgcbr32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"STDAFX.H" /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"STDAFX.H" /c
CPP_PROJ=/nologo /MDd /W3 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Dlgcbr32.pch" /Yu"STDAFX.H"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/DLGCBAR.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Dlgcbr32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /map /debug /machine:IX86
# ADD LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /debug /machine:IX86
# SUBTRACT LINK32 /map
LINK32_FLAGS=oldnames.lib /nologo /stack:0x2800 /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/Dlgcbr32.pdb" /debug /machine:IX86\
 /out:"$(OUTDIR)/Dlgcbr32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ABOUTDLG.OBJ" \
	"$(INTDIR)\Dlgbars.obj" \
	"$(INTDIR)\DLGCBAR.OBJ" \
	"$(INTDIR)\DLGCBAR.res" \
	"$(INTDIR)\MDLSMAIN.OBJ" \
	"$(INTDIR)\MODELESS.OBJ" \
	"$(INTDIR)\RATEDLG.OBJ" \
	"$(INTDIR)\STDAFX.OBJ" \
	"$(INTDIR)\WNDLIST.OBJ"

"$(OUTDIR)\Dlgcbr32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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

################################################################################
# Begin Target

# Name "DLGCBAR - Win32 Release"
# Name "DLGCBAR - Win32 Debug"

!IF  "$(CFG)" == "DLGCBAR - Win32 Release"

!ELSEIF  "$(CFG)" == "DLGCBAR - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\STDAFX.CPP
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "DLGCBAR - Win32 Release"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /WX /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Dlgcbr32.pch" /Yc"STDAFX.H" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Dlgcbr32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "DLGCBAR - Win32 Debug"

# ADD BASE CPP /Yc"STDAFX.H"
# ADD CPP /Yc"STDAFX.H"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Dlgcbr32.pch" /Yc"STDAFX.H"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\STDAFX.OBJ" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Dlgcbr32.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RATEDLG.CPP
DEP_CPP_RATED=\
	".\Ratedlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\RATEDLG.OBJ" : $(SOURCE) $(DEP_CPP_RATED) "$(INTDIR)"\
 "$(INTDIR)\Dlgcbr32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\DLGCBAR.CPP
DEP_CPP_DLGCB=\
	".\Aboutdlg.h"\
	".\Dlgbars.h"\
	".\Dlgcbar.h"\
	".\Mdlsmain.h"\
	".\Modeless.h"\
	".\stdafx.h"\
	".\Wndlist.h"\
	

"$(INTDIR)\DLGCBAR.OBJ" : $(SOURCE) $(DEP_CPP_DLGCB) "$(INTDIR)"\
 "$(INTDIR)\Dlgcbr32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABOUTDLG.CPP
DEP_CPP_ABOUT=\
	".\Aboutdlg.h"\
	".\stdafx.h"\
	

"$(INTDIR)\ABOUTDLG.OBJ" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\Dlgcbr32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\WNDLIST.CPP

!IF  "$(CFG)" == "DLGCBAR - Win32 Release"

DEP_CPP_WNDLI=\
	".\Ratedlg.h"\
	".\stdafx.h"\
	".\Wndlist.h"\
	

"$(INTDIR)\WNDLIST.OBJ" : $(SOURCE) $(DEP_CPP_WNDLI) "$(INTDIR)"\
 "$(INTDIR)\Dlgcbr32.pch"


!ELSEIF  "$(CFG)" == "DLGCBAR - Win32 Debug"

DEP_CPP_WNDLI=\
	".\Dlgbars.h"\
	".\Mdlsmain.h"\
	".\Modeless.h"\
	".\Ratedlg.h"\
	".\stdafx.h"\
	".\Wndlist.h"\
	

"$(INTDIR)\WNDLIST.OBJ" : $(SOURCE) $(DEP_CPP_WNDLI) "$(INTDIR)"\
 "$(INTDIR)\Dlgcbr32.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MODELESS.CPP
DEP_CPP_MODEL=\
	".\Modeless.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MODELESS.OBJ" : $(SOURCE) $(DEP_CPP_MODEL) "$(INTDIR)"\
 "$(INTDIR)\Dlgcbr32.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MDLSMAIN.CPP

!IF  "$(CFG)" == "DLGCBAR - Win32 Release"

DEP_CPP_MDLSM=\
	".\Dlgbars.h"\
	".\Mdlsmain.h"\
	".\Modeless.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MDLSMAIN.OBJ" : $(SOURCE) $(DEP_CPP_MDLSM) "$(INTDIR)"\
 "$(INTDIR)\Dlgcbr32.pch"


!ELSEIF  "$(CFG)" == "DLGCBAR - Win32 Debug"

DEP_CPP_MDLSM=\
	".\Mdlsmain.h"\
	".\stdafx.h"\
	

"$(INTDIR)\MDLSMAIN.OBJ" : $(SOURCE) $(DEP_CPP_MDLSM) "$(INTDIR)"\
 "$(INTDIR)\Dlgcbr32.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DLGCBAR.RC
DEP_RSC_DLGCBA=\
	".\RES\DLGCBAR.ICO"\
	".\res\Toolbar.bmp"\
	

"$(INTDIR)\DLGCBAR.res" : $(SOURCE) $(DEP_RSC_DLGCBA) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Dlgbars.cpp
DEP_CPP_DLGBA=\
	".\Dlgbars.h"\
	".\stdafx.h"\
	

"$(INTDIR)\Dlgbars.obj" : $(SOURCE) $(DEP_CPP_DLGBA) "$(INTDIR)"\
 "$(INTDIR)\Dlgcbr32.pch"


# End Source File
# End Target
# End Project
################################################################################
