proj = DDESPY
SRCS = DDESPY.C LISTS.C TESTSUBS.C

#set nodebug=1 to omit debug information from build

!include <ntwin32.mak>

all: $(proj).exe

#create resource binaries
$(proj).res: $(proj).rc $(proj).dlg $*.h
    $(rc) $(rcvars) -r -fo $(proj).res $(cvars) $(proj).rc
    cvtres -$(CPU) $(proj).res -o $(proj).rbj

# Update the object file if necessary
.c.obj:
    $(cc) $(cflags)  $(cvars) $(cdebug) $*.c

$(proj).exe: $(SRCS:.C=.OBJ) $(proj).res 
    $(link) $(linkdebug) $(guiflags) $(SRCS:.C=.OBJ)  $(guilibs) $(proj).rbj -out:$(proj).exe

clean:
	del *.obj
	del *.exe
	del *.map
	del *.res
	del *.rbj