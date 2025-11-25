!IF 0

Copyright (c) 1994  Microsoft Corporation

Module Name:

ppcmk.inc

Abstract:

This module contains the PPC specific build controls.  It is included
by makefile.def.

!ENDIF


#
# PPC option control
#

UMBASE=$(UMBASE:*=ppc)
UMLIBS=$(UMLIBS:*=ppc)
NTRES=$(NTRES:*=ppc)
UMRES=$(UMRES:*=ppc)
UMOBJS=$(UMOBJS:*=ppc)
LINKLIBS=$(LINKLIBS:*=ppc)
DLLBASE=$(DLLBASE:*=ppc)
DLLDEF=$(DLLDEF:*=ppc)
MACHINE_TARGETLIBS=$(MACHINE_TARGETLIBS:*=ppc)
TARGET=$(TARGET:*=ppc)
DYNLINK_LIB=$(DYNLINK_LIB:*=ppc)
TARGETEXEFILES=$(TARGETEXEFILES:*=ppc)
TARGETLIBFILES=$(TARGETLIBFILES:*=ppc)
TARGETOBJFILES=$(TARGETOBJFILES:*=ppc)
UMOBJFILES=$(UMOBJFILES:*=ppc)
UMEXEFILES=$(UMEXEFILES:*=ppc)
HEADERFILE=$(HEADERFILE:*=ppc)
HEADEROBJNAME=$(HEADEROBJNAME:*=ppc)
HEADEROBJ=$(HEADEROBJ:*=ppc)
PRECOMPILED=$(PRECOMPILED:*=ppc)
PRECOMPILED_CXX=$(PRECOMPILED_CXX:*=ppc)
PRECOMPILED_TARGET=$(PRECOMPILED_TARGET:*=ppc)

#
# Include 64-bit helper library
#
LINKLIBS=$(LINKLIBS) $(BASEDIR)\lib\ppc\$(DDKBUILDENV)\int64.lib
!IF "$(TARGETTYPE)"=="DRIVER" || \
    "$(TARGETTYPE)"=="GDI_DRIVER" || \
    "$(TARGETTYPE)"=="MINIPORT"
#
#Drivers don't link with link libs.
#
MACHINE_TARGETLIBS=$(MACHINE_TARGETLIBS) $(BASEDIR)\lib\ppc\$(DDKBUILDENV)\int64.lib
!ENDIF

!ifdef NTTARGETFILES
NTTARGETFILES=$(NTTARGETFILES:*=ppc)
!endif
!ifdef NTTARGETFILE0
NTTARGETFILE0=$(NTTARGETFILE0:*=ppc)
!endif
!ifdef NTTARGETFILE1
NTTARGETFILE1=$(NTTARGETFILE1:*=ppc)
!endif

!IF "$(GPSIZE)" != "0"
#
# GPSIZE is irrelevant on PowerPC
#
LINKGPSIZE=
!ENDIF

ENTRY_SUFFIX=
GDI_ENTRY_SUFFIX=

!IF "$(MSC_OPTFLAGS)" == "/Oxs"
DBGFLAGS=$(MSC_OPTFLAGS:/Oxs=/Ox)
!ENDIF

!IFDEF PPC_WARNING_LEVEL
MSC_WARNING_LEVEL=$(PPC_WARNING_LEVEL)
!ENDIF

!IFDEF PPC_OPTIMIZATION
MSC_OPTIMIZATION=$(PPC_OPTIMIZATION)
!ELSE
MSC_OPTIMIZATION=
!ENDIF

!IFDEF PPC_CPPFLAGS
MSC_CPPFLAGS=$(PPC_CPPFLAGS)
!ENDIF

PPC_AS=pas
PPC_DIS=pdis
PPC_AS_WARNINGS=

PPC_START_FILE=start.obj

ENV_DEFINES=$(LIBC_DEFINES) $(C_DEFINES) $(NET_C_DEFINES) $(MSC_CPPFLAGS) $(NTCPPFLAGS) $(PM_DEFINES)
STD_DEFINES=-DPPC=1 -D_PPC_=1 -DNO_EXT_KEYS $(STD_DEFINES) -D_M_PPC=1

!ifdef USING_VC2X
VC2_CRTAPI_DEFINES=-D_CRTAPI1=__cdecl -D_CRTAPI2=__cdecl \
                   -Ditoa=_itoa \
                   -Dstrcmpi=_strcmpi \
                   -Dstricmp=_stricmp \
                   -Dwcsicmp=_wcsicmp \
                   -Dwcsnicmp=_wcsnicmp

VC2_SEH_DEFINES=-Dtry=__try -Dleave=__leave -Dexcept=__except -Dfinally=__finally
!endif

STDFLAGS=-c -Zel -Zp8 -Gy $(MFC_FLAGS)

PPC_C_COMPILER_NAME = cl $(CBSTRING) -nologo
PPC_CC = $(PPC_C_COMPILER_NAME)

!ifdef USING_VC2X
CDEFINES=$(STD_DEFINES) $(VC2_SEH_DEFINES) $(VC2_CRTAPI_DEFINES) $(TARGET_DBG_DEFINES) $(ENV_DEFINES)
!else
CDEFINES=$(STD_DEFINES) $(TARGET_DBG_DEFINES) $(ENV_DEFINES)
!endif

CFLAGS=$(PPC_FLAGS) $(NTPPCFLAGS) $(STDFLAGS) $(DBGFLAGS) $(USER_C_FLAGS)

PPC_CDEFINES= $(CDEFINES)
PPC_CFLAGS  = $(CFLAGS)  -ZB64 $(MSC_WARNING_LEVEL) # -DALLOC_TEXT

!IFNDEF PPC_OPTIONS
PPC_OPTIONS=
!ENDIF

C_COMPILER_NAME      = $(PPC_C_COMPILER_NAME)
C_PREPROCESSOR_NAME  = $(PPC_C_COMPILER_NAME)
CXX_COMPILER_NAME    = $(PPC_C_COMPILER_NAME)

GLOBAL_C_FLAGS       = -nologo -Ippc\ -I. $(INCPATH0) $(CDEFINES) $(PPC_OPTIONS) \
                        $(PPC_CFLAGS) -D__stdcall= -D__cdecl= -D_cdecl= -Dcdecl= \
                        -DFPO=1 -DLANGUAGE_C

# Define a default WINVER value different from SDK winuser.h, but allow
#  an app to overrule this by specifying WINVER in C_DEFINES in sources file.
GLOBAL_C_FLAGS= -DWINVER=0x030A $(GLOBAL_C_FLAGS)


# Disable -WX for now

GLOBAL_C_FLAGS = $(GLOBAL_C_FLAGS:-WX=)
GLOBAL_C_FLAGS = $(GLOBAL_C_FLAGS:/WX=)

C_PREPROCESSOR_FLAGS = $(GLOBAL_C_FLAGS) $(PRECOMPILED) -EP -Tc
C_COMPILER_FLAGS     = $(GLOBAL_C_FLAGS) $(PRECOMPILED) $(COMPILER_WARNINGS)
CXX_COMPILER_FLAGS   = $(GLOBAL_C_FLAGS) $(PRECOMPILED_CXX) $(COMPILER_WARNINGS)

C_PREPROCESSOR       = $(C_PREPROCESSOR_NAME) $(C_PREPROCESSOR_FLAGS)
C_COMPILER           = $(C_COMPILER_NAME) $(C_COMPILER_FLAGS)
CXX_COMPILER         = $(CXX_COMPILER_NAME) $(CXX_COMPILER_FLAGS)

PPC_ASSEMBLER        = $(PPC_AS) $(PPC_AS_WARNINGS)

ECHO_MSG             = ClPpc $< " $(C_COMPILER) "
ECHO_PRECOMPILED_MSG = CpPpc $(PRECOMPILED_INCLUDE) " $(C_COMPILER) \
                        /Yl$(TARGETNAME) /Yc$(?F) $(HEADERFILE) \
                        $(HEADEROBJ) $(PRECOMPILED_FLAG)"

{.\}.cxx{}.obj:
    @$(ECHO_MSG)
    @$(CXX_COMPILER) -Fo$(MAKEDIR)\$@ $(MAKEDIR)\..\$(<F)

{.\}.cxx{obj\$(TARGET_DIRECTORY)\}.obj:
    @$(ECHO_MSG)
    @$(CXX_COMPILER) -Fo$(MAKEDIR)\$@ $(MAKEDIR)\..\$(<F)

{}.cxx{obj\$(TARGET_DIRECTORY)\}.obj:
    @$(ECHO_MSG)
    @$(CXX_COMPILER) -Fo$(MAKEDIR)\$@ $(MAKEDIR)\..\$(<F)

{..\ppc\}.s{obj\ppc\}.obj:
    @-erase $@ >nul 2>&1
    @echo AsPpc $<            " $(PPC_ASSEMBLER) "
    $(C_PREPROCESSOR) $(MAKEDIR)\..\ppc\$(<F) > $(MAKEDIR)\obj\ppc\$(<B).i && $(PPC_ASSEMBLER) -o $@ $(MAKEDIR)\obj\ppc\$(<B).i
    @-erase $(MAKEDIR)\obj\ppc\$(<B).i >nul 2>&1


{ppc\}.s{obj\ppc\}.obj:
    @-erase $@ >nul 2>&1
    @echo AsPpc $<            " $(PPC_ASSEMBLER) "
    $(C_PREPROCESSOR) $(MAKEDIR)\ppc\$(<F) > $(MAKEDIR)\obj\ppc\$(<B).i && $(PPC_ASSEMBLER) -o $@ $(MAKEDIR)\obj\ppc\$(<B).i
    @-erase $(MAKEDIR)\obj\ppc\$(<B).i >nul 2>&1

.SUFFIXES: .il

{}.il{}.cod:
    @-erase $@ >nul 2>&1
    @echo CL $<" $(C_COMPILER) "
    @$(C_COMPILER) -FAac -Fa$(<B).cod -B1..\bootbin\null.exe -Bk$(<B). -Fo$(MAKEDIR)\obj\ppc\$(<B).o $(MAKEDIR)\$(<B).c
    @del obj\ppc\$(<B).o

#
# No reodering under ppc.
#

LIBRARY_OBJS=$(LINKLIBS) $(OBJECTS)

!IFDEF _NTPPCTREE
_NTTREE=$(_NTPPCTREE)
!ENDIF

!IFDEF _NTPPCLIBS
_NTLIBS=$(_NTPPCLIBS)
!ENDIF

COPYDST=$(PPCCOPYDST)

LIB_COPY=ntppccp.cmd

NTTEST_LINK_OPTIONS= -base:0x80100000 -entry:KiSystemStartup
