#
# MAKEFILE
# Koala Object DLL Chapter 4
#
# Copyright (c)1992-1994 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(SDKROOT)\samples\ole\inole2\inc

!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

DEFFILE = dkoala.def

TARGET  = dkoala
OUTFILE = dkoala.dll

all: dkoala.dll

INCLS    = $(TARGET).H koala.h

OBJS1    = $(TARGET).obj koala.obj
OBJS2    = ""
OBJS3    = ""
OBJS4    = ""
OBJS5    = ""
OBJS6    = ""
OBJS     = $(OBJS1) $(TARGET).res

RCFILES  =

LIBS    = bttncur.lib gizmobar.lib ..\..\lib\stastrip.lib ..\..\lib\classmdi.lib ..\..\lib\bookui32.lib

OLE_FLAGS = -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####
$(TARGET).res : $(TARGET).rc $(INCLS) $(RCFILES)

#Application level things
$(TARGET).obj : $(TARGET).cpp $(INCLS)
koala.obj    : koala.cpp    $(INCLS)

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
    $(link) $(linkdebug) $(dlllflags) $(TARGET).exp $(OBJS) $(TARGET).res -out:$@ -map:$*.map $(olelibsdll) -base:0x77000000 $(LIBS)

$(TARGET).lib: $(OBJS) $(TARGET).def
    $(implib) $(OBJS) -out:$(TARGET).lib -def:$(TARGET).def -machine:$(CPU)

