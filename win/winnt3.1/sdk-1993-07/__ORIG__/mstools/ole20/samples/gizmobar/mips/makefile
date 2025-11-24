#
# MAKEFILE
# GizmoBar Version 1.00, March 1993
#
# Copyright (c)1992 Microsoft Corporation, All Rights Reserved
#
# Kraig Brockschmidt, Software Design Engineer
# Microsoft Systems Developer Relations
#
# Internet  :  kraigb@microsoft.com
# Compuserve:  >INTERNET:kraigb@microsoft.com
#

APP=gizmobar


!ifndef DEVROOT_DIR
!error You must define DEVROOT_DIR (ole20 sub-tree)
!endif
!ifndef INCLUDE
!error You must define INCLUDE (non ole20 include files)
!endif
!ifndef LIB
!error You must deinfe LIB (non ole20 libraries)
!endif
!ifndef OLEBUILD
OLEBUILD=NT
!endif

OLE2_H=$(DEVROOT_DIR)\h
OLE2_LIB=$(DEVROOT_DIR)\lib

!ifndef SAMPLE_DIR
SAMPLE_DIR = $(DEVROOT_DIR)\samples
!endif
SRC_DIR   = $(SAMPLE_DIR)\$(APP)


#Add '#' to next line for "noisy" operation
#!CMDSWITCHES +s


#
#Compiler and assembler flags
#Use "SET RETAIL=1" from MS-DOS to compile non-debug version.
#
#-GD -GEa
!if "$(OLEBUILD)" == "NT"

!if "$(MACHINE)" == ""
MACHINE   = MIPS
CPP       = mcl
MACHINE_D = _MIPS_
!endif

IMPLIB    = lib32 -out:$*.l32 -def:$*.d32 -machine:$(MACHINE)

CFLAGS   = /c /Fc /W3 /nologo $(CL)
LINKFLAGS =  -subsystem:windows -machine:$(MACHINE) -debug:mapped,full -debugtype:both
LIBS1     = kernel32.lib user32.lib gdi32.lib
LIBS      = $(LIBS1) crtdll.lib bttncur.lib
DEFS      = /DWIN32 /D_DEBUG /D$(MACHINE_D) /D$(MACHINE)=1 /D_NTWIN /D_WINDOWS /DWINVER=0x030A 

ECHOERR   = echo

!else

CPP     = cl
!ifndef RETAIL
CFLAGS  = -c -Od -AS -Zipe -G2sw -W3
LINK    = /al:16/ONERROR:NOEXE/CO
DEFS    = -DDEBUG
LIBS    = libw sdllcew bttncur
!else
CFLAGS  = -c -Oat -AS -Zpe -G2sw -W3
LINK    = /al:16/ONERROR:NOEXE
DEFS    =
LIBS    = libw sdllcew bttncur
!endif

!endif


.SUFFIXES: .h .c .obj .exe .dll .cpp .res .rc

!if "$(OLEBUILD)" == "NT"

goal:  setflags $(APP).dll
setflags:
        cd $(SRC_DIR)
	set INCLUDE=$(OLE2_H);$(INCLUDE)
	set LIB=$(OLE2_LIB);$(LIB)
!else

goal:  $(APP).lib

!endif

INCLS    = $(APP).h gizmoint.h gizmo.h win1632.h

OBJS1    = $(APP).obj init.obj
OBJS2    = api.obj paint.obj gizmo.obj
OBJS     = $(OBJS1) $(OBJS2)


RCFILES1 = $(APP).rcv
RCFILES2 =
RCFILES  = $(RCFILES1) $(RCFILES2)


#
# Tool Directives
#
.c.obj:
    echo +++++++++
    echo COMPILING $*.c
    $(CPP) $(CFLAGS) $(DEFS) $*.c


.rc.res:
    echo +++++++++
    echo Compiling Resources
    rc -r $(DEFS) $*.rc


clean:
    -del *.obj
    -del *.dll
    -del *.res
    -del *.lib
    -del err

!if "$(OLEBUILD)" == "NT"

$(APP).dll: $(OBJS) $(APP).lib $(APP).d32 $(APP).res 
    @$(ECHOERR) Linking $@...
    cvtres -$(MACHINE) $*.res -o $*.rs
    link32 $(LINKFLAGS) -entry:LibMain $(APP).exp $(OBJS) $*.rs -out:$@ -map:$*.map -dll $(LIBS)

$(APP).lib: $(OBJS) $(APP).d32 
    @$(ECHOERR) Creating $@...
    $(IMPLIB) $(OBJS)
    lib32 -out:$(APP).lib $(APP).l32

!else

#This rule builds a linker response file on the fly depending on debug flags
$(APP).dll : $(OBJS) $(APP).res $(APP).def
    echo ++++++++++
    echo Linking $@
    echo libentry.obj +                          > $(APP).lrf
    echo $(OBJS)                                >> $(APP).lrf

    echo $(APP).dll $(LINK)                  >> $(APP).lrf
    echo nul/li/                                >> $(APP).lrf
    echo $(LIBS)/NOD/NOE                        >> $(APP).lrf
    echo $(APP).def                          >> $(APP).lrf

    link @$(APP).lrf
    rc -v $(APP).res $(APP).dll
    del $(APP).lrf



$(APP).lib: $(APP).dll
    implib $(APP).lib $(APP).def

!endif

##### Dependencies #####
$(APP).obj  : $(APP).c    $(INCLS)
paint.obj      : paint.c        $(INCLS)
init.obj       : init.c         $(INCLS)
api.obj        : api.c          $(INCLS)
gizmo.obj      : gizmo.c        $(INCLS)


$(APP).res  : $(APP).rc   $(INCLS) $(RCFILES)

