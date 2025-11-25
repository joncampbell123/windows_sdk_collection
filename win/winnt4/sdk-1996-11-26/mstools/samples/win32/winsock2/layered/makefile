!include <win32.mak>

!IFNDEF NODEBUG
.cpp.obj:
	$(cc) $(cdebug) $(cflags) $(cvarsdll) -DSTRICT -YX\
        /D "DEBUG_TRACING"\
        /D "TRACING" /Od /D "_DEBUG" $<
!else

.cpp.obj:
	$(cc) $(cdebug) $(cflags) $(cvarsdll) -DSTRICT -YX -DNODEBUG $<
!endif
CPP_FILES= dbuffmgr.cpp\
           doverlap.cpp\
           dworker.cpp\
           dcatalog.cpp\
           dcatitem.cpp\
           dllmain.cpp\
           dprovide.cpp\
           dsocket.cpp\
           spi.cpp\
           trace.cpp\
           dthook.cpp


OBJS= $(CPP_FILES:.cpp=.obj)

all: lsp.dll inst_lsp.exe

lsp.dll: $(OBJS)
	$(link) $(linkdebug) $(dlllflags) \
	-export:WSPStartup \
	-out:$*.dll $(OBJS) $(guilibsdll) ws2_32.lib

inst_lsp.obj: inst_lsp.cpp
	$(cc) $(cdebug) $(cflags) $(cvars) inst_lsp.cpp

inst_lsp.exe: inst_lsp.obj
	$(link) $(linkdebug) $(conlflags) -out:$*.exe inst_lsp.obj\
        rpcrt4.lib $(conlibs) ws2_32.lib

clean:
        -del *.obj
	-del *.dll
	-del *.pch
	-del *.exp
	-del lsp.lib
