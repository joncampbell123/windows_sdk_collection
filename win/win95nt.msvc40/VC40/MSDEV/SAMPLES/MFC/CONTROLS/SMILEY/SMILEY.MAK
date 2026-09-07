# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Smiley - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Smiley - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Smiley - Win32 Release" && "$(CFG)" != "Smiley - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "smiley.mak" CFG="Smiley - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Smiley - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Smiley - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Smiley - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Smiley - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\smiley.exe"

CLEAN : 
	-@erase ".\Release\smiley.exe"
	-@erase ".\Release\stdafx.obj"
	-@erase ".\Release\smileydg.obj"
	-@erase ".\Release\smiley.pch"
	-@erase ".\Release\smilectl.obj"
	-@erase ".\Release\smiley.obj"
	-@erase ".\Release\smiley.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/smiley.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c\
 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/smiley.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/smiley.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/smiley.pdb" /machine:I386 /out:"$(OUTDIR)/smiley.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/smileydg.obj" \
	"$(INTDIR)/smilectl.obj" \
	"$(INTDIR)/smiley.obj" \
	"$(INTDIR)/smiley.res"

"$(OUTDIR)\smiley.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Smiley - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\smiley.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\smiley.pch"
	-@erase ".\Debug\smiley.exe"
	-@erase ".\Debug\smiley.obj"
	-@erase ".\Debug\stdafx.obj"
	-@erase ".\Debug\smileydg.obj"
	-@erase ".\Debug\smilectl.obj"
	-@erase ".\Debug\smiley.res"
	-@erase ".\Debug\smiley.ilk"
	-@erase ".\Debug\smiley.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/smiley.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/smiley.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/smiley.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/smiley.pdb" /debug /machine:I386 /out:"$(OUTDIR)/smiley.exe" 
LINK32_OBJS= \
	"$(INTDIR)/smiley.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/smileydg.obj" \
	"$(INTDIR)/smilectl.obj" \
	"$(INTDIR)/smiley.res"

"$(OUTDIR)\smiley.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Smiley - Win32 Release"
# Name "Smiley - Win32 Debug"

!IF  "$(CFG)" == "Smiley - Win32 Release"

!ELSEIF  "$(CFG)" == "Smiley - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "Smiley - Win32 Release"

!ELSEIF  "$(CFG)" == "Smiley - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\smiley.cpp
DEP_CPP_SMILE=\
	".\smiley.h"\
	".\smileydg.h"\
	".\smilectl.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Smiley - Win32 Release"


"$(INTDIR)\smiley.obj" : $(SOURCE) $(DEP_CPP_SMILE) "$(INTDIR)"\
 "$(INTDIR)\smiley.pch"


!ELSEIF  "$(CFG)" == "Smiley - Win32 Debug"


"$(INTDIR)\smiley.obj" : $(SOURCE) $(DEP_CPP_SMILE) "$(INTDIR)"\
 "$(INTDIR)\smiley.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SMILE) "$(INTDIR)"\
 "$(INTDIR)\smiley.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\smileydg.cpp
DEP_CPP_SMILEY=\
	".\smiley.h"\
	".\smileydg.h"\
	".\smilectl.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Smiley - Win32 Release"


"$(INTDIR)\smileydg.obj" : $(SOURCE) $(DEP_CPP_SMILEY) "$(INTDIR)"\
 "$(INTDIR)\smiley.pch"


!ELSEIF  "$(CFG)" == "Smiley - Win32 Debug"


"$(INTDIR)\smileydg.obj" : $(SOURCE) $(DEP_CPP_SMILEY) "$(INTDIR)"\
 "$(INTDIR)\smiley.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SMILEY) "$(INTDIR)"\
 "$(INTDIR)\smiley.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Smiley - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/smiley.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\smiley.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Smiley - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/smiley.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\smiley.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\smiley.rc
DEP_RSC_SMILEY_=\
	".\res\smiley.ico"\
	".\res\smiley.rc2"\
	

"$(INTDIR)\smiley.res" : $(SOURCE) $(DEP_RSC_SMILEY_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\smilectl.cpp
DEP_CPP_SMILEC=\
	".\smilectl.h"\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "Smiley - Win32 Release"


"$(INTDIR)\smilectl.obj" : $(SOURCE) $(DEP_CPP_SMILEC) "$(INTDIR)"\
 "$(INTDIR)\smiley.pch"


!ELSEIF  "$(CFG)" == "Smiley - Win32 Debug"


"$(INTDIR)\smilectl.obj" : $(SOURCE) $(DEP_CPP_SMILEC) "$(INTDIR)"\
 "$(INTDIR)\smiley.pch"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SMILEC) "$(INTDIR)"\
 "$(INTDIR)\smiley.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section Smiley : {175CB003-BEED-11CE-9611-00AA004A75CF}
# 	0:8:Font.cpp:D:\samples\Smiley\Font.cpp
# 	0:9:Smile.cpp:D:\samples\Smiley\Smile.cpp
# 	0:6:Font.h:D:\samples\Smiley\Font.h
# 	0:7:Smile.h:D:\samples\Smiley\Smile.h
# 	2:21:DefaultSinkHeaderFile:smilectl.h
# 	2:16:DefaultSinkClass:CSmileCtl
# End Section
################################################################################
################################################################################
# Section OLE Controls
# 	{175CB003-BEED-11CE-9611-00AA004A75CF}
# End Section
################################################################################
################################################################################
# Section Smiley : {175CB001-BEED-11CE-9611-00AA004A75CF}
# 	2:5:Class:CSmileCtl
# 	2:10:HeaderFile:smilectl.h
# 	2:8:ImplFile:smilectl.cpp
# End Section
################################################################################
