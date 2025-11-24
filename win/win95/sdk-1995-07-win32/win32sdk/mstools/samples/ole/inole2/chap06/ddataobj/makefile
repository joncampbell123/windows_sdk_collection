#
# MAKEFILE
# Data Object DLL Chapter 6
#
# Copyright (c)1992-1995 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

DEFFILE = ddataobj.def

TARGET  = ddataobj
OUTFILE = ddataobj.dll

all: ddataobj.dll

INCLS    = $(TARGET).H dataobj.h

OBJS1    = $(TARGET).obj dataobj.obj precomp.obj
OBJS2    = idataobj.obj ienumfe.obj render.obj
OBJS3    = ""
OBJS4    = ""
OBJS5    = ""
OBJS6    = ""
OBJS     = $(OBJS1) $(OBJS2) $(TARGET).res

RCFILES  = ..\res\16.bmp ..\res\64.bmp ..\res\256.bmp advisor.ico

LIBS    = bttncur.lib gizmobar.lib ..\..\lib\stastrip.lib ..\..\lib\classmdi.lib olestd.lib oledlg.lib

OLE_FLAGS = -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####
$(TARGET).res : $(TARGET).rc $(INCLS) $(RCFILES)

#Application level things
$(TARGET).obj : $(TARGET).cpp $(INCLS)
dataobj.obj    : dataobj.cpp    $(INCLS)
idataobj.obj    : idataobj.cpp    $(INCLS)
ienumfe.obj    : ienumfe.cpp    $(INCLS)
render.obj    : render.cpp    $(INCLS)

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

