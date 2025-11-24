#+---------------------------------------------------------------------------
#
#  Microsoft Windows
#  Copyright (C) Microsoft Corporation, 1994.
#
#  File:        makefile
#
#----------------------------------------------------------------------------

!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

#
#       Makefile for OLE Sample DEFO2V32.DLL
#
#       builds DEFO2V32.DLL: viewer DLL used by the OLE2VIEW tool.
#

OLEFLAGS = -I ..\..\idl -I ..\..\winhlprs
LINK = $(link)
LINKFLAGS = $(linkdebug)
RCFLAGS = -DWIN32

OBJS = iadvsi_a.obj     \
       iadvsink.obj     \
       idatao_a.obj     \
       idataobj.obj     \
       idisp_a.obj      \
       idisp.obj        \
       tofile_a.obj     \
       tofile.obj       \
       util_a.obj       \
       util.obj

LIBS = $(olelibsdll)
cflags = $(cflags) -DUNICODE -D_UNICODE
all: defo2v32.dll

clean:
    -del *.obj
    -del *.exp
    -del *.res
    -del *.dll
    -del *.lib

iadvsi_a.obj: iadvsi_a.cpp      \
              iadvsink.cpp      \
              precompa.h        \
              precomp.h         \
              defo2v.h          \
              resource.h        \
              util.h            \
              idataobj.h        \
              iadvsink.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) iadvsi_a.cpp

iadvsink.obj: iadvsink.cpp      \
              precomp.h         \
              defo2v.h          \
              resource.h        \
              util.h            \
              idataobj.h        \
              iadvsink.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) iadvsink.cpp

idatao_a.obj: idatao_a.cpp      \
              idataobj.cpp      \
              precompa.h        \
              precomp.h         \
              defo2v.h          \
              resource.h        \
              util.h            \
              idataobj.h        \
              iadvsink.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) idatao_a.cpp

idataobj.obj: idataobj.cpp       \
              precomp.h          \
              defo2v.h           \
              resource.h         \
              util.h             \
              idataobj.h         \
              iadvsink.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) idataobj.cpp

idisp_a.obj: idisp_a.cpp        \
             idisp.cpp          \
             precompa.h         \
             precomp.h          \
             defo2v.h           \
             resource.h         \
             util.h             \
             idisp.h            \
             tofile.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) idisp_a.cpp

idisp.obj:   idisp.cpp          \
             precomp.h          \
             defo2v.h           \
             resource.h         \
             util.h             \
             idisp.h            \
             tofile.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) idisp.cpp

tofile_a.obj: tofile_a.cpp      \
              tofile.cpp        \
              precompa.h        \
              precomp.h         \
              defo2v.h          \
              resource.h        \
              util.h            \
              idisp.h           \
              tofile.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) tofile_a.cpp

tofile.obj:   tofile.cpp        \
              precomp.h         \
              defo2v.h          \
              resource.h        \
              util.h            \
              idisp.h           \
              tofile.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) tofile.cpp

util_a.obj: util_a.cpp  \
            util.cpp    \
            precompa.h  \
            precomp.h   \
            defo2v.h    \
            resource.h  \
            util.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) util_a.cpp

util.obj:   util.cpp    \
            precomp.h   \
            defo2v.h    \
            resource.h  \
            util.h
    $(cc) $(cvarsdll) $(cflags) $(cdebug) $(OLEFLAGS) util.cpp

defo2v.res: defo2v.rc defo2v.h
    rc $(RCFLAGS) -r -fo$@ defo2v.rc

defo2v32.dll: $(OBJS) defo2v.res
    $(LINK) @<<
        $(LINKFLAGS)
        -dll
        -out:$@
        -def:defo2v32.def
        -noentry
        -MERGE:.CRT=.data
        -MERGE:_PAGE=PAGE
        -MERGE:_TEXT=.text
        -SECTION:INIT,d
        -OPT:REF
        -INCREMENTAL:NO
        -IGNORE:4037,4065
        -PDB:NONE
        -NODEFAULTLIB
        -subsystem:native
        $(OBJS)
        defo2v.res
        $(LIBS)
<<

