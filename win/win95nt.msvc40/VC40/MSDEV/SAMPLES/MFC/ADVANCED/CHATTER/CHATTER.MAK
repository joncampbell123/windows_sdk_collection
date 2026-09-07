# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Chatter - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Chatter - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Chatter - Win32 Release" && "$(CFG)" !=\
 "Chatter - Win32 Debug" && "$(CFG)" != "Chatter - Win32 Unicode Release" &&\
 "$(CFG)" != "Chatter - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Chatter.mak" CFG="Chatter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Chatter - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Chatter - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Chatter - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "Chatter - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Chatter - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "Chatter - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Chatter.exe"

CLEAN : 
	-@erase ".\Release\Chatter.idb"
	-@erase ".\Release\Chatter.pch"
	-@erase ".\Release\Chatter.exe"
	-@erase ".\Release\Msg.obj"
	-@erase ".\Release\Chatter.obj"
	-@erase ".\Release\Setupdlg.obj"
	-@erase ".\Release\Chatvw.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Sendvw.obj"
	-@erase ".\Release\Chatsock.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Chatdoc.obj"
	-@erase ".\Release\Chatter.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Chatter.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Chatter.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chatter.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Chatter.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Chatter.pdb" /machine:I386 /out:"$(OUTDIR)/Chatter.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Msg.obj" \
	"$(INTDIR)/Chatter.obj" \
	"$(INTDIR)/Setupdlg.obj" \
	"$(INTDIR)/Chatvw.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Sendvw.obj" \
	"$(INTDIR)/Chatsock.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Chatdoc.obj" \
	"$(INTDIR)/Chatter.res"

"$(OUTDIR)\Chatter.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Chatter.exe"

CLEAN : 
	-@erase ".\Debug\Chatter.pdb"
	-@erase ".\Debug\Chatter.pch"
	-@erase ".\Debug\Chatsock.obj"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Msg.obj"
	-@erase ".\Debug\Chatvw.obj"
	-@erase ".\Debug\Sendvw.obj"
	-@erase ".\Debug\Setupdlg.obj"
	-@erase ".\Debug\Chatdoc.obj"
	-@erase ".\Debug\Chatter.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Chatter.res"
	-@erase ".\Debug\Chatter.idb"
	-@erase ".\Debug\Chatter.exe"
	-@erase ".\Debug\Chatter.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Chatter.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Chatter.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chatter.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Chatter.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Chatter.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Chatter.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Chatsock.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Msg.obj" \
	"$(INTDIR)/Chatvw.obj" \
	"$(INTDIR)/Sendvw.obj" \
	"$(INTDIR)/Setupdlg.obj" \
	"$(INTDIR)/Chatdoc.obj" \
	"$(INTDIR)/Chatter.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Chatter.res"

"$(OUTDIR)\Chatter.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir Release
# PROP BASE Intermediate_Dir Release
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir UniRelease
# PROP Intermediate_Dir UniRelease
# PROP Target_Dir
OUTDIR=.\UniRelease
INTDIR=.\UniRelease

ALL : "$(OUTDIR)\Chatter.exe"

CLEAN : 
	-@erase ".\UniRelease\Chatter.idb"
	-@erase ".\UniRelease\Chatter.pch"
	-@erase ".\UniRelease\Chatter.exe"
	-@erase ".\UniRelease\Chatdoc.obj"
	-@erase ".\UniRelease\Msg.obj"
	-@erase ".\UniRelease\Stdafx.obj"
	-@erase ".\UniRelease\Chatter.obj"
	-@erase ".\UniRelease\Setupdlg.obj"
	-@erase ".\UniRelease\Mainfrm.obj"
	-@erase ".\UniRelease\Chatvw.obj"
	-@erase ".\UniRelease\Sendvw.obj"
	-@erase ".\UniRelease\Chatsock.obj"
	-@erase ".\UniRelease\Chatter.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Chatter.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Chatter.pdb" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chatter.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Chatter.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/Chatter.pdb" /machine:I386\
 /out:"$(OUTDIR)/Chatter.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Chatdoc.obj" \
	"$(INTDIR)/Msg.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Chatter.obj" \
	"$(INTDIR)/Setupdlg.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Chatvw.obj" \
	"$(INTDIR)/Sendvw.obj" \
	"$(INTDIR)/Chatsock.obj" \
	"$(INTDIR)/Chatter.res"

"$(OUTDIR)\Chatter.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Debug
# PROP BASE Intermediate_Dir Debug
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir UniDebug
# PROP Intermediate_Dir UniDebug
# PROP Target_Dir
OUTDIR=.\UniDebug
INTDIR=.\UniDebug

ALL : "$(OUTDIR)\Chatter.exe"

CLEAN : 
	-@erase ".\UniDebug\Chatter.pdb"
	-@erase ".\UniDebug\Chatter.pch"
	-@erase ".\UniDebug\Stdafx.obj"
	-@erase ".\UniDebug\Mainfrm.obj"
	-@erase ".\UniDebug\Msg.obj"
	-@erase ".\UniDebug\Chatvw.obj"
	-@erase ".\UniDebug\Chatsock.obj"
	-@erase ".\UniDebug\Chatdoc.obj"
	-@erase ".\UniDebug\Sendvw.obj"
	-@erase ".\UniDebug\Chatter.obj"
	-@erase ".\UniDebug\Setupdlg.obj"
	-@erase ".\UniDebug\Chatter.res"
	-@erase ".\UniDebug\Chatter.idb"
	-@erase ".\UniDebug\Chatter.exe"
	-@erase ".\UniDebug\Chatter.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Chatter.pch" /Yu"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Chatter.pdb" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chatter.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Chatter.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/Chatter.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Chatter.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Msg.obj" \
	"$(INTDIR)/Chatvw.obj" \
	"$(INTDIR)/Chatsock.obj" \
	"$(INTDIR)/Chatdoc.obj" \
	"$(INTDIR)/Sendvw.obj" \
	"$(INTDIR)/Chatter.obj" \
	"$(INTDIR)/Setupdlg.obj" \
	"$(INTDIR)/Chatter.res"

"$(OUTDIR)\Chatter.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Chatter - Win32 Release"
# Name "Chatter - Win32 Debug"
# Name "Chatter - Win32 Unicode Release"
# Name "Chatter - Win32 Unicode Debug"

!IF  "$(CFG)" == "Chatter - Win32 Release"

!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"

!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Chatter.pch" /Yc /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/Chatter.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Chatter.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Chatter.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Chatter.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Chatter.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Chatter.pch" /Yc\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/Chatter.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Chatter.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Chatsock.cpp
DEP_CPP_CHATS=\
	".\Chatdoc.h"\
	".\Chatsock.h"\
	".\Chatter.h"\
	".\Resource.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"


"$(INTDIR)\Chatsock.obj" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"


"$(INTDIR)\Chatsock.obj" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"


"$(INTDIR)\Chatsock.obj" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"


"$(INTDIR)\Chatsock.obj" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATS) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Chatter.cpp
DEP_CPP_CHATT=\
	".\Chatdoc.h"\
	".\Chatsock.h"\
	".\Chatter.h"\
	".\Resource.h"\
	".\Chatvw.h"\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"


"$(INTDIR)\Chatter.obj" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"


"$(INTDIR)\Chatter.obj" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"


"$(INTDIR)\Chatter.obj" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"


"$(INTDIR)\Chatter.obj" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATT) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Chatter.rc
DEP_RSC_CHATTE=\
	".\Res\Chatter.ico"\
	".\Res\Toolbar.bmp"\
	".\Res\Chatter.rc2"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"


"$(INTDIR)\Chatter.res" : $(SOURCE) $(DEP_RSC_CHATTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"


"$(INTDIR)\Chatter.res" : $(SOURCE) $(DEP_RSC_CHATTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"


"$(INTDIR)\Chatter.res" : $(SOURCE) $(DEP_RSC_CHATTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"


"$(INTDIR)\Chatter.res" : $(SOURCE) $(DEP_RSC_CHATTE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Chatvw.cpp
DEP_CPP_CHATV=\
	".\Chatdoc.h"\
	".\Chatsock.h"\
	".\Chatter.h"\
	".\Resource.h"\
	".\Chatvw.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"


"$(INTDIR)\Chatvw.obj" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"


"$(INTDIR)\Chatvw.obj" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"


"$(INTDIR)\Chatvw.obj" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"


"$(INTDIR)\Chatvw.obj" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\Chatdoc.h"\
	".\Chatsock.h"\
	".\Chatter.h"\
	".\Resource.h"\
	".\Chatvw.h"\
	".\Mainfrm.h"\
	".\Sendvw.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Msg.cpp
DEP_CPP_MSG_C=\
	".\Msg.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"


"$(INTDIR)\Msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"


"$(INTDIR)\Msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"


"$(INTDIR)\Msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"


"$(INTDIR)\Msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Sendvw.cpp
DEP_CPP_SENDV=\
	".\Chatdoc.h"\
	".\Chatsock.h"\
	".\Chatter.h"\
	".\Resource.h"\
	".\Sendvw.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"


"$(INTDIR)\Sendvw.obj" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"


"$(INTDIR)\Sendvw.obj" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"


"$(INTDIR)\Sendvw.obj" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"


"$(INTDIR)\Sendvw.obj" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_SENDV) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Setupdlg.cpp
DEP_CPP_SETUP=\
	".\Chatter.h"\
	".\Resource.h"\
	".\Setupdlg.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"


"$(INTDIR)\Setupdlg.obj" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"


"$(INTDIR)\Setupdlg.obj" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"


"$(INTDIR)\Setupdlg.obj" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"


"$(INTDIR)\Setupdlg.obj" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_SETUP) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Chatdoc.cpp
DEP_CPP_CHATD=\
	".\Chatdoc.h"\
	".\Chatsock.h"\
	".\Chatter.h"\
	".\Resource.h"\
	".\Chatvw.h"\
	".\Msg.h"\
	".\Setupdlg.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Chatter - Win32 Release"


"$(INTDIR)\Chatdoc.obj" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Debug"


"$(INTDIR)\Chatdoc.obj" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Release"


"$(INTDIR)\Chatdoc.obj" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ELSEIF  "$(CFG)" == "Chatter - Win32 Unicode Debug"


"$(INTDIR)\Chatdoc.obj" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.pdb" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"

"$(INTDIR)\Chatter.idb" : $(SOURCE) $(DEP_CPP_CHATD) "$(INTDIR)"\
 "$(INTDIR)\Chatter.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
