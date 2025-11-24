# -----------------------------------------------------------------------------
# $(MAPI)\src\sample.ab\makefile
#
# Copyright (C) 1992-1995 Microsoft Corporation. All Rights Reserved.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
#
#	Part 1: define the version to be built
#	Default is debug Win32 DLL
#

# Default build flavor
!IF "$(VERSION)" == ""
VERSION=DEBUG
!ENDIF

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN32DLLMT
!ENDIF

# Conditional key for MAPI dev tree vs. setup tree
DEVTREE=0

# -----------------------------------------------------------------------------
# The PRODUCT variable must be defined to be one of the following:
#	
#	WIN16EXE	Win16 Windows Application
#	WIN16DLL	Win16 DLL
#	WIN16LIB	Win16 Static library
#	WIN32EXE	Win32 Windows Application (single thread)
#	WIN32DLL	Win32 DLL (single thread)
#	WIN32DLLMT	Win32 DLL (multi thread)
#	WIN32LIB	Win32 Static library
#	CHIEXE		Win32c Windows Application (single thread)
#	CHIDLL		Win32c DLL (32-bit, single thread)
#	CHIDLLMT	Win32c DLL (multi thread)
#	CHILIB		Win32c Static library
#
# The VERSION variable must be defined to be one of the following:
#
#	DEBUG		Debug build (full symbols, full debug info, no optimizations)
#	TEST		Test build (symbols only, full optimizations)
#	SHIP		Ship build (no symbols, full optimizations)
#
# The following environment variables should be defined:
#
#	MAPI		Root of the MAPI project (defaults to c:\mapi)
#	TMP			Temp directory (defaults to c:\temp)
#	CPU			CPU to be targeted (defaults to i386)
#	LANG		The language version to compile (defaults to usa)
#	NOSILENT	Set to 1 to prevent silent execution of nmake
#	NOECHO		Set to 1 to prevent echo of progress
#
# As a result of the above definitions, the following variables are defined:
#
#	WIN16		Set to 1 if producing a Win16 product
#	WIN32		Set to 1 if producing a Win32 product
#	WINTAG		Set to 16/32/chi/dos/mac depending on PRODUCT
#
#	PRODEXE		Set to 1 if producing an EXE
#	PRODDLL		Set to 1 if producing a DLL
#	PRODMT		Set to 1 if producing a multi threaded product
#
#	VERDEBUG	Set to 1 if VERSION is DEBUG
#	VERTEST		Set to 1 if VERSION is TEST
#	VERSHIP		Set to 1 if VERSION is SHIP
#	VERTAG		Set to dbg/shp/tst depending on VERSION
#
#	MAPIBIN		Set to $(MAPI)\bin
#	MAPILIB		Set to $(MAPI)\lib\$(WINTAG)
#	MAPIDLL		Set to $(MAPI)\lib\$(WINTAG)\$(VERTAG)
#	MAPICOMMON	Set to $(MAPI)\src\common
#
# -----------------------------------------------------------------------------

# Silence and Echo ------------------------------------------------------------

!IF "$(NOSILENT)" == ""
!CMDSWITCHES +s
!ENDIF

!IF "$(NOECHO)" == ""
DOECHO	= 1
!ELSE
DOECHO	= 0
!ENDIF

# Process the PRODUCT variable ------------------------------------------------

WIN16		= 0
WIN32		= 0
CHICAGO		= 0

PRODEXE		= 0
PRODLIB		= 0
PRODDLL		= 0
PRODMT		= 0

!IF "$(PRODUCT)" == "WIN16EXE"
WIN16		= 1
PRODEXE		= 1
!ENDIF

!IF "$(PRODUCT)" == "WIN16DLL"
WIN16		= 1
PRODDLL		= 1
!ENDIF

!IF "$(PRODUCT)" == "WIN16LIB"
WIN16		= 1
PRODLIB		= 1
!ENDIF

!IF "$(PRODUCT)" == "WIN32EXE"
WIN32		= 1
PRODEXE		= 1
!ENDIF

!IF "$(PRODUCT)" == "WIN32DLL"
WIN32		= 1
PRODDLL		= 1
!ENDIF

!IF "$(PRODUCT)" == "WIN32DLLMT"
WIN32		= 1
PRODDLL		= 1
PRODMT		= 1
!ENDIF

!IF "$(PRODUCT)" == "WIN32LIB"
WIN32		= 1
PRODLIB		= 1
!ENDIF

!IF "$(PRODUCT)" == "CHIEXE"
WIN32		= 1
PRODEXE		= 1
CHICAGO		= 1
!ENDIF

!IF "$(PRODUCT)" == "CHIDLL"
WIN32		= 1
PRODDLL		= 1
CHICAGO		= 1
!ENDIF

!IF "$(PRODUCT)" == "CHIDLLMT"
WIN32		= 1
PRODDLL		= 1
PRODMT		= 1
CHICAGO		= 1
!ENDIF

!IF "$(PRODUCT)" == "CHILIB"
WIN32		= 1
PRODLIB		= 1
CHICAGO		= 1
!ENDIF

WINTAG		=

!IF $(WIN16)
WINTAG		= 16
!ENDIF

!IF $(WIN32)
!IF $(CHICAGO)
WINTAG		= chi
!ELSE
WINTAG		= 32
!ENDIF
!ENDIF

!IF "$(WINTAG)" == ""
!ERROR "The PRODUCT variable has not been defined correctly"
!ENDIF

# Process the VERSION variable ------------------------------------------------

VERDEBUG	= 0
VERTEST		= 0
VERSHIP		= 0

!IF	"$(VERSION)" == "DEBUG"
VERDEBUG	= 1
VERTAG		= dbg
!ENDIF

!IF "$(VERSION)" == "TEST"
VERTEST		= 1
VERTAG		= tst
!ENDIF

!IF "$(VERSION)" == "SHIP"
VERSHIP		= 1
VERTAG		= shp
!ENDIF

!IF $(VERDEBUG) == 0 && $(VERTEST) == 0 && $(VERSHIP) == 0
!ERROR "The VERSION variable has not been defined correctly"
!ENDIF

# Defaults --------------------------------------------------------------------

!IF "$(MAPI)" == ""
MAPI		= c:\mapi
!ENDIF

!IF "$(TMP)" == ""
TMP			= c:\temp
!ENDIF

!IF "$(LANG)" == ""
LANG		= usa
!ENDIF

!IF "$(CPU)" == ""
CPU			= i386
!ENDIF

!IF "$(MSVCNT)" == ""
MSVCNT		= 1		# Change this to a 0 to make NT SDK compiler the default
!ENDIF

# Construct directory variables -----------------------------------------------

!IF "$(MAPILIB)" == ""
MAPILIB		= $(MAPI)\lib\$(WINTAG)
!ENDIF

!IF $(DEVTREE)
SRCROOT		= src
!IF "$(MAPIDLL)" == ""
MAPIDLL		= $(MAPI)\lib\$(WINTAG)\$(VERTAG)
!ENDIF
!ELSE
SRCROOT		= dev
!IF "$(MAPIDLL)" == ""
MAPIDLL		= $(MAPI)
!ENDIF
!ENDIF

MAPIH		= $(MAPI)\h
MAPICOMMON	= $(MAPI)\$(SRCROOT)\common
Z			= $(MAPI)

# Construct MAKEDEFS ----------------------------------------------------------

MAKEDEFS	= PRODUCT=$(PRODUCT) VERSION=$(VERSION)

# Definitions -----------------------------------------------------------------

DEFS		=
AFLAGS		=
CFLAGS		=
LFLAGS		=
INCDIRS		=

!IF $(VERDEBUG)
DEFS		= $(DEFS) -DDEBUG
!ENDIF

!IF $(VERTEST)
DEFS		= $(DEFS) -DTEST
!ENDIF

!IF $(VERSHIP)
DEFS		= $(DEFS) -DSHIP
!ENDIF

# Programs --------------------------------------------------------------------

!IF $(WIN16)
CC			= cl
CPRE		= cl -nologo -EP
LINK		= link
LIBR		= lib
IMPLIB		= implib
!ENDIF

!IF $(WIN32) && "$(CPU)" == "i386"
!IF $(MSVCNT)
CC			= cl
!ELSE
CC			= cl386
!ENDIF
CCPU		= _X86_
CVTOBJ		= REM MIPS only
!ENDIF

!IF $(WIN32) && "$(CPU)" == "MIPS"
CC			= cc
CCPU		= MIPS
CVTOBJ		= mip2coff
!ENDIF

!IF $(WIN32)
CPRE		= $(CC) -EP
!IF $(MSVCNT)
LINK		= link
LIBR		= lib
!ELSE
LINK		= link32
LIBR		= lib32
!ENDIF
!ENDIF

!IF "$(OS)" == "Windows_NT"
MKTOOL		= mktool
!ELSE
MKTOOL		= mktool16
!ENDIF

AS			= masm

!IF "$(WARNLEVEL)" == ""
WARNLEVEL	= 3
!ENDIF

# Win16 Definitions -------------------------------------------------------

!IF $(WIN16)

DEFS		= $(DEFS) -DWIN16

CMODEL		= -AL

!IF $(PRODEXE)
CMODEL		= -AL -GA -GEs
!ENDIF

# Assume a static lib may be included in a DLL
!IF $(PRODDLL) || $(PRODLIB)
CMODEL		= -Alfw -GA -GEd -D_WINDLL
!ENDIF

AFLAGS		= -Mx -t
CFLAGS		= $(CFLAGS) -nologo -c $(CMODEL) -W$(WARNLEVEL) -Zp2 -Gy -Gr -Gs -Gx- -Gf -J
LFLAGS		= $(LFLAGS) /NOLOGO /ONERROR:NOEXE /NOD /NOE /MAP

!IF "$(DUMPASM)" != ""
CFLAGS		= $(CFLAGS) -Fa$*.out
!ENDIF

!IF $(VERDEBUG)
AFLAGS		= $(AFLAGS) -Z -Zi -L
CFLAGS		= $(CFLAGS) -f -Zi -Od -Ob1
LFLAGS		= $(LFLAGS) /CO
!ENDIF

!IF $(VERTEST)
AFLAGS		= $(AFLAGS) -Zi
CFLAGS		= $(CFLAGS) -Zi -Oilx -Ob1
LFLAGS		= $(LFLAGS) /CO
!ENDIF

!IF $(VERSHIP)
CFLAGS		= $(CFLAGS) -Zd -Oilx -Ob1
!ENDIF

CFLAGS		= $(CFLAGS) -G2

!ENDIF

# Win32 Definitions -----------------------------------------------------------

!IF $(WIN32)

DEFS		= $(DEFS) -DWIN32 -D$(CCPU)=1

!IF $(CHICAGO)
DEFS		= $(DEFS) -DCHICAGO
!ELSE
DEFS		= $(DEFS) -DNT
!ENDIF

AFLAGS		= -Mx -t
CFLAGS		= $(CFLAGS) -nologo -c -G3s -Gr -Gf -W$(WARNLEVEL) -J
!IF $(CHICAGO)
LFLAGS		= $(LFLAGS) -align:4096 -nod
!ELSE
LFLAGS		= $(LFLAGS) -align:4096 -incremental:no -pdb:none -nod
!ENDIF

!IF "$(DOPROFILE)" != ""
CFLAGS		= $(CFLAGS) -Gh
!ENDIF

!IF $(MSVCNT)
MAXOPT		= -Ob1 -O1 -Oi
!ELSE
LFLAGS		= $(LFLAGS) -ignore:505
MAXOPT		= -Ob1 -Og -Os -Oi
!ENDIF

!IF "$(DUMPASM)" != ""
CFLAGS		= $(CFLAGS) -Fa$*.out -FAs
!ENDIF

!IF $(PRODEXE)
LFLAGS		= $(LFLAGS) -subsystem:windows
ENTRYPOINT  = WinMainCRTStartup
!ENDIF

!IF $(PRODDLL)
!IF $(CHICAGO)
LFLAGS		= $(LFLAGS) -subsystem:windows,4.00
!ELSE
LFLAGS		= $(LFLAGS) -subsystem:windows
!ENDIF
!ENDIF

!IF $(PRODMT) && $(PRODDLL)
ENTRYPOINT	= _CRT_INIT@12
!ENDIF

!IF $(VERDEBUG)
AFLAGS		= $(AFLAGS) -Z -Zi -L
CFLAGS		= $(CFLAGS) -Ob1 -Od -Z7
LFLAGS		= $(LFLAGS) -debug:full -debugtype:cv
!ENDIF

!IF $(VERTEST)
AFLAGS		= $(AFLAGS) -Zi
CFLAGS		= $(CFLAGS) $(MAXOPT) -Z7
LFLAGS		= $(LFLAGS) -debug:full -debugtype:cv
!ENDIF

!IF $(VERSHIP)
CFLAGS		= $(CFLAGS) $(MAXOPT)
LFLAGS		= $(LFLAGS) -debug:none
!ENDIF

!ENDIF

# Libraries -------------------------------------------------------------------

!IF $(WIN16)
LIBS		= oldnames libw

!IF $(PRODEXE)
LIBS		= $(LIBS) llibcew
!ENDIF

!IF $(PRODDLL)
LIBS		= $(LIBS) ldllcew
!ENDIF

!ENDIF


!IF $(WIN32)

LIBS		= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib

!IF "$(DOPROFILE)" != ""
LIBS		= $(LIBS) cap.lib
!ENDIF

!IF $(PRODMT)
LIBS		= $(LIBS) libcmt.lib
!ELSE
!IF $(PRODDLL)
LIBS		= $(LIBS) crtdll.lib
!ELSE
LIBS		= $(LIBS) libc.lib
!ENDIF
!ENDIF

!ENDIF

# default goal target ---------------------------------------------------------

defgoal:	goal

# -----------------------------------------------------------------------------
#
#	Part 3: sample.ab macros, objects, and dependencies
#

H			= $(MAPI)\$(SRCROOT)\sample.ab

ENTRYPOINT	=

OTHERINCS	= -I$(MAPIH)

RCSOURCE	= $H\sampabp.rc
RCDEPS		= $H\sampabp.rh

!IF $(WIN16)
BASENAME	= smpab
OTHERLIBS	= $(MAPILIB)\mapiu.lib commdlg.lib
!ENDIF

!IF $(WIN32)
BASENAME	= smpab32
OTHERLIBS	= $(MAPILIB)\mapiu32.lib
!ENDIF

!IF $(VERDEBUG)
!IF $(WIN16)
COMMONOBJS	= mapidbg.obj wep.obj
!ELSE
COMMONOBJS	= mapidbg.obj
!ENDIF
!ELSE
!IF $(WIN16)
COMMONOBJS	= wep.obj
!ELSE
COMMONOBJS	= 
!ENDIF
!ENDIF

OBJS = abguid.obj wrap.obj status.obj abuser.obj tid.obj oouser.obj ootid.obj root.obj abcont.obj \
	absearch.obj ablogon.obj abp.obj abctbl1.obj abctbl2.obj abctbl3.obj \
	$(COMMONOBJS)

abp.obj: \
	$H\abp.c \
	$H\abp.h

ablogon.obj: \
	$H\ablogon.c \
	$H\sampabp.rh \
	$(MAPIH)\smpab.h \
	$H\abp.h

root.obj: \
	$H\root.c \
	$H\sampabp.rh \
	$H\abp.h

wrap.obj: \
	$H\wrap.c \
	$H\abp.h

abcont.obj: \
	$H\abcont.c \
	$H\sampabp.rh \
	$H\abp.h

absearch.obj: \
	$H\absearch.c \
	$H\sampabp.rh \
	$H\abp.h

abctbl1.obj: \
	$H\abctbl1.c \
	$H\abctbl.h \
	$H\sampabp.rh \
	$H\abp.h

abctbl2.obj: \
	$H\abctbl2.c \
	$H\abctbl.h \
	$H\abp.h

abctbl3.obj: \
	$H\abctbl3.c \
	$H\abctbl.h \
	$H\abp.h

abuser.obj: \
	$H\abuser.c \
	$H\sampabp.rh \
	$H\abp.h

status.obj: \
	$H\status.c \
	$H\sampabp.rh \
	$H\abp.h

tid.obj: \
	$H\tid.c \
	$H\abp.h

oouser.obj: \
	$H\oouser.c \
	$H\abp.h

ootid.obj: \
	$H\ootid.c \
	$H\abp.h


#	rules for files that don't use a pre-compiled header

abguid.obj: $H\abguid.c $(MAPIH)\mapiguid.h
	SET CL=$(CFLAGS) $(DEFS) -I$(MAPIH)
	$(CC) $H\$*.c

mapidbg.obj: $(MAPICOMMON)\mapidbg.c $(MAPIH)\mapidbg.h
	SET CL=$(CFLAGS) $(DEFS) -I$(MAPIH)
	$(CC) $(MAPICOMMON)\$*.c

!IF $(WIN16)
wep.obj: $(MAPICOMMON)\wep.c $(MAPIH)\mapidbg.h
	SET CL=$(CFLAGS) $(DEFS) -I$(MAPIH)
	$(CC) $(MAPICOMMON)\$*.c
!ENDIF

# -----------------------------------------------------------------------------
#
#	Part 4: build executable targets
#

.SUFFIXES: .asm .bas .c .cpp .cxx .des .rc .pch

# TARGET ----------------------------------------------------------------------

TARGET		= $(BASENAME)

# PCHDIR

!IF "$(PCHDIR)" == ""
PCHDIR = $H
!ENDIF

# Resources -------------------------------------------------------------------

!IF "$(RCSOURCE)" != ""
!IF $(WIN32) && $(MSVCNT) == 0
RCTARGET	= rc.rbj
!ELSE
RCTARGET	= rc.res
!ENDIF
!ENDIF

# Other Includes and Libraries ------------------------------------------------

LIBS		= $(OTHERLIBS) $(LIBS)
INCDIRS		= $(OTHERINCS) -I$H -I$(MAPIH) -I$(MAPIINC) -I$(MAPIEXTINC) $(INCDIRS)

# Entry point -----------------------------------------------------------------

!IF $(WIN32) && "$(ENTRYPOINT)" != ""
ENTRYPOINT	= -entry:$(ENTRYPOINT)
!ELSE
!IF $(WIN32) && $(MSVCNT)
LFLAGS = /NOENTRY $(LFLAGS)
!ENDIF
!ENDIF

# C Precompiled Header --------------------------------------------------------

!IF "$(PCH)" != ""
OBJS    = $(PCH).obj $(OBJS)
$(PCH).obj: $(PCH).pch
!IF $(DOECHO)
	echo ignoring precompiled header "C" file $(PCH).c
!ELSE
	rem
!ENDIF

$(PCH).pch: $(PCHDIR)\$(PCH).c $(PCHDEPS)
!IF $(DOECHO)
	echo ++++++++++++++++++++++++++++++++++++++++
	echo BUILDING Pre-compiled header $(PCH).pch
!ENDIF
	SET CL=$(CFLAGS) $(DEFS) $(INCDIRS)
	$(CC) -I$(PCHDIR) -Yc -Fp$(PCH).pch -Fo$(PCH).obj $(PCHDIR)\$(PCH).c

USEPCH		= -Yu$(PCH).h
!ENDIF

# .C to .OBJ ------------------------------------------------------------------

{$H}.c.obj:
!IF $(DOECHO)
	echo +++++++++
	echo COMPILING $*.c
!ENDIF
!IF $(DOECHO) && "$(USEPCH)" != ""
	echo USING Precompiled Header $(PCH).h
!ENDIF
	SET CL=$(CFLAGS) $(DEFS) $(INCDIRS)
	$(CC) $(USEPCH) $H\$*.c

# C++ Precompiled Header ------------------------------------------------------

!IF "$(CPPPCH)" != ""
OBJS    = $(CPPPCH).obj $(OBJS)
$(CPPPCH).OBJ: $(CPPPCH).pch
!IF $(DOECHO)
	echo ignoring precompiled header "C++" file $(CPPPCH).cpp
!ELSE
	rem
!ENDIF

$(CPPPCH).pch: $(PCHDIR)\$(CPPPCH).cpp $(CPPPCHDEPS)
!IF $(DOECHO)
	echo ++++++++++++++++++++++++++++++++++++++++
	echo BUILDING Pre-compiled header $(CPPPCH).pch
!ENDIF
	SET CL=$(CFLAGS) $(DEFS) $(INCDIRS)
	$(CC) -I$(PCHDIR) -Yc -Fp$(CPPPCH).pch -Fo$(CPPPCH).obj $(PCHDIR)\$(CPPPCH).cpp

USECPPPCH	= -Yu$(CPPPCH).h
!ENDIF

# .CPP to .OBJ ----------------------------------------------------------------

{$H}.cpp.obj:
!IF $(DOECHO)
	echo +++++++++
	echo COMPILING $*.cpp
!ENDIF
!IF $(DOECHO) && "$(USECPPPCH)" != ""
	echo USING Precompiled Header $(CPPPCH).h
!ENDIF
	SET CL=$(CFLAGS) $(DEFS) $(INCDIRS)
	$(CC) $(USECPPPCH) $H\$*.cpp

# .ASM to .OBJ ----------------------------------------------------------------

{$H}.asm.obj:
!IF $(DOECHO)
   echo +++++++++
   echo ASSEMBLING $*.asm
!ENDIF
	$(AS) $(AFLAGS) $(DEFS) $H\$*.asm;

# .RC to .RES/.RBJ ------------------------------------------------------------

!IF "$(RCTARGET)" != ""
$(RCTARGET): $(RCSOURCE) $(RCDEPS)
!IF $(DOECHO)
	echo +++++++++
	echo BUILDING $(VERSION) $@.res
!ENDIF
	@SET INCLUDE=$H;$H\..;$(MAPIINC);$(MAPIRESRC);$(INCLUDE)
!IF $(WIN16)
#DRM This next line doesn't work yet.  I'm working on it.
#	SET RC=$(OTHERINCS)
	$(RC) -r $(DEFS) $(OTHERINCS) -forc.res $(RCSOURCE)
!ELSE
	$(RC) -r $(DEFS) $(OTHERINCS) -forc.res $(RCSOURCE)
!ENDIF
!IF $(WIN32) && $(MSVCNT) == 0
	cvtres -$(CPU) rc.res -o $(RCTARGET)
!ENDIF
!ENDIF

# Build a subsystem target ----------------------------------------------------

subsys: $(OBJS)
!IF "$(OBJS)" != ""
	-del allobjs.txt > nul
	type >>allobjs.txt <<
$(OBJS)
<<
!ENDIF

# Building Executable ---------------------------------------------------------

!IF $(PRODEXE)

goal: $(TARGET).exe

$(TARGET).exe: subsys $(RCTARGET)
!IF $(WIN16)
	type >link.txt <<
$(OBJS: = +^
)
$(TARGET).exe
$(TARGET).map
$(LIBS: = +^
)
$H\$(BASENAME)$(WINTAG).def
<<
	$(LINK) $(LFLAGS) @link.txt
!IF "$(RCTARGET)" != ""
	$(RC) -t $(RCTARGET) $@
!ENDIF
!ENDIF
!IF $(WIN32)
!IF $(CHICAGO)
	type >link.txt <<
$(ENTRYPOINT) -map:$(TARGET).map -out:$@
<<
        type allobjs.txt >> link.txt
        type >>link.txt <<
$(RCTARGET) $(LIBS)
<<
        $(LINK) $(LFLAGS) @link.txt
!ELSE
	$(LINK) $(LFLAGS) $(ENTRYPOINT) -map:$(TARGET).map -out:$@ @allobjs.txt $(RCTARGET) $(LIBS)
!ENDIF
!ENDIF
!IF "$(COPYTO)" != ""
	-copy $(TARGET).exe $(COPYTO)
!ENDIF

!ENDIF

# Building DLL ----------------------------------------------------------------

!IF $(PRODDLL)

goal: $(TARGET).dll

lib: $(TARGET).lib

$(TARGET).lib: $H\dll$(WINTAG).def
!IF $(WIN16)
	$(IMPLIB) -nologo $(TARGET).lib $H\dll$(WINTAG).def
!ELSE
	$(LIBR) -machine:$(CPU) -def:$H\dll$(WINTAG).def -out:$@
!ENDIF
!IF "$(COPYTO)" != ""
	-copy $(TARGET).lib $(MAPILIB)
!ENDIF

$(TARGET).dll: subsys $(TARGET).lib $(RCTARGET)
!IF $(WIN16)
	type >link.txt <<
$(OBJS: = +^
)
$(TARGET).dll
$(TARGET).map
$(LIBS: = +^
)
$H\dll$(WINTAG).def
<<
	$(LINK) $(LFLAGS) @link.txt
!IF "$(RCTARGET)" != ""
	$(RC) -t $(RCTARGET) $@
!ENDIF
!ELSE
!IF $(CHICAGO)
	type >link.txt <<
-dll $(ENTRYPOINT) -out:$@ -map:$(TARGET).map -base:@$(MAPI)\mapimchi.txt,$(TARGET)
<<
        type allobjs.txt >> link.txt
        type >>link.txt <<
$(RCTARGET) $(TARGET).exp $(LIBS)
<<
        $(LINK) $(LFLAGS) @link.txt
!ELSE
	$(LINK) $(LFLAGS) -dll $(ENTRYPOINT) -out:$@ -map:$(TARGET).map -base:@$(MAPI)\mapimem.txt,$(TARGET) @allobjs.txt $(RCTARGET) $(TARGET).exp $(LIBS)
!ENDIF
!ENDIF
!IF "$(COPYTO)" != ""
	-copy $(TARGET).dll $(COPYTO)
!ENDIF

!ENDIF

# Building Library ------------------------------------------------------------

!IF $(PRODLIB)

goal: $(TARGET).lib

lib: $(TARGET).lib

$(TARGET).lib: subsys
!IF $(WIN16)
	type > lib.txt <<
$(OBJS: = &^
)
<<
	@if exist $(TARGET).lib del $(TARGET).lib
	$(LIBR) $(TARGET).lib @lib.txt
!ENDIF
!IF $(WIN32)
	$(LIBR) -out:$(TARGET).lib @allobjs.txt
!ENDIF
!IF "$(COPYTO)" != ""
	-copy $(TARGET).lib $(MAPIDLL)
!ENDIF

!ENDIF

# Building Clean --------------------------------------------------------------

clean:
!IF $(DOECHO)
	echo ++++++++++++++++++++++++++++++++++++++++
	echo MAKING CLEAN
!ENDIF
!IF "$(OS)" == "Windows_NT" || "$(FASTCLEAN)" != ""
	-del *.obj *.rbj *.exe *.dll *.exp *.lib *.pch *.pdb *.map *.sym *.txt *.res *.out *.vcw *.wsp $(OTHERCLEAN)
!ELSE
	-del *.obj 
	-del *.rbj 
	-del *.exe 
	-del *.dll 
	-del *.exp 
	-del *.lib 
	-del *.pch 
	-del *.pdb 
	-del *.map 
	-del *.sym 
	-del *.txt 
	-del *.res 
	-del *.out 
	-del *.vcw 
	-del *.wsp
!IF "$(OTHERCLEAN)" != ""
	-del $(OTHERCLEAN)
!ENDIF
!ENDIF

