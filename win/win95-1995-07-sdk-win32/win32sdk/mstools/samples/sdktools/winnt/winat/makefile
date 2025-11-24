ENV=WIN32

# rem this line to build a debug version
NODEBUG = 1

TARGETOS=WINNT

!include <$(ENV).MAK>

proj  = WINAT
proj2 = listhscr

mydefines = $(scall)
mylibs    = ADVAPI32.LIB NETAPI32.LIB SHELL32.LIB

all: $(proj2).obj $(proj).exe $(proj).hlp

# Update the help file if necessary
$(proj).hlp : $(proj).rtf $(proj).hpj
    hc -n $(proj).hpj

# Update the resource if necessary
$(proj).res: $(proj).rc $(proj).rcv $(proj).dlg $(proj).h
    $(rc) $(rcflags) -fo $(proj).res $(rcvars) $(proj).rc

# Update the object file if necessary
$(proj2).obj: $(proj2).c $(proj2).h
    $(cc) $(cflags) $(cvars) $(cdebug) $(mydefines) $(proj2).c

# Update the object file if necessary
$(proj).obj: $(proj).c $(proj).h
    $(cc) $(cflags) $(cvars) $(cdebug) $(mydefines) $(proj).c

# Update the executable file
$(proj).exe: $(proj).obj $(proj).res $(proj).hlp $(proj2).obj
    $(link) -MAP:$(proj).map $(linkdebug) $(guiflags) $(proj).obj $(proj2).obj $(guilibs) $(proj).res $(mylibs) -out:$(proj).exe

##### Clean Directory #####
clean:
    -del $(proj).exe
    -del $(proj).obj
    -del $(proj).res
    -del $(proj).res
    -del $(proj).map
#    -del $(proj).sym

    -del $(proj).hlp
    -del $(proj).err

    -del $(proj2).obj

