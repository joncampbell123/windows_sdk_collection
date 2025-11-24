#
# Copyright (c) 1993-1995, Microsoft Corp.  All rights reserved.
#
# =========================================================================
#
# W32CHICO.MK - Win32 application definition file for building Win32 
#               apps under Chicago
#
# -------------------------------------------------------------------------
# Makefile options
#    (These equates must precede the !include $(SDKROOT)\INC32\W32CHICO.MK)
#
#      SDKROOT -> Path to SDK (eg. SDKROOT = C:\SDK)
#      C32BIN -> Path to 32-bit compiler/linker/librarian
#      C32INC -> Path to 32-bit compiler include files
#      C32LIB -> Path to 32-bit compiler libraries
#      PROJ -> Name of .EXE, .RES, .RC, .MAP, .H files (eg. PROJ = GENERIC).
#      OBJS -> Object files to be built from .C files.  You must
#              have dependencies for all files in this list.  These
#              dependencies follow the !include $(SDKROOT)\INC32\W32CHICO.MK.
#              If OBJS is not defined, OBJS is set to $(PROJ).OBJ
#              (eg. OBJS = generic.obj foo.obj).
#      MAKEDLL -> If defined, builds a DLL, not an EXE (eg. MAKEDLL = 1)
#      MAKEHELP -> If defined, tries to find and build help files
#              (eg. MAKEHELP = 1).
#      OWNMAKE -> If defined, disables the automatic make system.
#              In this case, the makefile must provide link and cc
#              rules (eg. OWNMAKE = 1).
#      LIBS -> List extra libraries needed for this executable.
#              (eg. LIBS = extra.lib)
#      MULTITHREADED -> Define this if executable is for multithreaded use.
#              This defines _MT for source files and uses different CRT
#              libraries (eg. MULTITHREADED = 1).
#      CRTDLL -> Define this if CRTDLL.DLL should be used.  Otherwise, LIBC
#              (or LIBCMT) is used (eg. CRTDLL = 1).
#      CONSOLE -> Define this if the app is to be a console app
#              (eg. CONSOLE = 1).
#      DLLENTRY -> If a DLL entrypoint other than the default CRT entry
#              is used, this variable specifies the name
#              (eg. DLLENTRY = WinFoo).
#      DLLBASE -> Used to manually set the image base for a DLL
#              (default: DLLBASE = 0x40000000)
#      INCLUDE -> Sets include path for build (eg. INCLUDE = e:\foobar;d:\foo).
#      LIB -> Sets LIB path for build (eg. LIB = e:\libdir;d:\foo).
#
#    You must include dependencies for all .OBJ files in the OBJS list!!
#
# -------------------------------------------------------------------------
#
# Application Information Type         Invoke NMAKE
# ----------------------------         ------------
# For No Debugging Info                nmake NODEBUG=1
#
# Note: using the environment variable NODEBUG, is an alternate method 
#       for setting this option.
#
# Additional NMAKE Options             Invoke NMAKE
# ----------------------------         ------------
# For No ANSI NULL Compliance          nmake no_ansi=1
# (ANSI NULL is defined as PVOID 0)
#
#
# =========================================================================

CPU = i386
CPUTYPE = 1

!ifndef INCLUDE
INCLUDE = .
!else
INCLUDE = $(INCLUDE);.
!endif

!ifndef LIB
LIB = .
!else
LIB = $(LIB);.
!endif

# Path declarations
inclpath = $(SDKROOT)\inc32;$(C32INC);$(INCLUDE)
libpath = $(SDKROOT)\lib32;$(C32LIB);$(LIB)
PATH = $(C32BIN);$(SDKROOT)\binw;$(SDKROOT)\bin;$(PATH)

# binary declarations
cc     = cl

cflags = -c -W3 -G3 -D_X86_=1
lflags = -align:0x1000 -nodefaultlib
msymflags = -s
scall  = -Gz

!IFDEF NODEBUG
cdebug = -Ox
!ELSE
cdebug = -Zi -Od
!ENDIF

# -------------------------------------------------------------------------
# Target Module & Subsystem Dependant Compile Defined Variables - must be
#   specified after $(cc)
#
# The following is a table which indicates the various
# acceptable combinations of CRTDLL and LIBC(MT) with
# respect to DLLs and EXEs, along with the appropriate
# compiler flags for each.
#                           Libraries   
# Link EXE    Create Exe     to link    Link DLL    Create DLL
#   with        Using        with EXE     with         Using
# ----------------------------------------------------------------
#  LIBC        CVARS        GUILIBS       None        None      *
#  LIBC        CVARS        GUILIBS       LIBC        CVARS
#  LIBC        CVARS        GUILIBS       LIBCMT      CVARSMT
#  LIBCMT      CVARSMT      GUILIBSMT     None        None      *
#  LIBCMT      CVARSMT      GUILIBSMT     LIBC        CVARS
#  LIBCMT      CVARSMT      GUILIBSMT     LIBCMT      CVARSMT
#  CRTDLL      CVARSDLL     GUILIBSDLL    None        None      *
#  CRTDLL      CVARSDLL     GUILIBSDLL    LIBC        CVARS
#  CRTDLL      CVARSDLL     GUILIBSDLL    LIBCMT      CVARSMT
#  CRTDLL      CVARSDLL     GUILIBSDLL    CRTDLL      CVARSDLL  *
#
# Note: Any executable which accesses a DLL linked with CRTDLL.LIB must
#       also link with CRTDLL.LIB instead of LIBC.LIB or LIBCMT.LIB.
#       When using DLLs, it is recommended that all of the modules be
#       linked with CRTDLL.LIB.
#
# * - Recommended Configurations
#
# -------------------------------------------------------------------------

!ifdef NO_ANSI
noansi = -DNULL=0
!endif

cvars      = -DWIN32 $(noansi)
cvarsmt    = $(cvars) -D_MT
cvarsdll   = $(cvarsmt) -D_DLL
cvarsmtdll = $(cvarsmt) -D_DLL

# resource compiler
rcvars = -DWIN32 $(noansi)

# Default entrypoint name
!ifdef MAKEDLL
!ifndef DLLENTRY
defentry = DLLMainCRTStartup
!else
defentry = $(DLLENTRY)@12
!endif
!else 
!ifdef CONSOLE
defentry = mainCRTStartup
!else
defentry = WinMainCRTStartup
!endif
!endif

# basic subsystem specific libraries less the C-RunTime
baselibs   = kernel32.lib
winlibs    = kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib \
             winspool.lib

!ifdef CONSOLE
defaultlibs = $(baselibs)
!else
defaultlibs = $(winlibs)
!endif

# For applications that use the C-Runtime libraries
conlibs    = libc.lib $(baselibs)
conlibsmt  = libcmt.lib $(baselibs)
conlibsdll = msvcrt.lib $(baselibs)
guilibs    = libc.lib $(winlibs)
guilibsmt  = libcmt.lib $(winlibs)
guilibsdll = msvcrt.lib $(winlibs)
deflibs    = libc.lib $(defaultlibs)
deflibsmt  = libcmt.lib $(defaultlibs)
deflibsdll = msvcrt.lib $(defaultlibs)

# -------------------------------------------------------------------------
# Target Module Dependent Link Debug Flags - must be specified after $(link)
#
# These switches allow for source level debugging with WinDebug, profiling
# or performance tuning.
#
# Note: Debug switches are on by default.
# -------------------------------------------------------------------------

!ifdef NODEBUG
ldebug =
!else
ldebug = -debug:full -debugtype:cv
!endif

# -------------------------------------------------------------------------
# Subsystem Dependent Link Flags - must be specified after link
#
# These switches allow for source level debugging with WinDebug for local
# and global variables.
# -------------------------------------------------------------------------

# For applications that use the C-Runtime libraries
conlflags = -subsystem:console,4.0 -entry:$(defentry) $(lflags)
guilflags = -subsystem:windows,4.0 -entry:$(defentry) $(lflags)
!ifdef CONSOLE
deflflags = $(conlflags)
!else
deflflags = $(guilflags)
!endif


# -------------------------------------------------------------------------
# Automatic build section.
#
# Note that if OWNMAKE is defined, none of this is executed and the
# host makefile is responsible
#
# See definitions at top of file for equates to drive automatic building
# -------------------------------------------------------------------------

!ifndef OWNMAKE

# Figure out the correct CRT combination to use
!ifdef CRTDLL
targcvars = $(cvarsdll)
targlibs = $(LIBS) $(deflibsdll)
!else
!ifdef MULTITHREADED
targcvars = $(cvarsmt)
targlibs = $(LIBS) $(deflibsmt)
!else
targcvars = $(cvars)
targlibs = $(LIBS) $(deflibs)
!endif
!endif

# Figure out if we're making a DLL or an EXE
!ifdef MAKEDLL
target = $(PROJ).dll
!else
target = $(PROJ).exe
!endif

all: $(target)

!ifndef OBJS
OBJS = $(PROJ).obj
!endif

$(target): $(OBJS)

# Update the help file if necessary
!ifdef MAKEHELP
$(PROJ).hlp : $(PROJ).rtf
    hc -n $(PROJ).hpj
!endif

# Update the resource if necessary
!ifndef CONSOLE
$(PROJ).res: $(PROJ).rc
        set INCLUDE=$(inclpath)
        rc $(rcvars) -r -fo $(PROJ).res $(cvars) $(PROJ).rc
!endif

# Object file dependencies
.c.obj:
        set INCLUDE=$(inclpath)
        $(cc) $(cflags) $(targcvars) $(cdebug) $*.c

.cxx.obj:
        set INCLUDE=$(inclpath)
        $(cc) $(cflags) $(targcvars) $(cdebug) $*.cxx

# Target dependcies
targdepend = $(OBJS)
!ifndef CONSOLE
targdepend = $(targdepend) $(PROJ).res
!endif
!ifdef MAKEHELP
targdepend = $(targdepend) $(PROJ).hlp
!endif

# Link the target
$(target): $(targdepend)
        set LIB=$(libpath)
# If this is a DLL, we need to make an import library
!ifdef MAKEDLL
        link -lib @<<
-out:$(PROJ).lib
-def:$(PROJ).def
$(OBJS)
<<
!endif
        link -link @<<
$(ldebug)
$(deflflags)
-out:$(target)
-map:$(PROJ).map
!ifdef MAKEDLL
-dll
!ifdef DLLBASE
-base:$(DLLBASE)
!else
-base:0x40000000
!endif
$(PROJ).exp
!else
-base:0x400000
!endif
$(OBJS)
$(targlibs)
VERSION.LIB
!ifndef CONSOLE
$(PROJ).res
!endif
<<
        mapsym $(msymflags) $(PROJ).map

clean:
   del *.obj
   del *.exe
   del *.res
   del *.map
   del *.sym
   del *.hlp
   del *.pdb

!endif

