# Microsoft Developer Studio Generated NMAKE File, Based on Filter.dsp
!IF "$(CFG)" == ""
CFG=filter - Win32 Release
!MESSAGE No configuration specified. Defaulting to filter - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "filter - Win32 Release" && "$(CFG)" != "filter - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Filter.mak" CFG="filter - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "filter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "filter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "filter - Win32 Release"

OUTDIR=.
INTDIR=.\Release
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\filter.ax"

!ELSE 

ALL : "$(OUTDIR)\filter.ax"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Filter.res"
	-@erase "$(OUTDIR)\filter.ax"
	-@erase "$(OUTDIR)\filter.exp"
	-@erase "$(OUTDIR)\filter.lib"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /GB /Gz /MD /W3 /O2 /D "NDEBUG" /D "INC_OLE2" /D "STRICT" /D\
 "WIN32" /D "_MT" /D "_DLL" /D _X86_=1 /D WINVER=0x0400 /D try=__try /D\
 except=__except /D leave=__leave /D finally=__finally /U "DBG" /U "DEBUG"\
 /Fo"$(INTDIR)\\" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Filter.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Filter.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=strmbase.lib quartz.lib vfw32.lib winmm.lib kernel32.lib\
 advapi32.lib version.lib largeint.lib user32.lib gdi32.lib comctl32.lib\
 ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x1c400000"\
 /version:4.0 /entry:"DllEntryPoint@12" /dll /pdb:none /machine:I386\
 /nodefaultlib /def:".\Filter.def" /out:"$(OUTDIR)\filter.ax"\
 /implib:"$(OUTDIR)\filter.lib" /subsystem:windows,4.0 
DEF_FILE= \
	".\Filter.def"
LINK32_OBJS= \
	"$(INTDIR)\Filter.res"

"$(OUTDIR)\filter.ax" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "filter - Win32 Debug"

OUTDIR=.
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\filter.ax"

!ELSE 

ALL : "$(OUTDIR)\filter.ax"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Filter.res"
	-@erase "$(OUTDIR)\filter.ax"
	-@erase "$(OUTDIR)\filter.exp"
	-@erase "$(OUTDIR)\filter.lib"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /GB /Gz /MDd /W3 /GR /GX /Z7 /D "INC_OLE2" /D "STRICT" /D\
 "WIN32" /D "_MT" /D "_DLL" /D _X86_=1 /D WINVER=0x0400 /D DBG=1 /D "DEBUG" /D\
 try=__try /D except=__except /D leave=__leave /D finally=__finally\
 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Oid /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Filter.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Filter.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=strmbasd.lib quartz.lib libcmtd.lib vfw32.lib winmm.lib\
 kernel32.lib advapi32.lib version.lib largeint.lib user32.lib gdi32.lib\
 comctl32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /base:"0x1c400000" /entry:"DllEntryPoint@12" /dll /pdb:none /debug\
 /machine:I386 /nodefaultlib /def:".\Filter.def" /out:"$(OUTDIR)\filter.ax"\
 /implib:"$(OUTDIR)\filter.lib" /subsystem:windows,4.0 
DEF_FILE= \
	".\Filter.def"
LINK32_OBJS= \
	"$(INTDIR)\Filter.res"

"$(OUTDIR)\filter.ax" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "filter - Win32 Release" || "$(CFG)" == "filter - Win32 Debug"
SOURCE=.\Filter.rc
DEP_RSC_FILTE=\
	{$(INCLUDE)}"activex.rcv"\
	{$(INCLUDE)}"activex.ver"\
	

"$(INTDIR)\Filter.res" : $(SOURCE) $(DEP_RSC_FILTE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

