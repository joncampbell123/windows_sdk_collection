!include <win32.mak>

proj=ddex1

all: $(proj).exe

$(proj).obj:  $(proj).cpp resource.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).cpp
	
$(proj).res: $(proj).rc resource.h
   rc -r -fo $(proj).res $(proj).rc


$(proj).exe: $(proj).obj  $(proj).def  \
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