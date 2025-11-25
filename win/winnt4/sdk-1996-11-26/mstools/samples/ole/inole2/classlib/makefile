#
# Makefile
# Sample Code Class Libraries
#
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

!ifdef SDI
TARGET  = classSDI
OUTFILE = classSDI.lib
!else
TARGET  = classMDI
OUTFILE = classMDI.lib
!endif

all: $(OUTFILE)

OLE_FLAGS = -I..\inc -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif

.SUFFIXES: .h .c .obj .exe .res .rc .cpp

#
#


INCLS   = classlib.h classres.h

OBJS1   = cstrtabl.obj chourgls.obj
OBJS2   = cwindow.obj cframe.obj framewin.obj
OBJS3   = cclient.obj cdocumnt.obj docwin.obj
OBJS4   = chatch.obj cgizbar.obj cstastrp.obj
OBJS5   =
OBJS6   =
OBJS    = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4)

RCFILES1 = app.ico document.ico about.dlg
RCFILES2 = stdgz72.bmp stdgz96.bmp stdgz120.bmp
RCFILES  = $(RCFILES1) $(RCFILES2)

#
# Tool Directives
#
.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $*.c

.cpp.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $*.cpp

.rc.res:
    rc -r -DWIN32 -DDEBUG -I..\..\inc $*.rc

clean:
    -del *.obj
    -del *.lib
    -del *.map
    -del *.res

$(TARGET).lib: $(OBJS) classlib.res
    $(implib) $(OBJS) -out:$*.lib -machine:$(CPU) classlib.res
    if not exist ..\lib mkdir ..\lib
    copy $(TARGET).lib ..\lib

##### Dependencies #####
cstrtabl.obj  : cstrtabl.cpp $(INCLS)
chourgls.obj  : chourgls.cpp $(INCLS)

cwindow.obj   : cwindow.cpp  $(INCLS)

cframe.obj    : cframe.cpp   $(INCLS)
framewin.obj  : framewin.cpp $(INCLS)

cclient.obj   : cclient.cpp  $(INCLS)

cdocumnt.obj  : cdocumnt.cpp $(INCLS)
docwin.obj    : docwin.cpp   $(INCLS)

chatch.obj    : chatch.cpp   $(INCLS)

cgizbar.obj   : cgizbar.cpp  $(INCLS)
cstastrp.obj  : cstastrp.cpp $(INCLS)

classlib.res : classlib.rc $(INCLS) $(RCFILES)

