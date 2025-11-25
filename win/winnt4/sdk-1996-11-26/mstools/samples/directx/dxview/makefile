!include <win32.mak>

proj=dxview

all: $(proj).exe

$(proj).obj:  $(proj).c $(proj).h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).c
	
$(proj).res: $(proj).rc 
   rc -r -fo $(proj).res $(proj).rc



$(proj).exe: $(proj).obj    \
           $(proj).res
    $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           $(proj).obj   \
           $(proj).res   \
           $(guilibs) ddraw.lib comctl32.lib dplay.lib dsound.lib

# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
