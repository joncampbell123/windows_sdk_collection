# -----------------------------------------------------------------------------
# $(SAMPLES)\src\makestuf\xmake1.mak
#
# Copyright (C) 1995 Microsoft Corporation
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
#
#       Part 1: define the version to be built
#

#Default build flavor
!IF "$(VERSION)" == ""
VERSION=SHIP
!ENDIF

# Conditional key for MAPI dev tree vs. setup tree
DEVTREE=0

# -----------------------------------------------------------------------------
# Definitions depending on if this is in Setup vs Test tree

# Default samples location
!IF "$(SAMPLES)" != "" && $(DEVTREE)
MAPI=$(SAMPLES)
!ENDIF

# Additional Includes
!IF "$(DEVTREE)" == "0"
OTHERINCS = -I(MAPIH) -I..\h
!ENDIF

# -----------------------------------------------------------------------------
# The PRODUCT variable must be defined to be one of the following:
#       
#       WIN16EXE        Win16 Windows Application
#       WIN16DLL        Win16 DLL
#       WIN16LIB        Win16 Static library
#       WIN32EXE        Win32 Windows Application (single thread)
#       WIN32DLL        Win32 DLL (single thread)
#       WIN32DLLMT      Win32 DLL (multi thread)
#       WIN32LIB        Win32 Static library
#       CHIEXE          Win32c Windows Application (single thread)
#       CHIDLL          Win32c DLL (32-bit, single thread)
#       CHIDLLMT        Win32c DLL (multi thread)
#       CHILIB          Win32c Static library
#       WIN16DLLMFC     Win16 DLL (MFC)
#       WIN32DLLMFC     Win32 DLL (MFC)
#       CHIDLLMFC       Win32c DLL (MFC)
#
# The VERSION variable must be defined to be one of the following:
#
#       DEBUG           Debug build (full symbols, full debug info, no optimizations)
#       TEST            Test build (symbols only, full optimizations)
#       SHIP            Ship build (no symbols, full optimizations)
#
# The following environment variables should be defined:
#
#       MAPI            Root of the MAPI project (defaults to c:\mapi)
#       TMP                     Temp directory (defaults to c:\temp)
#       CPU                     CPU to be targeted (defaults to i386)
#       LANG            The language version to compile (defaults to usa)
#       NOSILENT        Set to 1 to prevent silent execution of nmake
#       NOECHO          Set to 1 to prevent echo of progress
#
# As a result of the above definitions, the following variables are defined:
#
#       WIN16           Set to 1 if producing a Win16 product
#       WIN32           Set to 1 if producing a Win32 product
#       WINTAG          Set to 16/32/chi/dos/mac depending on PRODUCT
#
#       PRODEXE         Set to 1 if producing an EXE
#       PRODDLL         Set to 1 if producing a DLL
#       PRODMT          Set to 1 if producing a multi threaded product
#
#       VERDEBUG        Set to 1 if VERSION is DEBUG
#       VERTEST         Set to 1 if VERSION is TEST
#       VERSHIP         Set to 1 if VERSION is SHIP
#       VERTAG          Set to dbg/shp/tst depending on VERSION
#
#       MAPIBIN         Set to $(MAPI)\bin
#       MAPILIB         Set to $(MAPI)\lib\$(WINTAG)
#       MAPIDLL         Set to $(MAPI)\lib\$(WINTAG)\$(VERTAG)
#       MAPICOMMON      Set to $(MAPI)\src\common
#
# -----------------------------------------------------------------------------

# Silence and Echo ------------------------------------------------------------

!IF "$(NOSILENT)" == ""
!CMDSWITCHES +s
!ENDIF

!IF "$(NOECHO)" == ""
DOECHO      = 1
!ELSE
DOECHO      = 0
!ENDIF

# Process the PRODUCT variable ------------------------------------------------

WIN16       = 0
WIN32       = 0
WIN95       = 0

PRODEXE     = 0
PRODLIB     = 0
PRODDLL     = 0
PRODMT      = 0
PRODMFC     = 0

!IF "$(PRODUCT)" == "WIN16EXE"
WIN16       = 1
PRODEXE     = 1
!ENDIF

!IF "$(PRODUCT)" == "WIN16DLL"
WIN16       = 1
PRODDLL     = 1
!ENDIF

!IF "$(PRODUCT)" == "WIN16DLLMFC"
WIN16       = 1
PRODDLL     = 1
PRODMFC     = 1
!ENDIF

!IF "$(PRODUCT)" == "WIN16LIB"
WIN16       = 1
PRODLIB     = 1
!ENDIF

!IF "$(PRODUCT)" == "WIN32EXE"
WIN32       = 1
PRODEXE     = 1
!ENDIF

!IF "$(PRODUCT)" == "WIN32DLL"
WIN32       = 1
PRODDLL     = 1
!ENDIF

!IF "$(PRODUCT)" == "WIN32DLLMFC"
WIN32       = 1
PRODDLL     = 1
PRODMFC     = 1
!ENDIF

!IF "$(PRODUCT)" == "WIN32DLLMT"
WIN32       = 1
PRODDLL     = 1
PRODMT      = 1
!ENDIF

!IF "$(PRODUCT)" == "WIN32LIB"
WIN32       = 1
PRODLIB     = 1
!ENDIF

!IF "$(PRODUCT)" == "CHIEXE"
WIN32       = 1
PRODEXE     = 1
WIN95       = 1
!ENDIF

!IF "$(PRODUCT)" == "CHIDLL"
WIN32       = 1
PRODDLL     = 1
WIN95       = 1
!ENDIF

!IF "$(PRODUCT)" == "CHIDLLMFC"
WIN32       = 1
PRODDLL     = 1
WIN95       = 1
PRODMFC     = 1
!ENDIF

!IF "$(PRODUCT)" == "CHIDLLMT"
WIN32       = 1
PRODDLL     = 1
PRODMT      = 1
WIN95       = 1
!ENDIF

!IF "$(PRODUCT)" == "CHILIB"
WIN32       = 1
PRODLIB     = 1
WIN95       = 1
!ENDIF

WINTAG      =

!IF $(WIN16)
WINTAG      = 16
!ENDIF

!IF $(WIN32)
!IF $(WIN95)
WINTAG      = chi
!ELSE
WINTAG      = 32
!ENDIF
!ENDIF

!IF "$(WINTAG)" == ""
!ERROR "The PRODUCT variable has not been defined correctly"
!ENDIF

# Process the VERSION variable ------------------------------------------------

VERDEBUG    = 0
VERTEST     = 0
VERSHIP     = 0

!IF     "$(VERSION)" == "DEBUG"
VERDEBUG    = 1
VERTAG      = dbg
!ENDIF

!IF "$(VERSION)" == "TEST"
VERTEST     = 1
VERTAG      = tst
!ENDIF

!IF "$(VERSION)" == "SHIP"
VERSHIP     = 1
VERTAG      = shp
!ENDIF

!IF $(VERDEBUG) == 0 && $(VERTEST) == 0 && $(VERSHIP) == 0
!ERROR "The VERSION variable has not been defined correctly"
!ENDIF

# Defaults --------------------------------------------------------------------

!IF "$(MAPI)" == ""
MAPI        = c:\mapi
!ENDIF

!IF "$(TMP)" == ""
TMP         = c:\temp
!ENDIF

!IF "$(LANG)" == ""
LANG        = usa
!ENDIF

!IF "$(CPU)" == ""
CPU         = i386
!ENDIF

!IF "$(MSVCNT)" == ""
MSVCNT      = 1             # Change this to a 0 to make NT SDK compiler the default
!ENDIF

#REM Key to CPU specific conditions, CPU == i386 || ALPHA || MIPS     1) cvtres
# 	cvtres -$(CPU) ...
!IF "$(CPU)" == ""
!IF "$(PROCESSOR_ARCHITECTURE)" == "MIPS" || "$(PROCESSOR_ARCHITECTURE)" == "ALPHA"
CPU = $(PROCESSOR_ARCHITECTURE)
!ELSE
CPU = i386
!ENDIF
!ENDIF

# Construct directory variables -----------------------------------------------
!IF "$(MAPILIB)" == ""
MAPILIB     = $(MAPI)\lib\$(WINTAG)
!ENDIF

!IF $(DEVTREE)
SRCROOT     = src
!IF "$(MAPIDLL)" == ""
MAPIDLL     = $(MAPI)\lib\$(WINTAG)\$(VERTAG)
!ENDIF
!ELSE
SRCROOT     = dev
!IF "$(MAPIDLL)" == ""
MAPIDLL     = $(MAPI)
!ENDIF
!ENDIF

# In Test tree configuration, recognize that user is a tester and
# requires different setup than build.  Build environment will have
# Environment variable MAPIPRIME set for the NMAKE session

!IFNDEF MAPIPRIME


# Default samples location
!IF "$(SAMPLES)" != "" && $(DEVTREE)

!IF $(WIN16)
MAPISLIB    = $(MAPI16)
!ELSE
MAPISLIB    = $(MAPI32)
!ENDIF

!ELSE

!IF $(WIN16)
MAPISLIB    = $(MAPI)
!ELSE
MAPISLIB    = $(MAPI)
!ENDIF

!ENDIF

!ELSE

MAPISLIB    = $(MAPIPRIME)\lib\$(WINTAG)\$(VERTAG)
!ENDIF


MAPIH       = $(MAPI)\h
MAPICOMMON  = $(MAPI)\$(SRCROOT)\common
Z           = $(MAPI)

# Construct MAKEDEFS ----------------------------------------------------------

MAKEDEFS        = PRODUCT=$(PRODUCT) VERSION=$(VERSION)

# Definitions -----------------------------------------------------------------

DEFS        =
AFLAGS      =
CFLAGS      =
LFLAGS      =
INCDIRS     =

!IF $(VERDEBUG)
DEFS        = $(DEFS) -DDEBUG -D_DEBUG
!ENDIF

!IF $(VERTEST)
DEFS        = $(DEFS) -DTEST
!ENDIF

!IF $(VERSHIP)
DEFS        = $(DEFS) -DSHIP
!ENDIF

# Programs --------------------------------------------------------------------

!IF $(WIN16)
CC          = cl
CPRE        = cl -nologo -EP
LINK        = link
LIBR        = lib
IMPLIB      = implib
!ENDIF


!IF $(WIN32) 

#REM   MSVCNT  == MS Visual C++ = 1      ==0 for SDK
#REM   LIBSDLL == C run time DLL lib
!IF "$(MSVCNT)" == "1"   &&  "$(LIBSDLL)" == ""
LIBSDLL = MSVCRT.LIB
!ENDIF
!IF "$(LIBSDLL)" == ""
LIBSDLL = CRTDLL.LIB
!ENDIF

#REM Compiler name:  1) Compile flags and   2) run the C Compiler  and  3) midl compiler name
# 1)	set $(CC)= ... -D$(CCPU)#1 ..
# 2)	$(CC) ...  name.c
# 3)    midl ...  cpp_cmd $(CC)
!IF "$(CPU)" == "ALPHA"   &&  "$(MSVCNT)" != "1"  &&  "$(cc)" == ""
cc = claxp
!ENDIF
!IF "$(CPU)" == "ALPHA"   &&  "$(MSVCNT)" == "1"
cc = cl
!ENDIF
!IF "$(cc)" == ""
cc = cl
!ENDIF

#REM Define compiler constant = _X86_ || _ALPHA_ || _MIPS_
#	$(CC) file.c -D$(CCPU) ...
!IF "$(CPU)" == "i386"    &&  "$(CCPU)" == ""
CCPU = _X86_
!ENDIF
!IF "$(CPU)" != "i386"    &&  "$(CCPU)" == ""
CCPU = _$(CPU)_
!ENDIF

#REM Link and Lib machine name: IX86 || MIPS || ALPHA
#	link -machine:$(LCPU)
#	lib  -machine:$(LCPU)
!IF "$(CPU)" == "i386"    &&  "$(LCPU)" == ""
LCPU = IX86
!ENDIF
!IF "$(CPU)" != "i386"    &&  "$(LCPU)" == ""
LCPU = $(CPU)
!ENDIF

#REM Link alignment i386 = 4096     ALPHA = 8192
#	link -machine:$(LCPU) -align:$(PAGE_SZ)
!IF "$(CPU)" == "ALPHA"   &&  "$(PAGE_SZ)" == ""
PAGE_SZ = 0x2000
!ENDIF
!IF "$(CPU)" != "ALPHA"   &&  "$(PAGE_SZ)" == ""
PAGE_SZ = 0x1000
!ENDIF

#REM Entrypoint names  and flag for numeric entry name extension  if $(DLLENTRY)="" then
#   no numbers shuld be appended to entry points.
#	link -entry:_DllMainCRTStartup$(DLLENTRY)
!IF "$(CPU)" == "i386"    &&  "$(DLLENTRY)" == ""
DLLENTRY = @12
!ENDIF
!IF "$(CPU)" != "i386"	  &&  "$(DLLENTRY)" == "@12"
DLLENTRY=
!ENDIF
!ENDIF

!IF $(WIN32) && "$(CPU)" == "i386"
!IF $(MSVCNT)
CC                      = cl
!ELSE
CC                      = cl386
!ENDIF
CCPU            = _X86_
CVTOBJ          = REM MIPS only
!ENDIF

!IF "$(CPU)" != "i386" && "$(CPU)" != "M68K"
CFLAGS		=  $(CFLAGS) -D_NTSDK
!ENDIF

!IF $(WIN32)
CPRE        = $(CC) -EP
!IF $(MSVCNT)
LINK        = link 
LIBR        = lib
!ELSE
LINK        = link32
LIBR        = lib32
!ENDIF
!ENDIF

!IF "$(OS)" == "Windows_NT"
MKTOOL      = mktool
!ELSE
MKTOOL      = mktool16
!ENDIF

AS          = masm

!IF "$(WARNLEVEL)" == ""
WARNLEVEL   = 3
!ENDIF

# Win16 Definitions -------------------------------------------------------

!IF $(WIN16)

# Set WINVER = 0x300 -not- 0x030a to build AFX with MAPI
# This informs AFX to not include shellapi.h, which causes duplicate
# definition warnings when building 16Bit

DEFS        = $(DEFS) -DWIN16
CMODEL      = -AL

!IF $(PRODEXE)
CMODEL     = $(CMODEL) -Alf -GA
!ENDIF

# Assume a static lib may be included in a DLL
!IF $(PRODDLL) || $(PRODLIB)
CMODEL      = $(CMODEL) -Alfu -D_WINDLL
!ENDIF

AFLAGS      = -Mx -t
CFLAGS      = $(CFLAGS) -nologo -c $(CMODEL) -W$(WARNLEVEL) -Zp -J -Ge -Gt16
LFLAGS      = $(LFLAGS) /align:16 /NOLOGO /ONERROR:NOEXE /NOD /NOE /MAP

!IF $(PRODDLL) && $(PRODMFC)
DEFS        = $(DEFS) /D_USRDLL
!ENDIF

!IF "$(DUMPASM)" != ""
CFLAGS      = $(CFLAGS) -Fa$*.out
!ENDIF

!IF $(VERDEBUG)
AFLAGS      = $(AFLAGS) -Z -Zi -L
CFLAGS      = $(CFLAGS) -f -Zi -Od -Ob1
LFLAGS      = $(LFLAGS) /CO
!ENDIF

!IF $(VERTEST)
AFLAGS      = $(AFLAGS) -Zi
CFLAGS      = $(CFLAGS) -Zi -Oilx -Ob1
LFLAGS      = $(LFLAGS) /CO
!ENDIF

!IF $(VERSHIP)
CFLAGS      = $(CFLAGS) -Zd -Oilx -Ob1
!ENDIF

CFLAGS      = $(CFLAGS) -G2

!ENDIF

# Win32 Definitions -----------------------------------------------------------

!IF $(WIN32)

DEFS        = $(DEFS) -DWIN32 -D$(CCPU)=1 /D_NTWIN /D_AFX_NO_BSTR_SUPPORT

!IF $(WIN95)
DEFS        = $(DEFS) -D_WIN95 
!ELSE
DEFS        = $(DEFS) -D_WINNT
!ENDIF

!IF "$(CPU)" == "ALPHA"
CMODEL		= -D_NTSDK -Gz
LFLAGS		= $(LFLAGS) -align:$(PAGE_SZ) -MACHINE:$(LCPU)
!IF $(PRODDLL)
CMODEL		= $(CMODEL) -D_DLL
!ENDIF
!ENDIF

!IF "$(CPU)" == "MIPS"
CMODEL		= -D_NTSDK
LFLAGS		= $(LFLAGS) -align:$(PAGE_SZ) -MACHINE:$(LCPU)
!IF $(PRODDLL)
CMODEL		= $(CMODEL) -D_DLL
!ENDIF
!ENDIF

!IF "$(CPU)" == "i386"
CMODEL		= -Bbb1 -G3s
LFLAGS		= $(LFLAGS) -align:$(PAGE_SZ) -MACHINE:$(LCPU)
!ENDIF

AFLAGS		= -Mx -t
CFLAGS      = $(CFLAGS) -nologo -c $(CMODEL) -Gf -W$(WARNLEVEL) -J
LFLAGS		= $(LFLAGS) -incremental:no -pdb:none -release

!IF "$(DOPROFILE)" != ""
CFLAGS      = $(CFLAGS) -Gh
!ENDIF

!IF $(MSVCNT)
!IF "$(CPU)" == "MIPS"
MAXOPT		= -Ob1 -Os -Oi
!else
MAXOPT		= -Ob1 -O1 -Oi
!endif
!ELSE
LFLAGS		= $(LFLAGS) -ignore:505
MAXOPT		= -Ob1 -Og -Os -Oi
!ENDIF

!IF "$(DUMPASM)" != ""
CFLAGS      = $(CFLAGS) -Fa$*.out -FAs
!ENDIF

!IF $(PRODEXE)
LFLAGS          = $(LFLAGS) -subsystem:windows
ENTRYPOINT      = WinMainCRTStartup
!ENDIF

!IF $(PRODDLL)
!IF $(WIN95)
LFLAGS      = $(LFLAGS) -subsystem:windows,4.00
!ELSE
LFLAGS      = $(LFLAGS) -subsystem:windows
!ENDIF
!ENDIF

!IF $(PRODMT) && $(PRODDLL)
ENTRYPOINT  = _CRT_INIT@12
!ENDIF

!IF $(PRODMFC) && $(PRODDLL)
DEFS        = $(DEFS) /D_USRDLL /D_WINDLL
CFLAGS      = $(CFLAGS) -LD
!IF "$(CPU)" == "MIPS" || "$(CPU)" == "ALPHA"
ENTRYPOINT  = _DllMainCRTStartup
!ELSE
ENTRYPOINT  = _DllMainCRTStartup@12
!ENDIF
!ENDIF

!IF $(VERDEBUG)
AFLAGS      = $(AFLAGS) -Z -Zi -L
#CFLAGS     = $(CFLAGS) -Od -Z7

CFLAGS      = $(CFLAGS) -Ob1 -Od -Z7
LFLAGS      = $(LFLAGS) -debug:full -debugtype:cv
!ENDIF

!IF $(VERTEST)
AFLAGS      = $(AFLAGS) -Zi
CFLAGS      = $(CFLAGS) $(MAXOPT) -Z7
LFLAGS      = $(LFLAGS) -debug:full -debugtype:cv
!ENDIF

!IF $(VERSHIP)
CFLAGS      = $(CFLAGS) $(MAXOPT)
LFLAGS      = $(LFLAGS) -debug:none
!ENDIF

!ENDIF

# Libraries -------------------------------------------------------------------

!IF $(WIN16)
LIBS        = oldnames libw

!IF $(PRODEXE)
LIBS        = $(LIBS) llibcew
!ENDIF

!IF $(PRODDLL)
LIBS        = $(LIBS) ldllcew
!ENDIF

!ENDIF


!IF $(WIN32)

LIBS        =   kernel32.lib user32.lib gdi32.lib \
                winspool.lib comdlg32.lib advapi32.lib \
                shell32.lib libcmt.lib

!IF "$(DOPROFILE)" != ""
LIBS        = $(LIBS) cap.lib
!ENDIF

!IF $(PRODMT)
LIBS        = $(LIBS) libcmt.lib
!ELSE
!IF $(PRODDLL)
LIBS        = $(LIBS) libc.lib
!ELSE
LIBS        = $(LIBS) libc.lib
!ENDIF
!ENDIF

!ENDIF

# default goal target ---------------------------------------------------------

defgoal:    goal

