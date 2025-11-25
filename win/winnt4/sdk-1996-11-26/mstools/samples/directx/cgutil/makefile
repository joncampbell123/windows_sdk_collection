!include <win32.mak>

mycflag=/I "..\include"

all: cgutil.lib linklist.lib

cgexcpt.obj:  cgexcpt.cpp ..\include\cgexcpt.h  ..\include\cgres.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) cgexcpt.cpp
	
cgres.obj:  cgres.cpp  ..\include\cgexcpt.h  ..\include\cgres.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) cgres.cpp

strrec.obj:  strrec.cpp  ..\include\strrec.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) strrec.cpp


# Update the import library

cgutil.lib: cgexcpt.obj cgres.obj strrec.obj
    $(implib) -machine:$(CPU)     \
    cgexcpt.obj cgres.obj strrec.obj \
    -out:cgutil.lib

#Also make linklist
linklist.obj:  linklist.cpp
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) linklist.cpp


# Update the import library

linklist.lib: linklist.obj
    $(implib) -machine:$(CPU)     \
    linklist.obj \
    -out:linklist.lib


# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
