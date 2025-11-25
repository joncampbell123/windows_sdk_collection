#
# MAKEFILE
# CLASSLIB Skeleton Chapter 2
#
# Copyright (c)1992-1996 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

DEFFILE = skel.def

TARGET  = skel
OUTFILE = skel.exe

all:$(TARGET).exe

INCLS    =

OBJS1    = $(TARGET).obj
OBJS2    = ""
OBJS3    = ""
OBJS4    = ""
OBJS5    = ""
OBJS6    = ""
OBJS     = $(OBJS1) $(TARGET).res

RCFILES1 = app.ico document.ico about.dlg
RCFILES2 = stdgz72.bmp stdgz96.bmp stdgz120.bmp
RCFILES  = $(RCFILES1) $(RCFILES2)

LIBS    = bttncur.lib gizmobar.lib ..\..\lib\stastrip.lib ..\..\lib\classmdi.lib

OLE_FLAGS = -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####

$(TARGET).obj : $(TARGET).cpp  $(INCLS)
$(TARGET).res : $(TARGET).rc $(INCLS) $(RCFILES)

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

$(TARGET).exe: $(OBJS)
    $(link) $(linkdebug) $(guilflags) $(OBJS) -out:$@ $(guilibs) $(LIBS)
    if not exist $(MSTOOLS)\samples\ole\bin mkdir $(MSTOOLS)\samples\ole\bin
    copy $@ $(MSTOOLS)\samples\ole\bin

