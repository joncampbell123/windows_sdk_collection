# makefile for CVTDOC ISAPI filter
!include <win32.mak>
CC=$(cc) $(cdebug)
CFLAGS=$(cflags)
CVARS=$(cvarsdll) -DWIN32 -DNDEBUG  
LINK=$(link) $(linkdebug)
LINKOPT= $(dlllflags)
LIBS= $(guilibsdll) user32.lib mapi32.lib advapi32.lib
OBJS=cvtdoc.obj
LINKOUT=/OUT:cvtdoc.dll
DEFS=cvtdoc.def

all: cvtdoc.dll

.cpp.obj:
    $(CC) $(CFLAGS) $(CVARS) $*.cpp

cvtdoc.dll: $(OBJS) $(DEFS)
    $(LINK) $(LINKOPT) /DEF:$(DEFS) $(LINKOUT) $(LIBS) $(OBJS)

clean:
    del cvtdoc.dll
    del cvtdoc.obj
    del cvtdoc.lib
    del cvtdoc.exp

