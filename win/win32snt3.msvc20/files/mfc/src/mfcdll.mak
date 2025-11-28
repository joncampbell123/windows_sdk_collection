# This is a part of the Microsoft Foundation Classes C++ library.
# Copyright (C) 1992 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Microsoft Foundation Classes Reference and Microsoft
# QuickHelp documentation provided with the library.
# See these sources for detailed information regarding the
# Microsoft Foundation Classes product.
#

# MFC30[D].DLL is a DLL
#  which exports all the MFC classes
#
# If you need a private build of the MFC DLL, be sure to rename
#  "MFC30.DLL" to something more appropriate for you application.
# Please do not re-distribute a privately built version with the
#  name "MFC30.DLL".
#
# Use nmake /f mfcdll.mak LIBNAME=<your name> to do this.
#
# Note: LIBNAME must be 6 characters or less.

!ifndef LIBNAME
!error LIBNAME is not defined. LIBNAME=MFC30 builds the prebuilt DLL.
!endif

TARGET=w
DLL=2
TARG=$(LIBNAME)
TARGDEFS=/D_AFX_CORE_IMPL
LFLAGS=/nodefaultlib

!if "$(UNICODE)" == "1"
TARG=$(TARG)U
!endif

!if "$(DEBUG)" != "0"
# Debug DLL build
TARG=$(TARG)D
RCDEFINES=/D_DEBUG
LFLAGS=$(LFLAGS)
!ELSE
# Release DLL build
RCDEFINES=
LFLAGS=$(LFLAGS)
!ENDIF

DEFFILE=$(PLATFORM)\$(TARG).DEF

!if "$(CODEVIEW)" != "0"
LFLAGS=$(LFLAGS) /debug:full /debugtype:cv
!if "$(NO_PDB)" != "1" && "$(REGEN)" != "1"
LFLAGS=$(LFLAGS) /pdb:$(TARG).pdb
!else
LFLAGS=$(LFLAGS) /pdb:none
!endif
!else
LFLAGS=$(LFLAGS) /debug:none /incremental:no
!endif

!ifdef RELEASE # Release VERSION info
RCDEFINES=$(RCDEFINES) /DRELEASE
LFLAGS=$(LFLAGS) /release
!endif

LFLAGS=$(LFLAGS) /dll /subsystem:windows /version:3.0 /base:0x5F800000

#############################################################################
## function ordering

!if "$(ORDER)" == "1" && "$(DEBUG)" != "1" && "$(UNICODE)" != "1"
!if "$(PLATFORM)" == "INTEL"
DEFS=$(DEFS) /D_AFX_FUNCTION_ORDER
LFLAGS=$(LFLAGS) /order:@intel\mfc30.prf
!endif
!if "$(PLATFORM)" == "MIPS"
DEFS=$(DEFS) /D_AFX_FUNCTION_ORDER
LFLAGS=$(LFLAGS) /order:@mips\mfc30.prf
!endif
!endif

LIBS=msvcrt.lib kernel32.lib gdi32.lib user32.lib shell32.lib \
	comdlg32.lib advapi32.lib

dll_goal: create2.dir $(TARG).dll ..\lib\$(TARG).lib

#############################################################################
# import most rules and library files from normal makefile

!include makefile

create2.dir:
	@-if not exist $D\*.* mkdir $D

#############################################################################
# Build target

$D\$(TARG).res: mfcdll.rc
	rc /r $(RCDEFINES) /fo $D\$(TARG).res mfcdll.rc

DLL_OBJS=$(OBJECT) $(OBJDIAG) $(INLINES) $(FILES) $(COLL1) $(COLL2) $(MISC) \
	$(WINDOWS) $(DIALOG) $(WINMISC) $(DOCVIEW) $(APPLICATION) $(OLEREQ) \
	$D\dllinit.obj

$(TARG).dll ..\lib\$(TARG).lib: $(DLL_OBJS) $(DEFFILE) $D\$(TARG).res
	$(LINK32) @<<
$(LFLAGS)
$(LIBS)
$(DLL_OBJS)
$D\$(TARG).res
/def:$(DEFFILE)
/out:$(TARG).DLL
/map:$D\$(TARG).MAP
/implib:..\lib\$(TARG).LIB
<<
	if exist ..\lib\$(TARG).exp del ..\lib\$(TARG).exp

#############################################################################
