#
# MAKEFILE
# Gizmobar Demo
#
# Copyright (c)1992-1995 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

DEFFILE = gbdemo.def

TARGET  = gbdemo
OUTFILE = gbdemo.exe

all:$(TARGET).exe

INCLS    = $(TARGET).h

OBJS1    = $(TARGET).obj init.obj $(TARGET).res
OBJS2    = ""
OBJS3    = ""
OBJS4    = ""
OBJS5    = ""
OBJS6    = ""
OBJS     = $(OBJS1)

RCFILES  = $(TARGET).ico image72.bmp image96.bmp image120.bmp

LIBS    = bttncur.lib gizmobar.lib

OLE_FLAGS = -DWIN32S -I..\inc -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####

$(TARGET).obj : $(TARGET).c  $(INCLS)
init.obj      : init.c       $(INCLS)
$(TARGET).res : $(TARGET).rc $(INCLS) $(RCFILES)

#
# Tool Directives
#
.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $*.c

.rc.res:
    rc -r -DWIN32 -DDEBUG -I..\inc $*.rc

clean:
    -del *.obj
    -del *.res
    -del *.exe

$(TARGET).exe: $(OBJS)
    $(link) $(linkdebug) $(guilflags) $(OBJS) -out:$@ $(guilibs) $(LIBS)
    if not exist $(MSTOOLS)\samples\ole\bin mkdir $(MSTOOLS)\samples\ole\bin
    copy $@ $(MSTOOLS)\samples\ole\bin

