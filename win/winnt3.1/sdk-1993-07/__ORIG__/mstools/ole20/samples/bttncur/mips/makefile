#
# Makefile
# Standard Button Images and Cursors DLL Version 1.1, March 1993
#
# Copyright (c)1992-1993 Microsoft Corporation, All Rights Reserved,
# as applied to redistribution of this source code in source form
# License is granted to use of compiled code in shipped binaries.
#
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
SRC_DIR   = $(SAMPLE_DIR)\bttncur

!CMDSWITCHES +s

#
#Compiler and assembler flags
#Use "SET RETAIL=1" from MS-DOS to compile non-debug version.
#

!if "$(OLEBUILD)" == "NT"

MACHINE   = MIPS
CPP       = mcl
IMPLIB    = lib32 -out:$*.l32 -def:$*.d32 -machine:$(MACHINE)

CLFLAGS   = /c /Fc /D_MIPS_ /D_DEBUG /D_DEBUGTRACE=0 /DWIN32 /D$(MACHINE)=1 /D_NTWIN /D_WINDOWS /DWINVER=0x030A /W3 /nologo $(CL)
LINKFLAGS = -subsystem:windows -machine:$(MACHINE) -debug:mapped,full -debugtype:both
LIBS      = kernel32.lib user32.lib gdi32.lib crtdll.lib
DEFS      = -DWIN32 -DDEBUG

ECHOERR   = echo

!else

CPP     = cl
IMPLIB = implib $@ $*.def

!ifndef RETAIL
CLFLAGS = -c -Od -AS -Zipe -G2sw -W3
LINK    = /al:16/ONERROR:NOEXE/CO
DEFS    = -DDEBUG
!else
CLFLAGS = -c -Oat -AS -Zpe -G2sw -W3
LINK    = /al:16/ONERROR:NOEXE
DEFS    =
!endif

!endif

.SUFFIXES: .h .c .obj .exe .res .rc

#
#


!if "$(OLEBUILD)" == "NT"

goal:  setflags bttncur.dll
setflags:
        cd $(SRC_DIR)
	set INCLUDE=$(OLE2_H);$(INCLUDE)
	set LIB=$(OLE2_LIB);$(LIB)

!else

goal: bttncur.lib

!endif




INCLS    = bttncur.h bttncuri.h
OBJS     = bttncur.obj cursors.obj

RCFILES1 = bttncur.rcv res\harrows.cur res\help.cur res\larrows.cur
RCFILES2 = res\magnify.cur res\neswarrs.cur res\nodrop.cur res\nwsearrs.cur
RCFILES3 = res\rarrow.cur res\sarrows.cur res\sizebarh.cur res\sizebarv.cur
RCFILES4 = res\splith.cur res\splitv.cur res\tabletop.cur res\varrows.cur
RCFILES5 = res\stdim72.bmp res\stdim96.bmp res\stdim120.bmp
RCFILES  = $(RCFILES1) $(RCFILES2) $(RCFILES3) $(RCFILES4) $(RCFILES5)


#
# Tool Directives
#
.c.obj:
    echo +++++++++
    echo COMPILING $*.c
    $(CPP) $(CLFLAGS) $(DEFS) $*.c

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

#This rule builds a linker response file on the fly depending on debug flags

!if "$(OLEBUILD)" == "NT"

bttncur.dll: $(OBJS) bttncur.lib bttncur.d32 bttncur.res 
    @$(ECHOERR) Linking $@...
    cvtres -$(MACHINE) $*.res -o $*.rs
    link32 $(LINKFLAGS) -entry:LibMain bttncur.exp $(OBJS) $*.rs -out:$@ -map:$*.map -dll $(LIBS) -base:0x76000000 -fixed

bttncur.lib: $(OBJS) bttncur.d32 
    @$(ECHOERR) Creating $@...
    $(IMPLIB) $(OBJS)
    lib32 -out:bttncur.lib bttncur.l32

!else

bttncur.dll : $(OBJS) bttncur.res bttncur.def
    echo ++++++++++
    echo Linking $@
    echo libentry.obj +                        > bttncur.lrf
    echo $(OBJS)                              >> bttncur.lrf

    echo bttncur.dll $(LINK)                  >> bttncur.lrf
    echo nul/li                               >> bttncur.lrf
    echo libw sdllcew/NOD/NOE                 >> bttncur.lrf
    echo bttncur.def                          >> bttncur.lrf

    link @bttncur.lrf
    rc -v -30 bttncur.res bttncur.dll
    del bttncur.lrf

bttncur.lib: bttncur.dll
    $(IMPLIB)

!endif



##### Dependencies #####
bttncur.obj  : bttncur.c    $(INCLS)
cursors.obj  : cursors.c    $(INCLS)
bttncur.res  : bttncur.rc   $(INCLS) $(RCFILES)

