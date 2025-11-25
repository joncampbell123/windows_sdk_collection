!include <win32.mak>

proj=mid2strm

all: $(proj).exe

$(proj).obj:  $(proj).c
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).c
	

$(proj).exe: $(proj).obj  $(proj).def
    $(link) $(linkdebug) $(conflags) -out:$(proj).exe      \
           $(proj).obj   \
           $(conlibs) ddraw.lib winmm.lib

# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
