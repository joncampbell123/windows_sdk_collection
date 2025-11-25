#
# MAKEFILE
# Polyline Component Object DLL Chapter 16
#
# Copyright (c)1992-1996 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

DEFFILE = polyline.def

TARGET  = polyline
OUTFILE = poly16.dll

all: poly16.dll

INCLS    = $(TARGET).H resource.h

OBJS1    = $(TARGET).obj precomp.obj dllpoly.obj
OBJS2    = ipolylin.obj polywin.obj iperstor.obj
OBJS3    = idataobj.obj
OBJS4    = ioleobj.obj iviewobj.obj irunobj.obj
OBJS5    = iextconn.obj
OBJS6    = iipobj.obj iipaobj.obj
OBJS     = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6) $(TARGET).res

RCFILES  =

LIBS    = bttncur.lib gizmobar.lib ..\..\lib\stastrip.lib ..\..\lib\classmdi.lib oledlg.lib

OLE_FLAGS = -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####
dllpoly.obj   : dllpoly.cpp   $(INCLS)
$(TARGET).obj : $(TARGET).cpp $(INCLS)
ipolylin.obj  : ipolylin.cpp  $(INCLS)
polywin.obj   : polywin.cpp   $(INCLS)
iperstor.obj  : iperstor.cpp  $(INCLS)
idataobj.obj  : idataobj.cpp  $(INCLS)
ioleobj.obj   : ioleobj.cpp   $(INCLS)
iviewobj.obj  : iviewobj.cpp  $(INCLS)
irunobj.obj   : irunobj.cpp   $(INCLS)
iipobj.obj    : iipobj.cpp    $(INCLS)
iipaobj.obj   : iipaobj.cpp   $(INCLS)
iextconn.obj  : iextconn.cpp  $(INCLS)
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

