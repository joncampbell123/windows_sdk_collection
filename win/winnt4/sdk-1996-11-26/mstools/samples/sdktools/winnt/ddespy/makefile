proj = DDESPY
SRCS = DDESPY.C LISTS.C TESTSUBS.C

#set nodebug=1 to omit debug information from build

!include <win32.mak>

all: $(proj).exe

#create resource binaries
$(proj).res: $(proj).rc $(proj).dlg $*.h
    $(rc) $(rcvars) -r -fo $(proj).res $(proj).rc

# Update the object file if necessary
.c.obj:
    $(cc) $(cflags)  $(cvars) $(cdebug) $*.c

$(proj).exe: $(SRCS:.C=.OBJ) $(proj).res
    $(link) $(linkdebug) $(guiflags) $(SRCS:.C=.OBJ)  $(guilibs) $(proj).res -out:$(proj).exe
!IF ("$(TARGETLANG)" == "LANG_JAPANESE") && ("$(OS)" == "Windows_NT")
    rlman -p 932 -n 17 1 -a $*.exe $*.tok $*.exe
!ENDIF

clean:
    del *.obj
    del *.exe
    del *.map
    del *.res
    del *.res
