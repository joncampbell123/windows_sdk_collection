!include <win32.mak>

all: fastfile.lib ffcreate.exe

fastfile.obj:  fastfile.c fastfile.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) fastfile.c
	

fastfile.lib: fastfile.obj
    $(implib) fastfile.Obj -name:fastfile.Lib

ffcreate.obj:  ffcreate.c ffent.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) ffcreate.c

ffcreate.exe: ffcreate.obj  ffcreate.def  
    $(link) $(linkdebug) $(conflags) -out:ffcreate.exe      \
           ffcreate.obj \
          $(conlibs)

# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
