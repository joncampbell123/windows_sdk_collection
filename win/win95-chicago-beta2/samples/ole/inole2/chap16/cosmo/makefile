#
# MAKEFILE
# Cosmo Chapter 16
#
# Copyright (c)1992-1994 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(SDKROOT)\samples\ole\inole2\inc

!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

DEFFILE = cosmo.def

TARGET  = cosmo
OUTFILE = cosmo16.exe

all:$(OUTFILE)

INCLS    = $(TARGET).h resource.h cosmole.h

OBJS1    = $(TARGET).obj client.obj precomp.obj
OBJS2    = document.obj polyline.obj polywin.obj
OBJS3    = idropsrc.obj idroptgt.obj iclassf.obj
OBJS4    = figure.obj iperstor.obj idataobj.obj ioleobj.obj
OBJS5    = iperfile.obj
OBJS6    = iipobj.obj iipaobj.obj $(TARGET).res
OBJS     = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6)

RCFILES1 = $(TARGET).ico document.ico about.dlg
RCFILES2 = gizmo72.bmp gizmo96.bmp gizmo120.bmp
RCFILES  = $(RCFILES1) $(RCFILES2)

LIBS    = bttncur.lib gizmobar.lib ..\..\lib\stastrip.lib ..\..\lib\classmdi.lib ..\..\lib\bookui32.lib

OLE_FLAGS = -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####
$(TARGET).res : $(TARGET).rc $(INCLS) $(RCFILES)

#Application level things
$(TARGET).obj : $(TARGET).cpp $(INCLS)
client.obj    : client.cpp    $(INCLS)

#Document level things
document.obj  : document.cpp $(INCLS) polyline.h

idropsrc.obj  : idropsrc.cpp $(INCLS)
idroptgt.obj  : idroptgt.cpp $(INCLS)


#Editor level things
polyline.obj  : polyline.cpp $(INCLS) polyline.h
polywin.obj   : polywin.cpp  $(INCLS) polyline.h


#Compound Document Things
iclassf.obj   : iclassf.cpp  $(INCLS)

figure.obj    : figure.cpp   $(INCLS)
iperstor.obj  : iperstor.cpp $(INCLS)
idataobj.obj  : idataobj.cpp $(INCLS)
ioleobj.obj   : ioleobj.cpp  $(INCLS)
iperfile.obj  : iperfile.cpp $(INCLS)

iipobj.obj   : iipobj.cpp  $(INCLS)
iipaobj.obj  : iipaobj.cpp $(INCLS)
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

$(OUTFILE): $(OBJS)
    $(link) $(linkdebug) $(guilflags) $(OBJS) -out:$@ $(olelibs) $(LIBS)
