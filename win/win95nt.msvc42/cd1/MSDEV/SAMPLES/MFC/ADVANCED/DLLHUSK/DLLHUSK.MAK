# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=DLLHusk - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to DLLHusk - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DLLHusk - Win32 Release" && "$(CFG)" !=\
 "DLLHusk - Win32 Debug" && "$(CFG)" != "TestDll1 - Win32 Release" && "$(CFG)"\
 != "TestDll1 - Win32 Debug" && "$(CFG)" != "TestDll2 - Win32 Release" &&\
 "$(CFG)" != "TestDll2 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dllhusk.mak" CFG="DLLHusk - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DLLHusk - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DLLHusk - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "TestDll1 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TestDll1 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TestDll2 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TestDll2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "TestDll2 - Win32 Debug"

!IF  "$(CFG)" == "DLLHusk - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir DLLHusk_
# PROP BASE Intermediate_Dir DLLHusk_
# PROP BASE Target_Dir
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir
OUTDIR=.\Release
INTDIR=.\Release

ALL : "TestDll2 - Win32 Release" "TestDll1 - Win32 Release"\
 "$(OUTDIR)\dllhusk.exe"

CLEAN : 
	-@erase ".\Release\dllhusk.exe"
	-@erase ".\Release\mainfrm.obj"
	-@erase ".\Release\dllhusk.obj"
	-@erase ".\Release\dllhusk.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dllhusk.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dllhusk.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Release\TestDll1.lib Release\TestDll2.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=Release\TestDll1.lib Release\TestDll2.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/dllhusk.pdb" /machine:I386\
 /out:"$(OUTDIR)/dllhusk.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dllhusk.obj" \
	"$(INTDIR)/dllhusk.res" \
	".\Release\TestDll1.lib" \
	".\Release\TestDll2.lib"

"$(OUTDIR)\dllhusk.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DLLHusk - Win32 Debug"

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

ALL : "TestDll2 - Win32 Debug" "TestDll1 - Win32 Debug" "$(OUTDIR)\dllhusk.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\dllhusk.exe"
	-@erase ".\Debug\mainfrm.obj"
	-@erase ".\Debug\dllhusk.obj"
	-@erase ".\Debug\dllhusk.res"
	-@erase ".\Debug\dllhusk.ilk"
	-@erase ".\Debug\dllhusk.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/dllhusk.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/dllhusk.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 Debug\TestDll1.lib Debug\TestDll2.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=Debug\TestDll1.lib Debug\TestDll2.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)/dllhusk.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/dllhusk.exe" 
LINK32_OBJS= \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/dllhusk.obj" \
	"$(INTDIR)/dllhusk.res" \
	".\Debug\TestDll1.lib" \
	".\Debug\TestDll2.lib"

"$(OUTDIR)\dllhusk.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TestDll1 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir TestDll1\Release
# PROP BASE Intermediate_Dir TestDll1\Release
# PROP BASE Target_Dir TestDll1
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir TestDll1
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\TestDll1.dll"

CLEAN : 
	-@erase ".\Release\TestDll1.dll"
	-@erase ".\Release\testdll1.obj"
	-@erase ".\Release\testdll1.res"
	-@erase ".\Release\TestDll1.lib"
	-@erase ".\Release\TestDll1.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXEXT" /D "_MBCS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testdll1.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/TestDll1.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/TestDll1.pdb" /machine:I386 /out:"$(OUTDIR)/TestDll1.dll"\
 /implib:"$(OUTDIR)/TestDll1.lib" 
LINK32_OBJS= \
	"$(INTDIR)/testdll1.obj" \
	"$(INTDIR)/testdll1.res"

"$(OUTDIR)\TestDll1.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TestDll1 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir TestDll1\Debug
# PROP BASE Intermediate_Dir TestDll1\Debug
# PROP BASE Target_Dir TestDll1
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir TestDll1
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\TestDll1.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\TestDll1.dll"
	-@erase ".\Debug\testdll1.obj"
	-@erase ".\Debug\testdll1.res"
	-@erase ".\Debug\TestDll1.ilk"
	-@erase ".\Debug\TestDll1.lib"
	-@erase ".\Debug\TestDll1.exp"
	-@erase ".\Debug\TestDll1.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXEXT" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testdll1.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/TestDll1.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/TestDll1.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/TestDll1.dll" /implib:"$(OUTDIR)/TestDll1.lib" 
LINK32_OBJS= \
	"$(INTDIR)/testdll1.obj" \
	"$(INTDIR)/testdll1.res"

"$(OUTDIR)\TestDll1.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TestDll2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir TestDll2\Release
# PROP BASE Intermediate_Dir TestDll2\Release
# PROP BASE Target_Dir TestDll2
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir Release
# PROP Intermediate_Dir Release
# PROP Target_Dir TestDll2
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\TestDll2.dll"

CLEAN : 
	-@erase ".\Release\TestDll2.dll"
	-@erase ".\Release\testdll2.obj"
	-@erase ".\Release\testdll2.res"
	-@erase ".\Release\TestDll2.lib"
	-@erase ".\Release\TestDll2.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXEXT" /D "_MBCS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testdll2.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/TestDll2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/TestDll2.pdb" /machine:I386 /out:"$(OUTDIR)/TestDll2.dll"\
 /implib:"$(OUTDIR)/TestDll2.lib" 
LINK32_OBJS= \
	"$(INTDIR)/testdll2.obj" \
	"$(INTDIR)/testdll2.res"

"$(OUTDIR)\TestDll2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TestDll2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir TestDll2\Debug
# PROP BASE Intermediate_Dir TestDll2\Debug
# PROP BASE Target_Dir TestDll2
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir Debug
# PROP Intermediate_Dir Debug
# PROP Target_Dir TestDll2
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\TestDll2.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\TestDll2.dll"
	-@erase ".\Debug\testdll2.obj"
	-@erase ".\Debug\testdll2.res"
	-@erase ".\Debug\TestDll2.ilk"
	-@erase ".\Debug\TestDll2.lib"
	-@erase ".\Debug\TestDll2.exp"
	-@erase ".\Debug\TestDll2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXEXT" /D "_MBCS" /c
# SUBTRACT CPP /YX /Yc /Yu
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXEXT" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=

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
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/testdll2.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/TestDll2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/TestDll2.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/TestDll2.dll" /implib:"$(OUTDIR)/TestDll2.lib" 
LINK32_OBJS= \
	"$(INTDIR)/testdll2.obj" \
	"$(INTDIR)/testdll2.res"

"$(OUTDIR)\TestDll2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "DLLHusk - Win32 Release"
# Name "DLLHusk - Win32 Debug"

!IF  "$(CFG)" == "DLLHusk - Win32 Release"

!ELSEIF  "$(CFG)" == "DLLHusk - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dllhusk.rc

!IF  "$(CFG)" == "DLLHusk - Win32 Release"

DEP_RSC_DLLHU=\
	".\res\dllhusk.ico"\
	".\res\toolbar.bmp"\
	

"$(INTDIR)\dllhusk.res" : $(SOURCE) $(DEP_RSC_DLLHU) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "DLLHusk - Win32 Debug"

DEP_RSC_DLLHU=\
	".\res\dllhusk.ico"\
	".\res\toolbar.bmp"\
	

"$(INTDIR)\dllhusk.res" : $(SOURCE) $(DEP_RSC_DLLHU) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "DLLHusk - Win32 Release"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\dllhusk.h"\
	".\mainfrm.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "DLLHusk - Win32 Debug"

DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\dllhusk.h"\
	".\mainfrm.h"\
	
# SUBTRACT CPP /YX /Yc /Yu

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dllhusk.cpp

!IF  "$(CFG)" == "DLLHusk - Win32 Release"

DEP_CPP_DLLHUS=\
	".\stdafx.h"\
	".\dllhusk.h"\
	".\mainfrm.h"\
	".\testdll1.h"\
	".\testdll2.h"\
	

"$(INTDIR)\dllhusk.obj" : $(SOURCE) $(DEP_CPP_DLLHUS) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "DLLHusk - Win32 Debug"

DEP_CPP_DLLHUS=\
	".\stdafx.h"\
	".\dllhusk.h"\
	".\mainfrm.h"\
	".\testdll1.h"\
	".\testdll2.h"\
	
# SUBTRACT CPP /YX /Yc /Yu

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\dllhusk.obj" : $(SOURCE) $(DEP_CPP_DLLHUS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DLLHUS) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "TestDll1"

!IF  "$(CFG)" == "DLLHusk - Win32 Release"

"TestDll1 - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\dllhusk.mak CFG="TestDll1 - Win32 Release" 

!ELSEIF  "$(CFG)" == "DLLHusk - Win32 Debug"

"TestDll1 - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\dllhusk.mak CFG="TestDll1 - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "TestDll2"

!IF  "$(CFG)" == "DLLHusk - Win32 Release"

"TestDll2 - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F .\dllhusk.mak CFG="TestDll2 - Win32 Release" 

!ELSEIF  "$(CFG)" == "DLLHusk - Win32 Debug"

"TestDll2 - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F .\dllhusk.mak CFG="TestDll2 - Win32 Debug" 

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "TestDll1 - Win32 Release"
# Name "TestDll1 - Win32 Debug"

!IF  "$(CFG)" == "TestDll1 - Win32 Release"

!ELSEIF  "$(CFG)" == "TestDll1 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\testdll1.cpp

!IF  "$(CFG)" == "TestDll1 - Win32 Release"

DEP_CPP_TESTD=\
	".\resource.h"\
	".\stdafx.h"\
	".\testres1.h"\
	

"$(INTDIR)\testdll1.obj" : $(SOURCE) $(DEP_CPP_TESTD) "$(INTDIR)"
   $(CPP) /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_AFXEXT" /D "_MBCS" /Fo"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "TestDll1 - Win32 Debug"

DEP_CPP_TESTD=\
	".\resource.h"\
	".\stdafx.h"\
	".\testres1.h"\
	
# SUBTRACT CPP /YX /Yc /Yu

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXEXT" /D "_MBCS" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\testdll1.obj" : $(SOURCE) $(DEP_CPP_TESTD) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TESTD) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\testdll1.rc

!IF  "$(CFG)" == "TestDll1 - Win32 Release"

DEP_RSC_TESTDL=\
	".\res\texttype.ico"\
	".\res\hello.ico"\
	".\testres1.h"\
	

"$(INTDIR)\testdll1.res" : $(SOURCE) $(DEP_RSC_TESTDL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TestDll1 - Win32 Debug"

DEP_RSC_TESTDL=\
	".\res\texttype.ico"\
	".\res\hello.ico"\
	".\testres1.h"\
	

"$(INTDIR)\testdll1.res" : $(SOURCE) $(DEP_RSC_TESTDL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "TestDll2 - Win32 Release"
# Name "TestDll2 - Win32 Debug"

!IF  "$(CFG)" == "TestDll2 - Win32 Release"

!ELSEIF  "$(CFG)" == "TestDll2 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\testdll2.cpp

!IF  "$(CFG)" == "TestDll2 - Win32 Release"

DEP_CPP_TESTDLL=\
	".\resource.h"\
	".\stdafx.h"\
	".\testres2.h"\
	

"$(INTDIR)\testdll2.obj" : $(SOURCE) $(DEP_CPP_TESTDLL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TestDll2 - Win32 Debug"

DEP_CPP_TESTDLL=\
	".\resource.h"\
	".\stdafx.h"\
	".\testres2.h"\
	

"$(INTDIR)\testdll2.obj" : $(SOURCE) $(DEP_CPP_TESTDLL) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TESTDLL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\testdll2.rc

!IF  "$(CFG)" == "TestDll2 - Win32 Release"

DEP_RSC_TESTDLL2=\
	".\res\listout.ico"\
	".\testres2.h"\
	

"$(INTDIR)\testdll2.res" : $(SOURCE) $(DEP_RSC_TESTDLL2) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TestDll2 - Win32 Debug"

DEP_RSC_TESTDLL2=\
	".\res\listout.ico"\
	".\testres2.h"\
	

"$(INTDIR)\testdll2.res" : $(SOURCE) $(DEP_RSC_TESTDLL2) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
