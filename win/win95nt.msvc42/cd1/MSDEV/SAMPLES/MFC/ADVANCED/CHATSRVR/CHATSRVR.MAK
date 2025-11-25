# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=ChatSrvr - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ChatSrvr - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ChatSrvr - Win32 Release" && "$(CFG)" !=\
 "ChatSrvr - Win32 Debug" && "$(CFG)" != "ChatSrvr - Win32 Unicode Release" &&\
 "$(CFG)" != "ChatSrvr - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ChatSrvr.mak" CFG="ChatSrvr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ChatSrvr - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ChatSrvr - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ChatSrvr - Win32 Unicode Release" (based on\
 "Win32 (x86) Application")
!MESSAGE "ChatSrvr - Win32 Unicode Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "ChatSrvr - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"

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

ALL : "$(OUTDIR)\ChatSrvr.exe"

CLEAN : 
	-@erase ".\Release\ChatSrvr.idb"
	-@erase ".\Release\ChatSrvr.pch"
	-@erase ".\Release\ChatSrvr.exe"
	-@erase ".\Release\Lstnsock.obj"
	-@erase ".\Release\Mainfrm.obj"
	-@erase ".\Release\Stdafx.obj"
	-@erase ".\Release\Msg.obj"
	-@erase ".\Release\Chatsrvr.obj"
	-@erase ".\Release\Srvrdoc.obj"
	-@erase ".\Release\Clntsock.obj"
	-@erase ".\Release\Srvrvw.obj"
	-@erase ".\Release\Dialogs.obj"
	-@erase ".\Release\Chatsrvr.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ChatSrvr.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/ChatSrvr.pdb" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chatsrvr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ChatSrvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/ChatSrvr.pdb" /machine:I386 /out:"$(OUTDIR)/ChatSrvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Lstnsock.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Msg.obj" \
	"$(INTDIR)/Chatsrvr.obj" \
	"$(INTDIR)/Srvrdoc.obj" \
	"$(INTDIR)/Clntsock.obj" \
	"$(INTDIR)/Srvrvw.obj" \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Chatsrvr.res"

"$(OUTDIR)\ChatSrvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"

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

ALL : "$(OUTDIR)\ChatSrvr.exe"

CLEAN : 
	-@erase ".\Debug\ChatSrvr.pdb"
	-@erase ".\Debug\ChatSrvr.pch"
	-@erase ".\Debug\Chatsrvr.obj"
	-@erase ".\Debug\Stdafx.obj"
	-@erase ".\Debug\Clntsock.obj"
	-@erase ".\Debug\Msg.obj"
	-@erase ".\Debug\Dialogs.obj"
	-@erase ".\Debug\Lstnsock.obj"
	-@erase ".\Debug\Srvrvw.obj"
	-@erase ".\Debug\Mainfrm.obj"
	-@erase ".\Debug\Srvrdoc.obj"
	-@erase ".\Debug\Chatsrvr.res"
	-@erase ".\Debug\ChatSrvr.idb"
	-@erase ".\Debug\ChatSrvr.exe"
	-@erase ".\Debug\ChatSrvr.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ChatSrvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/ChatSrvr.pdb" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chatsrvr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ChatSrvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/ChatSrvr.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/ChatSrvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Chatsrvr.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Clntsock.obj" \
	"$(INTDIR)/Msg.obj" \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Lstnsock.obj" \
	"$(INTDIR)/Srvrvw.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Srvrdoc.obj" \
	"$(INTDIR)/Chatsrvr.res"

"$(OUTDIR)\ChatSrvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"

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

ALL : "$(OUTDIR)\ChatSrvr.exe"

CLEAN : 
	-@erase ".\UniRelease\ChatSrvr.idb"
	-@erase ".\UniRelease\ChatSrvr.pch"
	-@erase ".\UniRelease\ChatSrvr.exe"
	-@erase ".\UniRelease\Clntsock.obj"
	-@erase ".\UniRelease\Srvrdoc.obj"
	-@erase ".\UniRelease\Lstnsock.obj"
	-@erase ".\UniRelease\Dialogs.obj"
	-@erase ".\UniRelease\Stdafx.obj"
	-@erase ".\UniRelease\Msg.obj"
	-@erase ".\UniRelease\Mainfrm.obj"
	-@erase ".\UniRelease\Chatsrvr.obj"
	-@erase ".\UniRelease\Srvrvw.obj"
	-@erase ".\UniRelease\Chatsrvr.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ChatSrvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/ChatSrvr.pdb" /c 
CPP_OBJS=.\UniRelease/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chatsrvr.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ChatSrvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)/ChatSrvr.pdb" /machine:I386\
 /out:"$(OUTDIR)/ChatSrvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Clntsock.obj" \
	"$(INTDIR)/Srvrdoc.obj" \
	"$(INTDIR)/Lstnsock.obj" \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Msg.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Chatsrvr.obj" \
	"$(INTDIR)/Srvrvw.obj" \
	"$(INTDIR)/Chatsrvr.res"

"$(OUTDIR)\ChatSrvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"

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

ALL : "$(OUTDIR)\ChatSrvr.exe"

CLEAN : 
	-@erase ".\UniDebug\ChatSrvr.pdb"
	-@erase ".\UniDebug\ChatSrvr.pch"
	-@erase ".\UniDebug\Chatsrvr.obj"
	-@erase ".\UniDebug\Dialogs.obj"
	-@erase ".\UniDebug\Clntsock.obj"
	-@erase ".\UniDebug\Srvrvw.obj"
	-@erase ".\UniDebug\Msg.obj"
	-@erase ".\UniDebug\Lstnsock.obj"
	-@erase ".\UniDebug\Mainfrm.obj"
	-@erase ".\UniDebug\Stdafx.obj"
	-@erase ".\UniDebug\Srvrdoc.obj"
	-@erase ".\UniDebug\Chatsrvr.res"
	-@erase ".\UniDebug\ChatSrvr.idb"
	-@erase ".\UniDebug\ChatSrvr.exe"
	-@erase ".\UniDebug\ChatSrvr.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ChatSrvr.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/ChatSrvr.pdb" /c 
CPP_OBJS=.\UniDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chatsrvr.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ChatSrvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/ChatSrvr.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/ChatSrvr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Chatsrvr.obj" \
	"$(INTDIR)/Dialogs.obj" \
	"$(INTDIR)/Clntsock.obj" \
	"$(INTDIR)/Srvrvw.obj" \
	"$(INTDIR)/Msg.obj" \
	"$(INTDIR)/Lstnsock.obj" \
	"$(INTDIR)/Mainfrm.obj" \
	"$(INTDIR)/Stdafx.obj" \
	"$(INTDIR)/Srvrdoc.obj" \
	"$(INTDIR)/Chatsrvr.res"

"$(OUTDIR)\ChatSrvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "ChatSrvr - Win32 Release"
# Name "ChatSrvr - Win32 Debug"
# Name "ChatSrvr - Win32 Unicode Release"
# Name "ChatSrvr - Win32 Unicode Debug"

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"

!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"

!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ChatSrvr.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(OUTDIR)/ChatSrvr.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ChatSrvr.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/ChatSrvr.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"

# ADD CPP /Yc

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ChatSrvr.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/ChatSrvr.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"

# ADD CPP /Yc"Stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ChatSrvr.pch" /Yc"Stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(OUTDIR)/ChatSrvr.pdb" /c $(SOURCE) \
	

"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ChatSrvr.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Chatsrvr.rc
DEP_RSC_CHATS=\
	".\Res\Chatsrvr.ico"\
	".\Res\Toolbar.bmp"\
	".\Res\Chatsrvr.rc2"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"


"$(INTDIR)\Chatsrvr.res" : $(SOURCE) $(DEP_RSC_CHATS) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"


"$(INTDIR)\Chatsrvr.res" : $(SOURCE) $(DEP_RSC_CHATS) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"


"$(INTDIR)\Chatsrvr.res" : $(SOURCE) $(DEP_RSC_CHATS) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"


"$(INTDIR)\Chatsrvr.res" : $(SOURCE) $(DEP_RSC_CHATS) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Clntsock.cpp
DEP_CPP_CLNTS=\
	".\Clntsock.h"\
	".\Msg.h"\
	".\Srvrdoc.h"\
	".\Lstnsock.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"


"$(INTDIR)\Clntsock.obj" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"


"$(INTDIR)\Clntsock.obj" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"


"$(INTDIR)\Clntsock.obj" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"


"$(INTDIR)\Clntsock.obj" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_CLNTS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Dialogs.cpp
DEP_CPP_DIALO=\
	".\Chatsrvr.h"\
	".\Resource.h"\
	".\Dialogs.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"


"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"


"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"


"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"


"$(INTDIR)\Dialogs.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Lstnsock.cpp
DEP_CPP_LSTNS=\
	".\Lstnsock.h"\
	".\Srvrdoc.h"\
	".\Clntsock.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"


"$(INTDIR)\Lstnsock.obj" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"


"$(INTDIR)\Lstnsock.obj" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"


"$(INTDIR)\Lstnsock.obj" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"


"$(INTDIR)\Lstnsock.obj" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_LSTNS) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainfrm.cpp
DEP_CPP_MAINF=\
	".\Chatsrvr.h"\
	".\Resource.h"\
	".\Mainfrm.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"


"$(INTDIR)\Mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Msg.cpp
DEP_CPP_MSG_C=\
	".\Msg.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"


"$(INTDIR)\Msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"


"$(INTDIR)\Msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"


"$(INTDIR)\Msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"


"$(INTDIR)\Msg.obj" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_MSG_C) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Srvrdoc.cpp
DEP_CPP_SRVRD=\
	".\Chatsrvr.h"\
	".\Resource.h"\
	".\Dialogs.h"\
	".\Msg.h"\
	".\Srvrdoc.h"\
	".\Clntsock.h"\
	".\Lstnsock.h"\
	".\Srvrvw.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"


"$(INTDIR)\Srvrdoc.obj" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"


"$(INTDIR)\Srvrdoc.obj" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"


"$(INTDIR)\Srvrdoc.obj" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"


"$(INTDIR)\Srvrdoc.obj" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_SRVRD) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Srvrvw.cpp
DEP_CPP_SRVRV=\
	".\Chatsrvr.h"\
	".\Resource.h"\
	".\Srvrdoc.h"\
	".\Clntsock.h"\
	".\Lstnsock.h"\
	".\Srvrvw.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"


"$(INTDIR)\Srvrvw.obj" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"


"$(INTDIR)\Srvrvw.obj" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"


"$(INTDIR)\Srvrvw.obj" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"


"$(INTDIR)\Srvrvw.obj" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_SRVRV) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Chatsrvr.cpp
DEP_CPP_CHATSR=\
	".\Chatsrvr.h"\
	".\Resource.h"\
	".\Mainfrm.h"\
	".\Srvrdoc.h"\
	".\Clntsock.h"\
	".\Lstnsock.h"\
	".\Srvrvw.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "ChatSrvr - Win32 Release"


"$(INTDIR)\Chatsrvr.obj" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Debug"


"$(INTDIR)\Chatsrvr.obj" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Release"


"$(INTDIR)\Chatsrvr.obj" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ELSEIF  "$(CFG)" == "ChatSrvr - Win32 Unicode Debug"


"$(INTDIR)\Chatsrvr.obj" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.pdb" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"

"$(INTDIR)\ChatSrvr.idb" : $(SOURCE) $(DEP_CPP_CHATSR) "$(INTDIR)"\
 "$(INTDIR)\ChatSrvr.pch"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
