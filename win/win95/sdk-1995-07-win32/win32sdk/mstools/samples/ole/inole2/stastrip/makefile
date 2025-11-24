#
# Makefile
# Standard Button Images and Cursors DLL Version 1.1, March 1993
#
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

all: stastrip.dll demo

OLE_FLAGS = -DWIN32S -I..\inc -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif

.SUFFIXES: .h .c .obj .exe .res .rc

#
#


INCLS    = stastrip.h statint.h
OBJS     = init.obj stastrip.obj

RCFILES = stastrip.rcv


#
# Tool Directives
#
.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $*.c

.rc.res:
    rc -r -DWIN32 -DDEBUG -I..\inc $*.rc

clean:
    -del *.obj
    -del *.dll
    -del *.res
    -del *.lib
    -del *.exp
    -del *.map

stastrip.dll: $(OBJS) stastrip.lib stastrip.def stastrip.res
    $(link) $(linkdebug) $(dlllflags) stastrip.exp $(OBJS) $*.res -out:$@ -map:$*.map $(guilibsdll) -base:0x77000000 bttncur.lib gizmobar.lib
    if not exist $(MSTOOLS)\samples\ole\bin mkdir $(MSTOOLS)\samples\ole\bin
    copy $@ $(MSTOOLS)\samples\ole\bin

stastrip.lib: $(OBJS) stastrip.def
    $(implib) $(OBJS) -out:stastrip.lib -def:stastrip.def -machine:$(CPU)
    if not exist ..\lib mkdir ..\lib
    copy stastrip.lib ..\lib

##### Dependencies #####
stastrip.obj : stastrip.c    $(INCLS)
init.obj     : init.c    $(INCLS)
stastrip.res : stastrip.rc   $(INCLS) $(RCFILES)

demo:
    cd demo
    @echo ole\stastrip\demo >> $(MSTOOLS)\samples\sdemo.tmp
    @nmake -a -i -nologo >$(MSTOOLS)\samples\sdemo.tmp
    cd ..
