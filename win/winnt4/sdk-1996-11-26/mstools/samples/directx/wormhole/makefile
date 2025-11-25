!include <win32.mak>

proj=wormhole

all: $(proj).exe

$(proj).obj:  $(proj).cpp 
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).cpp
	
$(proj).res: $(proj).rc 
   rc -r -fo $(proj).res $(proj).rc


$(proj).exe: $(proj).obj    \
           $(proj).res
    $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           $(proj).obj   \
           $(proj).res   \
           $(guilibs) ddraw.lib

# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
