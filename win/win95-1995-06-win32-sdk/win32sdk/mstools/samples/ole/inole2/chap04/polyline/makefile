#
# MAKEFILE
# Koala Object DLL Chapter 4
#
# Copyright (c)1992-1995 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

DEFFILE = polyline.def

TARGET  = polyline
OUTFILE = poly04.dll

all: poly04.dll

INCLS    = $(TARGET).H

OBJS1    = $(TARGET).obj precomp.obj dllpoly.obj
OBJS2    = ipolylin.obj polywin.obj
OBJS3    = ""
OBJS4    = ""
OBJS5    = ""
OBJS6    = ""
OBJS     = $(OBJS1) $(OBJS2) $(TARGET).res

RCFILES  =

LIBS    = bttncur.lib gizmobar.lib ..\..\lib\stastrip.lib ..\..\lib\classmdi.lib olestd.lib oledlg.lib

OLE_FLAGS = -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####
dllpoly.obj   : dllpoly.cpp   $(INCLS)
$(TARGET).obj : $(TARGET).cpp $(INCLS)
ipolylin.obj  : ipolylin.cpp  $(INCLS)
polywin.obj   : polywin.cpp   $(INCLS)
$(TARGET).res : $(TARGET).rc  $(RCFILES)

#
# Tool Directives
#
.cpp.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $*.cpp

.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $*.c

.rc.res:
    rc -r -DWIN32 -DDEBUG -I..\..\inc $*.rc

clean:
    -del *.obj
    -del *.res
    -del *.exe

$(OUTFILE): $(OBJS) $(TARGET).res $(TARGET).def $(TARGET).lib
    $(link) $(linkdebug) $(dlllflags) $(TARGET).exp $(OBJS) -out:$@ -map:$*.map $(olelibsdll) -base:0x77000000 $(LIBS)
    if not exist $(MSTOOLS)\samples\ole\bin mkdir $(MSTOOLS)\samples\ole\bin
    copy $@ $(MSTOOLS)\samples\ole\bin

$(TARGET).lib: $(OBJS) $(TARGET).def
    $(implib) $(OBJS) -out:$(TARGET).lib -def:$(TARGET).def -machine:$(CPU)

