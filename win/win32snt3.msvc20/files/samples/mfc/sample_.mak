# This is a part of the Microsoft Foundation Classes C++ library.
# Copyright (C) 1992,93 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Microsoft Foundation Classes Reference and Microsoft
# QuickHelp and/or WinHelp documentation provided with the library.
# See these sources for detailed information regarding the
# Microsoft Foundation Classes product.

# Common include for building MFC Sample programs
#
#  typical usage
#       PROJ=foo
#       OBJS=foo.obj bar.obj ...
#       !INCLUDE ..\SAMPLE_.MAK
#
#  ROOT specifies the location of the msvc20\samples\mfc directory,
#  relative to the project directory. Because the MFC tutorial samples
#  have an intermediate STEP<n> subdirectory, they use
#       ROOT=..\..
#  instead of the default
#       ROOT=..
#
# NOTE: do not include 'stdafx.obj' in the OBJS list - the correctly
#    built version will be included for you
#
# Options to NMAKE:
#     "PLATFORM=?"
#       This option chooses the appropriate tools and sources for the
#       different platforms support by Windows/NT.  Currently INTEL,
#       MIPS, ALPHA, MAC_68K, and MAC_PPC are supported; more will be
#       added as they become available.  The default is chosen based on
#       the host environment.  It must be set for MAC_ builds.
#     "DEBUG=0"     use retail (default debug)
#     "CODEVIEW=1"  include codeview info (even for retail builds)
#     "AFXDLL=1"    to use shared DLL version of MFC
#     "UNICODE=1"   to build UNICODE enabled applications
#                   (not all samples support UNICODE)
#     "NO_PCH=1"    do not use precompiled headers (defaults to use pch)
#     "COFF=1"      include COFF symbols

!ifndef PROJ
!ERROR You forgot to define 'PROJ' symbol!!
!endif

!ifndef ROOT
ROOT=..
!endif

!ifndef OBJS
!ERROR You forgot to define 'OBJS' symbol!!
!endif

!ifndef DEBUG
DEBUG=1
!endif

!ifndef AFXDLL
AFXDLL=0
!endif

!ifndef UNICODE
UNICODE=0
!endif

!ifndef PLATFORM
!ifndef PROCESSOR_ARCHITECTURE
!error PLATFORM must be set to intended target
!endif
!if "$(PROCESSOR_ARCHITECTURE)" == "x86"
PLATFORM=INTEL
!endif
!if "$(PROCESSOR_ARCHITECTURE)" == "ALPHA"
PLATFORM=ALPHA
!endif
!if "$(PROCESSOR_ARCHITECTURE)" == "MIPS"
PLATFORM=MIPS
!endif
!if "$(PROCESSOR_ARCHITECTURE)" == "PPC"
PLATFORM=PPC
!endif
!endif

!ifndef NTSDK
!if "$(PLATFORM)"=="INTEL"
NTSDK=0
!endif
!if "$(PLATFORM)"=="MIPS"
NTSDK=0
!endif
!if "$(PLATFORM)"=="MAC_68K"
NTSDK=0
!endif
!if "$(PLATFORM)"=="MAC_PPC"
NTSDK=0
!endif
!ifndef NTSDK
NTSDK=1
!endif
!endif

!ifndef USES_OLE
USES_OLE=0
!endif

!ifndef USES_DB
USES_DB=0
!endif

!ifndef CONSOLE
CONSOLE=0
!endif

!ifndef NO_PCH
NO_PCH=0
!endif

#
# Set BASE=W or BASE=M depending on platform
#
BASE=W
!if "$(PLATFORM)" == "MAC_68K" || "$(PLATFORM)" == "MAC_PPC"
!undef BASE
BASE=M
!endif

!if "$(UNICODE)" == "0"
!if "$(AFXDLL)" == "0"
STDAFX=stdafx
!else
STDAFX=stddll
!endif
!endif

!if "$(UNICODE)" == "1"
!if "$(AFXDLL)" == "0"
STDAFX=uniafx
!else
STDAFX=unidll
!endif
!endif

!if "$(DEBUG)" == "1"
STDAFX=$(STDAFX)d
!if "$(COFF)" != "1"
!ifndef CODEVIEW
CODEVIEW=1
!endif
!endif
!endif

!if "$(CODEVIEW)" == "1"
STDAFX=$(STDAFX)v
!endif

!if "$(DEBUG)" == "1"
DEBUG_SUFFIX=d
!endif

!if "$(DEBUG)" != "0"
DEBUGFLAGS=/Od
MFCDEFS=$(MFCDEFS) /D_DEBUG

!if "$(PLATFORM)" == "MAC_68K"
DEBUGFLAGS=/Q68m
!endif
!endif

!if "$(DEBUG)" == "0"
!if "$(PLATFORM)" == "INTEL"
DEBUGFLAGS=/O1 /Gy
!endif
!if "$(PLATFORM)" == "MIPS"
DEBUGFLAGS=/O1 /Gy
!endif
!if "$(PLATFORM)" == "ALPHA"
DEBUGFLAGS=/O1 /Gy
!endif
!if "$(PLATFORM)" == "PPC"
DEBUGFLAGS=/O1 /Gy
!endif
!if "$(PLATFORM)" == "MAC_68K"
DEBUGFLAGS=/O1 /Gy
!endif
!if "$(PLATFORM)" == "MAC_PPC"
DEBUGFLAGS=/O1 /Gy
!endif
!endif # DEBUG == 0

!if "$(CODEVIEW)" == "1" || "$(COFF)" == "1"
DEBUGFLAGS=$(DEBUGFLAGS) /Z7
!endif

!if "$(UNICODE)" == "1"
DLL_SUFFIX=u
!endif

!if "$(AFXDLL)" == "1"
MFCFLAGS=$(MFCFLAGS) /MD
MFCDEFS=$(MFCDEFS) /D_AFXDLL
!endif # AFXDLL == 1

!if "$(AFXDLL)" == "0"
!if "$(BASE)" != "M" && "$(NTSDK)" != "1"
MFCFLAGS=$(MFCFLAGS) /MT
!endif
!endif

!if "$(UNICODE)" == "1"
MFCDEFS=$(MFCDEFS) /D_UNICODE
!else
!if "$(NTSDK)" != "1"
MFCDEFS=$(MFCDEFS) /D_MBCS
!endif
!endif

!if "$(BASE)" == "M"
MFCDEFS=$(MFCDEFS) /D_MAC
!endif

!if "$(PLATFORM)" == "INTEL"
MFCDEFS=$(MFCDEFS) /D_X86_
CPP=cl
CFLAGS=/GX /c /W3 $(DEBUGFLAGS) $(MFCFLAGS) $(MFCDEFS)
!endif

!if "$(PLATFORM)" == "MIPS"
MFCDEFS=$(MFCDEFS) /D_MIPS_
CPP=cl
CFLAGS=/GX /c /W3 $(DEBUGFLAGS) $(MFCFLAGS) $(MFCDEFS)
!endif

!if "$(PLATFORM)" == "ALPHA"
MFCDEFS=$(MFCDEFS) /D_ALPHA_
CPP=cl
CFLAGS=/D_AFX_OLD_EXCEPTIONS /c /W3 $(DEBUGFLAGS) $(MFCFLAGS) $(MFCDEFS)
!endif

!if "$(PLATFORM)" == "PPC"
MFCDEFS=$(MFCDEFS) /D_PPC_
!if "$(PROCESSOR_ARCHITECTURE)" == "x86"
CPP=mcl
!else
CPP=cl
!endif
CFLAGS=/D_AFX_OLD_EXCEPTIONS /c /W3 $(DEBUGFLAGS) $(MFCFLAGS) $(MFCDEFS)
!endif

!if "$(PLATFORM)" == "MAC_68K"
MFCDEFS=$(MFCDEFS) /D_68K_
CPP=cl
CFLAGS=/GX /c /W3 /AL /Gt1 /Q68s $(DEBUGFLAGS) $(MFCFLAGS) $(MFCDEFS)
!endif

!if "$(PLATFORM)" == "MAC_PPC"
MFCDEFS=$(MFCDEFS) /D_POWERPC_ /D__pascal=
CPP=cl
CFLAGS=/D_AFX_OLD_EXCEPTIONS /c /W3 $(DEBUGFLAGS) $(MFCFLAGS) $(MFCDEFS)
!endif

!if "$(NTSDK)" == "1"
CFLAGS=$(CFLAGS) /D_NTSDK
EXTRA_LIBS=/nod:oldnames.lib $(EXTRA_LIBS) \
	gdi32.lib user32.lib kernel32.lib comdlg32.lib winspool.lib \
	advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib
!if "$(UNICODE)" == "1"
EXTRA_LIBS=mfcuiw32.lib $(EXTRA_LIBS)
!if "$(AFXDLL)" == "1"
EXTRA_LIBS=$(EXTRA_LIBS) crtdll.lib
CFLAGS=$(CFLAGS) /D_DLL
!if "$(DEBUG)" == "1"
EXTRA_LIBS=mfc30ud.lib mfco30ud.lib $(EXTRA_LIBS)
!else
EXTRA_LIBS=mfc30u.lib mfco30u.lib $(EXTRA_LIBS)
!endif
!else
!if "$(DEBUG)" == "1"
EXTRA_LIBS=uafxcwd.lib $(EXTRA_LIBS)
!else
EXTRA_LIBS=uafxcw.lib $(EXTRA_LIBS)
!endif
!endif
!endif
!if "$(UNICODE)" != "1"
EXTRA_LIBS=mfcans32.lib mfcuia32.lib $(EXTRA_LIBS)
!if "$(AFXDLL)" == "1"
EXTRA_LIBS=$(EXTRA_LIBS) crtdll.lib
!if "$(DEBUG)" == "1"
EXTRA_LIBS=mfc30d.lib mfco30d.lib mfcd30d.lib $(EXTRA_LIBS)
!else
EXTRA_LIBS=mfc30.lib mfco30.lib mfcd30.lib $(EXTRA_LIBS)
!endif
!else
!if "$(DEBUG)" == "1"
EXTRA_LIBS=nafxcwd.lib $(EXTRA_LIBS)
!else
EXTRA_LIBS=nafxcw.lib $(EXTRA_LIBS)
!endif
!endif
!endif #END lib selection logic
!if "$(PLATFORM)" == "PPC"
EXTRA_LIBS=int64.lib $(EXTRA_LIBS)
!endif
!endif

CPPMAIN_FLAGS=$(CFLAGS)

!if "$(NO_PCH)" == "1"
CPPFLAGS=$(CPPMAIN_FLAGS)
!else
CPPFLAGS=$(CPPMAIN_FLAGS) /Yustdafx.h /Fp..\$(STDAFX).pch
!endif

!if "$(COFF)" == "1"
NO_PDB=1
!if "$(CODEVIEW)" != "1"
LINKDEBUG=/incremental:no /debug:full /debugtype:coff
!else
LINKDEBUG=/incremental:no /debug:full /debugtype:both
!endif
!endif

!if "$(COFF)" != "1"
!if "$(CODEVIEW)" == "1"
LINKDEBUG=/incremental:no /debug:full /debugtype:cv
!else
LINKDEBUG=/incremental:no /debug:none
!endif
!endif

!if "$(NO_PDB)" == "1"
LINKDEBUG=$(LINKDEBUG) /pdb:none
!endif

!if "$(PLATFORM)" == "INTEL"
LINKCMD=link $(LINKDEBUG)
!endif

!if "$(PLATFORM)" == "MIPS"
LINKCMD=link $(LINKDEBUG)
!endif

!if "$(PLATFORM)" == "ALPHA"
LINKCMD=link $(LINKDEBUG)
!endif

!if "$(PLATFORM)" == "PPC"
LINKCMD=link $(LINKDEBUG)
!endif

!if "$(PLATFORM)" == "MAC_68K"
LINKCMD=link $(LINKDEBUG)
!endif

!if "$(PLATFORM)" == "MAC_PPC"
LINKCMD=link $(LINKDEBUG) /import:lib=AOCELib,weak=1
!endif

# link flags - must be specified after $(LINKCMD)
#
# conflags : creating a character based console application
# guiflags : creating a GUI based "Windows" application

!if "$(BASE)" != "M"
CONFLAGS=/subsystem:console
GUIFLAGS=/subsystem:windows
!else
!if defined(MACSIG)
GUIFLAGS=/mac:type=APPL /mac:creator=$(MACSIG)
!endif
!endif

!if "$(UNICODE)" == "1"
CONFLAGS=$(CONFLAGS) /entry:wmainCRTStartup
GUIFLAGS=$(GUIFLAGS) /entry:wWinMainCRTStartup
!endif

!if "$(BASE)" != "M"
PROJRESFILE=$(PROJ).res
!else
PROJRESFILE=$(PROJ).rsc $(MACSIG)mac.rsc
BASERESFILE=wlm.rsc commdlg.rsc
!if "$(PLATFORM)" == "MAC_PPC"
BASERESFILE=$(BASERESFILE) cfrg.rsc
!endif
!endif
RESFILE=$(PROJRESFILE) $(BASERESFILE)

.SUFFIXES:
.SUFFIXES: .c .cpp .rcm .rc

.cpp.obj:
	$(CPP) @<<
$(CPPFLAGS) $*.cpp
<<

.c.obj:
	$(CPP) @<<
$(CFLAGS) $(CVARS) $*.c
<<

!if "$(BASE)" != "M"
.rc.res:
	rc /r $(MFCDEFS) $<
!endif # BASE != M

!if "$(BASE)" == "M"
.rc.rsc:
	rc /r $(MFCDEFS) $<

.rcm.rsc:
	rc /r $(MFCDEFS) $<
!endif

#############################################################################

!if "$(NO_PCH)" == "0"
LINK_OBJS=$(OBJS) ..\$(STDAFX).obj
!else
LINK_OBJS=$(OBJS)
!endif

#
# Build CONSOLE Win32 application
#
!if "$(CONSOLE)" == "1"

!if "$(BASE)" == "M"
!error Macintosh targets do not support console applications
!endif

$(PROJ).exe: $(LINK_OBJS)
	$(LINKCMD) @<<
$(CONFLAGS) /out:$(PROJ).exe
$(LINK_OBJS) $(EXTRA_LIBS)
<<

!endif  # CONSOLE=1

#
# Build Win32 application
#
!if "$(CONSOLE)" == "0"

!if "$(PLATFORM)" == "MAC_68K"
copy: $(PROJ).exe
!if defined(MACNAME) && defined(MACPROJ)
	mfile copy $(PROJ).exe ":$(MACNAME):$(MACPROJ)"
!endif
!endif

!if "$(PLATFORM)" == "MAC_PPC"
copy: $(PROJ).exe
!if defined(PPCMAC) && defined(PPCNAME) && defined(MACPROJ)
	mfile copy /n "$(PPCMAC)" $(PROJ).exe ":$(PPCNAME):$(MACPROJ)"
!endif
!endif

!if "$(BASE)" == "M"
$(MACSIG)mac.rsc: $(MACSIG)mac.r
	mrc $(MFCDEFS) /o $(MACSIG)mac.rsc $(MACSIG)mac.r
!endif

!if "$(SIMPLE_APP)" != "1"
$(PROJ).exe: $(LINK_OBJS) $(PROJRESFILE)
	$(LINKCMD) @<<
$(GUIFLAGS) /out:$(PROJ).exe
$(LINK_OBJS) $(RESFILE) $(EXTRA_LIBS)
<<

$(PROJ).res:  resource.h
$(PROJ).rsc:  resource.h
!endif

!if "$(SIMPLE_APP)" == "1"
$(PROJ).exe: $(LINK_OBJS)
	$(LINKCMD) @<<
$(GUIFLAGS) /out:$(PROJ).exe
$(LINK_OBJS) $(EXTRA_LIBS)
<<

!endif

!if "$(NO_PCH)" == "0"
..\$(STDAFX).obj ..\$(STDAFX).pch: stdafx.h stdafx.cpp
	echo "BUILDING SHARED PCH and PCT files"
	$(CPP) @<<
$(CPPMAIN_FLAGS) /Ycstdafx.h /Fp..\$(STDAFX).pch /Fo..\$(STDAFX).obj /c $(ROOT)\stdafx.cpp
<<

$(OBJS): ..\$(STDAFX).pch
!endif

!endif  # CONSOLE=0

clean::
	if exist $(PROJ).exe erase $(PROJ).exe
	if exist *.aps erase *.aps
	if exist *.pch erase *.pch
	if exist *.map erase *.map
	if exist *.obj erase *.obj
	if exist *.pdb erase *.pdb
	if exist *.map erase *.map
	if exist *.lib erase *.lib
	if exist *.res erase *.res
	if exist *.rsc erase *.rsc
	if exist *.pef erase *.pef

#############################################################################
